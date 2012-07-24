/* except_sh3.c:
 * 	SH3 exception handling.
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
 *	Author:	Torsten Tetz, Bosch Security Systems
 * 	based on except_sh2.c by Ed Sutter
 */
#include "config.h"
#include "stddefs.h"
#include "genlib.h"			/* The getreg() function is placed in target/common/reg_cache.c. */
					/* The prototype is in genlib.h. */
#include "cpu.h"
#include "cpuio.h"			/* The port specific functions getsr(), putsr() and putvbr() are part of cpuio.c.*/
#include "warmstart.h"

int		ExceptionType;
unsigned long	ExceptionAddr;

/* ExceptionType2String():
 * This function returns a string holding a description for the
 * given exception. The function is used e.g. by exceptionAutoRestart()
 * in misc.c or start() in start.c.
 */
char *
ExceptionType2String(int type)
{
	/* Needs work...
	 */
	return("???");
}

/* exception():
 * This function is called by from vectors_sh3.S when an exception occured.
 * The global variable ExceptionType is also set in vectors_sh3.S.
 * Resume after handling an exception is prepared in vectors_sh3.S but not
 * used so far. Right now exception handling always ends with a restart of
 * uMon.
 */
void
exception(void)
{
	extern	void	coldstart();
	int		vnum;
	char		*vname;
	ulong		pc;

	vnum = (int)(ExceptionType & 0xff);
	getreg("PC",&pc);

	printf("EXCEPTION: #%d ",vnum);
	
	/* Choose text for console output depending on the exception offset. */
	switch(vnum) {
		case 1:	
			vname = "(General exception)";
			break;
		case 2:
			vname = "(TLB miss)";
			break;
		case 3:
			vname = "(Interrupt)";
			break;
		default:
			vname = "(Unknown)";
			break;
	}
	printf("%s @ 0x%lx\n",vname,pc);

	ExceptionAddr = pc;
	showregs();
	intsoff();

	if (pollConsole("coldstart?")) {
		warmstart(APP_EXIT);
	}
	else {
		coldstart();
	}	
}

/* vinit():
 * This function is called by init1() in start.c and initializes the
 * exception vector table. The table is defined in vectors_sh3.S.
 */
void
vinit(void)
{
	extern 	void	VectorTable();
	
	putvbr((ulong)VectorTable);

	/* Clear Block bit in StatusRegister.
	 * The block bit is set after reset. It can now be safely removed
	 * as we now have an exception vector table.
	 */
	putsr( (getsr()) & ~SR_BLOCKBIT );
}

