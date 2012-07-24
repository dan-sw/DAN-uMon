#include "monlib.h"
#include "cfg.h"

unsigned long AppStack[APPSTACKSIZE/4];
extern int main(int, char **);

void
__gccmain()
{
}

int
Cstart(void)
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

	/* Call main, then exit to monitor.
	 */
	main(argc,argv);

	mon_appexit(0);

	/* Won't get here. */
	return(0);
}
