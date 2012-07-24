/* dis_sh.c:
 *	
 *	This file contains an disassembler for SH2 code.
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
 */
#include "config.h"
#if INCLUDE_DISASSEMBLER
#include "genlib.h"

typedef unsigned long ulong;
typedef unsigned short ushort;
typedef unsigned char uchar;

extern	getreg();

int	inst0000(), inst0001(), inst0010(), inst0011();
int	inst0100(), inst0101(), inst0110(), inst0111();
int	inst1000(), inst1001(), inst1010(), inst1011();
int	inst1100(), inst1101(), inst1110(), inst1111();

int	(*functbl[])() = {
	inst0000,	inst0001,	inst0010,	inst0011,
	inst0100,	inst0101,	inst0110,	inst0111,
	inst1000,	inst1001,	inst1010,	inst1011,
	inst1100,	inst1101,	inst1110,	inst1111,
};

char *DisHelp[] = {
	"Disassemble memory",
	"-[m] {address | .} [lines]",
	" -m  interactive (-more-)",
	0,
};

static	ushort *loc;

int
Dis(argc,argv)
int	argc;
char	*argv[];
{
	void	disass(ushort *,int,int);
	int lines, opt, more;
	ushort	*at;

	more = 0;
	while ((opt=getopt(argc,argv,"m")) != -1) {
		switch(opt) {
		case 'm':
			more = 1;
			break;
		default:
			return(0);
		}
	}

	if (*argv[optind] == '.')
		getreg("PC",&at);
	else
		at = (ushort *)strtoul(argv[optind],0,0);

	if (argc == (optind+2))
		lines = strtoul(argv[optind+1],0,0);
	else
		lines = 8;

	while(1) {
		disass((ushort *)at,lines,0);
		if (more && More())
			at = loc;
		else
			break;
	}
	return(0);
}

void
disass(ushort *inst, int lines, int next)
{
	while (lines) {
		if (next)
			printf("Nxt inst: %04x ",*inst); 
		else
			printf("%08lx: %04x ",(ulong)inst,*inst);
		if (functbl[(*inst & 0xf000) >> 12](inst) == -1)
			printf("???\n");
		lines--;
		inst++;
	}
	loc = inst;
}

int
inst0000(ushort *inst)
{
	int	rs8, rs4;

	rs8 = (*inst & 0x0f00) >> 8;
	rs4 = (*inst & 0x00f0) >> 4;

	if (*inst == 0x0008)
		printf("clrt\n");
	else if (*inst == 0x0009)
		printf("nop\n");
	else if (*inst == 0x000b)
		printf("rts\n");
	else if (*inst == 0x0018)
		printf("sett\n");
	else if (*inst == 0x0019)
		printf("div0u\n");
	else if (*inst == 0x001b)
		printf("sleep\n");
	else if (*inst == 0x0028)
		printf("clrmac\n");
	else if (*inst == 0x002b)
		printf("rte\n");

	else if ((*inst & 0x00ff) == 0x0002)
		printf("stc\tsr,r%d\n",				rs8);
	else if ((*inst & 0x00ff) == 0x0003)
		printf("bsrf\tr%d\n",				rs8);
	else if ((*inst & 0x000f) == 0x0004)
		printf("mov.b\tr%d,@(r0,r%d)\n",	rs4,rs8);
	else if ((*inst & 0x000f) == 0x0005)
		printf("mov.w\tr%d,@(r0,r%d)\n",	rs4,rs8);
	else if ((*inst & 0x000f) == 0x0006)
		printf("mov.l\tr%d,@(r0,r%d)\n",	rs4,rs8);
	else if ((*inst & 0x000f) == 0x0007)
		printf("mul.l\tr%d,r%d\n",			rs4,rs8);
	else if ((*inst & 0x00ff) == 0x000a)
		printf("sts\tmach,r%d\n",			rs8);
	else if ((*inst & 0x000f) == 0x000c)
		printf("mov.b\t@(r0,r%d),@r%d\n",	rs4,rs8);
	else if ((*inst & 0x000f) == 0x000d)
		printf("mov.w\t@(r0,r%d),r%d\n",	rs4,rs8);
	else if ((*inst & 0x000f) == 0x000e)
		printf("mov.l\t@(r0,r%d),r%d\n",	rs4,rs8);
	else if ((*inst & 0x000f) == 0x000f)
		printf("mac.l\t@r%d+,@r%d+\n",		rs4,rs8);

	else if ((*inst & 0x00ff) == 0x0012)
		printf("stc\tgbr,r%d\n",			rs8);
	else if ((*inst & 0x00ff) == 0x001a)
		printf("sts\tmacl,r%d\n",			rs8);
	else if ((*inst & 0x00ff) == 0x0022)
		printf("stc\tvbr,r%d\n",			rs8);
	else if ((*inst & 0x00ff) == 0x0023)
		printf("braf\tr%d\n",				rs8);
	else if ((*inst & 0x00ff) == 0x0029)
		printf("movt\tr%d\n",				rs8);
	else if ((*inst & 0x00ff) == 0x002a)
		printf("sts\tpr,r%d\n",				rs8);
	else
		return(-1);
	return(0);
}

