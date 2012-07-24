/* aout.c:
	Perform some dumps/displays of an aout file.

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
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "aout.h"
#include "utils.h"
#include "version.h"

#ifdef BUILD_WITH_VCC
#include <io.h>
#else
#include <unistd.h>
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

#define btl32(val)	BeChk_btl32(val)

int 	aoutFD = 0;
char	*aoutFname = (char *)0;

struct	exec	Ahdr;

#define SHOWHDR			0x0001
#define SHOWSECTIONS	0x0002
#define SHOWMAP			0x0004
#define SHOWMAPOFFSET	0x0008
#define SHOWAPPEND		0x0010

struct exec *GetAoutFileHdr();
struct exec *afhdr;
int	Ecvt, verbose, debug;

void
StripAoutFile(char *stripto,char *append)
{
	int	size, sfd, afd;
	struct  exec *ep;

	unlink(stripto);
	if ((sfd = open(stripto,O_WRONLY|O_BINARY|O_CREAT,0666))==-1) {
		perror(stripto);
		exit(1);
	}
	fprintf(stderr,"Stripping %s into %s...\n",
		aoutFname,stripto);
	Lseek(aoutFD,0,SEEK_SET);
	size = sizeof(struct exec) + Ahdr.a_text + Ahdr.a_data;
	ep = (struct exec *)Malloc(size+32);
	Read(aoutFD,(char *)ep,size);
	ep->a_syms = 0;
	if (write(sfd,(char *)ep,size) != size) {
		perror(stripto);
		exit(1);
	}
	if (append) {
		struct	stat buf;
		char	*aspace;
		if ((afd = open(append,O_RDONLY|O_BINARY))==-1) {
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

void
AoutToBinary(char *binto)
{
	int	fd;
	struct	stat buf;
	struct  exec *ep;
	char	*tfrom, *dfrom, *cp;

	unlink(binto);
	if ((fd = open(binto,O_WRONLY|O_BINARY|O_CREAT,0777))==-1) {
		perror(binto);
		exit(1);
	}
	fprintf(stderr,"Converting %s into %s\n",
		aoutFname,binto);
	Lseek(aoutFD,0,SEEK_SET);
	stat(aoutFname,&buf);
	cp = Malloc(buf.st_size+32);
	Read(aoutFD,cp,buf.st_size);
	ep = (struct exec *)cp;
	tfrom = (char *)(ep+1);
	dfrom = (char *)(tfrom + Ahdr.a_text);
	if (write(fd,tfrom,Ahdr.a_text) != (int)Ahdr.a_text) {
		perror(binto);
		exit(1);
	}
	if (write(fd,dfrom,Ahdr.a_data) != (int)Ahdr.a_data) {
		perror(binto);
		exit(1);
	}
	close(fd);
}

void
ShowAppend(void)
{
	char c;

	Lseek(aoutFD,
		sizeof(struct exec)+Ahdr.a_text+Ahdr.a_data,SEEK_SET);
	while(1) {
		if (read(aoutFD,&c,1) != 1)
			break;
		putchar(c);
	}
}

void
ShowAoutHdr(void)
{
	printf("\n\t\tA.OUT FILE HEADER\n");
	printf("Magic:				0x%x\n",Ahdr.a_magic);
	printf("Text size:			0x%x (%d)\n",
		Ahdr.a_text,Ahdr.a_text);
	printf("Data size:			0x%x (%d)\n",
		Ahdr.a_data,Ahdr.a_data);
	printf("Bss size:			0x%x (%d)\n",
		Ahdr.a_bss,Ahdr.a_bss);
	printf("SymTable size:			0x%x (%d)\n",
		Ahdr.a_syms,Ahdr.a_syms);
	printf("Entrypoint:			0x%x\n",Ahdr.a_entry);
	printf("Text relocation table size:	0x%x (%d)\n",
		Ahdr.a_trsize,Ahdr.a_trsize);
	printf("Data relocation table size:	0x%x (%d)\n",
		Ahdr.a_drsize,Ahdr.a_drsize);
}

void
ShowAoutMap(void)
{
	unsigned long	tstart, dstart, bstart;

	printf("\n\t\tA.OUT FILE MEMORY MAP\n");
	tstart = Ahdr.a_entry;
	dstart = tstart + Ahdr.a_text;
	bstart = dstart + Ahdr.a_data;
	printf("Text: 0x%08x .. 0x%08x, 0x%x (%d) bytes.\n",
		tstart,dstart-1,Ahdr.a_text,Ahdr.a_text);
	printf("Data: 0x%08x .. 0x%08x, 0x%x (%d) bytes.\n",
		dstart,bstart-1,Ahdr.a_data,Ahdr.a_data);
	printf("Bss:  0x%08x .. 0x%08x, 0x%x (%d) bytes.\n",
		bstart,bstart+Ahdr.a_bss-1,Ahdr.a_bss,Ahdr.a_bss);
}

void
ShowAoutOffsets(void)
{
	unsigned long	tstart, dstart, bstart;

	printf("\n\t\tA.OUT FILE SECTION FILE OFFSETS\n");
	tstart = 0+sizeof(struct exec);
	dstart = tstart + Ahdr.a_text;
	bstart = dstart + Ahdr.a_data;
	printf("Text: 0x%08x .. 0x%08x (%d bytes)\n",
		tstart,dstart-1,Ahdr.a_text);
	printf("Data: 0x%08x .. 0x%08x (%d bytes)\n",
		dstart,bstart-1,Ahdr.a_data);
	printf("Bss:  0x%08x .. 0x%08x (%d bytes)\n",
		bstart,bstart+Ahdr.a_bss-1,Ahdr.a_bss);
}

void
ShowAoutAppendedDate(void)
{
	unsigned long	tstart, dstart, bstart;
	char	c;

	tstart = 0+sizeof(struct exec);
	dstart = tstart + Ahdr.a_text;
	bstart = dstart + Ahdr.a_data;
	Lseek(aoutFD,bstart+Ahdr.a_bss,SEEK_SET);
printf("Offset at 0x%x...\n",bstart+Ahdr.a_bss);
	while(read(aoutFD,&c,1) == 1)
		printf("%c",c);
	printf("\n");
}


/* GetAoutFileHdr():
   Retrieve the header from the file and do a BE-to-LE conversion
   on each of its members.
*/
struct exec *
GetAoutFileHdr(afhdr)
struct exec *afhdr;
{
	if (verbose)
		fprintf(stderr,"GetAoutFileHdr()\n");

