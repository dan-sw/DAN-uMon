/* pfc.h:
 *	Pin function controller (PFC) defines.
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
#ifndef __SH7720_PFC_H
#define __SH7720_PFC_H

/* PFC registers */
#define PACR	((volatile unsigned short *)(0xA4050100))
#define PBCR	((volatile unsigned short *)(0xA4050102))
#define PCCR	((volatile unsigned short *)(0xA4050104))
#define PDCR	((volatile unsigned short *)(0xA4050106))
#define PECR	((volatile unsigned short *)(0xA4050108))
#define PFCR	((volatile unsigned short *)(0xA405010A))
#define PGCR	((volatile unsigned short *)(0xA405010C))
#define PHCR	((volatile unsigned short *)(0xA405010E))
#define PJCR	((volatile unsigned short *)(0xA4050110))
#define PKCR	((volatile unsigned short *)(0xA4050112))
#define PLCR	((volatile unsigned short *)(0xA4050114))
#define PMCR	((volatile unsigned short *)(0xA4050116))
#define PPCR	((volatile unsigned short *)(0xA4050118))
#define PRCR	((volatile unsigned short *)(0xA405011A))
#define PSCR	((volatile unsigned short *)(0xA405011C))
#define PTCR	((volatile unsigned short *)(0xA405011E))
#define PUCR	((volatile unsigned short *)(0xA4050120))
#define PVCR	((volatile unsigned short *)(0xA4050122))
#define PSELA	((volatile unsigned short *)(0xA4050124))
#define PSELB	((volatile unsigned short *)(0xA4050126))
#define PSELC	((volatile unsigned short *)(0xA4050128))
#define PSELD	((volatile unsigned short *)(0xA405012A))

#endif	/* __SH7720_PFC_H */

