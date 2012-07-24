/* main.c:
	Front end to ttftp.
	See ttftp.c for details.

	General notice:
	This code is part of a boot-monitor package developed as a generic base
	platform for embedded system designs.  As such, it is likely to be
	distributed to various projects beyond the control of the original
	author.  Please notify the author of any enhancements made or bugs found
	so that all may benefit from the changes.  In addition, notification back
	to the author will allow the new user to pick up changes that may have
	been made by other users after this version of the code was distributed.

	Author:	Ed Sutter
	email:	esutter@lucent.com		(home: lesutter@worldnet.att.net)
	phone:	908-582-2351			(home: 908-889-5161)
*/
#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#ifdef BUILD_WITH_VCC
#include <io.h>
#include <winsock2.h>
#else
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "ttftp.h"
#include "version.h"

char *errmsg[] = {
	"Usage:",
	" ttftp [options] {sysname} {get|put} {source} [destination]",
	" ttftp {srvr}",
	"",
	"Options:",
	" -a     use netascii mode for 'put'; default is octet (binary)",
	" -d#    insert a per-packet-delay (milli-seconds) for client packets sent",
	" -p#    override default port number of 69",
	" -q     suppress dots, then ticker (additive visual noise reduction)",
	" -r rfc2349_tsize_off disable 'tsize' extension (see RFC2349)",
	" -t ##  inactivity timeout used by server (default is 60 seconds)",
	" -T {Test_Setup_String} see below",
	" -v     verbose mode, display details of each TFTP packet",
	" -V     show version of ttftp.exe.",
	" -x     automatically exit after srvr receives a file",
	"",
	" Test Setup String:",
	"  -T supports the ability to inject delays, early termination and",
	"  corruption into the stream of data fed to the client.",
	"  Syntax...",
	"  -T OPCODE,OPCODE_NUM,TESTTYPE,ARG1,ARG2",
	"  Where...",
	"   OPCODE is RRQ, WRQ, DAT, ACK or ERR;",
	"   OPCODE_NUM specifies the 'nth' repetition of that opcode",
	"   TESTTYPE is SLEEP, QUIT, BBNO or CORRUPT",
	"   ARG1 is SLEEPTIME, NA or CORRUPTBYTE",
	"   ARG2 is NA for now",
	"  Examples...",
	"  -T ACK,3,SLEEP,5",
	"   On the third ACK received, sleep for 5 seconds, then continue;",
	"  -T ACK,5,QUIT",
	"   On the fifth ACK received, terminate;"
	"\n",
	0,
};

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

#ifdef BUILD_WITH_VCC
/* Ctrl-c handler... */
BOOL
ConsCtrlHdlr(DWORD ctrltype)
{
	SetConsoleTitle(consoleTitle);
	return(FALSE);
}
#endif


int
main(int argc,char *argv[])
{
	int		opt;
	char	*mode, *destfile;

	if (argc == 1)
		usage(0);

	ttftp_init();
	mode = "octet";
	while((opt=getopt(argc,argv,"ad:p:qr:T:t:vVx")) != EOF) { 
		switch(opt) {
		case 'a':
			mode = "netascii";
			break;
		case 'd':
			tftpPpd = atoi(optarg);
			break;
		case 'p':
			tftpPort = atoi(optarg);
			break;
		case 'q':
			tftpQuiet++;
			break;
		case 'r':
			if (strcmp(optarg,"rfc2349_tsize_off") == 0)
				tftpRFC2349TsizeEnabled = 0;
			else
				usage("bad -r value");
			break;
		case 't':
			tftpSrvrTimeout = atoi(optarg);
			break;
		case 'T':
			if (testSetup(optarg) == -1)
				exit(1);
			break;
		case 'V':
			showVersion();
			break;
		case 'x':
			tftpExitAfterRcv = 1;
			break;
		case 'v':
			tftpVerbose = 1;
			break;
		default:
			usage("bad option");
		}
	}

	if ((argc == optind+1) && (!strcmp(argv[optind],"srvr"))) {
		int		srvrexit;

#ifdef BUILD_WITH_VCC
		// Establish a ctrl-c handler...
		if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsCtrlHdlr,TRUE) != TRUE)
			perror("Can't establish ctrl-c handler");

		// Save the current console title.
		GetConsoleTitle((LPTSTR)consoleTitle,(DWORD)sizeof(consoleTitle));
#endif
		srvrexit = tftpsrvr(tftpVerbose);
#ifdef BUILD_WITH_VCC
		// Restore the console title.
		SetConsoleTitle(consoleTitle);
#endif
		exit(srvrexit);
	}

	if (tftpExitAfterRcv)
		fprintf(stderr,"WARNING: -x option ignored (only valid with srvr)\n");

	if (argc < optind + 3)
		usage("bad arg count");

	if (argc == optind+4) 
		destfile = argv[optind+3];
	else
		destfile = argv[optind+2];

	return(ttftp(argv[optind],argv[optind+1],argv[optind+2],destfile,mode));
}
