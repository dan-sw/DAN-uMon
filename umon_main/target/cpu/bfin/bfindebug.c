/* bfindebug.c:
 *
 *  Blackfin Debug code...
 *  This file supports the debug features discussed in chapter 21 of the
 *	ADSP-BF53x/BF56x Processor Programming Reference Rev1.1 (Feb 2006).
 *  NOTE: NOT HEAVILY TESTED, SO BE AWARE!!!
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
#include "cli.h"
#include "tfs.h"
#include "tfsprivate.h"
#include "ctype.h"
#include "genlib.h"
#include "stddefs.h"
#include "cpu.h"
#include "cdefBF537.h"

/* Instruction Watchpoint Address Control Register Bits:
 */
#define WPAND		0x02000000
#define EMUSW5		0x01000000
#define EMUSW4		0x00800000
#define WPICNTEN5	0x00400000
#define WPICNTEN4	0x00200000
#define WPIAEN5		0x00100000
#define WPIAEN4		0x00080000
#define WPIRINV45	0x00040000
#define WPIREN45	0x00020000
#define EMUSW3		0x00010000
#define EMUSW2		0x00008000
#define WPICNTEN3	0x00004000
#define WPICNTEN2	0x00002000
#define WPIAEN3		0x00001000
#define WPIAEN2		0x00000800
#define WPIRINV23	0x00000400
#define WPIREN23	0x00000200
#define EMUSW1		0x00000100
#define EMUSW0		0x00000080
#define WPICNTEN1	0x00000040
#define WPICNTEN0	0x00000020
#define WPIAEN1		0x00000010
#define WPIAEN0		0x00000008
#define WPIRINV01	0x00000004
#define WPIREN01	0x00000002
#define WPPWR		0x00000001

/* Data Watchpoint Address Control Register Bits:
 */
#define WPDACC1		0x00003000
#define WPDSRC1		0x00000c00
#define WPDACC0		0x00000300
#define WPDSRC0		0x000000c0
#define WPDCNTEN1	0x00000020
#define WPDCNTEN0	0x00000010
#define WPDAEN1		0x00000008
#define WPDAEN0		0x00000004
#define WPDRINV01	0x00000002
#define WPDREN01	0x00000001
#define WPDACC0_W	0x00000100
#define WPDACC0_R	0x00000200
#define WPDACC0_B	0x00000300
#define WPDACC1_W	0x00001000
#define WPDACC1_R	0x00002000
#define WPDACC1_B	0x00003000

struct watchinfo {
	vulong *addr_reg;	/* address register */
	vulong *count_reg;	/* count register */
	ulong enable;		/* bit that enables this watchpoint */
	ulong rangeenable;	/* bit that enables 'range' mode for this watchpoint */
	ulong emulation;	/* bit that enables 'emulation-exception type' */
	ulong inclusive;	/* bit that enables 'inclusive' range */
	ulong countenable;	/* bit that enables 'countdown' mode */
	ulong dacc;			/* data access mode */
	ulong dsrc;			/* */
};	

#if INCLUDE_BFINWATCH
struct watchinfo itbl[] = {
	{ (vulong *)pWPIA0, pWPIACNT0,
		WPIAEN0, WPIREN01, EMUSW0, WPIRINV01, WPICNTEN0, 0, 0 },

	{ (vulong *)pWPIA1, pWPIACNT1,
		WPIAEN1, WPIREN01, EMUSW1, WPIRINV01, WPICNTEN1, 0, 0 },

	{ (vulong *)pWPIA2, pWPIACNT2,
		WPIAEN2, WPIREN23, EMUSW2, WPIRINV23, WPICNTEN2, 0, 0 },

	{ (vulong *)pWPIA3, pWPIACNT3,
		WPIAEN3, WPIREN23, EMUSW3, WPIRINV23, WPICNTEN3, 0, 0 },

	{ (vulong *)pWPIA4, pWPIACNT4,
		WPIAEN4, WPIREN45, EMUSW4, WPIRINV45, WPICNTEN4, 0, 0 },

