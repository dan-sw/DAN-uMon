#include "config.h"
#include "stddefs.h"
#include "cpuio.h"
#include "genlib.h"
#include "cache.h"
#include "warmstart.h"
#include "cli.h"
#include "cdefBF537.h"
#include "cycles.h"

extern void icache_off(void);
extern void dcache_off(void);

/* Cache configuration for the BF537 STAMP Eval Board:
 * To enable cache, the blackfin requires that the MMU be turned on.
 * If the MMU is turned on, then all memory accesses pass through the
 * protection established by the xCPLB_DATAn and xCPLB_ADDRn registers
 * for both instruction and data space.
 * To keep things simple, but still provide the ability to run with
 * cache enabled, the icache_on() and dcache_on() functions below set up
 * a memory configuration that *almost* covers all of the memory space
 * on the BF537 STAMP board.
 * I say 'almost' because there are only 16 configuration registers, but
 * there is 64M of sdram and 4M of flash, plus there's the internal L1
 * memory space.  All of this has to be mapped into the MMU if it is to
 * be accessible.  Since each xCPLB register set has a max page size of
 * 4M, then a max of 64M can be mapped at any given time.  
 * So, to avoid the need to have exception handlers deal with the hits, 
 * the map is set up as follows:
 *
 *
 *   -----------------------------------------------------------------
 *   | Address range             | mapped? | icache on? | dcache on? |
 *   -----------------------------------------------------------------
 *   | SDRAM:                                                        |
 *   | 0x03c00000 - 0x03ffffff   |   Y     |     Y      |     Y      |
 *   -----------------------------------------------------------------
 *   | 0x03800000 - 0x03bfffff   |   N     |     -      |     -      |
 *   -----------------------------------------------------------------
 *   | 0x03400000 - 0x037fffff   |   N     |     -      |     -      |
 *   -----------------------------------------------------------------
 *   | 0x03000000 - 0x033fffff   |   Y     |     N      |     N      |
 *   -----------------------------------------------------------------
 *   | 0x02c00000 - 0x02ffffff   |   Y     |     N      |     N      |
 *   -----------------------------------------------------------------
 *   | 0x02800000 - 0x02bfffff   |   Y     |     N      |     N      |
 *   -----------------------------------------------------------------
 *   | 0x02400000 - 0x027fffff   |   Y     |     N      |     N      |
 *   -----------------------------------------------------------------
 *   | 0x02000000 - 0x023fffff   |   Y     |     N      |     N      |
 *   -----------------------------------------------------------------
 *   | 0x01c00000 - 0x01ffffff   |   Y     |     N      |     N      |
 *   -----------------------------------------------------------------
 *   | 0x01800000 - 0x01bfffff   |   Y     |     N      |     N      |
 *   -----------------------------------------------------------------
 *   | 0x01400000 - 0x017fffff   |   Y     |     N      |     N      |
 *   -----------------------------------------------------------------
 *   | 0x01000000 - 0x013fffff   |   Y     |     N      |     N      |
 *   -----------------------------------------------------------------
 *   | 0x00c00000 - 0x00ffffff   |   Y     |     N      |     N      |
 *   -----------------------------------------------------------------
 *   | 0x00800000 - 0x00bfffff   |   Y     |     N      |     N      |
 *   -----------------------------------------------------------------
 *   | 0x00400000 - 0x007fffff   |   Y     |     N      |     N      |
 *   -----------------------------------------------------------------
 *   | 0x00000000 - 0x003fffff   |   Y     |     N      |     N      |
 *   -----------------------------------------------------------------
 *   | FLASH:
 *   | 0x20000000 - 0x023fffff   |   Y     |     N      |     N      |
 *   -----------------------------------------------------------------
 *   | ISRAM (mapped area is larger than actual space):
 *   | 0xffa00000 - 0xffafffff   |   Y     |     N      |     N      |
 *   -----------------------------------------------------------------
 *   | DSRAM (mapped area is larger than actual space):
 *   | 0xff800000 - 0xff9fffff   |   Y     |     N      |     N      |
 *   -----------------------------------------------------------------
 *
 * Notice that this scheme essentially 'wastes' 8Mg of space in SDRAM
 * but it eliminates the need to have the bootmonitor use exception 
 * handlers to catch hits to that space.  Plus, the downloaded application
 * always has the option to reconfigure the space anyway.
 */
 

