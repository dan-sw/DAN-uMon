#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#ifndef BUILD_WITH_VCC
#include <unistd.h>
#endif
#include "utils.h"
#include "version.h"

#ifndef O_BINARY
#define O_BINARY 0
#endif

int debug;
#define BLKSIZ	1024

/* converts a raw binary file to a C array */

int
main(int argc,char *argv[])
{
	FILE	*ofp;
	int		ofd, ifd, i, j, total, cnt, tenth, percent, width, opt, len;
	int		swap, begin, end, Size, AssemblerFormat, definetot, nullTerminate;
	char	arrayname[64], outputfilename[64], *filename;
	char	oline[128];
	unsigned char	ibuf[BLKSIZ+100];
	char	defines[16][80];
	struct	stat	mstat;
	
	width = 1;
	definetot = 0;
	arrayname[0] = 0;
	outputfilename[0] = 0;
	swap = 0;
	begin = end = 0;
	AssemblerFormat = 0;
	nullTerminate = 0;
	while((opt=getopt(argc,argv,"Aa:b:D:e:nw:o:sV")) != EOF) {
		switch(opt) {
		case 'a':
			strncpy(arrayname,optarg,sizeof arrayname -1);
			break;
		case 'A':
			AssemblerFormat = 1;
			break;
		case 'D':
			strcpy(defines[definetot++],optarg);
			break;
		case 'o':
			strncpy(outputfilename,optarg,sizeof outputfilename -1);
			break;
		case 'w':
			width = atoi(optarg);
			break;
		case 's':
			swap = 1;
			break;
		case 'b':
			begin = strtol(optarg,0,0);
			break;
		case 'e':
			end = strtol(optarg,0,0);
			break;
		case 'n':
			nullTerminate = 1;
			break;
		case 'V':
			showVersion();
			break;
		default:
			usage(0);
		}
	}

	if ((width != 1) && (width != 2) && (width != 4)) {
		fprintf(stderr,"Invalid width: %d (use 1,2, or 4).\n",width);
		usage(0);
	}
	if ((swap) && (width != 1)) {
		fprintf(stderr,"ERROR: Swap only supported for width = 1\n");
		exit(1);
	}
	if ((AssemblerFormat) && (width != 1)) {
		fprintf(stderr,
		     "ERROR: Assembler format only supported for width = 1\n");
		exit(1);
	}

	if (argc - optind + 1 == 2) 
		filename = argv[optind];
	else {
		usage("Bad argcnt");
	}

	if (!arrayname[0])
		strcpy(arrayname,argv[optind]);

	/* Open binary input file: */
	ifd = open(filename,O_RDONLY | O_BINARY);
	if (ifd == -1) {
		perror(filename);
		usage(0);
	}
	fstat(ifd,&mstat);
	fprintf(stderr,"Binary file size=%d\n",mstat.st_size);

	if (swap) {
		if ((mstat.st_size % width*(swap*2)) != 0)
			fprintf(stderr,"Warning: %d modulo %d != 0\n",
				mstat.st_size,width*(swap*2));
	}
	else {
		if ((mstat.st_size % width) != 0)
			fprintf(stderr,"Warning: %d modulo %d != 0\n",
				mstat.st_size,width);
	}

	if ((begin > end) || (end > mstat.st_size)) {
		usage("Bad -e or -b option");
	}

	if (!end)
		end = mstat.st_size;

	if (outputfilename[0]) {
		ofd = open(outputfilename,O_WRONLY | O_TRUNC | O_CREAT | O_BINARY,0666);
		if (ofd < 0) {
			perror(outputfilename);
			usage(0);
		}
	}
	else
		ofp = stdout;

	for(i=0;i<definetot;i++)  {
		len = sprintf(oline,"#define %s\n",defines[i]);
		write(ofd,oline,len);
	}

	if (definetot)
		write(ofd,"\n",1);

	if (AssemblerFormat) {
		write(ofd,".byte ",6);
	}
	else {
		switch(width) {
		case 1:
			len = sprintf(oline,"unsigned char %s[] = {\n    ",arrayname);
			break;
		case 2:
			len = sprintf(oline,"unsigned short %s[] = {\n    ",arrayname);
			break;
		case 4:
			len = sprintf(oline,"unsigned long %s[] = {\n    ",arrayname);
			break;
		}
		write(ofd,oline,len);
	}

	if (begin) {
		if (lseek(ifd,begin,SEEK_SET) == -1) {
			perror("lseek");
			return(-1);
		}
	}

	total = 0;
	Size = end-begin;
	tenth = Size/10;
	percent = 10;
	j = 1;
	while(1) {
		if ((total + BLKSIZ) > Size)
			cnt = Read(ifd,(char *)ibuf,Size-total);
		else
			cnt = Read(ifd,(char *)ibuf,BLKSIZ);
		if (cnt <= 0)
			break;
		if (width == 1) {
			if (swap) {
				for(i=0;i<cnt;i+=2,j+=2) {
					len = sprintf(oline,"0x%02x,",ibuf[i+1]);
					write(ofd,oline,len);
					len = sprintf(oline,"0x%02x",ibuf[i]);
					write(ofd,oline,len);
					if (j>12) {
						j = 0;
						if (AssemblerFormat) {
							write(ofd,"\n.byte ",7);
						}
						else {
							write(ofd,",\n    ",6);
						}
						write(ofd,oline,len);
					}
					else {
						write(ofd,",",1);
					}
				}
			}
			else {
				for(i=0;i<cnt;i++,j++) {
					len = sprintf(oline,"0x%02x",ibuf[i]);
					write(ofd,oline,len);
					if (j>12) {
						j = 0;
						if (AssemblerFormat)
							write(ofd,"\n.byte ",7);
						else
							write(ofd,",\n    ",6);
					}
					else {
						if ((!AssemblerFormat) ||
							(i+1 == cnt))
							len = sprintf(oline,",");
							write(ofd,",",1);
					}
				}
			}
		}
		else if (width == 2) {
			for(i=0;i<cnt;i+=2,j+=2) {
				len = sprintf(oline,"0x%02x%02x, ",ibuf[i],ibuf[i+1]);
				write(ofd,oline,len);
				if (j>12) {
					j = 0;
					write(ofd,"\n    ",5);
				}
			}
		}
		else if (width == 4) {
			for(i=0;i<cnt;i+=4,j+=4) {
				len = sprintf(oline,"0x%02x%02x%02x%02x, ",
					ibuf[i],ibuf[i+1],ibuf[i+2],ibuf[i+3]);
				write(ofd,oline,len);
				if (j>12) {
					j = 0;
					write(ofd,"\n    ",5);
				}
			}
		}
		total += cnt;
		if (total >= Size)
			break;
		if (total >= tenth) {
			tenth += Size/10;
			fprintf(stderr,"Completed %d%%\r",percent);
			fflush(stdout);
			percent += 10;
		}
	}

	if (nullTerminate) {
		switch(width) {
		case 1:
			write(ofd,"0x00",4);
			break;
		case 2:
			write(ofd,"0x0000",6);
			break;
		case 4:
			write(ofd,"0x00000000",8);
			break;
		}
	}

	if (!AssemblerFormat)
		write(ofd,"\n};",3);

	write(ofd,"\n",1);

	close(ifd);
	close(ofd);

	fprintf(stderr,"Completed 100%%\n");
	return(0);
}

char	*usage_txt[] = {
	"Usage: bin2array [options] {infile} [array-name]",
	"Options:",
	"       -A              use assembler .byte format",
	"       -a{arrayname}   (default = filename)",
	"       -b{offset}      offset into binary file to begin at",
	"       -D{define}      up to 16 '#define' statements inserted in output",
	"       -e{offset}      offset into binary file to end at",
	"       -n              null-terminate the created array",
	"       -o{filename}    output file (default = stdout)",
	"       -s              swap data",
	"       -w{width}       unit width 1|2|4 (default = 1)",
	"       -V              display version (build date) of tool",
	0,
};
