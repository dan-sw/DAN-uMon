/* 
 * main():
 * Startup for an embedded app running with uMon.
 * The purpose of main() is to simply parse the command line, then based
 * on the parsing, jump to an appropriate function that is specific
 * to this uMon application.
 */

#include <string.h>
#include <stdlib.h>
#include "monlib.h"
#include "tfs.h"
#include "cfg.h"
#include "cfi.h"

#define printf mon_printf

int		verbose;
int		optind = 1;
char	*optarg;
int		sp = 1;

unsigned long AppStack[APPSTACKSIZE/4];

void
usage(char *msg)
{
	if (msg)
		printf("%s\n");
	printf("Usage: cfi [options] {base-address-of-flash}\n");
	printf(" Options:\n");
	printf("  -v   enable verbosity\n");
	mon_appexit(1);
}


/* getopt() is pulled in as local code (instead of just getting it from
 * libc) because it has printf in it.  If getopt is pulled in, then it
 * will pull in the library's printf; hence, the support for stdio would
 * have to be added.  That could be done; however, pulling in getopt() here
 * is just easier.
 */
int
getopt(int argc,char *argv[],char *opts)
{
	register int c;
	register char *cp;

	if(sp == 1) {
		if (optind >= argc)
			return(-1);
		else if (argv[optind][0] != '-')
			return(-1);
		else if (argv[optind][1] == '\0')
			return(-1);
		else if(strcmp(argv[optind], "--") == 0) {
			optind++;
			return(-1);
		}
	}
	c = argv[optind][sp];
	if(c == ':' || (cp=strchr(opts, c)) == 0) {
		printf("Illegal option: %c\n",c);
		if(argv[optind][++sp] == '\0') {
			optind++;
			sp = 1;
		}
		return('?');
	}
	if(*++cp == ':') {
		if(argv[optind][sp+1] != '\0')
			optarg = &argv[optind++][sp+1];
		else if(++optind >= argc) {
			printf("Option requires argument: %c\n",c);
			sp = 1;
			return('?');
		} else
			optarg = argv[optind++];
		sp = 1;
	} else {
		if(argv[optind][++sp] == '\0') {
			sp = 1;
			optind++;
		}
		optarg = 0;
	}
	return(c);
}

int
main(int argc,char *argv[])
{
	int opt;
	unsigned long base;

	verbose = 0;
	while((opt=getopt(argc,argv,"v")) != -1) {
		switch(opt) {
		case 'v':
			verbose = 1;
			break;
		default:
			return(1);
		}
	}

	if (argc != optind + 1) 
		usage("Invalid arg cnt");
	
	base = strtoul(argv[optind],0,0);

	getCFIMode(base,verbose);
//	getCFIGeometry(base,verbose);
	return(0);
}


void
__gccmain()
{
}

int
Cstart(void)
{
	char	**argv;
	int		argc;

	/* Connect the application to the monitor.  This must be done
	 * prior to the application making any other attempts to use the
	 * "mon_" functions provided by the monitor.
	 */
	monConnect((int(*)())(*(unsigned long *)MONCOMPTR),(void *)0,(void *)0);

	/* When the monitor starts up an application, it stores the argument
	 * list internally.  The call to mon_getargv() retrieves the arg list
	 * for use by this application...
	 */
	mon_getargv(&argc,&argv);

	/* Call main, then exit with the return from main().
	 */
	mon_appexit(main(argc,argv));

	/* Won't get here. */
	return(0);
}
