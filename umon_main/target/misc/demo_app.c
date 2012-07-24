/* demo_app.c:
 * This file is a simple example of an application that could be run
 * on top of the monitor.
 *
 * start():
 * The start() function depends on the setting of MONCOMPTR in config.h.
 * It demonstrates the use of monConnect and the first mon_XXX function
 * typically called by an application, mon_getargv().
 *
 * main():
 * The main() function demonstrates argument processing (thanks to
 * the call to mon_getargv() in start()), environment variables and
 * a simple use of TFS to dump the content of an ASCII file.
 */

#include "config.h"
#include "monlib.h"
#include "tfs.h"
#include "stddefs.h"

int
main(int argc,char *argv[])
{
	int		i, tfd;
	char	line[80], *fname;

	/* If argument count is greater than one, then dump out the
	 * set of CLI arguments...
	 */
	if (argc > 1) {
		mon_printf("Argument list...\n");
		for(i=0;i<argc;i++)
			mon_printf("  arg[%d]: %s\n",i,argv[i]);
	}

	/* If the shell variable "USE_THIS_FILE" exists, then use the
	 * content of that shell variable as a filename; else use "monrc"...
	 */
	fname = mon_getenv("USE_THIS_FILE");
	if (!fname)
		fname = "monrc";

	/* If the file exists, the assume it is ASCII and dump it
	 * line by line...
	 */
	if (mon_tfsstat(fname)) {
		mon_printf("Dumping content of '%s'...\n",fname);

		tfd = mon_tfsopen(fname,TFS_RDONLY,0);
		if (tfd >= 0) {
			while(mon_tfsgetline(tfd,line,sizeof(line)))
				mon_printf("%s",line);
			mon_tfsclose(tfd,0);
		}
		else {
			mon_printf("TFS error: %s\n",
				(char *)mon_tfsctrl(TFS_ERRMSG,tfd,0));
		}
	}
	return(0);
}

void
__eabi(void)
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

	/* Extract argc/argv from structure and call main():
	 */
	mon_getargv(&argc,&argv);

	/* Call main, then return to monitor.
	 */
	return(main(argc,argv));
}
