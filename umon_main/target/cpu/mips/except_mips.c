/* except_mips.c:
 *	This code handles exceptions that are caught by the exception vectors
 *	that have been installed by the monitor.
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
ExceptionType2String(int cause)
{
	char	*string;
	static char buf[16];

	switch(cause) {
		case 0:
			string = "interrupt";
			break;
		case 1:
			string = "TLB modification";
			break;
		case 2:
			string = "TLB exception (load or fetch)";
			break;
		case 3:
			string = "TLB exception (store)";
			break;
		case 4:
			string = "Address err (load or fetch)";
			break;
		case 5:
			string = "Address err (store)";
			break;
		case 6:
			string = "Bus err (fetch)";
			break;
		case 7:
			string = "Bus err (data load or store)";
			break;
		case 8:
			string = "Syscall";
			break;
		case 9:
			string = "Breakpoint";
			break;
		case 10:
			string = "Reserved instruction";
			break;
		case 11:
			string = "Coprocessor unusable";
			break;
		case 12:
			string = "Arithmetic overflow";
			break;
		case 13:
			string = "Trap";
			break;
		case 23:
			string = "Watchpoint";
			break;
		case 24:
			string = "Machine check (duplicate TLB entry)";
			break;
		default:
			sprintf(buf,"%d",cause);
			string = buf;
			break;
	}
	return(string);
}

extern ulong getEPC(void);

void
exception(ulong cause_reg)
{
	ulong cause;

	cause = (cause_reg >> 2) & 0x1f;
	ExceptionType = cause;
	ExceptionAddr = getEPC();
	warmstart(EXCEPTION);
}

void
vinit(void)
{

}
