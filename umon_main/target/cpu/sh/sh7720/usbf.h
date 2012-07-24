/* usbf.h:
 *	USB function controller (USBF) defines.
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
#ifndef __SH7720_USBF_H
#define __SH7720_USBF_H

/* USBF registers. */
#define IFR0	((volatile unsigned char *)(0xA4420000))
enum
{
	IFR0_BRST		= 0x80,
	IFR0_EP1_FULL	= 0x40,
	IFR0_EP2_TR		= 0x20,
	IFR0_EP2_EMPTY	= 0x10,
	IFR0_SETUP_TS	= 0x08,
	IFR0_EP0o_TS	= 0x04,
	IFR0_EP0i_TR	= 0x02,
	IFR0_EP0i_TS	= 0x01
};

#define IFR1	((volatile unsigned char *)(0xA4420001))
enum
{
	IFR1_VBUS_MN	= 0x08,
	IFR1_EP3_TR		= 0x04,
	IFR1_EP3_TS		= 0x02,
	IFR1_VBUSF		= 0x01
};

#define IFR2	((volatile unsigned char *)(0xA4420002))
enum
{
	IFR2_SURSS	= 0x20,
	IFR2_SURSF	= 0x10,
	IFR2_CFDN	= 0x08,
	IFR2_SOF	= 0x04,
	IFR2_SETC	= 0x02,
	IFR2_SETI	= 0x01
};

#define IFR3	((volatile unsigned char *)(0xA4420003))
enum
{
	IFR3_EP5_TR		= 0x08,
	IFR3_EP5_TS		= 0x04,
	IFR3_EP4_TF		= 0x02,
	IFR3_EP4_TS		= 0x01
};

#define IER0	((volatile unsigned char *)(0xA4420004))
enum
{
	IER0_BRST_IE		= 0x80,
	IER0_EP1_FULL_IE	= 0x40,
	IER0_EP2_TR_IE		= 0x20,
	IER0_EP2_EMPTY_IE	= 0x10,
	IER0_SETUP_TS_IE	= 0x08,
	IER0_EP0o_TS_IE		= 0x04,
	IER0_EP0i_TR_IE		= 0x02,
	IER0_EP0i_TS_IE		= 0x01
};

#define IER1	((volatile unsigned char *)(0xA4420005))
enum
{
	IER1_EP3_TR_IE	= 0x04,
	IER1_EP3_TS_IE	= 0x02,
	IER1_VBUSF_IE	= 0x01
};

#define IER2	((volatile unsigned char *)(0xA4420006))
enum
{
	IER2_SURSE_IE	= 0x10,
	IER2_CFDN_IE	= 0x08,
	IER2_SOFE_IE	= 0x04,
	IER2_SETCE_IE	= 0x02,
	IER2_SETIE_IE	= 0x01
};

#define IER3	((volatile unsigned char *)(0xA4420007))
enum
{
	IER3_EP5_TR_IE	= 0x08,
	IER3_EP5_TS_IE	= 0x04,
	IER3_EP4_TF_IE	= 0x02,
	IER3_EP4_TS_IE	= 0x01
};

#define ISR0	((volatile unsigned char *)(0xA4420008))
enum
{
	ISR0_BRST_IS		= 0x80,
	ISR0_EP1_FULL_IS	= 0x40,
	ISR0_EP2_TR_IS		= 0x20,
	ISR0_EP2_EMPTY_TS	= 0x10,
	ISR0_SETUP_TS_IS	= 0x08,
	ISR0_EP0o_TS_IS		= 0x04,
	ISR0_EP0i_TR_IS		= 0x02,
	ISR0_EP0i_TS_IS		= 0x01
};

#define ISR1	((volatile unsigned char *)(0xA4420009))
enum
{
	ISR1_EP3_TR_IS	= 0x04,
	ISR1_EP3_TS_IS	= 0x02,
	ISR1_VBUSF_IS	= 0x01
};

#define ISR2	((volatile unsigned char *)(0xA442000A))
enum
{
	ISR2_SURSE_IS	= 0x10,
	ISR2_CFDN_IS	= 0x08,
	ISR2_SOFE_IS	= 0x04,
	ISR2_SETCE_IS	= 0x02,
	ISR2_SETIE_IS	= 0x01
};

#define ISR3	((volatile unsigned char *)(0xA442000B))
enum
{
	ISR3_EP5_TR_IS	= 0x08,
	ISR3_EP5_TS_IS	= 0x04,
	ISR3_EP4_TF_IS	= 0x02,
	ISR3_EP4_TS_IS	= 0x01
};

#define EPDR0i	((volatile unsigned char *)(0xA442000C))
#define EPDR0o	((volatile unsigned char *)(0xA442000D))
#define EPDR0s	((volatile unsigned char *)(0xA442000E))
#define EPDR1	((volatile unsigned char *)(0xA4420010))
#define EPDR2	((volatile unsigned char *)(0xA4420014))
#define EPDR3	((volatile unsigned char *)(0xA4420018))
#define EPDR4	((volatile unsigned char *)(0xA442001C))
#define EPDR5	((volatile unsigned char *)(0xA4420020))
#define EPSZ0o	((volatile unsigned char *)(0xA4420024))
#define EPSZ1	((volatile unsigned char *)(0xA4420025))
#define EPSZ4	((volatile unsigned char *)(0xA4420026))
#define DASTS	((volatile unsigned char *)(0xA4420027))
enum
{
	DASTS_EP3_DE	= 0x20,
	DASTS_EP2_DE	= 0x10,
	DASTS_EP0iDE	= 0x01
};

