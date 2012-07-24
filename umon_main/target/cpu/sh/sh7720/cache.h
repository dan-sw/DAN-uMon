/* cache.h:
 *	Header file for the cache control register addresses and
 *	the cache control register bit definitions.
 *	
 *	General notice:
 *	This code is part of a boot-monitor package developed as a generic base
 *	platform for embedded system designs.  As such, it is likely to be
 *	distributed to various projects beyond the control of the original
 *	author.  Please notify the author of any enhancements made or bugs found
 *	so that all may benefit from the changes.  In addition, notification back
 *	to the author will allow the new user to pick up changes that may have
 *	been made by other users after this version of the code was distributed.
 *
 *	Author:	Torsten Tetz, Bosch Security Systems
 *	        Mark Jonas, Bosch Security Systems, mark.jonas@de.bosch.com
 */
#ifndef __SH7720_CACHE_H
#define __SH7720_CACHE_H

/* Cache controll register 1. */
#define CCR1	((volatile unsigned long *)(0xFFFFFFEC))
enum
{
	CCR1_CF		= 0x00000008,	/* Flush all cache entries. */
	CCR1_CB		= 0x00000004,	/* Write-Back (P1) */
	CCR1_WT		= 0x00000002,	/* Write-Through (P0, U0, P3) */
	CCR1_CE		= 0x00000001	/* Cache enable */
};

/* Cache control register 2. */
#define CCR2	((volatile unsigned long *)(0xA40000B0))

/* Cache controll register 3. */
#define CCR3	((volatile unsigned long *)(0xA40000B4))
enum
{
	CCR3_CSIZE_32		= 0x20000,	/* 32-kbyte cache */
	CCR3_CSIZE_16		= 0x10000	/* 16-kbyte cache */
};

#endif	/* __SH7720_CACHE_H */

