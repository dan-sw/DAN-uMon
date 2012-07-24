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
#include "warmstart.h"
#include "genlib.h"
#include "cpuio.h"
#include "stddefs.h"
#include "arch_ppc.h"

ulong	ExceptionAddr;
int		ExceptionType;

char *
ExceptionType2String(int type)
{
	char	*string;
	static char buf[16];

	switch(type) {
		case 0x100:
			string = "system_reset";
			break;
		case 0x200:
			string = "machine_check";
			break;
		case 0x300:
			string = "dsi";
			break;
		case 0x400:
			string = "isi";
			break;
		case 0x500:
			string = "external_interrupt";
			break;
		case 0x600:
			string = "alignment";
			break;
		case 0x700:
			string = "program";
			break;
		case 0x800:
			string = "floating_point_unavailable";
			break;
		case 0x900:
			string = "decrementer";
			break;
		case 0xc00:
			string = "system_call";
			break;
		case 0xd00:
			string = "trace";
			break;
		case 0xf00:
			string = "performance_monitor";
			break;
		case 0x1300:
			string = "instruction_addr_breakpoint";
			break;
		case 0x1400:
			string = "system_management_interrupt";
			break;
		case 0x1700:
			string = "thermal_management_interrupt";
			break;
		case 0x0:
		case 0xa00:
		case 0xb00:
		case 0xe00:
		case 0x1000:
		case 0x1100:
		case 0x1200:
		case 0x1500:
		case 0x1600:
			string = "reserved";
			break;
		default:
			sprintf(buf,"0x%x",type);
			string = buf;
			break;
	}
	return(string);
}

void
exception(int type)
{
	ExceptionType = type;
	getreg("SRR0",&ExceptionAddr);
	warmstart(EXCEPTION);
}

void
vinit()
{
	extern	void (*vectortable_start)(), (*vectortable_end)();
	ulong	*from, *to, *end;

	from = (ulong *)&vectortable_start;
	end = (ulong *)&vectortable_end;

	/* Start at 0x100.
	 */
	to = (ulong *)0x100;
	while(from < end)
		*to++ = *from++; 

	/* Clear the IP bit in MSR so that we are now pointing to an
	 * exception table that is at 0x100...
	 */
	ppcMtmsr(ppcMfmsr() & ~ppcMsrIP);
}

ulong
baseofExceptionTable(void)
{
	if (ppcMfmsr() & MSR_EP)
		return(0xFFF00000);
	else
		return(0x00000000);
}
