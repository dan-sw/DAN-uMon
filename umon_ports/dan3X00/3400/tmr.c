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
 * OWNER:                     IgorL
 *-----------------------------------------------------------
 */

#include "config.h"
#include "stddefs.h"
#include "genlib.h"
#include "common.h"
#include "CGEN.h"
#include "cpu.h"
#include "tmr.h"


void TMR_tick_delay (uint32 tick)
{
	uint32 ticks_start, ticks_end;

	ticks_start = TMR_get_clk();
	do
	{
		ticks_end = TMR_get_clk(); 
	}
	while ((ticks_end - ticks_start)<=tick); 
}


void TMR_usec_delay(uint32 usec)	// Add ASSERT/PRINT	when (clk_out * usec) wrap
{
	uint32 clk_out = CGEN_PLL0_get_clk(INPUT_CLOCK_HZ);
	TMR_tick_delay(clk_out/2/1000000 * usec);	// TODO: check it, clk_out/2 = CPU_BusClock
}



/**********************************************************************\
						Timer functions
\**********************************************************************/

uint64 TMR_AbsTick;
uint32 TMR_LastTick;
uint32 TMR_Interval;



void TMR_Start(uint32 nseconds)
{
	TMR_Interval = nseconds * CPU_BusClock;
	TMR_LastTick = TMR_get_clk();
	TMR_AbsTick  = 0;
}


void TMR_Pinch()
{
	if (!TMR_Interval)
		return;

	uint32 curtick = TMR_get_clk();
	uint32 diftick = curtick - TMR_LastTick;

	if (diftick >= TMR_Interval) {
		TMR_AbsTick += diftick;
		TMR_LastTick = curtick;

		uint32 cur = TMR_AbsTick / CPU_BusClock;
		uint32 s = cur % 60;	cur /= 60;
		uint32 m = cur % 60;	cur /= 60;
		uint32 h = cur % 24;	cur /= 24;

		printf ("\nTIME: %dd %d:%02d:%02d\n", cur,h,m,s);
	}
}
