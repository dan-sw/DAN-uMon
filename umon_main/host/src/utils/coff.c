/* coff.c:
	Perform some dumps/conversions of a coff file.

	General notice:
	This code is part of a boot-monitor package developed as a generic base
	platform for embedded system designs.  As such, it is likely to be
	distributed to various projects beyond the control of the original
	author.  Please notify the author of any enhancements made or bugs found
	so that all may benefit from the changes.  In addition, notification back
	to the author will allow the new user to pick up changes that may have
	been made by other users after this version of the code was distributed.

	Author:	Ed Sutter
	email:	esutter@lucent.com
	phone:	908-582-2351
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef BUILD_WITH_VCC
#include <io.h>
#else
#include <unistd.h>
#endif
#include "packdata.h"
#include "coff.h"
#include "../zlib/zlib.h"
#include "utils.h"
#include "version.h"

#ifndef O_BINARY
#define O_BINARY 0
#endif

int 	coffFD = 0, noPad;
int		tiMode = 0;
char	*coffFname = (char *)0;

struct	filehdr	CoffFhdr;
struct	aouthdr	OptAhdr;
struct	scnhdr	*ScnTbl;

/* open verbosity: */
#define SHOWHDR			0x0001
#define SHOWSECTIONS	0x0002
#define SHOWMAP			0x0004
#define SHOWMAPOFFSET	0x0008
#define SHOWAPPEND		0x0010

struct	filehdr *GetCoffFileHdr();
int		Ecvt, verbose, debug;


void
ShowAppend(void)
{
	int	i;
	char	c;
	long	maxsize, maxoff;

	maxoff = 0;
	for(i=0;i<CoffFhdr.f_nscns;i++) {
		if (ScnTbl[i].s_scnptr > maxoff) {
			maxoff = ScnTbl[i].s_scnptr;
			maxsize = ScnTbl[i].s_size;
		}
	}
	Lseek(coffFD,maxoff+maxsize,SEEK_SET);
	while(1) {
		if (read(coffFD,&c,1) != 1)
			break;
		putchar(c);
	}

}

void
ShowCoffSections(void)
{
	int	i;

	printf("\n\t\tCOFF FILE SECTION HEADERS\n");
	for(i=0;i<CoffFhdr.f_nscns;i++) {
		printf("Section: %s...\n",ScnTbl[i].s_name);
		printf("  s_paddr:   0x%x\n",ScnTbl[i].s_paddr);
		printf("  s_vaddr:   0x%x\n",ScnTbl[i].s_vaddr);
		printf("  s_size:    %d\n",ScnTbl[i].s_size);
		printf("  s_scnptr:  0x%x\n",ScnTbl[i].s_scnptr);
		printf("  s_relptr:  0x%x\n",ScnTbl[i].s_relptr);
		printf("  s_lnnoptr: 0x%x\n",ScnTbl[i].s_lnnoptr);
		printf("  s_nreloc:  %d\n",ScnTbl[i].s_nreloc);
		printf("  s_nlnno:   %d\n",ScnTbl[i].s_nlnno);
		printf("  s_flags:   0x%x\n\n",ScnTbl[i].s_flags);
		if (ScnTbl[i].s_size < 0) {
			fprintf(stderr,"Warning: section size < 0\n");
			break;
		}
	}
}

void
ShowCoffHdr(void)
{
	printf("\n\t\tCOFF FILE HEADER\n");

	printf("Magic:				0x%x\n",CoffFhdr.f_magic);
	if (CoffFhdr.f_opthdr) {
		printf("Opt magic:			0x%x (%d)\n",
			OptAhdr.magic,OptAhdr.magic);
		printf("Text size:			0x%x (%d)\n",
			OptAhdr.tsize,OptAhdr.tsize);
		printf("Data size:			0x%x (%d)\n",
			OptAhdr.dsize,OptAhdr.dsize);
		printf("Bss size:			0x%x (%d)\n",
			OptAhdr.bsize,OptAhdr.bsize);
		printf("Entrypoint:			0x%x\n",OptAhdr.entry);
	}
	printf("Number of sections:		%d\n",CoffFhdr.f_nscns);
	printf("Number of symbols:		%d\n",CoffFhdr.f_nsyms);
}

