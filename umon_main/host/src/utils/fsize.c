#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "utils.h"
#include "version.h"
#include "crc32.h"
#ifndef BUILD_WITH_VCC
#include <unistd.h>
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

int debug;

/* Dump the size of the specified file to stdout..
 */

int
main(int argc,char *argv[])
{
	int	hex, noerror, opt, size;
	struct	stat	mstat;

	hex = noerror = 0;
	while((opt=getopt(argc,argv,"nVx")) != EOF) {
		switch(opt) {
			case 'n':
				noerror = 1;
				break;
			case 'V':
				showVersion();
				break;
			case 'x':
				hex = 1;
				break;
			default:
				exit(1);
		}
	}
	if (argc != optind + 1)
		usage(0);

	if (stat(argv[optind],&mstat) < 0) {
		if (noerror == 0)
			usage(0);
		else
			size = -1;
	}
	else
		size = mstat.st_size;

	if (hex)
		printf("0x%x\n",size);
	else
		printf("%d\n",size);
	exit(0);
}

char *usage_txt[] = {
	"Usage: fsize [-nxV] {filename}",
	" Print the size of the specified file.",
	" Exit 0 if successful (file is found),",
	"   else non-zero to indicate failure (unless -n is used).",
	"",
	" Options:",
	" -n   noerror mode, if file doesn't exist print -1 and exit 0",
	" -x   show size in hex (default is decimal)",
	" -V   show version (build date) of tool",
	(char *)0,
};
