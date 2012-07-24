/* make2flist.c:
 * This is a simple but potentially VERY useful tool for helping out with
 * the tedious task of debugging a large source-code base from which only
 * a small portion of the files are actually part of the build (hence, just
 * using 'find' on the directory is worthless)..
 * This tool attempts to extract the file/directory information for each
 * compiled file from the output of 'make'.  It looks for the "Entering"
 * string and assumes this is an indication of where 'make' is at when it
 * compiles the files after that point.  As a result, the directory name
 * extracted from the "Entering" string is used as a prefix to the filenames
 * that are extracted from the following compilation lines.
 *
 * This (hopefully) produces a list of source files that were actually 
 * compiled by make to build the program; hence, a list useable by cscope
 * and/or ctags.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef BUILD_WITH_VCC
#else
#include <unistd.h>
#endif
#include "version.h"
#include "utils.h"

int debug;

char *usage_txt[] = {
	"Usage: make2flist [options] {compile_cmd} {filename}",
	"  Options:",
	"    -h    include header files",
	"    -V    show version",
	"    -v    verbose",
	"",
	(char *)0,
};

static char line[1024*8], dir[1024*8];

int
main(int argc, char *argv[])
{
	FILE *fp;
	int opt, verbose, headers;
	char *compile, *file, *openquote, *closequote;

	file = 0;
	dir[0] = 0;
	compile = 0;
	verbose = headers = 0;

	while((opt=getopt(argc,argv,"hvV")) != EOF) {
		switch(opt) {
		case 'h':
			headers = 1;
			break;
		case 'V':
			showVersion();
			break;
		case 'v':
			verbose = 1;
			break;
		default:
			usage(0);
		}
	}
	if (argc == optind+2) {
		compile = argv[optind];
		file = argv[optind+1];
	}
	else {
		usage(0);
	}

	if (verbose) {
		fprintf(stderr, "%s: opening %s\n",argv[0],file);
		fprintf(stderr, "%s: scanning for %s\n",argv[0],compile);
	}

	fp = fopen(file,"r");
	if (fp == (FILE *)NULL) {
		fprintf(stderr,"can't open %s\n",file);
		usage(0);
	}

	while(fgets(line,sizeof(line),fp)) {
		if (strstr(line,"Leaving"))
			dir[0] = 0;

		if (strstr(line,"Entering")) {
			openquote = strchr(line,'`');
			closequote = strchr(line,'\'');
			*closequote = 0;
			strcpy(dir,openquote+1);
			if (headers) {
				char cmd[512];

				fflush(stdout);
				sprintf(cmd,"ls %s/*.h 2> /dev/null",dir);
				system(cmd);
			}
		}
		if (strstr(line,compile)) {
			char *name;

			name = strrchr(line,' ') + 1;
			if ((name[0] == '/') || (name[1] == ':'))
				printf("%s",name);
			else if (dir[0])
				printf("%s/%s",dir,name);
			else
				printf("%s",name);
		}
	}
	fclose(fp);
	return(0);
}
