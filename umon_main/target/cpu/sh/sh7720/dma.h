/* dma.h:
 *	Direct memory access (DMA) defines.
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
#ifndef __SH7720_DMA_H
#define __SH7720_DMA_H

/* DMA registers */
#define SAR_0		((volatile unsigned long  *)(0xA4010020))
#define DAR_0		((volatile unsigned long  *)(0xA4010024))
#define DMATCR_0	((volatile unsigned long  *)(0xA4010028))
#define CHCR_0		((volatile unsigned long  *)(0xA401002C))
#define SAR_1		((volatile unsigned long  *)(0xA4010030))
#define DAR_1		((volatile unsigned long  *)(0xA4010034))
#define DMATCR_1	((volatile unsigned long  *)(0xA4010038))
#define CHCR_1		((volatile unsigned long  *)(0xA401003C))
#define SAR_2		((volatile unsigned long  *)(0xA4010040))
#define DAR_2		((volatile unsigned long  *)(0xA4010044))
#define DMATCR_2	((volatile unsigned long  *)(0xA4010048))
#define CHCR_2		((volatile unsigned long  *)(0xA401004C))
#define SAR_3		((volatile unsigned long  *)(0xA4010050))
#define DAR_3		((volatile unsigned long  *)(0xA4010054))
#define DMATCR_3	((volatile unsigned long  *)(0xA4010058))
#define CHCR_3		((volatile unsigned long  *)(0xA401005C))
#define DMAOR		((volatile unsigned short *)(0xA4010060))
#define SAR_4		((volatile unsigned long  *)(0xA4010070))
#define DAR_4		((volatile unsigned long  *)(0xA4010074))
#define DMATCR_4	((volatile unsigned long  *)(0xA4010078))
#define CHCR_4		((volatile unsigned long  *)(0xA401007C))
#define SAR_5		((volatile unsigned long  *)(0xA4010080))
#define DAR_5		((volatile unsigned long  *)(0xA4010084))
#define DMATCR_5	((volatile unsigned long  *)(0xA4010088))
#define CHCR_5		((volatile unsigned long  *)(0xA401008C))
#define DMARS0		((volatile unsigned short *)(0xA4090000))
#define DMARS1		((volatile unsigned short *)(0xA4090004))
#define DMARS2		((volatile unsigned short *)(0xA4090008))

#endif	/* __SH7720_DMA_H */

