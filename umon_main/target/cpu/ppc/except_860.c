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
#include "genlib.h"
#include "cpuio.h"
#include "stddefs.h"
#include "warmstart.h"

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
		case 0xe00:
			string = "floating_point_assist";
			break;
		case 0x1000:
			string = "software_emulation";
			break;
		case 0x1100:
			string = "instruction_tlbmiss";
			break;
		case 0x1200:
			string = "data_tlbmiss";
			break;
		case 0x1300:
			string = "instruction_tlberror";
			break;
		case 0x1400:
			string = "data_tlberror";
			break;
		case 0x1c00:
			string = "data_breakpoint";
			break;
		case 0x1d00:
			string = "instruction_breakpoint";
			break;
		case 0x1e00:
			string = "peripheral_breakpoint";
			break;
		case 0x1f00:
			string = "nonmaskable_development_port";
			break;
		case 0xa00:
		case 0xb00:
		case 0x1500:
		case 0x1600:
		case 0x1700:
		case 0x1800:
		case 0x1900:
		case 0x1a00:
		case 0x1b00:
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
	switch(type) {
		case 0x0c00:
		case 0x1c00:
		case 0x1d00:
		case 0x1e00:
			ExceptionAddr -= 4;
			break;
	}
	warmstart(EXCEPTION);
}

void
vinit()
{
	extern	void (*vectortable_start)(), (*vectortable_end)();
	ulong	*from, *to, *end;

#if 0	// Changed this to eliminate "dereferencing type-punned pointer will
		// break strict-aliasing rules".
	from = (ulong *)&vectortable_start;
	end = (ulong *)&vectortable_end;
#else
	ulong tmp = (ulong)&vectortable_start;
	from = (ulong *)tmp;
	tmp = (ulong)&vectortable_end;
	end = (ulong *)tmp;
#endif
	to = (ulong *)0x100;
	while(from < end)
		*to++ = *from++; 
}

ulong
baseofExceptionTable(void)
{
	return(0);
}
