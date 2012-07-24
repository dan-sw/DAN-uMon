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
	sprintf(CPU_name_buf, "DAN3400 (CPU: %u MHz, DDR: %u MHz)", 
			CPU_SystemClock / 1000000, DDR_CLOCK);
}
