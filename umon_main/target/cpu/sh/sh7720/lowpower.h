/* lowpower.h:
 *	Power-down modes defines.
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
#ifndef __SH7720_LOWPOWER_H
#define __SH7720_LOWPOWER_H

/* Low Power */
#define STBCR3		((volatile unsigned char *)(0xA40A0000))
enum
{
	STBCR3_MSTP37		= 0x80,
	STBCR3_MSTP37_OPERATE	= 0x00,
	STBCR3_MSTP37_HALT	= 0x80,
	STBCR3_MSTP36		= 0x40,
	STBCR3_MSTP36_OPERATE	= 0x00,
	STBCR3_MSTP36_HALT	= 0x40,
	STBCR3_MSTP35		= 0x20,
	STBCR3_MSTP35_OPERATE	= 0x00,
	STBCR3_MSTP35_HALT	= 0x20,
	STBCR3_MSTP33		= 0x08,
	STBCR3_MSTP33_OPERATE	= 0x00,
	STBCR3_MSTP33_HALT	= 0x08,
	STBCR3_MSTP32		= 0x04,
	STBCR3_MSTP32_OPERATE	= 0x00,
	STBCR3_MSTP32_HALT	= 0x04,
	STBCR3_MSTP31		= 0x02,
	STBCR3_MSTP31_OPERATE	= 0x00,
	STBCR3_MSTP31_HALT	= 0x02,
	STBCR3_MSTP30		= 0x01,
	STBCR3_MSTP30_OPERATE	= 0x00,
	STBCR3_MSTP30_HALT	= 0x01
};

#define STBCR4		((volatile unsigned char *)(0xA40A0004))
enum
{
	STBCR4_MSTP45		= 0x20,
	STBCR4_MSTP45_OPERATE	= 0x00,
	STBCR4_MSTP45_HALT	= 0x20,
	STBCR4_MSTP44		= 0x10,
	STBCR4_MSTP44_OPERATE	= 0x00,
	STBCR4_MSTP44_HALT	= 0x10,
	STBCR4_MSTP43		= 0x08,
	STBCR4_MSTP43_OPERATE	= 0x00,
	STBCR4_MSTP43_HALT	= 0x08,
	STBCR4_MSTP42		= 0x04,
	STBCR4_MSTP42_OPERATE	= 0x00,
	STBCR4_MSTP42_HALT	= 0x04,
	STBCR4_MSTP41		= 0x02,
	STBCR4_MSTP41_OPERATE	= 0x00,
	STBCR4_MSTP41_HALT	= 0x02,
	STBCR4_MSTP40		= 0x01,
	STBCR4_MSTP40_OPERATE	= 0x00,
	STBCR4_MSTP40_HALT	= 0x01
};

#define STBCR5		((volatile unsigned char *)(0xA40A0010))
enum
{
	STBCR5_MSTP54		= 0x10,
	STBCR5_MSTP54_OPERATE	= 0x00,
	STBCR5_MSTP54_HALT	= 0x10,
	STBCR5_MSTP52		= 0x04,
	STBCR5_MSTP52_OPERATE	= 0x00,
	STBCR5_MSTP52_HALT	= 0x04,
	STBCR5_MSTP51		= 0x02,
	STBCR5_MSTP51_OPERATE	= 0x00,
	STBCR5_MSTP51_HALT	= 0x02,
	STBCR5_MSTP50		= 0x01,
	STBCR5_MSTP50_OPERATE	= 0x00,
	STBCR5_MSTP50_HALT	= 0x01
};

#define SRSTR		((volatile unsigned char *)(0xA40A0024))
#define SRSTR2		((volatile unsigned char *)(0xA40A0028))
#define STBC		((volatile unsigned char *)(0xA415FF82))
enum
{
	STBCR_STBY		= 0x80,
	STBCR_STBY_SLEEP	= 0x00,
	STBCR_STBY_STANDBY	= 0x80,
	STBCR_STBXTL		= 0x10,
	STBCR_STBXTL_OPERATE	= 0x10,
	STBCR_STBXTL_HALT	= 0x00,
	STBCR_MSTP2		= 0x04,
	STBCR_MSTP2_OPERATE	= 0x00,
	STBCR_MSTP2_HALT	= 0x04,
	STBCR_MSTP1		= 0x02,
	STBCR_MSTP1_OPERATE	= 0x00,
	STBCR_MSTP1_HALT	= 0x01
};

#define STBCR2		((volatile unsigned char *)(0xA415FF88))
enum
{
	STBCR2_MSTP10		= 0x80,
	STBCR2_MSTP10_OPERATE	= 0x00,
	STBCR2_MSTP10_HALT	= 0x80,
	STBCR2_MSTP9		= 0x40,
	STBCR2_MSTP9_OPERATE	= 0x00,
	STBCR2_MSTP9_HALT	= 0x40,
	STBCR2_MSTP8		= 0x20,
	STBCR2_MSTP8_OPERATE	= 0x00,
	STBCR2_MSTP8_HALT	= 0x20,
	STBCR2_MSTP7		= 0x10,
	STBCR2_MSTP7_OPERATE	= 0x00,
	STBCR2_MSTP7_HALT	= 0x10,
	STBCR2_MSTP6		= 0x08,
	STBCR2_MSTP6_OPERATE	= 0x00,
	STBCR2_MSTP6_HALT	= 0x08,
	STBCR2_MSTP5		= 0x04,
	STBCR2_MSTP5_OPERATE	= 0x00,
	STBCR2_MSTP5_HALT	= 0x04,
	STBCR2_MSTP3		= 0x01,
	STBCR2_MSTP3_OPERATE	= 0x00,
	STBCR2_MSTP3_HALT	= 0x01
};

#endif	/* __SH7720_LOWPOWER_H */

