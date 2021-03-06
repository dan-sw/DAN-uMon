/* adc.h
 *	A/D converter (ADC) defines.
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
#ifndef __SH7720_ADC_H
#define __SH7720_ADC_H

/* ADC registers */
#define ADDRA	((volatile unsigned short *)(0xA44C0000))
#define ADDRB	((volatile unsigned short *)(0xA44C0002))
#define ADDRC	((volatile unsigned short *)(0xA44C0004))
#define ADDRD	((volatile unsigned short *)(0xA44C0006))
#define ADCSR	((volatile unsigned short *)(0xA44C0008))

#endif	/* __SH7720_ADC_H */

