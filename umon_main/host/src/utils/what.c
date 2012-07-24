#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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

#define WHATSEARCH	1
#define WHATMATCH	2

/* Just what is "what"?
 * This is a tool that simply prints all strings found that start with
 * "@(#)" in an input file.  This is called a "what string".  It is useful
 * for storage of retrievable information from a binary file.
 */

/* converts a file to a Motorola S3 record */

int
main(int argc,char **argv)
{
	int		ifd, opt, state;
	char	*ifile, *buf, *end, *bp, *whatstring, *wsp;
	struct	stat	mstat;

	debug = 0;
	whatstring = "@(#)";
	while((opt=getopt(argc,argv,"ds:V")) != EOF) {
		switch(opt) {
		case 's':
			whatstring = optarg;
			break;
		case 'd':
			debug = 1;
			break;
		case 'V':
			showVersion();
			break;
		default:
			exit(0);
		}
	}
	if (argc-optind != 1)
		usage(0);
	ifile = argv[optind];

	/* Open input file: */
	ifd = open(ifile,O_RDONLY | O_BINARY);
	if (ifd == -1) {
		perror(ifile);
		usage(0);
	}
	fstat(ifd,&mstat);
	if (debug)
		fprintf(stderr,"fsize=%d\n",mstat.st_size);
	bp = buf = malloc(mstat.st_size);
	if (!buf) {
		perror("malloc failed");
		exit(1);
	}
	if (read(ifd,buf,mstat.st_size) != mstat.st_size) {
		perror(ifile);
		exit(1);
	}
	close(ifd);
	end = buf + mstat.st_size;

	printf("%s:\n",ifile);

	state = WHATSEARCH;
	wsp = whatstring;
	while(bp < end) {
		switch(state) {
		case WHATSEARCH:
			if (*bp == *wsp) {
				wsp++;
				if (*wsp == 0) {
					wsp = whatstring;
					state = WHATMATCH;
					putchar('\t');
				}
			}
			else
				wsp = whatstring;
			break;
		case WHATMATCH:
			if (isprint(*bp)) {
				putchar(*bp);
			}
			else {
				putchar('\n');
				state = WHATSEARCH;
			}
			break;
		}
		bp++;
	}

	free(buf);
	return(0);
}


char *usage_txt[] = {
	"Usage: what [options] {ifile}",
	"Display any string starting with '@(#)' found in the input file.",
	" Options:",
	" -d        debug mode (for development)",
	" -s{sss}   use 'sss' in place of default '@(#)' string",
	" -V        display version of 'what'",
	(char *)0,
};
