/* except.c:

	General notice:
	This code is part of a boot-monitor package developed as a generic base
	platform for embedded system designs.  As such, it is likely to be
	distributed to various projects beyond the control of the original
	author.  Please notify the author of any enhancements made or bugs found
	so that all may benefit from the changes.  In addition, notification back
	to the author will allow the new user to pick up changes that may have
	been made by other users after this version of the code was distributed.

	Author:	Ed Sutter
	email:	esutter@lucent.com		(home: lesutter@worldnet.att.net)
	phone:	908-582-2351			(home: 908-889-5161)
*/
#include "config.h"
#include "cpu.h"
#include "cpuio.h"
#include "stddefs.h"
#include "genlib.h"
#include "warmstart.h"

extern	int getreg(), putreg(), putvbr();

int	ExceptionType;
ulong	ExceptionAddr;
ulong	VectorTable[256];

char *
ExceptionType2String(int type)
{
	/* Needs work...
	 */
	return("???");
}

void
exception()
{
	extern	void	coldstart();
	int		vnum;
	char	*vname;
	ulong	pc;

	vnum = (int)(ExceptionType & 0xff);
	getreg("PC",&pc);

	/* First check to see if the vector that was just taken is the one */
	/* used by the 'at' command.  If yes, then there are two possibilities: */
	/* 1. If StateOfMonitor == BREAKPOINT, then this trap is the one that */
	/*    is used by the breakpoint handler for the instruction AFTER the */
	/*    breakpoint address to allow the breakpoint to be re-installed. */
	/* 2. Otherwise, it is a trap that must be handed over to attrap(). */

	vname = "";
	printf("EXCEPTION: #%d ",vnum);
	if ((vnum >=64) && (vnum <=71)) {
		pc -= 2;
		putreg("PC",pc);
		printf("(IRQ%d) @ 0x%lx\n",vnum-64,pc);
	}
	else if ((vnum >= 32) && (vnum <= 63)) {
		pc -= 2;
		putreg("PC",pc);
		printf("(TRAP%d) @ 0x%lx\n",vnum-32,pc);
	}
	else if ((vnum >= 72) && (vnum <= 255)) {
		pc -= 2;
		putreg("PC",pc);
		printf("(On-Chip-Peripheral) @ 0x%lx\n",pc);
	}
	else {
		switch(vnum) {
			case 4:
				vname = "(General Illegal Instruction)";
				break;
			case 6:
				vname = "(Slot Illegal Instruction)";
				break;
			case 9:
				vname = "(CPU Address Error)";
				pc -= 2;
				break;
			case 10:
				vname = "(DMAC/DTC Address Error)";
				break;
			case 11:
				vname = "(NMI)";
				break;
			case 12:
				vname = "(User Break)";
				break;
		}
		printf("%s @ 0x%lx\n",vname,pc);
	}
	ExceptionAddr = pc;
	showregs();
	intsoff();
	if (pollConsole("coldstart?"))
		warmstart(APP_EXIT);
	else
		coldstart();
}

void
vinit()
{
	extern	void	Vector4(), Vector5(), coldstart();
	extern	ulong	MonStackEnd;
	int		i, offset, vptr;

	VectorTable[0] = (ulong)coldstart;
	VectorTable[1] = (ulong)&MonStackEnd;
	VectorTable[2] = (ulong)coldstart;
	VectorTable[3] = (ulong)&MonStackEnd;

	offset = (int)Vector5 - (int)Vector4;
	vptr = (int)Vector4;
	for(i=4;i<256;i++) {
		VectorTable[i] = vptr;
		vptr += offset; 
	}
	putvbr(VectorTable);
}


/* Put cacheinit in this file because it is CPU specific... */
/* Enable cache for FLASH and DRAM. */
void
cacheInitForTarget()
{
	int	i;
	volatile ushort *addarray;

	addarray = CAAWRS;
	for(i=0;i<256;i++)
		*addarray++ = 0;
	*CCR = CACHEN_CS0 | CACHEN_DRAM;
}
