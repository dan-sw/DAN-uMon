/* port.h:
 *	I/O ports defines.
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
#ifndef __SH7720_PORT_H
#define __SH7720_PORT_H

/* PORT register */
#define PADR	((volatile unsigned char *)(0xA4050140))
#define PBDR	((volatile unsigned char *)(0xA4050142))
#define PCDR	((volatile unsigned char *)(0xA4050144))
#define PDDR	((volatile unsigned char *)(0xA4050146))
#define PEDR	((volatile unsigned char *)(0xA4050148))
#define PFDR	((volatile unsigned char *)(0xA405014A))
#define PGDR	((volatile unsigned char *)(0xA405014C))
#define PHDR	((volatile unsigned char *)(0xA405014E))
#define PJDR	((volatile unsigned char *)(0xA4050150))
#define PKDR	((volatile unsigned char *)(0xA4050152))
#define PLDR	((volatile unsigned char *)(0xA4050154))
#define PMDR	((volatile unsigned char *)(0xA4050156))
#define PPDR	((volatile unsigned char *)(0xA4050158))
#define PRDR	((volatile unsigned char *)(0xA405015A))
#define PSDR	((volatile unsigned char *)(0xA405015C))
#define PTDR	((volatile unsigned char *)(0xA405015E))
#define PUDR	((volatile unsigned char *)(0xA4050160))
#define PVDR	((volatile unsigned char *)(0xA4050162))

#endif	/* __SH7720_PORT_H */

