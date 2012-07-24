#ifndef _TIMER_H_
#define _TIMER_H_

struct elapsed_tmr {
	unsigned long start;
	unsigned long tmrval;

	unsigned long elapsed_low;
	unsigned long elapsed_high;

	unsigned long timeout_low;
	unsigned long timeout_high;

	unsigned long tmrflags;
};

#define ELAPSED_TIMEOUT(tmr)	((tmr)->tmrflags & TIMEOUT_OCCURRED)

/* Timer flags:
 */
#define HWTMR_ENABLED		(1 << 0)
#define TIMEOUT_OCCURRED	(1 << 1)

extern unsigned long target_timer(void);
extern void startElapsedTimer(struct elapsed_tmr *tmr,long timeout);
extern int msecElapsed(struct elapsed_tmr *tmr);
extern unsigned long msecRemaining(struct elapsed_tmr *tmr);
extern void monDelay(int msecs);

#endif
