#include "monlib.h"

int
main(int argc, char *argv[])
{
	mon_printf("Hello embedded world!\n");
	return(0);
}

int
start(void)
{
	int	argc;
	char	**argv;

	monConnect((int(*)())(*(unsigned long *)MONCOMPTR),(void *)0,(void *)0);

	/* Extract argc/argv from structure and call main(): */
	mon_getargv(&argc, &argv);

	/* Call main, then return to monitor. */
	return(main(argc, argv));
}
