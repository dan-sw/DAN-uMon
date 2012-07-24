/* usbpmc.h:
 *	USB pin multiplex controller defines.
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
#ifndef __SH7720_USBPMC_H
#define __SH7720_USBPMC_H

/* USBPMC register */
#define UTRCTL	((volatile unsigned short *)(0xA405012C))
enum
{
	UTRCTL_DRV		= 0x0100,
	UTRCTL_USB_TRANS	= 0x0002,
	UTRCTL_USB_TRANS_ENABLE	= 0x0000,
	UTRCTL_USB_TRANS_DIGITA	= 0x0002,
	UTRCTL_USB_SEL		= 0x0001,
	UTRCTL_USB_SEL_HOST	= 0x0000,
	UTRCTL_USB_SEL_FUNCTION	= 0x0001
};

#endif	/* __SH7720_USBPMC_H */

