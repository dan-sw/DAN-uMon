/* mips_dis.c:
 * This is a hacked up version of an old PMON disassembler that was put into
 * the public domain by LSI.
 */
#include "config.h"
#if INCLUDE_DISASSEMBLER
#include "mips.h"
#include "ctype.h"
#include "cli.h"
#include "genlib.h"
#include "stddefs.h"

#define getfield(w,s,p) (((w)&(((1<<(s))-1)<<(p)))>>(p))

int imm(char *);
int simm(char *);

#if INCLUDE_MKCOMMENT
int mkcomment(char *,char *,long v);
#else
#define mkcomment(a,b,c);
#endif

char prnbuf[128];

#define FS_(x)		(((x) >> 11) & ((1L <<  5) - 1))
#define FT_(x)		(((x) >> 16) & ((1L <<  5) - 1))
#define FD_(x)		(((x) >>  6) & ((1L <<  5) - 1))
#define RS_(x)		(((x) >> 21) & ((1L <<  5) - 1))
#define RT_(x)		(((x) >> 16) & ((1L <<  5) - 1))
#define RD_(x)		(((x) >> 11) & ((1L <<  5) - 1))
#define IMM_(x)		(((x) >>  0) & ((1L << 16) - 1))
#define TARGET_(x)	(((x) >>  0) & ((1L << 26) - 1))
#define SHAMT_(x)	(((x) >>  6) & ((1L <<  5) - 1))

#define comma()		strcat(dest,",")
#define rd()		strcat(dest,regname[(int)RD_(inst)])
#define rs()		strcat(dest,regname[(int)RS_(inst)])
#define rt()		strcat(dest,regname[(int)RT_(inst)])
#define fd()		strcat(dest,c1reg[(int)FD_(inst)])
#define fs()		strcat(dest,c1reg[(int)FS_(inst)])
#define ft()		strcat(dest,c1reg[(int)FT_(inst)])
#define c0()		strcat(dest,c0reg[(int)RD_(inst)])
#define c1()		strcat(dest,c1reg[(int)RD_(inst)])
#define c2()		strcat(dest,regs_c2d[(int)RD_(inst)])
#define cn()		strcat(dest,regs_hw[(int)RD_(inst)])
#define c0ft()		strcat(dest,c0reg[(int)RT_(inst)])
#define c1ft()		strcat(dest,c1reg[(int)RT_(inst)])
#define cnft()		strcat(dest,regs_hw[(int)RT_(inst)])
#define cc1()		strcat(dest,regs_hw[(int)RD_(inst)])
#define cc2()		strcat(dest,regs_c2c[(int)RD_(inst)])

typedef enum {
	RD_RS_RT, RT_RS_IMM, OFF, RS_RT_OFF, RS_OFF, NONE, RT_RD, COFUN,
	RS_RT, TARGET, JALR, RSX, RD_RT_SFT, LOAD_STORE, RT_IMM, RDX, RD_RT_RS,
	RT_RS_SIMM, RT_SIMM, JR, RT_C0, RT_C1, RT_CN, RT_CC1, LDSTC0, LDSTC1,
	LDSTCN, WORD, FT_FS_FD, FS_FD, FT_FS, RT_C2, RT_CC2, BPCODE, RS_SIMM,
	CACHE_OP, RD_RS, OFF_BASE
} TYPE;

typedef struct DISTBL {
	char *str;
	long mask, code;
	TYPE type;
} DISTBL;

/* software register names */
char *regs_sw[] = {
	"zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
	"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
	"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
	"t8", "t9", "k0", "k1", "gp", "sp", "s8", "ra"
};

/* hardware register names */
char *regs_hw[] = {
	"$0", "$1", "$2", "$3", "$4", "$5", "$6", "$7",
	"$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15",
	"$16", "$17", "$18", "$19", "$20", "$21", "$22", "$23",
	"$24", "$25", "$26", "$27", "$28", "$29", "$30", "$31"
};

char *regs_c0[] = {
	"C0_INX", "C0_RAND", "C0_TLBLO", "C0_BPC",
	"C0_CTEXT", "C0_BDA", "C0_TAR", "C0_DCIC",
	"C0_BADADDR", "$9", "C0_TLBHI", "$11",
	"C0_SR", "C0_CAUSE", "C0_EPC", "C0_PRID",
	"$16", "$17", "$18", "$19", "$20", "$21", "$22", "$23",
	"$24", "$25", "$26", "$27", "$28", "$29", "$30", "$31"
};

