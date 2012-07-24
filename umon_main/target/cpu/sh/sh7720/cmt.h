/* cmt.h:
 *	Compare match timer (CMT) defines.
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
#ifndef __SH7720_CMT_H
#define __SH7720_CMT_H

/* CMT registers */
#define CMSTR		((volatile unsigned short *)(0xA44A0000))
#define CMCSR_0		((volatile unsigned short *)(0xA44A0010))
#define CMCNT_0		((volatile unsigned long  *)(0xA44A0014))
#define CMCOR_0		((volatile unsigned long  *)(0xA44A0018))
#define CMCSR_1		((volatile unsigned short *)(0xA44A0020))
#define CMCNT_1		((volatile unsigned long  *)(0xA44A0024))
#define CMCOR_1		((volatile unsigned long  *)(0xA44A0028))
#define CMCSR_2		((volatile unsigned short *)(0xA44A0030))
#define CMCNT_2		((volatile unsigned long  *)(0xA44A0034))
#define CMCOR_2		((volatile unsigned long  *)(0xA44A0038))
#define CMCSR_3		((volatile unsigned short *)(0xA44A0040))
#define CMCNT_3		((volatile unsigned long  *)(0xA44A0044))
#define CMCOR_3		((volatile unsigned long  *)(0xA44A0048))
#define CMCSR_4		((volatile unsigned short *)(0xA44A0050))
#define CMCNT_4		((volatile unsigned long  *)(0xA44A0054))
#define CMCOR_4		((volatile unsigned long  *)(0xA44A0058))

#endif	/* __SH7720_CMT_H */

