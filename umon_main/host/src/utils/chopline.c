/* chopline.c:
 * Quick and dirty means of setting a max line size of a text file
 * so that editors that get confused with large line sizes can still
 * read the file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef BUILD_WITH_VCC
#include <io.h>
extern	int optind;
extern	char *optarg;
#else
#include <unistd.h>
#endif

#include "utils.h"
#include "version.h"

int		debug;

char *usage_txt[] = {
	"Usage: chopline [options] {in_file}",
	" For each line in the specified file 'in_file' insert an extra",
	" carriage-return/line-feed into the string to shorten each line."
	" Output is sent to stdout.",
	"",
	" Options:",
	" -l ###     override default of 79 chars per line",
	" -V         display version of tool",
	0,
};

int
main(int argc, char *argv[])
{
    FILE *fp;
    char *line;
    int i, len, linelen, opt;

	linelen = 79;
	while((opt=getopt(argc,argv,"l:V")) != EOF) {
		switch(opt) {
		case 'l':
			linelen = strtol(optarg,0,0);
			break;
		case 'V':
			showVersion();
			break;
		default:
			usage(0);
		}
	}

	if (argc < optind+1)
		usage(0);

	line = malloc(linelen+1);
	if (!line) {
		perror("malloc");
		exit(1);
	}

    fp = fopen(argv[optind],"r");
    while(1) {
        if (fgets(line, linelen+1, fp) == NULL)
            break;
        len = strlen(line);
        for(i=0;i<len;i++)
            putchar(line[i]);
        if (len >= linelen)
            putchar('\n');
    }
	free(line);
    fclose(fp);
    exit(0);
}