void
ShowCoffMap(void)
{
	int	i, size;

	printf("\n\t\tCOFF FILE MEMORY MAP\n\n");
	for(i=0;i<CoffFhdr.f_nscns;i++) {
		char name[9];
		memcpy(name,ScnTbl[i].s_name,8);
		name[8] = 0;
		
		size = ScnTbl[i].s_size;
		printf("%8s: 0x%08x .. 0x%08x, 0x%x (%d) bytes.\n",
			name,ScnTbl[i].s_paddr,ScnTbl[i].s_paddr+size,size,size);
	}
}

void
ShowCoffOffsets(void)
{
	int	i, start;

	start = sizeof(struct filehdr) + CoffFhdr.f_opthdr;
	printf("\n\t\tCOFF FILE SECTION FILE OFFSETS\n");

	for(i=0;i<CoffFhdr.f_nscns;i++) {
		printf("%8s: 0x%08x .. 0x%08x (%d bytes)\n",
			ScnTbl[i].s_name,ScnTbl[i].s_scnptr,
			ScnTbl[i].s_scnptr+ScnTbl[i].s_size,
			ScnTbl[i].s_size);
		start += ScnTbl[i].s_size;
	}
}

/* GetCoffFileHdr():
   Retrieve the header from the file and do a BE-to-LE conversion
   on each of its members.
*/
struct filehdr *
GetCoffFileHdr(void)
{
	off_t here;

	if (verbose)
		fprintf(stderr,"GetCoffFileHdr()\n");

	/* Read in the coff file header. */
	Lseek(coffFD,0,SEEK_SET);
	Read(coffFD,(char *)&CoffFhdr,(unsigned)sizeof(struct filehdr));
	CoffFhdr.f_magic = otherEnd16(Ecvt,CoffFhdr.f_magic);
	CoffFhdr.f_nscns = otherEnd16(Ecvt,CoffFhdr.f_nscns);
	CoffFhdr.f_timdat = otherEnd32(Ecvt,CoffFhdr.f_timdat);
	CoffFhdr.f_symptr = otherEnd32(Ecvt,CoffFhdr.f_symptr);
	CoffFhdr.f_nsyms = otherEnd32(Ecvt,CoffFhdr.f_nsyms);
	CoffFhdr.f_opthdr = otherEnd16(Ecvt,CoffFhdr.f_opthdr);
	CoffFhdr.f_flags = otherEnd16(Ecvt,CoffFhdr.f_flags);

	if (CoffFhdr.f_opthdr) {
		here = Lseek(coffFD,0,SEEK_CUR);
		if (tiMode != 0)
			Lseek(coffFD,2,SEEK_CUR);
		Read(coffFD,(char *)&OptAhdr,(unsigned)sizeof(struct aouthdr));
		OptAhdr.magic = otherEnd16(Ecvt,OptAhdr.magic);
		OptAhdr.vstamp = otherEnd16(Ecvt,OptAhdr.vstamp);
		OptAhdr.tsize = otherEnd32(Ecvt,OptAhdr.tsize);
		OptAhdr.dsize = otherEnd32(Ecvt,OptAhdr.dsize);
		OptAhdr.bsize = otherEnd32(Ecvt,OptAhdr.bsize);
		OptAhdr.entry = otherEnd32(Ecvt,OptAhdr.entry);
		OptAhdr.text_start = otherEnd32(Ecvt,OptAhdr.text_start);
		OptAhdr.data_start = otherEnd32(Ecvt,OptAhdr.data_start);
		if (CoffFhdr.f_opthdr != sizeof(struct aouthdr)) {
			fprintf(stderr,"Warning: f_opthdr != size of aouthdr\n");
			Lseek(coffFD,here+CoffFhdr.f_opthdr,SEEK_SET);
		}
	}
	return(&CoffFhdr);
}

