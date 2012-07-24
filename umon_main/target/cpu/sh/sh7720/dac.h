/* dac.h:
 *	D/A converter (DAC) defines.
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
#ifndef __SH7720_DAC_H
#define __SH7720_DAC_H

/* DAC registers */
#define DADR0		((volatile unsigned char *)(0xA44D0000))
#define DADR1		((volatile unsigned char *)(0xA44D0002))
#define DACR		((volatile unsigned char *)(0xA44D0004))

#endif	/* __SH7720_DAC_H */

