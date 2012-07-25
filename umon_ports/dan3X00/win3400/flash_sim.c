/*
All files except if stated otherwise in the begining of the file are under the ISC license:
-----------------------------------------------------------------------------------

Copyright (c) 2010-2012 Design Art Networks Ltd.

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

-----------------------------------------------------------------------------------
*/
/**********************************************************************\
 Library    :  uMON (Micro Monitor)
 Filename   :  flash3400.c
 Purpose    :  Flash driver port for DAN3400
 Owner		:  Sergey Krasnitsky
 Created    :  2.11.2010

 Note :
	This file is written based on flash_template.c, the template file 
	for building a flash driver for the uMON. Refer to the original 
	umon_main\target\flash\README for naming conventions and other 
	details regarding uMon1.0's flash device driver interface.
\**********************************************************************/

#include <stdio.h>
#include "config.h"

// instead of include "genlib.h" 
// (conflicts with stdio.h)
char *memset(char *, char, int);
char *memcpy(char *, char *, int);
void free(void *);
char *malloc(int);


#if INCLUDE_FLASH

#include "flash.h"
#include "stddefs.h"


/**********************************************************************\
				Flash simulator via file system
\**********************************************************************/


int flash3400RamWin32 [FLASH3400RAMWIN32_SIZE / sizeof(int)];


void FLASH_Read(uint8 bank, uint8 *data, unsigned addr, unsigned len)
{
    FILE * h = fopen ("tfs_win.bin", "rb");

    if (!h) {
        puts ("tfs_win.bin not found");
        return;
    }

    fseek(h,addr,0);
    fread((void*)data, 1, len, h);
    fclose(h);
}

int FLASH_Write(uint8 bank, unsigned addr, uint8* data, unsigned len)
{
    FILE * h = fopen ("tfs_win.bin", "rb+");

    if (!h) {
        puts ("tfs_win.bin not found");
        return 1;
    }

    fseek(h,addr,0);
    fwrite((void*)data, 1, len, h);
    fclose(h);
    return 0 /*Ok*/;
}

int FLASH_EraseSector(uint8 bank, unsigned secnum)
{
    static uint8 erasebuf[FLASH3400_SECTORSIZE] = { 0xFF };
	memset (erasebuf, 0xFF, FLASH3400_SECTORSIZE);
    FLASH_Write(0, secnum * FLASH3400_SECTORSIZE, erasebuf, FLASH3400_SECTORSIZE);
    return 0;
}


/**********************************************************************\
						Flash3400 functions
\**********************************************************************/

#define flash3400R_GetSectorBase(isector)		\
		(FLASH3400_RAM_BASE + (isector * FLASH3400_SECTORSIZE))


#define flash3400R_CheckRangeSector(isector)	\
		if ((unsigned)isector >= FLASH3400_NUMSECTORS)												\
		{																							\
			printf ("uMON ERROR : flash3400 : sector number (%d) is out of range\n", isector);		\
			return -1;																				\
		}

#define flash3400R_CheckRangeAddr(addr)	\
		if (((unsigned)addr - FLASH3400_RAM_BASE) >= (FLASH3400_NUMSECTORS * FLASH3400_SECTORSIZE))	\
		{																							\
			printf ("uMON ERROR : flash3400 : address (0x%X) is out of flash region\n", addr);		\
			return -1;																				\
		}


/* Used to correlate between the ID and a string representing the name
 * of the flash device.  Usually, there's only one string; however, some
 * flash device footprints may support compatible devices from different
 * manufacturers; hence, the need for possibly more than one name...
 */
struct flashdesc FlashNamId[] = {
	{ FLASH3400_SYS_DEVID, "UMONDISK" },
	{ 0, 0 }
};


/* Erase the specified sector on the device specified by the flash info
 * pointer.  Return 0 if success, else negative to indicate device-specific
 * error or reason for failure.
 */
int Flash3400_erase(struct flashinfo *fdev, int snum)
{
	flash3400R_CheckRangeSector(snum);
	memset((void*)flash3400R_GetSectorBase(snum), 0xFF, FLASH3400_SECTORSIZE);
	return FLASH_EraseSector((uint8)(fdev->id), FLASH3400_START_SECTOR + snum);
}

static inline unsigned Flash3400_mem2flash_addr(unsigned mem_addr)
{
	unsigned mem_offset = (unsigned)mem_addr - FLASH3400_RAM_BASE;
	return (mem_offset + FLASH3400_START_SECTOR * FLASH3400_SECTORSIZE);
}

