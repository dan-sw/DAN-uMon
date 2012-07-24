/* rtc.h:
 *	Realtime clock (RTC) defines.
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
#ifndef __SH7720_RTC_H
#define __SH7720_RTC_H

/* RTC registers */
#define R64CNT		((volatile unsigned char  *)(0xA413FEC0))
#define RSECCNT		((volatile unsigned char  *)(0xA413FEC2))
#define RMINCNT		((volatile unsigned char  *)(0xA413FEC4))
#define RHRCNT		((volatile unsigned char  *)(0xA413FEC6))
#define RWKCNT		((volatile unsigned char  *)(0xA413FEC8))
#define RDAYCNT		((volatile unsigned char  *)(0xA413FECA))
#define RMONCNT		((volatile unsigned char  *)(0xA413FECC))
#define RYRCNT		((volatile unsigned short *)(0xA413FECE))
#define RSECAR		((volatile unsigned char  *)(0xA413FED0))
#define RMINAR		((volatile unsigned char  *)(0xA413FED2))
#define RHRAR		((volatile unsigned char  *)(0xA413FED4))
#define RWKAR		((volatile unsigned char  *)(0xA413FED6))
#define RDAYAR		((volatile unsigned char  *)(0xA413FED8))
#define RMONAR		((volatile unsigned char  *)(0xA413FEDA))
#define RCR1		((volatile unsigned char  *)(0xA413FEDC))
#define RCR2		((volatile unsigned char  *)(0xA413FEDE))
#define RYRAR		((volatile unsigned short *)(0xA413FEE0))
#define RCR3		((volatile unsigned char  *)(0xA413FEE4))

/* RCR1 register bits */
#define RCR1_CF		0x80
#define RCR1_CIE	0x10
#define RCR1_AIE	0x08
#define RCR1_AF		0x01

/* RCR2 register bits */
#define RCR2_PEF	0x80
#define RCR2_PES	0x70
#define RCR2_PES_NOINT	0x00
#define RCR2_PES_1_256	0x10
#define RCR2_PES_1_64	0x20
#define RCR2_PES_1_16	0x30
#define RCR2_PES_1_4	0x40
#define RCR2_PES_1_2	0x50
#define RCR2_PES_1	0x60
#define RCR2_PES_2	0x70
#define RCR2_RTCEN	0x08
#define RCR2_ADJ	0x04
#define RCR2_RESET	0x02
#define RCR2_START	0x01

/* RCR3 register bits */
#define RCR3_ENB	0x80

#endif	/* __SH7720_RTC_H */

