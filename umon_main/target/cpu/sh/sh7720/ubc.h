/* ubc.h:
 *	User break controller defines.
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
#ifndef	__SH7720_UBC_H
#define	__SH7720_UBC_H

/* UBC registers */
#define BDRB	((volatile unsigned long  *)(0xA4FFFF90))
#define BDMRB	((volatile unsigned long  *)(0xA4FFFF94))
#define BRCR	((volatile unsigned long  *)(0xA4FFFF98))
#define BETR	((volatile unsigned short *)(0xA4FFFF9C))
#define BARB	((volatile unsigned long  *)(0xA4FFFFA0))
#define BAMRB	((volatile unsigned long  *)(0xA4FFFFA4))
#define BBRB	((volatile unsigned short *)(0xA4FFFFA8))
#define BRSR	((volatile unsigned long  *)(0xA4FFFFAC))
#define BARA	((volatile unsigned long  *)(0xA4FFFFB0))
#define BAMRA	((volatile unsigned long  *)(0xA4FFFFB4))
#define BBRA	((volatile unsigned short *)(0xA4FFFFB8))
#define BRDR	((volatile unsigned long  *)(0xA4FFFFBC))
#define BASRA	((volatile unsigned char  *)(0xFFFFFFE4))
#define BASRB	((volatile unsigned char  *)(0xFFFFFFE8))

#endif	/* __SH7720_UBC_H */

