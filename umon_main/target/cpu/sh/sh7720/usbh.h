/* usbh.h:
 *	USB host controller (USBH) defines.
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
#ifndef	__SH7720_USBH_H
#define	__SH7720_USBH_H

/* USBH registers */
#define USBHR		((volatile unsigned long *)(0xA4428000))
#define USBHC		((volatile unsigned long *)(0xA4428004))
#define USBHCS		((volatile unsigned long *)(0xA4428008))
#define USBHIS		((volatile unsigned long *)(0xA442800C))
#define USBHIE		((volatile unsigned long *)(0xA4428010))
#define USBHID		((volatile unsigned long *)(0xA4428014))
#define USBHHCCA	((volatile unsigned long *)(0xA4428018))
#define USBHPCED	((volatile unsigned long *)(0xA442801C))
#define USBHCHED	((volatile unsigned long *)(0xA4428020))
#define USBHCCED	((volatile unsigned long *)(0xA4428024))
#define USBHBHED	((volatile unsigned long *)(0xA4428028))
#define USBHBCED	((volatile unsigned long *)(0xA442802C))
#define USBHDHED	((volatile unsigned long *)(0xA4428030))
#define USBHFI		((volatile unsigned long *)(0xA4428034))
#define USBHFR		((volatile unsigned long *)(0xA4428038))
#define USBHFN		((volatile unsigned long *)(0xA442803C))
#define USBHPS		((volatile unsigned long *)(0xA4428040))
#define USBHLST		((volatile unsigned long *)(0xA4428044))
#define USBHRDA		((volatile unsigned long *)(0xA4428048))
#define USBHRDB		((volatile unsigned long *)(0xA442804C))
#define USBHRS		((volatile unsigned long *)(0xA4428050))
#define USBHRPS1	((volatile unsigned long *)(0xA4428054))
#define USBHRPS2	((volatile unsigned long *)(0xA4428058))

#endif	/* __SH7720_USBH_H */