#define FCLR0	((volatile unsigned char *)(0xA4420028))
enum
{
	FCLR0_EP3_CLR	= 0x40,
	FCLR0_EP1_CLR	= 0x20,
	FCLR0_EP2_CLR	= 0x10,
	FCLR0_EP0o_CLR	= 0x02,
	FCLR0_EP0i_CLR	= 0x01
};

#define FCLR1	((volatile unsigned char *)(0xA4420029))
enum
{
	FCLR1_EP5_CCLR	= 0x10,
	FCLR1_EP5_CLR	= 0x02,
	FCLR1_EP4_CLR	= 0x01
};

#define EPSTL0	((volatile unsigned char *)(0xA442002A))
enum
{
	EPSTL0_EP3_STL	= 0x08,
	EPSTL0_EP2_STL	= 0x04,
	EPSTL0_EP1_STL	= 0x02,
	EPSTL0_EP0_STL	= 0x01
};

#define EPSTL1	((volatile unsigned char *)(0xA442002B))
enum
{
	EPSTL1_EP5_STL	= 0x02,
	EPSTL1_EP4_STL	= 0x01
};

#define TRG	((volatile unsigned char *)(0xA442002C))
enum
{
	TRG_EP3_PKTE	= 0x40,
	TRG_EP1_RDFN	= 0x20,
	TRG_EP2_PKTE	= 0x10,
	TRG_EP0s_RDFN	= 0x04,
	TRG_EP0o_RDFN	= 0x02,
	TRG_EP0i_PKTE	= 0x01
};

#define DMA	((volatile unsigned char *)(0xA442002D))
enum
{
	DMA_PULLUP_E		= 0x04,
	DMA_PULLUP_E_HIGH	= 0x04,
	DMA_PULLUP_E_LOW	= 0x00,
	DMA_EP2_DMAE		= 0x02,
	DMA_EP1_DMAE		= 0x01
};

#define CVR	((volatile unsigned char *)(0xA442002E))
enum
{
	CVR_CNFV		= 0xc0,
	CVR_CNFV_SHIFT	= 6,
	CVR_INTV		= 0x30,
	CVR_INTV_SHIFT	= 4,
	CVR_ALTV		= 0x07,
	CVR_ALTV_SHIFT	= 0
};

#define CTLR0	((volatile unsigned char *)(0xA442002F))
enum
{
	CTLR0_RWUPS				= 0x10,
	CTLR0_RSME				= 0x08,
	CTLR0_PWMD				= 0x04,
	CTLR0_PWMD_SELF_POWERED	= 0x00,
	CTLR0_PWMD_BUS_POWERED	= 0x04,
	CTLR0_ASCE				= 0x02
};

#define TSRH	((volatile unsigned char *)(0xA4420030))
#define TSRL	((volatile unsigned char *)(0xA4420031))
#define EPIR	((volatile unsigned char *)(0xA4420032))
enum
{
	EPIRN0_ENDPOINT				= 0xf0,
	EPIRN0_ENDPOINT_SHIFT		= 4,
	EPIRN0_CONFIG				= 0x0c,
	EPIRN0_CONFIG_SHIFT			= 2,
	EPIRN0_INTERFACE			= 0x03,
	EPIRN0_INTERFACE_SHIFT		= 0,

	EPIRN1_ALTERNATE			= 0xc0,
	EPIRN1_ALTERNATE_SHIFT		= 6,
	EPIRN1_TRANSFER				= 0x30,
	EPIRN1_TRANSFER_CONTROL		= 0x00,
	EPIRN1_TRANSFER_ISOCHRONOUS	= 0x10,
	EPIRN1_TRANSFER_BULK		= 0x20,
	EPIRN1_TRANSFER_INTERRUPT	= 0x30,
	EPIRN1_DIRECTION			= 0x08,
	EPIRN1_DIRECTION_IN			= 0x08,
	EPIRN1_DIRECTION_OUT		= 0x00,

	EPIRN2_PACKET_SIZE			= 0xfe,
	EPIRN2_PACKET_SIZE_SHIFT	= 1
};

#define IFR4	((volatile unsigned char *)(0xA4420034))
enum
{
	IFR4_TMOUT		= 0x01
};

#define IER4	((volatile unsigned char *)(0xA4420035))
enum
{
	IER4_TMOUT_IE	= 0x01
};

#define ISR4	((volatile unsigned char *)(0xA4420036))
enum
{
	ISR4_TMOUT_IS	= 0x01
};

#define CTLR1	((volatile unsigned char *)(0xA4420037))
enum
{
	CTLR1_TMR_ACLR	= 0x02,
	CTLR1_TMR_EN	= 0x01
};

#define TMRH	((volatile unsigned char *)(0xA4420038))
#define TMRL	((volatile unsigned char *)(0xA4420039))
#define STOH	((volatile unsigned char *)(0xA442003A))
#define STOL	((volatile unsigned char *)(0xA442003B))

#endif	/* __SH7720_USBF_H */