int
GetCoffSectionHdrs(void)
{
	int	i;

	ScnTbl = (struct scnhdr *)Malloc(CoffFhdr.f_nscns * sizeof(struct scnhdr));

	/* Read in the coff section headers. */
	if (tiMode == 0)
		Lseek(coffFD,sizeof(struct filehdr)+CoffFhdr.f_opthdr,SEEK_SET);
	else
		Lseek(coffFD,0x782,SEEK_SET);


	for(i=0;i<CoffFhdr.f_nscns;i++) {
		Read(coffFD,(char *)&ScnTbl[i],sizeof(struct scnhdr));

		ScnTbl[i].s_paddr = otherEnd32(Ecvt,ScnTbl[i].s_paddr);
		ScnTbl[i].s_vaddr = otherEnd32(Ecvt,ScnTbl[i].s_vaddr);
		ScnTbl[i].s_size = otherEnd32(Ecvt,ScnTbl[i].s_size);
		ScnTbl[i].s_scnptr = otherEnd32(Ecvt,ScnTbl[i].s_scnptr);
		ScnTbl[i].s_relptr = otherEnd32(Ecvt,ScnTbl[i].s_relptr);
		ScnTbl[i].s_lnnoptr = otherEnd32(Ecvt,ScnTbl[i].s_lnnoptr);
		ScnTbl[i].s_nreloc = otherEnd16(Ecvt,ScnTbl[i].s_nreloc);
		ScnTbl[i].s_nlnno = otherEnd16(Ecvt,ScnTbl[i].s_nlnno);
		ScnTbl[i].s_flags = otherEnd32(Ecvt,ScnTbl[i].s_flags);
		if (tiMode)
			Lseek(coffFD,8,SEEK_CUR);
	}

	return(CoffFhdr.f_nscns);
}

void
CoffToBinary(char *binto)
{
	int	fd, sidx, firstone;
	uchar	*cp;
	struct	scnhdr	*sptr;

	unlink(binto);
	if ((fd = open(binto,O_WRONLY|O_BINARY|O_CREAT,0777))==-1)
	{
		perror(binto);
		exit(1);
	}
	fprintf(stderr,"Converting %s into %s\n",
		coffFname,binto);
	firstone = 1;
	for(sptr=ScnTbl,sidx=0;sidx<CoffFhdr.f_nscns;sidx++,sptr++) {
		char	name[9], padbyte;
		int		padtot;
		long	nextpaddr;
		
		memcpy(name,sptr->s_name,8);
		name[8] = 0;

		if ((sptr->s_flags != STYP_BSS) &&
		    (sptr->s_flags != STYP_INFO) &&
		    (sptr->s_size)) {
			if (!firstone) {	/* Pad if necessary... */
				if ((!noPad) && (nextpaddr != sptr->s_paddr)) {
					if (sptr->s_paddr < nextpaddr) {
						fprintf(stderr,"Unexpected section address\n");
						exit(1);
					}
					padtot = sptr->s_paddr-nextpaddr;
					printf("Pad 0x%x bytes\n",padtot);
					padbyte = 0;
					while(padtot) {
						write(fd,&padbyte,1);
						padtot--;
					}
				}
			}
			printf("Sec %-8s at 0x%-8x 0x%-6x bytes\n",
				name,sptr->s_paddr,sptr->s_size);
			cp = (uchar *)Malloc(sptr->s_size);
			Lseek(coffFD,sptr->s_scnptr,SEEK_SET);
			read(coffFD,cp,sptr->s_size);
			write(fd,cp,sptr->s_size);
			free(cp);
			nextpaddr = sptr->s_paddr + sptr->s_size;
			firstone = 0;
		}
	}
	close(fd);
}	

