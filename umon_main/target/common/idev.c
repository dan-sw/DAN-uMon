/* idev.c:
 *	Provide some interface to display/modify current device configuration.
 *
 *	General notice:
 *	This code is part of a boot-monitor package developed as a generic base
 *	platform for embedded system designs.  As such, it is likely to be
 *	distributed to various projects beyond the control of the original
 *	author.  Please notify the author of any enhancements made or bugs found
 *	so that all may benefit from the changes.  In addition, notification back
 *	to the author will allow the new user to pick up changes that may have
 *	been made by other users after this version of the code was distributed.
 *
 *	Note1: the majority of this code was edited with 4-space tabs.
 *	Note2: as more and more contributions are accepted, the term "author"
 *		   is becoming a mis-representation of credit.
 *
 *	Original author:	Ed Sutter
 *	Email:				esutter@lucent.com
 *	Phone:				908-582-2351
 */
#include "config.h"
#include "genlib.h"
#include "stddefs.h"
#include "devices.h"
#include "cli.h"

char *IdevHelp[] = {
    "Device interface",
#if INCLUDE_VERBOSEHELP
    "-[d:] [command] [arg(s)]",
    " -d {devid}  device id (default = console)",
	" Commands:",
	"  baud {baudrate}",
	"  console",
	" Note: not all commands apply to all devices",
#endif
    0,
};

int
Idev(int argc,char **argv)
{
    int	opt, devid;

    /* If no args, just dump current status. */
    if (argc == 1) {
		devtbldump();
		return(CMD_SUCCESS);
    }

	devid = ConsoleDevice;
	while ((opt=getopt(argc,argv,"d:")) != -1) {
		switch(opt) {
			case 'd':
				devid = atoi(optarg);
				break;
			default:
				return(CMD_PARAM_ERROR);
		}
	}

	if (isbaddev(devid))
		return(CMD_FAILURE);

	if (!strcmp(argv[optind],"console")) {
		ConsoleDevice = devid;
	}
	else if (!strcmp(argv[optind],"baud")) {
		ulong new_rate = atoi(argv[optind+1]);
		if (ioctl(devid,SETBAUD,new_rate,0) < 0) {
			printf("Command '%s' failed for %s\n",
				argv[optind],devtbl[devid].name);
			return(CMD_FAILURE);
		}					
		/* Success */
		if (devid == ConsoleDevice) {
			/* Update environment vars... */
			ConsoleBaudRate = new_rate;
			ConsoleBaudEnvSet();
		}
	}
	else
		return(CMD_PARAM_ERROR);

    return(CMD_SUCCESS);
}

