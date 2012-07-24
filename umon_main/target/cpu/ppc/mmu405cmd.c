/* tlbcmd405.c:

    General notice:
    This code is part of a boot-monitor package developed as a generic base
    platform for embedded system designs.  As such, it is likely to be
    distributed to various projects beyond the control of the original
    author.  Please notify the author of any enhancements made or bugs found
    so that all may benefit from the changes.  In addition, notification back
    to the author will allow the new user to pick up changes that may have
    been made by other users after this version of the code was distributed.

    Author: Ed Sutter
    email:  esutter@lucent.com  (home: lesutter@worldnet.att.net)
    phone:  908-582-2351        (home: 908-889-5161)
*/
#include "config.h"

#if INCLUDE_MMUCMD
#include "cpu.h"
#include "cpuio.h"
#include "genlib.h"
#include "stddefs.h"
#include "cli.h"
#include "arch_ppc.h"

/* Externs from tlb405.s...
 */
extern void ppctlbia(void);
extern unsigned long ppcMftlblo(int);
extern unsigned long ppcMftlbhi(int);
extern unsigned long ppcMfpid(void);
extern unsigned long ppcMfzpr(void);
extern void ppcMttlblo(int,unsigned long);
extern void ppcMttlbhi(int,unsigned long);
extern void ppcMtpid(unsigned long);
extern void ppcMtzpr(unsigned long);

#define PID_SPRNUM	945

/* Bit Fields in TLB entries...
 */
#define PID_MASK			0x000000ff
#define TLBHI_TID			0x0000000f
#define TLBHI_U0			0x00000010
#define TLBHI_ENDIAN		0x00000020
#define TLBHI_VALID			0x00000040
#define TLBHI_PGSIZE		0x00000380
#define TLBHI_EPN			0xfffffc00
#define TLBLO_RPN			0xfffffc00
#define TLBLO_EXECUTEABLE	0x00000200
#define TLBLO_WRITEABLE		0x00000100
#define TLBLO_ZONESELECT	0x000000f0
#define TLBLO_WRITETHRU		0x00000008
#define TLBLO_INHIBITCACHE	0x00000004
#define TLBLO_MEMCOHERENT	0x00000002
#define TLBLO_GUARDED		0x00000001

/* Notes regarding MMU and cache...
 * When MMU is fully enabled (MSR[IR & DR] == 11), the TLB entries
 * control the cacheability of a page.  There are three possibilities
 * for a given page I THINK!!! :-(  ...
 *
 *  MODE                  TLBLO_WRITETHRU     TLBLO_INHIBITCACHE
 *
 *  Dcache enabled             0                   0
 *  Icache enabled
 * ---------------------------------------------------------------
 *  Dcache disabled            1                   0
 *  Icache enabled
 * ---------------------------------------------------------------
 *  Dcache disabled            0                   1
 *  Icache disabled
 * ---------------------------------------------------------------
 *  Illegal State              1                   1
 */

struct pagesizeinfo {
	char *name;
	unsigned long size;
	unsigned long sizebits;
	unsigned long rpnmask;
};

static struct pagesizeinfo pagesizetbl[] = {
	{ "1K",		0x400,			(0<<7),		0xfffffc00 },
	{ "4K",		0x1000,			(1<<7),		0xfffff000 },
	{ "16K",	0x4000,			(2<<7),		0xffffc000 },
	{ "64K",	0x10000,		(3<<7),		0xffff0000 },
	{ "256K",	0x40000,		(4<<7),		0xfffc0000 },
	{ "1M",		0x100000,		(5<<7),		0xfff00000 },
	{ "4M",		0x400000,		(6<<7),		0xffc00000 },
	{ "16M",	0x1000000,		(7<<7),		0xff000000 },
	{ 0,0,0 }
};

static char *
yesorno(ulong  val)
{
	return(val == 0 ? "N" : "Y");
}

static unsigned long
getpagesize(char *name)
{
	struct pagesizeinfo *psi;

	psi = pagesizetbl;
	while(psi->name) {
		if (!strcmp(psi->name,name))
			return(psi->sizebits);
		psi++;
	}
	printf(" Invalid page size: %s\n",name);
	return(0xffffffff);
}

/* printpagesize():
 * Verbosely converts an incoming size entry.
 */
static void
printpagesize(ulong sizebits)
{
	struct pagesizeinfo *psi;

	psi = pagesizetbl;
	while(psi->name) {
		if (psi->sizebits == sizebits) {
			printf("%4s",psi->name);
			return;
		}
		psi++;
	}
	printf("???");
}

/* printtlb():
 * Dump a verbose description of the incoming hi & lo portions of
 * a TLB entry...
 */
