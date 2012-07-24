/* lcdc.h:
 *	LCD controller defines.	
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
#ifndef __SH7720_LCDC_H
#define __SH7720_LCDC_H

/* LCDC registers */
#define LDPR00		((volatile unsigned long  *)(0xA4400000))
#define LDICKR		((volatile unsigned short *)(0xA4400400))
#define LDMTR		((volatile unsigned short *)(0xA4400402))
#define LDDFR		((volatile unsigned short *)(0xA4400404))
#define LDSMR		((volatile unsigned short *)(0xA4400406))
#define LDSARU		((volatile unsigned long  *)(0xA4400408))
#define LDSARL		((volatile unsigned long  *)(0xA440040C))
#define LDLAOR		((volatile unsigned short *)(0xA4400410))
#define LDPALCR		((volatile unsigned short *)(0xA4400412))
#define LDHCNR		((volatile unsigned short *)(0xA4400414))
#define LDHSYNR		((volatile unsigned short *)(0xA4400416))
#define LDVDLNR		((volatile unsigned short *)(0xA4400418))
#define LDVTLNR		((volatile unsigned short *)(0xA440041A))
#define LDVSYNR		((volatile unsigned short *)(0xA440041C))
#define LDACLNR		((volatile unsigned short *)(0xA440041E))
#define LDINTR		((volatile unsigned short *)(0xA4400420))
#define LDPMMR		((volatile unsigned short *)(0xA4400424))
#define LDPSPR		((volatile unsigned short *)(0xA4400426))
#define LDCNTR		((volatile unsigned short *)(0xA4400428))
#define LDUINTR		((volatile unsigned short *)(0xA4400434))
#define LDUINTLNR	((volatile unsigned short *)(0xA4400436))
#define LDLIRNR		((volatile unsigned short *)(0xA4400440))

#endif	/* __SH7720_LCDC_H */

