/*
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
 */

#include "config.h"

#ifdef FLASHRAM_BASE
/* As of Jan 2004, this module is no longer needed.
 * I added an option to FlashRamInit() so that if the incoming sector
 * size table is NULL, then all sectors are initialized to the same
 * size.  For cases where this table was used, the call to FlashRamInit
 * should have the ssizes argument changed to zero.  See FlashRamInit()
 * for more on this.
 */
#if 0
#include "flash.h"

/* Generic Flash RAM configuration information...
 * The assumption is a 16-element array (FLASHRAM_SECTORCOUNT = 16).
 * 
 * This can be included in a monitor build if the build has 
 * a block of RAM dedicated to TFS file storage.  If some other
 * configuration is required, then copy this to target-specific space
 * and modify a local version.
 */
int
ramSectors[FLASHRAM_SECTORCOUNT] = {
	FLASHRAM_SECTORSIZE, FLASHRAM_SECTORSIZE,
	FLASHRAM_SECTORSIZE, FLASHRAM_SECTORSIZE,
	FLASHRAM_SECTORSIZE, FLASHRAM_SECTORSIZE,
	FLASHRAM_SECTORSIZE, FLASHRAM_SECTORSIZE,
	FLASHRAM_SECTORSIZE, FLASHRAM_SECTORSIZE,
	FLASHRAM_SECTORSIZE, FLASHRAM_SECTORSIZE,
	FLASHRAM_SECTORSIZE, FLASHRAM_SECTORSIZE,
	FLASHRAM_SECTORSIZE, FLASHRAM_SECTORSIZE,
};
#endif
#endif
