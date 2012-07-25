/*
All files except if stated otherwise in the begining of the file are under the ISC license:
-----------------------------------------------------------------------------------

Copyright (c) 2010-2012 Design Art Networks Ltd.

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

-----------------------------------------------------------------------------------
*/

/*****************************************************************************\
 Library    :  uMON (Micro Monitor)
 Filename   :  cofig.h
 Purpose    :  Main configuration file needed by all other uMON modules
 Owner		:  Sergey Krasnitsky
 Created    :  24.10.2010
 
 Note: 
    Despite this file is located in win3400 folder and it is dedicated 
    for Windows simulator only, we are using #if UMON_TARGET == UMON_TARGET_WIN
    in order to be consistent.
\*****************************************************************************/

#ifndef _CONFIG_H
#define _CONFIG_H


/*****************************************************************************\
                Addition definitions for Windows only
\*****************************************************************************/
#define UMON_TARGET_WIN	   0	// This Windows simulator


/*****************************************************************************\
 						Platform & CPU names
\*****************************************************************************/

extern const char *CPU_name;

#define PLATFORM_NAME			"DAN3400 Board"
#define CPU_NAME    			"DAN3400 - Windows simulator"


/*****************************************************************************\
            UMON_TARGET (target configuration) possible values
\*****************************************************************************/

#define UMON_TARGET_XT		        1	// Tensilica CPU, real board with UART
#define UMON_TARGET_ARM             4	// ARM CPU


/*****************************************************************************\
                UMON_DEBUG possible values
\*****************************************************************************/

#define UMON_DEBUG_OFF		        0	// Debug On
#define UMON_DEBUG_ON		        1	// Debug Off

#if !defined(UMON_DEBUG)
#define UMON_DEBUG UMON_DEBUG_OFF
#endif

#if !defined(UMON_DEBUG) || ((UMON_DEBUG != UMON_DEBUG_OFF) && (UMON_DEBUG != UMON_DEBUG_ON))
#error UMON_DEBUG not defined correctly. Possible values: UMON_DEBUG_OFF(0), UMON_DEBUG_ON(1)
#endif


/*****************************************************************************\
								Common types
\*****************************************************************************/

#define uint32  unsigned
#define uint16  unsigned short
#define uint8   unsigned char
#define int32   int
#define int16   signed short
#define int8    signed char
#define uint64  unsigned long long
#define int64   long long

#define UINT32  unsigned
#define UINT16  unsigned short

#define VOID	void 

#ifndef NULL
#define NULL    0
#endif

#ifndef BOOL
#define BOOL    int
#endif

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifndef __cplusplus
#ifndef bool
#define bool    BOOL
#endif
// conflicts with the Umon code:
// #ifndef true
// #define true    TRUE
// #endif
#ifndef false
#define false   FALSE
#endif
#endif /* __cplusplus */


#if UMON_TARGET == UMON_TARGET_WIN
#define __func__ __FUNCTION__
#endif


/*****************************************************************************\
						Memory registers access macros
\*****************************************************************************/

#define RegWrite8(Address,Data)		*((volatile uint8* ) (Address)) = (Data)
#define RegRead8(Address)			*((volatile uint8* ) (Address))
#define RegWrite16(Address,Data)	*((volatile uint16*) (Address)) = (Data)
#define RegRead16(Address)			*((volatile uint16*) (Address))
#define RegWrite32(Address,Data)	*((volatile uint32*) (Address)) = (Data)
#define RegRead32(Address)			*((volatile uint32*) (Address))
#define RegWrite64(Address,Data)	*((volatile uint64*) (Address)) = (Data)
#define RegRead64(Address)			*((volatile uint64*) (Address))



/*****************************************************************************\
								Common macros
\*****************************************************************************/
#define	UNUSED(x)		((void)(x))

#define	MIN(x, y)		((x) > (y)) ? (y) : (x)
#define	MAX(x, y)		((x) > (y)) ? (x) : (y)


