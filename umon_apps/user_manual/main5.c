#include "cfg.h"
#include "monlib.h"
#include <string.h>

#if CPU_IS_MIPS
#define EXCEPTION()		asm("syscall 99");	/* System call */
#elif CPU_IS_68K
#define EXCEPTION()		asm("trap #3");		/* Trap */
#elif CPU_IS_SH
#define EXCEPTION()		asm("trap #3");		/* Trap */
#elif CPU_IS_PPC
#define EXCEPTION()		asm("sc");			/* System call */
#elif CPU_IS_ARM
#define EXCEPTION()		asm("swi");			/* Software interrupt */
#else
#error: Must specify CPU type for exception demo.
#endif
 
int

unsigned long AppStack[APPSTACKSIZE/4];

int debug_enabled;

struct abc {
    long    l;
    short   s;
    char    c;
    char    x;
    char    *p; 
};

struct abc abc_s;

int value;

void
syserr(void)
{
    mon_printf("System Error!\n");
    EXCEPTION();
}

int
main(int argc,char *argv[])
{
    char line[64];

    if ((argc == 2) && (strcmp(argv[1],"debug") == 0))
        debug_enabled = 1;

    value = 0;
    abc_s.l = 0x12345678;
    abc_s.s = 0xBEEF;
    abc_s.c = 'Z';
    abc_s.x = 'Y';
    abc_s.p = "hi mom!";

    while(1) {
        mon_printf("MYCLI:");
        if (mon_getline(line,sizeof(line),1) > 0) {
            if (strcmp(line,"exit") == 0) {
                break;
            }
            else if (strcmp(line,"hi") == 0) {
                mon_printf("Hello\n");
                value ++;
            }
            else if (strcmp(line,"err") == 0) {
                syserr();
            }
            else {
                mon_memtrace("Pass '%s' to monitor",line);
                mon_docommand(line,0);
            }
        }
    }
    mon_malloc(100);
    mon_memtrace("All done!");
    return(0);
}

int
testfunc(int arg)
{
    mon_printf("arg=%d\n",arg);
    return(arg+1);
}

void
Cstart(void)
{
	extern char __bss_start, _end;
	char *ramstart, *ramend;
	int		argc, ret;
	char	**argv;

	/* Initialize application-owned BSS space.
	 * If this application is launched by TFS, then TFS does
	 * it automatically, however since MicroMonitor provides
	 * other alternatives for launching an application, we
	 * clear bss here anyway (just in case TFS is not launching
	 * the app)...
	 */
	ramstart = &__bss_start;
	ramend = &_end;
	while(ramstart < ramend)
		*ramstart++ = 0;

	monConnect((int(*)())(*(unsigned long *)MONCOMPTR),(void *)0,(void *)0);

	/* Extract argc/argv from structure and call main(): */
	mon_getargv(&argc, &argv);

	/* Call main, then return to monitor. */
	ret = main(argc, argv);

	/* Since we established a stack frame we can't just return directly,
	 * we must return through mon_appexit()...
	 */
	mon_appexit(ret);
}
