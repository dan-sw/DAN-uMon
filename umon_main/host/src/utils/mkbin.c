/* mkbin.c:
 *	This tool is used as part of a test for the TFS defragger.
 *	It builds a file of a specified size, and optionally (-t option) 
 *  tests a file that already exists to verify that its content is 
 *	what would have been created.
 *	This allows a file of any size to be created, downloaded to TFS,
 *	put through defragmentation, uploaded and then verified.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef BUILD_WITH_VCC
#else
#include <unistd.h>
#endif
#include "version.h"
#include "utils.h"

#ifndef O_BINARY
#define O_BINARY 0
#endif

int	debug, optind;

char *usage_txt[] = {
	"Usage: mkbin [options] {filename} {size}",
	" Options:",
	" -d            debug mode",
	" -t            test file",
	" -V            display version of tool",
	0
};
/* Data array used to create the file...
 * All of the data is readable just to make things easier to debug.
 */
char *data = "012345678901234567890123456789\nabcdefghijklmnopqrstuvwxyz\nABCDEFGHIJKLMNOPQRSTUVWXYZ\n!@#$%^&*()_+!@#$%^&*()_+!@#$%^&*()_+\n";


int
main(int argc,char *argv[])
{
	char	*fname;
	long	opt, fd, tot, dlen, size, test;

	test = 0;
	while ((opt=getopt(argc,argv,"dtV")) != EOF) {
		switch(opt) {
		case 'd':
			debug = 1;
			break;
		case 't':
			test = 1;
			break;
		case 'V':
			showVersion();
			break;
		default:
			usage(0);
		}
	}

	if (argc != optind+2)
		usage(0);

	dlen = strlen(data);
	fname = argv[optind];
	size = strtol(argv[optind+1],0,0);

	if (debug) {
		fprintf(stderr,"dlen=%d, fname=%s, size=%d\n",dlen,fname,size);
	}
	
	if (test) {
		int		pkt;
		char	*buf, *bp;
		struct	stat fstat;

		if (stat(fname,&fstat) < 0) {
			fprintf(stderr,"mkbin: File '%s' doesn't exist.\n",fname);
			exit(1);
		}
		if (fstat.st_size != size) {
			fprintf(stderr,"mkbin: Invalid file size (%d != %d)\n",
				fstat.st_size,size);
			exit(1);
		}
		buf = Malloc(size);	
		if ((fd = open(fname,O_RDONLY | O_BINARY))==-1) {
			fprintf(stderr,"mkbin: Can't open file '%s'\n",fname);
			exit(1);
		}
		Read(fd,buf,size);
		close(fd);
		tot = 0;
		bp = buf;
		pkt = 0;
		while(1) {
			if (tot < (size-dlen)) {
				if (memcmp(bp,data,dlen) != 0) {
					fprintf(stderr,"mkbin: Mismatch in packet %d\n",pkt);
					fprintf(stderr,
						"mkbin: Packet %d starts at offset %d (0x%x)\n",
						pkt,tot,tot);
					exit(1);
				}
				tot += dlen;
				bp += dlen;
				pkt++;
			}
			else {
				if (memcmp(bp,data,size-tot) != 0) {
					fprintf(stderr,"mkbin: Mismatch in last packet (%d)\n",pkt);
					fprintf(stderr,
						"mkbin: Packet %d starts at offset %d (0x%x)\n",
						pkt,tot,tot);
					exit(1);
				}
				break;
			}
		}
		free(buf);
		fprintf(stderr,"mkbin: %s test passed\n",fname);
	}
	else {
		if ((fd = open(fname,O_WRONLY|O_BINARY|O_CREAT|O_TRUNC,0777))==-1) {
			fprintf(stderr,"mkbin: Can't open file %s\n",fname);
			exit(1);
		}

		tot = 0;
		while(1) {
			if (tot < (size-dlen)) {
				write(fd,data,dlen);
				tot += dlen;
			}
			else {
				write(fd,data,size-tot);
				break;
			}
		}
		close(fd);
	}
	exit(0);
}
