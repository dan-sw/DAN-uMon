/* elf.c:
 * Perform some dumps/displays/conversions on the elf file format.
 *
 * General notice:
 * This code is part of a boot-monitor package developed as a generic base
 * platform for embedded system designs.  As such, it is likely to be
 * distributed to various projects beyond the control of the original
 * author.  Please notify the author of any enhancements made or bugs found
 * so that all may benefit from the changes.  In addition, notification back
 * to the author will allow the new user to pick up changes that may have
 * been made by other users after this version of the code was distributed.
 *
 * Author:	Ed Sutter
 * email:	esutter@lucent.com
 * phone:	908-582-2351
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "elf.h"
#include "version.h"
#include "utils.h"
#include "../zlib/zlib.h"
#ifdef BUILD_WITH_VCC
#else
#include <unistd.h>
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

#define USE_OFFSET_DELTA	0

void ZipElfSections(int), ElfToBinary(char *);
void StripElfFile(char *,char *), ShowElfMap(long);
void ShowAppend(void), ShowElfHdr(void), ShowSections(int);
Elf32_Half GetElfSectionHdrs(void);
struct elf_fhdr *GetElfFileHdr(struct elf_fhdr *);
char *VerboseShtype(unsigned long); 
char *VerboseShflags(unsigned long), *GetElfSectionName(unsigned long);

struct	elf_fhdr Ehdr;
struct	elf_shdr *ScnTbl;
struct	elf_phdr *PhdrTbl;
char	*elfFname;
unsigned char PadByte;
int 	elfFD, debug, xdebug, Ecvt, verbose, insertPAD;
int		remove_zouts = 1;

/* open verbosity: */
#define SHOWELFHDR		0x0001
#define SHOWSECTIONS	0x0002
#define SHOWMAP			0x0004
#define SHOWMAPOFFSET	0x0008
#define SHOWAPPEND		0x0010
#define SHOWPRGMHDR		0x0020
#define SHOWMAPNONZERO	0x0040

/* The Linux box that I tried to build this on did not have tell(),
 * so this is a very inefficient equivalent...
 * Originally, I thought lseek(fd,0,SEEK_CUR) would work, but
 * that generates an error on linux due to a bad argument; apparently
 * it doesn't like the value of 0 for the offset when 'whence' is set to
 * SEEK_CUR.
 */
#ifndef BUILD_WITH_VCC
long
tell(int fd)
{
	Lseek(fd,1,SEEK_CUR);
	return(Lseek(fd,-1,SEEK_CUR));
}
#endif

void
ShowAppend(void)
{
	int	i;
	char	c;
	Elf32_Word	maxsize;
	Elf32_Off	maxoff;
	struct	elf_shdr	*eshdr;

	/* Find the highest offset then seek to the end of that section. This */
	/* should be the end of the REAL data in the elf file.  Any remaining */
	/* stuff has been appended (hopefully!). */
	maxoff = 0;
	for(i=0;i<Ehdr.e_shnum;i++) {
		eshdr = &ScnTbl[i];
		if (eshdr->sh_offset > maxoff) {
			maxoff = eshdr->sh_offset;
			maxsize = eshdr->sh_size;
		}
	}
	Lseek(elfFD,maxoff+maxsize,SEEK_SET);
	while(1) {
		if (read(elfFD,&c,1) != 1)
			break;
		putchar(c);
	}
}

void
ShowElfHdr(void)
{
	int	i;

	printf("\t\tELF FILE HEADER\n");
	printf("E_IDENT (hex):		");
	for(i=0;i<EI_NIDENT;i++)
		printf("%02x ",Ehdr.e_ident[i]);
	printf("\n");
	printf("File Type:			0x%x\n",Ehdr.e_type);
	printf("Machine:			0x%x\n",Ehdr.e_machine);
	printf("Version:			0x%x\n",Ehdr.e_version);
	printf("Entrypoint:			0x%08x\n",Ehdr.e_entry);
	printf("Program Hdr Tbl Offset:		0x%x\n",Ehdr.e_phoff);
	printf("Section Hdr Tbl Offset:		0x%x\n",Ehdr.e_shoff);
	printf("Processor-specific flags:	0x%08x\n",Ehdr.e_flags);
	printf("ELF Header size:		%u\n",Ehdr.e_ehsize);
	printf("Size of 1 Pgm Hdr Entry:	%u\n",Ehdr.e_phentsize);
	printf("# of entries in Pgm Hdr Tbl:	%u\n",Ehdr.e_phnum);
	printf("Size of 1 Scn Hdr Entry:	%u\n",Ehdr.e_shentsize);
	printf("# of entries in Scn Hdr Tbl:	%u\n",Ehdr.e_shnum);
	printf("ScnHdr string Tbl Index:	0x%x\n",Ehdr.e_shstrndx);
}

void
ShowProgramHdrTbl(int verbose)
{
	int	i;

	printf("\t\tProgram Header Table:\n");
	for(i=0;i<Ehdr.e_phnum;i++) {
		printf("PHDR[%d]:\n",i);
		printf("p_type:      0x%lx\n",PhdrTbl[i].p_type);
		printf("p_offset:    0x%lx\n",PhdrTbl[i].p_offset);
		printf("p_vaddr:     0x%lx\n",PhdrTbl[i].p_vaddr);
		printf("p_paddr:     0x%lx\n",PhdrTbl[i].p_paddr);
		printf("p_filesz:    0x%lx\n",PhdrTbl[i].p_filesz);
		printf("p_memsz:     0x%lx\n",PhdrTbl[i].p_memsz);
		printf("p_flags:     0x%lx\n",PhdrTbl[i].p_flags);
		printf("p_align:     0x%lx\n",PhdrTbl[i].p_align);
	}
}

