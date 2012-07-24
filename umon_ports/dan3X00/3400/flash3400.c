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

#include "config.h"

#if INCLUDE_FLASH

#include "stddefs.h"
#include "genlib.h"
#include "cpu.h"
#include "CGEN.h"
#include "common.h"
#include "flash.h"
#include "flash3400.h"
#include "spi.h"



/**********************************************************************\
							Definitions
\**********************************************************************/

#define FLASH_CMD_PP			0x02	// Program one Page (256 bytes)
#define	FLASH_CMD_READ			0x03	// Read data
#define FLASH_CMD_FAST_READ		0x0B	// Fast read data (> 20 MHz)
#define FLASH_CMD_WRDI			0x04	// Reset WEL (write enable latch) bit on the flash controller
#define FLASH_CMD_WRSR			0x01	// Write status reg
#define FLASH_CMD_RDSR			0x05	// Read status reg
#define FLASH_CMD_WREN			0x06	// Set WEL (write enable latch) bit on the flash controller
#define FLASH_CMD_SE			0xD8	// Sector erase
#define FLASH_CMD_BE			0xC7	// Bulk erase
#define FLASH_CMD_RDID			0x9F	// Read flash ID

#define FLASH_STATUS_WIP		0x01	// Status flag: Write in Process
#define FLASH_STATUS_WEL		0x02	// Status flag: Write Enable Latch
#define FLASH_STATUS_PROT		0x7C


#ifdef DAN2400_SPI
#define	FLASH_FIFO_SIZE			 32
#else
#define	FLASH_FIFO_SIZE			 128
#endif
#define	FLASH_PAGE_SIZE			 256

#define	FLASH_WEL_TIMEOUT		   1	// In seconds
#define	FLASH_WRITE_TIMEOUT		   1	// In seconds
#define	FLASH_ERASE_SEC_TIMEOUT	   10	// In seconds
#define	FLASH_ERASE_ALL_TIMEOUT	   200	// In seconds


/**********************************************************************\
							Macros
\**********************************************************************/

//#define DEBUGFLASH
#ifdef DEBUGFLASH
#define dprint(body)  printf body
#else
#define dprint(body)
#endif
#define derror(body)  printf body


/**********************************************************************\
						Flash low-level functions
\**********************************************************************/

static inline uint8 FLASH_ReadStatusReg(uint8 bank)
{
	uint8 res[4];
	// In order to eliminate a possible problem when the transaction may be 
	// uncompleted, we will repeat 4 same commands
	uint8 cmd[4] = { FLASH_CMD_RDSR, FLASH_CMD_RDSR, FLASH_CMD_RDSR, FLASH_CMD_RDSR };

	// Perform SPI transaction
	SPI_process
	(
        bank,           // bank
		sizeof(cmd),	// len_in
		sizeof(cmd),	// len_out1
		0,				// len_out2
		res,			// inbuf
		cmd,			// outbuf1
		0,				// outbuf2
		0,				// in_skip_start
		0				// in_skip_end
	);

	return res[3]; // last answered byte
}

static inline uint8 FLASH_WriteStatusReg(uint8 bank, uint8 status)
{
	uint8 cmd[2];

	cmd[0] = FLASH_CMD_WRSR;
	cmd[1] = status;

	// Perform SPI transaction
	SPI_process
	(
        bank,           // bank
		sizeof(cmd),	// len_in
		sizeof(cmd),	// len_out1
		0,				// len_out2
		NULL,			// inbuf
		cmd,			// outbuf1
		0,				// outbuf2
		sizeof(cmd),	// in_skip_start
		0				// in_skip_end
	);

	return 0; // last answered byte
}

static int FLASH_WaitStatusWel(uint8 bank)
{
	unsigned tstart  = CPU_GetCurrentClockCount();
	unsigned timeout = FLASH_WEL_TIMEOUT * CPU_SystemClock;
	unsigned telapsed;
	uint8    status;
	do
	{
		telapsed = CPU_GetCurrentClockCount() - tstart;
		if (telapsed > timeout) {
			derror (("FLASH_WaitStatusWel timeout!\n"));
			return -1;
		}

		status = FLASH_ReadStatusReg(bank);
	}
	while (!(status & FLASH_STATUS_WEL));
	return 0;
}


