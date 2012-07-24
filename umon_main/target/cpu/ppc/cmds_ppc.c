/*
 * cmds_ppc.c:
 * This file contains monitor commands that are specific to the PowerPC.
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
#include "arch_ppc.h"

#define MTSPR_R3		0x7c6003a6
#define MFSPR_R3		0x7c6002a6

#define MTDCR_R3		0x7c600386
#define MFDCR_R3		0x7c600286

#define SYSCALL			0x44000002
#define TRAPWORD		0x7fe00008
#define SYNC			0x7c0004ac
#define ISYNC			0x4c00012c
#define BLR				0x4e800020

#define SPRREG			1
#define DCRREG			2

extern void ppc_invalidateIcache();
extern void ppc_flushDcache();

#if INCLUDE_SPRCMD | INCLUDE_DCRCMD

/* mtreg() and mfreg():
 * Build the appropriate mf/mt (move from/move to) instruction
 * right on the stack and execute it.
 */

static void
mtreg(ulong regnum, ulong regval, int type)
{
	ulong tfunc[4];
	ulong	reg;
	void (*func)();

	reg = (((regnum & 0x1f) << 5) | ((regnum >> 5) & 0x1f));
	tfunc[0] = SYNC;
	tfunc[1] = ISYNC;
	switch(type) {
	case SPRREG:
		tfunc[2] = (MTSPR_R3 | (reg << 11));
		break;
	case DCRREG:
		tfunc[2] = (MTDCR_R3 | (reg << 11));
		break;
	}
	tfunc[3] = BLR;		
	ppc_invalidateIcache(tfunc,32);
	ppc_flushDcache(tfunc,32);
	func = (void(*)())tfunc;
	func(regval);
}

static ulong
mfreg(ulong regnum, int type)
{
	ulong tfunc[4];
	ulong	reg;
	ulong (*func)(int);

	reg = (((regnum & 0x1f) << 5) | ((regnum >> 5) & 0x1f));
	tfunc[0] = SYNC;
	tfunc[1] = ISYNC;
	switch(type) {
	case SPRREG:
		tfunc[2] = (MFSPR_R3 | (reg << 11));		/* mfspr r3,SPRNUM */
		break;
	case DCRREG:
		tfunc[2] = (MFDCR_R3 | (reg << 11));		/* mfdcr r3,DCRNUM */
		break;
	}
	tfunc[3] = BLR;	
	ppc_invalidateIcache(tfunc,32);
	ppc_flushDcache(tfunc,32);
	func = (ulong(*)())tfunc;
	return(func(regnum));
}
#endif

#if INCLUDE_SPRCMD

char *SprHelp[] = {
	"Display/modify SPR register",
	"{sprnum | REGNAME} [sprval]",
	" where REGNAME = msr",
	0,
};

/* SprCmd:
 * Command for easy CLI access to PPC SPR registers.
 * The mtreg() and mfreg() functions build small assy subroutines
 * right in their own stack space.  This is necessary so that the
 * appropriate SPR index is place in the mtreg/mfreg instruction.
 * I-cache and D-cache must be invalidated/flushed after the assy routine
 * is placed in ram.
 */
int
SprCmd(int argc,char **argv)
{
	char	*arg1, *arg2, *regname;
	int		opt;
	ulong	sprnum, sprval;

	regname = (char *)0;
	while((opt=getopt(argc,argv,"n:")) != -1) {
		switch(opt) {
		case 'n':
			regname = optarg;
			break;
		default:
			return(CMD_PARAM_ERROR);
		}
	}

	if ((argc != optind+1) && (argc != optind+2))
		return(CMD_PARAM_ERROR);

	arg1 = argv[optind];
	arg2 = argv[optind+1];

	sprnum = strtoul(arg1,0,0);
	sprval = strtoul(arg2,0,0);

	if (argc == optind+1) {
		if (!strcmp(arg1,"msr")) {
			printf("MSR = 0x%lx\n",ppcMfmsr());
		}
		else {
			if (regname) {
				printf("%s (spr %ld) = 0x%08lx\n",regname,sprnum,
					mfreg(sprnum,SPRREG));
			}
			else {
				printf("SPR %ld (0x%lx) = 0x%08lx\n",sprnum,sprnum,
					mfreg(sprnum,SPRREG));
			}
		}
	}
	else if (argc == optind+2) {
		if (!strcmp(arg1,"msr")) {
			ppcMtmsr(sprval);
		}
		else {
			mtreg(sprnum,sprval,SPRREG);
		}
	}

	return(CMD_SUCCESS);
}
#endif	/* INCLUDE_SPRCMD */

