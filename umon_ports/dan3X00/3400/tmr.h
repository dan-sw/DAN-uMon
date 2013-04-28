/* Copyright 2013, Qualcomm Atheros, Inc. */
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
/*-----------------------------------------------------------
 * TMR.c
 *
 * This file implements the HW timer driver.
 *
 * Design Art Networks Copyright (c) 2012
 *
 * OWNER:                     Igor
 *-----------------------------------------------------------
 */

#ifndef _TMR_H
#define _TMR_H


#define DAN_HW_TIMER0_BASE  				 	0xE5730000
#define dw_timers_if_TIMER1CURRENTVAL_OFFSET 	0x4


static inline void TMR_init(void)
{
	// Initialization wall-clock
	RegWrite32 (DAN_HW_TIMER0_BASE, 0xFFFFFFFF);	// Set the load count to all 1's
	RegWrite32 (DAN_HW_TIMER0_BASE + 8, 0x1);		// Enable the timer - unmasked int, free run mode, enabled
}


/* Get current clock from HW timer (should be previously started) */
static inline uint32 TMR_get_clk(void)
{
	return (0xFFFFFFFF - RegRead32(DAN_HW_TIMER0_BASE + dw_timers_if_TIMER1CURRENTVAL_OFFSET));
}


/* Delay N-tick of HW timer */
void TMR_tick_delay(uint32 tick);


/* Delay N usec-tick of HW timer */
void TMR_usec_delay(uint32 usec);



/**********************************************************************\
							Timer functions
\**********************************************************************/

/* Start the timer - print current time to the console each nseconds.
   Stop the timer if nseconds = 0.  */
void TMR_Start(uint32 nseconds);


/* Pinch the timer, must be called at least once in 14 seconds */
void TMR_Pinch();


#endif /* _TMR_H */