void
PackCoffSections(char *packto)
{
	char	*ibuf,*obuf, *sdata, *obase, *ibase;
	int		i, pfd, sno, savings;
	struct	stat finfo;
	struct  filehdr *coffp;
	struct	scnhdr *sp;

	unlink(packto);
	if ((pfd = open(packto,O_WRONLY|O_BINARY|O_CREAT,0777))==-1)
	{
		perror(packto);
		exit(1);
	}
	fprintf(stderr,"Packing %s into %s...\n",
		coffFname,packto);

	/* First copy the entire COFF file into the input buffer: */
	fstat(coffFD,&finfo);
	Lseek(coffFD,0,SEEK_SET);
	ibuf = Malloc(finfo.st_size+32);
	ibase = ibuf;
	obuf = Malloc(finfo.st_size+32);
	obase = obuf;
	coffp = (struct filehdr *)obase;
	Read(coffFD,ibuf,finfo.st_size);

	/* Now copy the file header and optional header to the out buffer: */
	for(i=0;i<sizeof(struct filehdr)+CoffFhdr.f_opthdr;i++)
		*obuf++ = *ibuf++;

	/* Since the output file will be stripped, modify the new header: */
	coffp->f_symptr = 0;
	coffp->f_nsyms = 0;

	/* At this point ibuf & obuf are at the base of the table of section */
	/* headers.  The section headers must be adjusted because the size */
	/* of the sections will be changing, so for each section, compress */
	/* the data and adjust the header information... */
	sp = (struct scnhdr *)obuf;

	/* Set sdata to point into the new buffer at the point immediately */
	/* after the headers. This pointer will be used to write the new */
	/* compressed sections. */
	sdata = obase + sizeof(struct filehdr) + CoffFhdr.f_opthdr +
		(CoffFhdr.f_nscns * sizeof(struct scnhdr));

	/* For each section, if the section is loadable, the compress the */
	/* data associated with that section... */
	savings = 0;
	for (sno=0;sno<CoffFhdr.f_nscns;sno++) {
		int	psize;

		/* If the section has data, compress it and adjust the */
		/* section header information. */
		if (ScnTbl[sno].s_scnptr) {
			/* Compress the section: */
			printf("Section %s: ",ScnTbl[sno].s_name);
			psize = packdata(ibase+ScnTbl[sno].s_scnptr,sdata,
				ScnTbl[sno].s_size,1);

			/* Keep track of total space saved: */
			savings += (ScnTbl[sno].s_size - psize);

			/* Adjust the affected section header members: */
			ScnTbl[sno].s_size = psize;
			ScnTbl[sno].s_scnptr = (unsigned long)(sdata - obase);
			sdata += psize;
		}

		memcpy(sp->s_name,ScnTbl[sno].s_name,8);
		sp->s_paddr = otherEnd32(Ecvt,ScnTbl[sno].s_paddr);
		sp->s_vaddr = otherEnd32(Ecvt,ScnTbl[sno].s_vaddr);
		sp->s_size = otherEnd32(Ecvt,ScnTbl[sno].s_size);
		sp->s_scnptr = otherEnd32(Ecvt,ScnTbl[sno].s_scnptr);
		sp->s_relptr = otherEnd32(Ecvt,ScnTbl[sno].s_relptr);
		sp->s_lnnoptr = otherEnd32(Ecvt,ScnTbl[sno].s_lnnoptr);
		sp->s_nreloc = otherEnd16(Ecvt,ScnTbl[sno].s_nreloc);
		sp->s_nlnno = otherEnd16(Ecvt,ScnTbl[sno].s_nlnno);
		sp->s_flags = otherEnd32(Ecvt,ScnTbl[sno].s_flags);
		sp++;
	}

	if (write(pfd,(char *)obase,sdata-obase) != (sdata-obase)) {
		perror(packto);
		exit(1);
	}
	close(pfd);
	free(ibase);
	free(obase);
	printf("Total savings of %d bytes\n",savings);
}

int
zipdata(src,dest,srcsize,mode,verbose)
char	*src, *dest, *mode;
int		srcsize, verbose;
{
	int	 zfd;
	FILE *zout;
	char *zoutfile;
	struct stat zstat;

	zoutfile = "zout";
	unlink(zoutfile);

   	zout = gzopen(zoutfile, mode);
    if (zout == NULL) {
        fprintf(stderr, "can't gzopen %s\n", zoutfile);
        exit(1);
    }
	if (gzwrite(zout, src, (unsigned)srcsize) != srcsize) {
		fprintf(stderr, "gzwrite failed\n");
		exit(1);
	}
    if (gzclose(zout) != Z_OK) {
        fprintf(stderr, "gzclose failed\n");
		exit(1);
	}
	stat(zoutfile,&zstat);
	if ((zfd = open(zoutfile,O_RDONLY|O_BINARY)) == -1) {
        fprintf(stderr, "can't open %s\n",zoutfile);
		exit(1);
	}
	Read(zfd,dest,zstat.st_size);
	close(zfd);
	unlink(zoutfile);
	if (verbose) {
		printf("%d%% compression (from %d to %d bytes)\n",
			(int)(((double)(zstat.st_size)/
			 (double)srcsize)*100),srcsize,zstat.st_size);
	}
	return(zstat.st_size);
}

