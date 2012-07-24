#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "utils.h"
#include "version.h"
#ifndef BUILD_WITH_VCC
#include <unistd.h>
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

int debug;

/* converts a file to a Motorola S3 record */

int
main(int argc,char *argv[])
{
	int	ifd, i, total, cnt, csum, opt;
	unsigned long	address, offset, base;
	unsigned char	ibuf[100], *cp;
	struct	stat	mstat;

	offset = 0;
	debug = base = 0;
	while((opt=getopt(argc,argv,"b:do:V")) != EOF) {
		switch(opt) {
		case 'b':
			base = strtoul(optarg,0,0);
			break;
		case 'd':
			debug = 1;
			break;
		case 'o':
			offset = strtoul(optarg,0,0);
			break;
		case 'V':
			showVersion();
			break;
		default:
			exit(0);
		}
	}
	optind--;
	if (argc-optind != 2)
		usage(0);

	/* Open input file: */
	ifd = open(argv[1+optind],O_RDONLY | O_BINARY);
	if (ifd == -1) {
		perror(argv[1+optind]);
		usage(0);
	}
	fstat(ifd,&mstat);
	fprintf(stderr,"Input file size=%d\n",mstat.st_size);

	if (offset) {
		if (offset < 0) {
			fprintf(stderr,"Bad offset\n");
			exit(1);
		}
		mstat.st_size -= offset;
		if (lseek(ifd,offset,SEEK_SET) == -1) {
			perror("lseek");
			exit(1);
		}
	}

	/* Create Srecord output file: */
	printf("S0030000FC\n");
	address = base;
	total = 0;

	while(1) {
		cnt = read(ifd,ibuf,32);

		/* Print S3 idendifier with byte count for line plus
		   4-byte address: */
		printf("S3%02X%08X",cnt+5,address);

		/* Accumlate checksum subtotal: */
		csum = 0;
		cp = (unsigned char *)&address;
		csum += (cnt+5);
		csum += cp[0]; csum += cp[1]; csum += cp[2]; csum += cp[3];

		/* Print each byte of data: */
		for(i=0;i<cnt;i++) {
			printf("%02X",ibuf[i]);
			csum += (int)ibuf[i];
		}

		/* Print sumcheck (includes bytecount, address & data): */
		/* (binary one's compliment) */
		printf("%02X\n",255 - (csum & 0xff));
		total += cnt;
		if (total >= mstat.st_size)
			break;
		address += cnt;
	}
	printf("S705000019E001\n");
	close(ifd);
	return(0);
}

char *usage_txt[] = {
	"Usage: bin2srec [options] {ifile}",
	" Options:",
	" -o   {offset into binary file}",
	" -b   {base address}",
	" -V   show version (build date) of tool",
	(char *)0,
};
