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

#define	MMU_SECT_DESC_NUM		(4096)
#define	MMU_SECT_MEM_SIZE		(0x100000)
#define	MMU_DESC_TYPE_SECTION	(2)
#define MMU_DESC_DEFAULT		(0x00000C16)
#define MMU_DESC_ADDR_SHIFT		(20)
#define MMU_DESC_ADDR_MASK		(0xFFF00000)
#define MMU_DESC_CACHE_MASK		(0x00000008)
#define MMU_DESC_CACHE_MODE		(0x00000004)
#define MMU_DESC_AP_SHIFT		(10)
#define MMU_DESC_AP_MASK		(0x00000C00)
#define MMU_DOMAIN_MASK			(0x000001E0)
#define MMU_DOMAIN_SHIFT		(5)

extern unsigned CPU_SystemClock;		// System clock in Hz
extern unsigned CPU_BusClock;			// System Bus clock in Hz

void CPU_Init();
void CPU_WaitMicroSeconds (unsigned microsec);
unsigned CPU_GetCurrentClockCount();
void CPU_WaitCycles (unsigned cycles);



/**********************************************************************\
                            Inline functions
\**********************************************************************/

// Drains the write buffer
static inline void PLATFORM_write_buffer_drain(void)
{
	register int sbz = 0;
	__asm__ volatile ("MCR p15,0,%0,c7,c10,4" : : "r"(sbz));
}

// Invalidate TLBs
static inline void PLATFORM_MMU_invalidate
(
	void
)
{
	__asm__ volatile
	(
		"MOV r0, #0"					"\n\t"	/* Put 0 in the register    */
		"MCR p15,0,r0,c8,c7,0"			"\n\t"	/* Invalidate TLB           */
		 : : :"r0"
	);
}

// Prefetch the code cache line (address is aligned)
static inline void PLATFORM_cacheline_prefetch(volatile void *address)
{
	__asm__ volatile ("MCR p15,0,%0,c7,c13,1" : : "r"(address));
}

// Invalidate the data cache line (address is aligned)
static inline void PLATFORM_cacheline_invalidate(volatile void *address)
{
	__asm__ volatile ("MCR p15,0,%0,c7,c6,1" : : "r"(address));
}

// Clean the data cache line (address is aligned)
static inline void PLATFORM_cacheline_clean(volatile void *address)
{
	__asm__ volatile ("MCR p15,0,%0,c7,c10,1" : : "r"(address));
}

// Clean and invalidate the data cache line (address is aligned)
static inline void PLATFORM_cacheline_clean_invalidate(volatile void *address)
{
	__asm__ volatile ("MCR p15,0,%0,c7,c14,1" : : "r"(address));
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
	char	*address = (void *)(((uint32)start & CACHE_LINE_ADR_MASK));

	/* Fixup len due to cache line alignment */
	len += ((uint32)start & ~CACHE_LINE_ADR_MASK);

	PLATFORM_cache_control_cacheline_aligned(address, len, opcode);
}

static inline void PLATFORM_posted_write_on(void)
{
	// Nothing to do ,switched on automatically
}

#endif // _CPU_H