	{ (vulong *)pWPIA5, pWPIACNT5,
		WPIAEN5, WPIREN45, EMUSW5, WPIRINV45, WPICNTEN5, 0, 0 },
};

struct watchinfo dtbl[] = {
	{ (vulong *)pWPDA0, pWPDACNT0,
		WPDAEN0, WPDREN01, 0, WPDRINV01, WPDCNTEN0, WPDACC0, WPDSRC0 },

	{ (vulong *)pWPDA1, pWPDACNT1,
		WPDAEN1, WPDREN01, 0, WPDRINV01, WPDCNTEN1, WPDACC1, WPDSRC1},
};

int
watchIShow(void)
{
	int i, tot;
	struct watchinfo *wip;

	tot = 0;
	for(i=0;i<6;i++) {
		wip = &itbl[i];

		if (((i & 1) == 0) && (*pWPIACTL & wip->rangeenable)) {
			printf(" WPIA%d-%d: 0x%lx-0x%lx range=%s, event=%s, mode=%s\n",
				i,i+1,*wip->addr_reg, *(wip+1)->addr_reg,
				*pWPIACTL & wip->inclusive ? "inclusive" : "exclusive",
				*pWPIACTL & wip->emulation ? "emulation" : "exception",
				*pWPIACTL & wip->countenable ? "countdown" : "oneshot");
			i++;
			tot++;
		}
		else {
			if (*pWPIACTL & wip->enable)  {
				printf(" WPIA%d: 0x%lx  event=%s, mode=%s, count=%ld\n",
					i,*wip->addr_reg,
					*pWPIACTL & wip->emulation ? "emulation" : "exception",
					*pWPIACTL & wip->countenable ? "countdown" : "oneshot",
					*pWPIACNT0);
				tot++;
			}
		}
	}
	return(tot);
}

int
watchDShow(void)
{
	static char accessmode[] = { '?', 'w', 'r', 'b' };
	int tot = 0;

	if (*pWPDACTL & WPDREN01) {
		printf(" WPDA0-1: 0x%lx-0x%lx range=%s, mode=%s, access=%c\n",
			*pWPDA0, *pWPDA1,
			*pWPDACTL & WPDRINV01 ? "inclusive" : "exclusive",
			*pWPDACTL & WPDCNTEN0 ? "countdown" : "oneshot",
			accessmode[(*pWPDACTL & WPDACC0)>>12]);
		tot++;
	}
	else {
		if (*pWPDACTL & WPDAEN0) {
			printf(" WPDA0: 0x%lx  mode=%s, access=%c\n",
				*pWPDA0, 
				*pWPDACTL & WPDCNTEN0 ? "countdown" : "oneshot",
				accessmode[(*pWPDACTL & WPDACC0)>>8]);
			tot++;
		}
		if (*pWPDACTL & WPDAEN1) {
			printf(" WPDA1: 0x%lx  mode=%s, access=%c\n",
				*pWPDA1, 
				*pWPDACTL & WPDCNTEN1 ? "countdown" : "oneshot",
				accessmode[(*pWPDACTL & WPDACC1)>>12]);
			tot++;
		}
	}
	return(tot);
}

char *WatchHelp[] = {
    "Watchpoint support command",
    "-[d:h:mxv] {regno | init | on | off} [{addr} | {addr_lo addr_hi}]",
    " -d#       data-access type (read 'r', write 'w' or both 'b')",
    " -h##      hit count (default = 1)",
    " -m        emulation (default = exception)",
    " -x        exclusive range (default = inclusive)",
    " -v        verbose",
    0,
};

