#include "config.h"
#include "cli.h"
#include "genlib.h"
#include "stddefs.h"
#include "arch_ppc.h"

/* Miscellaneous, generic PPC405-specific functions.
 */

/* target_reset():
 * For ppc405, set SYS_RESET bits in debug ctrl register 0 to issue
 * a hard reset...
 */
void
target_reset(void)
{
	flush_console_out();
	ppcMtdbcr0(0x70000000);
}

/* target_timer():
 * Used with uMon's hardware-assisted timer facility.
 * Return the current value of the Time-Base-Low register.
 */
ulong
target_timer(void)
{
	volatile unsigned register tbl;
	
	// Retrieve the current value of the TBL register:
	asm volatile ("mfspr %0, 0x10c" : "=r" (tbl));

	return(tbl);
}

void
intsrestore(ulong msr)
{
	ppcMtmsr(msr);
    return;
}

ulong
intsoff(void)
{
	ulong	msr;

	msr = ppcMfmsr();
	ppcMtmsr(msr & ~MSR_EE);
    return(msr);
}

/* OcmDataConfig():
 *	On-Chip-Memory data config...
 *	Map the 4K of on-chip memory to start at that point.
 *  If the incoming base value is 0xffffffff, then disable the OCM
 *  data accessibility.
 */
void
OcmDataConfig(ulong base)
{
	ppcMtocm0isarc(0);
	ppcMtocm0isctl(0);
	if (base == 0xffffffff) {
		ppcMtocm0dsarc(0);
		ppcMtocm0dsctl(0);
	}
	else {
		ppcMtocm0dsarc(base & 0xfc000000);
		ppcMtocm0dsctl(0xc0000000);
	}
}

void
watchdog_check(void)
{
	volatile unsigned long register tsr_reg;

	/* Read the TSR register to see if the reset occurred due to a 
	 * watchdog timeout...  Report status if it did.
	 *
	 * This asm syntax is discussed in the GNU-GCC on-line manual
	 * section titled:
	 *    "Assembler Instructions with C Expression Operands".
	 */
	asm volatile ("mfspr %0, 0x3d8" : "=r" (tsr_reg));

	// printf("TSR: %08x\n",tsr_reg);

	switch(tsr_reg & 0x30000000) {
		case 0x00000000:
			break;
		case 0x10000000:
			printf("Watchog core reset\n");
			break;
		case 0x20000000:
			printf("Watchog core reset\n");
			break;
		case 0x30000000:
			printf("Watchog system reset\n");
			break;
	}

	/* Clear the TSR watchdog reset status bits:
	 */
	tsr_reg = 0x70000000;
	asm volatile ("mtspr 0x3d8, %0" : : "r" (tsr_reg));
}


#if INCLUDE_PBERRCHECK

#define EET0 0xE0000000
#define RWS0 0x10000000
#define EET1 0x03800000
#define RWS1 0x00400000
#define EET2 0x000E0000
#define RWS2 0x00010000
#define EET3 0x00003800
#define RWS3 0x00000400
#define FL3  0x00000200
#define AL3  0x00000100
#define EET4 0xE0000000
#define RWS4 0x10000000
#define FL4	 0x08000000
#define AL4	 0x04000000
#define EET5 0x03800000
#define RWS5 0x00400000
#define FL5	 0x00200000
#define AL5	 0x00100000

char *
pberr(int errno)
{
	switch(errno) {
		case 1:
			return("parity");
		case 4:
			return("protection");
		case 6:
			return("ext bus input");
		case 7:
			return("ext bus timeout");
		case 2:
		case 3:
		case 5:
			return("reserved");
		default:
			return("???");
	}
}

char *
pbrow(int readorwrite)
{
	if (readorwrite)
		return("read");
	return("write");
}

/* Peripheral bus error status reporting...
 */
