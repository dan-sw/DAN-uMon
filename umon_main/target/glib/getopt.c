#include "config.h"
#include "ctype.h"
#include "genlib.h"
#include "stddefs.h"

/* Variables used by getopt: */
int	optind;			/* argv index to first cmd line arg that is not part
					 * of the option list.
					 */
char	*optarg;	/* pointer to argument associated with an option */
int	sp;

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

/* getoptinit():
 * Since getopt() can be used by every command in the monitor
 * it's variables must be reinitialized prior to each command
 * executed through docommand()...
 */
void
getoptinit(void)
{
	sp = 1;
	optind = 1;
}
