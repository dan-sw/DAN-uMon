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

#ifdef DAN2400
unsigned CPU_SystemClock = 90909091 * 2;			// ~182 MHz
#else
unsigned CPU_SystemClock;
#endif
unsigned CPU_BusClock;

char CPU_name_buf[64];
const char *CPU_name = CPU_name_buf;

extern void cache_on_asm(void);
extern uint32 *core_mmu_init(uint32 *TLBs);
extern void cache_off_asm(void);

#define	ROUND_UP_DIV(x,y)		(((x) + (y) - 1) / (y))

#ifdef USE_NPU_CB_ONLY
#define USE_MMU
#define DAN3400_ARM_CACHE
#undef DDR_CLOCK
#define DDR_CLOCK 0
#else
#ifdef DAN3400_ARM_CACHE
#define USE_MMU
#endif
#endif

#ifdef USE_MMU
#define	TTB_ADDR_DEFAULT	0x5057C000
static uint32 *TTB;
#endif

#ifdef USE_NPU_CB_ONLY
/*
 *===========================================================================
 *                      CPU_mmu_memory_map
 *===========================================================================
 * Description: MMU mapping of the memory region
 * Returns: 	NONE
 * Called:		Init, LISR, HISR, thread
 */
static void CPU_mmu_memory_map
(
	void				*virt_addr,	/* The address in the virtual memory    */
	void				*phys_addr,	/* The address in the physical memory   */
	int					len			/* The memory length to map             */
)
{
	UINT32		start_idx = (UINT32)virt_addr / MMU_SECT_MEM_SIZE;
	UINT32		end_idx = start_idx + ROUND_UP_DIV(len, MMU_SECT_MEM_SIZE) - 1;
	UINT32		idx = start_idx;

	/* Flush and invalidate cache before remap */
	PLATFORM_cache_control(virt_addr, len,
									CACHE_OP_FLUSH | CACHE_OP_INVALIDATE);
	do
	{
		TTB[idx] &= ~MMU_DESC_ADDR_MASK;
		TTB[idx] |= (UINT32)phys_addr & MMU_DESC_ADDR_MASK;
		phys_addr = (void *)((unsigned)phys_addr + MMU_SECT_MEM_SIZE);
	} while (++idx <= end_idx);
	/* Flush the table itself */
	PLATFORM_cache_control(TTB + start_idx,
						(end_idx - start_idx + 1) * sizeof(unsigned),
						CACHE_OP_FLUSH);
	/* Wait for write buffer drain */
    PLATFORM_write_buffer_drain();
	/* Invalidate TLBs */
	PLATFORM_MMU_invalidate();
}
#endif

#ifdef DAN3400_ARM_CACHE
static void CPU_cache_region_enable
(
	void				*start,		/* The start address of the memory      */
	int					len			/* The memory length to enable cache    */
)
{
	uint32		start_idx = (UINT32)start / MMU_SECT_MEM_SIZE;
	uint32		end_idx = start_idx + ROUND_UP_DIV(len, MMU_SECT_MEM_SIZE) - 1;
	uint32		idx = start_idx;

	do
	{
		TTB[idx] |= MMU_DESC_CACHE_MASK;
	} while (++idx <= end_idx);
	/* Flush the table itself */
	PLATFORM_cache_control(TTB + start_idx,
						(end_idx - start_idx + 1) * sizeof(unsigned),
						CACHE_OP_FLUSH);
	/* Wait for write buffer drain */
    PLATFORM_write_buffer_drain();
	/* Invalidate TLBs */
	PLATFORM_MMU_invalidate();
}
#endif

