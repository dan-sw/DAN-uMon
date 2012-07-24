#include "cfg.h"
#include "string.h"
#include "monlib.h"

unsigned long AppStack[APPSTACKSIZE/4];

char *mycmdHelp[] = {
    "really doesn't do anything",
    "[echo string]",
    0,
};

int
mycmdFunc(int argc,char *argv[])
{
    int  i;

    if (argc == 1) {
        mon_printf("Error: missing arguments\n");
        return(CMD_FAILURE);
    }
    else {
        for(i=1;i<argc;i++)
            mon_printf("Arg %d is '%s'\n",i,argv[i]);
    }
    return(CMD_SUCCESS);
}

struct monCommand mycmdTbl[] = {
    { "mycmd",  mycmdFunc,  mycmdHelp, 0 },
    { 0,0,0,0 }
};

char mycmdUlvltbl[] = { 0 };

int
main(int argc,char *argv[])
{
    char line[80];

    mon_addcommand(mycmdTbl,mycmdUlvltbl);

    while(1) {
        mon_printf("MYCLI:");
        if (mon_getline(line,sizeof(line),1) > 0)
            if (strcmp(line,"exit") == 0)
                break;
            mon_docommand(line,0);
        
    }
    mon_printf("Returning control to MicroMonitor...\n");
    return(0);
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