char *c1reg[] = {
	"$f0", "$f1", "$f2", "$f3", "$f4", "$f5", "$f6", "$f7",
	"$f8", "$f9", "$f10", "$f11", "$f12", "$f13", "$f14", "$f15",
	"$f16", "$f17", "$f18", "$f19", "$f20", "$f21", "$f22", "$f23",
	"$f24", "$f25", "$f26", "$f27", "$f28", "$f29", "$f30", "$f31"
};

DISTBL distbl[] = {
	{"sll", 0xffe0003fL, 0x00000000L, RD_RT_SFT},
	{"selsr", 0xfc0007ffL, 0x00000001L, RD_RS_RT},
	{"srl", 0xffe0003fL, 0x00000002L, RD_RT_SFT},
	{"sra", 0xffe0003fL, 0x00000003L, RD_RT_SFT},
	{"sllv", 0xfc0007ffL, 0x00000004L, RD_RT_RS},
	{"selsl", 0xfc0007ffL, 0x00000005L, RD_RS_RT},
	{"srlv", 0xfc0007ffL, 0x00000006L, RD_RT_RS},
	{"srav", 0xfc0007ffL, 0x00000007L, RD_RT_RS},
	{"jr", 0xfc1fffffL, 0x00000008L, JR},
	{"jalr", 0xfc1f07ffL, 0x00000009L, JALR},
	{"ffs", 0xfc1f07ffL, 0x0000000aL, RD_RS},
	{"ffc", 0xfc1f07ffL, 0x0000000bL, RD_RS},
	{"break", 0xfc00003fL, 0x0000000dL, BPCODE},
	{"mfhi", 0xffff07ffL, 0x00000010L, RDX},
	{"mthi", 0xfc1fffffL, 0x00000011L, RSX},
	{"mflo", 0xffff07ffL, 0x00000012L, RDX},
	{"mtlo", 0xfc1fffffL, 0x00000013L, RSX},
	{"mult", 0xfc00ffffL, 0x00000018L, RS_RT},
	{"multu", 0xfc00ffffL, 0x00000019L, RS_RT},
	{"div", 0xfc00ffffL, 0x0000001aL, RS_RT},
	{"divu", 0xfc00ffffL, 0x0000001bL, RS_RT},
	{"madd", 0xfc00ffffL, 0x0000001cL, RS_RT},
	{"maddu", 0xfc00ffffL, 0x0000001dL, RS_RT},
	{"msub", 0xfc00ffffL, 0x0000001eL, RS_RT},
	{"msubu", 0xfc00ffffL, 0x0000001fL, RS_RT},
	{"add", 0xfc0007ffL, 0x00000020L, RD_RS_RT},
	{"addu", 0xfc0007ffL, 0x00000021L, RD_RS_RT},
	{"sub", 0xfc0007ffL, 0x00000022L, RD_RS_RT},
	{"subu", 0xfc0007ffL, 0x00000023L, RD_RS_RT},
	{"and", 0xfc0007ffL, 0x00000024L, RD_RS_RT},
	{"or", 0xfc0007ffL, 0x00000025L, RD_RS_RT},
	{"xor", 0xfc0007ffL, 0x00000026L, RD_RS_RT},
	{"nor", 0xfc0007ffL, 0x00000027L, RD_RS_RT},
	{"min", 0xfc0007ffL, 0x00000028L, RD_RS_RT},
	{"max", 0xfc0007ffL, 0x00000029L, RD_RS_RT},
	{"slt", 0xfc0007ffL, 0x0000002aL, RD_RS_RT},
	{"sltu", 0xfc0007ffL, 0x0000002bL, RD_RS_RT},
	{"tge", 0xfc00003fL, 0x00000030L, RS_RT},
	{"tgeu", 0xfc00003fL, 0x00000031L, RS_RT},
	{"tlt", 0xfc00003fL, 0x00000032L, RS_RT},
	{"tltu", 0xfc00003fL, 0x00000033L, RS_RT},
	{"teq", 0xfc00003fL, 0x00000034L, RS_RT},
	{"tne", 0xfc00003fL, 0x00000036L, RS_RT},
	{"rmul", 0xfc0007ffL, 0x00000058L, RS_RT},
	{"radd", 0xfc0007ffL, 0x00000098L, RS_RT},
	{"rsub", 0xfc0007ffL, 0x000000d8L, RS_RT},
	{"r2u", 0xfc0007ffL, 0x00000118L, RSX},
	{"u2r", 0xfc0007ffL, 0x00000158L, RSX},
	{"bltz", 0xfc1f0000L, 0x04000000L, RS_OFF},
	{"bgez", 0xfc1f0000L, 0x04010000L, RS_OFF},
	{"bltzl", 0xfc1f0000L, 0x04020000L, RS_OFF},
	{"bgezl", 0xfc1f0000L, 0x04030000L, RS_OFF},
	{"tgei", 0xfc1f0000L, 0x04080000L, RS_SIMM},
	{"tgeiu", 0xfc1f0000L, 0x04090000L, RS_SIMM},
	{"tlti", 0xfc1f0000L, 0x040a0000L, RS_SIMM},
	{"tltiu", 0xfc1f0000L, 0x040b0000L, RS_SIMM},
	{"teqi", 0xfc1f0000L, 0x040c0000L, RS_SIMM},
	{"tnei", 0xfc1f0000L, 0x040e0000L, RS_SIMM},
	{"bltzal", 0xfc1f0000L, 0x04100000L, RS_OFF},
	{"bgezal", 0xfc1f0000L, 0x04110000L, RS_OFF},
	{"bltzall", 0xfc1f0000L, 0x04120000L, RS_OFF},
	{"bgezall", 0xfc1f0000L, 0x04130000L, RS_OFF},
	{"j", 0xfc000000L, 0x08000000L, TARGET},
	{"jal", 0xfc000000L, 0x0c000000L, TARGET},

	{"beq", 0xfc000000L, 0x10000000L, RS_RT_OFF},
	{"bne", 0xfc000000L, 0x14000000L, RS_RT_OFF},
	{"blez", 0xfc1f0000L, 0x18000000L, RS_OFF},
	{"bgtz", 0xfc1f0000L, 0x1c000000L, RS_OFF},

	{"addi", 0xfc000000L, 0x20000000L, RT_RS_SIMM},
	{"addiu", 0xfc000000L, 0x24000000L, RT_RS_SIMM},
	{"slti", 0xfc000000L, 0x28000000L, RT_RS_SIMM},
	{"sltiu", 0xfc000000L, 0x2c000000L, RT_RS_SIMM},

	{"andi", 0xfc000000L, 0x30000000L, RT_RS_IMM},
	{"ori", 0xfc000000L, 0x34000000L, RT_RS_IMM},
	{"xori", 0xfc000000L, 0x38000000L, RT_RS_IMM},
	{"lui", 0xfc000000L, 0x3c000000L, RT_IMM},

	{"mfc0", 0xffe007ffL, 0x40000000L, RT_C0},
	{"waiti", 0xffffffffL, 0x40000020L, NONE},
	{"ctc0", 0xffe007ffL, 0x40c00000L, RT_RD},
	{"cfc0", 0xffe007ffL, 0x40400000L, RT_RD},
	{"mtc0", 0xffe007ffL, 0x40800000L, RT_C0},
	{"bc0f", 0xffff0000L, 0x41000000L, OFF},
	{"bc0t", 0xffff0000L, 0x41010000L, OFF},
	{"bc0fl", 0xffff0000L, 0x41020000L, OFF},
	{"bc0tl", 0xffff0000L, 0x41030000L, OFF},
	{"tlbp", 0xffffffffL, 0x42000008L, NONE},
	{"tlbr", 0xffffffffL, 0x42000001L, NONE},
	{"tlbwi", 0xffffffffL, 0x42000002L, NONE},
	{"tlbwr", 0xffffffffL, 0x42000006L, NONE},
	{"rfe", 0xffffffffL, 0x42000010L, NONE},
	{"waiti", 0xffffffffL, 0x42000020L, NONE},
	{"cop0", 0xfe000000L, 0x42000000L, COFUN},
	{"mfc1", 0xffe007ffL, 0x44000000L, RT_C1},
	{"cfc1", 0xffe007ffL, 0x44400000L, RT_CC1},
	{"mtc1", 0xffe007ffL, 0x44800000L, RT_C1},
	{"ctc1", 0xffe007ffL, 0x44c00000L, RT_CC1},
	{"bc1f", 0xffff0000L, 0x45000000L, OFF},
	{"bc1t", 0xffff0000L, 0x45010000L, OFF},
	{"bc1fl", 0xffff0000L, 0x45020000L, OFF},
	{"bc1tl", 0xffff0000L, 0x45030000L, OFF},
	{"add.s", 0xfee0003fL, 0x46000000L, FT_FS_FD},
	{"add.d", 0xfee0003fL, 0x46200000L, FT_FS_FD},
	{"sub.s", 0xfee0003fL, 0x46000001L, FT_FS_FD},
	{"sub.d", 0xfee0003fL, 0x46200001L, FT_FS_FD},
	{"mul.s", 0xfee0003fL, 0x46000002L, FT_FS_FD},
	{"mul.d", 0xfee0003fL, 0x46200002L, FT_FS_FD},
	{"div.s", 0xfee0003fL, 0x46000003L, FT_FS_FD},
	{"div.d", 0xfee0003fL, 0x46200003L, FT_FS_FD},
	{"abs.s", 0xfee0003fL, 0x46000005L, FS_FD},
	{"abs.d", 0xfee0003fL, 0x46200005L, FS_FD},
	{"mov.s", 0xfee0003fL, 0x46000006L, FS_FD},
	{"mov.d", 0xfee0003fL, 0x46200006L, FS_FD},
	{"neg.s", 0xfee0003fL, 0x46000007L, FS_FD},
	{"neg.d", 0xfee0003fL, 0x46200007L, FS_FD},
	{"cvt.s.w", 0xfee0003fL, 0x46800020L, FS_FD},
	{"cvt.s.d", 0xfee0003fL, 0x46200020L, FS_FD},
	{"cvt.d.s", 0xfee0003fL, 0x46000021L, FS_FD},
	{"cvt.d.w", 0xfee0003fL, 0x46800021L, FS_FD},
	{"cvt.w.d", 0xfee0003fL, 0x46200024L, FS_FD},
	{"cvt.w.s", 0xfee0003fL, 0x46000024L, FS_FD},
	{"c.f.s", 0xfee0003fL, 0x46000030L, FT_FS},
	{"c.f.d", 0xfee0003fL, 0x46200030L, FT_FS},
	{"c.un.s", 0xfee0003fL, 0x46000031L, FT_FS},
	{"c.un.d", 0xfee0003fL, 0x46200031L, FT_FS},
	{"c.eq.s", 0xfee0003fL, 0x46000032L, FT_FS},
	{"c.eq.d", 0xfee0003fL, 0x46200032L, FT_FS},
	{"c.ueq.s", 0xfee0003fL, 0x46000033L, FT_FS},
	{"c.ueq.d", 0xfee0003fL, 0x46200033L, FT_FS},
	{"c.olt.s", 0xfee0003fL, 0x46000034L, FT_FS},
	{"c.olt.d", 0xfee0003fL, 0x46200034L, FT_FS},
	{"c.ult.s", 0xfee0003fL, 0x46000035L, FT_FS},
	{"c.ult.d", 0xfee0003fL, 0x46200035L, FT_FS},
	{"c.ole.s", 0xfee0003fL, 0x46000036L, FT_FS},
	{"c.ole.d", 0xfee0003fL, 0x46200036L, FT_FS},
	{"c.ule.s", 0xfee0003fL, 0x46000037L, FT_FS},
	{"c.ule.d", 0xfee0003fL, 0x46200037L, FT_FS},
	{"c.sf.s", 0xfee0003fL, 0x46000038L, FT_FS},
	{"c.sf.d", 0xfee0003fL, 0x46200038L, FT_FS},
	{"c.ngle.d", 0xfee0003fL, 0x46200039L, FT_FS},
	{"c.ngle.s", 0xfee0003fL, 0x46000039L, FT_FS},
	{"c.seq.s", 0xfee0003fL, 0x4600003aL, FT_FS},
	{"c.lt.s", 0xfee0003fL, 0x4600003cL, FT_FS},
	{"c.ngl.s", 0xfee0003fL, 0x4600003bL, FT_FS},
	{"c.nge.s", 0xfee0003fL, 0x4600003dL, FT_FS},
	{"c.le.s", 0xfee0003fL, 0x4600003eL, FT_FS},
	{"c.ngt.s", 0xfee0003fL, 0x4600003fL, FT_FS},
	{"c.seq.d", 0xfee0003fL, 0x4620003aL, FT_FS},
	{"c.ngl.d", 0xfee0003fL, 0x4620003bL, FT_FS},
	{"c.lt.d", 0xfee0003fL, 0x4620003cL, FT_FS},
	{"c.nge.d", 0xfee0003fL, 0x4620003dL, FT_FS},
	{"c.le.d", 0xfee0003fL, 0x4620003eL, FT_FS},
	{"c.ngt.d", 0xfee0003fL, 0x4620003fL, FT_FS},
	{"cop1", 0xfe000000L, 0x46000000L, COFUN},
	{"mfc2", 0xffe007ffL, 0x48000000L, RT_CN},
	{"cfc2", 0xffe007ffL, 0x48400000L, RT_CN},
	{"mtc2", 0xffe007ffL, 0x48800000L, RT_CN},
	{"ctc2", 0xffe007ffL, 0x48c00000L, RT_CN},
	{"bc2f", 0xffff0000L, 0x49000000L, OFF},
	{"bc2t", 0xffff0000L, 0x49010000L, OFF},
	{"bc2fl", 0xffff0000L, 0x49020000L, OFF},
	{"bc2tl", 0xffff0000L, 0x49030000L, OFF},

	{"cop2", 0xfe000000L, 0x4a000000L, COFUN},
	{"mfc3", 0xffe007ffL, 0x4c000000L, RT_CN},
	{"mtc3", 0xffe007ffL, 0x4c800000L, RT_CN},
	{"ctc3", 0xffe007ffL, 0x4cc00000L, RT_CN},
	{"cfc3", 0xffe007ffL, 0x4c400000L, RT_CN},
	{"bc3f", 0xffff0000L, 0x4d000000L, OFF},
	{"bc3t", 0xffff0000L, 0x4d010000L, OFF},
	{"bc3fl", 0xffff0000L, 0x4d020000L, OFF},
	{"bc3tl", 0xffff0000L, 0x4d030000L, OFF},
	{"cop3", 0xfe000000L, 0x4e000000L, COFUN},

	{"beql", 0xfc000000L, 0x50000000L, RS_RT_OFF},
	{"bnel", 0xfc000000L, 0x54000000L, RS_RT_OFF},
	{"blezl", 0xfc1f0000L, 0x58000000L, RS_OFF},
	{"bgtzl", 0xfc1f0000L, 0x5c000000L, RS_OFF},

	{"addciu", 0xfc000000L, 0x70000000L, RT_RS_IMM},

	{"lb", 0xfc000000L, 0x80000000L, LOAD_STORE},
	{"lh", 0xfc000000L, 0x84000000L, LOAD_STORE},
	{"lwl", 0xfc000000L, 0x88000000L, LOAD_STORE},
	{"lw", 0xfc000000L, 0x8c000000L, LOAD_STORE},

	{"lbu", 0xfc000000L, 0x90000000L, LOAD_STORE},
	{"lhu", 0xfc000000L, 0x94000000L, LOAD_STORE},
	{"lwr", 0xfc000000L, 0x98000000L, LOAD_STORE},

	{"sb", 0xfc000000L, 0xa0000000L, LOAD_STORE},
	{"sh", 0xfc000000L, 0xa4000000L, LOAD_STORE},
	{"sw", 0xfc000000L, 0xac000000L, LOAD_STORE},
	{"swl", 0xfc000000L, 0xa8000000L, LOAD_STORE},

	{"swr", 0xfc000000L, 0xb8000000L, LOAD_STORE},
	{"flushi", 0xfc1f0000L, 0xbc010000L, NONE},
	{"flushd", 0xfc1f0000L, 0xbc020000L, NONE},
	{"flushid", 0xfc1f0000L, 0xbc030000L, NONE},
	{"wb", 0xfc1f0000L, 0xbc040000L, OFF_BASE},
	{"cache", 0xfc000000L, 0xbc000000L, CACHE_OP},

	{"lwc0", 0xfc000000L, 0xc0000000L, LDSTC0},
	{"lwc1", 0xfc000000L, 0xc4000000L, LDSTC1},
	{"lwc2", 0xfc000000L, 0xc8000000L, LDSTCN},
	{"lwc3", 0xfc000000L, 0xcc000000L, LDSTCN},

	{"swc0", 0xfc000000L, 0xe0000000L, LDSTC0},
	{"swc1", 0xfc000000L, 0xe4000000L, LDSTC1},
	{"swc2", 0xfc000000L, 0xe8000000L, LDSTCN},
	{"swc3", 0xfc000000L, 0xec000000L, LDSTCN},

	/* must be last !! never be move/removed */
	{".word", 0x00000000L, 0x00000000L, WORD}
};

