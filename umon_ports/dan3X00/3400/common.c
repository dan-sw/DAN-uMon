/*
All files except if stated otherwise in the begining of the file are under the GPLv2 license:
-----------------------------------------------------------------------------------

Copyright (c) 2010-2012 Design Art Networks Ltd.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

-----------------------------------------------------------------------------------
*/
#include "config.h"
#include "stddefs.h"
#include "genlib.h"
#include "warmstart.h"
#include "cpu.h"
#include "CGEN.h"
#include "tmr.h"
#include "common.h"
#include "spi.h"
#include "cli.h"
#include "tfs.h"
#include "tfsprivate.h"
#include "flash.h"
#include "flash3400.h"
#include "ddr.h"

//#define DDR_BURST4
#define ddr0_if_MODULE_ADDR     0xe5044000
#define ddr1_if_MODULE_ADDR     0xe5046000
#define BUFFER_SIZE 256
#define MAX_IMAGE_SIZE (1024*1024)

uint8 SPIF_device = FLASH3400_SYS_DEVID;

#ifdef DAN2400
int upgrade_umon(int argc, char *argv[])
{
	static const unsigned uMon_flash_addr = 0x00300000;
	static const unsigned uMon_temp_addr = 0x10000000;

	unsigned image_size = 0, flash_address;
	int size_to_write;

	if (argc >= 2)
	{
		int fd;
		char *fname = (char*) "//ramfs/umon.gz";
		fname = argv[1];
		fd = tfsopen(fname, TFS_RDONLY, NULL);
		if (fd < 0)
		{
			printf("File not found: %s\n", fname);
			return (fd);
		}
		flash_address = uMon_flash_addr;

		printf("Programming uMon: ");
		do
		{
			static uint8 buffer[BUFFER_SIZE];
			size_to_write = tfsread(fd, (void *)buffer, BUFFER_SIZE);
			if ((size_to_write < 0) && (size_to_write != TFSERR_EOF))
			{
				printf("Error reading file: %s (%d)\n", fname, size_to_write);
				tfsclose(fd, NULL);
				return (size_to_write);
			}
			else if (size_to_write > 0)
			{
				if (flash_address % FLASH3400_SECTORSIZE == 0)
				{
					FLASH_EraseSector(FLASH3400_SYS_DEVID, flash_address / FLASH3400_SECTORSIZE);
					printf(".");
				}
				FLASH_Write(FLASH3400_SYS_DEVID, flash_address, buffer, size_to_write);
				flash_address += size_to_write;
				image_size += size_to_write;
			}
		}
		while (size_to_write > 0);
		printf(" %u bytes Ok\n", image_size);
		tfsclose(fd, NULL);
	}
	else
	{
		flash_address = uMon_flash_addr;

		printf("Programming uMon: ");
		do
		{
			static uint8 buffer[BUFFER_SIZE];
			size_to_write = 0;
			if (image_size < MAX_IMAGE_SIZE)
			{
				size_to_write = BUFFER_SIZE;
				memcpy((void *)buffer, (void *)(uMon_temp_addr + image_size),
						size_to_write);
				if (flash_address % FLASH3400_SECTORSIZE == 0)
				{
					FLASH_EraseSector(FLASH3400_SYS_DEVID,
							flash_address / FLASH3400_SECTORSIZE);
					printf(".");
				}
				FLASH_Write(FLASH3400_SYS_DEVID, flash_address, buffer,
						size_to_write);
				flash_address += size_to_write;
				image_size += size_to_write;
			}
		}
		while (size_to_write > 0);
		printf(" %u bytes Ok\n", image_size);
	}
    return(0);
}

char *upgrade_umon_help[] = {
    "Upgrade specified software element from file",
    "[<filename>]",
    0,
};

#endif



int pll_on(int argc, char *argv[])
{
	uint32 pll_main_mode = 0, main_clock_rate = 600 * 1000000;
	printf("Configure pll ...");
#ifdef DAN2400
#else

	if (argc > 1)
	{
		uint32 rate_mhz = atoi(argv[1]);
		switch (rate_mhz)
		{
			case 600:
				pll_main_mode = 0;
				break;
			case 150:
				pll_main_mode = 1;
				break;
			case 100:
				pll_main_mode = 2;
				break;
			case 200:
				pll_main_mode = 3;
				break;
			case 300:
				pll_main_mode = 4;
				break;
			case 240:
				pll_main_mode = 5;
				break;
			case 270:
				pll_main_mode = 6;
				break;
			default:
				puts("Unsupported frequency, set default 600 MHz");
				rate_mhz = 600;
				break;
		}
		main_clock_rate = rate_mhz * 1000000;
	}

	TMR_usec_delay(100);
	CGEN_PLL0_init(pll_main_mode, FALSE);
	CPU_SystemClock = CGEN_PLL0_get_clk(INPUT_CLOCK_HZ);// 600 MHz default
	CPU_BusClock = CPU_SystemClock / 2;					// Half system clock
	SPI_set_speed(SPI_SPEED);							// <10 MHz maximum
	ConsoleBaudSet(DEFAULT_BAUD_RATE);			// Reconfigure UART
	TMR_usec_delay(100);	
#endif
	printf(" Ok (%u MHz)\n", CPU_SystemClock / 1000000);
	return (0);
}

char *pll_on_help[] = {
    "Switch pll on and initialize it to the default values",
    "<MHz>",
    0,
};

int spif_device(int argc, char *argv[])
{
	if (argc >= 2)
	{
		int8 dev = atoi(argv[1]);
		if ((dev >=0) && (dev <= 3))
		{
			SPIF_device = (uint8)dev;
			printf("SPI flash device set: %u\n", SPIF_device);
		}
		else
		{
			printf("Wrong SPI flash device selected: %d\n", dev);
		}
	}
	else
	{
		SPIF_device = FLASH3400_SYS_DEVID;
		printf("SPI flash device set: %u\n", SPIF_device);
	}
	return (0);
}

char *spif_device_help[] = {
    "Set SPI flash device to work with",
    "<device 0..3>",
    0,
};

// This function must be supplied for the uart16550 driver
// We will always use Baud rate = 115200, it simplifies this func implementation
// The CPU_BusClock variable is defined in CPU file.
int getUartDivisor (int baud, unsigned char *hi, unsigned char *lo)
{
	int bus_clock = CPU_SystemClock / 2;
	int divisor =
			(bus_clock + (8 * DEFAULT_BAUD_RATE)) / (16 * DEFAULT_BAUD_RATE);
	*lo = (unsigned char) (divisor);
	*hi = (unsigned char) (divisor >> 8);
	return 0;
}

void dan3400_hook()
{
	set_version();
}
