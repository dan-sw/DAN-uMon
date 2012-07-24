/* docmd:
 *	This code supports the command line interface (CLI) portion of the
 *	monitor.  It is a table-driven CLI that uses the table in cmdtbl.c.
 *	A limited amount of "shell-like" capabilities are supported...
 *		shell variables, symbol-table lookup, command history,
 *		command line editiing, command output redirection, etc...
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
#include "genlib.h"
#include "cli.h"

extern	struct monCommand cmdlist[];

/* tokenize():
 *	Take the incoming string and create an argv[] array from that.  The
 *	incoming string is assumed to be writeable.  The argv[] array is simple
 *	a set of pointers into that string, where the whitespace delimited
 *	character sets are each NULL terminated.
 */
int
tokenize(char *string,char *argv[])
{
	int	argc, done;

	/* Null out the incoming argv array. */
	for(argc=0;argc<ARGCNT;argc++)
		argv[argc] = (char *)0;

	argc = 0;
	while(1) {
		while ((*string == ' ') || (*string == '\t'))
			string++;
		if (*string == 0)
			break;
		argv[argc] = string;
		while ((*string != ' ') && (*string != '\t')) {
			if ((*string == '\\') && (*(string+1) == '"')) {
				strcpy(string,string+1);
			}
			else if (*string == '"') {
				strcpy(string,string+1);
				while(*string != '"') {
					if ((*string == '\\') && (*(string+1) == '"'))
						strcpy(string,string+1);
					if (*string == 0)
						return(-1);
					string++;
				}
				strcpy(string,string+1);
				continue;
			}
			if (*string == 0)
				break;
			string++;
		}
		if (*string == 0)
			done = 1;
		else {
			done = 0;
			*string++ = 0;
		}
		argc++;
		if (done)
			break;
		if (argc >= ARGCNT) {
			argc = -1;
			break;
		}
	}
	return(argc);
}

#if INCLUDE_HELP
/* Help():
 */
char *HelpHelp[] = {
	"Dump command info",
	"[cmdname]",
	0,
};

int
Help(int argc,char *argv[])
{
	int		i;
	struct	monCommand *cptr;

#if !INCLUDE_HWTMR
	if (((argc == 2) || (argc == 3)) && !strcmp(argv[1],"ttest")) {
		extern void tickTest(long);
		long val;

		if (argc == 2)
			val = 0;
		else
			val = atoi(argv[2]);
		tickTest(val);
		return(CMD_SUCCESS);
	}
#endif
	cptr = cmdlist;
	i = 0;
	while(cptr->name) {
		if (argc == 1) {
			printf("%-12s",cptr->name);
			if ((++i%6) == 0)
				putchar('\n');
		}
		else {
			if (strcmp(cptr->name,argv[optind]) == 0) {
				char **hp;
	
				printf("%s\n", cptr->helptxt[0]);
				printf("Usage: %s %s\n",
				    cptr->name,cptr->helptxt[1]);
				hp = &cptr->helptxt[2];
				while(*hp) 
					printf("%s\n",*hp++);
				break;
			}
		}
		cptr++; 
	}
	putchar('\n');
	return(CMD_SUCCESS);
}
#endif

/*	_docommand():
 *	Assume the incoming string is a null terminated character string
 *	that is made up of whitespace delimited tokens that will be parsed
 *	into command line arguments.  The first argument is the command name
 *	and all following arguments (if any) are specific to that command.
 * If verbose is non-zero, print the list of arguments after tokenization.
 */
static int
_docommand(char *cmdline)
{
	int	ret, argc;
	struct	monCommand	*cmdptr;
	char	*argv[ARGCNT];

	/* Build argc/argv structure based on incoming command line.
	 */
	argc = tokenize(cmdline,argv);
	if (argc == 0)
		return(CMD_LINE_ERROR);

	if (argc < 0) {
		printf("Cmd err\n");
		return(CMD_LINE_ERROR);
	}

	getoptinit();

	/* Search through command table looking for a match with
	 * argv[0].  If a match is found, execute that command 
	 * function, and return...
	 */
	for(cmdptr = cmdlist; cmdptr->name; cmdptr++) {
		if (strcmp(argv[0],cmdptr->name) == 0) {
			ret = cmdptr->func(argc,argv);
	
			if (ret == CMD_PARAM_ERROR)
				printf("cmd param err\n");
			
			return(ret);
		}
	}

	/* If we get here, then the first token does not match on any of
	 * the command names in ether of the command tables.  As a last
	 * resort, we look to see if the first token matches an executable
	 * file name in TFS.
	 */
	ret = CMD_NOT_FOUND;

	printf("%s: not found\n",argv[0]);
	return(ret);
}

int
docommand(char *line)
{
	int	verbose, ret, loop, len;
	char *lp, *semi, cmdcpy[CMDLINESIZE];

	verbose = loop = 0;
	len = strlen(line);
	lp = line + len - 1;
	if (*lp == '<')  {
		loop = 1;
		*lp-- = 0;
	}

repeat:
	lp = cmdcpy;
	strcpy(lp,line);
	do {
		semi = strchr(lp,';');
		if (semi)
			*semi = 0;
		ret = _docommand(lp);
		if (semi) {
			*semi = ';';
			lp = semi+1;
		}
	} while (semi);

	if ((loop) && !gotachar()) 
		goto repeat;

	return(ret);
}