/*****************************************************************************\
								    ASSERTS
\*****************************************************************************/
#define ASSERT(cond)											\
	if (!(cond))												\
{																\
	printf("ASSERT: File %s, Line %u\n", __FILE__, __LINE__);	\
	while(1);													\
}


/*****************************************************************************\
                UMON_TARGET supported by this makefile
\*****************************************************************************/

#if UMON_TARGET != UMON_TARGET_WIN
#error UMON_TARGET not defined correctly. Possible values: UMON_TARGET_WIN(0)
#endif



/*****************************************************************************\
					Other Umon global flags & constants
\*****************************************************************************/

#define CPU_LE					// We are little-endian

/* LOOPS_PER_SECOND:
 * Approximately the size of a loop that will cause a 1-second delay.
 * This value can be adjusted by using the "sleep -c" command.
 */
#define LOOPS_PER_SECOND		140000
#define TIMER_TICKS_PER_MSEC	20000

/* ALLOCSIZE:
 * Specify the size of the memory block (in monitor space) that
 * is to be allocated to malloc in the monitor.  Note that this
 * size can be dynamically increased using the heap command at
 * runtime.
 */
#define ALLOCSIZE 				(256*1024)

/* MONSTACKSIZE:
 * The amount of space allocated to the monitor's stack. 
 * This is the size in bytes of MonStack[] array, defined in start.c
 * KS: we are not using this stack mechanizm, so defining the dummy.
 */
#define MONSTACKSIZE			4

#define FLASHBANKS				1
#define BOOTROMBASE_OVERRIDE	0x10000000	// KS: UMON-TODO

#define CLK_GEN_BASE			0xE573E000
#define MASTER_RESET_ADDR		CLK_GEN_BASE 
#define MASTER_RESET_VAL		0x00010000 

// Base addr for our UART device control registers 
// Used in standard uart16550 driver
#ifdef DAN2400
#define CONSOLE_UART_BASE		0x20002000
#else
#define CONSOLE_UART_BASE		0xE572E000
#endif


// Required by uart16550 address step (in bytes) between two subsequent registers
#define SIO_STEP				4

// Required by start.c
#define DEFAULT_BAUD_RATE		115200

// There is an inconsistence in the Umon's naming of devInit and InitUART 
// (from uart16550) funcs. We want use this name: InitUART
#define devInit	InitUART

// Umon uses the following bss symbols (start.c, misc.c)
// They are automatically generated by the linker with leading "_"
// Redefine them when necessary 
#if UMON_TARGET == UMON_TARGET_ARM
extern int          __bss_start;
extern int          _end;
#define bss_start 	__bss_start
#define bss_end 	_end
#else
extern int          _bss_start;
extern int          _bss_end;
#define bss_start 	_bss_start
#define bss_end 	_bss_end
#endif

/*****************************************************************************\
						    APPLICATION BASE
\*****************************************************************************/
extern int apprambase_win [0x100000];
#define APPRAMBASE_OVERRIDE &apprambase_win;

/*****************************************************************************\
						FLASH & TFS configuration
\*****************************************************************************/

#define FLASH3400_SYS_DEVID		0           // actually it's Flash Bank number (in DAN3400 0..3)
#define FLASH3400_SECTORSIZE	0x10000
#define FLASH3400_START_SECTOR	0
#define FLASH3400_NUMSECTORS	2


#if UMON_TARGET == UMON_TARGET_WIN

// Simulate the Flash RAM region for MS Win32
// We need allocate FLASH3400_SECTORSIZE * FLASH3400_NUMSECTORS bytes here
// Another requirement is that this address will be permanent and will not vary in different compilations.
// This is because we need to pass it to mktfsimg (uMon TFS build utility, see the makefile)
// So, try do the following: 
//	1. After 1st compilation it is observed that &flash3400RamWin32 ~= 0x9df840
//  2. Try to align it to 0x?000000
//  3. The allocation size must huge: e.g. 6*16 Mb (0x6000000)
//  4. Suppose if we take address 0x3000000 - it will be inside our array
//  5. In order to be sure - call the validation macro from the Windows main()

