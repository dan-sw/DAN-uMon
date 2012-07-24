/* Power PC (403-GA) disassembler.
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
 */
#include "config.h"
#include "genlib.h"
#include "stddefs.h"

#if INCLUDE_DISASSEMBLER

#ifndef USE_SPRDCR_NAMES
#define USE_SPRDCR_NAMES 0
#endif

void prdata();

/* Instruction fields taken from  chapter 10: */
#define AA				0x00000002
#define BA				0x001F0000
#define BB				0x0000F800
#define BD				0x0000FFFC
#define BD_NEGATIVE		0x00008000
#define BF				0x03800000
#define BFA				0x001C0000
#define BI				0x001F0000
#define BO				0x03E00000
#define BO_0			0x02000000
#define BO_1			0x01000000
#define BO_2			0x00800000
#define BO_3			0x00400000
#define BO_4			0x00200000
#define BT				0x03E00000
#define D				0x0000FFFF
#define DCRN			0x001FF800
#define FXM				0x000FF000
#define IM				0x0000FFFF
#define LI				0x03FFFFFC
#define LI_NEGATIVE		0x02000000
#define LK				0x00000001
#define MB				0x000007C0
#define ME				0x0000003E
#define NB				0x0000F800
#define OPCD			0xFC000000
#define OE				0x00000400
#define RA				0x001F0000
#define RB				0x0000F800
#define RC				0x00000001
#define RSRT			0x03E00000
#define SH				0x0000F800
#define SPRF			0x001FF800
#define TO				0x03E00000
#define XO_OE			0x000007FE
#define XO_NOOE			0x000003FE
#define XO_SC			0x00000002
#define XO_STWCX		0x00000001

#define SYNC			0x7c0004ac
#define ISYNC			0x4c00012c

#define	rs	rsrt
#define	rt	rsrt

int opcodeXX(), opcode03(), opcode07();
int opcode08(), opcode10(), opcode11();
int opcode12(), opcode13(), opcode14(), opcode15();
int opcode16(), opcode17(), opcode18(), opcode19();
int opcode20(), opcode21(), opcode23();
int opcode24(), opcode25(), opcode26(), opcode27();
int opcode28(), opcode29(), opcode31();
int opcode32(), opcode33(), opcode34(), opcode35();
int opcode36(), opcode37(), opcode38(), opcode39();
int opcode40(), opcode41(), opcode42(), opcode43();
int opcode44(), opcode45(), opcode46(), opcode47();

int	(*opfuncs[])() = {
	opcodeXX, opcodeXX, opcodeXX, opcode03,
	opcodeXX, opcodeXX, opcodeXX, opcode07,
	opcode08, opcodeXX, opcode10, opcode11,
	opcode12, opcode13, opcode14, opcode15,
	opcode16, opcode17, opcode18, opcode19,
	opcode20, opcode21, opcodeXX, opcode23,
	opcode24, opcode25, opcode26, opcode27,
	opcode28, opcode29, opcodeXX, opcode31,
	opcode32, opcode33, opcode34, opcode35,
	opcode36, opcode37, opcode38, opcode39,
	opcode40, opcode41, opcode42, opcode43,
	opcode44, opcode45, opcode46, opcode47,
	opcodeXX, opcodeXX, opcodeXX, opcodeXX,
	opcodeXX, opcodeXX, opcodeXX, opcodeXX,
	opcodeXX, opcodeXX, opcodeXX, opcodeXX,
	opcodeXX, opcodeXX, opcodeXX, opcodeXX,
};

#if USE_SPRDCR_NAMES
struct sprdat {
	ushort	val;
	char	*name;
} sprtbl[] = {
	{ 0x03d7,		"cdbcr" },
	{ 0x0009,		"ctr" },
	{ 0x03f6,		"dac1" },
	{ 0x03f7,		"dac2" },
	{ 0x03f2,		"dbcr" },
	{ 0x03f0,		"dbsr" },
	{ 0x03fa,		"dccr" },
	{ 0x03d5,		"dear" },
	{ 0x03d4,		"esr" },
	{ 0x03d6,		"evpr" },
	{ 0x03f4,		"iac1" },
	{ 0x03f5,		"iac2" },
	{ 0x03fb,		"iccr" },
	{ 0x03d3,		"icdbdr" },
	{ 0x0008,		"lr" },
	{ 0x03fc,		"pbl1" },
	{ 0x03fe,		"pbl2" },
	{ 0x03fd,		"pbu1" },
	{ 0x03ff,		"pbu2" },
	{ 0x03db,		"pit" },
	{ 0x011f,		"pvr" },
	{ 0x0110,		"sprg0" },
	{ 0x0111,		"sprg1" },
	{ 0x0112,		"sprg2" },
	{ 0x0113,		"sprg3" },
	{ 0x001a,		"srr0" },
	{ 0x001b,		"srr1" },
	{ 0x03de,		"srr2" },
	{ 0x03df,		"srr3" },
	{ 0x03dc,		"tbhi" },
	{ 0x03dd,		"tblo" },
	{ 0x03da,		"tcr" },
	{ 0x03d8,		"tsr" },
	{ 0x0001,		"xer" },
	{ 0x03b9,		"sgr" },		/* 403-GC only */
	{ 0x03ba,		"dcwr" },		/* 403-GC only */
	{ 0x0000,		"rsvd0" },
	{ 0x0010,		"rsvd1" },
	{ 0x03d0,		"rsvd2" },
	{ 0x03d1,		"rsvd3" },
	{ 0x03d2,		"rsvd4" },
	{ 0x03d9,		"rsvd5" },
	{ 0x03f1,		"rsvd6" },
	{ 0x03f3,		"rsvd7" },
	{ 0x03f8,		"rsvd8" },
	{ 0x03f9,		"rsvd9" },
};