#if INCLUDE_DCRCMD

char *DcrHelp[] = {
	"Display/modify DCR register",
	"{dcrnum} [dcrval]",
	0,
};

/* DCRCmd:
 * Command for easy CLI access to PPC DCR registers.
 * The mtreg() and mfreg() functions build small assy subroutines
 * right in their own stack space.  This is necessary so that the
 * appropriate DCR index is place in the mtreg/mfreg instruction.
 * I-cache and D-cache must be invalidated/flushed after the assy routine
 * is placed in ram.
 */
int
DcrCmd(int argc,char **argv)
{
	char	*arg1, *arg2, *regname;
	int		opt;
	ulong	dcrnum, dcrval;

	regname = (char *)0;
	while((opt=getopt(argc,argv,"n:")) != -1) {
		switch(opt) {
		case 'n':
			regname = optarg;
			break;
		default:
			return(CMD_PARAM_ERROR);
		}
	}

	if ((argc != optind+1) && (argc != optind+2))
		return(CMD_PARAM_ERROR);

	arg1 = argv[optind];
	arg2 = argv[optind+1];

	dcrnum = strtoul(arg1,0,0);
	dcrval = strtoul(arg2,0,0);

	if (argc == optind+1) {
		if (regname) {
			printf("%s (dcr %ld) = 0x%08lx\n",regname,dcrnum,
				mfreg(dcrnum,DCRREG));
		}
		else {
			printf("DCR %ld (0x%lx) = 0x%08lx\n",dcrnum,dcrnum,
				mfreg(dcrnum,DCRREG));
		}
	}
	else if (argc == optind+2) {
		mtreg(dcrnum,dcrval,DCRREG);
	}

	return(CMD_SUCCESS);
}
#endif	/* INCLUDE_DCRCMD */

#if INCLUDE_TRAPCMD | INCLUDE_SYSCALLCMD | INCLUDE_IABRCMD | INCLUDE_DABRCMD

/* installHandler():
 * This function is used by the trap, syscall, iabr and dabr commands.
 * The idea is this...
 * You're running with an RTOS that may have overwritten the exception
 * handlers that were used by the monitor.  Now you want to use one of
 * these exceptions to generate a monitor-controlled breakpoint.  This
 * means that the monitor must "own" the exception handler (so that taking
 * the exception puts the application back into monitor space); hence, you
 * need to overwrite the exception handler installed by the RTOS with the
 * monitor's version.  Note that this requires that the monitor exception
 * handler be relocatable.
 *
 * To accomplish this, each of these commands works in several steps...
 * 1. Configure the command so that it knows where the monitor's exception
 *    handler is.  This is done with the -c option and must be run prior
 *    to starting up the RTOS.
 * 2. After the RTOS has come up and has installed its own exception 
 *    handlers, the -i (or -I) option of the command is run to re-install
 *    the monitor's excpetion handler.
 * 3. Activate the exception, this is very exception-specific, so see below
 *    for these details.
 */
void
installHandler(char *src, char *dest, int size, int verbose)
{
	if (verbose) {
		printf("Copying %d-byte handler at 0x%lx to 0x%lx.\n",
			size,(ulong)src,(ulong)dest);
	}

	if (src == dest)
		return;

	memcpy(dest,src,size);
	ppc_flushDcache(dest,size);
	ppc_invalidateIcache(dest,size);
}
#endif

#if INCLUDE_TRAPCMD

#ifndef TRAP_OFFSET
#define TRAP_OFFSET 0x0700
#endif

char *TrapHelp[] = {
	"Use trap instruction for breakpoints",
	"-[cI:iv] {address}",
	"Options:",
	" -c  store away the current location of the trap exception",
	" -i  install exception handler @ standard location",
	" -I{adr}",
	"     install exception handler @ 'adr'",
	" -v  verbose install",
	0,
};


/* TrapCmd():
 * Load the monitor's specified vector table entry into
 * the current vector table.
 */