DISTBL *get_distbl();

long inst;
char **regname, **c0reg;

char *
strccat(char *s, char c)
{
	char	*base;

	base = s;
	while(*s)
		s++;
	*s++ = c;
	*s = 0;

	return(base);
}

/*************************************************************
*  long disasm(dest,addr,bits)
*	disassemble instruction 'bits'
*/
long 
disasm(char *dest, long addr, long bits)
{
	DISTBL *pt;
	char tmp[40], *p;
	long v, v1;
	int i;

	inst = bits;
	if (regname == 0)
		regname = regs_sw;
	if (c0reg == 0)
		c0reg = regs_c0;
	sprintf(dest, "%08lx: %08lx ", addr, inst);

	if (inst == 0L) {
		strcat(dest, "nop");
		return (addr + 4L);
	}
	if ((inst & 0x3F) == 0x0C) {
		sprintf(tmp, "syscall %ld",(inst & 0x03ffffc0) >> 6);
		strcat(dest,tmp);
		return (addr + 4L);
	}

	pt = get_distbl(inst);
	i = strlen(pt->str);
	strcat(dest, pt->str);
	while (i++ < 8)
		strcat(dest, " ");
	switch (pt->type) {
	case FT_FS_FD:
		fd(), comma(), fs(), comma(), ft();
		break;
	case FS_FD:
		fd(), comma(), fs();
		break;
	case FT_FS:
		fs(), comma(), ft();
		break;
	case RT_RS_IMM:
		rt(), comma(), rs(), comma(), imm(dest);
		break;
	case RT_RS_SIMM:
		rt(), comma(), rs(), comma(), simm(dest);
		break;
	case RS_SIMM:
		rs(), comma(), simm(dest);
		break;
	case RT_IMM:
		rt(), comma(), imm(dest);
		break;
	case RT_SIMM:
		rt(), comma(), simm(dest);
		break;
	case RT_RD:
		rt(), comma();
	case RDX:
		rd();
		break;
	case RT_C0:
		rt(), comma(), c0();
		break;
	case RT_C1:
		rt(), comma(), c1();
		break;
	case RT_CN:
		rt(), comma(), cn();
		break;
	case RT_CC1:
		rt(), comma(), cc1();
		break;
	case RD_RT_RS:
		rd(), comma(), rt(), comma();
	case JR:
	case RSX:
		rs();
		break;
	case RD_RS:
		rd(), comma(), rs();
		break;
	case RD_RS_RT:
		rd(), comma();
	case RS_RT:
		rs(), comma(), rt();
		break;
	case RD_RT_SFT:
		rd(), comma(), rt(), comma();
		sprintf(tmp, "0x%lx", SHAMT_(inst));
		strcat(dest, tmp);
		mkcomment(dest, "# %d", SHAMT_(inst));
		break;
	case RS_RT_OFF:
	case RS_OFF:
		rs(), comma();
		if (pt->type == RS_RT_OFF)
			rt(), comma();
	case OFF:
		v = IMM_(inst);
		if (v & (1L << 15))
			v |= 0xffff0000L;
		v1 = addr + 4L + (v << 2);
		sprintf(tmp, "%lx", v1);
		strcat(dest, tmp);
		mkcomment(dest, "# 0x%08lx", v1);
		break;
	case BPCODE:
		sprintf(tmp, "%ld", (inst >> 16) & 0x3ff);
		strcat(dest, tmp);
		break;
	case COFUN:
		sprintf(tmp, "0x%lx", inst & 0x01ffffffL);
		strcat(dest, tmp);
		break;
	case NONE:
		break;
	case TARGET:
		v = (inst & 0x03ffffffL) << 2;
		v |= (addr & 0xf0000000L);
		sprintf(tmp, "%lx", v);
		strcat(dest, tmp);
		mkcomment(dest, "# 0x%08lx", v);
		break;
	case JALR:
		if (RD_(inst) != 31L)
			rd(), comma();
		rs();
		break;
	case LDSTC0:
		v = IMM_(inst);
		if (v & (1L << 15))
			v |= 0xffff0000L;
		c0ft(), comma();
		sprintf(tmp, "%ld(", v);
		strcat(dest, tmp);
		rs();
		strcat(dest, ")");
		mkcomment(dest, "# 0x%x", v);
		break;
	case LDSTC1:
		v = IMM_(inst);
		if (v & (1L << 15))
			v |= 0xffff0000L;
		c1ft(), comma();
		sprintf(tmp, "%ld(", v);
		strcat(dest, tmp);
		rs();
		strcat(dest, ")");
		mkcomment(dest, "# 0x%x", v);
		break;
	case LDSTCN:
		v = IMM_(inst);
		if (v & (1L << 15))
			v |= 0xffff0000L;
		cnft(), comma();
		sprintf(tmp, "%ld(", v);
		strcat(dest, tmp);
		rs();
		strcat(dest, ")");
		mkcomment(dest, "# 0x%x", v);
		break;
	case CACHE_OP:
		v = IMM_(inst);
		if (v & (1L << 15))
			v |= 0xffff0000L;
		sprintf(tmp, "%ld,%ld(", RT_(inst), v);
		strcat(dest, tmp);
		rs();
		strcat(dest, ")");
		mkcomment(dest, "# 0x%x", v);
		break;
	case OFF_BASE:
		v = IMM_(inst);
		if (v & (1L << 15))
			v |= 0xffff0000L;
		sprintf(tmp, "%ld(", v);
		strcat(dest, tmp);
		rs();
		strcat(dest, ")");
		mkcomment(dest, "# 0x%x", v);
		break;
	case LOAD_STORE:
		v = IMM_(inst);
		if (v & (1L << 15))
			v |= 0xffff0000L;
		rt(), comma();
		sprintf(tmp, "%ld(", v);
		strcat(dest, tmp);
		rs();
		strcat(dest, ")");
		mkcomment(dest, "# 0x%x", v);
		break;
	case RT_C2:
	case RT_CC2:
		sprintf(tmp, "%08lx", inst);
		strcat(dest, tmp);
#if INCLUDE_MKCOMMENT
		strcat(dest, "      # ");
#endif
		p = (char *) &inst;
		for (i = 0; i < 4; i++) {
			v = *p++;
			if (isprint(v))
				strccat(dest, v);
			else
				strccat(dest, '.');
		}
		break;
	case WORD:
		sprintf(tmp, "%08lx", inst);
		strcat(dest, tmp);
		break;
	}
	return (addr + 4L);
}

