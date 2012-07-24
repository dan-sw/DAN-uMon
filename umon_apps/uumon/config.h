#ifndef __ADSPLPBLACKFIN__
#define __ADSPLPBLACKFIN__
#endif

#define CPU_LE
#define SYSCLKFREQ	100000000


/* MONSTACKSIZE:
 * The amount of space allocated to the monitor's stack.
 */
#define MONSTACKSIZE	(1*1024)

#define APPLICATION_RAMSTART	0x100000

#define CMDLINESIZE			80

/* Basic set of commands...
 */
#define INCLUDE_DM			1
#define INCLUDE_PM			1
#define INCLUDE_MT			1
#define INCLUDE_XMODEM		1
#define INCLUDE_CALL		1
#define INCLUDE_HELP		1

/* Extensions for flash and network access...
 */
#define INCLUDE_HWTMR		0
#define INCLUDE_FLASH		1
#define INCLUDE_FLASHINFO	1
#define FLASHBANKS			1
#define FLASH_TIMEOUT		100000
#define INCLUDE_ENETCORE	1
#define INCLUDE_BOOTP		INCLUDE_ENETCORE
#define INCLUDE_TFTP		INCLUDE_ENETCORE
#define INCLUDE_PHY			0

#if INCLUDE_FLASH
#define SINGLE_FLASH_DEVICE	
#define FLASH_BANK0_BASE_ADDR   0x20000000
#define FLASH_BANK0_SIZE        0x00400000
#define FLASH_PROTECT_RANGE     "0-1"
#define FLASH_BANK0_WIDTH       2
#endif

#if INCLUDE_HWTMR
#define TIMER_TICKS_PER_MSEC 500000
#else
#define LOOPS_PER_MILLISECOND	250
#endif


#define WATCHDOG_MACRO						\
{											\
 /* watchdog handler code here (if any) */	\
}

#define RESETFUNC()			\
{							\
    *((volatile unsigned short *)0xFFC00200) = 0x0ad0;	\
    *((volatile unsigned short *)0xFFC00200) = 0x0004;	\
    *((volatile unsigned short *)0xFFC00200) = 0x0000;	\
    while(1); 				\
}