void CPU_cache_on()
{
	cache_off_asm();

#ifdef USE_MMU
	TTB = core_mmu_init((void *)TTB_ADDR_DEFAULT);
#endif

#ifdef USE_NPU_CB_ONLY
	CPU_mmu_memory_map((void *)0x00000000, (void *)0x60400000, 0x00100000);
	CPU_mmu_memory_map((void *)0x80000000, (void *)0x60400000, 0x00100000);
	CPU_mmu_memory_map((void *)0x00800000, (void *)0x40400000, 0x00200000);
	CPU_mmu_memory_map((void *)0x80800000, (void *)0x40400000, 0x00200000);
#endif

#ifdef DAN3400_ARM_CACHE
	if (CGEN_is_chip_revision(CGEN_CHIP_REVISION_A))
		return;
	
#ifndef USE_NPU_CB_ONLY
	CPU_cache_region_enable((void *)0x00000000, 0x20000000); // DDR0
	CPU_cache_region_enable((void *)0x20000000, 0x20000000); // DDR1
#endif
	CPU_cache_region_enable((void *)0x40000000, 0x00800000); // TX CB
	CPU_cache_region_enable((void *)0x50000000, 0x00800000); // RX CB
	CPU_cache_region_enable((void *)0x60000000, 0x00800000); // NPU CB

	CPU_cache_region_enable((void *)0x54110000, 0x100000); // RX_CPU0 code
	CPU_cache_region_enable((void *)0x54310000, 0x100000); // RX_CPU1 code
	CPU_cache_region_enable((void *)0x54410000, 0x100000); // RX_DSP0 code
	CPU_cache_region_enable((void *)0x54510000, 0x100000); // RX_DSP1 code
	CPU_cache_region_enable((void *)0x54610000, 0x100000); // RX_DSP2 code
	CPU_cache_region_enable((void *)0x54710000, 0x100000); // RX_DSP3 code
	CPU_cache_region_enable((void *)0x44010000, 0x100000); // TX_CPU0 code
	CPU_cache_region_enable((void *)0x44110000, 0x100000); // TX_CPU1 code
	CPU_cache_region_enable((void *)0x44210000, 0x100000); // TX_CPU2 code
	CPU_cache_region_enable((void *)0x44310000, 0x100000); // TX_CPU3 code
	CPU_cache_region_enable((void *)0x44410000, 0x100000); // TX_DSP0 code
	CPU_cache_region_enable((void *)0x44510000, 0x100000); // TX_DSP1 code

	CPU_cache_region_enable((void *)0x54100000, 0x100000); // RX_CPU0 data
	CPU_cache_region_enable((void *)0x54300000, 0x100000); // RX_CPU1 data
	CPU_cache_region_enable((void *)0x54400000, 0x100000); // RX_DSP0 data
	CPU_cache_region_enable((void *)0x54500000, 0x100000); // RX_DSP1 data
	CPU_cache_region_enable((void *)0x54600000, 0x100000); // RX_DSP2 data
	CPU_cache_region_enable((void *)0x54700000, 0x100000); // RX_DSP3 data
	CPU_cache_region_enable((void *)0x44000000, 0x100000); // TX_CPU0 data
	CPU_cache_region_enable((void *)0x44100000, 0x100000); // TX_CPU1 data
	CPU_cache_region_enable((void *)0x44200000, 0x100000); // TX_CPU2 data
	CPU_cache_region_enable((void *)0x44300000, 0x100000); // TX_CPU3 data
	CPU_cache_region_enable((void *)0x44400000, 0x100000); // TX_DSP0 data
	CPU_cache_region_enable((void *)0x44500000, 0x100000); // TX_DSP1 data

	cache_on_asm();
#endif
}
#ifndef DAN3400_ARM_CACHE
void cacheInitForTarget()
{
}
#endif

void CPU_WaitMicroSeconds (unsigned microsec)
{
	unsigned clock0 = CPU_GetCurrentClockCount();
	unsigned clocks2wait = (CPU_SystemClock / 1000000) * microsec;
	while (CPU_GetCurrentClockCount() - clock0 < clocks2wait);
}

unsigned CPU_GetCurrentClockCount()
{
	static volatile unsigned clock_count = 0;
	return (clock_count++);
}

// Wait number of loops (cycles)
void CPU_WaitCycles (unsigned cycles)
{
	volatile unsigned iter = cycles;
	while (iter--);
}

void CPU_Init()
{
#ifndef DAN2400
	CPU_SystemClock = CGEN_PLL0_get_clk(INPUT_CLOCK_HZ);// Input clock in Hz
#endif
	CPU_BusClock = CPU_SystemClock / 2;					// Half system clock
	sprintf(CPU_name_buf, "DAN3400 (ARM: %u MHz, DDR: %u MHz)", 
			CPU_SystemClock / 1000000, DDR_CLOCK);
}