#define DCPLB_BITS PAGE_SIZE_4MB | CPLB_L1_AOW | CPLB_WT | CPLB_USER_RD | CPLB_USER_WR | CPLB_SUPV_WR | CPLB_VALID

#define ICPLB_BITS PAGE_SIZE_4MB | CPLB_USER_RD | CPLB_VALID

void
icache_on(void)
{
	icache_off();

	/* Map all memory space used on BF537...
	 * 64Mg of SDRAM, 4Mg of flash, and the L1 instruction ram.	
	 * The upper 4Mg of SDRAM is cacheable, all other space is non-cacheable.
	 */

	/* SDRAM...
	 */
	asm("ssync;");
	*pICPLB_DATA0 = ICPLB_BITS | CPLB_L1_CHBL;
	asm("csync;");
	*pICPLB_ADDR0 = (void *)0x03C00000;
	asm("csync;");
	*pICPLB_DATA1 = ICPLB_BITS;
	asm("csync;");
	*pICPLB_ADDR1 = (void *)0x03000000;
	asm("csync;");
	*pICPLB_DATA2 = ICPLB_BITS;
	asm("csync;");
	*pICPLB_ADDR2 = (void *)0x02C00000;
	asm("csync;");
	*pICPLB_DATA3 = ICPLB_BITS;
	asm("csync;");
	*pICPLB_ADDR3 = (void *)0x02800000;
	asm("csync;");
	*pICPLB_DATA4 = ICPLB_BITS;
	asm("csync;");
	*pICPLB_ADDR4 = (void *)0x02400000;
	asm("csync;");
	*pICPLB_DATA5 = ICPLB_BITS;
	asm("csync;");
	*pICPLB_ADDR5 = (void *)0x02000000;
	asm("csync;");
	*pICPLB_DATA6 = ICPLB_BITS;
	asm("csync;");
	*pICPLB_ADDR6 = (void *)0x01C00000;
	asm("csync;");
	*pICPLB_DATA7 = ICPLB_BITS;
	asm("csync;");
	*pICPLB_ADDR7 = (void *)0x01800000;
	asm("csync;");
	*pICPLB_DATA8 = ICPLB_BITS;
	asm("csync;");
	*pICPLB_ADDR8 = (void *)0x01400000;
	asm("csync;");
	*pICPLB_DATA9 = ICPLB_BITS;
	asm("csync;");
	*pICPLB_ADDR9 = (void *)0x01000000;
	asm("csync;");
	*pICPLB_DATA10 = ICPLB_BITS;
	asm("csync;");
	*pICPLB_ADDR10 = (void *)0x00C00000;
	asm("csync;");
	*pICPLB_DATA11 = ICPLB_BITS;
	asm("csync;");
	*pICPLB_ADDR11 = (void *)0x00800000;
	asm("csync;");
	*pICPLB_DATA12 = ICPLB_BITS;
	asm("csync;");
	*pICPLB_ADDR12 = (void *)0x00400000;
	asm("csync;");
	*pICPLB_DATA13 = ICPLB_BITS;
	asm("csync;");
	*pICPLB_ADDR13 = (void *)0x00000000;
	asm("csync;");

	/* FLASH...
	 */
	*pICPLB_DATA14 = ICPLB_BITS;
	asm("csync;");
	*pICPLB_ADDR14 = (void *)0x20000000;
	asm("csync;");

	/* L1 Instruction Ram...
	 */
	*pICPLB_DATA15 = 0x00020005;
	asm("csync;");
	*pICPLB_ADDR15 = (void *)0xffA00000;
	asm("csync;");

	*pIMEM_CONTROL = 0x00000007;
	asm("ssync;");
}

