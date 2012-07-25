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
/*-----------------------------------------------------------
 * TMR.c
 *
 * This file implements the HW timer driver.
 *
 * Design Art Networks Copyright (c) 2012
 *
 * OWNER:                     IgorL
 *
 *-----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------
 * Include section
 * -----------------------------------------------------------
 */

#include "config.h"
#include "stddefs.h"
#include "genlib.h"
#include "common.h"
#include "CGEN.h"
#include "tmr.h"


/*
 * -----------------------------------------------------------
 * MACRO (define) section
 * -----------------------------------------------------------
 */

#define DAN_HW_TIMER0_BASE  				 	(0xE5730000)
#define dw_timers_if_TIMER1CURRENTVAL_OFFSET 	(0x4)


/*
 * -----------------------------------------------------------
 * Type definition section
 * -----------------------------------------------------------
 */
/*
 * -----------------------------------------------------------
 * Global prototypes section
 * -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------
 * Local prototypes section
 * -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------
 * Global data section
 * -----------------------------------------------------------
 */


/*
 * -----------------------------------------------------------
 * Local (static) data section
 * -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------
 * Local (static) and inline functions section
 * -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------
 * Global functions section
 * -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------
 * Function:	TMR_init
 * Description:	Timer initialization
 * Input:		None
 * Output:		None
 * -----------------------------------------------------------
 */

void TMR_init(void)
{
	// initialization wallclock
    //Set the load count to all 1's
	RegWrite32(DAN_HW_TIMER0_BASE, 0xFFFFFFFF); 
    //Enable the timer - unmasked int, free run mode, enabled
	RegWrite32((DAN_HW_TIMER0_BASE + 0x08), 0x1);
}

/*
 * -----------------------------------------------------------
 * Function:	TMR_get_clk
 * Description:	Get current clock from HW timer (should be previously started)
 * Input:		None
 * Output:		None
 * -----------------------------------------------------------
 */
uint32 TMR_get_clk(void) 
{
	return (0xFFFFFFFF - RegRead32(DAN_HW_TIMER0_BASE + 
			dw_timers_if_TIMER1CURRENTVAL_OFFSET)); 
}

/*
 * -----------------------------------------------------------
 * Function:	TMR_tick_delay
 * Description:	Delay N-tick of HW timer
 * Input:		ticks number
 * Output:		None
 * -----------------------------------------------------------
 */
void TMR_tick_delay(uint32 tick)  
{
	uint32 ticks_start, ticks_end;

	ticks_start = TMR_get_clk();
	do
	{
		ticks_end = TMR_get_clk(); 
	}
	while ((ticks_end - ticks_start)<=tick); 
}

/*
 * -----------------------------------------------------------
 * Function:	TMR_tick_delay
 * Description:	Delay N usec-tick of HW timer
 * Input:		usec number
 * Output:		None
 * -----------------------------------------------------------
 */
void TMR_usec_delay(uint32 usec)	// ADD ASSERT/PRINT	when (clk_out * usec) wrap
{
	uint32 clk_out;
	
	clk_out = CGEN_PLL0_get_clk(INPUT_CLOCK_HZ);
	TMR_tick_delay((uint32)(clk_out/1000000/2) * usec);		
}
