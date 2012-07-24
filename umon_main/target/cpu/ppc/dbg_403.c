/* debugPPC40X.c:
 *	Provide some convenient access to the PPC403 & PPC405's debug registers...
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
#include "genlib.h"
#include "stddefs.h"
#include "arch_ppc.h"

struct regfunc {
	char *name;
	void (*putfunc)(unsigned long);
	unsigned long (*getfunc)(void);
};

struct regfunc regfunctbl[] = {
	{ "dbcr0",	ppcMtdbcr0,		ppcMfdbcr0 },
	{ "dbcr1",	ppcMtdbcr1,		ppcMfdbcr1 },
	{ "dbsr",	ppcMtdbsr,		ppcMfdbsr },
	{ "iac1",	ppcMtiac1,		ppcMfiac1 },
	{ "iac2",	ppcMtiac2,		ppcMfiac2 },
	{ "iac3",	ppcMtiac3,		ppcMfiac3 },
	{ "iac4",	ppcMtiac4,		ppcMfiac4 },
	{ "dac1",	ppcMtdac1,		ppcMfdac1 },
	{ "dac2",	ppcMtdac2,		ppcMfdac2 },
	{ "dvc1",	ppcMtdvc1,		ppcMfdvc1 },
	{ "dvc2",	ppcMtdvc2,		ppcMfdvc2 },
	{ "msr",	ppcMtmsr,		ppcMfmsr },
	{ 0,0,0 }
};

void
enableInternalDebug()
{
	ppcMtmsr(ppcMfmsr() | 0x00000200);
	ppcMtdbcr0(ppcMfdbcr0() | 0x40000000);
}

void
disableInternalDebug()
{
	ppcMtmsr(ppcMfmsr() & ~0x00000200);
	ppcMtdbcr0(ppcMfdbcr0() & ~0x40000000);
}

void
badDebugReg(char *name)
{
	printf("Unrecognized debug register name: %s\n",name);
}

int
getDebugReg(char *name, ulong *value)
{
	struct	regfunc *rfp;

	for(rfp = regfunctbl; rfp->name; rfp++) {
		if (strcmp(name,rfp->name) == 0) {
			*value = rfp->getfunc();
			return(1);
		}
	}
	badDebugReg(name);
	return(0);
}

int
setDebugReg(char *name, ulong value)
{
	struct	regfunc *rfp;

	for(rfp = regfunctbl; rfp->name; rfp++) {
		if (strcmp(name,rfp->name) == 0) {
			rfp->putfunc(value);
			return(1);
		}
	}
	badDebugReg(name);
	return(0);
}

void
listDebugReg(void)
{
	int i;
	struct	regfunc *rfp;

	i = 1;
	for(rfp = regfunctbl; rfp->name; rfp++,i++) {
		printf("%-12s",rfp->name);
		if (i == 6) {
			putchar('\n');
			i = 0;
		}
	}
	if (i != 0)
		putchar('\n');
}

void
dumpDebugReg(void)
{
	int i;
	struct	regfunc *rfp;

	i = 1;
	for(rfp = regfunctbl; rfp->name; rfp++,i++) {
		printf("%8s: 0x%08lx",rfp->name,rfp->getfunc());
		if (i == 3) {
			putchar('\n');
			i = 0;
		}
		else
			printf(",  ");
	}
	if (i != 0)
		putchar('\n');
}


char *DebugPPC40XHelp[] = {
    "Debug register access",
    "-[EDv:] {regname} [value]",
    " -E               enable internal debug",
    " -D               disable internal debug",
    " -v {varname}     put retreived value into shell var 'varname'",
	"Note...",
    "Set regname to '?' for list of valid registers.",
    "Set regname to 'all' to dump content of all registers.",
    0,
};

int
DebugPPC40X(int argc,char *argv[])
{
    int		opt;
	char	*varname;
	ulong	regval;

	varname = (char *)0;
    while ((opt=getopt(argc,argv,"EDv:")) != -1) {
		switch(opt) {
			case 'D':
				disableInternalDebug();
				return(0);
			case 'E':
				enableInternalDebug();
				return(0);
			case 'v':
				varname = optarg;
				break;
			default:
				return(0);
		}
	}

	if (argc == optind+2) {
			setDebugReg(argv[optind],strtoul(argv[optind+1],0,0));
	}
	else if (argc == optind+1) {
		if (strcmp(argv[optind],"?") == 0) 
			listDebugReg();
		else if (strcmp(argv[optind],"all") == 0) 
			dumpDebugReg();
		else {
			getDebugReg(argv[optind],&regval);
			if (varname)
				shell_sprintf(varname,"0x%lx",regval);
			else
				printf("%s: 0x%lx\n",argv[optind],regval);
		}
	}
	else
		return(-1);
    return(0);
}


#if 0
/**************************************************************************/

#define BP_READ		1
#define BP_WRITE	2

char *BreakPointHelp[] = {
    "Use CPU facilites to set a breakpoint",
    "-[D] {type} {address}",
    " -D               disable internal debug mode.",
    " Valid types are:",
	"    'instr'  for setting an instruction fetch breakpoint",
	"    'dread'  for setting a data read breakpoint",
	"    'dwrite' for setting a data write breakpoint",
	"    'drdwr'  for setting a data read/write breakpoint",
    0,
};


int
BreakPoint(int argc, char *argv[])
{
	int	opt;

    while ((opt=getopt(argc,argv,"D")) != -1) {
		switch(opt) {
			case 'D':
				disableInternalDebug();
				break;
			default:
				return(0);
		}
	}
	if (argc != optind+1) {
		disableInternalDebug();
		return(-1);
	}

	if (!strcmp(argv[optind],"inst")) {
	}
	else if (!strcmp(argv[optind],"dread")) {
	}
	else if (!strcmp(argv[optind],"dwrite")) {
	}
	else if (!strcmp(argv[optind],"drdwr")) {
	}
	else
		return(-1);
	
	enableInternalDebug();
	return(0);
}
#endif