void
dcache_on(void)
{
	dcache_off();

	/* Map all memory space used on BF537...
	 * 64Mg of SDRAM, 4Mg of flash, and the L1 data ram.	
	 * The upper 4Mg of SDRAM is cacheable, all other space is non-cacheable.
	 */

	/* SDRAM...
	 */
	asm("ssync;");
	*pDCPLB_DATA0 = DCPLB_BITS | CPLB_L1_CHBL;
	asm("csync;");
	*pDCPLB_ADDR0 = (void *)0x03C00000;
	asm("csync;");
	*pDCPLB_DATA1 = DCPLB_BITS;
	asm("csync;");
	*pDCPLB_ADDR1 = (void *)0x03000000;
	asm("csync;");
	*pDCPLB_DATA2 = DCPLB_BITS;
	asm("csync;");
	*pDCPLB_ADDR2 = (void *)0x02C00000;
	asm("csync;");
	*pDCPLB_DATA3 = DCPLB_BITS;
	asm("csync;");
	*pDCPLB_ADDR3 = (void *)0x02800000;
	asm("csync;");
	*pDCPLB_DATA4 = DCPLB_BITS;
	asm("csync;");
	*pDCPLB_ADDR4 = (void *)0x02400000;
	asm("csync;");
	*pDCPLB_DATA5 = DCPLB_BITS;
	asm("csync;");
	*pDCPLB_ADDR5 = (void *)0x02000000;
	asm("csync;");
	*pDCPLB_DATA6 = DCPLB_BITS;
	asm("csync;");
	*pDCPLB_ADDR6 = (void *)0x01C00000;
	asm("csync;");
	*pDCPLB_DATA7 = DCPLB_BITS;
	asm("csync;");
	*pDCPLB_ADDR7 = (void *)0x01800000;
	asm("csync;");
	*pDCPLB_DATA8 = DCPLB_BITS;
	asm("csync;");
	*pDCPLB_ADDR8 = (void *)0x01400000;
	asm("csync;");
	*pDCPLB_DATA9 = DCPLB_BITS;
	asm("csync;");
	*pDCPLB_ADDR9 = (void *)0x01000000;
	asm("csync;");
	*pDCPLB_DATA10 = DCPLB_BITS;
	asm("csync;");
	*pDCPLB_ADDR10 = (void *)0x00C00000;
	asm("csync;");
	*pDCPLB_DATA11 = DCPLB_BITS;
	asm("csync;");
	*pDCPLB_ADDR11 = (void *)0x00800000;
	asm("csync;");
	*pDCPLB_DATA12 = DCPLB_BITS;
	asm("csync;");
	*pDCPLB_ADDR12 = (void *)0x00400000;
	asm("csync;");
	*pDCPLB_DATA13 = DCPLB_BITS;
	asm("csync;");
	*pDCPLB_ADDR13 = (void *)0x00000000;
	asm("csync;");

	/* FLASH...
	 */
	*pDCPLB_DATA14 = DCPLB_BITS;
	asm("csync;");
	*pDCPLB_ADDR14 = (void *)0x20000000;
	asm("csync;");

	/* L1 Data Ram...
	 */
	*pDCPLB_DATA15 = DCPLB_BITS;
	asm("csync;");
	*pDCPLB_ADDR15 = (void *)0xff800000;
	asm("csync;");

	*pDMEM_CONTROL = 0x0000100b;
	asm("ssync;");
}

int
dmaxfer(char *src, char *dest, long len)
{
	*pMDMA_S0_START_ADDR = src;
	*pMDMA_D0_START_ADDR = dest;
	*pMDMA_S0_X_COUNT = len;
	*pMDMA_D0_X_COUNT = len;
	*pMDMA_D0_CURR_X_COUNT = len;
	*pMDMA_S0_X_MODIFY = 1;
	*pMDMA_D0_X_MODIFY = 1;
	*pMDMA_S0_CONFIG = DMAEN;
	*pMDMA_D0_CONFIG = DMAEN | WNR;

	while(*pMDMA_D0_CURR_X_COUNT != 0);
	return(0);
}


/* tfsld_memcpy() & tfsld_decompress():
 * Front end to all memcpy and decompression calls done by the loader
 * (refer to umon_main/target/common/tfsloader.c for generic comments).
 *
 * This function replaces the standard function used by TFS
 * because with Blackfin's L1 instruction space, you need to use
 * DMA to copy instructions into that area; hence, we can't just use
 * simply memcpy()...
 */
