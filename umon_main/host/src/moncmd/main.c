/** 
 *	\file main.c:
 *	\brief The front end to moncmd.c.  
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
 *	Author:	Ed Sutter
 *	email:	esutter@lucent.com
 *	phone:	908-582-2351
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#ifdef BUILD_WITH_VCC
#define usleep(n) Sleep(n/1000)
#define WIN32_LEAN_AND_MEAN
#include <winsock.h>
#else
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "moncmd.h"
#include "version.h"

/**
 *	\var errmsg
 *	\brief Initialized string table of usage message.
 */
char *errmsg[] = {
	"Usage: moncmd [options] {sysname} \"monitor command\"",
	" Options:",
	"  -i     interactive mode",
	"  -l#    loop till interrupt (#=msec delay between repeats)",
	"  -m     receive multiple responses",
	"  -p#    override default port number of 777",
	"  -w#    override default timeout of 10 seconds with '#' seconds",
	"  -q     quiet mode (don't print timeout message)",
	"  -r     if timeout waiting for command reception, retry (forever)",
	"  -v     verbose mode (print 'Sending...' message)",
	"  -V     show version of moncmd.exe.",
	" Exit status:",
	"  0 if successful",
	"  1 if timeout waiting for command completion",
	"  2 if error",
	"  3 if timeout waiting for command reception",
	0,
};

/** \fn void usage(char *error)
 *	\brief Called if a command line error was detected.
 *	\param error Optional error message.
 */
void
usage(char *error)
{
	int	i;

	if (error)
		fprintf(stderr,"ERROR: %s\n",error);
	for(i=0;errmsg[i];i++)
		fprintf(stderr,"%s\n",errmsg[i]);
#include "builtwith.c"
	exit(EXIT_ERROR);
}

/** \fn void err(char *msg)
 *	\brief Common error handling function.
 *	\param msg Optional error message.
  
 *	This function is called whenever an error is detected.  
 *	It simply prints a message and exits, with EXIT_ERROR.
 */
void
err(char *msg)
{
	if (msg)
		fprintf(stderr,"%s, ",msg);
	perror("");
	exit(EXIT_ERROR);
}

/** \fn int main(int argc, char **argv)
 *	\brief Starting point of moncmd
 *	\param argc Argument count
 *	\param argv Argument list
 *
 *	This function processes the incoming argument list and options
 *	and provides the user interface to the do_moncomd() function.
 */
int
main(int argc,char **argv)
{
	int		opt;		/** stores the return value from getopt() */
	int		loop;		/** variable containing the loop count (if any) */
	short	portnum;	/** contains UDP port to connect to */

	if (argc == 1)
		usage(0);

	loop = 0;
	moncmd_init(argv[0]);

	portnum = IPPORT_MONCMD;

	/* This is the getopt loop:
	 */
	while((opt=getopt(argc,argv,"il:mp:qrvVw:")) != EOF) { 
		switch(opt) {
		case 'i':
			interactiveMode = 1;
			break;
		case 'l':
			loop = (int)strtol(optarg,0,0);
			loop *= 1000;
			break;
		case 'm':
			multirespMode = 1;
			break;
		case 'p':
			portnum = atoi(optarg);
			break;
		case 'q':
			quietMode = 1;
			break;
		case 'r':
			retryFlag = 1;
			break;
		case 'V':
			showVersion();
			break;
		case 'v':
			verbose = 1;
			break;
		case 'w':
			fwaitTime = atof(optarg);
			break;
		default:
			usage("bad option");
		}
	}

	/* Convert wait time from float to some integral number
	 * of 100ths of a second...
	 */
	fwaitTime *= 100.0;
	waitTime = (int)fwaitTime;

	/* If interactive mode is enabled, then the command line doesn't
	 * require (or expect) a final command string argument...
	 */
	if (interactiveMode) {
		if (argc < optind+1)
			usage("bad arg count");
	}
	else {
		if (argc < optind+2)
			usage("bad arg count");
	}

	if ((waitTime == 0) && (retryFlag)) {
		fprintf(stderr,
			"Warning: -r option ignored with zero waittime\n"); 
		retryFlag = 0;
	}
	if (loop) {
		while(1) {
			do_moncmd(argv[optind],argv[optind+1],portnum);
			usleep(loop);
			moncmd_init(argv[0]);
		}
	}
	else
		exit(do_moncmd(argv[optind],argv[optind+1],portnum));
}
