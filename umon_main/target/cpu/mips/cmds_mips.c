/*
 * cmds_mips.c:
 * This file contains monitor commands that are specific to the MIPS.
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
#include "cli.h"
#include "genlib.h"
#include "stddefs.h"

#define TEQ			0x00000034
#define SYSCALL		0x0000000c

#if INCLUDE_TRAPCMD

char *TrapHelp[] = {
	"Insert \"TEQ R0,R0\" instruction at specified address",
	"{address}",
	0,
};

/* TrapCmd():
 * This command inserts a TEQ R0,R0 instruction at the address specified.
 * This can be used for setting breakpoints if the code is in RAM.
 */
int
TrapCmd(int argc,char **argv)
{
	int		opt, verbose;
	ulong	*addr;
	
	if (argc < 2)
		return(CMD_PARAM_ERROR);

	while((opt=getopt(argc,argv,"v")) != -1) {
		switch(opt) {
		case 'v':
			verbose = 1;
			break;
		}
	}

	addr = (ulong *)strtoul(argv[optind],0,0);
	*addr = TEQ;

	if (dcacheFlush)
		dcacheFlush((char *)addr,4);
	if (icacheInvalidate)
		icacheInvalidate((char *)addr,4);

	return(CMD_SUCCESS);
}
#endif	/* INCLUDE_TRAPCMD */

#if INCLUDE_SYSCALLCMD

char *SyscallHelp[] = {
	"Insert \"SYSCALL #\" instruction at specified address",
	"{syscall #} {address}",
	0,
};

/* SyscallCmd():
 * This command inserts a SYSCALL # instruction at the address specified.
 * This can be used for setting breakpoints if the code is in RAM.
 */
int
SyscallCmd(int argc,char **argv)
{
	int		num;
	ulong	*addr;
	
	if (argc < 3)
		return(CMD_PARAM_ERROR);

	num = (ulong)strtoul(argv[optind],0,0);
	num <<= 6;
	addr = (ulong *)strtoul(argv[optind+1],0,0);
	*addr = SYSCALL | num;

	if (dcacheFlush)
		dcacheFlush((char *)addr,4);
	if (icacheInvalidate)
		icacheInvalidate((char *)addr,4);

	return(CMD_SUCCESS);
}
#endif	/* INCLUDE_TRAPCMD */