struct dcrdat {
	ushort	val;
	char	*name;
} dcrtbl[] = {
	{ 0x0090,		"bear" },
	{ 0x0091,		"besr" },
	{ 0x0080,		"br0" },
	{ 0x0081,		"br1" },
	{ 0x0082,		"br2" },
	{ 0x0083,		"br3" },
	{ 0x0084,		"br4" },
	{ 0x0085,		"br5" },
	{ 0x0086,		"br6" },
	{ 0x0087,		"br7" },
	{ 0x00c4,		"dmacc0" },
	{ 0x00cc,		"dmacc1" },
	{ 0x00d4,		"dmacc2" },
	{ 0x00dc,		"dmacc3" },
	{ 0x00c0,		"dmacr0" },
	{ 0x00c8,		"dmacr1" },
	{ 0x00d0,		"dmacr2" },
	{ 0x00d8,		"dmacr3" },
	{ 0x00c1,		"dmact0" },
	{ 0x00c9,		"dmact1" },
	{ 0x00d1,		"dmact2" },
	{ 0x00d9,		"dmact3" },
	{ 0x00c2,		"dmada0" },
	{ 0x00ca,		"dmada1" },
	{ 0x00d2,		"dmada2" },
	{ 0x00da,		"dmada3" },
	{ 0x00c3,		"dmasa0" },
	{ 0x00cb,		"dmasa1" },
	{ 0x00d3,		"dmasa2" },
	{ 0x00db,		"dmasa3" },
	{ 0x00e0,		"dmasr" },
	{ 0x0042,		"exier" },
	{ 0x0040,		"exisr" },
	{ 0x00a0,		"iocr" },
	{ 0x0041,		"rsvd" },
	{ 0x00e1,		"rsvd" },
};
#endif

char *crfld[] = {
	"CRf00", "CRf01", "CRf02", "CRf03",
	"CRf04", "CRf05", "CRf06", "CRf07",
};

char *crbit[] = {
	"CRb00", "CRb01", "CRb02", "CRb03",
	"CRb04", "CRb05", "CRb06", "CRb07",
	"CRb08", "CRb09", "CRb10", "CRb11",
	"CRb12", "CRb13", "CRb14", "CRb15",
	"CRb16", "CRb17", "CRb18", "CRb19",
	"CRb20", "CRb21", "CRb22", "CRb23",
	"CRb24", "CRb25", "CRb26", "CRb27",
	"CRb28", "CRb29", "CRb30", "CRb31",
};

char *Regs[] = {
	"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
	"r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15",
	"r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23",
	"r24", "r25", "r26", "r27", "r28", "r29", "r30", "r31",
};

char	*tbl1[] = { "add", "add.", "addo", "addo." };
char	*tbl2[] = { "addc", "addc.", "addco", "addco." };
char	*tbl3[] = { "adde", "adde.", "addeo", "addeo." };
char	*tbl4[] = { "addme", "addme.", "addmeo", "addmeo." };
char	*tbl5[] = { "addze", "addze.", "addzeo", "addzeo." };
char	*tbl6[] = { "b", "bl", "ba", "bla" };
char	*tbl7[] = { "bc", "bca", "bcl", "bcla" };
char	*tbl8[] = { "divw", "divw.", "divwo", "divwo." };
char	*tbl9[] = { "divwu", "divwu.", "divwuo", "divwuo." };
char	*tbl10[] = { "mullw", "mullw.", "mullwo", "mullwo." };
char	*tbl11[] = { "neg", "neg.", "nego", "nego." };
char	*tbl12[] = { "subf", "subf.", "subfo", "subfo." };
char	*tbl13[] = { "subfc", "subfc.", "subfco", "subfco." };
char	*tbl14[] = { "subfe", "subfe.", "subfeo", "subfeo." };
char	*tbl15[] = { "subfme", "subfme.", "subfmeo", "subfmeo." };
char	*tbl16[] = { "subfze", "subfze.", "subfzeo", "subfzeo." };
char	*tbl17[] = { "bdnz", "bdnza", "bdnzl", "bdnzla" };
char	*tbl18[] = { "bdnzf", "bdnzfa", "bdnzfl", "bdnzfla" };
char	*tbl19[] = { "bdnzt", "bdnzta", "bdnztl", "bdnztla" };
char	*tbl20[] = { "bdz", "bdza", "bdzl", "bdzla" };
char	*tbl21[] = { "bdzf", "bdzfa", "bdzfl", "bdzfla" };
char	*tbl22[] = { "bdzt", "bdzta", "bdztl", "bdztla" };

