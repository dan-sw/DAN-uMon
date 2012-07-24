/** \file getopt.c:
 *	\brief Retrieve options from the command line
 */	
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/** \var int getopt_sp
 *	\brief Initialized variable used by getopt.
 */
int	getopt_sp = 1;

/** \var int optopt
 *	\brief Global variable used by getopt.
 */
int	optopt;

/** \var int optind
 *	\brief Global variable used by callers to getopt.  It contains
 *	\brief the index into the argument list that is next.
 */
int	optind = 1;

/** \var char *optarg
 *	\brief Pointer to an argument that has an option.
 */
char	*optarg;


/**	\fn int getopt(int argc,char *argv[],char *opts)
 *	\brief Standard mechanism used to retrieve options (dash-prefixed
 *	\brief letters that provide the program with "optional" 
 *	\brief parameters.
 */
int
getopt(int argc,char *argv[],char *opts)
{
	extern char	*strchr();
	register int c;
	register char *cp;

	if (getopt_sp == 1) {
		char	c0, c1;

		if (optind >= argc)
			return(EOF);

		c0 = argv[optind][0];
		c1 = argv[optind][1];

		if (c0 != '-')
			return(EOF);
		if (c1 == '\0')
			return(EOF);
		if (c0 == '-') {
			if ((isdigit(c1)) && (!strchr(opts,c1)))
				return(EOF);
			if (c1 == '-') {
				optind++;
				return(EOF);
			}
		}
	}
	optopt = c = argv[optind][getopt_sp];
	if (c == ':' || ((cp=strchr(opts, c)) == NULL)) {
		fprintf(stderr,"Illegal '%s' option: %c.\n",argv[0],c);
		if(argv[optind][++(getopt_sp)] == '\0') {
			optind++;
			getopt_sp = 1;
		}
		return('?');
	}
	if (*++cp == ':') {
		if(argv[optind][getopt_sp+1] != '\0')
			optarg = &argv[optind++][getopt_sp+1];
		else if(++(optind) >= argc) {
			fprintf(stderr,
				"Option '%c' of '%s' requires argument.\n",
				c,argv[0]);
			getopt_sp = 1;
			return('?');
		} else
			optarg = argv[optind++];
		getopt_sp = 1;
	} else {
		if(argv[optind][++(getopt_sp)] == '\0') {
			getopt_sp = 1;
			optind++;
		}
		optarg = NULL;
	}
	return(c);
}

