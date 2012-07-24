/* cpg.h:
 *	Clock pulse generator (CPG) defines.
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
#ifndef __SH7720_CPG_H
#define __SH7720_CPG_H

/* CPG registers */
#define UCLKCR_R	((volatile const unsigned char  *)(0xA40A0008))
#define UCLKCR_W	((volatile unsigned short *)(0xA40A0008))
enum
{
	UCLKCR_USSCS		= 0xe0,
	UCLKCR_USSCS_SHIFT	= 5,
	UCLKCR_USSCS_EXTAL_USB	= 0xc0,
	UCLKCR_USSCS_CRYSTAL	= 0xe0,
	UCLKCR_USSTB		= 0x10,
	UCLKCR_STOP		= 0x00,
	UCLKCR_OPERATE		= 0x10,
	/* write operations have to combine this value using binary OR */
	UCLKCR_WRITE               = 0xa500,
};

#define FRQCR		((volatile unsigned short *)(0xA415FF80))
enum
{
	FRQCR_PLL2EN			= 0x8000,
	FRQCR_PLL2EN_OFF		= 0x0000,
	FRQCR_PLL2EN_ON			= 0x8000,
	FRQCR_CKOEN			= 0x1000,
	FRQCR_CKOEN_CKIO_LOW		= 0x0000,
	FRQCR_CKOEN_CLOCK_OUTPUT	= 0x1000,
	FRQCR_STC			= 0x0300,
	FRQCR_STC_SHIFT			= 8,
	FRQCR_IFC			= 0x0030,
	FRQCR_IFC_SHIFT			= 4,
	FRQCR_PFC			= 0x0007,
	FRQCR_PFC_ONE			= 0x0000,
	FRQCR_PFC_ONE_HALF		= 0x0001,
	FRQCR_PFC_ONE_THIRD		= 0x0002,
	FRQCR_PFC_ONE_QUARTER		= 0x0003,
	FRQCR_PFC_ONE_SIXTH		= 0x0004,
};

#endif	/* __SH7720_CPG_H */

