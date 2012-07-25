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
 Filename   :  cpu.h
 Purpose    :  CPU dependent routines
 Owner		:  Sergey Krasnitsky
 Created    :  11.11.2010
\**********************************************************************/

#include "config.h"
#include "cpu.h"
#include "stdio.h"
#include "common.h"
#include "CGEN.h"
#include "spi.h"

unsigned CPU_SystemClock;
unsigned CPU_BusClock;

char CPU_name_buf[64];
const char *CPU_name = CPU_name_buf;


void CPU_cache_on(void)
{
}

void CPU_WaitMicroSeconds (unsigned microsec)
{
	unsigned clock0 = CPU_GetCurrentClockCount();
	unsigned clocks2wait = CPU_SystemClock / 1000000 * microsec;
	while (CPU_GetCurrentClockCount() - clock0 < clocks2wait);
}

void CPU_Init()
{
#ifdef DEBUG
	SOC_init();
#endif
	CPU_SystemClock = CGEN_PLL0_get_clk(INPUT_CLOCK_HZ);// Input clock in Hz
	CPU_BusClock = CPU_SystemClock / 2;					// Half system clock
	sprintf(CPU_name_buf, "DAN3400 (DSP: %u MHz, DDR: %u MHz)", 
			CPU_SystemClock / 1000000, DDR_CLOCK);
}
