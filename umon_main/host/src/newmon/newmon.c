/* newmon.c:
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
#include <ctype.h>
#ifdef BUILD_WITH_VCC
#include <winsock.h>
#else
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "ttftp.h"
#include "moncmd.h"
#include "version.h"
#include "crc32.h"

#ifndef O_BINARY
#define O_BINARY 0
#endif

#define WHATSTRING	"@(#)"
#define WHATSEARCH	1
#define WHATMATCH	2


char *errmsg[] = {
	"Usage: newmon [options] {targetip} {binfile}",
	" Options:",
	"  -A#    override default use of $APPRAMBASE",
	"  -B#    override default use of $BOOTROMBASE",
	"  -b#    override default flash bank 0",
	"  -P     don't run PLATFORM test",
	"  -p#    override default port number of 777",
	"  -u     issue 'flash unlock' prior to 'ewrite'",
	"  -v     verbose mode (print 'Sending...' message)",
	"  -V     show version of newmon.exe.",
	"  -w#    override default timeout of 10 seconds with '#' seconds",
	" Exit status:",
	"  0 if successful",
	"  1 if timeout waiting for command completion",
	"  2 if error",
	"  3 if timeout waiting for command reception",
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

void
testTftp(int i,char *cp,int j)
{
}

/* platformcheck():
 * Added Aug 2009.
 * This function attempts to compare the WHAT string for PLATFORM
 * in the image with the PLATFORM shell variable on the target.
 * If the WHAT-string doesn't exist or the comparison fails return
 * -1; else return 0.
 */
int
platformcheck(char *targetip, short portnum, char *binfile, int verbose)
{
	char	*eol;
	char	platform_shellvar[128];
	int		ifd, state, matchidx;
	char	*buf, *end, *bp, *wsp, *whatstring, whatmatch[128];
	struct	stat	mstat;

	/* Retrieve the PLATFORM shell variable from the running target...
	 */
	moncmd_buffer_last_line(platform_shellvar, sizeof(platform_shellvar));
	do_moncmd(targetip, "echo $PLATFORM", portnum);

	eol = strpbrk(platform_shellvar,"\r\n");
	if (eol) *eol = 0;

	if (verbose)
		printf("PLATFORM var: <%s>\n",platform_shellvar);

	/* Open input file: */
	ifd = open(binfile,O_RDONLY | O_BINARY);
	if (ifd == -1) {
		perror(binfile);
		return(-1);
	}
	fstat(ifd,&mstat);
	bp = buf = malloc(mstat.st_size);
	if (!buf) {
		perror("malloc failed");
		close(ifd);
		return(-1);
	}
	if (read(ifd,buf,mstat.st_size) != mstat.st_size) {
		perror(binfile);
		free(buf);
		close(ifd);
		return(-1);
	}
	close(ifd);
	end = buf + mstat.st_size;

	whatstring = WHATSTRING;
	state = WHATSEARCH;
	wsp = whatstring;
	matchidx = 0;
	while(bp < end) {
		switch(state) {
		case WHATSEARCH:
			if (*bp == *wsp) {
				wsp++;
				if (*wsp == 0) {
					wsp = whatstring;
					state = WHATMATCH;
				}
			}
			else
				wsp = whatstring;
			break;
		case WHATMATCH:
			if (isprint(*bp)) {
				whatmatch[matchidx++] = *bp;
			}
			else {
				whatmatch[matchidx] = 0;
				if (strncmp(whatmatch,"PLATFORM=",9) == 0) {
					if (verbose)
						printf("PLATFORM what: <%s>\n",whatmatch+9);
					if (strcmp(whatmatch+9,platform_shellvar)) {
						free(buf);
						fprintf(stderr,
							"PLATFORM test failed: (%s != %s)\n",
							platform_shellvar,whatmatch+9);
						return(-1);
					}
					else {
						free(buf);
						return(0);
					}
				}
				matchidx = 0;
				state = WHATSEARCH;
			}
			break;
		}
		bp++;
	}

	/* If we got here, then we didn't find a 'What' string that starts
	 * with "PLATFORM=", so we return failure...
	 */
	free(buf);
	fprintf(stderr,
		"PLATFORM test failed: PLATFORM string not found in '%s'\n",binfile);
	return(-1);
}

