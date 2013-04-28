// Copyright 2013, Qualcomm Atheros, Inc.
/*
All files except if stated otherwise in the begining of the file are under the GPLv2 license:
-----------------------------------------------------------------------------------

Copyright (c) 2010-2012 Design Art Networks Ltd.

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
#include "cli.h"
#include "tfs.h"
#include "tfsprivate.h"
#include "flash.h"
#include "flash3400.h"
#include "trc.h"


// We will define the function prototype type needed by monConnect()
typedef int	(*MONCOMPTR)(int,void *, void *, void *);

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
	TRCINIT();
	SPI_init();
	start(INITIALIZE);
	return 0;
}
