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


#if UMON_TARGET == UMON_TARGET_WIN
// list prototypes from <conio.h> here (because of it conflicts with genlib.h)
int __cdecl _getch(void);
int __cdecl _putch(int);
#elif UMON_TARGET == UMON_TARGET_XT_SIM
#include "simcall.h"
#endif


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

#if UMON_TARGET == UMON_TARGET_XT_SIM

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
	#if UMON_TARGET == UMON_TARGET_WIN

	return _putch (c);

	#elif UMON_TARGET == UMON_TARGET_XT_SIM

	register int a2		__asm__ ("a2")	= SYS_write;	// from simcall.h
	register int a3		__asm__ ("a3")	= 1;			// TEN_IO_STDOUT
	register char *a4	__asm__ ("a4")	= &c;			// 1 data byte
	register int a5		__asm__ ("a5")	= 1;			// 1 byte len
	register int retval __asm__ ("a2");
	register int reterr __asm__ ("a3");
	__asm__ ("simcall"
		: "=a" (retval), "=a" (reterr)
		: "a" (a2), "a" (a3), "a" (a4), "a" (a5));
	
	#elif UMON_TARGET == UMON_TARGET_XT_JTAG

	register int a2		__asm__ ("a2")	= -5;			// GDBIO_TARGET_SYSCALL_WRITE
	register char *a3	__asm__ ("a3")	= &c;			// 1 data byte
	register int a4		__asm__ ("a4")	= 1;			// 1 byte len
	register int a6		__asm__ ("a6")	= 1;			// TEN_IO_STDOUT
	register int retval __asm__ ("a2");
	register int reterr __asm__ ("a3");
	__asm__ ("break 1,14"
		: "=a" (retval), "=a" (reterr)
		: "a" (a2), "a" (a3), "a" (a4), "a" (a6));
	//reterr = TEN_IO_errno_translation(reterr);

	#endif

    return c;
}


/* Assume there is a character in the UART's input buffer and just
 * pull it out and return it.
 */
int target_getchar(void)
{
    char c;

	if (cpuioKeyboardBufFull)
	{
		cpuioKeyboardBufFull = false;
		return cpuioKeyboardBuf;
	}

	#if UMON_TARGET == UMON_TARGET_WIN

	c = _getch();

	#elif UMON_TARGET == UMON_TARGET_XT_SIM

	register int a2		__asm__ ("a2")	= SYS_read; // from simcall.h
	register int a3		__asm__ ("a3")	= 0;		// TEN_IO_STDIN
	register char *a4	__asm__ ("a4")	= &c;		// 1 data byte
	register int a5		__asm__ ("a5")	= 1;		// 1 byte len
	register int retval __asm__ ("a2");
	register int reterr __asm__ ("a3");
	__asm__ ("simcall"
		: "=a" (retval), "=a" (reterr)
		: "a" (a2), "a" (a3), "a" (a4), "a" (a5));

	#elif UMON_TARGET == UMON_TARGET_XT_JTAG

	register int a2		__asm__ ("a2")	= -4;		// GDBIO_TARGET_SYSCALL_READ
	register char *a3	__asm__ ("a3")	= &c;		// 1 data byte
	register int a4		__asm__ ("a4")	= 1;		// 1 byte len
	register int a6		__asm__ ("a6")	= 0;		// TEN_IO_STDIN
	register int retval __asm__ ("a2");
	register int reterr __asm__ ("a3");
	__asm__ ("break 1,14"
		: "=a" (retval), "=a" (reterr)
		: "a" (a2), "a" (a3), "a" (a4), "a" (a6));
	//reterr = TEN_IO_errno_translation(reterr);

	#endif

	return c;
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

#if INCLUDE_BLINKLED
	static uint8_t ledstate;
	static struct elapsed_tmr tmr;

	#ifndef BLINKON_MSEC
	#define BLINKON_MSEC 500
	#define BLINKOFF_MSEC 500
	#endif

	switch(ledstate) {
		case 0:
			startElapsedTimer(&tmr,BLINKON_MSEC);
			ledstate = 1;
			STATLED_ON();
			break;
		case 1:
			if(msecElapsed(&tmr)) {
				STATLED_OFF();
				ledstate = 2;
				startElapsedTimer(&tmr,BLINKOFF_MSEC);
			}
			break;
		case 2:
			if(msecElapsed(&tmr)) {
				STATLED_ON();
				ledstate = 1;
				startElapsedTimer(&tmr,BLINKON_MSEC);
			}
			break;
	}
#endif
}

#endif  // UMON_TARGET != UMON_TARGET_XT


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

