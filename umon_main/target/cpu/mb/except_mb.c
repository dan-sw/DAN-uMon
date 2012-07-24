/* except_template.c:
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
 *	Author:	Ed Sutter
 *	email:	esutter@lucent.com
 *	phone:	908-582-2351		
 */
#include "config.h"
#include "cpu.h"
#include "cpuio.h"
#include "genlib.h"
#include "stddefs.h"
#include "warmstart.h"

ulong	ExceptionAddr;
int		ExceptionType;

/* exception():
 * This is the first 'C' function called out of a monitor-installed
 * exception handler. 
 */
void
exception(ulong type)
{
	ExceptionType = type;
	switch(type) {
		case EXCEPTION_INTERRUPT:
			getreg("R14",&ExceptionAddr);
			break;
		case EXCEPTION_USER:
			getreg("R15",&ExceptionAddr);
			break;
		case EXCEPTION_BREAK:
			getreg("R16",&ExceptionAddr);
			break;
		case EXCEPTION_HARDWARE:
			getreg("R17",&ExceptionAddr);
			break;
	}
	putreg("PC",ExceptionAddr);

	/* Allow the console uart fifo to empty...
	 */
	flush_console_out();
	monrestart(EXCEPTION);
}

/* vinit():
 * This function is called by init1() at startup of the monitor to
 * install the monitor-based vector table.
 * This must be done prior to cache being turned on.
 */
void
vinit()
{
	extern char vector_table, vector_table_end;

	memcpy(RAM_VECTOR_TABLE,&vector_table,&vector_table_end - &vector_table);
}

/* ExceptionType2String():
 * This function simply returns a string that verbosely describes
 * the incoming exception type (vector number).
 */
char *
ExceptionType2String(int type)
{
	char *string;

	switch(type) {
		case EXCEPTION_USER:
			string = "USER";
			break;
		case EXCEPTION_INTERRUPT:
			string = "INTERRUPT";
			break;
		case EXCEPTION_BREAK:
			string = "BREAK";
			break;
		case EXCEPTION_HARDWARE:
			string = "HARDWARE";
			break;
		default:
			string = "???";
			break;
	}
	return(string);
}

