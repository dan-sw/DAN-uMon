/* sim.h:
 *	SIM card module defines.
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
#ifndef __SH7720_SIM_H
#define __SH7720_SIM_H

/* SIM registers */
#define SCSMR	((volatile unsigned char  *)(0xA4490000))
#define SCBRR	((volatile unsigned char  *)(0xA4490002))
#define SCSCR	((volatile unsigned char  *)(0xA4490004))
#define SCTDR	((volatile unsigned char  *)(0xA4490006))
#define SCSSR	((volatile unsigned char  *)(0xA4490008))
#define SCRDR	((volatile unsigned char  *)(0xA449000A))
#define SCSCMR	((volatile unsigned char  *)(0xA449000C))
#define SCSC2R	((volatile unsigned char  *)(0xA449000E))
#define SCWAIT	((volatile unsigned short *)(0xA4490010))
#define SCGRD	((volatile unsigned char  *)(0xA4490012))
#define SCSMPL	((volatile unsigned short *)(0xA4490014))

#endif	/* __SH7720_SIM_H */