int
main(int argc,char *argv[])
{
	int		opt, flashbank, ret, unlock, runplatformtest;
	char	*AppRamBase, *BootRomBase, *binfile, *targetip;
	char	cmd[128];
	short	portnum;
	struct	stat	mstat;

	if (argc == 1)
		usage(0);

	AppRamBase = "$APPRAMBASE";
	BootRomBase = "$BOOTROMBASE";
	flashbank = -1;
	runplatformtest = 1;
	unlock = tftpVerbose = 0;
	moncmd_init(argv[0]);
	ttftp_init();
	portnum = IPPORT_MONCMD;

	/* newmon will always generate a timeout error message, so we
	 * stifle that by forcing quietMode.  If -w is set below, then
	 * we assume that the user wants to see the error, so quietMode
	 * is cleared in that case.
	 */
	quietMode = 1;

	while((opt=getopt(argc,argv,"A:B:b:Pp:uw:vV")) != EOF) {
		switch(opt) {
		case 'A':
			AppRamBase = optarg;
			break;
		case 'B':
			BootRomBase = optarg;
			break;
		case 'b':
			flashbank = atoi(optarg);
			break;
		case 'P':
			runplatformtest = 0;
			break;
		case 'p':
			portnum = atoi(optarg);
			break;
		case 'u':
			unlock = 1;
			break;
		case 'v':
			if (verbose == 1)
				tftpVerbose = 1;
			verbose = 1;
			break;
		case 'w':
			fwaitTime = atof(optarg);
			quietMode = 0;		/* see comment above */
			break;
		case 'V':
			showVersion();
			break;
		default:
			usage("bad option");
		}
	}

	if (optind+2 != argc)
		usage(0);

	targetip = argv[optind];
	binfile = argv[optind+1];

	if (stat(binfile,&mstat) == -1) {
		perror(binfile);
		exit(EXIT_ERROR);
	}

	/* Convert wait time from float to some integral number
	 * of 100ths of a second...
	 */
	fwaitTime *= 100.0;
	waitTime = (int)fwaitTime;

	/* If do_moncmd() returns, then it succeeded... */
	do_moncmd(targetip, "version", portnum);

	/* Issue "echo $ETHERADD" so that it can be reloaded when the
	 * target restarts (this is useful if the target's uMon was
	 * built with INCLUDE_STOREMAC).
	 */
	do_moncmd(targetip, "echo $ETHERADD", portnum);

	if (fcrc32(binfile,0,1) < 0)
		exit(EXIT_ERROR);

	if (runplatformtest) {
		if (platformcheck(targetip, portnum, binfile, verbose) < 0)
			exit(EXIT_ERROR);
	}

	ret = ttftp(targetip,"put",binfile,AppRamBase,"octet");
	if (ret != EXIT_SUCCESS) {
		fprintf(stderr,"TFTP failed\n");
		exit(EXIT_ERROR);
	}

	if (flashbank != -1) {
		sprintf(cmd,"flash bank %d",flashbank);
		do_moncmd(targetip, cmd, portnum);
	}

	if (unlock)
		do_moncmd(targetip, "flash unlock", portnum);

	do_moncmd(targetip, "flash opw", portnum);

	sprintf(cmd,"flash ewrite %s %s %d",
		BootRomBase,AppRamBase,mstat.st_size);

	waitTime = 0;
	do_moncmd(targetip, cmd, portnum);

	printf("The 'flash ewrite' command will take ~30 seconds to complete\n");
	exit(EXIT_SUCCESS);
}


