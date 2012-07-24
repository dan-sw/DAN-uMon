/* cache.c:
 *
 * General notice:
 * This code is part of a boot-monitor package developed as a generic base
 * platform for embedded system designs.  As such, it is likely to be
 * distributed to various projects beyond the control of the original
 * author.  Please notify the author of any enhancements made or bugs found
 * so that all may benefit from the changes.  In addition, notification back
 * to the author will allow the new user to pick up changes that may have
 * been made by other users after this version of the code was distributed.
 *
 * Portions of this code were yanked from IBM source...
 *
 * This source code has been made available to you by IBM on an AS-IS
 * basis.  Anyone receiving this source is licensed under IBM
 * copyrights to use it in any way he or she deems fit, including
 * copying it, modifying it, compiling it, and redistributing it either
 * with or without modifications.  No license under IBM patents or
 * patent applications is to be implied by the copyright license.
 *
 * Any user of this software should understand that IBM cannot provide
 * technical support for this software and will not be responsible for
 * any consequences resulting from the use of this software.
 *
 * Any person who transfers this source code or any derivative work
 * must include the IBM copyright notice, this paragraph, and the
 * preceding two paragraphs in the transferred software.
 *
 * COPYRIGHT   I B M   CORPORATION 1992, 1996
 * LICENSED MATERIAL  -  PROGRAM PROPERTY OF I B M
 *
 * Author:  Ed Sutter
 * email:   esutter@lucent.com
 * phone:   908-582-2351
 *
 */
#include "config.h"
#include "stddefs.h"
#include "cache.h"
#include "arch_ppc.h"

#define SECTOR_SIZE	32		/* 32 byte cache line */
#define SECTOR_MASK	0x1F

int
flush405Dcache(char *address, int count)
{
	unsigned int lcl_addr = (unsigned int) address;
	unsigned int lcl_target;

	/* promote to nearest cache sector
	 */
	lcl_target =  (lcl_addr + count + SECTOR_SIZE - 1) & ~SECTOR_MASK;
	lcl_addr &= ~SECTOR_MASK;
	while (lcl_addr != lcl_target) {
		ppcDcbf((void *)lcl_addr);
		lcl_addr += SECTOR_SIZE;
	}
	ppcSync();
	return(0);
}


int
invalidate405Icache(char *addr, int size)
{
	/* The ICCCI instruction on the PPC405 invalidates the entire I-cache.
	 */
    asm("iccci   %r0,%r0");
	return(0);
}

void
cacheInitForTarget()
{
	ppcMticcr(0x80000001);
	dcacheFlush = flush405Dcache;
	icacheInvalidate = invalidate405Icache;
}