static int FLASH_WaitStatusWip(uint8 bank, uint32 tout)
{
	unsigned tstart  = CPU_GetCurrentClockCount();
	unsigned tout_cnt  = 0;
	unsigned telapsed;
	uint8    status = 0;

#if UMON_TARGET != UMON_TARGET_ARM
	// Do this wait for Tensilica CPUs only. ARM is too slow and this wait will be started after
	// FLASH_STATUS_WIP is already raised and then cleared, in other words after the operation was completed.
	do
	{
		telapsed = CPU_GetCurrentClockCount() - tstart;
		if (telapsed > CPU_SystemClock)
		{
			tout_cnt++;
			if (tout_cnt > FLASH_WRITE_TIMEOUT) {
				derror (("FLASH_WaitStatusWip timeout!\n"));
				//derror(("WIP timeout-1! tstart=%u,telapsed=%u,CPU_SystemClock=%u,tout=%u,tout_cnt=%u,status=0x%02x\n", tstart, telapsed, CPU_SystemClock, tout, tout_cnt, status));
				break;
			}
			tstart = CPU_GetCurrentClockCount();
		}

		status = FLASH_ReadStatusReg(bank);
	}
	while (!(status & FLASH_STATUS_WIP));
#endif

	tstart  = CPU_GetCurrentClockCount();
	tout_cnt = 0;
	do
	{
		telapsed = CPU_GetCurrentClockCount() - tstart;
		if (telapsed > CPU_SystemClock)
		{
			tout_cnt++;
			if (tout_cnt > tout)
			{
				dprint(("WIP timeout!\n"));
				//derror(("WIP timeout-2! tstart=%u,telapsed=%u,CPU_SystemClock=%u,tout=%u,tout_cnt=%u,status=0x%02x\n", tstart, telapsed, CPU_SystemClock, tout, tout_cnt, status));
				return -1;
			}
			tstart = CPU_GetCurrentClockCount();
		}

		status = FLASH_ReadStatusReg(bank);
	}
	while (status & FLASH_STATUS_WIP);
	return 0;
}


void FLASH_Read(uint8 bank, uint8 *data, unsigned addr, unsigned len)
{
	uint8	 cmd[4];
	unsigned i;

	dprint (("FLASH_Read: bank %x, data %x, addr %x, len %x\n", bank, data, addr, len));

	cmd[0] = FLASH_CMD_READ;
	for (i = 0; i < len; i += FLASH_FIFO_SIZE)
	{
		unsigned page_size = MIN(len - i, FLASH_FIFO_SIZE);

		// Prepare flash read command
		cmd[1] = ((uint8)(addr >> 16));
		cmd[2] = ((uint8)(addr >> 8));
		cmd[3] = ((uint8)(addr));

		// Pre-fetch data to cache (touch) to avoid line fill on write
		PLATFORM_cache_control((void *)data, page_size, CACHE_OP_TOUCH);

		// Perform SPI transaction
		SPI_process
		(
            bank,                       // bank
			page_size + sizeof(cmd),	// len_in
			sizeof(cmd),				// len_out1
			0,							// len_out2
			data,						// inbuf
			cmd,						// outbuf1
			0,							// outbuf2
			sizeof(cmd),				// in_skip_start
			0							// in_skip_end
		);

		data += page_size;
		addr += page_size;
	}
}

static inline void FLASH_WriteEnable(uint8 bank)
{
    uint8  res, cmd = FLASH_CMD_WREN;

	SPI_process
	(
        bank,           // bank
		sizeof(cmd),	// len_in
		sizeof(cmd),	// len_out1
		0,				// len_out2
		&res,			// inbuf
		&cmd,			// outbuf1
		0,				// outbuf2
		0,				// in_skip_start
		0				// in_skip_end
	);
}

static inline void FLASH_WriteDisable(uint8 bank)
{
    uint8  res, cmd = FLASH_CMD_WRDI;

	SPI_process
	(
        bank,           // bank
		sizeof(cmd),	// len_in
		sizeof(cmd),	// len_out1
		0,				// len_out2
		&res,			// inbuf
		&cmd,			// outbuf1
		0,				// outbuf2
		0,				// in_skip_start
		0				// in_skip_end
	);
}

int FLASH_EraseSector(uint8 bank, unsigned secnum)
{
	uint8 cmd[4];

	dprint (("FLASH_EraseSector: bank %x, secnum %x\n", bank, secnum));

	FLASH_WriteEnable(bank);
	if (FLASH_WaitStatusWel(bank) < 0)
		return -1;

	// Prepare flash read command
	cmd[0] = FLASH_CMD_SE;
	cmd[1] = (uint8)secnum;
	cmd[2] = cmd[3] = 0;

	// Perform SPI transaction
	SPI_process
	(
        bank,           // bank
		sizeof(cmd),	// len_in
		sizeof(cmd),	// len_out1
		0,				// len_out2
		0,				// inbuf
		cmd,			// outbuf1
		0,				// outbuf2
		sizeof(cmd),	// in_skip_start
		0				// in_skip_end
	);

	if (FLASH_WaitStatusWip(bank, FLASH_ERASE_SEC_TIMEOUT) < 0)
		return -1;

	return 0;
}

