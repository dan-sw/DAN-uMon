/* wdt.h:
 *	Watchdog timer (WDT) defines.
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
#ifndef __SH7720_WDT_H
#define __SH7720_WDT_H

/* WDT registers 
 * The watchdog timer counter (WTCNT) and watchdog timer control/status register
 * (WTCSR) are more difficult to write to than other registers.
 * These registers must be written by a word transfer instruction. They cannot 
 * be written by a byte or longword transfer instruction. When writing to WTCNT,
 * set the upper byte to H'5A and transfer the lower byte as the write data.
 * When writing to WTCSR, set the upper byte to H'A5 and transfer the lower 
 * byte as the write data. This transfer procedure writes the lower byte data 
 * to WTCNT or WTCSR.
 */

/* Watchdog timer counter. */
#define WTCNT_R		((volatile const unsigned char *)( 0xA415FF84 ))
#define WTCNT_W		((volatile unsigned short *)( 0xA415FF84 ))
enum
{
	WTCNT_SECURITY		= 0x5A00,
	WTCNT_CNT			= 0x00FF
};

/* Watchdog timer control/status register. */
#define WTCSR_R		((volatile const unsigned char *)( 0xA415FF86 ))
#define WTCSR_W		((volatile unsigned short *)( 0xA415FF86 ))
enum
{
	WTCSR_SECURITY	= 0xA500,
	WTCSR_TME	= 0x0080,
	WTCSR_WT_IT	= 0x0040,
	WTCSR_RSTS	= 0x0020,
	WTCSR_WOVF	= 0x0010,
	WTCSR_IOVF	= 0x0008,
	WTCSR_CKS	= 0x0007,
	WTCSR_CKS_1	= 0x0000,
	WTCSR_CKS_4	= 0x0001,
	WTCSR_CKS_16	= 0x0002,
	WTCSR_CKS_32	= 0x0003,
	WTCSR_CKS_64	= 0x0004,
	WTCSR_CKS_256	= 0x0005,
	WTCSR_CKS_1024	= 0x0006,
	WTCSR_CKS_4096	= 0x0007
};

#endif	/* __SH7720_WDT_H */
