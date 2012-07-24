/* 
 * except.c:
 *
 * This code handles exceptions that are caught by the exception
 * vectors installed by the monitor. These exception vectors look for
 * user-installed exception handlers and call them if found. Otherwise
 * they issue a software interrupt (trap, SWI) which engages the
 * monitor. In goes like this:
 *
 * by Nick Patavalis (npat@inaccessnetworks.com)
 *
 * General notice: 
 *   This code is part of a boot-monitor package
 *   developed as a generic base platform for embedded system designs.
 *   As such, it is likely to be distributed to various projects
 *   beyond the control of the original author.  Please notify the
 *   author of any enhancements made or bugs found so that all may
 *   benefit from the changes.  In addition, notification back to the
 *   author will allow the new user to pick up changes that may have
 *   been made by other users after this version of the code was
 *   distributed.
 *
 * Micromonitor Author: Ed Sutter
 *	 email:	esutter@lucent.com 
 *	 phone: 908-582-2351
 *
 */

#include "config.h"
#include "arm.h"
#include "genlib.h"
#include "cpuio.h"
#include "stddefs.h"
#include "warmstart.h"

ulong	ExceptionAddr;
int		ExceptionType, SwiNum;

void (* usr_handle_swi)();
void (* usr_handle_irq)();
void (* usr_handle_firq)();
void (* usr_handle_abt_data)();
void (* usr_handle_und_instr)();
void (* usr_handle_abt_prefetch)();

/***********************************************************************
 *
 * assign_handler():
 * This function is hooked to the monitor's API function mon_assignhandler().
 * to allow the application to assign an exception handler to any of the
 * ARM exceptions.
 */

unsigned long
assign_handler(long except_id, ulong fptr, ulong notused)
{
	unsigned long ofptr;

	switch(except_id) {
		case VEC_UND:
			ofptr = (unsigned long)usr_handle_und_instr;
			usr_handle_und_instr = (void (*)())fptr;
			break;
		case VEC_ABP:
			ofptr = (unsigned long)usr_handle_abt_prefetch;
			usr_handle_abt_prefetch = (void(*)())fptr;
			break;
		case VEC_ABD:
			ofptr = (unsigned long)usr_handle_abt_data;
			usr_handle_abt_data = (void(*)())fptr;
			break;
		case VEC_SWI:
			ofptr = (unsigned long)usr_handle_swi;
			usr_handle_swi = (void(*)())fptr;
			break;
		case VEC_IRQ:
			ofptr = (unsigned long)usr_handle_irq;
			usr_handle_irq = (void(*)())fptr;
			break;
		case VEC_FIQ:
			ofptr = (unsigned long)usr_handle_firq;
			usr_handle_firq = (void(*)())fptr;
			break;
		default:
			ofptr = 0;
			break;
	}
	return(ofptr);
}

/***********************************************************************
 *
 * umon_exception() & umon_swi_handler():
 * These two functions are the default exception handlers used by the
 * low level code in vectors_arm.s.
 * The SWI handler is slightly different because it is passed the address
 * of the exception plus the SWI number.
 */
void
umon_exception(ulong addr, ulong type)
{
	ExceptionAddr = addr;
	ExceptionType = type;
	monrestart(EXCEPTION);
}

void
umon_swi_handler(ulong addr, ulong swinum)
{
	ExceptionAddr = addr;
	ExceptionType = EXCTYPE_SWI;
	SwiNum = swinum;
	monrestart(EXCEPTION);
}

/***********************************************************************
 *
 * vinit():
 * Called at startup to initialize the vector table.
 * In the case of the ARM, the vector table is fixed in ROM, so this
 * function initializes the function pointers used by the low level
 * exception handler in the monitor.
 * This points all exceptions to the monitor's default exception handling
 * routines.
 */

void
vinit(void)
{
	usr_handle_swi = 0;
	usr_handle_irq = 0;
	usr_handle_firq = 0;
	usr_handle_abt_data = 0;
	usr_handle_und_instr = 0;
	usr_handle_abt_prefetch = 0;
}

/***********************************************************************
 *
 * ExceptionType2String():
 * This function simply returns a string that verbosely describes
 * the incoming exception type (vector number).
 */
char *
ExceptionType2String(int type)
{
	char *string;
	static char buf[32];

	switch(type) {
		case EXCTYPE_UNDEF:
			string = "Undefined instruction";
			break;
		case EXCTYPE_ABORTP:
			string = "Abort prefetch";
			break;
		case EXCTYPE_ABORTD:
			string = "Abort data";
			break;
		case EXCTYPE_IRQ:
			string = "IRQ";
			break;
		case EXCTYPE_FIRQ:
			string = "Fast IRQ";
			break;
		case EXCTYPE_NOTASSGN:
			string = "Not assigned";
			break;
		case EXCTYPE_SWI:
			sprintf(buf,"Software Interrupt %d",SwiNum);
			string = buf;
			break;
		default:
			string = "???";
			break;
	}
	return(string);
}

void
DumpInstalledHandlers(void)
{
	printf("SWI: 0x%lx\n",(long)usr_handle_swi);
	printf("IRQ: 0x%lx\n",(long)usr_handle_irq);
	printf("FIRQ: 0x%lx\n",(long)usr_handle_firq);
	printf("ABTD: 0x%lx\n",(long)usr_handle_abt_data);
	printf("ABTP: 0x%lx\n",(long)usr_handle_abt_prefetch);
	printf("UNDEF: 0x%lx\n",(long)usr_handle_und_instr);
}
