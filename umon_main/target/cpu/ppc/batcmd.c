#include "config.h"
#include "cli.h"
#include "genlib.h"
#include "stddefs.h"
#include "arch_ppc.h"

	asm ( ".equ IBAT0U, 528");
	asm ( ".equ IBAT0U,	528");
	asm ( ".equ IBAT0L,	529");
	asm ( ".equ IBAT1U,	530");
	asm ( ".equ IBAT1L,	531");
	asm ( ".equ IBAT2U,	532");
	asm ( ".equ IBAT2L,	533");
	asm ( ".equ IBAT3U,	534");
	asm ( ".equ IBAT3L,	535");
	asm ( ".equ IBAT4U,	560");
	asm ( ".equ IBAT4L,	561");
	asm ( ".equ IBAT5U,	562");
	asm ( ".equ IBAT5L,	563");
	asm ( ".equ IBAT6U,	564");
	asm ( ".equ IBAT6L,	565");
	asm ( ".equ IBAT7U,	566");
	asm ( ".equ IBAT7L,	567");

	asm ( ".equ DBAT0U,	536");
	asm ( ".equ DBAT0L,	537");
	asm ( ".equ DBAT1U,	538");
	asm ( ".equ DBAT1L,	539");
	asm ( ".equ DBAT2U,	540");
	asm ( ".equ DBAT2L,	541");
	asm ( ".equ DBAT3U,	542");
	asm ( ".equ DBAT3L,	543");
	asm ( ".equ DBAT4U,	568");
	asm ( ".equ DBAT4L,	569");
	asm ( ".equ DBAT5U,	570");
	asm ( ".equ DBAT5L,	571");
	asm ( ".equ DBAT6U,	572");
	asm ( ".equ DBAT6L,	573");
	asm ( ".equ DBAT7U,	574");
	asm ( ".equ DBAT7L,	575");

#define IBAT0U	528
#define IBAT0L	529
#define IBAT1U	530
#define IBAT1L	531
#define IBAT2U	532
#define IBAT2L	533
#define IBAT3U	534
#define IBAT3L	535
#define IBAT4U	560
#define IBAT4L	561
#define IBAT5U	562
#define IBAT5L	563
#define IBAT6U	564
#define IBAT6L	565
#define IBAT7U	566
#define IBAT7L	567

#define DBAT0U	536
#define DBAT0L	537
#define DBAT1U	538
#define DBAT1L	539
#define DBAT2U	540
#define DBAT2L	541
#define DBAT3U	542
#define DBAT3L	543
#define DBAT4U	568
#define DBAT4L	569
#define DBAT5U	570
#define DBAT5L	571
#define DBAT6U	572
#define DBAT6L	573
#define DBAT7U	574
#define DBAT7L	575

#define BAT_WIM_INVALID(a)	((a & BAT_WI_MASK) == BAT_WI_MASK)
#define BAT_VALID(a)		((a & BAT_VALID_MASK) != 0)
#define BAT_BLOCKLEN(a) 	((((a & BAT_BL_MASK) >> 2) + 1) * 0x20000)
#define BAT_BASEADDR(a) 	(a & BAT_BRPN_MASK)

/* Command to play with the PPC BAT (Base Address Translation) registers
 */

