/* bswap.c:
 * Just take a file and swap the bytes (either by 2 or by 4).
 */
#include <stdio.h>
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

/* Takes a file and swaps the data */

int
main(int argc,char *argv[])
{
	struct	stat stat;
	char	*ifile, *ofile, fname[128], buf[16], sbuf[16];
	int		opt, ofd, ifd, swapsize, size;
	
	ifile = ofile = 0;
	while((opt=getopt(argc,argv,"o:V")) != EOF) {
		switch(opt) {
		case 'V':
			showVersion();
			break;
		case 'o':
			ofile = optarg;
			break;
		default:
			usage(0);
		}
	}

	if (argc != optind+2) {
		fprintf(stderr,"argc=%d, optind=%d\n",argc,optind);
		usage(0);
	}

	swapsize = atoi(argv[optind]);
	ifile = argv[optind+1];

	if (!ofile) {
		sprintf(fname,"%s.swap%d",ifile,swapsize);
		ofile = fname;
	}

	fprintf(stderr,"Infile: %s\n",ifile);
	fprintf(stderr,"Outfile: %s\n",ofile);
	fprintf(stderr,"Swapsize: %d\n",swapsize);

	ifd = open(ifile,O_RDONLY | O_BINARY);
	if (ifd < 0) {
		fprintf(stderr,"can't open file %s\n",ifile);
		exit(1);
	}

	ofd = open(ofile,O_WRONLY | O_TRUNC | O_CREAT | O_BINARY,0666);
	if (ofd < 0) {
		fprintf(stderr,"can't open file %s\n",ofile);
		exit(1);
	}

	fstat(ifd,&stat);
	if ((stat.st_size % swapsize) != 0) {
		fprintf(stderr,"Warning: %s is %d bytes (not modulo %d)\n",
			ifile,stat.st_size,swapsize);
	}

	while((size = read(ifd,buf,swapsize)) == swapsize) {
		if (swapsize == 2) {
			sbuf[0] = buf[1];
			sbuf[1] = buf[0];
		}
		else {
			sbuf[0] = buf[3];
			sbuf[1] = buf[2];
			sbuf[2] = buf[1];
			sbuf[3] = buf[0];
		}
		write(ofd,sbuf,swapsize);
	}
	if (size) 
		write(ofd,buf,size);
	
	close(ifd);
	close(ofd);
	return(0);
}

char	*usage_txt[] = {
	"Usage: bswap [options] {2|4} {infile}",
	"Options:",
	"   -o {fname} specify output filename",
	"   -V         display version (build date) of tool",
	0,
};