int
inst0001(ushort *inst)
{
	printf("mov.l\tr%d,@(0x%x,r%d)\n",
		(*inst & 0x00f0) >> 4,(*inst & 0x00f) * 4,(*inst & 0x0f00) >> 8);
	return(0);
}

char *mnemfmt0010[] =  {
	"mov.b\tr%d,@r%d\n",
	"mov.w\tr%d,@r%d\n",
	"mov.l\tr%d,@r%d\n",
	0,
	"mov.b\tr%d,@-r%d\n",
	"mov.w\tr%d,@-r%d\n",
	"mov.l\tr%d,@-r%d\n",
	"div0s\tr%d,r%d\n",
	"tst\tr%d,r%d\n",
	"and\tr%d,r%d\n",
	"xor\tr%d,r%d\n",
	"or\tr%d,r%d\n",
	"cmp/str\tr%d,r%d\n",
	"xtrct\tr%d,r%d\n",
	"mulu.w\tr%d,r%d\n",
	"muls.w\tr%d,r%d\n",
};

int
inst0010(ushort *inst)
{
	char	*fmt;

	fmt = mnemfmt0010[*inst & 0x000f];
	if (!fmt)
		return(-1);
	printf(fmt,(*inst & 0x00f0) >> 4,(*inst & 0x0f00) >> 8);
	return(0);
}

char *mnemfmt0011[] =  {
	"cmp/eq\tr%d,r%d\n",
	0,
	"cmp/hs\tr%d,r%d\n",
	"cmp/ge\tr%d,r%d\n",
	"div1\tr%d,r%d\n",
	"dmulu.l\tr%d,r%d\n",
	"cmp/hi\tr%d,r%d\n",
	"cmp/gt\tr%d,r%d\n",
	"sub\tr%d,r%d\n",
	0,
	"subc\tr%d,r%d\n",
	"subv\tr%d,r%d\n",
	"add\tr%d,r%d\n",
	"dmuls.l\tr%d,r%d\n",
	"addc\tr%d,r%d\n",
	"addv\tr%d,r%d\n",
};

int
inst0011(ushort *inst)
{
	char	*fmt;

	fmt = mnemfmt0011[*inst & 0x000f];
	if (!fmt)
		return(-1);
	printf(fmt,(*inst & 0x00f0) >> 4,(*inst & 0x0f00) >> 8);
	return(0);
}

char *mnemfmt0100[] =  {
	"shll\tr%d\n",
	"shlr\tr%d\n",
	"sts.l\tmach,@-r%d\n",
	"stc.l\tsr,@-r%d\n",
	"rotl\tr%d\n",
	"rotr\tr%d\n",
	"lds\t@r%d+,mach\n",
	"ldc.l\t@r%d+,sr\n",
	"shll2\tr%d\n",
	"shlr2\tr%d\n",
	"lds\tr%d,mach\n",
	"jsr\t@r%d\n",
	0,
	0,
	"ldc\t@r%d,sr\n",
	0,
	"dt\tr%d\n",
	"cmp/pz\tr%d\n",
	"sts.l\tmacl,@-r%d\n",
	"stc.l\tgbr,@-r%d\n",
	0,
	"cmp/pl\tr%d\n",
	"lds\t@r%d+,macl\n",
	"ldc.l\t@r%d+,gbr\n",
	"shll8\tr%d\n",
	"shlr8\tr%d\n",
	"lds\tr%d,macl\n",
	"tas.b\t@r%d\n",
	0,
	0,
	"ldc\t@r%d,gbr\n",
	0,
	"shal\tr%d\n",
	"shar\tr%d\n",
	"sts.l\tpr,@-r%d\n",
	"stc.l\tvbr,@-r%d\n",
	"rotcl\tr%d\n",
	"rotcr\tr%d\n",
	"lds\t@r%d+,pr\n",
	"ldc.l\t@r%d+,vbr\n",
	"shll16\tr%d\n",
	"shlr16\tr%d\n",
	"lds\tr%d,pr\n",
	"jmp\t@r%d\n",
	0,
	0,
	"ldc\t@r%d,vbr\n",
};