char *DisHelp[] = {
	"Disassemble memory",
	"-[m] {address} [linecount]",
	0,
};

/* DisAddr:
 * This variable can be used by the opcodeXX() functions if there is
 * good reason to know the address of the disassembly.
 */
static ulong	*DisAddr;

/* disppc.c:
 * Disassembler for POWER-PC
 *
 * Dis():
 * Use the upper 6 bits of the opcode as an offset into a table of 
 * function pointers.  Opon entry into the function, break down the
 * remaining fields and print the mnemonic.  Note that the program is
 * intentionally left un-optimized.  There's absolutely nothing tricky
 * about it so it should be trivial to add other POWER-PC instructions
 * to this disassembler.  
 * NOTE: The branch instructions need work.
 */
int
Dis(argc,argv)
int	argc;
char	*argv[];
{
	extern	int optind;
	int		opt, i, count, more;
	uchar	*cp;

	more = 0;
	while ((opt = getopt(argc,argv,"m")) != -1) {
		switch(opt) {
		case 'm':
			more = 1;
			break;
		default:
			return(0);
		}
	}

	if (argc == optind+1)
		count = 1;
	else if (argc == optind+2)
		count = strtol(argv[optind+1],0,0);
	else
		return(-1);

	DisAddr = (ulong *)strtoul(argv[optind],0,0);

again:
	for(i=0;i<count;i++) {
		cp = (uchar *)DisAddr;
		printf("%08lx: %02x%02x%02x%02x ",
			(ulong)DisAddr,cp[0],cp[1],cp[2],cp[3]);
		opfuncs[((cp[0] & 0xfc) >> 2)](*DisAddr);
		DisAddr++;
	}
	if (more) {
		if (More())
			goto again;
	}
	
	return(0);
}

int
aalk(ulong instr)
{
	switch (instr & (AA | LK)) {
	case (AA | LK):
		return(3);
	case (AA):
		return(2);
	case (LK):
		return(1);
	default:
		return(0);
	}
}

short
d(ulong instr)	/* 16 bit 2's compliment */
{
	return(instr & D);
}

/* sprf():
 * This function attempts to replace the SPR number with some
 * name.  Since this is somewhat CPU dependent, I have turned
 * this off for now..
 */
char *
sprf(ulong instr)
{
	static char	buf[16];
	ushort	hi, lo, sprval;

	hi = lo = (ushort)((instr & DCRN) >> 11);
	hi &= 0x03e0;
	lo &= 0x001f;
	hi >>= 5;
	lo <<= 5;
	sprval = hi | lo;
#if USE_SPRDCR_NAMES	/* see above note */
	for(int i=0;i<sizeof sprtbl/sizeof(struct sprdat);i++) {
		if (sprval == sprtbl[i].val) 
			return(sprtbl[i].name);
	}
	sprintf(buf,"spr	%d",sprval);
#else
	sprintf(buf,"%d",sprval);
#endif
	return(buf);
}

char *
dcrn(ulong instr)
{
	static char	buf[16];
	ushort	hi, lo, dcrnval;

	hi = lo = (ushort)((instr & DCRN) >> 11);
	hi &= 0x03e0;
	lo &= 0x001f;
	hi >>= 5;
	lo <<= 5;
	dcrnval = hi | lo;
#if USE_SPRDCR_NAMES
	for(int i=0;i<sizeof dcrtbl/sizeof(struct dcrdat);i++) {
		if (dcrnval == dcrtbl[i].val) 
			return(dcrtbl[i].name);
	}
	sprintf(buf,"dcrn=0x%x",dcrnval);
#else
	sprintf(buf,"%d",dcrnval);
#endif
	return(buf);
}

ushort
nb(ulong instr)
{
	return((instr & NB) >> 11);
}

int
oerc(ulong instr)
{
	switch (instr & (OE | RC)) {
	case (OE | RC):
		return(3);
	case (OE):
		return(2);
	case (RC):
		return(1);
	default:
		return(0);
	}
}

