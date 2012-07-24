/* mmc.h:
 *	Multi media card (MMC) defines.
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
#ifndef __SH7720_MMC_H
#define __SH7720_MMC_H

/* MMC registers */
#define CMDR0		((volatile unsigned char  *)(0xA4448000))
#define CMDR1		((volatile unsigned char  *)(0xA4448001))
#define CMDR2		((volatile unsigned char  *)(0xA4448002))
#define CMDR3		((volatile unsigned char  *)(0xA4448003))
#define CMDR4		((volatile unsigned char  *)(0xA4448004))
#define CMDR5		((volatile unsigned char  *)(0xA4448005))
#define CMDSTRT		((volatile unsigned char  *)(0xA4448006))
#define OPCR		((volatile unsigned char  *)(0xA444800A))
#define CSTR		((volatile unsigned char  *)(0xA444800B))
#define INTCR0		((volatile unsigned char  *)(0xA444800C))
#define INTCR1		((volatile unsigned char  *)(0xA444800D))
#define INTSTR0		((volatile unsigned char  *)(0xA444800E))
#define INTSTR1		((volatile unsigned char  *)(0xA444800F))
#define CLKON		((volatile unsigned char  *)(0xA4448010))
#define CTOCR		((volatile unsigned char  *)(0xA4448011))
#define IOMCR		((volatile unsigned char  *)(0xA4448013))
#define TBCR		((volatile unsigned char  *)(0xA4448014))
#define MODER		((volatile unsigned char  *)(0xA4448016))
#define CMDTYR		((volatile unsigned char  *)(0xA4448018))
#define RSPTYR		((volatile unsigned char  *)(0xA4448019))
#define TBNCR		((volatile unsigned short *)(0xA444801A))
#define RSPR0		((volatile unsigned char  *)(0xA4448020))
#define RSPR1		((volatile unsigned char  *)(0xA4448021))
#define RSPR2		((volatile unsigned char  *)(0xA4448022))
#define RSPR3		((volatile unsigned char  *)(0xA4448023))
#define RSPR4		((volatile unsigned char  *)(0xA4448024))
#define RSPR5		((volatile unsigned char  *)(0xA4448025))
#define RSPR6		((volatile unsigned char  *)(0xA4448026))
#define RSPR7		((volatile unsigned char  *)(0xA4448027))
#define RSPR8		((volatile unsigned char  *)(0xA4448028))
#define RSPR9		((volatile unsigned char  *)(0xA4448029))
#define RSPR10		((volatile unsigned char  *)(0xA444802A))
#define RSPR11		((volatile unsigned char  *)(0xA444802B))
#define RSPR12		((volatile unsigned char  *)(0xA444802C))
#define RSPR13		((volatile unsigned char  *)(0xA444802D))
#define RSPR14		((volatile unsigned char  *)(0xA444802E))
#define RSPR15		((volatile unsigned char  *)(0xA444802F))
#define RSPR16		((volatile unsigned char  *)(0xA4448030))
#define DTOUTR		((volatile unsigned short *)(0xA4448032))
#define DR		((volatile unsigned short *)(0xA4448040))
#define FIFOCLR		((volatile unsigned char  *)(0xA4448042))
#define DMACR		((volatile unsigned char  *)(0xA4448044))
#define INTCR2		((volatile unsigned char  *)(0xA4448046))
#define INTSTR2		((volatile unsigned char  *)(0xA4448048))

#endif	/* __SH7720_MMC_H */

