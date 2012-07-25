/*
All files except if stated otherwise in the begining of the file are under the ISC license:
-----------------------------------------------------------------------------------

Copyright (c) 2010-2012 Design Art Networks Ltd.

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

-----------------------------------------------------------------------------------
*/
/**********************************************************************\
 Library    :  uMON (Micro Monitor)
 Filename   :  cpuio.c
 Purpose    :  CPU IO port (basic IO routines)
 Owner		:  Sergey Krasnitsky
 Created    :  24.10.2010
\**********************************************************************/

#include "config.h"
#include "stddefs.h"
#include "cpuio.h"
#include "genlib.h"
#include "cache.h"
#include "warmstart.h"
#include "timer.h"
#include "misc_arm.h"

char cpuioKeyboardBuf;		// One-char buffer for implementing target_gotachar()
bool cpuioKeyboardBufFull;	// True when cpuioKeyboardBuf contains a char

/*
 When we are compiled as UMON_TARGET_XT (UART PHY is present),
 the following functions are implemented in uart16650 driver.
 Another compilation modes implementations are placed below.
 List of UART necessary functions:
	 void devInit (int baud);
	 int ConsoleBaudSet (int baud);
	 int target_getchar (void);
	 int target_gotachar (void);
	 int target_putchar (char c);
	 int target_console_empty (void);
*/

/* Disable all system interrupts here and return a value that can
 * be used by intsrestore() (later) to restore the interrupt state.
 */
ulong
intsoff(void)
{
	ulong status = getpsr();
	putpsr(status | 0xC0);
	return(status);
}

/* Re-establish system interrupts here by using the status value
 * that was returned by an earlier call to intsoff().
 */
void
intsrestore(ulong status)
{
	putpsr(status);
}

/* The default (absolute minimum) action to be taken by this function
 * is to call monrestart(INITIALIZE).  It would be better if there was
 * some target-specific function that would really cause the target
 * to reset...
 * UMON-TODO: revise it
 */
void
target_reset(void)
{
	flushDcache(0,0);
	disableDcache();
	invalidateIcache(0,0);
	disableIcache();
	monrestart(INITIALIZE);
}

/* If any CPU IO wasn't initialized in reset.S, do it here...
 * This just provides a "C-level" IO init opportunity. 
 */
void
initCPUio(void)
{
	/* ADD_CODE_HERE */
}