int
WatchCmd(int argc,char *argv[])
{
	vulong *ctlreg;
	char	data;
	struct watchinfo *watch;
	unsigned long hitcount, addr1, addr2;
    int		opt, verbose, exclusive, emulation, regno;

	data = 0;
	hitcount = 0;
	exclusive = 0;
	emulation = 0;
	verbose = 0;
    while ((opt=getopt(argc,argv,"d:h:mxv")) != -1) {
		switch(opt) {
		case 'd':
			data = *optarg;
			if ((data == 'r') || (data == 'b') || (data == 'w'))
				break;
			return(CMD_PARAM_ERROR);
		case 'h':
			hitcount = strtoul(optarg,0,0);
			break;
		case 'm':
			emulation = 1;
			break;
		case 'x':
			exclusive = 1;
			break;
		case 'v':
			verbose = 1;
			break;
	    default:
			return(CMD_FAILURE);
		}
	}

	if (argc == optind) {
		if (*pWPIACTL & WPPWR) {
			int tot;
			tot = watchIShow();
			tot += watchDShow();
			if (tot == 0)
				printf("No watchpoints currently configured.\n");
		}
		else {
			printf("Watchpoint unit disabled\n");
		}
		return(CMD_SUCCESS);
	}

	/* First see if the initial argument is 'special'...
	 */
	if (argc == optind+1) {
		if (strcmp(argv[optind],"init") == 0) {
			*pWPIACTL = 0;
			*pWPDACTL = 0;
		}
		else if (strcmp(argv[optind],"on") == 0) {
			*pWPIACTL |= WPPWR;
		}
		else if (strcmp(argv[optind],"off") == 0) {
			*pWPIACTL &= ~WPPWR;
		}
		else
			return(CMD_PARAM_ERROR);

		return(CMD_SUCCESS);
	}


	regno = atoi(argv[optind]);
	if ((regno < 0) || (data && (regno > 1)) || ((regno > 5))) {
		printf("Error: regno out of range\n");
		return(CMD_FAILURE);
	}

	/* It appears that the Watchpoint Unit must be enabled for any
	 * other writes to the watchpoint registers to succeed...
	 */
	*pWPIACTL |= WPPWR;

	addr1 = strtoul(argv[optind+1],0,0);
	addr2 = 0;
	if (argc == optind+3) {
		regno &= ~1;
		addr2 = strtoul(argv[optind+2],0,0);
	}

	if (data) {
		watch = &dtbl[regno];
		ctlreg = (vulong *)pWPDACTL;
		*ctlreg &= ~watch->dacc;
		*ctlreg |= 0x00000cc0;	/* Watch on DAG0 or DAG1 */
		switch(data) {
			case 'b':
				if (regno == 0)
					*ctlreg |= WPDACC0_B;
				else
					*ctlreg |= WPDACC1_B;
				break;
			case 'r':
				if (regno == 0)
					*ctlreg |= WPDACC0_R;
				else
					*ctlreg |= WPDACC1_R;
				break;
			case 'w':
				if (regno == 0)
					*ctlreg |= WPDACC0_W;
				else
					*ctlreg |= WPDACC1_W;
				break;
		}
	}
	else {
		watch = &itbl[regno];
		ctlreg = (vulong *)pWPIACTL;
		if (emulation) {
			*ctlreg |= watch->emulation;
		}
		else {
			*ctlreg &= ~(watch->emulation);
		}
	}

	*watch->addr_reg = addr1;

	/* Note that the hitcount value loaded into the CNT register is one less
	 * than the hit count value because the exception occurs AFTER the CNT
	 * register has decremented to zero.
	 */
	if (hitcount) {
		*watch->count_reg = hitcount-1;
		*ctlreg |= watch->countenable;
	}
	else {
		*watch->count_reg = 0;
		*ctlreg &= ~(watch->countenable);
	}
	
	if (argc == optind+3) {
		*(watch+1)->addr_reg = addr2;
		if (exclusive) {
			*ctlreg &= ~(watch->inclusive);
		}
		else {
			*ctlreg |= watch->inclusive;
		}
		*ctlreg |= watch->rangeenable;
	}
	else if (argc == optind+2) {
		*ctlreg |= watch->enable;
	}
	else
		return(CMD_PARAM_ERROR);

    return(CMD_SUCCESS);
}
#endif
