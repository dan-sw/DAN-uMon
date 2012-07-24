/* alias.c:
 *
 *	NOT READY YET
 *
 *	General notice:
 *	This code is part of a boot-monitor package developed as a generic base
 *	platform for embedded system designs.  As such, it is likely to be
 *	distributed to various projects beyond the control of the original
 *	author.  Please notify the author of any enhancements made or bugs found
 *	so that all may benefit from the changes.  In addition, notification back
 *	to the author will allow the new user to pick up changes that may have
 *	been made by other users after this version of the code was distributed.
 *
 *	Note1: the majority of this code was edited with 4-space tabs.
 *	Note2: as more and more contributions are accepted, the term "author"
 *		   is becoming a mis-representation of credit.
 *
 *	Original author:	Ed Sutter
 *	Email:				esutter@lucent.com
 *	Phone:				908-582-2351
 */
#include "config.h"
#if INCLUDE_ALIAS
#include "genlib.h"
#include "cli.h"

#define ALIAS_TOT	16

struct alias {
	char *name;
	char *value;
};

struct alias aliasList[ALIAS_TOT];

void
aliasInit(void)
{
	int	i;

	for(i=0;i<ALIAS_TOT;i++) {
		aliasList[i].name = 0;
		aliasList[i].value = 0;
	}
}

char *AliasHelp[] = {
		"Establish or list command alias'",
		"[name=value]",
		0,
};

int
Alias(int argc,char *argv[])
{
	static	int pass;
	struct	alias	*ap;
	int	debug, i;

	if (pass == 0) {
		alias_init();
	}
	debug = 0;
	while((opt=getopt(argc,argv,"d")) != -1) {
		switch(opt) {
		case 'd':
			debug = 1;
			break;
		default:
			return(CMD_PARAM_ERROR);
		}
	}
	if (argc == optind) {
		ap = aliasList;
		for(i=0;i<ALIAS_TOT;i++) {
			if (ap->name) {
				printf("%16s = ", ap->name);
				puts(ap->value);	
				putchar('\n');
			}
			ap++;
		}
	}
	return(CMD_SUCCESS);
}
#endif
