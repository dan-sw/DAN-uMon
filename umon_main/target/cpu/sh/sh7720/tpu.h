/* tpu.h:
 *	16-Bit timer pulse unit (TPU) defines.
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
#ifndef	__SH7720_TPU_H
#define	__SH7720_TPU_H

/* TPU registers */
#define TPU_TSTR	((volatile unsigned short *)(0xA4480000))
#define TPU_TCR_0	((volatile unsigned short *)(0xA4480010))
#define TPU_TMDR_0	((volatile unsigned short *)(0xA4480014))
#define TPU_TIOR_0	((volatile unsigned short *)(0xA4480018))
#define TPU_TIER_0	((volatile unsigned short *)(0xA448001C))
#define TPU_TSR_0	((volatile unsigned short *)(0xA4480020))
#define TPU_TCNT_0	((volatile unsigned short *)(0xA4480024))
#define TPU_TGRA_0	((volatile unsigned short *)(0xA4480028))
#define TPU_TGRB_0	((volatile unsigned short *)(0xA448002C))
#define TPU_TGRC_0	((volatile unsigned short *)(0xA4480030))
#define TPU_TGRD_0	((volatile unsigned short *)(0xA4480034))
#define TPU_TCR_1	((volatile unsigned short *)(0xA4480050))
#define TPU_TMDR_1	((volatile unsigned short *)(0xA4480054))
#define TPU_TIOR_1	((volatile unsigned short *)(0xA4480058))
#define TPU_TIER_1	((volatile unsigned short *)(0xA448005C))
#define TPU_TSR_1	((volatile unsigned short *)(0xA4480060))
#define TPU_TCNT_1	((volatile unsigned short *)(0xA4480064))
#define TPU_TGRA_1	((volatile unsigned short *)(0xA4480068))
#define TPU_TGRB_1	((volatile unsigned short *)(0xA448006C))
#define TPU_TGRC_1	((volatile unsigned short *)(0xA4480070))
#define TPU_TGRD_1	((volatile unsigned short *)(0xA4480074))
#define TPU_TCR_2	((volatile unsigned short *)(0xA4480090))
#define TPU_TMDR_2	((volatile unsigned short *)(0xA4480094))
#define TPU_TIOR_2	((volatile unsigned short *)(0xA4480098))
#define TPU_TIER_2	((volatile unsigned short *)(0xA448009C))
#define TPU_TSR_2	((volatile unsigned short *)(0xA44800A0))
#define TPU_TCNT_2	((volatile unsigned short *)(0xA44800A4))
#define TPU_TGRA_2	((volatile unsigned short *)(0xA44800A8))
#define TPU_TGRB_2	((volatile unsigned short *)(0xA44800AC))
#define TPU_TGRC_2	((volatile unsigned short *)(0xA44800B0))
#define TPU_TGRD_2	((volatile unsigned short *)(0xA44800B4))
#define TPU_TCR_3	((volatile unsigned short *)(0xA44800D0))
#define TPU_TMDR_3	((volatile unsigned short *)(0xA44800D4))
#define TPU_TIOR_3	((volatile unsigned short *)(0xA44800D8))
#define TPU_TIER_3	((volatile unsigned short *)(0xA44800DC))
#define TPU_TSR_3	((volatile unsigned short *)(0xA44800E0))
#define TPU_TCNT_3	((volatile unsigned short *)(0xA44800E4))
#define TPU_TGRA_3	((volatile unsigned short *)(0xA44800E8))
#define TPU_TGRB_3	((volatile unsigned short *)(0xA44800EC))
#define TPU_TGRC_3	((volatile unsigned short *)(0xA44800F0))
#define TPU_TGRD_3	((volatile unsigned short *)(0xA44800F4))

#endif	/* __SH7720_TPU_H */

