/* tmu.h:
 *	Timer unit (TMU) defines.
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
#ifndef __SH7720_TMU_H
#define __SH7720_TMU_H

/* TMU registers */
#define TMU_TSTR	((volatile unsigned char  *)(0xA412FE92))
enum
{
	TMU_TSTR_STR1	= 0x0002,
	TMU_TSTR_STR0	= 0x0001
};

#define TMU_TCOR_0	((volatile unsigned long  *)(0xA412FE94))
#define TMU_TCNT_0	((volatile unsigned long  *)(0xA412FE98))
#define TMU_TCR_0	((volatile unsigned short *)(0xA412FE9C))
enum
{
	TMU_TCR_UNF	= 0x0100,
	TMU_TCR_UNIE	= 0x0020,
	TMU_TCR_TPSC2	= 0x0004,
	TMU_TCR_TPSC1	= 0x0002,
	TMU_TCR_TPSC0	= 0x0001
};

#define TMU_TCOR_1	((volatile unsigned long  *)(0xA412FEA0))
#define TMU_TCNT_1	((volatile unsigned long  *)(0xA412FEA4))
#define TMU_TCR_1	((volatile unsigned short *)(0xA412FEA8))
#define TMU_TCOR_2	((volatile unsigned long  *)(0xA412FEAC))
#define TMU_TCNT_2	((volatile unsigned long  *)(0xA412FEB0))
#define TMU_TCR_2	((volatile unsigned short *)(0xA412FEB4))

#endif	/* __SH7720_TMU_H */