/*************************************************************
*  DISTBL *get_distbl(bits)
*/
DISTBL *
get_distbl(long bits)
{
	DISTBL *pt = distbl;

	while ((bits & pt->mask) != pt->code)
		++pt;
	return (pt);
}


/*************************************************************
*  int is_branch(adr)
*/
int 
is_branch(unsigned long adr)
{
	DISTBL *pt;
	unsigned long inst;

	inst = *(unsigned long *)(adr);
	pt = get_distbl(inst);
	switch (pt->type) {
	case OFF:
	case RS_RT_OFF:
	case RS_OFF:
	case TARGET:
	case JALR:
	case JR:
		return (1);
	default:
		return (0);
	}
}

/*************************************************************
*  int is_branch_likely(adr)
*	returns true if instr at adr is a branch likely instr
*/
int 
is_branch_likely(unsigned long adr)
{
	DISTBL *pt;
	unsigned long inst;
	int len;

	inst = *(unsigned long *)(adr);
	pt = get_distbl(inst);
	if (pt->str[0] != 'b')
		return (0);
	len = strlen(pt->str);
	if (pt->str[len - 1] == 'l' && pt->str[len - 2] != 'a')
		return (1);
	return (0);
}

/*************************************************************
*  int is_conditional_branch(adr)
*/
int 
is_conditional_branch(unsigned long adr)
{
	DISTBL *pt;
	unsigned long inst;

	inst = *(unsigned long *)(adr);
	pt = get_distbl(inst);
	switch (pt->type) {
	case OFF:
	case RS_RT_OFF:
	case RS_OFF:
		return (1);
	default:
		return (0);
	}
}

