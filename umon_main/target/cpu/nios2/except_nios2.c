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
 *  email:  esutter@lucent.com
 *  phone: 908-582-2351
 *
 */
#include "stddefs.h"
#include "config.h"
#include "genlib.h"
#include "cpuio.h"
#include "nios2.h"
#include "warmstart.h"

unsigned long    ExceptionAddr;
int              ExceptionType, IntNum;

extern void (*_swi_int_handler)();
extern void (*_hwi_int_handler)();

extern ulong hal_vsr_table[2];

/***********************************************************************
 *
 * umon_exception() & umon_swi_handler():
 * These two functions are the default exception handlers used by the
 * low level code in vectors_arm.s.
 * The SWI handler is slightly different because it is passed the address
 * of the exception plus the SWI number.
 */
void
umon_hwi_handler( ulong addr, ulong hwinum)
{
   ExceptionAddr = addr;
   ExceptionType = EXCTYPE_HWI;
   IntNum        = hwinum;
   monrestart(EXCEPTION);
}

void
umon_swi_handler( ulong addr)
{
   ExceptionAddr = addr;
   ExceptionType = EXCTYPE_SWI;
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
  hal_vsr_table[VEC_SWI] = (ulong)_swi_int_handler;
  hal_vsr_table[VEC_HWI] = (ulong)_hwi_int_handler;
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
      case EXCTYPE_HWI:
         sprintf(buf,"Hardware Interrupt %d",IntNum);
         string = buf;
         break;
      case EXCTYPE_SWI:
         sprintf(buf,"Software Interrupt");
         sprintf(buf,"Software Interrupt at 0x%8x : 0x%8x", ExceptionAddr, *(int *)ExceptionAddr);
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
}
