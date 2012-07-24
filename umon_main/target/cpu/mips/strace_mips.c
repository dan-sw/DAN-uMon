/* strace_mips.c:
 *  
 * Support for retrieving a call-nesting or stack trace after an exception.
 *
 * General notice:
 * This code is part of a boot-monitor package developed as a generic base
 * platform for embedded system designs.  As such, it is likely to be
 * distributed to various projects beyond the control of the original
 * author.  Please notify the author of any enhancements made or bugs found
 * so that all may benefit from the changes.  In addition, notification back
 * to the author will allow the new user to pick up changes that may have
 * been made by other users after this version of the code was distributed.
 *
 * Note1: the majority of this code was edited with 4-space tabs.
 * Note2: as more and more contributions are accepted, the term "author"
 *		   is becoming a mis-representation of credit.
 *
 * Original author:	Ed Sutter
 * Email:			esutter@lucent.com
 * Phone:			908-582-2351
 *
 */
#include "config.h"
#if INCLUDE_STRACE
#include "tfs.h"
#include "cli.h"
#include "tfsprivate.h"
#include "ctype.h"
#include "genlib.h"
#include "stddefs.h"

#define ADDIU_SP	0x27bd0000
#define SW_RA_SP	0xafbf0000
#define JR_RA		0x03e00008

/* As far as I can figure out, there is no frame pointer in the MIPS
 * environment.  The stack pointer is used and the compiler decrements
 * the stack prior to returning from a function call.
 * For each function, the stack is decremented by some specified value.
 * This value is what is needed to determine the frame size, so it must
 * be extracted from the instruction stream...
 * There are two ways to get this out of the stream ( that I can think
 * of)...
 * 1. If you know the address of the starting point in the function, then
 *    assume the very first instruction is the decrementation of the SP.
 *    In this case, the instruction is typically "addiu   sp,sp,0xNNNN"
 *    where 0xNNNN is the stack adjustment.  The ADDIU_SP macro can be
 *	  used to search for this instruction encoding.
 * 2. Given an address within a function step forward looking for the 
 *	  return ("jr ra") instruction.  The value JR_RA is the encoding for
 *	  the instruction "jr ra" which is the MIPS equivalent of a function
 *	  return.  The very next instruction is the instruction that adds
 *	  the delta back to the SP, so the lower 16 bits of this instruction
 *	  can be used to get the delta.
 *
 * NEEDS TO BE DONE:
 * Since the framesize() function below is retrieving data from instruction
 * space, the D & I caches should be flushed prior to starting this trace.
 */

/* get_framesize():
 * The incoming address is assumed to be the starting point of a function.
 * This function walks through the the function looking for the instruction
 * that adjusts the stack pointer for the function's local work (the 
 * function's frame).  It appears that the instruction used for this is
 * "addiu sp, sp, XX" where 'XX' is some number that ajusts the stack pointer.
 *
 * get_ra_offset():
 * This function is similar to get_framesize() in that it also searches
 * through the text of the function looking for a value embedded within
 * the instruction stream.  In this case, it is looking for the location
 * that the RA register (return address) is placed on the stack.
 * In the case of get_ra_offset(); however, the search is started in
 * the middle of the function working back to the beginning.  This is
 * because it is possible that the exception occurred prior to the RA
 * value being stored onto the stack; hence, get_ra_offset() can legally
 * return zero.
 *
 * For example, in the following mixed C/ASM disassembly...
 *	int
 *	func2(int i)
 *	{
 *	a010005c:	27bdffe8 	addiu	sp,sp,-24	<<<--Frame adjustment.
 *	a0100060:	afb00010 	sw	s0,16(sp)
 *	a0100064:	00808021 	move	s0,a0
 *		mon_printf("func2\n");
 *	a0100068:	3c04a010 	lui	a0,0xa010
 *	a010006c:	24841fd8 	addiu	a0,a0,8152
 *	a0100070:	afbf0014 	sw	ra,20(sp)		<<<--Storage of RA on stack.
 *	a0100074:	0c0402d0 	jal	a0100b40 <mon_printf>
 *	a0100078:	26100003 	addiu	s0,s0,3
 *		func3(i+3);
 *	a010007c:	0c040007 	jal	a010001c <func3>
 *	a0100080:	02002021 	move	a0,s0
 *		return(99);
 *	}
 *	a0100084:	8fbf0014 	lw	ra,20(sp)
 *	a0100088:	8fb00010 	lw	s0,16(sp)
 *	a010008c:	24020063 	li	v0,99
 *	a0100090:	03e00008 	jr	ra
 *	a0100094:	27bd0018 	addiu	sp,sp,24	<<<-- Frame adjustment
 *
 */