void
ZipCoffSections(int zlevel)
{
	char	*ibuf,*obuf, *sdata, *obase, *ibase;
	int		i, pfd, sno, savings;
	char	zipto[128], mode[16];
	struct	stat finfo;
	struct  filehdr *coffp;
	struct	scnhdr *sp;

	sprintf(mode,"wb%d ",zlevel);
	sprintf(zipto,"%s.czip",coffFname);
	fprintf(stderr,"Compressing %s into %s \n",coffFname,zipto);

	unlink(zipto);
	if ((pfd = open(zipto,O_WRONLY|O_BINARY|O_CREAT,0777))==-1) {
		perror(zipto);
		exit(1);
	}
	fprintf(stderr,"Zipping %s into %s...\n",coffFname,zipto);

	/* First copy the entire COFF file into the input buffer: */
	fstat(coffFD,&finfo);
	Lseek(coffFD,0,SEEK_SET);
	ibuf = Malloc(finfo.st_size+32);
	ibase = ibuf;
	obuf = Malloc(finfo.st_size+32);
	obase = obuf;
	coffp = (struct filehdr *)obase;
	Read(coffFD,ibuf,finfo.st_size);

	/* Now copy the file header and optional header to the out buffer: */
	for(i=0;i<sizeof(struct filehdr)+CoffFhdr.f_opthdr;i++)
		*obuf++ = *ibuf++;

	/* Since the output file will be stripped, modify the new header: */
	coffp->f_symptr = 0;
	coffp->f_nsyms = 0;

	/* At this point ibuf & obuf are at the base of the table of section */
	/* headers.  The section headers must be adjusted because the size */
	/* of the sections will be changing, so for each section, compress */
	/* the data and adjust the header information... */
	sp = (struct scnhdr *)obuf;

	/* Set sdata to point into the new buffer at the point immediately */
	/* after the headers. This pointer will be used to write the new */
	/* compressed sections. */
	sdata = obase + sizeof(struct filehdr) + CoffFhdr.f_opthdr +
		(CoffFhdr.f_nscns * sizeof(struct scnhdr));

	/* For each section, if the section is loadable, the compress the */
	/* data associated with that section... */
	savings = 0;
	for (sno=0;sno<CoffFhdr.f_nscns;sno++) {
		int	psize;

		/* If the section has data, compress it and adjust the */
		/* section header information. */
		if (ScnTbl[sno].s_scnptr) {
			/* Compress the section: */
			printf("Section %s: ",ScnTbl[sno].s_name);
			psize = zipdata(ibase+ScnTbl[sno].s_scnptr,sdata,
				ScnTbl[sno].s_size,mode,1);

			/* Keep track of total space saved: */
			savings += (ScnTbl[sno].s_size - psize);

			/* Adjust the affected section header members: */
			ScnTbl[sno].s_size = psize;
			ScnTbl[sno].s_scnptr = (unsigned long)(sdata - obase);
			sdata += psize;
		}

		memcpy(sp->s_name,ScnTbl[sno].s_name,8);
		sp->s_paddr = otherEnd32(Ecvt,ScnTbl[sno].s_paddr);
		sp->s_vaddr = otherEnd32(Ecvt,ScnTbl[sno].s_vaddr);
		sp->s_size = otherEnd32(Ecvt,ScnTbl[sno].s_size);
		sp->s_scnptr = otherEnd32(Ecvt,ScnTbl[sno].s_scnptr);
		sp->s_relptr = otherEnd32(Ecvt,ScnTbl[sno].s_relptr);
		sp->s_lnnoptr = otherEnd32(Ecvt,ScnTbl[sno].s_lnnoptr);
		sp->s_nreloc = otherEnd16(Ecvt,ScnTbl[sno].s_nreloc);
		sp->s_nlnno = otherEnd16(Ecvt,ScnTbl[sno].s_nlnno);
		sp->s_flags = otherEnd32(Ecvt,ScnTbl[sno].s_flags);
		sp++;
	}

	if (write(pfd,(char *)obase,sdata-obase) != (sdata-obase)) {
		perror(zipto);
		exit(1);
	}
	close(pfd);
	free(ibase);
	free(obase);
	printf("Total savings of %d bytes\n",savings);
}