int
inst0100(ushort *inst)
{
	char	*fmt;

	if ((*inst & 0x000ff) == 0x000f) {
		printf("mac.w\t@r%d+,@r%d+\n",
			(*inst & 0x00f0) >> 4,(*inst & 0x0f00) >> 8);
		return(0);
	}

	fmt = mnemfmt0100[*inst & 0x00ff];
	if (!fmt)
		return(-1);
	printf(fmt,(*inst & 0x0f00) >> 8);
	return(0);
}

int
inst0101(ushort *inst)
{
	printf("mov.l\t@(0x%x,r%d),r%d\n",
		((*inst & 0x000f)*4),((*inst & 0x00f0) >> 4),(*inst & 0x0f00) >> 8);
	return(0);
}

char *mnemfmt0110[] =  {
	"mov.b\t@r%d,r%d\n",
	"mov.w\t@r%d,r%d\n",
	"mov.l\t@r%d,r%d\n",
	"mov\tr%d,r%d\n",
	"mov.b\t@r%d+,r%d\n",
	"mov.w\t@r%d+,r%d\n",
	"mov.l\t@r%d+,r%d\n",
	"not\tr%d,r%d\n",
	"swap.b\tr%d,r%d\n",
	"swap.w\tr%d,r%d\n",
	"negc\tr%d,r%d\n",
	"neg\tr%d,r%d\n",
	"extu.b\tr%d,r%d\n",
	"extu.w\tr%d,r%d\n",
	"exts.b\tr%d,r%d\n",
	"exts.w\tr%d,r%d\n",
};

int
inst0110(ushort *inst)
{
	char	*fmt;

	fmt = mnemfmt0110[*inst & 0x000f];
	printf(fmt,(*inst & 0x00f0) >> 4,(*inst & 0x0f00) >> 8);
	return(0);
}

int
inst0111(ushort *inst)
{
	printf("add\t#0x%02x,r%d\n",(*inst & 0x00ff),(*inst & 0x0f00) >> 8);
	return(0);
}

int
inst1000(ushort *inst)
{
	unsigned long	rs4, label, disp;

	rs4 = (*inst & 0x00f0) >> 4;
	label = (ulong)(inst + ((*inst & 0x00ff) * 2));
	disp = *inst & 0x000f;

	if ((*inst & 0x0f00) == 0x0000)
		printf("mov.b\tr0,@(0x%lx,r%ld)\n",	disp,rs4);
	else if ((*inst & 0x0f00) == 0x0100)
		printf("mov.w\tr0,@(0x%lx,r%ld)\n",	(disp * 2),rs4);
	else if ((*inst & 0x0f00) == 0x0400)
		printf("mov.b\t@(0x%lx,r%ld),r0\n",	disp,rs4);
	else if ((*inst & 0x0f00) == 0x0500)
		printf("mov.w\t@(0x%lx,r%ld),r0\n",	(disp * 2),rs4);
	else if ((*inst & 0x0f00) == 0x0800)
		printf("cmp/eq\t#0x%02x,r0\n",		(*inst & 0x00ff));
	else if ((*inst & 0x0f00) == 0x0900)
		printf("bt\t0x%08lx\n",				label);
	else if ((*inst & 0x0f00) == 0x0b00)
		printf("bf\t0x%08lx\n",				label);
	else if ((*inst & 0x0f00) == 0x0d00)
		printf("bt/s\t0x%08lx\n",			label);
	else if ((*inst & 0x0f00) == 0x0f00)
		printf("bf/s\t0x%08lx\n",			label);
	else
		return(-1);
	return(0);
}

