/* 
 * dis_ten.c
 *
 * Tensilica disassembler.
 *
 * by Albert Yosher (alberty@designartnetworks.com)
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
 *	Note1: the majority of this code was edited with 4-space tabs.
 *	Note2: as more and more contributions are accepted, the term "author"
 *		   is becoming a mis-representation of credit.
 *
 *	Original author:	Ed Sutter
 *	Email:				esutter@lucent.com
 *	Phone:				908-582-2351
 *
 */

#include "config.h"
#include "xtensa/hal.h"

#if INCLUDE_DISASSEMBLER


/************************************************************************/


/**************************************************************************/

char *DisHelp[] = {
	"Disassemble memory - not supported",
	0,
};

/**************************************************************************/

int
Dis(int argc, char *argv[])
{
	
	return(0);
}

/**************************************************************************/

#endif	/* INCLUDE_DISASSEMBLER */
