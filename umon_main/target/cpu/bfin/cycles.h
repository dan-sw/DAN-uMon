/***************************************************************************
 * 
 * Blackfin Cycle Counting Routines...
 * This set of functions are accessible by a 'C' program for counting
 * cycles (i.e. timing the duration of a small block of code).
 *
 * Following is a typical usage example from C:
 * Assume the functions "func()" needs to be timed...
 */
#if 0

#include "cycles.h"

	/* func():
	 * This is some arbitrary function that we want to measure the 
	 * duration of.
	 */
	func()
	{
		blah, blah
	}


	/* func_wrapper():
	 * This is a call to func(), but wrapped with the code that will
	 * print out the duration of the call.
	 * The value of CORECLK_MHZ is defined in cycles.h as the core clock
	 * frequency divided by 1000000.
 	 */
	func_wrapper()
	{
		unsigned long cadjust, hi, lo;

		/* First we determine the overhead of the 'C' calls (stored in
		 * 'cadjust' for use later)...
		 */
		bfin_cycles_init();	
		cadjust = bfin_cycles_lo();
		

		bfin_cycles_init();

		func();
		
		lo = bfin_cycles_lo();
		hi = bfin_cycles_his();
		if (hi)
			mon_printf("Duration: 0x%lx%lx cycles\n",hi,lo,cadjust);
		else
			mon_printf("Duration: %ld uSec\n",(lo-cadjust)/CORECLK_MHZ);
	}
#endif

#define CORECLKFREQ 500000000
#define CORECLK_MHZ	(CORECLKFREQ/1000000)

extern unsigned long bfin_cycles_lo(void);
extern unsigned long bfin_cycles_hi(void);
extern unsigned long bfin_cycles_his(void);
extern unsigned long bfin_cycles_init(void);

/* TIME_THIS_FUNCTION():
 * Useful macro for timing a function using the CPU's cycle counter.
 * If function to be timed is...

	func(a,b,c);

 * then wrap it as follows...

	TIME_THIS_FUNCTION("my_function",func(a,b,c);)

 */

#define TIME_THIS_FUNCTION(NAME,FUNCTION) { \
	volatile unsigned long hi, lo, cadjust; \
	\
	bfin_cycles_init(); \
	cadjust = bfin_cycles_lo(); \
	bfin_cycles_init(); \
	FUNCTION \
	lo = bfin_cycles_lo(); \
	hi = bfin_cycles_his(); \
	if (hi) \
		mon_printf("%s: 0x%lx%lx cycles\n",NAME,hi,lo,cadjust); \
	else \
		mon_printf("%s: %ld uSec (%ld cycles)\n", NAME, \
			(lo-cadjust)/CORECLK_MHZ, (lo-cadjust)); \
	}