static inline int FLASH_EraseAll(uint8 bank)
{
	uint8 cmd[1];

	dprint (("FLASH_EraseAll: bank %u\n", bank));

	FLASH_WriteEnable(bank);
	if (FLASH_WaitStatusWel(bank) < 0)
		return -1;

	// Prepare flash read command
	cmd[0] = FLASH_CMD_BE;

	// Perform SPI transaction
	SPI_process
	(
        bank,           // bank
		sizeof(cmd),	// len_in
		sizeof(cmd),	// len_out1
		0,				// len_out2
		0,				// inbuf
		cmd,			// outbuf1
		0,				// outbuf2
		sizeof(cmd),	// in_skip_start
		0				// in_skip_end
	);

	if (FLASH_WaitStatusWip(bank, FLASH_ERASE_ALL_TIMEOUT) < 0)
		return -1;

	return 0;
}

static inline int FLASH_ProgramPage(uint8 bank, unsigned addr, uint8* data, unsigned len)
{
	uint8	 cmd[4];

	// ASSERT (len <= FLASH_FIFO_SIZE)

	// Prepare flash read command
	cmd[0] = FLASH_CMD_PP;
	cmd[1] = ((uint8)(addr >> 16));
	cmd[2] = ((uint8)(addr >> 8));
	cmd[3] = ((uint8)(addr));

	SPI_process
	(
        bank,               // bank
		len + sizeof(cmd),	// len_in
		sizeof(cmd),		// len_out1
		len,				// len_out2
		0,					// inbuf
		cmd,				// outbuf1
		data,				// outbuf2
		sizeof(cmd),		// in_skip_start
		len					// in_skip_end
	);

	if (FLASH_WaitStatusWip(bank, FLASH_WRITE_TIMEOUT) < 0)
		return -1;
	
	return 0;
}


int FLASH_Write(uint8 bank, unsigned addr, uint8* data, unsigned len)
{
	unsigned i;

	dprint (("FLASH_Write: bank %x, addr %x, data %x, len %x\n", bank, addr, data, len));

	for (i = 0; i < len; )
	{
		unsigned page_size = MIN(FLASH_PAGE_SIZE - (addr & (FLASH_PAGE_SIZE-1)), len - i);
		if (page_size)
		{
			FLASH_WriteEnable(bank);
			if (FLASH_WaitStatusWel(bank) < 0)
				return -1;

			if (page_size <= FLASH_FIFO_SIZE) 
            {
				FLASH_ProgramPage(bank, addr, data, page_size);
			}
			else 
            {
                int len_rest = page_size;
                unsigned offset = 0;
                do
                {
                    len_rest -= FLASH_FIFO_SIZE;
                    FLASH_ProgramPage(bank, addr + offset, data + offset, FLASH_FIFO_SIZE);
                    offset += FLASH_FIFO_SIZE;

                    FLASH_WriteEnable(bank);
                    if (FLASH_WaitStatusWel(bank) < 0)
                        return -1;
                }
                while (len_rest > FLASH_FIFO_SIZE);
				FLASH_ProgramPage(bank, addr + offset, data + offset, len_rest);
				// Send the rest data
			}

			data += page_size;
			addr += page_size;
			i    += page_size;
		}
	}

	return 0 /*Ok*/;
}


#if 0
void FLASH_Read_ID(uint8 bank, uint32 *jedec, uint16 *ext_jedec)
{
	uint8	 cmd;
	uint8	 data[5];

	cmd = FLASH_CMD_RDID;
	SPI_process
	(
        bank,                       // bank
		sizeof(data),				// len_in
		sizeof(cmd),				// len_out1
		0,							// len_out2
		data,						// inbuf
		&cmd,						// outbuf1
		0,							// outbuf2
		sizeof(cmd),				// in_skip_start
		0							// in_skip_end
	);
	*jedec = ((uint32)data[0] << 16) | ((uint32)data[1] << 8) | ((uint32)data[2]);
	*ext_jedec = ((uint32)data[3] << 8) | ((uint32)data[4]);
}
#endif


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
	dprint (("Flash3400_erase: fdev = %x, snum = %x\n", fdev, snum));
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
	unsigned flash_offset = flash_addr + 
				FLASH3400_START_SECTOR * FLASH3400_SECTORSIZE;
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
#ifdef CHECK_WRITTEN_TO_FLASH
    if (rc == 0)
    {
        printf("Checking the written info by read ... ");
        memset((void *)dest, 0, (unsigned)bytecnt);
        FLASH_Read((uint8)(fdev->id), dest, 
					Flash3400_mem2flash_addr((unsigned)dest),
					(unsigned)bytecnt);
        rc = memcmp((void *)dest, (void *)src, (unsigned)bytecnt);
        printf("%s\n", rc ? "Fail" : "Ok");
    }
