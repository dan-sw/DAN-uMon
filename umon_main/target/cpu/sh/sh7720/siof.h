/* siof.h:
 *	Serial I/O with FIFO (SIOF) defines.
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
#ifndef __SH7720_SIOF_H
#define __SH7720_SIOF_H

/* SIOF registers */
#define SIMDR_0		((volatile unsigned short *)(0xA4410000))
#define SISCR_0		((volatile unsigned short *)(0xA4410002))
#define SITDAR_0	((volatile unsigned short *)(0xA4410004))
#define SIRDAR_0	((volatile unsigned short *)(0xA4410006))
#define SICDAR_0	((volatile unsigned short *)(0xA4410008))
#define SICTR_0		((volatile unsigned short *)(0xA441000C))
#define SIFCTR_0	((volatile unsigned short *)(0xA4410010))
#define SISTR_0		((volatile unsigned short *)(0xA4410014))
#define SIIER_0		((volatile unsigned short *)(0xA4410016))
#define SITDR_0		((volatile unsigned long  *)(0xA4410020))
#define SIRDR_0		((volatile unsigned long  *)(0xA4410024))
#define SITCR_0		((volatile unsigned long  *)(0xA4410028))
#define SIRCR_0		((volatile unsigned long  *)(0xA441002C))
#define SPICR_0		((volatile unsigned short *)(0xA4410030))
#define SIMDR_1		((volatile unsigned short *)(0xA4418000))
#define SISCR_1		((volatile unsigned short *)(0xA4418002))
#define SITDAR_1	((volatile unsigned short *)(0xA4418004))
#define SIRDAR_1	((volatile unsigned short *)(0xA4418006))
#define SICDAR_1	((volatile unsigned short *)(0xA4418008))
#define SICTR_1		((volatile unsigned short *)(0xA441800C))
#define SIFCTR_1	((volatile unsigned short *)(0xA4418010))
#define SISTR_1		((volatile unsigned short *)(0xA4418014))
#define SIIER_1		((volatile unsigned short *)(0xA4418016))
#define SITDR_1		((volatile unsigned long  *)(0xA4418020))
#define SIRDR_1		((volatile unsigned long  *)(0xA4418024))
#define SITCR_1		((volatile unsigned long  *)(0xA4418028))
#define SIRCR_1		((volatile unsigned long  *)(0xA441802C))
#define SPICR_1		((volatile unsigned short *)(0xA4418030))

#endif	/* __SH7720_SIOF_H */