void
ShowSections(int verbose)
{
	int	i;
	unsigned long	size;
	struct	elf_shdr	*eshdr;

	if (debug >= 2)
		fprintf(stderr,"ShowSections()\n");

	for(i=0;i<Ehdr.e_shnum;i++) {
		eshdr = &ScnTbl[i];
		size = eshdr->sh_size;
		if (size)
			size--;
		printf("%10s: 0x%08x..0x%08x (%ld bytes) %s %s\n",
			GetElfSectionName(eshdr->sh_name),
			eshdr->sh_addr,eshdr->sh_addr+size,eshdr->sh_size,
			VerboseShflags(eshdr->sh_flags),
			VerboseShtype(eshdr->sh_type));
		if (verbose) {
			printf("         offset=   0x%08x, link=   0x%08x, info=0x%08x\n",
				eshdr->sh_offset, eshdr->sh_link, eshdr->sh_info);
			printf("         addralign=0x%08x, entsize=0x%08x\n\n",
				eshdr->sh_addralign, eshdr->sh_entsize);
		}
	}
}

/* FindLowestSectionInFile():
 * The section table (ScnTbl[] array) contains a list of all sections in
 * the ELF file.  I had thought that this table listed the sections
 * in the same order as they exist in the file (meaning that the section
 * pointed to by ScnTbl[1] was before the section pointed to by ScnTbl[2]).
 * Turns out that this is not necessarily the case, so this function simply
 * returns the offset to the lowest section in the file.
 */
long
FindLowestSectionInFile(void)
{
	int	i;
	Elf32_Off	lowest_offset;

	lowest_offset = 0xffffffff;
	for(i=1;i<Ehdr.e_shnum;i++) {
		if ((ScnTbl[i].sh_size) &&
			(ScnTbl[i].sh_offset < lowest_offset))
			lowest_offset = ScnTbl[i].sh_offset;
	}
	if (debug >= 1)
		fprintf(stderr,"lowest section in file = 0x%lx\n",lowest_offset);
	return(lowest_offset);
}

/* ZipElfSections():
 * First pass...
 *   Create one file of compressed data for each section to be compressed.
 * Second pass...
 *   Replace each original section with the data from the compressed file
 *   and adjust header information appropriately.
 */
