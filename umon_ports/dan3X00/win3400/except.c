/*
All files except if stated otherwise in the begining of the file are under the GPLv2 license:
-----------------------------------------------------------------------------------

Copyright (c) 2010-2012 Design Art Networks Ltd.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

-----------------------------------------------------------------------------------
*/
/* except.c:
 *	This code handles exceptions that are caught by the exception vectors
 *	that have been installed by the monitor through vinit().  It is likely
 *	that there is already a version of this function available for the CPU
 *	being ported to, so check the cpu directory prior to porting this to a
 *	new target.
 */
#include "config.h"
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
exception(void)
{
	/* UMON-TODO: ADD_CODE_HERE */

	/* Populating these two values is target specific.
	 * Refer to other target-specific examples for details.
 	 * In some cases, these values are extracted from registers
	 * already put into the register cache by the lower-level
	 * portion of the exception handler in vectors_template.s
	 */
	ExceptionAddr = 0;
	ExceptionType = 0;

	/* Allow the console uart fifo to empty...
	 */
	flush_console_out();
	monrestart(EXCEPTION);
}

/* vinit():
 * This function is called by init1() at startup of the monitor to
 * install the monitor-based vector table.  The actual functions are
 * in vectors.s.
 */
void
vinit()
{
	/* UMON-TODO: ADD_CODE_HERE */
}

/* ExceptionType2String():
 * This function simply returns a string that verbosely describes
 * the incoming exception type (vector number).
 */
char *
ExceptionType2String(int type)
{
	char *string = 0;

	/* UMON-TODO: ADD_CODE_HERE */
	return(string);
}

