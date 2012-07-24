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
