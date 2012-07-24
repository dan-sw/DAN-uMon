/* 
 * This file is a simple example of an application that could be run
 * on top of the monitor.
 *
 * Cstart():
 * The Cstart() function depends on the setting of MONCOMPTR in config.h.
 * It demonstrates the use of monConnect and the first mon_XXX function
 * typically called by an application, mon_getargv().
 *
 * main():
 * The main() function demonstrates argument processing (thanks to
 * the call to mon_getargv() in start()), environment variables and
 * a simple use of TFS to dump the content of an ASCII file.
 */

#include <string.h>
#include <stdlib.h>
#include "monlib.h"
#include "tfs.h"
#include "cfg.h"
#include "picoc.h"

unsigned long AppStack[APPSTACKSIZE/4];

int
main(int argc, char **argv)
{
	if (argc < 2) {
		mon_printf("Format: picoc <program.c> - run a program\n        picoc -i          - interactive mode\n");
		mon_appexit(1);
    }
    
	Initialise();
    
	if (strcmp(argv[1], "-i") == 0)
        ParseInteractive();
    else
        PlatformScanFile(argv[1]);
    
    Cleanup();
    return 0;
}

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
