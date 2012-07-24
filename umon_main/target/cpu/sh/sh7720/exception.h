/* exception.h:
 * 	Exception handling defines.
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
#ifndef __SH7720_EXCEPTION_H
#define __SH7720_EXCEPTION_H

/* Exception */
#define INTEVT2		((volatile unsigned long *)(0xA4000000))
#define TRA		((volatile unsigned long *)(0xFFFFFFD0))
#define EXPEVT		((volatile unsigned long *)(0xFFFFFFD4))
#define INTEVT		((volatile unsigned long *)(0xFFFFFFD8))
#define TEA		((volatile unsigned long *)(0xFFFFFFFC))

#endif	/* __SH7720_EXCEPTION_H */