void
ZipElfSections(int zlevel)
{
	uchar	*cp;
	FILE	*zout;
	unsigned long	phoffz, shoffz, offset_delta;
	int		zipfd, sidx, reduction;
	char	zoutfile[128], zipto[128], mode[16], *proghdr;
	struct	elf_shdr *sptr, *scntbl_z, *sptr_z;
	struct stat zstat;
	Elf32_Off	lowest_section_offset, offset1, offset2;

	sprintf(mode,"wb%d ",zlevel);
	sprintf(zipto,"%s.ezip",elfFname);
	fprintf(stderr,"Compressing %s into %s \n",elfFname,zipto);

	/* Make a copy of the section table... */
	scntbl_z = (struct elf_shdr *)Malloc(Ehdr.e_shnum * Ehdr.e_shentsize);
	memcpy(scntbl_z,ScnTbl,Ehdr.e_shnum * Ehdr.e_shentsize);

	reduction = 0;

	/* For each section, if it is of the appropriate type, compress it to
	 * a file named "_section_name_%d.gz".
	 */
	for(sptr=ScnTbl,sidx=0;sidx<Ehdr.e_shnum;sidx++,sptr++) {
		char	*name;
			
		name = GetElfSectionName(ScnTbl[sidx].sh_name);

		if ((sptr->sh_flags & SHF_ALLOC) &&
			(sptr->sh_type != SHT_NOBITS) && (sptr->sh_size)) {

			sprintf(zoutfile,"_%s_%d.gz",name,sidx);
			unlink(zoutfile);

			cp = (uchar *)Malloc(sptr->sh_size);
			Lseek(elfFD,sptr->sh_offset,SEEK_SET);
			read(elfFD,cp,sptr->sh_size);

	    	zout = gzopen(zoutfile, mode);
		    if (zout == NULL) {
		        fprintf(stderr, "can't gzopen %s\n", zoutfile);
		        exit(1);
		    }
	        if (gzwrite(zout, cp, (unsigned)sptr->sh_size) != sptr->sh_size) {
		        fprintf(stderr, "gzwrite failed\n");
				exit(1);
			}
		    if (gzclose(zout) != Z_OK) {
		        fprintf(stderr, "gzclose failed\n");
				exit(1);
			}
	
			free(cp);
			
			stat(zoutfile,&zstat);
			printf("Sec %-8s compressed from %8d to %8d bytes.\n",
				name,sptr->sh_size,zstat.st_size);
			reduction += (sptr->sh_size - zstat.st_size);
		}
		free(name);
	}
	printf("Total reduction: %d bytes\n",reduction);

	/* Create the destination file... */
	unlink(zipto);
	if ((zipfd = open(zipto,O_WRONLY|O_BINARY|O_CREAT,0777))==-1)
	{
		perror(zipto);
		exit(1);
	}

	/* Do a blind copy of all data up to the start of the first
	 * section.  Note that it is not required that the sections be
	 * listed in ScnTbl[] in the same order as they are arranged within
	 * the file, so we must determine which section in the table is first
	 * in the file.  Use the lowest_section_offset variable as a pointer
	 * to the first location in the new file into which the compressed
	 * sections can be placed.
	 *
	 * Fix...
	 * Turns out that the "blind copy" approach wastes memory in certain
	 * cases... For some reason some elf files have an empty 64K
	 * block of space between the end of the program header table and the
	 * first section, so this meant that a 64k block of space was wasted.
	 * 
	 * In the first fix for the above problem, the lowest_section_offset 
	 * pointed to the address just after the end of the file header (below
	 * the wasted 64k block).  This was incorrect because it chopped off
	 * the program header; hence, now the lowest_sectino_offset points to
	 * the address just after the program header...
	 *
	 * Yet another change to this...
	 * Turns out that we can't assume that the program header will be at
	 * the beginning of the file.  I've had cases where it is at the end
	 * (after the sections, next to the section header table).
	 * So... we look at both potential offsets (offset1 & offset2).
	 * If it turns out that the lowest section is below the program
	 * header in the elf file, then the program header is at the end;
	 * hence, it must be copied to the new .ezip file after the sections
	 * have been copied.
	 */
	offset1 = FindLowestSectionInFile();	/* orignal technique */
	offset2 = Ehdr.e_phoff+(Ehdr.e_phentsize*Ehdr.e_phnum);

	if (verbose) {
		fprintf(stderr,"offset1: 0x%lx\n",offset1);
		fprintf(stderr,"offset2: 0x%lx\n",offset2);
	}

	if (offset1 < offset2) {
		if (verbose)
			fprintf(stderr,"Copying first %ld bytes\n",offset1);
		lowest_section_offset = offset1;

		/* If we're here, then the program header must be AFTER the
		 * sections area in the elf file, so we extract the program
		 * header now so that we can re-insert it after the sections
		 * have been copied...
		 */
		Lseek(elfFD,Ehdr.e_phoff,SEEK_SET);
		proghdr = Malloc(Ehdr.e_phentsize*Ehdr.e_phnum);
		Read(elfFD,proghdr,Ehdr.e_phentsize*Ehdr.e_phnum);
	}
	else {
		if (verbose)
			fprintf(stderr,"Copying ELF file & program hdr\n");
		lowest_section_offset = offset2;
		proghdr = 0;
	}

	Lseek(elfFD,0,SEEK_SET);
	cp = (uchar *)Malloc(lowest_section_offset);
	Read(elfFD,(char *)cp,lowest_section_offset);
	Write(zipfd,(char *)cp,lowest_section_offset);
	free(cp);

	offset_delta = 0;

	/* For each section, copy from original file or from zipped file to */
	/* the new file... */
	for(sptr=&ScnTbl[1],sidx=1;sidx<Ehdr.e_shnum;sidx++,sptr++) {
		char	*name;
			
		sptr_z = scntbl_z+sidx;

		name = GetElfSectionName(ScnTbl[sidx].sh_name);
		if ((sptr->sh_flags & SHF_ALLOC) &&
			(sptr->sh_type != SHT_NOBITS) && (sptr->sh_size)) {
			int secfd;

			sprintf(zoutfile,"_%s_%d.gz",name,sidx);
		
			stat(zoutfile,&zstat);
			if (verbose)
				fprintf(stderr,"Replacing sec %s with %s (%d bytes)\n",
					name,zoutfile,zstat.st_size);
			sptr_z->sh_size = zstat.st_size;
#if USE_OFFSET_DELTA
			sptr_z->sh_offset -= offset_delta;
#else
			sptr_z->sh_offset = tell(zipfd);
#endif
			if(debug) {
				fprintf(stderr,"offset_delta += %d (%d-%d)\n",
				(sptr->sh_size - sptr_z->sh_size),sptr->sh_size,
				sptr_z->sh_size);
			}
			offset_delta += (sptr->sh_size - sptr_z->sh_size);
			if ((secfd = open(zoutfile,O_RDONLY|O_BINARY)) == -1) {
				fprintf(stderr,"Couldn't open %s\n",zoutfile);
				exit(1);
			}
			cp = (uchar *)Malloc(zstat.st_size);
			Read(secfd,(char *)cp,zstat.st_size);
			Write(zipfd,(char *)cp,zstat.st_size);
			free((char *)cp);
			close(secfd);
			if (remove_zouts)
				unlink(zoutfile);
		}
		else {
			if ((sptr->sh_size) && (sptr->sh_type != SHT_NOBITS)) {
				if (debug)
					fprintf(stderr,"Read %s offset=%d size=%d\n",
						name,sptr->sh_offset,sptr->sh_size);
				Lseek(elfFD,sptr->sh_offset,SEEK_SET);
				cp = (uchar *)Malloc(sptr->sh_size);
				Read(elfFD,(char *)cp,sptr->sh_size);
#if USE_OFFSET_DELTA
				sptr_z->sh_offset -= offset_delta;
#else
				sptr_z->sh_offset = tell(zipfd);
#endif
				Write(zipfd,(char *)cp,sptr->sh_size);
				free(cp);
			}
		}

		if ((sptr->sh_type != SHT_NOBITS) && (sptr->sh_flags & SHF_ALLOC)) {
			/* I found that the size of the section does not necessarily */
			/* correspond to the point at which the next section starts in */
			/* the file, so this makes sure that offset_delta is properly */
			/* adjusted if that correspondance does not exist... */
			if ((sptr->sh_size + sptr->sh_offset) != (sptr+1)->sh_offset) {
				int delta;
				delta=((sptr+1)->sh_offset-(sptr->sh_size+sptr->sh_offset));
				offset_delta += 
					((sptr+1)->sh_offset - (sptr->sh_size + sptr->sh_offset));
				if (debug)
					fprintf(stderr,"offset_delta compensation = %d\n",delta); 
			}
		}
	}

	/* Now that all sections have been written to the file, insert the
	 * section header table at the end. Note that the header may be
	 * endian-converted,  so compensate for this prior to writing to file...
	 */
	if (1) {
		int	i;
		shoffz = tell(zipfd);
		/* The section header table base must be 4-byte aligned.. */
		if (shoffz & 3) {
			if (debug)
				fprintf(stderr,"Shifting scnhdr %d bytes up\n",
					4-(shoffz&3));
			shoffz = Lseek(zipfd,4-(shoffz&3),SEEK_CUR);
		}
		if(debug)
			fprintf(stderr,"Inserting scnhdr at 0x%x\n",shoffz);
		for(i=0;i<Ehdr.e_shnum;i++) {
			scntbl_z[i].sh_name = otherEnd32(Ecvt,scntbl_z[i].sh_name);
			scntbl_z[i].sh_type = otherEnd32(Ecvt,scntbl_z[i].sh_type);
			scntbl_z[i].sh_flags = otherEnd32(Ecvt,scntbl_z[i].sh_flags);
			scntbl_z[i].sh_addr = otherEnd32(Ecvt,scntbl_z[i].sh_addr);
			scntbl_z[i].sh_offset = otherEnd32(Ecvt,scntbl_z[i].sh_offset);
			scntbl_z[i].sh_size = otherEnd32(Ecvt,scntbl_z[i].sh_size);
			scntbl_z[i].sh_link = otherEnd32(Ecvt,scntbl_z[i].sh_link);
			scntbl_z[i].sh_info = otherEnd32(Ecvt,scntbl_z[i].sh_info);
			scntbl_z[i].sh_addralign =
				otherEnd32(Ecvt,scntbl_z[i].sh_addralign);
			scntbl_z[i].sh_entsize = otherEnd32(Ecvt,scntbl_z[i].sh_entsize);
		}
		Write(zipfd,(char *)scntbl_z,(int)(Ehdr.e_shnum*Ehdr.e_shentsize));
		for(i=0;i<Ehdr.e_shnum;i++) {
			scntbl_z[i].sh_name = otherEnd32(Ecvt,scntbl_z[i].sh_name);
			scntbl_z[i].sh_type = otherEnd32(Ecvt,scntbl_z[i].sh_type);
			scntbl_z[i].sh_flags = otherEnd32(Ecvt,scntbl_z[i].sh_flags);
			scntbl_z[i].sh_addr = otherEnd32(Ecvt,scntbl_z[i].sh_addr);
			scntbl_z[i].sh_offset = otherEnd32(Ecvt,scntbl_z[i].sh_offset);
			scntbl_z[i].sh_size = otherEnd32(Ecvt,scntbl_z[i].sh_size);
			scntbl_z[i].sh_link = otherEnd32(Ecvt,scntbl_z[i].sh_link);
			scntbl_z[i].sh_info = otherEnd32(Ecvt,scntbl_z[i].sh_info);
			scntbl_z[i].sh_addralign =
				otherEnd32(Ecvt,scntbl_z[i].sh_addralign);
			scntbl_z[i].sh_entsize = otherEnd32(Ecvt,scntbl_z[i].sh_entsize);
		}
	}

	/* If the program header was buffered earlier,then we need to insert
	 * it now...
	 */
	if (proghdr) {
		phoffz = tell(zipfd);
		Write(zipfd,proghdr,(Ehdr.e_phentsize*Ehdr.e_phnum));
		Lseek(zipfd,0x1c,SEEK_SET);
		phoffz = otherEnd32(Ecvt,phoffz);
		Write(zipfd,(char *)&phoffz,4);
	}

	if (debug)
		fprintf(stderr,"offset_delta = %d\n",offset_delta);

	/* Adjust the file header to point to the new section header table.
	 */
	Lseek(zipfd,0x20,SEEK_SET);
	shoffz = otherEnd32(Ecvt,shoffz);
	Write(zipfd,(char *)&shoffz,4);


	close(zipfd);
}