#endif
	return (rc);
}


/* 
 * Erase all sectors that are part of the address space to be written,
 * then write the data to that address space.  This is essentially a
 * concatenation of the above erase & write done in one step.  This is used
 * primarily by uMon's ability to update itself; because after the boot
 * code is erased, there is nowhere to return so the re-write must be done
 * while executing out of ram also.  Note that this function is only needed
 * in systems that are executing the monitor out of the same device that
 * is being updated.
 */
#if 0
// KS: Disabled flash functions. 
//	   Not needed for the 1st phase (or at all).
int Flash3400_ewrite(struct flashinfo *fdev, uchar *dest, uchar *src, long bytecnt)
{
	/* Upon completion of the 'ewrite' operation, jump to the reset point
	 * to restart the monitor.  Note that you can't return because the
	 * code that called this function has been replaced.
	 */
	{
	#ifdef RESETMACRO
	RESETMACRO();
	#else
    void    (*reset)();
    reset = RESETFUNC();
    reset();
	#endif
	}
	/* Should not get here. */
	return 0;
}


/* Run the AUTOSELECT algorithm to retrieve the manufacturer and
 * device id of the flash.
 */
int Flash3400_type(struct flashinfo *fdev)
{
	// Define 'ftype' to be vuchar, vushort or vulong depending on the
	// width of the flash bank in this driver.
	ftype preval;

	/* Sample the data.
	 */
	preval = FLASH_READBASE();

	/* Add code here to read the device's id. */
	
	/* Wait for the original data to be readable...
	 */
	for(i=0;i<SR_WAIT;i++) {
		if (FLASH_READBASE() == preval)
			break;
	}
	return((int)(fdev->id));
}
#endif



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
    SPI_set_speed(SPI_SPEED_WITHFIX);
	FLASH_Read(FLASH3400_SYS_DEVID, (uint8 *)FLASH3400_RAM_BASE,
					Flash3400_mem2flash_addr(FLASH3400_RAM_BASE),
					FLASH3400_NUMSECTORS * FLASH3400_SECTORSIZE);
	return 0;
}

void spifQinit(void)
{
}

int spifInit(void)
{
	return (0);
}

int spifWaitForReady(void)
{
	return (FLASH_WaitStatusWip(SPIF_device, FLASH_WRITE_TIMEOUT));
}

int spifId(int verbose)
{
	return (SPIF_device);
}

int spifWriteEnable(void)
{
	FLASH_WriteEnable(SPIF_device);
	return (0);
}

int spifWriteDisable(void)
{
	FLASH_WriteDisable(SPIF_device);
	return (0);
}

int spifChipErase(void)
{
	return (FLASH_EraseAll(SPIF_device));
}

int spifGlobalUnprotect(void)
{
	FLASH_WriteEnable(SPIF_device);
	if (FLASH_WaitStatusWel(SPIF_device) < 0)
		return -1;
	FLASH_WriteStatusReg(SPIF_device, 0);
	return (FLASH_WaitStatusWip(SPIF_device, FLASH_WRITE_TIMEOUT));
}

int spifGlobalProtect(void)
{
	FLASH_WriteEnable(SPIF_device);
	if (FLASH_WaitStatusWel(SPIF_device) < 0)
		return -1;
	FLASH_WriteStatusReg(SPIF_device, FLASH_STATUS_PROT);
	return (FLASH_WaitStatusWip(SPIF_device, FLASH_WRITE_TIMEOUT));
}

int spifReadBlock(unsigned long addr,char *data,int len)
{
	FLASH_Read((uint8)(SPIF_device), (uint8 *)data,
					addr, (unsigned)len);
	return (0);
}

int spifWriteBlock(unsigned long addr, char *data, int len)
{
	int rc = FLASH_Write((uint8)(SPIF_device),
					addr, (uint8 *)data, (unsigned)len);
	return (rc);
}

int spifBlockErase(int bsize, long addr)
{
	int rc = 0;
	unsigned sector = addr / FLASH3400_SECTORSIZE;
	while (bsize > 0 && rc == 0)
	{
		rc = FLASH_EraseSector((uint8)(SPIF_device), sector++);
		bsize -= FLASH3400_SECTORSIZE;
		
	}
	return (rc);
}

unsigned short spifReadStatus(int verbose)
{
	uint8 status = FLASH_ReadStatusReg(SPIF_device);
	if (verbose)
		printf("SPIF: bank: %u, status: 0x%02x\n", SPIF_device, status);
	return (status);
}

#endif	/* INCLUDE_FLASH */