static void
printtlb(int idx, ulong hi, ulong lo, int hdr)
{
	if (hdr) {
		printf("Idx  Effective  Real      Page  Vld End ");
		printf("U0 TID Ex Wr Zone WrThru Inhib Mem   Grd\n");
		printf("     PgNum      PgNum     Size          ");
		printf("          En      Cache  Cache Cohrnt\n");
	}
	printf("%2d: x%08lx  x%08lx  ",idx,(hi & TLBHI_EPN),(lo & TLBLO_RPN));
	printpagesize(hi & TLBHI_PGSIZE);
	printf("   %s  %s  %d %3d  %s  %s  %3d   %s      %s     %s    %s\n",
		yesorno(hi & TLBHI_VALID),(hi & TLBHI_ENDIAN) ? "lit" : "big",
		(int)((hi & TLBHI_U0) >> 4),(int)(hi & TLBHI_TID),
		yesorno(lo & TLBLO_EXECUTEABLE),yesorno(lo & TLBLO_WRITEABLE),
		(int)((lo & TLBLO_ZONESELECT) >> 4),yesorno(lo & TLBLO_WRITETHRU),
		yesorno(lo & TLBLO_INHIBITCACHE),yesorno(lo & TLBLO_MEMCOHERENT),
		yesorno(lo & TLBLO_GUARDED));
}


/* checktlb():
 * There are a few rules (according to the PPC405GP users's manual) that
 * must be followed by software when configuring the TLB.  This function
 * makes sure those rules are not violated by the incoming TLB entry.
 *
 * 1: Software must set all unused bits (as determined by page size) of
 *    the RPN (Real Page Number) field to zero.
 *
 * 2: The PowerPC architecture does not support memory models in which
 *    write-thru is enabled and caching is inhibited.
 */
static void
checktlb(int index,ulong hi,ulong lo)
{
	ulong sizebits, rpn;
	struct pagesizeinfo *psi;

	/* If the entry isn't valid, then don't bother checking the RPN...
	 */
	if ((hi & TLBHI_VALID) == 0)
		return;


	sizebits = hi & TLBHI_PGSIZE;

	psi = pagesizetbl;
	while(psi->name) {
		if (psi->sizebits == sizebits)
			break;
		psi++;
	}
	if (psi->name == 0) {
		printf("checktlb() error\n");
		return;
	}

	rpn = lo & TLBLO_RPN;
	if (rpn & ~psi->rpnmask)
		printf("TLB %d: RPN <-> PageSize conflict\n",index);

	if ((lo & TLBLO_WRITETHRU) && (lo & TLBLO_INHIBITCACHE))
		printf("TLB %d: WriteThru <-> CacheInhibit conflict\n",index);
}

char *MmuHelp[] = {
    "MMU-Specific Operations",
    " [-v] {cmd} [params]",
    " Where {cmd} is: ir, dr, tlb, tlbinv, pid, tlbrd, tlbwl, or tlbwh...",
    "   ir [1|0]     instruction relocation {set|clr}",
    "   dr [1|0]     data relocation {set|clr}",
    "   tlb [#]      dump all valid (or specified) tlb entries",
    "   tlbinv       invalidate all tlb entries",
    "   pid [value]  read or write the PID (spr 945)",
    "   zpr [value]  read or write the ZPR (spr 944)",
    "   tlbwh # EPN PgSize Valid Endian U0 TID",
    "   tlbwl # RPN Exec Writeable Zone WriteThru InhibCache MemCohrnt Guarded",
    0,
};

/* MmuCmd():
 * This command allows the user to play around with the MMU-related
 * facilities of the PPC405GP.  
 */
