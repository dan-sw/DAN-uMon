#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "version.h"
#include "utils.h"
#include "crc32.h"
#ifndef BUILD_WITH_VCC
#include <unistd.h>
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

int debug;

/* Run the same CRC on a file as is run in the "mt -C" command
 * in the monitor...
 */

int
main(int argc,char *argv[])
{
	int	opt;
	unsigned long crc;

	while((opt=getopt(argc,argv,"V")) != EOF) {
		switch(opt) {
			case 'V':
				showVersion();
				break;
			default:
				exit(1);
		}
	}
	if (argc != optind + 1)
		usage(0);

	if (fcrc32(argv[optind],&crc,1) < 0)
		usage(0);

	exit(0);
}

char *usage_txt[] = {
	"Usage: fcrc [options] {ifile}",
	" Run the same CRC as is used in 'mt -c'",
	" Options:",
	" -V   show version (build date) of tool",
	(char *)0,
};
