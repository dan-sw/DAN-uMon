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


// list prototypes from <conio.h> here (because of it conflicts with genlib.h)
int __cdecl _getch(void);
int __cdecl _putch(int);


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

/* As a bare minimum, initialize the console UART here using the
 * incoming 'baud' value as the baud rate.
 */
void devInit (int baud)
{
}

/* Provide a means to change the baud rate of the running
 * console interface.  If the incoming value is not a valid
 * baud rate, then default to 9600.
 * In the early stages of a new port this can be left empty.
 * Return 0 if successful; else -1.
 */
int ConsoleBaudSet (int baud)
{
	return 0;
}

/* Target-specific portion of flush_console() in chario.c.
 * This function returns 1 if there are no characters waiting to
 * be put out on the UART; else return 0 indicating that the UART
 * is still busy outputting characters from its FIFO.
 * In the early stages of a new port this can simply return 1.
 */
int target_console_empty (void)
{
	return 1;
}


/* When buffer has space available, load the incoming character into the UART.
 */
int target_putchar (char c)
{
	return _putch (c);
}


/* Assume there is a character in the UART's input buffer and just
 * pull it out and return it.
 */
int target_getchar(void)
{
	if (cpuioKeyboardBufFull)
	{
		cpuioKeyboardBufFull = false;
		return cpuioKeyboardBuf;
	}

	return _getch();
}


/* Return 0 if no char is available at UART rcv fifo; else 1.
 * Do NOT pull character out of fifo, just return status. 
 *
 * Define INCLUDE_BLINKLED in config.h and add STATLED_ON()
 * and STATLED_OFF() macros (or functions)  so that uMon will
 * run and blink a target LED at a configured (or default 500msec)
 * interval...
 */
int target_gotachar (void)
{
	if (!cpuioKeyboardBufFull)
	{
		cpuioKeyboardBuf	 = target_getchar();
		cpuioKeyboardBufFull = 1 /*true*/;
	}
	return 1;
}



/* Disable all system interrupts here and return a value that can
 * be used by intsrestore() (later) to restore the interrupt state.
 */
ulong
intsoff(void)
{
	ulong status;

	/* UMON_TODO: ADD_CODE_HERE */
	return(status);
}

/* Re-establish system interrupts here by using the status value
 * that was returned by an earlier call to intsoff().
 */
void
intsrestore(ulong status)
{
	/* UMON_TODO: ADD_CODE_HERE */
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

