/* afeif.h:
 *	Analog front end interface (AFEIF) defines.
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
#ifndef __SH7720_AFEIF_H
#define __SH7720_AFEIF_H

/* AFEIF registers */
#define ACTR1	((volatile unsigned short *)(0xA44E0180))
#define ACTR2	((volatile unsigned short *)(0xA44E0182))
#define ASTR1	((volatile unsigned short *)(0xA44E0184))
#define ASTR2	((volatile unsigned short *)(0xA44E0186))
#define MRCR	((volatile unsigned short *)(0xA44E0188))
#define MPCR	((volatile unsigned short *)(0xA44E018A))
#define DPNQ	((volatile unsigned short *)(0xA44E018C))
#define RCNT	((volatile unsigned short *)(0xA44E018E))
#define ACDR	((volatile unsigned short *)(0xA44E0190))
#define ASDR	((volatile unsigned short *)(0xA44E0192))
#define TDFP	((volatile unsigned short *)(0xA44E0194))
#define RDFP	((volatile unsigned short *)(0xA44E0198))

#endif	/* __SH7720_AFEIF_H */

