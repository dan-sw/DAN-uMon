/* except.c:
 *	This code handles exceptions that are caught by the exception vectors
 *	that have been installed by the monitor through vinit().
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
#include "cpu.h"
#include "cpuio.h"
#include "stddefs.h"
#include "genlib.h"
#include "cpu_403.h"
#include "arch_ppc.h"
#include "warmstart.h"

int		ExceptionType;
ulong	ExceptionAddr;

void
exception(int type)
{
	extern	int getreg();
	extern void flushRegtbl(void);
	ulong srr;

	intsoff();

	/* Critical interrupts store the address of the interrupted instruction
	 * in SRR2,  Non-critical interrupts store it in SRR0
	 */
	switch (type) {
	case 0x0100:			/* Critical interrupt pin */
	case 0x0200:			/* Machine check */
	case 0x1020:			/* Watchdog timer */
	case 0x2000:			/* Debug exception */
		getreg("SRR2", &srr);
		break;
	default:
		getreg("SRR0",&srr);
		break;
	}

	ExceptionType = type;
	ExceptionAddr = srr;

	/* Some exceptions load SRR with address of next instruction
	 * so decrement by 4 (size of 1 instruction) for those cases...
	 */
	switch (type) {
		case 0x0100:		/* Critical */
		case 0x0500:		/* External interrupt */
		case 0x0c00:		/* System call */
		case 0x1000:		/* PIT */
		case 0x1010:		/* FIT */
		case 0x1020:		/* WDT */
			ExceptionAddr -= 4;
			break;
	}
	flushRegtbl();
	flushDcache((char *)&ExceptionType,sizeof(int));
	flushDcache((char *)&ExceptionAddr,sizeof(ulong));
	monrestart(EXCEPTION);
}

/* The value of GV_ID_OFFSET (general vector id offset) is based on
 * the position of the "immediate" portion of the instruction in
 * the general_vector exception handler (found in vectors_403.s).
 */
#define GV_ID_OFFSET	3

void
copyGeneralVector(ushort *to, ushort vid)
{
	extern	char general_vector, end_general_vector;
	int		size;

	size = &end_general_vector - &general_vector;
	memcpy((char *)to,&general_vector,size);
	*(to+GV_ID_OFFSET) = vid;
}

void
vinit()
{
	extern	char pit_vector, end_wdt_vector;
	char	*base;
	int		offset, size;

	base = (char *)RAM_VECTOR_TABLE;

	for(offset = 0x100; offset < 0x1000; offset += 0x100)
		copyGeneralVector((ushort *)(base+offset),offset);

	/* The PIT, FIT and WDT vectors are copied as a common
	 * block...
	 */
	size = &end_wdt_vector - &pit_vector;
	memcpy(base+0x1000,(char *)&pit_vector,size);

	copyGeneralVector((ushort *)(base+0x1100),0x1100);
	copyGeneralVector((ushort *)(base+0x1200),0x1200);
	copyGeneralVector((ushort *)(base+0x2000),0x2000);

	asm("	sync");				/* Wait for writes to complete */
	ppcMtevpr(RAM_VECTOR_TABLE);
}

char *
ExceptionType2String(int type)
{
	char	*string;
	static char	buf[32];

	switch(type) {
		case 0x100:
			string = "Critical Interrupt Pin";
			break;
		case 0x200:
			string = "Machine Check";
			break;
		case 0x300:
			string = "Data Store";
			break;
		case 0x400:
			string = "Instruction Store";
			break;
		case 0x500:
			string = "External Interrupt";
			break;
		case 0x600:
			string = "Alignment";
			break;
		case 0x700:
			string = "Program";
			break;
		case 0xc00:
			string = "System Call";
			break;
		case 0x1000:
			string = "Programmable Interval Timer";
			break;
		case 0x1010:
			string = "Fixed Interval Timer";
			break;
		case 0x1020:
			string = "Watchdog Timer";
			break;
		case 0x1100:
			string = "Data TLB Miss";
			break;
		case 0x1200:
			string = "Instruction TLB Miss";
			break;
		case 0x2000:
			string = "Debug";
			break;
		default:
			sprintf(buf,"vtbl offset 0x%x",type);
			string = buf;
			break;
	}
	return(string);
}

ulong
baseofExceptionTable(void)
{
	return(ppcMfevpr());
}