void
ElfToBinary(char *binto)
{
	uchar	*cp;
	int	fd, sidx, firstone;
	struct	elf_shdr	*sptr;

	unlink(binto);
	if ((fd = open(binto,O_RDWR|O_BINARY|O_CREAT,0777))==-1)
	{
		perror(binto);
		exit(1);
	}
	fprintf(stderr,"Converting %s into %s\n",
		elfFname,binto);
	firstone = 1;
	for(sptr=ScnTbl,sidx=0;sidx<Ehdr.e_shnum;sidx++,sptr++) {
		char	*name, padbyte;
		int		padtot;
		unsigned long	nextpaddr;
		
		name = GetElfSectionName(ScnTbl[sidx].sh_name);

		if ((sptr->sh_flags & SHF_ALLOC) &&
		    (sptr->sh_type != SHT_NOBITS) &&
		    (sptr->sh_size)) {
			if (!firstone) {	/* Pad if necessary... */
				if (nextpaddr != sptr->sh_addr) {
					if (sptr->sh_addr < nextpaddr) {
						fprintf(stderr,"Unexpected section address\n");
						exit(1);
					}
					padtot = sptr->sh_addr-nextpaddr;
					printf("Pad 0x%x bytes\n",padtot);
					padbyte = PadByte;
					while(padtot) {
						Write(fd,&padbyte,1);
						padtot--;
					}
				}
			}
			printf("Sec %-8s at 0x%-8x 0x%-6x bytes\n",
				name,sptr->sh_addr,sptr->sh_size);
			cp = (uchar *)Malloc(sptr->sh_size);
			Lseek(elfFD,sptr->sh_offset,SEEK_SET);
			read(elfFD,(char *)cp,sptr->sh_size);
			Write(fd,(char *)cp,sptr->sh_size);
			free(cp);
			nextpaddr = sptr->sh_addr + sptr->sh_size;
			firstone = 0;
		}
		free(name);
	}
	/* If non-zero, then insert the specified number of bytes just prior */
	/* to the end of the binary file.  Starting at a point 4-bytes befor */
	/* the end.  This is needed because of a problem with GNU linker that */
	/* does not allow a 4-byte section  to exist at 0xfffffffc (branch */
	/* instruction location for PPC403GCX). */
	if (insertPAD) {
		int i;
		char branch[4];

		Lseek(fd,-4,SEEK_CUR);
		Read(fd,branch,4);
		Lseek(fd,-4,SEEK_CUR);
		for(i=0;i<insertPAD;i++)
			Write(fd,(char *)&PadByte,1);
		Write(fd,branch,4);
	}
	close(fd);
}

