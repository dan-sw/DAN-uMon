/** \file moncmd.c:
 *	\brief The guts of the moncmd UDP client.
 *	\author Ed Sutter esutter@lucent.com
 *
 *	This tool is used to support simple UDP communication between a host and
 *	a target running MicroMonitor.  The target is listening on port 
 *	IPPORT_MONCMD (777) for incoming command strings just as they would be
 *	seen if typed in at the RS-232 console port.  When the monitor receives
 *	this, it processes the string through the same mechanism as the console.
 *	The only addition is that all output is copied to the sender of the UDP
 *	packet (one line at a time).  Once the command has been processed, the
 *	final packet returned to the sender is a packet of size 1 with the data
 *	being zero.  This is what tells moncmd that the command has completed
 *	on the target.
 *
 *	\attention
 *	This code is part of a boot-monitor package developed as a generic base
 *	platform for embedded system designs.  As such, it is likely to be
 *	distributed to various projects beyond the control of the original
 *	author.  Please notify the author of any enhancements made or bugs found
 *	so that all may benefit from the changes.  In addition, notification back
 *	to the author will allow the new user to pick up changes that may have
 *	been made by other users after this version of the code was distributed.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#ifdef BUILD_WITH_VCC
#include <windows.h>
#include <winsock2.h>
#define uint32_t unsigned long
#else
#include <stdint.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "moncmd.h"

#ifdef BUILD_WITH_VCC
#define sleep(n) Sleep(n*1000)
#define SOCKET_INVALID(s)	(s == INVALID_SOCKET)
#else
#define SOCKET_INVALID(s)	(s < 0)
#endif

typedef unsigned char uchar;

extern	void err(char *);

/** \var char *thisProgname
 *	\brief String containing the value of argv[0] when the program was
 *	\brief executed.
 */
char *thisProgname;
				
/**	\var char *msgString
 *	\brief Pointer to the message that this program is 
 *	\brief to send to the target host.
 */
char *msgString;

/**	\var char *targetHost
 *	\brief Ip address or DNS name of host that this
 *	\brief program is to communicate with.
 */
char *targetHost;

/**	\var int retryFlag
 *	\brief If set, then the program will retry.
 */
int	retryFlag;


/**	\var int retryCount
 *	\brief Running total number of retries.
 */
int	retryCount;

/**	\var int quietMode
 *	\brief If set, then don't output status message at
 *	\brief the end of the program.
 */
int	quietMode;

/**	\var int	interactiveMode
 *	\brief If set, then moncmd runs similar to netcat, in
 *	\brief an interactive mode where each line typed is passed
 *	\brief to the target.
 */
int	interactiveMode;

/**	\var int	multirespMode
 *	\brief If set, then it is possible that more than one
 *	\brief target will respond, so don't quit after one
 *	\brief response is received.
 */
int	multirespMode;

/**	\var int waitTime
 *	\brief This is the amount of time that program will
 *	\brief wait for a response prior to giving up.
 *	\brief It is 100ths of a second.
 */
int waitTime;
double fwaitTime;

/**	\var int	socketFd
 *	\brief File descriptor of the open socket used to
 *	\brief communicate with the target host.
 */
int	socketFd;

/** \var int verbose
 *	\brief Set for more verbosity.
 */
int verbose;

/**	\var int	cmdRcvd
 *	\brief Set if the command has been received by the target, but no
 *	\brief additional response has been received.
 */
int	cmdRcvd;

/**	\var int binMode
 *	\brief If set, then moncmd is sending/receiving binary.
 */
int binMode;

/** \var char *lastlinebuf
 *	\brief If set, then the last line of response from the target
 *	\brief is stored to this buffer.
 */
char *lastlinebuf;

/** \var int lastlinesize
 *	\brief Used if lastlinebuf is set so that the buffer is not overrun.
 */
int lastlinesize;

/** \var struct sockaddr_in targetAddr
 *	\brief This structure is used to hold the socket
 *	\brief address info related to the target host.
 */
struct sockaddr_in targetAddr;

#ifdef BUILD_WITH_VCC
WSADATA WsaData;
DWORD	tid = (DWORD)0;
HANDLE	tHandle;
#else
pthread_attr_t attr;
pthread_t tid = (pthread_t)0;
#endif

/** \fn void Giveup(int sig)
 *	\brief This function is called if the alarm worker determines that
 *	\brief it is time to call it quits.
 */
void
Giveup(int sig)
{
	if (!quietMode) {
		if (cmdRcvd) {
			fprintf(stderr,
				"\n%s timeout: command received but not completed.\n",
				thisProgname);
		}
		else {
			fprintf(stderr,"\n%s timeout: command not received.\n",
				thisProgname);
		}
	}
	else {
							/* If quiteMode is set, then we just exit with */
		exit(EXIT_SUCCESS);	/* success because the user chooses to ignore */
							/* the error. */
	}

	if (cmdRcvd)
		exit(EXIT_CMP_TIMEOUT);
	else
		exit(EXIT_ACK_TIMEOUT);
}

/**	\fn int do_moncmd(char *hostname, char *command_to_monitor, short portnum)
 *	\param hostname Name or IP of target.
 *	\param command_to_monitor String to be sent to the target.
 *	\param portnum UDP port number to use.
 *	\brief Open a socket and send the command to the specified port of the
 *	\brief specified host.  Wait for a response if necessary.
 */