ushort
xo_nooe(ulong instr)
{
	return((ushort)((instr & XO_NOOE) >> 1));
}

ushort
xo_oe(ulong instr)
{
	return((ushort)((instr & XO_OE) >> 1));
}

short
mb(ulong instr)
{
	return((instr & MB) >> 6);
}

short
me(ulong instr)
{
	return((instr & ME) >> 1);
}

short
to(ulong instr)
{
	return((instr & TO) >> 21);
}

short
sh(ulong instr)
{
	return((instr & SH) >> 11);
}

ushort
fxm(ulong instr)
{
	return((instr & FXM) >> 12);
}

char *
ra(ulong instr)		/* GPR used as source or target */
{
	return(Regs[(uchar)((instr & RA) >> 16)]);
}

char *
rb(ulong instr)		/* GPR used as source */
{
	return(Regs[(uchar)((instr & RB) >> 11)]);
}

char *
ba(ulong instr)
{
	return(crbit[(uchar)(((instr & BA) >> 16) & 0x1f)]);
}

char *
bb(ulong instr)
{
	return(crbit[(uchar)(((instr & BB) >> 11) & 0x1f)]);
}

char *
bt(ulong instr)
{
	return(crbit[(uchar)(((instr & BT) >> 21) & 0x1f)]);
}

char *
bo(ulong instr)
{
	static char	bostr[32];

	bostr[0] = 0;
	if ((instr & BO_0) == 0) {
		if (instr & BO_1)
			strcat(bostr,"if crbit=1 ");
		else
			strcat(bostr,"if crbit!=1 ");
	}
	if ((instr & BO_2) == 0) {
		if (instr & BO_3)
			strcat(bostr,"if --ctr=0 ");
		else
			strcat(bostr,"if --ctr!=0 ");
	}
	if (instr & BO_4)
		strcat(bostr,"bpr");

	return(bostr);
}

char *
bfa(ulong instr)
{
	return(crfld[(uchar)(((instr & BFA) >> 18) & 0x3)]);
}

char *
bf(ulong instr)
{
	return(crfld[(uchar)(((instr & BF) >> 23) & 0x3)]);
}

char *
bi(ulong instr)
{
	return(crbit[(uchar)(((instr & BI) >> 16) & 0x1f)]);
}

char *
rsrt(ulong instr)	/* GPR used as source (rs) or destination (rt) */
{
	return(Regs[(uchar)(((instr & RSRT) >> 21) & 0x1f)]);
}

/* prnem():
 * Print the mnemonic and append the number of spaces needed
 * so that the total length is 8 characters.
 */
void
prnem(char *mnem)
{
	int	spacecount;

	spacecount = 8 - strlen(mnem);
	putstr(mnem);
	while(spacecount > 0) {
		putchar(' ');
		spacecount--;
	}
}

void
prnemdot(char *mnem, int dot)
{
	int	spacecount;

	spacecount = 8 - strlen(mnem);
	putstr(mnem);
	if (dot) {
		putchar('.');
		spacecount--;
	}
	while(spacecount > 0) {
		putchar(' ');
		spacecount--;
	}
}

int
bf_ra(char *mnemonic, ulong instr)
{
	prnem(mnemonic);
	printf("%s,0,%s,0x%04X\n",bf(instr),ra(instr),(ushort)(instr&IM));
	return(0);
}

int
rt_ra_x(char *mnemonic, ulong instr)
{
	prnem(mnemonic);
	printf("%s,%s,0x%04X\n",rt(instr),ra(instr),(ushort)(instr&IM));
	return(0);
}

int
rt_d_ra(char *mnemonic, ulong instr)
{
	prnem(mnemonic);
	printf("%s,%d(%s)\n",rt(instr),d(instr),ra(instr));
	return(0);
}

int
rs_d_ra(char *mnemonic, ulong instr)
{
	prnem(mnemonic);
	printf("%s,%d(%s)\n",rs(instr),d(instr),ra(instr));
	return(0);
}

int
bt_ba_bb(char *mnemonic, ulong instr)
{
	prnem(mnemonic);
	printf("%s,%s,%s\n",bt(instr),ba(instr),bb(instr));
	return(0);
}

int
ra_rs_sh_mb_me(char *mnemonic, ulong instr)
{
	prnemdot(mnemonic,instr & RC);
	printf("%s,%s,0x%04X,0x%04X,0x%04X\n",
		ra(instr),rs(instr),sh(instr),mb(instr),me(instr));
	return(0);
}

int
ra_rs(char *mnemonic, ulong instr)
{
	prnem(mnemonic);
	printf("%s,%s,0x%04X\n",ra(instr),rs(instr),(ushort)(instr&IM));
	return(0);
}