/* StripElfFile():
	This is a bit tricky for elf.  We can only strip out certain
	sections, and the sections are not necessarily at the end of 
	the file; hence, chunks of the file are removed and the file
	offsets in the section header table must be adjusted.
*/
void
StripElfFile(char *stripto,char *append)
{
	uchar	*cp;
	unsigned long	shoffs, offset_delta;
	int		stripfd, sidx, newsectot;
	struct	elf_shdr *sptr, *scntbl_s, *sptr_s;

	fprintf(stderr,"Stripping %s into %s \n",elfFname,stripto);
fprintf(stderr,"Not working yet\n");
return;

	/* Allocate space for new section header table that may be smaller */
	/* than the current table... */
	scntbl_s = (struct elf_shdr *)Malloc(Ehdr.e_shnum * Ehdr.e_shentsize);

	/* Create the destination file... */
	unlink(stripto);
	if ((stripfd = open(stripto,O_WRONLY|O_BINARY|O_CREAT,0777))==-1) {
		perror(stripto);
		exit(1);
	}

	/* Up to the first section, do a blind copy from original */
	/* file to new compressed file... */
	if (debug)
		fprintf(stderr,"Copying first %ld bytes\n",ScnTbl[1].sh_offset);
	Lseek(elfFD,0,SEEK_SET);
	cp = (uchar *)Malloc(ScnTbl[1].sh_offset);
	Read(elfFD,(char *)cp,ScnTbl[1].sh_offset);
	Write(stripfd,(char *)cp,ScnTbl[1].sh_offset);
	free(cp);

	scntbl_s[0] = ScnTbl[0];
	newsectot = 0;
	offset_delta = 0;
	sptr_s = scntbl_s+1;

	/* For each section, either copy it or remove it... */
	for(sptr=&ScnTbl[1],sidx=1;sidx<Ehdr.e_shnum;sidx++,sptr++) {
		char	*name;
			
		name = GetElfSectionName(ScnTbl[sidx].sh_name);

		if (!strcmp(name,".comment")) {
			if (verbose)
				fprintf(stderr,"Removing %s (%d bytes)\n",name,sptr->sh_size);
			offset_delta += sptr->sh_size;
		}
		else {

			*sptr_s++ = *sptr;
			newsectot++;
			if ((sptr->sh_size) && (sptr->sh_type != SHT_NOBITS)) {
				if (verbose)
					fprintf(stderr,"Copying %s (%d bytes)\n",
						name,sptr->sh_size);
				Lseek(elfFD,sptr->sh_offset,SEEK_SET);
				cp = (uchar *)Malloc(sptr->sh_size);
				Read(elfFD,(char *)cp,sptr->sh_size);
				Write(stripfd,(char *)cp,sptr->sh_size);
				sptr_s->sh_offset -= offset_delta;
				free(cp);
			}
		}
		/* Assuming the section header table comes just after the section */
		/* header string table section, insert the modified table... */
		/* Note that the header may be endian-converted,  so compensate for */
		/* this prior to writing to file... */
		if (!strcmp(name,".shstrtab")) {
			int	i;

			sidx++;
			sptr++;
			sptr_s++;

fprintf(stderr,"section headers at section # %d\n",sidx);
			/* Adjust the size of the section header table: */
			scntbl_s[sidx].sh_size = newsectot * Ehdr.e_shentsize;

			/* Store this current file location: */
			shoffs = tell(stripfd);

			if(debug)
				fprintf(stderr,"Inserting scnhdr at 0x%x\n",shoffs);
			for(i=0;i<newsectot;i++) {
				scntbl_s[i].sh_name = otherEnd32(Ecvt,scntbl_s[i].sh_name);
				scntbl_s[i].sh_type = otherEnd32(Ecvt,scntbl_s[i].sh_type);
				scntbl_s[i].sh_flags = otherEnd32(Ecvt,scntbl_s[i].sh_flags);
				scntbl_s[i].sh_addr = otherEnd32(Ecvt,scntbl_s[i].sh_addr);
				scntbl_s[i].sh_offset = otherEnd32(Ecvt,scntbl_s[i].sh_offset);
				scntbl_s[i].sh_size = otherEnd32(Ecvt,scntbl_s[i].sh_size);
				scntbl_s[i].sh_link = otherEnd32(Ecvt,scntbl_s[i].sh_link);
				scntbl_s[i].sh_info = otherEnd32(Ecvt,scntbl_s[i].sh_info);
				scntbl_s[i].sh_addralign =
					otherEnd32(Ecvt,scntbl_s[i].sh_addralign);
				scntbl_s[i].sh_entsize = 
					otherEnd32(Ecvt,scntbl_s[i].sh_entsize);
			}
			Write(stripfd,(char *)scntbl_s,(int)(newsectot*Ehdr.e_shentsize));
			for(i=0;i<newsectot;i++) {
				scntbl_s[i].sh_name = otherEnd32(Ecvt,scntbl_s[i].sh_name);
				scntbl_s[i].sh_type = otherEnd32(Ecvt,scntbl_s[i].sh_type);
				scntbl_s[i].sh_flags = otherEnd32(Ecvt,scntbl_s[i].sh_flags);
				scntbl_s[i].sh_addr = otherEnd32(Ecvt,scntbl_s[i].sh_addr);
				scntbl_s[i].sh_offset = otherEnd32(Ecvt,scntbl_s[i].sh_offset);
				scntbl_s[i].sh_size = otherEnd32(Ecvt,scntbl_s[i].sh_size);
				scntbl_s[i].sh_link = otherEnd32(Ecvt,scntbl_s[i].sh_link);
				scntbl_s[i].sh_info = otherEnd32(Ecvt,scntbl_s[i].sh_info);
				scntbl_s[i].sh_addralign =
					otherEnd32(Ecvt,scntbl_s[i].sh_addralign);
				scntbl_s[i].sh_entsize =
					otherEnd32(Ecvt,scntbl_s[i].sh_entsize);
			}
			offset_delta += (Ehdr.e_shnum - newsectot) * Ehdr.e_shentsize;
		}
	}

	if (debug)
		fprintf(stderr,"offset_delta = %d\n",offset_delta);

	/* Adjust the file header to point to the new section header table. */
	Lseek(stripfd,0x20,SEEK_SET);
	shoffs = otherEnd32(Ecvt,shoffs);
	Write(stripfd,(char *)&shoffs,4);
	
	close(stripfd);
}

