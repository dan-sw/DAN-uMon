/* pcc.h:
 *	PC card controller (PCC) defines.
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
#ifndef __SH7720_PCC_H
#define __SH7720_PCC_H

/* PCC registers */
#define PCC0ISR		((volatile unsigned char *)(0xA44B0000))
#define PCC0GCR		((volatile unsigned char *)(0xA44B0002))
#define PCC0CSCR	((volatile unsigned char *)(0xA44B0004))
#define PCC0CSCIER	((volatile unsigned char *)(0xA44B0006))

#endif	/* __SH7720_PCC_H */
