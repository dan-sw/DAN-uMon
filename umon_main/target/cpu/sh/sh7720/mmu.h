/* mmu.h:
 *	Memory management unit (MMU) defines.
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
#ifndef __SH7720_MMU_H
#define __SH7720_MMU_H

/* MMU registers */
#define MMUCR	((volatile unsigned long *)(0xFFFFFFE0))
#define PTEH	((volatile unsigned long *)(0xFFFFFFF0))
#define PTEL	((volatile unsigned long *)(0xFFFFFFF4))
#define TTB	((volatile unsigned long *)(0xFFFFFFF8))

#endif	/* __SH7720_MMU_H */