int
MmuCmd(int argc,char *argv[])
{
	int verbose = 0;
    int opt, index, hdr;
	ulong value, hi, lo, mmupid;

    while ((opt=getopt(argc,argv,"v")) != -1) {
	    switch(opt) {
	        case 'v':
	            verbose = 1;
	            break;
	        default:
	            return(CMD_PARAM_ERROR);
	    }
    }

	if (argc < optind+1)
		return(CMD_PARAM_ERROR);

	if (!strcmp(argv[optind],"tlbinv")) {
		if (argc != optind+1)
			return(CMD_PARAM_ERROR);

		ppctlbia();
	}
	else if (!strcmp(argv[optind],"ir")) {
		if (argc == optind+1) {
			printf("Instruction relocation: %sabled\n",
				ppcMfmsr() & MSR_IR ? "en" : "dis");
    		return(CMD_SUCCESS);
		}
		else if (argc != optind+2)
			return(CMD_PARAM_ERROR);

		if (!strcmp(argv[optind+1],"0")) {
			ppcMtmsr(ppcMfmsr() & ~MSR_IR);
		}
		else if (!strcmp(argv[optind+1],"1")) {
			ppcMtmsr(ppcMfmsr() | MSR_IR);
		}
		else
			return(CMD_PARAM_ERROR);
	}
	else if (!strcmp(argv[optind],"dr")) {
		if (argc == optind+1) {
			printf("Data relocation: %sabled\n",
				ppcMfmsr() & MSR_DR ? "en" : "dis");
    		return(CMD_SUCCESS);
		}
		else if (argc != optind+2)
			return(CMD_PARAM_ERROR);

		if (!strcmp(argv[optind+1],"0")) {
			ppcMtmsr(ppcMfmsr() & ~MSR_DR);
		}
		else if (!strcmp(argv[optind+1],"1")) {
			ppcMtmsr(ppcMfmsr() | MSR_DR);
		}
		else
			return(CMD_PARAM_ERROR);
	}
	else if (!strcmp(argv[optind],"pid")) {
		if (argc == optind+1)
			printf("PID: 0x%lx\n",ppcMfpid());
		else if (argc == optind+2)
			ppcMtpid(strtoul(argv[optind+1],0,0));
		else
			return(CMD_PARAM_ERROR);
	}
	else if (!strcmp(argv[optind],"zpr")) {
		if (argc == optind+1)
			printf("ZPR: 0x%lx\n",ppcMfzpr());
		else if (argc == optind+2)
			ppcMtzpr(strtoul(argv[optind+1],0,0));
		else
			return(CMD_PARAM_ERROR);
	}
	else if (!strcmp(argv[optind],"tlb")) {
		hdr = 1;

		/* The tlbre instruction modifies the PID, so we save it here
		 * so that it can be restored later...
		 */
		mmupid = ppcMfpid();
		for(index=0;index<64;index++) {
			hi = ppcMftlbhi(index);
			lo = ppcMftlblo(index);
			if (argc == optind+1) {
				if (hi & TLBHI_VALID) {
					printtlb(index,hi,lo,hdr);
					hdr = 0;
				}
			}
			else if (argc == optind+2) {
				if (inRange(argv[optind+1],index)) {
					printtlb(index,hi,lo,hdr);
					hdr = 0;
				}
			}
			else
				return(CMD_PARAM_ERROR);

			checktlb(index,hi,lo);
		}
		/* Restore PID...
		 */
		ppcMtpid(mmupid);
	}
	else if (!strcmp(argv[optind],"tlbwl")) {
		if (argc == optind+3) {
			index = atoi(argv[optind+1]);
			value = strtoul(argv[optind+2],0,0);
		}
		else if (argc == optind+10) {
			unsigned long rpn, ex, wr, zsel, w, i, m, g;
			
			index = atoi(argv[optind+1]);
			rpn = strtoul(argv[optind+2],0,0);
			ex = strtoul(argv[optind+3],0,0);
			wr = strtoul(argv[optind+4],0,0);
			zsel = strtoul(argv[optind+5],0,0);
			w = strtoul(argv[optind+6],0,0);
			i = strtoul(argv[optind+7],0,0);
			m = strtoul(argv[optind+8],0,0);
			g = strtoul(argv[optind+9],0,0);
			value = (rpn | ((ex & 1) << 9) | ((wr & 1) << 8) |
						((zsel & 0xf) << 4) | ((w & 1) << 3) |
						((i & 1) << 2) | ((m & 1) << 1) | (g & 1));
		}
		else
			return(CMD_PARAM_ERROR);

		if (verbose)
			printf("tlblo[%d] = 0x%08lx\n", index,value);
		ppcMttlblo(index,value);
	}
	else if (!strcmp(argv[optind],"tlbwh")) {
		if (argc == optind+3) {
			index = atoi(argv[optind+1]);
			value = strtoul(argv[optind+2],0,0);
		}
		else if (argc == optind+8) {
			unsigned long epn, sizebits, v, e, u0, tid;

			index = atoi(argv[optind+1]);
			epn = strtoul(argv[optind+2],0,0);
			if ((sizebits = getpagesize(argv[optind+3])) == 0xffffffff)
				return(CMD_FAILURE);
			v = strtoul(argv[optind+4],0,0);
			e = strtoul(argv[optind+5],0,0);
			u0 = strtoul(argv[optind+6],0,0);
			tid = strtoul(argv[optind+7],0,0);
			value = (epn | (sizebits) | ((v & 1) << 6) |
						((e & 0xf) << 5) | ((u0 & 1) << 4) | ((tid & 0xf)));
		}
		else
			return(CMD_PARAM_ERROR);

		if (verbose)
			printf("tlbhi[%d] = 0x%08lx\n", index,value);
		ppcMttlbhi(index,value);
	}
	else
		return(CMD_PARAM_ERROR);
		
    return(CMD_SUCCESS);
}

#endif
