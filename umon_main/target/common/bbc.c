/* bbc.c:
 * Support some basic ability to to basic-block-coverage.
 * This is VERY basic; nevertheless, useful.  It simply organizes
 * the ability to determine if a block of code was executed or not
 * based on the filename and line number in the file.
 *
 * Here's the idea...
 * Two ways to use this:
 * 1. You have 1 file that you want to test for basic block coverage.
 *    In this case, the bbc() function below is called with the file
 *    argument NULLed out.  Assuming bbc was previously configured for
 *    the number of lines in the file (using "bbc ltot" command), then
 *    each time bbc() is called, a line-specific flag is set to indicate
 *    that the line was hit.
 * 2. Similar to 1, but now several files are involved.  You initialize
 *    bbc through "bbc ftot" "bbc file" commands.  Then the application
 *    calls bbc() with both file and line number specified.
 *
 * In either case, the "bbc dump" command will show the results.
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
#include "bbc.h"

#if INCLUDE_BBC
struct bbcinfo {
	char filename[64];
	int	linetot;
	char *linehit;
};

static int		bbcFtot, bbcLtot;
static char		*bbcLineHit;
static struct	bbcinfo *bbcData;

/* bbc():
 * This function is called by the code that has the BBC()
 * macro inserted for basic block coverage.
 */
void
bbc(char *file, int line)
{
	int	i;
	struct	bbcinfo *bbcp;

	if (file) {
		if (bbcData) {
			bbcp = bbcData;
			for(i=0;i<bbcFtot;i++,bbcp++) {
				if (!strcmp(file,bbcp->filename)) {
					bbcp->linehit[line] = 1;
					break;
				}
			}
		}
	}
	else {
		if (bbcLineHit)
			bbcLineHit[line] = 1;
	}
}

void
bbcDump(char *filename)
{
	int	line, i;
	struct	bbcinfo *bbcp;

	if (filename) {
		printf("BBC info for %s...\n",filename);
		bbcp = bbcData;
		for(i=0;i<bbcFtot;i++,bbcp++) {
			if (!strcmp(filename,bbcp->filename)) {
				for(line=0;line<bbcp->linetot;line++) {
					if (bbcp->linehit[line])
						printf("%5d:1 ",line);
				}
			}
		}
	}
	else {
		printf("BBC ltot = %d...\n",bbcLtot);
		for(line=0;line<bbcLtot;line++) {
			if (bbcLineHit[line])
				printf("%5d:1 ",line);
		}
	}
	printf("\n");
}

char *BbcHelp[] = {
	"Basic Block Coverage",
	"-[m:] {cmd} [cmd-specific args]",
#if INCLUDE_VERBOSEHELP
	" -m {addr}  use 'addr' instead of malloc",
	"",
	" cmds:",
	"  init",
	"  dump [fname]",
	"  ltot {linecount}",
	"  ftot {filecount}",
	"  file {fname} {linecount}",
#endif
	0,
};

int
BbcCmd(int argc,char *argv[])
{
	char	*fname, *cmd, *memory;
	struct	bbcinfo *bbcp;
	int		opt, lcnt, i, j, line;

	memory = (char *)0;
	while((opt=getopt(argc,argv,"m:")) != -1) {
		switch(opt) {
		case 'm':
			memory = (char *)strtoul(optarg,0,0);
			break;
		default:
			return(CMD_PARAM_ERROR);
		}
	}
	if (argc >= optind+1) {

		cmd = argv[optind];

		if (!strcmp(cmd,"init")) {
			bbcp = bbcData;

			if (bbcLineHit)
				free(bbcLineHit);
			for(i=0;i<bbcFtot;i++) {
				if (bbcp->filename[0] != 0) {
					free(bbcp->linehit);
					bbcp->linehit = 0;
				}
			}
			if (bbcData)
				free((char *)bbcData);
			bbcFtot = 0;
			bbcLtot = 0;
			bbcData = 0;
			bbcLineHit = 0;
		}
		else if (!strcmp(cmd,"clear")) {
			for(i=0;i<bbcLtot;i++)
				bbcLineHit[i] = 0;
			bbcp = bbcData;
			for(i=0;i<bbcFtot;i++,bbcp++) {
				for(j=0;j<bbcp->linetot;j++)
					bbcp->linehit[j] = 0;
			}
		}
		else if (!strcmp(cmd,"dump")) {
			if (argc == optind+1) 
				bbcDump(0);
			else if (argc == optind+2)
				bbcDump(argv[optind+1]);
			else
				return(CMD_PARAM_ERROR);
		}
		else if (!strcmp(cmd,"ftot")) {
			if (argc != optind+2)
				return(CMD_PARAM_ERROR);
			bbcFtot = atoi(argv[optind+1]);
			printf("ftot = %d\n",bbcFtot);
			if (!memory)
				memory = malloc(bbcFtot*sizeof(struct bbcinfo));
			bbcData = (struct bbcinfo *)memory;
			memset((char *)bbcData,0,bbcFtot*sizeof(struct bbcinfo));
		}
		else if (!strcmp(cmd,"ltot")) {
			if (argc != optind+2)
				return(CMD_PARAM_ERROR);
			bbcLtot = atoi(argv[optind+1]);
			printf("ltot = %d\n",bbcLtot);
			if (!memory)
				memory = malloc(bbcLtot);
			bbcLineHit = memory;
			memset(bbcLineHit,0,bbcLtot);
		}
		else if (!strcmp(cmd,"file")) {
			if (argc != optind+3)
				return(CMD_PARAM_ERROR);

			if (bbcFtot == 0) {
				printf("Must set ftot first\n");
				return(CMD_FAILURE);
			}
			fname = argv[optind+1];
			lcnt = atoi(argv[optind+2]);
			bbcp = bbcData;
			for(i=0;i<bbcFtot;i++) {
				if (bbcp->filename[0] == 0) {
					printf("File %d = %s, lcnt = %d\n",i,fname,lcnt);
					strcpy(bbcp->filename,fname);
					bbcp->linetot = lcnt;
					if (!memory)
						memory = malloc(lcnt);
					bbcp->linehit = memory;
					break;
				}
			}
		}
		else if (!strcmp(cmd,"call")) {
			if (argc != optind+3)
				return(CMD_PARAM_ERROR);
			fname = argv[optind+1];
			line = atoi(argv[optind+2]);
			printf("bbc(%s,%d)\n",fname,line);
			if (!strcmp(fname,"0")) 
				fname = 0;
			bbc(fname,line);
		}
		else
			return(CMD_PARAM_ERROR);
	}
	else
		return(CMD_PARAM_ERROR);

	return(CMD_SUCCESS);
}
#else
void
bbc(char *file, int line)
{
	return;
}
#endif