int
rt_ra_rb(char *mnemonic, ulong instr)
{
	prnem(mnemonic);
	printf("%s,%s,%s\n", rt(instr),ra(instr),rb(instr));
	return(0);
}

int
rs_ra_rb(char *mnemonic,ulong instr)
{
	prnem(mnemonic);
	printf("%s,%s,%s\n",rs(instr),ra(instr),rb(instr));
	return(0);
}

int
ra_rb(char *mnemonic,ulong instr)
{
	prnem(mnemonic);
	printf("%s,%s\n",ra(instr),rb(instr));
	return(0);
}

int
rt_ra(char *mnemonic, ulong instr)
{
	prnem(mnemonic);
	printf("%s,%s\n", rt(instr),ra(instr));
	return(0);
}

int
opcodeXX(ulong instr)
{
	uchar	*cp;

	cp = (uchar *)&instr;
	printf("data    %02x%02x%02x%02x\n",cp[0],cp[1],cp[2],cp[3]);
	return(0);
}

int
opcode03(ulong instr)
{
	printf("twi	%x,%s,0x%04X\n",to(instr),ra(instr),(ushort)(instr&IM));
	return(0);
}

int
opcode07(ulong instr)
{
	return(rt_ra_x("mulli",instr));
}

int
opcode08(ulong instr)
{
	return(rt_ra_x("subfic",instr));
}

int
opcode10(ulong instr)
{
	return(bf_ra("cmpli",instr));
}

int
opcode11(ulong instr)
{
	return(bf_ra("cmpi",instr));
}


int
opcode12(ulong instr)
{
	return(rt_ra_x("addic",instr));
}

int
opcode13(ulong instr)
{
	return(rt_ra_x("addic.",instr));
}

int
opcode14(ulong instr)
{
	return(rt_ra_x("addi",instr));
}

int
opcode15(ulong instr)
{
	return(rt_ra_x("addis",instr));
}

int
opcode16(ulong instr)
{
	ulong	mask;

	if (instr & BD_NEGATIVE) 
		mask = 0xffff0000;
	else
		mask = 0;

	switch((instr & BO) >> 21) {
	case 16:
		prnem(tbl17[aalk(instr)]);
		printf("0x%lx\n", (instr & BD) | mask);
		return(0);
	case 0:
		prnem(tbl18[aalk(instr)]);
		printf("%s,0x%lx\n", bi(instr), (instr & BD) | mask);
		return(0);
	case 8:
		prnem(tbl19[aalk(instr)]);
		printf("%s,0x%lx\n", bi(instr), (instr & BD) | mask);
		return(0);
	case 18:
		prnem(tbl20[aalk(instr)]);
		printf("0x%lx\n", (instr & BD) | mask);
		return(0);
	case 2:
		prnem(tbl21[aalk(instr)]);
		printf("%s,0x%lx\n", bi(instr), (instr & BD) | mask);
		return(0);
	case 10:
		prnem(tbl22[aalk(instr)]);
		printf("%s,0x%lx\n", bi(instr), (instr & BD) | mask);
		return(0);
	}
	prnem(tbl7[aalk(instr)]);
	printf("%ld,%ld,0x%lx\n", ((instr & BO) >> 21),((instr & BI) >> 16),
		(instr & BD) | mask);
	return(0);
}

int
opcode17(ulong instr)
{
	if (instr & 0x2)
		printf("sc\n");
	else
		return(-1);
	return(0);
}

int
opcode18(ulong instr)
{
	ulong	mask, delta, dest;
#if INCLUDE_TFSSYMTBL
	ulong	offset;
	char	func[32];
#endif

	if (instr & LI_NEGATIVE) 
		mask = 0xfe000000;
	else
		mask = 0;
	prnem(tbl6[aalk(instr)]);
	delta = (instr & LI) | mask;
	if (instr & AA)
		dest = delta;
	else
		dest = (ulong)DisAddr + delta;
#if INCLUDE_TFSSYMTBL
	if (AddrToSym(-1,dest,func,&offset)) {
		printf("0x%lx (%s",delta,func);
		if (offset) 
			printf(": 0x%lx",dest);
		printf(")\n");
	}
	else
#endif
		printf("0x%lx (addr=0x%lx)\n",delta,dest);
	return(0);
}