/*************************************************************
*  is_jr(adr)
*/
int
is_jr(unsigned long adr)
{
	DISTBL *pt;
	unsigned long inst;

	inst = *(unsigned long *)(adr);
	pt = get_distbl(inst);
	if (pt->type == JR)
		return (1);
	return (0);
}

/*************************************************************
*  is_jal(adr)
*/
int
is_jal(unsigned long adr)
{
	unsigned long inst;
	int op;

	inst = *(unsigned long *)(adr);
	op = getfield(inst, 6, 26);
	if (op == 3)
		return (1);
	if (op == 0 && getfield(inst, 6, 0) == 9)
		return (1);
	return (0);
}

/*************************************************************
*  unsigned long branch_target_address(adr)
*/
unsigned long 
branch_target_address(adr)
unsigned long adr;
{
	DISTBL *pt;
	unsigned long val;
	unsigned long inst;
	extern unsigned long regtbl[];

	inst = *(unsigned long *)(adr);
	pt = get_distbl(inst);
	switch (pt->type) {
	case OFF:
	case RS_RT_OFF:
	case RS_OFF:
		val = inst & 0xffff;
		if (val & 0x8000)
			val |= 0xffff0000;
		return (adr + 4 + (val << 2));
	case TARGET:
		val = inst & 0x3ffffff;
		return (((adr + 4) & 0xf0000000) | (val << 2));
	case JALR:
	case JR:
		val = RS_(inst);
		return (regtbl[val]);
	default:
		return (0);
	}
}