#define FLASH3400RAMWIN32_SIZE	0x6000000
#define FLASH3400_RAM_BASE		0x3000000

// #define FLASH3400RAMWIN32_SIZE (FLASH3400_SECTORSIZE * FLASH3400_NUMSECTORS)
// #define FLASH3400_RAM_BASE ((unsigned)flash3400RamWin32)

extern int flash3400RamWin32 [FLASH3400RAMWIN32_SIZE / sizeof(int)];

#define VALIDATE_TFS()	\
			ASSERT (	\
					(FLASH3400_RAM_BASE >= ((unsigned)flash3400RamWin32))  && \
					(FLASH3400_RAM_BASE + FLASH3400_SECTORSIZE*FLASH3400_NUMSECTORS <= ((unsigned)flash3400RamWin32 + FLASH3400RAMWIN32_SIZE)) \
				)
#endif


// #define TFSSECTORCOUNT    	(FLASH3400_NUMSECTORS - 1)	// 1 - spare
// #define TFSSPARESIZE    		FLASH3400_SECTORSIZE
// #define TFSSPARE        		(TFSEND + 1)

#define TFSSECTORCOUNT    		FLASH3400_NUMSECTORS
#define TFSSPARESIZE    		0
#define TFSSPARE        		0

#define TFSSTART        		FLASH3400_RAM_BASE
#define TFSEND          		FLASH3400_RAM_BASE + TFSSECTORCOUNT * FLASH3400_SECTORSIZE - 1
#define TFS_EBIN_ELF    		1
#define TFS_VERBOSE_STARTUP		1


#if UMON_TARGET == UMON_TARGET_WIN
// To solve conflict of funcs defined twice
void my_puts(char *);
int	 my_printf(char *fmt, ...);
#define printf	my_printf
#define puts	my_puts
#endif


/*****************************************************************************\
							Umon used modules
\*****************************************************************************/

#define INCLUDE_MALLOC			1
#define INCLUDE_MEMCMDS         1
#define INCLUDE_SHELLVARS		1
#define INCLUDE_XMODEM          1
#define INCLUDE_DISASSEMBLER    0
#define INCLUDE_UNZIP           1
#define INCLUDE_ETHERNET        0
#define INCLUDE_ICMP			0
#define INCLUDE_IGMP			0
#define INCLUDE_TFTP            0
#define INCLUDE_TFS             1
#define INCLUDE_EDIT            0
#define INCLUDE_FLASH           1
#define INCLUDE_LINEEDIT        1
#define INCLUDE_DHCPBOOT        0
#define INCLUDE_TFSAPI          INCLUDE_TFS
#define INCLUDE_TFSSYMTBL       0
#define INCLUDE_TFSSCRIPT       INCLUDE_TFS
#define INCLUDE_TFSCLI          INCLUDE_TFS
#define INCLUDE_EE              1
#define	INCLUDE_GDB             0
#define	INCLUDE_SPIF			0
#define INCLUDE_STRACE          0
#define INCLUDE_CAST            0
#define INCLUDE_STRUCT          INCLUDE_TFS
#define INCLUDE_REDIRECT        INCLUDE_TFS
#define INCLUDE_QUICKMEMCPY     1
#define INCLUDE_PROFILER        0
#define INCLUDE_BBC             0
#define INCLUDE_MEMTRACE        0
#define INCLUDE_STOREMAC        0
#define INCLUDE_VERBOSEHELP     1
#define INCLUDE_HWTMR	 	    0
#define INCLUDE_PORTCMD	 	    0
#define INCLUDE_USRLVL	 	    0
#define INCLUDE_TIMEOFDAY	    0
#define INCLUDE_JFFS2			0
#define INCLUDE_JFFS2ZLIB		0
#define	INCLUDE_SYSLOG			0

#include "inc_check.h"


#endif // _CONFIG_H
