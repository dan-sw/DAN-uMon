/* scif.h:
 *	Serial communication interface with FIFO (SCIF) register and bit defines.
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
#ifndef __SH7720_SCIF_H
#define __SH7720_SCIF_H

/* SCIF registers */
#define SCSMR_0		((volatile unsigned short *)(0xA4430000))
enum
{
	SCSMR_SRC	= 0x0700,
	SCSMR_SRC_SHIFT	= 8,
	SCSMR_SRC_16	= 0x0000,
	SCSMR_SRC_5	= 0x0100,
	SCSMR_SRC_7	= 0x0200,
	SCSMR_SRC_11	= 0x0300,
	SCSMR_SRC_13	= 0x0400,
	SCSMR_SRC_17	= 0x0500,
	SCSMR_SRC_19	= 0x0600,
	SCSMR_SRC_27	= 0x0700,
	SCSMR_CA	= 0x0080,
	SCSMR_CHR	= 0x0040,
	SCSMR_PE	= 0x0020,
	SCSMR_OE	= 0x0010,
	SCSMR_STOP	= 0x0008,
	SCSMR_CKS	= 0x0003,
	SCSMR_CKS_1	= 0x0000,
	SCSMR_CKS_4	= 0x0001,
	SCSMR_CKS_16	= 0x0002,
	SCSMR_CKS_64	= 0x0003
};

#define SCBRR_0		((volatile unsigned char  *)(0xA4430004))
#define SCSCR_0		((volatile unsigned short *)(0xA4430008))
enum
{
	SCSCR_TDRQE	= 0x8000,
	SCSCR_RDRQE	= 0x4000,
	SCSCR_TSIE	= 0x0800,
	SCSCR_ERIE	= 0x0400,
	SCSCR_BRIE	= 0x0200,
	SCSCR_DRIE	= 0x0100,
	SCSCR_TIE	= 0x0080,
	SCSCR_RIE	= 0x0040,
	SCSCR_TE	= 0x0020,
	SCSCR_RE	= 0x0010,
	SCSCR_CKE1	= 0x0002,
	SCSCR_CKE0	= 0x0001
};

#define SCTDSR_0	((volatile unsigned char  *)(0xA443000C))
#define SCFER_0		((volatile unsigned short *)(0xA4430010))
enum
{
	SCFER_PER	= 0x3F00,
	SCFER_PER_SHIFT	= 8,
	SCFER_FER	= 0x003F
};

#define SCSSR_0		((volatile unsigned short *)(0xA4430014))
enum
{
	SCSSR_ORER	= 0x0200,
	SCSSR_TSF	= 0x0100,
	SCSSR_ER	= 0x0080,
	SCSSR_TEND	= 0x0040,
	SCSSR_TDFE	= 0x0020,
	SCSSR_BRK	= 0x0010,
	SCSSR_FER	= 0x0008,
	SCSSR_PER	= 0x0004,
	SCSSR_RDF	= 0x0002,
	SCSSR_DR	= 0x0001
};

#define SCFCR_0		((volatile unsigned short *)(0xA4430018))
enum
{
	SCFCR_TSE	= 0x8000,
	SCFCR_TCRST	= 0x4000,
	SCFCR_RSTRG	= 0x0700,
	SCFCR_RSTRG_63	= 0x0000,
	SCFCR_RSTRG_1	= 0x0100,
	SCFCR_RSTRG_8	= 0x0200,
	SCFCR_RSTRG_16	= 0x0300,
	SCFCR_RSTRG_32	= 0x0400,
	SCFCR_RSTRG_48	= 0x0500,
	SCFCR_RSTRG_54	= 0x0600,
	SCFCR_RSTRG_60	= 0x0700,
	SCFCR_RTRG	= 0x00C0,
	SCFCR_RTRG_1	= 0x0000,
	SCFCR_RTRG_16	= 0x0040,
	SCFCR_RTRG_32	= 0x0080,
	SCFCR_RTRG_48	= 0x00C0,
	SCFCR_TTRG	= 0x0030,
	SCFCR_TTRG_32	= 0x0000,
	SCFCR_TTRG_16	= 0x0010,
	SCFCR_TTRG_1	= 0x0020,
	SCFCR_TTRG_0	= 0x0030,
	SCFCR_MCE	= 0x0008,
	SCFCR_TFRST	= 0x0004,
	SCFCR_RFRST	= 0x0002,
	SCFCR_LOOP	= 0x0001
};

#define SCFDR_0		((volatile unsigned short *)(0xA443001C))
enum
{
	SCFDR_T		= 0x7F00,
	SCFDR_T_SHIFT	= 8,
	SCFDR_R		= 0x007F
};

#define SCFTDR_0	((volatile unsigned char  *)(0xA4430020))
#define SCFRDR_0	((volatile unsigned char  *)(0xA4430024))
#define SCSMR_1		((volatile unsigned short *)(0xA4438000))
#define SCBRR_1		((volatile unsigned char  *)(0xA4438004))
#define SCSCR_1		((volatile unsigned short *)(0xA4438008))
#define SCTDSR_1	((volatile unsigned char  *)(0xA443800C))
#define SCFER_1		((volatile unsigned short *)(0xA4438010))
#define SCSSR_1		((volatile unsigned short *)(0xA4438014))
#define SCFCR_1		((volatile unsigned short *)(0xA4438018))
#define SCFDR_1		((volatile unsigned short *)(0xA443801C))
#define SCFTDR_1	((volatile unsigned char  *)(0xA4438020))
#define SCFRDR_1	((volatile unsigned char  *)(0xA4438024))

struct SH7720_SCIF
{
	unsigned short scsmr;		/* 0x00 */
	unsigned short rsvd0;
	unsigned char  scbrr;		/* 0x04 */
	unsigned char  rsvd1[3];
	unsigned short scscr;		/* 0x08 */
	unsigned short rsvd2;
	unsigned char  sctdsr;		/* 0x0C */
	unsigned char  rsvd3[3];
	unsigned short scfer;		/* 0x10 */
	unsigned short rsvd4;
	unsigned short scssr;		/* 0x14 */
	unsigned short rsvd5;
	unsigned short scfcr;		/* 0x18 */
	unsigned short rsvd6;
	unsigned short scfdr;		/* 0x1C */
	unsigned short rsvd7;
	unsigned char  scftdr;		/* 0x20 */
	unsigned char  rsvd8[3];
	unsigned char  scfrdr;		/* 0x24 */
	unsigned char  rsvd9[3];
};

/* SCIF base addresses */
#define SH7720_SCIF0_BASE	((volatile SH7720_SCIF *)0xA4430000)
#define SH7720_SCIF1_BASE	((volatile SH7720_SCIF *)0xA4438000)

#endif	/* __SH7720_SCIF_H */

