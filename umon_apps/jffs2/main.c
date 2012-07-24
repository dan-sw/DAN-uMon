/* 
 * This application uses uMon's jffs2 command code to create a standalone
 * application that can be installed into any uMon based target and used
 * similar to the built-in jffs2 (for targets that don't already have
 * the jffs2 command built in).
 */

#include <stdarg.h>

#define SHOWVARARGS 1

#include "monlib.h"

#define printf mon_printf
#define tfsadd mon_tfsadd
#define tfsstat mon_tfsstat
#define malloc mon_malloc
#define free mon_free
#define gotachar mon_gotachar
#define getenv mon_getenv
#define setenv mon_setenv

#include "../../umon_main/target/common/jffs2.c"

extern char *Jffs2Help[];

int
main(int argc,char *argv[])
{
	int i;

	if ((argc == 2) && (!strcmp(argv[1],"help") || !strcmp(argv[1],"?"))) {
		for(i=0;Jffs2Help[i];i++)
			printf("%s\n",Jffs2Help[i]);
		return(0);
	}
		
	Jffs2Cmd(argc,argv);
	return(0);
}

int
putchar(char c)
{
	return(mon_putchar(c));
}

void
puts(char *s)
{
	while(*s)
		mon_putchar(*s++);
}

char *
tfserrmsg(int err)
{
	return((char *)mon_tfsctrl(TFS_ERRMSG,err,0));
}

unsigned long
getAppRamStart(void)
{
	char	*apprambase;
	unsigned long	value;

	apprambase = mon_getenv("APPRAMBASE");
	if (apprambase)
		value = strtoul(apprambase,0,0);
	else {
		mon_printf("JFFS2 requires APPRAMBASE\n");
		mon_appexit(1);
		value = 0;	/* won't get here */
	}
	return(value);
}

/* shell_sprintf():
 *	pulled in from ../../umon_main/target/common/env.c.
 */
int
shell_sprintf(char *varname, char *fmt, ...)
{
	int	tot;
	char buf[CMDLINESIZE];
	va_list argp;

	va_start(argp,fmt);
	tot = vsnprintf(buf,CMDLINESIZE-1,fmt,argp);
	va_end(argp);
	mon_setenv(varname,buf);
	return(tot);
}

void
__gccmain()
{
}

int
start(void)
{
	char	**argv;
	int		argc;

	/* Connect the application to the monitor.  This must be done
	 * prior to the application making any other attempts to use the
	 * "mon_" functions provided by the monitor.
	 */
	monConnect((int(*)())(*(unsigned long *)MONCOMPTR),(void *)0,(void *)0);

	/* When the monitor starts up an application, it stores the argument
	 * list internally.  The call to mon_getargv() retrieves the arg list
	 * for use by this application...
	 */
	mon_getargv(&argc,&argv);

	getoptinit();

	/* Call main, then return to monitor.
	 */
	main(argc,argv);

	return(0);
}
