/* cfi.c:
 * Common Flash Interface
 * This code does some basic CFI accesses to the flash.
 * This code is derived two references:
 *
 * 1: Common Flash Memory Interface Specification
 *		Release 2.0 dated December 1, 2001
 * 2: Common Flash Memory Interface Publication 100
 *		Vendor & Device ID Code Assignments
 *		Volume Number 97.1 dated May 30, 1997
 *
 * The goal of this code is to determine the device's maximum width,
 * device's configured width and the number of devices in parallel.
 * NOT YET COMPLETE!!!
 */
#include <string.h>
#include <stdlib.h>
#include "monlib.h"
#include "cfi.h"

#define printf mon_printf

void
cfi_delay()
{
	volatile int i;
	for(i=0;i<CFI_DELAY_COUNT;i++);
}

void
printCFIMode(int mode)
{
	printf("CFI MODE: ",mode);
	if (mode == CFI_MODE_UNKNOWN) {
		printf("  unknown\n");
	}
	else {
		printf("(0x%lx)\n",mode);
		printf("  Device's maximum width = %d bytes\n",CFI_MAXWIDTH(mode));
		printf("  Device's configured width = %d bytes\n",CFI_CFGWIDTH(mode));
		printf("  Number of devices in parallel = %d\n",CFI_PARTOT(mode));
	}
}

/* Query the device to determin the device width and mode.
 */
int
cfi_mode(unsigned long devbase)
{
	volatile unsigned char *cdp;
	volatile unsigned short *sdp;
	volatile unsigned long *ldp;
	char buf[16];

	/* Writing the CFI Query command sequence in each mode until the
	 * CFI ID is read back.  For each test, put the device in CFI 
	 * query mode, pull the CFI data out of the device, then restore
	 * read/reset mode prior to testing the CFI data.
	 */

	cdp = (unsigned char *)devbase;
	sdp = (unsigned short *)devbase;
	ldp = (unsigned long *)devbase;

	/* Test for 1 8-bit device...
	 */
	*(cdp + 0x55) = 0x98;
	memcpy(buf,(char *)(devbase+0x10),sizeof(buf));
	*cdp = 0xf0;
	*cdp = 0xff;
	if (memcmp(buf,"QRY",3) == 0) {
		return(MODE_8_8_1);
	}
	if ((memcmp(buf,"Q\0R\0Y\0",6) == 0) ||
		(memcmp(buf,"\0Q\0R\0Y",6) == 0)) {
		return(MODE_16_8_1);
	}

	/* Test for 1 16-bit device...
	 */
	*(sdp + 0x55) = 0x0098;
	memcpy(buf,(char *)(devbase+0x20),sizeof(buf));
	*sdp = 0x00f0;
	*sdp = 0x00ff;
	if ((memcmp(buf,"Q\0R\0Y\0",6) == 0) ||
		(memcmp(buf,"\0Q\0R\0Y",6) == 0)) {
		return(MODE_16_16_1);
	}
	if ((memcmp(buf,"Q\0\0\0R\0\0\0Y\0\0\0",12) == 0) ||
		(memcmp(buf,"\0\0\0Q\0\0\0R\0\0\0Y",12) == 0)) {
		return(MODE_32_16_1);
	}

	/* Test for 1 32-bit device...
	 */
	*(ldp + 0x55) = 0x00000098;
	memcpy(buf,(char *)(devbase+0x40),sizeof(buf));
	*ldp = 0x000000f0;
	*ldp = 0x000000ff;
	if ((memcmp(buf,"Q\0\0\0R\0\0\0Y\0\0\0",12) == 0) ||
		(memcmp(buf,"\0\0\0Q\0\0\0R\0\0\0Y",12) == 0)) {
		return(MODE_32_32_1);
	}

	/* Test for 2 8-bit devices in parallel...
	 */
	*(sdp + 0x55) = 0x9898;
	memcpy(buf,(char *)(devbase+0x20),sizeof(buf));
	*sdp = 0xf0f0;
	*sdp = 0xffff;
	if (memcmp(buf,"QQRRYY",6) == 0) {
		return(MODE_8_8_2);
	}

	/* Test for 2 16-bit devices in parallel...
	 */
	*(ldp + 0x55) = 0x00980098;
	memcpy(buf,(char *)(devbase+0x40),sizeof(buf));
	*ldp = 0xf0f0f0f0;
	*ldp = 0xffffffff;
	if ((memcmp(buf,"Q\0Q\0R\0R\0Y\0Y\0",12) == 0) ||
		(memcmp(buf,"\0Q\0Q\0R\0R\0Y\0Y",12) == 0)) {
		return(MODE_16_16_2);
	}

	/* Test for 4 8-bit devices in parallel...
	 */
	*(ldp + 0x55) = 0x98989898;
	memcpy(buf,(char *)(devbase+0x40),sizeof(buf));
	*ldp = 0xf0f0f0f0;
	*ldp = 0xffffffff;
	if (memcmp((char *)(devbase+0x40),"QQQQRRRRYYYY",12) == 0) {
		return(MODE_8_8_4);
	}

	return(CFI_MODE_UNKNOWN);
}

int
getCFIMode(unsigned long devbase, int verbose)
{
	int mode;

	mode = cfi_mode(devbase);
	
	if (verbose) 
		printCFIMode(mode);

	return(mode);
}

int
getCFIGeometry(unsigned long devbase, int verbose)
{
	return(0);
}