void
StripCoffFile(char *stripto,char *append)
{
	int	size, sfd, afd;
	struct  filehdr *coffp;

	unlink(stripto);
	if ((sfd = open(stripto,O_WRONLY|O_BINARY|O_CREAT,0777))==-1)
	{
		perror(stripto);
		exit(1);
	}
	fprintf(stderr,"Stripping %s into %s...\n",
		coffFname,stripto);
	Lseek(coffFD,0,SEEK_SET);
	size = CoffFhdr.f_symptr;
	coffp = (struct filehdr *)Malloc(size+32);
	Read(coffFD,(char *)coffp,size);
	coffp->f_symptr = 0;
	coffp->f_nsyms = 0;
	if (write(sfd,(char *)coffp,size) != size) {
		perror(stripto);
		exit(1);
	}
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
		write(sfd,aspace,buf.st_size);
		free(aspace);
		close(afd);
	}
	close(sfd);
}

char *usage_txt[] = {
	"Usage: coff [options] {filename}",
	" Options:",
	" -a{filename}  append file to end of -S file",
	" -A            print what was appended by -a",
	" -B{filename}  coff-2-bin to filename",
	" -c            BE-to-LE convert",
	" -f            show coff file header",
	" -M            show coff map with file offsets",
	" -m            show coff map without file offsets",
	" -n            no padding with -B option",
	" -p{filename}  pack to specified file (output file is also stripped)",
	" -s            show coff section headers",
	" -S{filename}  strip to specified file",
	" -T{mode}      TI-COFF mode (experimental)",
	" -V            display version of tool",
	" -v            verbose mode",
	" -z{zlvl}      compress sections of filename to 'filename.czip'",
	"               zlvl can range from 1-9 (typically 6) where...",
	"               1 is faster/lighter compression",
	"               9 is slower/heavier compression",
	0,
};

int
main(int argc,char *argv[])
{
	extern	int optind;
	extern	char *optarg;
	char	fname[128], *append, *stripto, *binto, *packto;
	int		tot, opt, showstuff, zlevel;

	zlevel = verbose = 0;
	showstuff = 0;
	packto = (char *)0;
	stripto = (char *)0;
	append = (char *)0;
	binto = (char *)0;
	noPad = 0;
	tiMode = 0;
#ifdef BUILD_WITH_VCC
	Ecvt = 1;
#else
	Ecvt = 0;
#endif
	while ((opt=getopt(argc,argv,"Aa:cB:dfmMnp:sS:T:vVz:")) != EOF) {
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
			if (Ecvt)
				Ecvt = 0;
			else
				Ecvt = 1;
			break;
		case 'f':
			showstuff |= SHOWHDR;
			break;
		case 'M':
			showstuff |= SHOWMAPOFFSET;
			break;
		case 'm':
			showstuff |= SHOWMAP;
			break;
		case 'n':
			noPad = 1;
			break;
		case 'p':
			packto = optarg;
			break;
		case 'S':
			stripto = optarg;
			break;
		case 's':
			showstuff |= SHOWSECTIONS;
			break;
		case 'T':
			tiMode = atoi(optarg);
			break;
		case 'v':
			verbose = 1;
			break;
		case 'V':
			showVersion();
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

	if (!fileexists(fname)) {
		fprintf(stderr,"No such file: %s\n",fname);
		exit(1);
	}

	if ((coffFD = open(fname,O_RDONLY|O_BINARY)) == -1)
	{
		perror(fname);
		exit(1);
	}

	if (append && !stripto)
		usage("-a requires -S");

	tot = 0;
	if (binto) tot++;
	if (packto) tot++;
	if (stripto) tot++;
	if (tot > 1)
		usage("-S, -B and -p options must be run individually");

	coffFname = StrAllocAndCopy(fname);
	GetCoffFileHdr();
	GetCoffSectionHdrs();

	if (binto) {
		CoffToBinary(binto);
	}
	else if (zlevel) {
		ZipCoffSections(zlevel);
	}
	else if (packto) {
		PackCoffSections(packto);
	}
	else if (stripto) {
		StripCoffFile(stripto,append);
	}
	else {
		if (showstuff & SHOWAPPEND)
			ShowAppend();
		if (showstuff & SHOWHDR)
			ShowCoffHdr();
		if (showstuff & SHOWMAP)
			ShowCoffMap();
		if (showstuff & SHOWMAPOFFSET)
			ShowCoffOffsets();
		if (showstuff & SHOWSECTIONS)
			ShowCoffSections();
	}
	return(0);
}