void
showBAT(int bnum)
{
	volatile unsigned register breg;
	volatile unsigned ibatu, ibatl, dbatu, dbatl;

	switch(bnum) {
		case 0:
			asm volatile ("mfspr %0, IBAT0U" : "=r" (breg));
			ibatu = breg;
			asm volatile ("mfspr %0, IBAT0L" : "=r" (breg));
			ibatl = breg;
			asm volatile ("mfspr %0, DBAT0U" : "=r" (breg));
			dbatu = breg;
			asm volatile ("mfspr %0, DBAT0L" : "=r" (breg));
			dbatl = breg;
			break;
		case 1:
			asm volatile ("mfspr %0, IBAT1U" : "=r" (breg));
			ibatu = breg;
			asm volatile ("mfspr %0, IBAT1L" : "=r" (breg));
			ibatl = breg;
			asm volatile ("mfspr %0, DBAT1U" : "=r" (breg));
			dbatu = breg;
			asm volatile ("mfspr %0, DBAT1L" : "=r" (breg));
			dbatl = breg;
			break;
		case 2:
			asm volatile ("mfspr %0, IBAT2U" : "=r" (breg));
			ibatu = breg;
			asm volatile ("mfspr %0, IBAT2L" : "=r" (breg));
			ibatl = breg;
			asm volatile ("mfspr %0, DBAT2U" : "=r" (breg));
			dbatu = breg;
			asm volatile ("mfspr %0, DBAT2L" : "=r" (breg));
			dbatl = breg;
			break;
		case 3:
			asm volatile ("mfspr %0, IBAT3U" : "=r" (breg));
			ibatu = breg;
			asm volatile ("mfspr %0, IBAT3L" : "=r" (breg));
			ibatl = breg;
			asm volatile ("mfspr %0, DBAT3U" : "=r" (breg));
			dbatu = breg;
			asm volatile ("mfspr %0, DBAT3L" : "=r" (breg));
			dbatl = breg;
			break;
		case 4:
			asm volatile ("mfspr %0, IBAT4U" : "=r" (breg));
			ibatu = breg;
			asm volatile ("mfspr %0, IBAT4L" : "=r" (breg));
			ibatl = breg;
			asm volatile ("mfspr %0, DBAT4U" : "=r" (breg));
			dbatu = breg;
			asm volatile ("mfspr %0, DBAT4L" : "=r" (breg));
			dbatl = breg;
			break;
		case 5:
			asm volatile ("mfspr %0, IBAT5U" : "=r" (breg));
			ibatu = breg;
			asm volatile ("mfspr %0, IBAT5L" : "=r" (breg));
			ibatl = breg;
			asm volatile ("mfspr %0, DBAT5U" : "=r" (breg));
			dbatu = breg;
			asm volatile ("mfspr %0, DBAT5L" : "=r" (breg));
			dbatl = breg;
			break;
		case 6:
			asm volatile ("mfspr %0, IBAT6U" : "=r" (breg));
			ibatu = breg;
			asm volatile ("mfspr %0, IBAT6L" : "=r" (breg));
			ibatl = breg;
			asm volatile ("mfspr %0, DBAT6U" : "=r" (breg));
			dbatu = breg;
			asm volatile ("mfspr %0, DBAT6L" : "=r" (breg));
			dbatl = breg;
			break;
		case 7:
			asm volatile ("mfspr %0, IBAT7U" : "=r" (breg));
			ibatu = breg;
			asm volatile ("mfspr %0, IBAT7L" : "=r" (breg));
			ibatl = breg;
			asm volatile ("mfspr %0, DBAT7U" : "=r" (breg));
			dbatu = breg;
			asm volatile ("mfspr %0, DBAT7L" : "=r" (breg));
			dbatl = breg;
			break;
	}

	printf("IBAT%d  U: 0x%08lx, L: 0x%08lx...",bnum,ibatu,ibatl);
	if (BAT_VALID(ibatu)) {
		printf("\n");
		if ((ibatu & BAT_BEPI_MASK) == (ibatu & BAT_BRPN_MASK))
			printf("  Address:     0x%08lx-0x%08lx\n", BAT_BASEADDR(ibatu),
				BAT_BASEADDR(ibatu) + BAT_BLOCKLEN(ibatu) - 1);
		
		if (BAT_WIM_INVALID(ibatl)) {
			printf("  *** WARNING: invalid WIM setting.\n");
		}
		else if ((ibatl & BAT_WRITE_THROUGH) || (ibatl & BAT_GUARDED)) {
			printf("  *** WARNING: W & G bits invalid for IBAT.\n");
		}
		else {
			printf("  Cache:      ");
			if (ibatl & BAT_CACHE_INHIBITED)
				printf(" inhibited\n");
			else if (ibatl & BAT_COHERENT)
				printf(" coherent\n");
			else 
				printf(" enabled\n");

			printf("  Valid:      ");
			if (ibatu & BAT_VALID_USER)
				printf(" user");
			if (ibatu & BAT_VALID_SUPERVISOR)
				printf(" supervisor");
			printf("\n");

			printf("  Protection: ");
			if ((ibatl & BAT_PROTECT_MASK) == 0)
				printf(" no access ");
			if (ibatl & BAT_READ_WRITE)
				printf(" read/write ");
			if (ibatl & BAT_READ_ONLY)
				printf(" read only ");
			printf("\n");
		}
	}
	else {
		printf("  Disabled\n");
	}
	printf("\n");

	printf("DBAT%d  U: 0x%08lx, L: 0x%08lx...",bnum,dbatu,dbatl);
	if (BAT_VALID(dbatu)) {
		printf("\n");
		if ((dbatu & BAT_BEPI_MASK) == (dbatu & BAT_BRPN_MASK))
			printf("  Address:     0x%08lx-0x%08lx\n", BAT_BASEADDR(dbatu),
				BAT_BASEADDR(dbatu) + BAT_BLOCKLEN(dbatu) - 1);
		
		if (BAT_WIM_INVALID(dbatl)) {
			printf("  *** WARNING: invalid WIM setting.\n");
		}
		else {
			printf("  Cache:      ");
			if (dbatl & BAT_CACHE_INHIBITED)
				printf(" inhibited");
			else if (dbatl & BAT_COHERENT)
				printf(" coherent");
			else if (dbatl & BAT_WRITE_THROUGH)
				printf(" write-through");
			else if (dbatl & BAT_GUARDED)
				printf(" guarded");
			else 
				printf(" enabled");
			printf("\n");

			printf("  Valid:      ");
			if (dbatu & BAT_VALID_USER)
				printf(" user");
			if (dbatu & BAT_VALID_SUPERVISOR)
				printf(" supervisor");
			printf("\n");

			printf("  Protection: ");
			if ((dbatl & BAT_PROTECT_MASK) == 0)
				printf(" no access ");
			if (dbatl & BAT_READ_WRITE)
				printf(" read/write ");
			if (dbatl & BAT_READ_ONLY)
				printf(" read only ");
			printf("\n");
		}
	}
	else {
		printf("  Disabled\n");
	}
	printf("\n");
}


char *BatHelp[] = {
	"BAT register dump",
	"[bat number]",
	0,
};


int
BatCmd(int argc, char *argv[])
{
	int bnum;

	if (argc == 1) {
		for(bnum=0;bnum<8;bnum++) 
			showBAT(bnum);
	}
	else if (argc == 2) {
		bnum = atoi(argv[1]);
		if ((bnum < 0) || (bnum > 7)) {
			printf("BAT regnum (%d) out of range\n",bnum);
			return(CMD_FAILURE);
		}
		showBAT(bnum);
	}
	else
		return(CMD_PARAM_ERROR);

	return(CMD_SUCCESS);
}
