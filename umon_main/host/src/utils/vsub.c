/* vsub:
 * Variable substitution.
 * Simply parse an ASCII file and substitute each instance of VARNAME
 * with VALUE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#ifdef BUILD_WITH_VCC
#else
#include <unistd.h>
#endif
#include "utils.h"
#include "version.h"

#define	LINESIZE	256
#define	MAXPAIRS	32

int debug = 0;
int verbose = 0;

struct nameval {
	char *name;
	char *val;
	int nlen;
	int vlen;
};

struct nameval nvtbl[MAXPAIRS];

char *usage_txt[] = {
	"Usage: vsub [options] {ifile} {ofile} [VARNAME=VALUE] ...",
	" Parse a file and substitute all instances of VARNAME with VALUE.",
	"",
	" Options:",
	" -V               display version of tool",
	" -v               verbose mode",
	0,
};

int
main(int argc,char *argv[])
{
	FILE *ifp, *ofp;
	int	opt, i, pairtot, vsubtot, lno;
	char iline[LINESIZE], *eq, *cp;

	while((opt=getopt(argc,argv,"vV")) != EOF) {
		switch(opt) {
		case 'V':
			showVersion();
			break;
		case 'v':  
			verbose++;
			break;
		default:
			usage(0);
		}
	}

	if (argc < optind+2)
		usage(0);

	/* Open the files: */
	if ((ifp = fopen(argv[optind],"r")) == (FILE *)NULL) {
		perror(argv[optind]);
		usage(0);
	}
	if ((ofp = fopen(argv[optind+1],"w")) == (FILE *)NULL) {
		perror(argv[optind+1]);
		usage(0);
	}
	
	/* Build a list of varname/value pairs from the command line...
	 */
	pairtot = 0;
	for (i=optind+2;i<argc;i++) {
		eq = strchr(argv[i],'=');
		if (!eq) {
			fprintf(stderr,"%s: Invalid VARNAME=VALUE syntax\n",argv[i]);
			exit(1);
		}
		*eq = 0;
		nvtbl[pairtot].name = argv[i];
		nvtbl[pairtot].nlen = strlen(argv[i]);
		nvtbl[pairtot].val = eq+1;
		nvtbl[pairtot].vlen = strlen(eq+1);
		pairtot++;
		if (pairtot >= MAXPAIRS) {
			fprintf(stderr,"Pair overflow\n");
			exit(1);
		}
	}

	vsubtot = 0;
	lno = 0;
	while(1) {
		if (fgets(iline,LINESIZE,ifp) == NULL)
			break;
		lno++;
		cp = iline;
		while(*cp) {
			for(i=0;i<pairtot;i++) {
				if (strncmp(cp,nvtbl[i].name,nvtbl[i].nlen) == 0) {
					if (verbose) {
						fprintf(stderr,"Line %d: substituted '%s' for '%s'\n",
							lno,nvtbl[i].name,nvtbl[i].val);
					}
					vsubtot++;
					fprintf(ofp,"%s",nvtbl[i].val);
					cp += nvtbl[i].nlen;
					break;
				}
			}
			if (i == pairtot) {
				fprintf(ofp,"%c",*cp);
				cp++;
			}
		}
	}
	fclose(ifp);
	fclose(ofp);
	if (verbose) 
		fprintf(stderr,"%d substitutions\n",vsubtot);
	return(0);
}