int
opcode19(ulong instr)
{
	if (instr == ISYNC) {
		printf("isync\n");
		return(0);
	}
	switch(xo_oe(instr)) {
	case 0:
		prnem("mcrf");
		printf("%s,%s\n",bf(instr),bfa(instr));
		break;
	case 16:
		if ((instr & (BO_0 | BO_2)) != (BO_0 | BO_2)) {
			if (instr & LK)
				prnem("bclrl");
			else
				prnem("bclr");
		}
		else {
			if (instr & LK)
				prnem("blrl");
			else
				prnem("blr");
		}
		if ((instr & (BO_0 | BO_2)) != (BO_0 | BO_2))
			printf("%s,%s",bo(instr),bi(instr));
		printf("\n");
		return(0);
	case 33:
		return(bt_ba_bb("crnor",instr));
	case 50:
		printf("rfi\n");
		return(0);
	case 51:
		printf("rfci\n");
		return(0);
	case 129:
		return(bt_ba_bb("crandc",instr));
	case 193:
		return(bt_ba_bb("crxor",instr));
	case 225:
		return(bt_ba_bb("crnand",instr));
	case 257:
		return(bt_ba_bb("crand",instr));
	case 289:
		return(bt_ba_bb("creqv",instr));
	case 417:
		return(bt_ba_bb("crorc",instr));
	case 449:
		return(bt_ba_bb("cror",instr));
	case 528:
		if ((instr & (BO_0 | BO_2)) != (BO_0 | BO_2))
			prnem("bcctr");
		else
			prnem("bctr");
		if (instr & LK)
			printf("l");
		if ((instr & (BO_0 | BO_2)) != (BO_0 | BO_2))
			printf("%s,%s",bo(instr),bi(instr));
		printf("\n");
		return(0);
	default:
		return(opcodeXX(instr));
	}
	return(-1);
}

int
opcode20(ulong instr)
{
	return(ra_rs_sh_mb_me("rlwimi",instr));
}

int
opcode21(ulong instr)
{
	return(ra_rs_sh_mb_me("rlwinm",instr));
}

int
opcode23(ulong instr)
{
	prnemdot("rlwnm",instr & RC);
	printf("%s,%s,%s,0x%04X,0x%04X\n",
		ra(instr),rs(instr),rb(instr),mb(instr),me(instr));
	return(0);
}

int
opcode24(ulong instr)
{
	prnem("ori");
	printf("%s,%s,0x%04X%s",
		ra(instr),rs(instr),(ushort)(instr&IM),
		instr==0x60000000 ? " (nop)\n" : "\n");
	return(0);
}

int
opcode25(ulong instr)
{
	return(ra_rs("oris",instr));
}

int
opcode26(ulong instr)
{
	return(ra_rs("xori",instr));
}

int
opcode27(ulong instr)
{
	return(ra_rs("xoris",instr));
}

int
opcode28(ulong instr)
{
	return(ra_rs("addi.",instr));
}

int
opcode29(ulong instr)
{
	return(ra_rs("addis.",instr));
}

