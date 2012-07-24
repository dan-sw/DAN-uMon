/* timestuff.c
 * These functions support the monitor's ability to deal with elapsed
 * time on targets with or without hardware-timer support.
 *
 * The INCLUDE_HWTMR definition in config.h determines which mode
 * the timer will run in.
 *
 * The monitor does not require any hardware-assist for maintaining
 * elapsed time.  With no hardware assist (INCLUDE_HWTMR set to 0),
 * the value of LOOPS_PER_SECOND must be established in config.h, and
 * can be calibrated using the '-c' option in the sleep command.  Even
 * with this calibration the accuracy of this mechanism is limited;
 * however, since there is no code in the monitor that really requires
 * extremely accurate timing this may be ok.
 * 
 * On the other hand, it is preferrable to have accuracy, so on targets
 * that have a pollable clock,  the hooks can be put in place to allow
 * that clock to be used as the hardware assist for the monitor's 
 * elapsed time measurements.  The INCLUDE_HWTMR is set to 1, and 
 * TIMER_TICKS_PER_MSEC defines the number of ticks of the timer that
 * correspond to 1 millisecond of elapsed time.  The function target_timer() 
 * is assumed to be established and must retrun an unsigned long value
 * that is the content of the polled hardware timer.
 *
 * Regardless of the hardware-assist or not, the following interface is
 * used by the code in the monitor...
 *
 *		#include "timer.h"
 *
 *		struct elapsed_tmr tmr;
 *
 *		startElapsedTimer(&tmr,TIMEOUT):
 *		do {
 *			SOMETHING();
 *		} while(!msecElapsed(&tmr));
 *
 * Refer to the functions startElapsedTimer() and msecElapsed() below for
 * more details.
 * 
 * General notice:
 * This code is part of a boot-monitor package developed as a generic base
 * platform for embedded system designs.  As such, it is likely to be
 * distributed to various projects beyond the control of the original
 * author.  Please notify the author of any enhancements made or bugs found
 * so that all may benefit from the changes.  In addition, notification back
 * to the author will allow the new user to pick up changes that may have
 * been made by other users after this version of the code was distributed.
 *
 * Note1: the majority of this code was edited with 4-space tabs.
 * Note2: as more and more contributions are accepted, the term "author"
 *		   is becoming a mis-representation of credit.
 *
 * Original author:	Ed Sutter
 * Email:				esutter@lucent.com
 * Phone:				908-582-2351
 */

#include "config.h"
#include "stddefs.h"
#include "cli.h"
#include "genlib.h"
#include "ether.h"
#include "timer.h"

#if !INCLUDE_HWTMR
static unsigned long lpm;

unsigned long
getLoopsPerMillisecond(void)
{
	if (lpm == 0)
		lpm = LOOPS_PER_MILLISECOND;

	return(lpm);
}
#endif

/* startElapsedTimer() & msecElapsed():
 * The timer is started by loading the values timeout_low and timeout_high
 * with the number of ticks that must elapse for the timer to expire.
 *
 * In the case of the non-hardware-assisted timer, the expiration count
 * is based on the value of LoopsPerMillisecond (derived from the 
 * LOOPS_PER_SECOND definition in config.h).  Each time msecElapsed()
 * is called the elapsed count is incremented until it exceeds the timeout
 * timeout_low timeout_high values recorded by startElapsedTimer().
 * 
 * The case of the hardware-assisted timer is similar except that now
 * the number of ticks initialized in timeout_low and timeout_high are
 * based on the tick rate of the hardware timer (TIMER_TICKS_PER_MSEC).
 * This value is expected to be set in config.h.  Each time msecElapsed()
 * is called, it samples the timer and adds to the running total of ticks
 * until it matches or exceeds the timeout_low and timeout_high values.
 *
 * Notice that 64-bit values are used (high & low) because a 32-bit value
 * isn't large enough to deal with the tick rates (per second) of various
 * CPUs. 
 */