void
pberror_check(void)
{
	volatile unsigned register reg_pbesr0, reg_pbesr1, reg_bear, rnum;

	/* The bear, pbesr0 and pbesr1 registers are accessed 
	 * indirectly through DCR 0x12 & 0x13...
	 */

	rnum = 0x21;
	asm volatile ("mtdcr 0x12, %0" : : "r" (rnum));
	asm volatile ("mfdcr %0, 0x13" : "=r" (reg_pbesr0));

	rnum = 0x22;
	asm volatile ("mtdcr 0x12, %0" : : "r" (rnum));
	asm volatile ("mfdcr %0, 0x13" : "=r" (reg_pbesr1));

	if ((reg_pbesr0 & (EET0|EET1|EET2|EET3)) || (reg_pbesr1 & (EET4|EET5))) {
		printf("Peripheral bus error status:\n");

		if (reg_pbesr0 & EET0) {
			printf("Instruction fetcher '%s' error (%s)\n",
				pberr((reg_pbesr0 & EET0) >> 29), pbrow(reg_pbesr0 & RWS0));
		}
		if (reg_pbesr0 & EET1) {
			printf("Data access '%s' error (%s)\n",
				pberr((reg_pbesr0 & EET1) >> 23), pbrow(reg_pbesr0 & RWS1));
		}
		if (reg_pbesr0 & EET2) {
			printf("Ext bus master '%s' error (%s)\n",
				pberr((reg_pbesr0 & EET2) >> 17), pbrow(reg_pbesr0 & RWS2));
		}
		if (reg_pbesr0 & EET3) {
			printf("PCI bridge '%s' error (%s)\n",
				pberr((reg_pbesr0 & EET3) >> 11), pbrow(reg_pbesr0 & RWS3));
		}
		if (reg_pbesr1 & EET4) {
			printf("MAL '%s' error (%s)\n",
				pberr((reg_pbesr1 & EET4) >> 29), pbrow(reg_pbesr1 & RWS4));
		}
		if (reg_pbesr1 & EET5) {
			printf("DMA ctrlr '%s' error (%s)\n",
				pberr((reg_pbesr1 & EET5) >> 23), pbrow(reg_pbesr1 & RWS5));
		}

		rnum = 0x20;
		asm volatile ("mtdcr 0x12, %0" : : "r" (rnum));
		asm volatile ("mfdcr %0, 0x13" : "=r" (reg_bear));

		printf("Bus error address: 0x%x\n",reg_bear); 
	}

	/* Clear the Error status registers:
	 */
	rnum = 0x21;
	reg_pbesr0 = 0;
	asm volatile ("mtdcr 0x12, %0" : : "r" (rnum));
	asm volatile ("mtdcr 0x13, %0" : : "r" (reg_pbesr0));

	rnum = 0x22;
	reg_pbesr1 = 0;
	asm volatile ("mtdcr 0x12, %0" : : "r" (rnum));
	asm volatile ("mtdcr 0x13, %0" : : "r" (reg_pbesr1));
}
#endif

#if INCLUDE_WDCMD

char *WdHelp[] = {
	"Watchdog",
	"{test | enable}",
	0
};

int
WdCmd(int argc, char *argv[])
{
	static int enabled;

	if (argc != 2)
		return(CMD_PARAM_ERROR);

	if (strcmp(argv[1],"test") == 0) {
		printf("Endless loop with no watchdog tickle...\n");
		if (enabled == 0)
			printf("(it wasn't enabled here)\n");
		while(1);
	}
	else if (strcmp(argv[1],"enable") == 0) {
		volatile unsigned register reg_tsr, reg_tcr;

		enabled = 1;
//		docommand("spr 0x3d8 0x40000000",1);		/* TSR */
//		docommand("spr 0x3da 0xd0000000",1);		/* TCR */
		reg_tsr = 0x40000000;
		reg_tcr = 0xd0000000;
		asm volatile ("mtspr 0x3d8, %0" : : "r" (reg_tsr));
		asm volatile ("mtspr 0x3da, %0" : : "r" (reg_tcr));
	}
	else
		return(CMD_PARAM_ERROR);

	return(CMD_SUCCESS);
}
#endif
