/* intcntrl.h:
 *	Header file for interupt controller registers and 
 *	register bits.
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
#ifndef __SH7720_INTCNTRL_H
#define __SH7720_INTCNTRL_H

/* Interrupt controller registers */
#define IPRA		((volatile unsigned short *)(0xA414FEE2))
#define IPRB		((volatile unsigned short *)(0xA414FEE4))
#define IPRC		((volatile unsigned short *)(0xA4140016))
#define IPRD		((volatile unsigned short *)(0xA4140018))
#define IPRE		((volatile unsigned short *)(0xA414001A))
#define IPRF		((volatile unsigned short *)(0xA4080000))
#define IPRG		((volatile unsigned short *)(0xA4080002))
#define IPRH		((volatile unsigned short *)(0xA4080004))
#define IPRI		((volatile unsigned short *)(0xA4080006))
#define IPRJ		((volatile unsigned short *)(0xA4080008))
#define IRR0		((volatile unsigned char  *)(0xA4140004))
#define IRR1		((volatile unsigned char  *)(0xA4140006))
#define IRR2		((volatile unsigned char  *)(0xA4140008))
#define IRR3		((volatile unsigned char  *)(0xA414000A))
#define IRR4		((volatile unsigned char  *)(0xA414000C))
#define IRR5		((volatile unsigned char  *)(0xA4080020))
#define IRR6		((volatile unsigned char  *)(0xA4080022))
#define IRR7		((volatile unsigned char  *)(0xA4080024))
#define IRR8		((volatile unsigned char  *)(0xA4080026))
#define IRR9		((volatile unsigned char  *)(0xA4080028))
#define ICR0		((volatile unsigned short *)(0xA414FEE0))
#define ICR1		((volatile unsigned short *)(0xA4140010))
enum
{
	ICR1_MAI	= 0x8000,
	ICR1_IRQLVL	= 0x4000,
	ICR1_BLMSK	= 0x2000,
	ICR1_IRQ5	= 0x0C00,
	ICR1_IRQ5_HIGH	= 0x0C00,
	ICR1_IRQ5_LOW	= 0x0800,
	ICR1_IRQ5_RISE	= 0x0400,
	ICR1_IRQ5_FALL	= 0x0000,
	ICR1_IRQ4	= 0x0300,
	ICR1_IRQ4_HIGH	= 0x0300,
	ICR1_IRQ4_LOW	= 0x0200,
	ICR1_IRQ4_RISE	= 0x0100,
	ICR1_IRQ4_FALL	= 0x0000,
	ICR1_IRQ3	= 0x00C0,
	ICR1_IRQ3_HIGH	= 0x00C0,
	ICR1_IRQ3_LOW	= 0x0080,
	ICR1_IRQ3_RISE	= 0x0040,
	ICR1_IRQ3_FALL	= 0x0000,
	ICR1_IRQ2	= 0x0030,
	ICR1_IRQ2_HIGH	= 0x0030,
	ICR1_IRQ2_LOW	= 0x0020,
	ICR1_IRQ2_RISE	= 0x0010,
	ICR1_IRQ2_FALL	= 0x0000,
	ICR1_IRQ1	= 0x000C,
	ICR1_IRQ1_HIGH	= 0x000C,
	ICR1_IRQ1_LOW	= 0x0008,
	ICR1_IRQ1_RISE	= 0x0004,
	ICR1_IRQ1_FALL	= 0x0000,
	ICR1_IRQ0	= 0x0003,
	ICR1_IRQ0_HIGH	= 0x0003,
	ICR1_IRQ0_LOW	= 0x0002,
	ICR1_IRQ0_RISE	= 0x0001,
	ICR1_IRQ0_FALL	= 0x0000
};

#define ICR2		((volatile unsigned short *)(0xA4140012))
enum
{
	ICR2_PINT15S	= 0x8000,
	ICR2_PINT14S	= 0x4000,
	ICR2_PINT13S	= 0x2000,
	ICR2_PINT12S	= 0x1000,
	ICR2_PINT11S	= 0x0800,
	ICR2_PINT10S	= 0x0400,
	ICR2_PINT9S	= 0x0200,
	ICR2_PINT8S	= 0x0100,
	ICR2_PINT7S	= 0x0080,
	ICR2_PINT6S	= 0x0040,
	ICR2_PINT5S	= 0x0020,
	ICR2_PINT4S	= 0x0010,
	ICR2_PINT3S	= 0x0008,
	ICR2_PINT2S	= 0x0004,
	ICR2_PINT1S	= 0x0002,
	ICR2_PINT0S	= 0x0001
};
#define PINTER		((volatile unsigned short *)(0xA4140014))
enum
{
	PINTER_PINT15E	= 0x8000,
	PINTER_PINT14E	= 0x4000,
	PINTER_PINT13E	= 0x2000,
	PINTER_PINT12E	= 0x1000,
	PINTER_PINT11E	= 0x0800,
	PINTER_PINT10E	= 0x0400,
	PINTER_PINT9E	= 0x0200,
	PINTER_PINT8E	= 0x0100,
	PINTER_PINT7E	= 0x0080,
	PINTER_PINT6E	= 0x0040,
	PINTER_PINT5E	= 0x0020,
	PINTER_PINT4E	= 0x0010,
	PINTER_PINT3E	= 0x0008,
	PINTER_PINT2E	= 0x0004,
	PINTER_PINT1E	= 0x0002,
	PINTER_PINT0E	= 0x0001
};

#endif	/* __SH7720_INTCNTRL_H */