#if 0
	if (append) {
		struct	stat buf;
		char	*aspace;
		if ((afd = open(append,O_RDONLY|O_BINARY))==-1)
		{
			perror(append);
			exit(1);
		}
		stat(append,&buf);
		aspace = Malloc(buf.st_size+32);
		read(afd,aspace,buf.st_size);
		Write(sfd,aspace,buf.st_size);
		free(aspace);
		close(afd);
	}
	close(sfd);
#endif

void
ShowElfMap(long flag)
{
	int	i;
	unsigned long	size, totsize;
	struct	elf_shdr	*eshdr;

	if (debug >= 2)
		fprintf(stderr,"ShowElfMap()\n");

	totsize = 0;
	for(i=0;i<Ehdr.e_shnum;i++) {
		eshdr = &ScnTbl[i];
		size = eshdr->sh_size;
		if (size)
			size--;
		else if (flag & SHOWMAPNONZERO)
			continue;
		if ((eshdr->sh_flags) || (flag & SHOWMAPOFFSET)) {
			printf("%18s: 0x%08x..0x%08x (%ld bytes) %s\n",
				GetElfSectionName(eshdr->sh_name),
				eshdr->sh_addr,eshdr->sh_addr+size,eshdr->sh_size,
				VerboseShflags(eshdr->sh_flags));
				totsize += eshdr->sh_size;
			if (flag & SHOWMAPOFFSET) {
				printf("            ELF file offset: 0x%08x\n",
					eshdr->sh_offset);
			}
		}
	}
	printf("Entrypoint: 0x%08x\n",Ehdr.e_entry);
	printf("Total size: 0x%08lx (%ld)\n",totsize,totsize);
}

char *
VerboseShtype(unsigned long type)
{
	switch(type) {
	case SHT_NULL:
		return("null");
	case SHT_PROGBITS:
		return("progbits");
	case SHT_SYMTAB:
		return("symtab");
	case SHT_STRTAB:
		return("strtab");
	case SHT_RELA:
		return("rela");
	case SHT_HASH:
		return("hash");
	case SHT_DYNAMIC:
		return("dynamic");
	case SHT_NOTE:
		return("note");
	case SHT_NOBITS:
		return("nobits");
	case SHT_REL:
		return("rel");
	case SHT_SHLIB:
		return("shlib");
	case SHT_DYNSYM:
		return("dynsym");
	default:
		fprintf(stderr,"Unexpeced type: 0x%x\n",type);
		exit(1);
	}
}

char *
VerboseShflags(unsigned long flags)
{
	static	char buf[32];

	*buf = 0;
	if (flags & SHF_ALLOC)
		strcat(buf,"allocated ");
	if (flags & SHF_WRITE)
		strcat(buf,"writeable ");
	if (flags & SHF_EXECINSTR)
		strcat(buf,"text ");
	return(buf);
}

char *
GetElfSectionName(unsigned long index)
{
	static	char buf[64];
	char	*strings, *name;
	struct	elf_shdr	*eshdr;

	if (debug >= 2)
		fprintf(stderr,"GetElfSectionName(%d)\n",index);

	buf[0] = (char)0;
	eshdr = &ScnTbl[Ehdr.e_shstrndx];
	Lseek(elfFD,eshdr->sh_offset,SEEK_SET);
	strings = Malloc((unsigned)(eshdr->sh_size));
	Read(elfFD,strings,(unsigned)(eshdr->sh_size));
	name = Malloc(strlen(&strings[index]) + 1);
	strcpy(name,&strings[index]);
	free(strings);
	return(name);
}

