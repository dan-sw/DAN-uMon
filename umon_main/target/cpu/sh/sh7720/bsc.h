/* bsc.h:
 *	Bus state controller (BSC) defines.
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
#ifndef __SH7720_BSC_H
#define __SH7720_BSC_H

/* BSC registers */
#define CMNCR		((volatile unsigned long *)(0xA4FD0000))
#define CS0BCR		((volatile unsigned long *)(0xA4FD0004))
#define CS2BCR		((volatile unsigned long *)(0xA4FD0008))
#define CS3BCR		((volatile unsigned long *)(0xA4FD000C))
#define CS4BCR		((volatile unsigned long *)(0xA4FD0010))
#define CS5ABCR		((volatile unsigned long *)(0xA4FD0014))
#define CS5BBCR		((volatile unsigned long *)(0xA4FD0018))
#define CS6ABCR		((volatile unsigned long *)(0xA4FD001C))
#define CS6BBCR		((volatile unsigned long *)(0xA4FD0020))
#define CS0WCR		((volatile unsigned long *)(0xA4FD0024))
#define CS2WCR		((volatile unsigned long *)(0xA4FD0028))
#define CS3WCR		((volatile unsigned long *)(0xA4FD002C))
#define CS4WCR		((volatile unsigned long *)(0xA4FD0030))
#define CS5AWCR		((volatile unsigned long *)(0xA4FD0034))
#define CS5BWCR		((volatile unsigned long *)(0xA4FD0038))
#define CS6AWCR		((volatile unsigned long *)(0xA4FD003C))
#define CS6BWCR		((volatile unsigned long *)(0xA4FD0040))
#define SDCR		((volatile unsigned long *)(0xA4FD0044))
#define RTCSR		((volatile unsigned long *)(0xA4FD0048))
#define RTCNT		((volatile unsigned long *)(0xA4FD004C))
#define RTCOR		((volatile unsigned long *)(0xA4FD0050))
#define SDMR2		0xA4FD4000
#define SDMR3		0xA4FD5000

#endif	/* __SH7720_BSC_H */