int
get_framesize(ulong *addr)
{
	while(*addr != JR_RA) {
		if ((*addr & 0xffff0000) == ADDIU_SP) 
			return((0xffff - (*addr & 0xffff) + 1)/4);
		addr++;
	}
	addr++;
	if ((*addr & 0xffff0000) == ADDIU_SP) 
		return((*addr & 0xffff)/4);

	printf("strace framesize error\n");
	return(0);
}

int 
get_ra_offset(ulong *start, ulong offset)
{
	ulong *addr = start + offset/4;

	while(addr != start) {
		if ((*addr & 0xffff0000) == SW_RA_SP) 
			return((*addr & 0xffff)/4);
		addr--;
	}
	return(0);
}

char *StraceHelp[] = {
    "Stack trace",
    "-[d:F:f:rs:v]",
    " -d #   max depth count (def=20)",
    " -r     dump regs",
    " -s {func} stop at function 'func'",
    " -v     verbose",
    0,
};

int
Strace(int argc,char *argv[])
{
	TFILE	*tfp;
	ulong	*sp, pc, offset;
	ulong *function_address;
	ulong ra_adjust, framesize, ltmp;
	char	*symfile, fname[64], *stopat;
    int		tfd, opt, maxdepth, pass, verbose, fsize;

	tfd = 0;
	stopat = 0;
	verbose = 0;
	maxdepth = 20;
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
			return(CMD_PARAM_ERROR);
		}
	}
	
	getreg("R29/SP", &ltmp);
	sp = (ulong *)ltmp;
	sp += 1;

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
	if (!tfp) {
		printf("Strace requires symbol table file\n");
		return(CMD_FAILURE);
	}

	/* Show current position: */
	printf("   0x%08lx",pc);
	AddrToSym(tfd,pc,fname,&offset);
	printf(": %s()",fname);
	if (offset)
		printf(" + 0x%lx",offset);

	/* Now step through the stack frame... */
	pass = fsize = 0;
	while(maxdepth) {
		int match;

		function_address = (ulong *)(pc - offset);
		ra_adjust = get_ra_offset(function_address,offset);
		framesize = get_framesize(function_address);

		if (ra_adjust == 0)
			getreg("R31/RA", (ulong *)&pc);
		else
			pc = *(sp + ra_adjust);
		sp += framesize;

		if (verbose) {
			printf("\n   ra_adj: %ld, fsize: %ld, sp: 0x%lx, pc: 0x%lx\n",
				ra_adjust*4,framesize*4,(long)sp,(long)pc);
		}
		else
			putchar('\n');

		/* Retrieve a symbolic representation for the address.
		 * If not found, terminate the trace.
	 	 */
		if ((match = AddrToSym(tfd,pc,fname,&offset)) == 0)
			break;
		
		printf("   0x%08lx: %s()",pc,fname);
		if (offset)
			printf(" + 0x%lx",offset);

		if ((stopat != 0) && (strcmp(fname,stopat) == 0))
			break;

		maxdepth--;
		pass++;
	}

	putchar('\n');

	if (!maxdepth)
		printf("Max depth termination\n");
	
	if (tfp) {
		tfsclose(tfd,0);
	}
    return(0);
}

#endif