int
tfsld_memcpy(char *to, char *from, int count, int verbose, int verify)
{
	int rc;

	if (verbose <= 1) {
		if (inUmonBssSpace(to,to+count-1))
			return(-1);
	}

	if ((to >= (char *)BFIN_L1I_START) && (to <= (char *)BFIN_L1I_END)) {
		if (verify) {
			printf("verify-only mode not yet working for L1 space\n");
			return(0);
		}
		if (verbose) {
			printf("%s %7d bytes from 0x%08lx to 0x%08lx\n",
				verify ? "vrfy" : "copy", count,(ulong)from,(ulong)to);
		}
		rc = dmaxfer(from, to, (long)count);
	}
	else {
		rc = s_memcpy(to,from,count,verbose,verify);

		if (verbose <= 1) {
			flushDcache(to,count);
			invalidateIcache(to,count);
		}
	}
	return(rc);
}

int
tfsld_decompress(char *src, int srclen, char *dest, int verbose, int /*align4*/)
{
	int outsize;

	/* Note that this check for decompression into uMon's bss space is
	 * of limited value because at this point we don't know what the
	 * size of the decompression will be, so we just use the size of
	 * the source... (better than nothing).
	 */
	if (verbose <= 1) {
		if (inUmonBssSpace(dest, (char *)dest+srclen-1))
			return(-1);
	}

	if ((dest >= (char *)BFIN_L1I_START) && (dest <= (char *)BFIN_L1I_END)) {
		char *unzipto, *env;

		env = getenv("TFSLD_UNZIPTO");
		if (!env) {
			printf("Error: TFSLD_UNZIPTO not set\n");
			return(-1);
		}
	
		unzipto = (char *)strtoul(env,0,0);
		outsize = decompress(src,srclen,unzipto);

		if (dmaxfer(unzipto, dest, outsize) < 0)
			return(-1);
	}
	else {
		outsize = decompress(src,srclen,dest);
	}


	if (verbose)
		printf("dcmp %7d bytes from 0x%08lx to 0x%08lx\n",
			outsize,src,dest);

	return(outsize);
}

char *BfinCmdHelp[] = {
	"Blackfin-specific operations.",
	"-[v] {bfin-operation} [op-specific-args]",
#if INCLUDE_VERBOSEHELP
	"Options:",
	" -v   verbose",
	"",
	"Operations:",
	" csync",
	" ssync",
	" dspid",
	" dmax {src} {dst} {len}",
	" dcache-off",
	" dcache-on",
	" icache-off",
	" icache-on",
#endif
	0
};

int
BfinCmd(int argc,char *argv[])
{
	long	len;
	char	*arg, *src, *dst;
	int		opt, verbose;

	while((opt=getopt(argc,argv,"v")) != -1) {
		switch(opt) {
		case 'v':
			verbose = 1;
			break;
		}
	}
	if (argc >= optind+1) {
		arg = argv[optind];

		if (strcmp(arg,"icache-off") == 0) {
			icache_off();
		}
		else if (strcmp(arg,"icache-on") == 0) {
			icache_on();
		}
		else if (strcmp(arg,"dcache-off") == 0) {
			dcache_off();
		}
		else if (strcmp(arg,"dcache-on") == 0) {
			dcache_on();
		}
		else if (strcmp(arg,"csync") == 0) {
			asm("csync;");
		}
		else if (strcmp(arg,"ssync") == 0) {
			asm("ssync;");
		}
		else if (strcmp(arg,"dmax") == 0) {
			if (argc != optind+4)
				return(CMD_PARAM_ERROR);
			src = (char *)strtol(argv[optind+1],0,0);
			dst = (char *)strtol(argv[optind+2],0,0);
			len = strtol(argv[optind+3],0,0);
			if (dmaxfer(src,dst,len) == 0)
				return(CMD_SUCCESS);
			else
				return(CMD_FAILURE);
		}
		else if (strcmp(arg,"dspid") == 0) {
			printf("DSPID: 0x%lx\n",*pDSPID);
		}
		else
			return(CMD_PARAM_ERROR);
	}
	else
		return(CMD_PARAM_ERROR);
	return(CMD_SUCCESS);
}
