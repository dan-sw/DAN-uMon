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
#include "cli.h"
#include "tfs.h"
#include "tfsprivate.h"
#include "flash.h"
#include "flash3400.h"

// Forward declaration uMON's functions not declared in API

int addcommand (struct monCommand *cmdlist, char *cmdlvl);


// For xthal_set_region_attribute()
// #include <xtensa/config/core.h>

// We will define the function prototype type needed by monConnect()
typedef int	(*MONCOMPTR)(int,void *, void *, void *);
//int ExceptionType;
//unsigned long ExceptionAddr;

// This function is the real Umon entry point, and it is implemented in
// umon_main\target\common\start.c
extern void start(int);

// Global var used by Umon
extern int moncom(int cmd, void *arg1, void *arg2, void *arg3);
MONCOMPTR  moncomptr = moncom;
void warmstart(int val)
{
    // Master Reset
    RegWrite32(MASTER_RESET_ADDR, MASTER_RESET_VAL);
}

// K.S.
// The reginit() is strange function required by Umon: there are no description about it.
// Probably it was added in the recent Umon versions.
// There are only some comments in Umon modules, saying:
// "Init register data structures", "Clear the register cache"
// It has an implementation in umon_main\target\common\reg_cache.c,
// but this module is not part of recommended inclusions.
void reginit(void)
{
}

void userinit(void)
{
}

int main(int argc,char *argv[])
{
	CPU_Init();
	SPI_init();
	start(INITIALIZE);
	return 0;
}

