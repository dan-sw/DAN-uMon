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
    "-[a:d:F:P:rs:v]",
    " -a #   fp adjustment",
    " -d #   max depth count (def=20)",
    " -F #   specify frame-pointer (don't use content of R1)",
    " -P #   specify PC (don't use content of SRR#)",
    " -r     dump regs",
    " -s #   srr reg num (def=0)",
    " -v     verbose",
    0,
};

int
Strace(int argc,char *argv[])
{
	char	*symfile, fname[64];
	TFILE	*tfp;
	ulong	*framepointer, pc, fp, offset;
    int		tfd, opt, srrnum, maxdepth, pass, verbose, fadjust;

	tfd = fp = 0;
	srrnum = -1;
	maxdepth = 20;
	offset = 0;
	verbose = 0;
	fadjust = 0;
	pc = ExceptionAddr;
    while ((opt=getopt(argc,argv,"a:d:F:P:rs:v")) != -1) {
		switch(opt) {
		case 'a':
			fadjust = atoi(optarg);
			break;
		case 'd':
			maxdepth = atoi(optarg);
			break;
		case 'F':
			fp = strtoul(optarg,0,0);
			break;
		case 'P':
			pc = strtoul(optarg,0,0);
			break;
		case 'r':
			showregs();
			break;
		case 's':
			srrnum = atoi(optarg);
			break;
		case 'v':
			verbose = 1;
			break;
	    default:
			return(0);
		}
	}
	
	if (!fp) {
#if 0	// Changed this to eliminate "dereferencing type-punned pointer will
		// break strict-aliasing rules.
		getreg("R1", (ulong *)&framepointer);
#else
		ulong tmp = (ulong)&framepointer;
		getreg("R1", (ulong *)tmp);
#endif
	}
	else
		framepointer = (ulong *)fp;

	if (srrnum != -1) {
		if (srrnum == 0)
			getreg("SRR0", &pc);
		else if (srrnum == 2)
			getreg("SRR2", &pc);
		else  {
			printf("Invalid -s value\n");
			return(0);
		}
	}

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
		/* If we are at the first instruction of a function (offset=0),
		 * and this is the first pass in this loop; then we assume that
		 * this function's frame has not been accounted for yet...
		 */
		if ((pass == 0) && (offset == 0)) {
		}
		else {
			framepointer = (ulong *)*(framepointer + fadjust);
		}

		pc = *(framepointer + fadjust + 1);

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
			if (!match) {
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
