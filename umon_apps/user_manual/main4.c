#include "cfg.h"
#include "string.h"
#include "monlib.h"

unsigned long AppStack[APPSTACKSIZE/4];

int
printfile(char *filename)
{
    int tfd;
    char c;

    tfd = mon_tfsopen(filename,TFS_RDONLY,0);
    if (tfd < 0) {
        mon_printf("%s: %s\n",filename,(char *)mon_tfsctrl(TFS_ERRMSG,tfd,0));
        return(-1);
    }
    mon_printf("The content of '%s' follows:\n",filename);
    while(mon_tfsread(tfd,&c,1) == 1) {
        if (c == '\n')
            mon_putchar('\r');
        mon_putchar(c);
    }

    mon_tfsclose(tfd,0);
    return(0);
}

int
newfile(char *filename)
{
    int err;
    char *filedata = "This is a file\nwith several lines\nthat can be read\n";

    err = mon_tfsadd(filename,0,0,filedata,strlen(filedata));
    if (err != TFS_OKAY) {
        mon_printf("mon_tfsadd(%s) failed: %s\n",filename,
            (char *)mon_tfsctrl(TFS_ERRMSG,err,0));
    }
    return(err);
}

int
listfiles(void)
{
    int tot;
    TFILE *tfshdr;

    tot = 0;
    tfshdr = (TFILE *)0;
    while((tfshdr = mon_tfsnext(tfshdr)) != 0)
        mon_printf("%2d: %s\n",++tot,TFS_NAME(tfshdr));
    
    return(tot);
}

int
main(int argc,char *argv[])
{
    if (argc != 2) {
        mon_printf("Must specify filename\n");
        mon_appexit(1);
    }

    printfile(argv[1]);
    newfile("newfile");
    listfiles();

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
