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
/**********************************************************************\
 Library    :  uMON (Micro Monitor)
 Filename   :  cpu.h
 Purpose    :  CPU dependent routines
 Owner		:  Sergey Krasnitsky
 Created    :  11.11.2010
\**********************************************************************/

#ifndef _CPU_H
#define _CPU_H

#define	CACHE_LINE_SIZE		(32)		 // Cache line size
#define CACHE_LINE_ADR_MASK	(0xFFFFFFE0) // Cache line address mask
#define CACHE_OP_FLUSH		(0x00000001) // Bit mask for cache flush operation
#define CACHE_OP_INVALIDATE	(0x00000002) // Bit mask for cache invalidate operation
#define CACHE_OP_PREFETCH	(0x00000004) // Bit mask for cache prefetch operation
#define CACHE_OP_TOUCH		(0x00000008) // Bit mask for cache touch operation

#if UMON_TARGET == UMON_TARGET_WIN
unsigned long __stdcall GetTickCount(void);
#define CPU_GetCurrentClockCount()	GetTickCount()
#else
#include <xtensa/hal.h>
#include <xtensa/tie/xt_timer.h>
#define CPU_GetCurrentClockCount()	((unsigned)XT_RSR_CCOUNT())
#endif


extern unsigned CPU_SystemClock;		// System clock in Hz
extern unsigned CPU_BusClock;			// System Bus clock in Hz

void CPU_Init();
void CPU_WaitMicroSeconds (unsigned microsec);

// Wait number of loops (cycles)
static inline void CPU_WaitCycles (int cycles)
{
	volatile int vcycles = cycles;
	while (vcycles--);
}

/**********************************************************************\
                            Inline functions
\**********************************************************************/

// Drains the write buffer
static inline void PLATFORM_write_buffer_drain(void)
{
#pragma flush
}

// Prefetch the code cache line (address is aligned)
static inline void PLATFORM_cacheline_prefetch(volatile void *address)
{
}

// Invalidate the data cache line (address is aligned)
static inline void PLATFORM_cacheline_invalidate(volatile void *address)
{
#if UMON_TARGET != UMON_TARGET_WIN
	xthal_icache_line_invalidate((void *)address);
	xthal_dcache_line_invalidate((void *)address);
#endif
}

// Clean the data cache line (address is aligned)
static inline void PLATFORM_cacheline_clean(volatile void *address)
{
#if UMON_TARGET != UMON_TARGET_WIN
	xthal_dcache_line_writeback((void *)address);
#endif
}

// Clean and invalidate the data cache line (address is aligned)
static inline void PLATFORM_cacheline_clean_invalidate(volatile void *address)
{
#if UMON_TARGET != UMON_TARGET_WIN
	xthal_dcache_line_writeback_inv((void *)address);
#endif
}

// Perform operations on the CPU cache
static inline void PLATFORM_cache_control_cacheline_aligned
        (
	        void		*start,		// The start address of the memory
	        int			len,		// The memory length to operate cache
	        unsigned	opcode		// Operations to be performed on cache
        )
{
	char *address = (char*) start;

	if (opcode & CACHE_OP_PREFETCH)
	{
		for (; len > 0; len -= CACHE_LINE_SIZE, address += CACHE_LINE_SIZE)
		{
			PLATFORM_cacheline_prefetch(address);
		}
	}
	else if ((opcode & CACHE_OP_FLUSH) && (opcode & CACHE_OP_INVALIDATE))
	{
		for (; len > 0; len -= CACHE_LINE_SIZE, address += CACHE_LINE_SIZE)
		{
			PLATFORM_cacheline_clean_invalidate(address);
		}
	}
	else if (opcode & CACHE_OP_FLUSH)
	{
		for (; len > 0; len -= CACHE_LINE_SIZE, address += CACHE_LINE_SIZE)
		{
			PLATFORM_cacheline_clean(address);
		}
	}
	else if (opcode & CACHE_OP_INVALIDATE)
	{
		for (; len > 0; len -= CACHE_LINE_SIZE, address += CACHE_LINE_SIZE)
		{
			PLATFORM_cacheline_invalidate(address);
		}
	}
	else if (opcode & CACHE_OP_TOUCH)
	{
		for (; len > 0; len -= CACHE_LINE_SIZE, address += CACHE_LINE_SIZE)
		{
			*(volatile char *)address;
		}
	}
}

// Perform operations on the CPU cache
static inline void PLATFORM_cache_control
        (
	        void		*start,		// The start address of the memory
	        int			len,		// The memory length to operate cache
	        unsigned	opcode		// Operations to be performed on cache
        )
{
	char	*address = (void *)(((UINT32)start & CACHE_LINE_ADR_MASK));

	/* Fixup len due to cache line alignment */
	len += ((uint32)start & ~CACHE_LINE_ADR_MASK);

	PLATFORM_cache_control_cacheline_aligned(address, len, opcode);
}

static inline void PLATFORM_posted_write_on(void)
{
	// Switch Posted write on
	*(volatile uint32 *)0xd4900550 = 1;
#pragma flush
}

#endif // _CPU_H
