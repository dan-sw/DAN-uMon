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
/**********************************************************************\
 Library    :  uMON (Micro Monitor)
 Filename   :  startup.c
 Purpose    :  Client's C-entry point
 Owner		:  Sergey Krasnitsky
 Created    :  30.10.2010

 Note :
	This file intended to be instead of startup object module, usually 
	written on assembler with several required by Umon mandatory global 
	definitions. Also, the main() function is implemented here.
\**********************************************************************/

#include "config.h"
#include "stddefs.h"
#include "genlib.h"
#include "warmstart.h"
#include "cpu.h"
#include "CGEN.h"
#include "common.h"
#include "spi.h"
#include "ddr.h"
#include "cli.h"
#include "tfs.h"
#include "tfsprivate.h"
#include "flash.h"
#include "flash3400.h"

// Forward declaration uMON's functions not declared in API

int addcommand (struct monCommand *cmdlist, char *cmdlvl);

// We will define the function prototype type needed by monConnect()
typedef int	(*MONCOMPTR)(int, void *, void *, void *);

// This function is the real Umon entry point, and it is implemented in
// umon_main\target\common\start.c
extern void start(int);

extern void reboot(void);
extern void cache_off_asm(void);
extern void CPU_cache_on(uint32 TTB_addr);

// Global var used by Umon
extern int moncom(int cmd, void *arg1, void *arg2, void *arg3);
MONCOMPTR  moncomptr = moncom;


#define TTB_ADDR_DEFAULT	(0x20004000)

void warmstart(int val)
{
#ifdef DAN2400
	reboot();
#else
    // Master Reset
    RegWrite32(MASTER_RESET_ADDR, MASTER_RESET_VAL);
#endif
}
int cache(int argc, char *argv[])
{
	static uint8 cache_state = 1;
	if (argc > 1 && strcmp("on", argv[1]) == 0)
	{
		uint32 TTB_addr = TTB_ADDR_DEFAULT;
		if (argc > 2)
		{
			TTB_addr = atoi(argv[2]);
		}
		printf("Switching MMU on, TTB = 0x%08x ... ", TTB_addr);
		cache_state = 0;
		CPU_cache_on(TTB_addr);
		cache_state = 2;
		printf("Ok\n");
	}
	else if (argc > 1 && strcmp("off", argv[1]) == 0)
	{
		puts("Switching caches off");
		cache_off_asm();
		cache_state = 0;
	}
	else
	{
		printf("Cache state is : %s\n",
			cache_state ? ((cache_state == 1) ? "ICache on" : "on") : "off");
	}
	return 0;
}

char *cache_help[] = {
    "Switch on/off the caches",
    "on|off",
    0,
};

void userinit(void)
{
    static struct monCommand mycmds[] = {     // App-specific command table
        { "cache", cache, cache_help, 0 },
        { 0,0,0,0 }
    };

    addcommand (mycmds, 0);
}

int umon_main(int argc,char *argv[])
{
//	SOC_arm_init();
	CPU_Init();
//	CPU_cache_on(TTB_ADDR_DEFAULT);
	SPI_init();
	start(INITIALIZE);
	return 0;
}

