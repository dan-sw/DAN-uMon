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
 */
#include "config.h"
#if INCLUDE_STRACE
#include "tfs.h"
#include "tfsprivate.h"
#include "ctype.h"
#include "genlib.h"
#include "stddefs.h"

#define ADDI_INSTR	0x7f			/* add	#imm,r15 */
#define RTS_INSTR	0x000b			/* rts */
#define STS_PR_R15	0x4f22			/* sts.l pr,@-r15 */

extern  int getreg();
extern	int AddrToSym(int,ulong,char *,ulong *);
extern	ulong ExceptionAddr;

char *StraceHelp[] = {
    "Stack trace", "-[d:F:P:rs:]",
	" -d #	max depth count (def=20)",
    " -F #	specify frame-pointer (don't use content of R1)",
	" -P #  specify PC (don't use content of SRR#)",
	" -r	dump regs",
	0,
};

int
Strace(int argc,char *argv[])
{
	uchar	*ftext, dlta;
	char	*symfile, fname[64];
	ulong	*framepointer, pc, fp, offset;
	int		fdelta, leaf, tfd, opt, maxdepth, debug;

	tfd = fp = debug = 0;
	maxdepth = 20;
	pc = ExceptionAddr;
    while ((opt=getopt(argc,argv,"Dd:F:P:r")) != -1) {
		switch(opt) {
		case 'D':
			debug = 1;
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
	    default:
			return(0);
		}
	}
	
	/* Start by detecting the presence of a symbol table file... */
	symfile = getenv("SYMFILE");
	if (!symfile)
		symfile = SYMFILE;

	tfd = tfsopen(symfile,TFS_RDONLY,0);
	if (tfd < 0) {
		printf("Stack trace needs %s file\n",symfile);
		return(0);
	}

	/* Load initial frame pointer: */
	if (!fp)
		getreg("R14", &framepointer);
	else
		framepointer = (ulong *)fp;

	/* Show current position: */
	AddrToSym(tfd,pc,fname,&offset);
	printf("   0x%08lx: %s()",pc,fname);
	if (offset)
		printf(" + 0x%lx",offset);
	putchar('\n');

	/* Now the tricky stuff...
	 * If there are no arguments passed to the functions in the stack's
	 * function nesting, then all we need to do is jump from one frame
	 * pointer to the next.  Unfortunately, this is not usually the case,
	 * so we need to figure out how many arguments each function has as we
	 * are processing the stack trace.  Also, for the first function of the
	 * nesting, we must determine if it is a leaf function because the frame
	 * will be a bit different in that case.  In either case, we need to
	 * be able to take an address in text space, determine what function it
	 * is, and then determine where the starting address for that function is.
	 * This is done with the AddrToSym() function.  It takes in an address,
	 * returns the name of the function as well as the gap between the 
	 * incoming address and the actual starting point of the function's text.
	 * Point being: without the symbol table, this is likely not to work.
	 *
	 * Determining if function is a leaf:
	 * This is done by searching through the text of the function and
	 * looking for the "STS.L PR,@-R15" instruction.  If this instruction
	 * is found prior to the RTS, then we assume the function is NOT a leaf
	 * because the Procedture Register is being pushed onto the stack
	 * (meaning that the compiler added the code to prepare for the fact
	 * that another function may be called from within the current function).
	 *
	 * Determining number of arguments:
	 * Since the arguments passed into a function are placed on the stack
	 * frame, and the code for a function must be aware of this, we can once
	 * again, look to the code for hints.  In this case we look for the
	 * first occurrence of the instruction "ADD #IMM,R15".  This is code
	 * that adjusts the stack pointer based on the number of arguments passed
	 * into the function that are on the stack frame.  The value of #IMM is
	 * what we use to determine the number of arguments.  For example, if
	 * the instruction is ADD #0xFC,R15 then we know that one argument is 
	 * using stack space.  Similarly "ADD #0xF8,R15" is 2, "ADD #0xF4,R15"
	 * is 3 and so on.
	 */
	ftext = (uchar *)pc;
	ftext -= offset;
	leaf = 1;
	fdelta = 0;
	while((*ftext != ADDI_INSTR) && (*(ushort*)ftext != RTS_INSTR)) {
		if (*(ushort *)ftext == STS_PR_R15)
			leaf = 0;
		ftext+=2;
	}
	if (*(ushort*)ftext != RTS_INSTR) {
		dlta = (int)*(ftext+1);
		while(dlta) {
			fdelta++;
			dlta += 4;
		}
	}
	if (debug) {
		printf("                     FRAME_PTR=0x%08lx, DLTA=%d, LEAF=%d\n",
			(ulong)framepointer, fdelta, leaf);
	}
	framepointer += (fdelta+leaf);

	/* Now step through the stack frame... */
	while(maxdepth) {
		if (((int)framepointer & 3) || (!framepointer) || (!*framepointer))
			break;

		if (debug)
			printf("                     FUNC_ADDR=0x%08lx\n",*framepointer);
		
		if (!AddrToSym(tfd,*framepointer,fname,&offset))
			break;

		printf("   0x%08lx: %s()",*framepointer,fname);
		if (offset)
			printf(" + 0x%lx",offset);
		putchar('\n');

		/* See notes above regarding the computation of fdelta... */
		fdelta = 0;
		ftext = (uchar *)*framepointer;
		ftext -= offset;
		while((*ftext != ADDI_INSTR) && (*(ushort*)ftext != RTS_INSTR))
			ftext+=2;
		if (*(ushort*)ftext != RTS_INSTR) {
			dlta = (int)*(ftext+1);
			while(dlta) {
				fdelta++;
				dlta += 4;
			}
		}
		if (debug) {
			printf("                     FRAME_PTR=0x%08lx, DLTA=%d\n",
				(ulong)framepointer, fdelta);
		}
		framepointer = (ulong *)*(framepointer+1);
		framepointer += fdelta;
		maxdepth--;
	}

	if (!maxdepth)
		printf("Max depth termination\n");
	
	if (tfd >= 0)
		tfsclose(tfd,0);
    return(0);
}
#endif