/* GetElfFileHdr():
   Retrieve the header from the file and do a endian conversion
   on each of its members.
*/
struct elf_fhdr *
GetElfFileHdr(struct elf_fhdr *efhdr)
{
	static int	pass = 0;

	if (debug >= 2)
		fprintf(stderr,"GetElfFileHdr()\n");

retry:

	/* Read in the elf header. */
	Lseek(elfFD,0,SEEK_SET);
	Read(elfFD,(char *)efhdr,(unsigned)sizeof(struct elf_fhdr));

	/* If this is the first pass through this function, then
	 * read in the size of the program header.  If the size is
	 * greater than 4k, then assume we need to do an endian
	 * adjustment...
	 */
	if (pass == 0) {
		pass++;
		efhdr->e_phentsize = otherEnd16(Ecvt,efhdr->e_phentsize);
		if (efhdr->e_phentsize > 0x1000) {
			printf("Automatic endianness adjustment\n");
#if HOST_IS_SOLARIS
			Ecvt = 1;
#else
			Ecvt = 0;
#endif
			goto retry;
		}
	}

	efhdr->e_type = otherEnd16(Ecvt,efhdr->e_type);
	efhdr->e_machine = otherEnd16(Ecvt,efhdr->e_machine);
	efhdr->e_version = otherEnd32(Ecvt,efhdr->e_version);
	efhdr->e_entry = otherEnd32(Ecvt,efhdr->e_entry);
	efhdr->e_phoff = otherEnd32(Ecvt,efhdr->e_phoff);
	efhdr->e_shoff = otherEnd32(Ecvt,efhdr->e_shoff);
	efhdr->e_flags = otherEnd32(Ecvt,efhdr->e_flags);
	efhdr->e_ehsize = otherEnd16(Ecvt,efhdr->e_ehsize);
	efhdr->e_phentsize = otherEnd16(Ecvt,efhdr->e_phentsize);
	efhdr->e_phnum = otherEnd16(Ecvt,efhdr->e_phnum);
	efhdr->e_shentsize = otherEnd16(Ecvt,efhdr->e_shentsize);
	efhdr->e_shnum = otherEnd16(Ecvt,efhdr->e_shnum);
	efhdr->e_shstrndx = otherEnd16(Ecvt,efhdr->e_shstrndx);
	return(efhdr);
}

Elf32_Half
GetElfSectionHdrs(void)
{
	int	i, size;

	/* Allocate space for the section table in memory. */
	size = Ehdr.e_shnum * Ehdr.e_shentsize;
	ScnTbl = (struct elf_shdr *)Malloc(size);

	/* Read in the elf section headers. */
	Lseek(elfFD,Ehdr.e_shoff,SEEK_SET);
	Read(elfFD,(char *)ScnTbl,size);

	for(i=0;i<Ehdr.e_shnum;i++) {
		ScnTbl[i].sh_name = otherEnd32(Ecvt,ScnTbl[i].sh_name);
		ScnTbl[i].sh_type = otherEnd32(Ecvt,ScnTbl[i].sh_type);
		ScnTbl[i].sh_flags = otherEnd32(Ecvt,ScnTbl[i].sh_flags);
		ScnTbl[i].sh_addr = otherEnd32(Ecvt,ScnTbl[i].sh_addr);
		ScnTbl[i].sh_offset = otherEnd32(Ecvt,ScnTbl[i].sh_offset);
		ScnTbl[i].sh_size = otherEnd32(Ecvt,ScnTbl[i].sh_size);
		ScnTbl[i].sh_link = otherEnd32(Ecvt,ScnTbl[i].sh_link);
		ScnTbl[i].sh_info = otherEnd32(Ecvt,ScnTbl[i].sh_info);
		ScnTbl[i].sh_addralign = otherEnd32(Ecvt,ScnTbl[i].sh_addralign);
		ScnTbl[i].sh_entsize = otherEnd32(Ecvt,ScnTbl[i].sh_entsize);
	}

	return(Ehdr.e_shnum);
}

Elf32_Half
GetElfProgramHdrTbl(void)
{
	int	i, size;

	/* Allocate space for the program header table in memory.
	 */
	size = Ehdr.e_phnum * Ehdr.e_phentsize;
	PhdrTbl = (struct elf_phdr *)Malloc(size);

	/* Read in the elf section headers. */
	Lseek(elfFD,Ehdr.e_phoff,SEEK_SET);
	Read(elfFD,(char *)PhdrTbl,size);

	for(i=0;i<Ehdr.e_phnum;i++) {
		PhdrTbl[i].p_type = otherEnd32(Ecvt,PhdrTbl[i].p_type);
		PhdrTbl[i].p_offset = otherEnd32(Ecvt,PhdrTbl[i].p_offset);
		PhdrTbl[i].p_vaddr = otherEnd32(Ecvt,PhdrTbl[i].p_vaddr);
		PhdrTbl[i].p_paddr = otherEnd32(Ecvt,PhdrTbl[i].p_paddr);
		PhdrTbl[i].p_filesz = otherEnd32(Ecvt,PhdrTbl[i].p_filesz);
		PhdrTbl[i].p_memsz = otherEnd32(Ecvt,PhdrTbl[i].p_memsz);
		PhdrTbl[i].p_flags = otherEnd32(Ecvt,PhdrTbl[i].p_flags);
		PhdrTbl[i].p_align = otherEnd32(Ecvt,PhdrTbl[i].p_align);
	}

	return(Ehdr.e_phnum);
}