/*************************************************************
*  is_writeable(adr)
*/
int
is_writeable(adr)
unsigned long adr;
{
	unsigned long x;

	x = *(unsigned long *)(adr);
	*(long *)adr =  ~x;
	flushDcache((char *)adr,4);
	if (*(unsigned long *)(adr) != ~x)
		return (0);
	*(long *)adr = x;
	return (1);
}

/*************************************************************
*  simm(dest)
*	signed immediate value
*/
int
simm(char *dest)
{
	char tmp[20];
	long v;

	v = IMM_(inst);
	sprintf(tmp, "0x%lx", v);
	strcat(dest, tmp);
	if (v >= 0 && v <= 9)
		return(0);
	if (v & (1L << 15))
		v |= 0xffff0000L;
	mkcomment(dest, "# %d", v);
	return(0);
}

/*************************************************************
*  imm(dest)
*	unsigned immediate value
*/
int
imm(char *dest)
{
	char tmp[20];
	long v;

	v = IMM_(inst);
	sprintf(tmp, "0x%lx", v);
	strcat(dest, tmp);
	if (v >= 0 && v <= 9)
		return(0);
	mkcomment(dest, "# %d", v);
	return(0);
}

#if INCLUDE_MKCOMMENT
/*************************************************************
*  mkcomment(p,fmt,v)
*	generate an appropriate comment
*/
int
mkcomment(char *p, char *fmt, long v)
{
	char tmp[20];
	int n;

	if (v >= 0 && v <= 9)
		return(0);
	for (n = 60 - strlen(p); n > 0; n--)
		strcat(p, " ");
	sprintf(tmp, fmt, v);
	strcat(p, tmp);
	return(0);
}
#endif

char *
DisHelp[] = {
	"Disassemble memory",
	"-[m]    {address} [linecount]",
	0,
};

/************************************************************************
 *                          dis
 ************************************************************************/
int
Dis(int argc, char *argv[])
{
	long	adr, lcnt;
	int		i, opt, more;

	lcnt = 8;
    more  = 0;
	while ((opt = getopt(argc,argv,"m")) != -1) {
		switch(opt) {
		case 'm':
			more = 1;
			break;
		default:
			return(0);
		}
	}

	if (argc < (optind+1))
		return(CMD_PARAM_ERROR);

	adr = strtoul(argv[optind],0,0);
	if (argc == (optind+2))
		lcnt = strtol(argv[optind+1],0,0);

	while (1) {
		for(i=0;i<lcnt;i++) {
			adr = disasm(prnbuf, adr, *(ulong *)(adr));
			printf("%s\n",prnbuf);
		}
		if (!more)
			break;
		if (!More())
			break;
	}
	return(CMD_SUCCESS);
}

#endif