int
inst1001(ushort *inst)
{
	int	disp, rs8;

	disp = (*inst & 0x00ff);
	rs8 = (*inst & 0x0f00) >> 8;

	printf("mov.w\t@(0x%x,pc),r%d (0x%04x -> r%d)\n",
		disp*2,rs8,*(ushort *)(inst+disp+2),rs8);
	return(0);
}

int
inst1010(ushort *inst)
{
	printf("bra\t0x%08x\n",(int)inst + ((*inst & 0x0fff) * 2) + 4);
	return(0);
}

int
inst1011(ushort *inst)
{
	printf("bsr\t0x%08x\n",(int)inst + ((*inst & 0x0fff) * 2));
	return(0);
}

int
inst1100(ushort *inst)
{
	int	disp;

	disp = *inst & 0x00ff;
	
	if ((*inst & 0x0f00) == 0x0000)
		printf("mov.b\tr0,@(0x%x,gbr)\n",	disp);
	else if ((*inst & 0x0f00) == 0x0100)
		printf("mov.w\tr0,@(0x%x,gbr)\n",	disp*2);
	else if ((*inst & 0x0f00) == 0x0200)
		printf("mov.l\tr0,@(0x%x,gbr)\n",	disp*4);
	else if ((*inst & 0x0f00) == 0x0300)
		printf("trapa\t#0x%x\n",			disp);
	else if ((*inst & 0x0f00) == 0x0400)
		printf("mov.b\t@(0x%x,gbr),r0\n",	disp);
	else if ((*inst & 0x0f00) == 0x0500)
		printf("mov.w\t@(0x%x,gbr),r0\n",	disp*2);
	else if ((*inst & 0x0f00) == 0x0600)
		printf("mov.l\t@(0x%x,gbr),r0\n",	disp*4);
	else if ((*inst & 0x0f00) == 0x0700)
		printf("mova\t@(0x%x,pc),r0\n",		disp*4);
	else if ((*inst & 0x0f00) == 0x0800)
		printf("tst\t#0x%x,r0\n",			disp);
	else if ((*inst & 0x0f00) == 0x0900)
		printf("and\t#0x%02x,r0\n",			disp);
	else if ((*inst & 0x0f00) == 0x0a00)
		printf("xor\t#0x%x,r0\n",			disp);
	else if ((*inst & 0x0f00) == 0x0b00)
		printf("or\t#0x%x,r0\n",			disp);
	else if ((*inst & 0x0f00) == 0x0c00)
		printf("tst.b\t#0x%x,@(r0,gbr)\n",	disp);
	else if ((*inst & 0x0f00) == 0x0d00)
		printf("and.b\t#0x%02x,@(r0,gbr)\n",disp);
	else if ((*inst & 0x0f00) == 0x0e00)
		printf("xor.b\t#0x%x,@(r0,gbr)\n",	disp);
	else if ((*inst & 0x0f00) == 0x0f00)
		printf("or.b\t#0x%x,@(r0,gbr)\n",	disp);
	else
		return(-1);
	return(0);
}

int
inst1101(ushort *inst)
{
	int	disp, rs8;

	disp = *inst & 0x00ff;
	rs8 = (*inst & 0x0f00) >> 8;
	printf("mov.l\t@(0x%x,pc),r%d (0x%08lx -> r%d)\n",
		disp*4,rs8,
		*(ulong *)((((int)inst&~3)+4)+(disp*4)),
		rs8);
	return(0);
}

int
ginst1101(ushort *inst)
{
	printf("mov.l\t@(0x%x,pc),r%d (0x%08lx -> r%d)\n",
		((*inst & 0x00ff)*4),(*inst & 0x0f00) >> 8,
		*(long *)(inst+((*inst & 0x00ff)*2)+2), (*inst & 0x0f00) >> 8);
	return(0);
}

int
inst1110(ushort *inst)
{
	printf("mov\t#0x%x,r%d\n",(*inst & 0x00ff),((*inst & 0x0f00) >> 8));
	return(0);
}

int
inst1111(ushort *inst)
{
	return(-1);
}
#endif