static inline unsigned Flash3400_flash2mem_addr(unsigned flash_addr)
{
	unsigned flash_offset = flash_addr + FLASH3400_START_SECTOR * FLASH3400_SECTORSIZE;
	return (FLASH3400_RAM_BASE + flash_offset);
}

/* Write the specified block of data.  Return 0 if success, else negative
 * to indicate device-specific error or reason for failure.
 */
int Flash3400_write(struct flashinfo *fdev, uchar *dest, uchar *src, long bytecnt)
{
    int rc;
	flash3400R_CheckRangeAddr(dest);
	memcpy((void*)dest, (void*)src, bytecnt);
    rc = FLASH_Write((uint8)(fdev->id), 
					Flash3400_mem2flash_addr((unsigned)dest),
					dest, (unsigned)bytecnt);
	return (rc);
}



/* Deal with the flash devices's ability to do a per-sector lock/unlock/query.
 * Note that if the device does not support lock, then the FLASH_LOCKABLE
 * operation should return 0 and all others should return -1.
 */
int Flash3400_lock(struct flashinfo *fdev, int snum, int operation)
{
	vulong	add;

	add = (ulong)(fdev->base);

	if (operation == FLASH_LOCKABLE) {
		return 0;		/* Return 1 if device is lockable; else 0. */
	}
	else if (operation == FLASH_UNLOCK) {
		return -1;		/* Return negative for failure else 1 */
	}
	else if (operation == FLASH_LOCK) {
		return -1;		/* Return negative for failure else 1 */
	}
	else if (operation == FLASH_LOCKQRY) {
		return -1;		/* Return 1 if device is locked else 0. */
	}
	else
		return -1;
}


/* Initialize the sector info table for each flash bank.
 * For most systems, there's only one flash bank, but that's not a rule.
 */
int FlashBankInit(struct flashinfo *fbnk, int snum)
{
	uchar	*saddr;
	int		i, msize;
	struct	sectorinfo *sinfotbl;

	/* Create the per-sector information table.  The size of the table
	 * depends on the number of sectors in the device... 
	 */
	if (fbnk->sectors)
		free((char*)fbnk->sectors);

	msize = fbnk->sectorcnt * sizeof(struct sectorinfo);
	sinfotbl = (struct sectorinfo*) malloc(msize);
	if (!sinfotbl) {
		printf("Can't allocate space (%d bytes) for flash sector info\n", msize);
		return -1;
	}
	fbnk->sectors = sinfotbl;

	/* Using the above-determined sector count and size table, build
	 * the sector information table as part of the flash-bank structure:
	 */
	saddr = fbnk->base;
	for(i=0; i<fbnk->sectorcnt; i++) {
		fbnk->sectors[i].snum	= snum + i;
		fbnk->sectors[i].size	= FLASH3400_SECTORSIZE;
		fbnk->sectors[i].begin	= saddr;
		fbnk->sectors[i].end	= fbnk->sectors[i].begin + fbnk->sectors[i].size - 1;
		fbnk->sectors[i].protected = 0;
		saddr += FLASH3400_SECTORSIZE;
	}
	fbnk->end = saddr - 1;
	return fbnk->sectorcnt;
}


/* Initialize data structures for each bank of flash.  
 * Called by the MicroMonitor core code prior to enabling cache.
 */
int FlashInit(void)
{
	int		snum;
	struct	flashinfo *fbnk;

	snum = 0;

	fbnk = &FlashBank [FlashCurrentBank = 0];

	fbnk->base		= (uint8*) FLASH3400_RAM_BASE;
	fbnk->id		= FLASH3400_SYS_DEVID;
	fbnk->width		= 1; // 1 Byte
	fbnk->sectorcnt = FLASH3400_NUMSECTORS;

	fbnk->fltype	= 0; //Flash3400_type;
	fbnk->flerase	= Flash3400_erase;
	fbnk->flwrite	= Flash3400_write;
	fbnk->flewrite	= 0; //Flash3400_ewrite;
	fbnk->fllock	= Flash3400_lock;

	snum += FlashBankInit(fbnk,snum);

	// KS: May be used for protection specific sectors against writing
	//sectorProtect(FLASH_PROTECT_RANGE,1);

	// Flash present
	FLASH_Read(FLASH3400_SYS_DEVID, (uint8 *)FLASH3400_RAM_BASE,
					Flash3400_mem2flash_addr(FLASH3400_RAM_BASE),
					FLASH3400_NUMSECTORS * FLASH3400_SECTORSIZE);
	return 0;
}


#endif	/* INCLUDE_FLASH */