	/* Read in the aout header. */
	Lseek(aoutFD,0,SEEK_SET);
	Read(aoutFD,(char *)afhdr,(unsigned)sizeof(struct exec));
	afhdr->a_mid = otherEnd16(Ecvt,afhdr->a_mid);
	afhdr->a_magic = otherEnd16(Ecvt,afhdr->a_magic);
	afhdr->a_text = otherEnd32(Ecvt,afhdr->a_text);
	afhdr->a_data = otherEnd32(Ecvt,afhdr->a_data);
	afhdr->a_bss = otherEnd32(Ecvt,afhdr->a_bss);
	afhdr->a_syms = otherEnd32(Ecvt,afhdr->a_syms);
	afhdr->a_entry = otherEnd32(Ecvt,afhdr->a_entry);
	afhdr->a_trsize = otherEnd32(Ecvt,afhdr->a_trsize);
	afhdr->a_drsize = otherEnd32(Ecvt,afhdr->a_drsize);
	return(afhdr);
}

char *usage_txt[] = {
	"Usage: aout [options] {filename}",
	" Options:",
	" -a{filename}  append file to end of -S file",
	" -A            print what was appended by -a",
	" -B{filename}  aout-2-bin to filename",
	" -c            BE-to-LE convert",
	" -f            show aout file header",
	" -M            show aout map with file offsets",
	" -m            show aout map without file offsets",
	" -s            show aout section headers",
	" -S{filename}  strip to specified file",
	" -V            display version of tool",
	" -v            verbose (debug) mode",
	0,
};

int
main(int argc,char *argv[])
{
	char	fname[128], *append, *stripto, *binto;
	int	opt, showstuff;

	verbose = 0;
	showstuff = 0;
	stripto = (char *)0;
	append = (char *)0;
	binto = (char *)0;
	Ecvt = 1;
	while ((opt=getopt(argc,argv,"a:AcB:dfmMsS:vV")) != EOF) {
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
			Ecvt = 0;
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
		case 'S':
			stripto = optarg;
			break;
		case 's':
			showstuff |= SHOWSECTIONS;
			break;
		case 'v':
			verbose = 1;
			break;
		case 'V':
			showVersion();
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

	if ((aoutFD = open(fname,O_RDONLY | O_BINARY)) == -1) {
		perror(fname);
		exit(1);
	}

	aoutFname = StrAllocAndCopy(fname);
	GetAoutFileHdr(&Ahdr);

	if (binto) {
		AoutToBinary(binto);
	}
	else if (stripto) {
		StripAoutFile(stripto,append);
	}
	else {
		if (showstuff & SHOWAPPEND)
			ShowAppend();
		if (showstuff & SHOWHDR)
			ShowAoutHdr();
		if (showstuff & SHOWMAP)
			ShowAoutMap();
		if (showstuff & SHOWMAPOFFSET)
			ShowAoutOffsets();
	}
	close(aoutFD);
	return(0);
}
