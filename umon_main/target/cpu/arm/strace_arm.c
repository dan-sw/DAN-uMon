/* strace.c:
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
 *
 *	Note that this stack trace can be used to trace REAL exceptions or
 *	user-induced exceptions.  If one is user-induced, then the user is
 *	typically just replacing an instruction at the point at which the break
 *	is to occur with a SC (syscall) instruction.  The point at which this
 *	insertion is made must be after the function sets up its stack frame
 *	otherwise it is likely that the trace will be bogus.
 *
 *	BTW... the SC instruction is 0x44000002.
 */
#include "config.h"
#if INCLUDE_STRACE
#include "tfs.h"
#include "tfsprivate.h"
#include "ctype.h"
#include "genlib.h"
#include "stddefs.h"

char *StraceHelp[] = {
    "Stack trace",
    "-[d:rs:v]",
    " -d #      max depth count (def=20)",
    " -r        dump regs",
    " -s {func} stop at function 'func'",
    " -v        verbose",
    0,
};

int
Strace(int argc,char *argv[])
{
	TFILE	*tfp;
	char	*symfile, *stopat, fname[64];
	ulong	*framepointer, pc, fp, offset;
    int		tfd, opt, maxdepth, pass, verbose;

	tfd = fp = 0;
	maxdepth = 20;
	verbose = 0;
	stopat = 0;
	pc = ExceptionAddr;
    while ((opt=getopt(argc,argv,"d:rs:v")) != -1) {
		switch(opt) {
		case 'd':
			maxdepth = atoi(optarg);
			break;
		case 'r':
			showregs();
			break;
		case 's':
			stopat = optarg;
			break;
		case 'v':
			verbose = 1;
			break;
	    default:
			return(0);
		}
	}
	
	if (!fp)
		getreg("R11", &fp);

	framepointer = (ulong *)fp;

	/* Start by detecting the presence of a symbol table file... */
	symfile = getenv("SYMFILE");
	if (!symfile)
		symfile = SYMFILE;

	tfp = tfsstat(symfile);
	if (tfp)  {
		tfd = tfsopen(symfile,TFS_RDONLY,0);
		if (tfd < 0)
			tfp = (TFILE *)0;
	}

	/* Show current position: */
	printf("   0x%08lx",pc);
	if (tfp) {
		AddrToSym(tfd,pc,fname,&offset);
		printf(": %s()",fname);
		if (offset)
			printf(" + 0x%lx",offset);
	}
	putchar('\n');

	/* Now step through the stack frame... */
	pass = 0;
	while(maxdepth) {
		if (pass != 0) 
			framepointer = (ulong *)*(framepointer - 3);
		
		pc = *(framepointer - 1);

		if (verbose) {
			printf("fp=0x%lx,*fp=0x%lx,pc=%lx\n", (ulong)framepointer,
				(ulong)*framepointer,pc);
		}

		if (((ulong)framepointer & 3) || (!framepointer) ||
			(!*framepointer) || (!pc)) {
			break;
		}

		printf("   0x%08lx",pc);
		if (tfp) {
			int match;

			match = AddrToSym(tfd,pc,fname,&offset);
			printf(": %s()",fname);
			if (offset)
				printf(" + 0x%lx",offset);
			if ((!match) || ((stopat != 0) && (strcmp(fname,stopat) == 0))) {
				putchar('\n');
				break;
			}
		}
		putchar('\n');
		maxdepth--;
		pass++;
	}

	if (!maxdepth)
		printf("Max depth termination\n");
	
	if (tfp) {
		tfsclose(tfd,0);
	}
    return(0);
}
#endif
