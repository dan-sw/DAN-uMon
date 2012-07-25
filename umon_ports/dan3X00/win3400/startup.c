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
 Purpose    :  Client's C-entry point (Windows simulator)
 Owner		:  Sergey Krasnitsky
 Created    :  30.10.2010
\**********************************************************************/

#include <stdio.h>
#include "config.h"
#include "stddefs.h"
#include "genlib.h"
#include "warmstart.h"


void load_jffs2()
{
	static uint8 buftmp[196608];
	char s[30];

	FILE * h = fopen ("c:\\TFTP_home\\apps2.jffs2", "rb");

    if (!h) {
        //puts ("tfs.dump not found");
		return;
    }

	fread((void*)buftmp, 1, 196608, h);
	fclose(h);
    //printf ("TFS was restored from the dump (%d bytes)\n", nbytes);

	snprintf(s, 30, "0x%X", (void*)buftmp);
	setenv("JFFS2BASE", s);
	setenv("JFFS2SIZE", "188956");
}


#if UMON_TARGET == UMON_TARGET_WIN
// Define dummy BSS for Windows simulation only
int _bss[1024];
int _bss_start = (int) &_bss[0];
int _bss_end   = (int) &_bss[1023];

int apprambase_win [0x100000];
#endif


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
}

void reginit(void)
{
}

void userinit(void)
{
}

int main(int argc,char *argv[])
{
	VALIDATE_TFS();																		// Validate our FLASH3400_RAM_BASE
	memset ((void*)FLASH3400_RAM_BASE, 0, FLASH3400_SECTORSIZE*FLASH3400_NUMSECTORS);	// it's also for validation purpose

	start(INITIALIZE);
	return 0;
}
