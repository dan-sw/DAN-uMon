#include "config.h"
#include "stddefs.h"
#include "genlib.h"

/* MonStack:
 * The monitor's stack is declared within the monitor's own .bss space.
 * This keeps the memory map simple, the only thing that needs to be
 * accounted for is that in the bss init loop, this section should not
 * be initialized.  MONSTACKSIZE must be defined in config.h.  
 */
ulong	MonStack[MONSTACKSIZE/4];

int
main(void)
{
	extern int _bss_start, _end;
    int *tmp;
    volatile ulong *bssptr;
	static	char cmdline[CMDLINESIZE];

    tmp = &_bss_start;
    bssptr = (ulong *)tmp;
    while(bssptr < MonStack)
        *bssptr++ = 0;
    bssptr = (ulong *)MonStack+(MONSTACKSIZE/4);
    tmp = &_end;
    while(bssptr < (ulong *)tmp)
        *bssptr++ = 0;

	ioInit();

	while(1) {
		puts("uuMON>");
		memset(cmdline,0,CMDLINESIZE);	/* Clear command line buffer */
		if (getline(cmdline,CMDLINESIZE) == 0)
			continue;
		docommand(cmdline);
	}
	return(0);
}
