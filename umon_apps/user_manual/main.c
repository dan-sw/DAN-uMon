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
 * Also, if the first argument is "strace_demo", then the strace_demo()
 * function is called.  Refer to strace.c for details.
 */

#include <string.h>
#include <stdlib.h>
#include "monlib.h"
#include "tfs.h"
#include "cfg.h"

unsigned long AppStack[APPSTACKSIZE/4];

extern void strace_demo(void);

int
main(int argc,char *argv[])
{
	int		i, tfd;
	char	line[80], *ab, *filename;

	/* If argument count is greater than one, then dump out the
	 * set of CLI arguments...
	 */
	if (argc > 1) {
		if ((argc == 2) && (strcmp(argv[1],"strace_demo") == 0))
			strace_demo();
		
		mon_printf("Argument list...\n");
		for(i=0;i<argc;i++) {
			mon_printf("  arg[%d]: %s\n",i,argv[i]);
			if (strcmp(argv[i],"match") == 0)
				mon_printf("got a match!\n");
		}
	}

	/* Using the content of the shell variable APPRAMBASE, dump the
	 * memory starting at that location...
	 */
	ab = mon_getenv("APPRAMBASE");
	mon_printf("ab: 0x%lx\n",(long)ab);
	if (ab) {
		char *addr = (char *)strtol(ab,0,0);

		mon_printf("Dumping memory at 0x%lx...\n",addr);
		mon_printmem(addr,128,1);
	}

	filename = "monrc";

	/* If the 'monrc' file exists, the assume it is ASCII and dump it
	 * line by line...
	 */
	if (mon_tfsstat(filename)) {
		mon_printf("Dumping content of '%s'...\n",filename);

		tfd = mon_tfsopen(filename,TFS_RDONLY,0);
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
	else {
		mon_printf("File '%s' not found\n",filename);
	}
	return(0);
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

/* CstartAlt():
 * Demonstrates the use of the "call -a" command in uMon. 
 * For example, if for some reason you wanted to do this...
 * Load the application then load the symtbl file using
 * "make TARGET_IP=1.2.3.4 sym", then issue these commands:
 *
 *  tfs -v ld app
 *  call -a %CstartAlt one two three
 *
 * The "call -a" command in uMon correctly sets up the function
 * call parameters so that the following function would see 4
 * arguments (including arg0), with argv[1] thru argv[3] being
 * pointers to each of the number strings (i.e. "one", "two", "three")
 * and argv[0] being the ascii-coded-hex address of the function
 * CstartAlt.
 */
int
CstartAlt(int argc, char *argv[])
{
	monConnect((int(*)())(*(unsigned long *)MONCOMPTR),(void *)0,(void *)0);
	main(argc,argv);
	mon_appexit(0);
	return(0);
}
