/* iic.h:
 *	IIC bus interface defines.
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
#ifndef __SH7720_IIC_H
#define __SH7720_IIC_H

/* IIC registers */
#define ICCR1		((volatile unsigned char *)(0xA4470000))
#define ICCR2		((volatile unsigned char *)(0xA4470004))
#define ICMR		((volatile unsigned char *)(0xA4470008))
#define ICIER		((volatile unsigned char *)(0xA447000C))
#define ICSR		((volatile unsigned char *)(0xA4470010))
#define IIC_SAR		((volatile unsigned char *)(0xA4470014))
#define ICDRT		((volatile unsigned char *)(0xA4470018))
#define ICDRR		((volatile unsigned char *)(0xA447001C))

#endif	/* __SH7720_IIC_H */
