#include "cfg.h"
#include "monlib.h"

unsigned long AppStack[APPSTACKSIZE/4];

int
main(int argc,char *argv[])
{
	int	i;

	/* Use MicroMonitor's printf() for standard out...
	 */
	mon_printf("Hello embedded world!\n");

	/* The argument list is available just like a standard
	 * application...
	 */
	for(i=0;i<argc;i++)
		mon_printf("argv[%d] = %s\n",i,argv[i]);

	/* If memory trace is enabled, then this statement will be
	 * logged, and readable by the mtrace command...
	 */
	mon_memtrace("hey, this is a trace statement");

	/* The application can use previously established shell
	 * variables to configure its runtime...
	 */
	if (mon_getenv("ABC"))
		mon_printf("ABC = %s\n",mon_getenv("ABC"));

	return(0);
}

void
Cstart(void)
{
	extern	char __bss_start, _end;
	int		argc, ret;
	char	**argv, *ramstart, *ramend;

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