int
opcode31(ulong instr)
{
	if (instr == SYNC) {
		printf("sync\n");
		return(0);
	}
	switch(xo_oe(instr)) {
	case 8:
		return(rt_ra_rb(tbl13[oerc(instr)],instr));
	case 10:
		return(rt_ra_rb(tbl2[oerc(instr)],instr));
	case 20:
		return(rt_ra_rb("lwarx",instr));
	case 23:
		return(rt_ra_rb("lwzx",instr));
	case 40:
		return(rt_ra_rb(tbl12[oerc(instr)],instr));
	case 55:
		return(rt_ra_rb("lwzux",instr));
	case 87:
		return(rt_ra_rb("lbzx",instr));
	case 104:
		return(rt_ra(tbl11[oerc(instr)],instr));
	case 119:
		return(rt_ra_rb("lbzux",instr));
	case 136:
		return(rt_ra_rb(tbl14[oerc(instr)],instr));
	case 138:
		return(rt_ra_rb(tbl3[oerc(instr)],instr));
	case 151:
		return(rs_ra_rb("stwx",instr));
	case 183:
		return(rs_ra_rb("stwux",instr));
	case 200:
		return(rt_ra(tbl16[oerc(instr)],instr));
	case 202:
		return(rt_ra(tbl5[oerc(instr)],instr));
	case 215:
		return(rs_ra_rb("stbx",instr));
	case 232:
		return(rt_ra(tbl15[oerc(instr)],instr));
	case 234:
		return(rt_ra(tbl4[oerc(instr)],instr));
	case 235:
		return(rt_ra_rb(tbl10[oerc(instr)],instr));
	case 266:
		return(rt_ra_rb(tbl1[oerc(instr)],instr));
	case 279:
		return(rt_ra_rb("lhzx",instr));
	case 311:
		return(rt_ra_rb("lhzux",instr));
	case 459:
		return(rt_ra_rb(tbl9[oerc(instr)],instr));
	case 491:
		return(rt_ra_rb(tbl8[oerc(instr)],instr));
	case 534:
		return(rt_ra_rb("lwbrx",instr));
	case 662:
		return(rs_ra_rb("stwbrx",instr));
	case 512:
		prnem("mcrxr");
		printf("%s\n",bf(instr));
		return(0);
	case 533:
		return(rt_ra_rb("lswx",instr));
	case 536:
		prnemdot("srw",instr & RC);
		printf("%s,%s,%s\n", ra(instr),rs(instr),rb(instr));
		return(0);
	case 597:
		prnem("lswi");
		printf("%s,%s,0x%04X\n",rt(instr),ra(instr),nb(instr));
		return(0);
	case 661:
		return(rs_ra_rb("stswx",instr));
	case 725:
		prnem("stswi");
		printf("%s,%s,0x%04X\n",rs(instr),ra(instr),nb(instr));
		return(0);
	case 790:
		return(rt_ra_rb("lhbrx",instr));
	case 792:
		prnemdot("sraw",instr & RC);
		printf("%s,%s,%s\n", ra(instr),rs(instr),rb(instr));
		return(0);
	case 824:
		prnemdot("srawi",instr & RC);
		printf("%s,%s,0x%04X\n", ra(instr),rs(instr),sh(instr));
		return(0);
	case 854:
		printf("eieio\n");	/* yep, that's really an instruction */
		return(0);
	case 918:
		return(rs_ra_rb("sthbrx",instr));
	case 922:
		prnemdot("extsh",instr & RC);
		printf("%s,%s,%s\n", rs(instr),ra(instr),rb(instr));
		return(0);
	case 954:
		prnemdot("extsb",instr & RC);
		printf("%s,%s,%s\n", rs(instr),ra(instr),rb(instr));
		return(0);
	case 966:
		return(ra_rb("icci",instr));
	case 982:
		return(ra_rb("icbi",instr));
	case 998:
		return(ra_rb("icread",instr));
	case 1014:
		return(ra_rb("dcbz",instr));
	}
	switch(xo_nooe(instr)) {
	case 0:
		prnem("cmp");
		printf("%s,0,%s,%s\n",bf(instr),ra(instr),rb(instr));
		return(0);
	case 4:
		prnem("tw");
		printf("0x%04X,%s,%s\n",to(instr),ra(instr),rb(instr));
		return(0);
	case 11:
		prnemdot("mulhwu",instr & RC);
		printf("%s,%s,%s\n", rt(instr),ra(instr),rb(instr));
		return(0);
	case 19:
		prnem("mfcr");
		printf("%s\n",rt(instr));
		return(0);
	case 24:
		prnemdot("slw",instr & RC);
		printf("%s,%s,%s\n", ra(instr),rs(instr),rb(instr));
		return(0);
	case 26:
		prnemdot("cntizw",instr & RC);
		printf("%s,0,%s,%s\n", rs(instr),ra(instr),rb(instr));
		return(0);
	case 28:
		prnemdot("andc",instr & RC);
		printf("%s,%s,%s\n", rs(instr),ra(instr),rb(instr));
		return(0);
	case 32:
		prnem("cmpl");
		printf("%s,0,%s,%s\n",bf(instr),ra(instr),rb(instr));
		return(0);
	case 54:
		prnem("dcbst");
		printf("%s,%s\n",ra(instr),rb(instr));
		return(0);
	case 60:
		prnemdot("and",instr & RC);
		printf("%s,%s,%s\n", rs(instr),ra(instr),rb(instr));
		return(0);
	case 75:
		prnemdot("mulhw",instr & RC);
		printf("%s,%s,%s\n", rt(instr),ra(instr),rb(instr));
		return(0);
	case 83:
		prnem("mfmsr");
		printf("%s\n",rt(instr));
		return(0);
	case 86:
		prnem("dcbf");
		printf("%s,%s\n",ra(instr),rb(instr));
		return(0);
	case 124:
		prnemdot("nor",instr & RC);
		printf("%s,%s,%s\n", ra(instr),rs(instr),rb(instr));
		return(0);
	case 131:
		prnem("wrtee");
		printf("%s\n",rs(instr));
		return(0);
	case 144:
		prnem("mtcrf");
		printf("0x%x,%s\n",fxm(instr),rs(instr));
		return(0);
	case 146:
		prnem("mtmsr");
		printf("%s\n",rs(instr));
		return(0);
	case 150:
		if (instr & 1)
			return(rs_ra_rb("stwcx.",instr));
		else
			return(-1);
	case 163:
		prnem("wrteei");
		if (instr & 0x8000)
			printf("1\n");
		else
			printf("0\n");
		return(0);
	case 246:
		prnem("dcbtst");
		printf("%s,%s\n",ra(instr),rb(instr));
		return(0);
	case 247:
		prnem("stbux");
		printf("%s,%s\n",rs(instr),ra(instr));
		return(0);
	case 262:
		prnem("icbt");
		printf("%s,%s\n",ra(instr),rb(instr));
		return(0);
	case 278:
		prnem("dcbt");
		printf("%s,%s\n",ra(instr),rb(instr));
		return(0);
	case 284:
		prnemdot("eqv",instr & RC);
		printf("%s,%s,%s\n", rs(instr),ra(instr),rb(instr));
		return(0);
	case 316:
		prnemdot("xor",instr & RC);
		printf("%s,%s,%s\n", rs(instr),ra(instr),rb(instr));
		return(0);
	case 323:
#if USE_SPRDCR_NAMES
		printf("mf%s	%s\n",dcrn(instr),rt(instr));
#else
		prnem("mfdcr");
		printf("%s,%s\n",dcrn(instr),rt(instr));
#endif
		return(0);
	case 339:
#if USE_SPRDCR_NAMES
		printf("mf%s,%s\n",sprf(instr),rt(instr));
#else
		prnem("mfspr");
		printf("%s,%s\n",sprf(instr),rt(instr));
#endif
		return(0);
	case 343:
		prnem("lhax");
		printf("%s,%s,%s\n",rt(instr),ra(instr),rb(instr));
		return(0);
	case 370:
		prnem("tlbia");
		printf("\n");
		return(0);
	case 375:
		prnem("lhaux");
		printf("%s,%s,%s\n",rt(instr),ra(instr),rb(instr));
		return(0);
	case 407:
		return(rs_ra_rb("sthx",instr));
	case 412:
		prnemdot("orc",instr & RC);
		printf("%s,%s,%s\n", ra(instr),rs(instr),rb(instr));
		return(0);
	case 439:
		return(rs_ra_rb("sthux",instr));
	case 444:
		prnemdot("or",instr & RC);
		printf("%s,%s,%s\n", ra(instr),rs(instr),rb(instr));
		return(0);
	case 451:
#if USE_SPRDCR_NAMES
		printf("mt%s	%s\n",dcrn(instr),rs(instr));
#else
		prnem("mtdcr");
		printf("%s,%s\n",dcrn(instr),rs(instr));
#endif
		return(0);
	case 454:
		prnem("dcci");
		printf("%s,%s\n",ra(instr),rb(instr));
		return(0);
	case 467:
#if USE_SPRDCR_NAMES
		printf("mt%s,%s\n",sprf(instr),rs(instr));
#else
		prnem("mtspr");
		printf("%s,%s\n",sprf(instr),rs(instr));
#endif
		return(0);
	case 470:
		prnem("dcbi");
		printf("%s,%s\n",ra(instr),rb(instr));
		return(0);
	case 476:
		prnemdot("nand",instr & RC);
		printf("%s,%s,%s\n", ra(instr),rs(instr),rb(instr));
		return(0);
	case 486:
		return(rt_ra_rb("dcread",instr));
	}
	printf("opcode31 error: 0x%02x%02x%02x%02x\n",
		((uchar *)&instr)[0], ((uchar *)&instr)[1],
		((uchar *)&instr)[2], ((uchar *)&instr)[3]);
	printf("xo_oe() = 0x%04X, xo_nooe() = 0x%04X\n",
		xo_oe(instr),xo_nooe(instr));
	return(-1);
}