char *usage_txt[] = {
	"Usage: elf [options] {filename}",
	" Options:",
	" -a{filename}  append file to end of -S file",
	" -A            print what was appended by -a",
	" -B{filename}  elf-2-bin to filename",
	" -c            endian convert",
	" -d            debug mode (for development only)",
	" -f            show elf file header",
	" -i#           insert '#' pad bytes, 4 bytes prior to end of binary",
	" -M            show elf map with file offsets",
	" -m            show elf map without file offsets",
	" -p            dump program header table.",
	" -P{pad byte}  byte to use for padding (default = 0xff)",
	" -s            show elf section headers",
	" -S{filename}  strip to specified file",
	" -V            display version of tool",
	" -v            verbose mode",
	" -z{zlvl}      compress sections of filename to 'filename.ezip'",
	"               zlvl can range from 1-9 (typically 6) where...",
	"               1 is faster/lighter compression",
	"               9 is slower/heavier compression",
	" -Z{zlvl}      same as -z, but compressed section files are not removed",
	"",
	" Note:",
	"  The GNU-equivalent for 'elf -B outfile.bin infile.elf' is:",
	"    objcopy --output-target=binary --gap-fill 0xff infile.elf outfile.bin",
	0,
};

int
main(int argc,char *argv[])
{
	extern	int optind;
	extern	char *optarg;
	char	fname[128], buf[16], *append, *binto, *stripto;
	int	opt, showstuff, zlevel;

	/* sanity checks, to prevent Bad Things from happening
	   on 64-bit hosts */
	assert(sizeof(Elf32_Half) == 2);
	assert(sizeof(Elf32_Word) == 4);
	assert(sizeof(Elf32_Addr) == 4);
	assert(sizeof(Elf32_Off) == 4);

	elfFD = 0;
	zlevel = 0;
	debug = 0;
	xdebug = 0;
	verbose = 0;
	insertPAD = 0;
	showstuff = 0;
#if HOST_IS_SOLARIS
	Ecvt = 0;
#else
	Ecvt = 1;
#endif
	PadByte = 0xff;
	binto = (char *)0;
	append = (char *)0;
	stripto = (char *)0;
	elfFname = (char *)0;

	while ((opt=getopt(argc,argv,"Aa:B:cdfi:mMpP:S:sVvxz:Z:")) != EOF) {
		switch(opt) {
		case 'a':
			append = optarg;
			break;
		case 'A':
			showstuff |= SHOWAPPEND;
			break;
		case 'B':
			binto = optarg;
			break;
		case 'c':
#if HOST_IS_SOLARIS
			Ecvt = 1;
#else
			Ecvt = 0;
#endif
			break;
		case 'd':
			debug++;
			break;
		case 'f':
			showstuff |= SHOWELFHDR;
			break;
		case 'i':
			insertPAD = strtol(optarg,0,0);
			break;
		case 'M':
			showstuff |= SHOWMAPOFFSET;
			break;
		case 'm':
			if (showstuff & SHOWMAP)
				showstuff |= SHOWMAPNONZERO;
			else
				showstuff |= SHOWMAP;
			break;
		case 'p':
			showstuff |= SHOWPRGMHDR;
			break;
		case 'P':
			PadByte = strtol(optarg,0,0);
			break;
		case 's':
			showstuff |= SHOWSECTIONS;
			break;
		case 'S':
			stripto = optarg;
			break;
		case 'V':
			showVersion();
			break;
		case 'v':
			verbose = 1;
			break;
		case 'x':
			xdebug++;
			break;
		case 'Z':
			remove_zouts = 0;
			zlevel = atoi(optarg);
			if ((zlevel < 1) || (zlevel > 9))
				usage("zlvl out of range\n");
			break;
		case 'z':
			zlevel = atoi(optarg);
			if ((zlevel < 1) || (zlevel > 9))
				usage("zlvl out of range\n");
			break;
		default:
			usage(0);
		}
	}

	if (argc != (optind+1))
		usage("Missing filename");

	strcpy(fname,argv[optind]);

	if (verbose)
		printf("\n%s:\n",fname);

	if (!fileexists(fname)) {
		fprintf(stderr,"No such file: %s\n",fname);
		exit(1);
	}

	if ((elfFD = open(fname,O_RDONLY | O_BINARY)) == -1) {
		perror(fname);
		return(0);
	}

	/* Start by verifying that the file is ELF format... */
	Read(elfFD,buf,4);
	if ((buf[0] != 0x7f) || (buf[1] != 'E') ||
	    (buf[2] != 'L') || (buf[3] != 'F')) {
		fprintf(stderr,"File '%s' not ELF format\n",fname);
		exit(1);
	}

	elfFname = StrAllocAndCopy(fname);
	GetElfFileHdr(&Ehdr);
	GetElfProgramHdrTbl();
	GetElfSectionHdrs();

	if (zlevel) {
		ZipElfSections(zlevel);
	}
	else if (binto) {
		ElfToBinary(binto);
	}
	else if (stripto) {
		StripElfFile(stripto,append);
	}
	else {
		if (showstuff & SHOWAPPEND) 
			ShowAppend();
		if ((showstuff & SHOWMAPOFFSET) ||
			(showstuff & SHOWMAP) ||
			(showstuff & SHOWMAPNONZERO)) {
			ShowElfMap(showstuff);
		}
		if (showstuff & SHOWELFHDR) 
			ShowElfHdr();
		if (showstuff & SHOWSECTIONS) 
			ShowSections(verbose);
		if (showstuff & SHOWPRGMHDR) 
			ShowProgramHdrTbl(verbose);
	}
	return(0);
}