int
TrapCmd(int argc,char **argv)
{
	static	ulong	umon_program_exception;
	ulong	*addr, handler_at;
	int		opt, install_handler, verbose;
	
	if (argc < 2)
		return(CMD_PARAM_ERROR);

	verbose = 0;
	handler_at = baseofExceptionTable() + TRAP_OFFSET;
	install_handler = 0;
	while((opt=getopt(argc,argv,"cI:iv")) != -1) {
		switch(opt) {
		case 'c':
			umon_program_exception = handler_at;
			return(CMD_SUCCESS);
		case 'I':
			install_handler = 1;
			handler_at = strtoul(optarg,0,0);
			break;
		case 'i':
			install_handler = 1;
			break;
		case 'v':
			verbose = 1;
			break;
		}
	}

	/* If -l option is set, then we install the monitor's exception
	 * handler at the PROGRAM_EXCEPTION location. 
	 */
	if (install_handler) {
		installHandler((char *)umon_program_exception,(char *)handler_at,256,
			verbose);
		if (argc == optind)
			return(CMD_SUCCESS);
	}

	if (argc != (optind + 1))
		return(CMD_PARAM_ERROR);

	addr = (ulong *)strtoul(argv[optind],0,0);
	*addr = TRAPWORD;
	ppc_flushDcache(addr,4);
	ppc_invalidateIcache(addr,4);
	return(CMD_SUCCESS);
}
#endif	/* INCLUDE_TRAPCMD */

#if INCLUDE_SYSCALLCMD

#ifndef SYSCALL_OFFSET
#define SYSCALL_OFFSET 0x0c00
#endif

char *SyscallHelp[] = {
	"Use syscall instruction for breakpoints",
	"-[I:iv] {address}",
	"Options:",
	" -c  store away the current location of the syscall exception",
	" -i  install exception handler @ standard location",
	" -I{adr}",
	"     install exception handler @ 'adr'",
	" -v  verbose install",
	0,
};

/* SyscallCmd():
 * Use SC (system call) instruction to generate a SYSTEM_CALL exception.
 */
int
SyscallCmd(int argc,char **argv)
{
	static	ulong	umon_syscall_exception;
	ulong	*addr, handler_at;
	int		opt, install_handler, verbose;

	if (argc < 2)
		return(CMD_PARAM_ERROR);

	verbose = 0;
	install_handler = 0;
	handler_at = baseofExceptionTable() + SYSCALL_OFFSET;
	while((opt=getopt(argc,argv,"cI:iv")) != -1) {
		switch(opt) {
		case 'c':
			umon_syscall_exception = handler_at;
			return(CMD_SUCCESS);
		case 'I':
			install_handler = 1;
			handler_at = strtoul(optarg,0,0);
			break;
		case 'i':
			install_handler = 1;
			break;
		case 'v':
			verbose = 1;
			break;
		}
	}

	/* If -i option is set, then we install the monitor's exception
	 * handler at the SYSCALL_EXCEPTION location.
	 */
	if (install_handler) {
		installHandler((char *)umon_syscall_exception,(char *)handler_at,
			256, verbose);
		if (argc == optind)
			return(CMD_SUCCESS);
	}

	if (argc != (optind + 1))
		return(CMD_PARAM_ERROR);

	addr = (ulong *)strtoul(argv[optind],0,0);
	*addr = SYSCALL;
	ppc_flushDcache(addr,4);
	ppc_invalidateIcache(addr,4);
	return(CMD_SUCCESS);
}

#endif /* INCLUDE_SYSCALLCMD */

#if INCLUDE_IABRCMD

#ifndef IABR_OFFSET
#define IABR_OFFSET 0x1300
#endif

char *IabrHelp[] = {
	"Use IABR (spr 1010) to establish instruction-breakpoint.",
	"-[cdI:itv] [address]",
	"Options:",
	" -c  store away the current location of the iabr exception",
	" -d  disable the exception",
	" -i  install exception handler @ standard location",
	" -I{adr}",
	"     install exception handler @ 'adr'",
	" -t  translation enabled",
	" -v  verbose install",
	0,
};

#define IABR_BE	0x2
#define IABR_TE	0x1

/* IabrCmd():
 * Use IABR register to generate a DSI exception.
 */