int
do_moncmd(char *hostname, char *command_to_monitor, short portnum)
{
	int	i;
	unsigned msglen;
	uint32_t inaddr;
	struct	hostent	*hp, host_info;
	char	rcvmsg[4096*4], cmdline[128];

#ifdef BUILD_WITH_VCC
	if (WSAStartup (0x0101, &WsaData) == SOCKET_ERROR)
		err("WSAStartup Failed");
#endif

	cmdRcvd = 0;
	retryCount = 0;
	targetHost = hostname;

	/* Accept target name as string or internet dotted-decimal address:
	 */
	memset((char *)&targetAddr,0,sizeof(struct sockaddr));
	if ((inaddr = inet_addr(targetHost)) != INADDR_NONE) {
		memcpy((char *)&targetAddr.sin_addr,(char *)&inaddr,sizeof(inaddr));
		host_info.h_name = NULL;
	}
	else {
		hp = gethostbyname(targetHost);
		if (hp == NULL)
			err("gethostbyname failed");
		host_info = *hp;
		memcpy((char *)&targetAddr.sin_addr,hp->h_addr,hp->h_length);
	}
	targetAddr.sin_family = AF_INET;
	targetAddr.sin_port = htons(portnum);

	socketFd = socket(AF_INET,SOCK_DGRAM,0);

	if (SOCKET_INVALID(socketFd))
		err("socket failed");

	do {
		if (interactiveMode) {
			do {
				fgets(cmdline,sizeof(cmdline),stdin);
				command_to_monitor = cmdline;
			} while (strlen(command_to_monitor) == 0);
		}

		if (verbose)
			printf("Sending <%s> to %s...\n",command_to_monitor,targetHost);

		msgString = command_to_monitor;
		if (sendto(socketFd,msgString,(int)strlen(msgString)+1,0,
		    (struct sockaddr *)&targetAddr,sizeof(targetAddr)) < 0) {
			close(socketFd);
			err("sendto failed");
		}

		/* If the -w option says that wait time is zero, then don't bother
		 * waiting for a response, just return here.
		 */
		if (waitTime <= 0) {
			close(socketFd);
			return(EXIT_SUCCESS);
		}

here:
		while(1) {
			int	j;
			fd_set	fd_read;
			struct	timeval tval;

			tval.tv_sec = waitTime/100;
			tval.tv_usec = (waitTime % 100) * 10000;

			FD_ZERO(&fd_read);
			FD_SET(socketFd,&fd_read);

			/* Wait for incoming message: */
			switch(select(socketFd+1,&fd_read,0,0,&tval)) {
				case 0:
					if ((retryFlag) && (!cmdRcvd)) {
						retryCount++;
						if (verbose)
							fprintf(stderr,"Retry %d: <%s> to %s...\n",
								retryCount, msgString,targetHost);
						if (sendto(socketFd,msgString,
							(int)strlen(msgString)+1,0,
						    (struct sockaddr *)&targetAddr,
							sizeof(targetAddr))<0) {
							close(socketFd);
							err("re-sendto failed");
						}
						goto here;
					}
					else {
						close(socketFd);
						Giveup(0);
					}
					break;
				case 1:
					break;
				default:
					perror("select");
					close(socketFd);
					exit(EXIT_ERROR);
			}

			msglen = sizeof(struct sockaddr);
			i = recvfrom(socketFd,rcvmsg,sizeof(rcvmsg),0,
				(struct sockaddr *)&targetAddr,&msglen);

			if (i == 0) {
				fprintf(stderr,"Connection closed\n");
				close(socketFd);
				exit(EXIT_ERROR);
			}
			else if (SOCKET_INVALID(i)) {
				perror("recvfrom");
				close(socketFd);
				exit(EXIT_ERROR);
			}

			/* If ANY response is received from the target, then the command
			 * was received...
			 */
			cmdRcvd = 1;

			/* If size is 1 and 1st byte is 0 assume that's the target
			 * saying "I'm done".
			 */
			if ((i==1) && (rcvmsg[0] == 0)) {
				if (multirespMode)
					continue;
				break;
			}

			/* Print the received message:
			 */
			for(j=0;j<i;j++)
				putchar(rcvmsg[j]);
			fflush(stdout);
			if (lastlinebuf) {
				memset(lastlinebuf,0,lastlinesize);
				if (i < lastlinesize)
					memcpy(lastlinebuf,rcvmsg,i);
				else
					memcpy(lastlinebuf,rcvmsg,lastlinesize-1);
			}
		}
		if (interactiveMode) {
			printf("uMON>");
			fflush(stdout);
		}
	} while (interactiveMode);

	close(socketFd);
	return(EXIT_SUCCESS);
}

/**	\fn void moncmd_buffer_last_line(char *buf, int size)
 *	\brief Called before do_moncmd() if the caller wants to
 *	\brief look at the last line of the received response.
 */
void
moncmd_buffer_last_line(char *buf, int size)
{
	lastlinebuf = buf;
	lastlinesize = size;
}

/**	\fn void moncmd_init(char *progname)
 *	\brief Initialize globals at startup
 *	\param progname Value of argv[0] at startup.
 */
void
moncmd_init(char *progname)
{
	thisProgname = progname;
	binMode = 0;
	fwaitTime = 10.0;
	verbose = 0;
	multirespMode = 0;
	lastlinebuf = 0;
	lastlinesize = 0;
	interactiveMode = 0;
	retryFlag = quietMode = 0;
}