void
startElapsedTimer(struct elapsed_tmr *tmr, long milliseconds)
{
	unsigned long new_tm_low, ticks_per_msec;
	unsigned long stepmsecs, stepticks, remainder;

#if INCLUDE_HWTMR
	tmr->tmrflags = HWTMR_ENABLED;
	ticks_per_msec = (unsigned long)TIMER_TICKS_PER_MSEC;
#else
	tmr->tmrflags = 0;
	ticks_per_msec = getLoopsPerMillisecond();
#endif
	
	tmr->elapsed_low = 0;
	tmr->elapsed_high = 0;
	tmr->timeout_high = 0;
	tmr->timeout_low = 0;
	
	/* Convert incoming timeout from a millisecond count to a
	 * tick count...
	 * Maximum number of milliseconds and ticks before 32-bit
	 * (tick counter) unsigned long overlaps
	 */
	stepmsecs = 0xffffffff / ticks_per_msec;
	stepticks = stepmsecs * ticks_per_msec;
	remainder = (milliseconds % stepmsecs);

	/* Take care of the step remainder
	 */
	tmr->timeout_low = remainder * ticks_per_msec;
	milliseconds -= remainder;

	for (;milliseconds; milliseconds -= stepmsecs) {
		new_tm_low = tmr->timeout_low + stepticks;

		if (new_tm_low < tmr->timeout_low)
			tmr->timeout_high++;
		tmr->timeout_low = new_tm_low;
	}
	
#if INCLUDE_HWTMR
	tmr->tmrval = target_timer();
#else
	tmr->tmrval = 0;
#endif

}

int
msecElapsed(struct elapsed_tmr *tmr)
{
	ulong new_elapsed_low, new_tmrval, elapsed;

	/* If timeout has already occurred, then we can assume that this
	 * function being called without a matching startElapsedTimer() call.
	 */
	if (ELAPSED_TIMEOUT(tmr))
		return(1);

#if INCLUDE_HWTMR
	new_tmrval = target_timer();
#else
	new_tmrval = tmr->tmrval + 1;
#endif

	/* Record how many ticks elapsed since the last call to msecElapsed
	 * and add that value to the total number of ticks that have elapsed.
	 */
	elapsed = new_tmrval - tmr->tmrval;
	new_elapsed_low = tmr->elapsed_low + elapsed;

	if (new_elapsed_low < tmr->elapsed_low)
		tmr->elapsed_high++;

	/* If the total elapsed number of ticks exceeds the timeout number
	 * of ticks, then we can return 1 to indicate that the requested
	 * amount of time has elapsed.  Otherwise, we record the values and
	 * return 0.
	 */
	if ((tmr->elapsed_high >= tmr->timeout_high) &&
		(new_elapsed_low >= tmr->timeout_low)) {
		tmr->tmrflags |= TIMEOUT_OCCURRED;
		return(1);
	}
	
	tmr->tmrval = new_tmrval;
	tmr->elapsed_low = new_elapsed_low;
	return(0);
}

/* msecRemainging():
 * Used to query how many milliseconds were left (if any) in the timeout.
 */
ulong
msecRemaining(struct elapsed_tmr *tmr)
{
	ulong high, low, msectot, leftover, divisor;

	if (ELAPSED_TIMEOUT(tmr))
		return(0);
	
	high = tmr->timeout_high - tmr->elapsed_high;
	low = tmr->timeout_low - tmr->elapsed_low;

	msectot = leftover = 0;

#if INCLUDE_HWTMR
	divisor = (ulong)TIMER_TICKS_PER_MSEC;
#else
	divisor = (ulong)LOOPS_PER_MILLISECOND;
#endif

	while(1) {
		while (low > divisor) {
			msectot++;
			low -= divisor;
		}
		leftover += low;
		if (high == 0)
			break;
		else {
			high--;
			low = 0xffffffff;
		}
	} 

	while(leftover > divisor) {
		msectot++;
		low -= divisor;
	}
	return(msectot);
}

/* monDelay():
 * Delay for specified number of milliseconds.
 * Refer to msecElapsed() description for a discussion on the
 * accuracy of this delay.
 */
void
monDelay(int milliseconds)
{
	struct elapsed_tmr tmr;

#if INCLUDE_ENETCORE
	if (EtherIsActive)
		milliseconds >>= 2;
#endif
	if (milliseconds <= 0)
		milliseconds = 1;
		
	startElapsedTimer(&tmr,milliseconds);
	while(!msecElapsed(&tmr)) {
		WATCHDOG_MACRO;
		pollethernet();
	}
}

#if !INCLUDE_HWTMR
void
tickTest(unsigned long val)
{
	if (val > 0)
		lpm = val;
	if (lpm == 0)
		lpm = LOOPS_PER_MILLISECOND;

	printf("LPM %d: should be one dot per second...\n",lpm);
	while(!gotachar()) {
		putchar('.');
		monDelay(1000);
	}
}
#endif