int
IabrCmd(int argc,char **argv)
{
	static	ulong	umon_addrbkpt_exception;
	ulong	addr, mask, handler_at;
	int		opt, install_handler, disable, verbose;

	if (argc < 2)
		return(CMD_PARAM_ERROR);

	handler_at = baseofExceptionTable() + IABR_OFFSET;
	mask = IABR_BE;
	install_handler = 0;
	verbose = disable = 0;
	while((opt=getopt(argc,argv,"cdI:itv")) != -1) {
		switch(opt) {
		case 'c':
			umon_addrbkpt_exception = handler_at;
			return(CMD_SUCCESS);
		case 'd':
			disable = 1;
			break;
		case 'I':
			install_handler = 1;
			handler_at = strtoul(optarg,0,0);
			break;
		case 'i':
			install_handler = 1;
			break;
		case 't':
			mask = IABR_TE;
			break;
		case 'v':
			verbose = 1;
			break;
		}
	}

	/* If -d is set, then we disable the DABR's ability to 
	 * generate an exception.  All other arguments and options
	 * are ignored.
	 */
	if (disable) {
		ppcMtiabr(0);
		return(CMD_SUCCESS);
	}

	/* If -i option is set, then we install the monitor's exception
	 * handler at the DSI_EXCEPTION location.
	 */
	if (install_handler) {
		installHandler((char *)umon_addrbkpt_exception,
			(char *)handler_at,256,verbose);
		if (argc == optind)
			return(CMD_SUCCESS);
	}

	if (argc != (optind + 1))
		return(CMD_PARAM_ERROR);

	addr = strtoul(argv[optind],0,0);
	addr &= ~3;

	ppcMtiabr(addr | mask);

	return(CMD_SUCCESS);
}

#endif /* INCLUDE_IABRCMD */

#if INCLUDE_DABRCMD

#ifndef DABR_OFFSET
#define DABR_OFFSET 0x0300
#endif

char *DabrHelp[] = {
	"Use DABR (spr 1013) to establish data-breakpoint.",
	"-[cdI:irtwv] [address]",
	"Options:",
	" -c  store away the current location of the dabr exception",
	" -d  disable the exception",
	" -i  install exception handler @ standard location",
	" -I{adr}",
	"     install exception handler @ 'adr'",
	" -r  enable 'read' access exception",
	" -t  translation enabled",
	" -w  enable 'write' access exception",
	" -v  verbose install",
	0,
};

#define DABR_BT	0x4
#define DABR_DW	0x2
#define DABR_DR	0x1

/* DabrCmd():
 * Use DABR register to generate a DSI exception.
 */
int
DabrCmd(int argc,char **argv)
{
	static	ulong	umon_dsi_exception;
	ulong	addr, mask, handler_at;
	int		opt, install_handler, disable, verbose;

	if (argc < 2)
		return(CMD_PARAM_ERROR);

	mask = 0;
	disable = 0;
	verbose = 0;
	handler_at = baseofExceptionTable() + DABR_OFFSET;
	install_handler = 0;
	while((opt=getopt(argc,argv,"cdI:irtvw")) != -1) {
		switch(opt) {
		case 'c':
			umon_dsi_exception = handler_at;
			return(CMD_SUCCESS);
		case 'd':
			disable = 1;
			break;
		case 'I':
			install_handler = 1;
			handler_at = strtoul(optarg,0,0);
			break;
		case 'i':
			install_handler = 1;
			break;
		case 'r':
			mask |= DABR_DR;	/* Enable read-acces to trigger exception */
			break;
		case 't':
			mask |= DABR_BT;
			break;
		case 'w':
			mask |= DABR_DW;	/* Enable write-acces to trigger exception */
			break;
		case 'v':
			verbose = 1;
			break;
		}
	}

	/* If -d is set, then we disable the DABR's ability to 
	 * generate an exception.  All other arguments and options
	 * are ignored.
	 */
	if (disable) {
		ppcMtdabr(0);
		return(CMD_SUCCESS);
	}

	/* If -l option is set, then we install the monitor's exception
	 * handler at the DSI_EXCEPTION location.
	 */
	if (install_handler) {
		installHandler((char *)umon_dsi_exception, (char *)handler_at,
			256,verbose);
		if (argc == optind)
			return(CMD_SUCCESS);
	}

	if (argc != (optind + 1))
		return(CMD_PARAM_ERROR);

	if (mask == 0) {
		printf("Must specify -r and/or -w\n");
		return(CMD_FAILURE);
	}

	addr = strtoul(argv[optind],0,0) & ~7;
	ppcMtdabr(addr | mask);

	return(CMD_SUCCESS);
}

#endif /* INCLUDE_DABRCMD */