int
opcode32(ulong instr)
{
	return(rt_d_ra("lwz",instr));
}

int
opcode33(ulong instr)
{
	return(rt_d_ra("lwzu",instr));
}

int
opcode34(ulong instr)
{
	return(rt_d_ra("lbz",instr));
}

int
opcode35(ulong instr)
{
	return(rt_d_ra("lbzu",instr));
}

int
opcode36(ulong instr)
{
	return(rs_d_ra("stw",instr));
}

int
opcode37(ulong instr)
{
	return(rs_d_ra("stwu",instr));
}

int
opcode38(ulong instr)
{
	return(rt_d_ra("stb",instr));
}

int
opcode39(ulong instr)
{
	return(rt_d_ra("stbu",instr));
}

int
opcode40(ulong instr)
{
	return(rt_d_ra("lhz",instr));
}

int
opcode41(ulong instr)
{
	return(rt_d_ra("lhzu",instr));
}

int
opcode42(ulong instr)
{
	return(rt_d_ra("lha",instr));
}

int
opcode43(ulong instr)
{
	return(rt_d_ra("lhau",instr));
}

int
opcode44(ulong instr)
{
	return(rt_d_ra("sth",instr));
}

int
opcode45(ulong instr)
{
	return(rs_d_ra("sthu",instr));
}

int
opcode46(ulong instr)
{
	return(rt_d_ra("lmw",instr));
}

int
opcode47(ulong instr)
{
	return(rs_d_ra("stmw",instr));
}

#endif	/* INCLUDE_DISASSEMBLER */
