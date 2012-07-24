
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#ifdef BUILD_WITH_VCC
#include <io.h>
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#define sleep(n) Sleep(n*1000)
#define SOCKET_INVALID(s)	(s == INVALID_SOCKET)
typedef unsigned short ushort;
#else
#define INT int
#define SOCKET int
#ifndef O_BINARY
#define O_BINARY 0
#endif
#define SOCKET_INVALID(s)	(s < 0)
#define SOCKADDR_IN struct sockaddr_in
#define PSOCKADDR struct sockaddr *
#define SetConsoleTitle(n)
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include "ttftp.h"


#define USE_SELECT

#define TFTP_BUSY		1
#define TFTP_IDLE		2
#define TFTP_GETLASTACK	3

#define OPEN_WRITE_FLAGS	O_WRONLY|O_BINARY|O_TRUNC|O_CREAT
#define OPEN_READ_FLAGS		O_RDONLY | O_BINARY

int		TftpState, msgCount, RRQfd, WRQfd;
char	WRQfile[128], RRQfile[128];
#ifdef BUILD_WITH_VCC
HANDLE	hdlTimer;
#else
pthread_t tidTimer;
#endif

void	stopTimer(void);
void	startTimer(void);
char	*ServerTitleBar="TFTP SERVER";
extern	int tftpSrvrTimeout, tftpVerbose;

#ifdef BUILD_WITH_VCC
int
showMyIp(void)
{
	struct	hostent	*hp;
	char	myip[32], hostname[128];

	strcpy(myip,"???");
	if (gethostname(hostname,sizeof(hostname)) != 0) {
//		perror("gethostname failed");
		return(-1);
	}

	if ((hp = gethostbyname(hostname)) == (struct hostent *)0) {
//		perror("gethostbyname failed");
		return(-1);
	}

	if (hp->h_addrtype == AF_INET)
		strcpy(myip,inet_ntoa(*(struct in_addr *)hp->h_addr_list[0]));

	printf("Server: '%s'\n",myip);
	return(0);
}

#else

int
showMyIp(void)
{
	struct  in_addr *ipr;
	struct  hostent *hostptr;
	struct  utsname uts;

	/* Retrieve system name and address: */
	uname(&uts);
	hostptr = gethostbyname(uts.nodename);
	if (hostptr == NULL) {
//		perror("gethostbyname failed");
		return(-1);
	}
	ipr = *(struct in_addr **)hostptr->h_addr_list;
	if (*hostptr->h_aliases)
		printf("Server '%s' addr '%s'\n",*hostptr->h_aliases,
			inet_ntoa(*ipr));
	else
		printf("Server '%s' addr '%s'\n",hostptr->h_name,
			inet_ntoa(*ipr));

	return(0);
}
#endif

int
tftpsrvr(void)
{
#ifdef BUILD_WITH_VCC
	WSADATA		WsaData;
#endif
#ifdef USE_SELECT
	fd_set		rset;
	struct timeval t;
	INT			tmout, rc;
#endif
	SOCKET		listener;
	SOCKADDR_IN localAddr;
	char		RRQmode[32];
	unsigned	localAddr_len;
	char		*WRQmode, *errmsg;
	ushort		opcode, blockno, errcode;
	char		rcvmsg[1024], sndmsg[1024], title[256];
	INT			err, datlen, rcvlen, len, rcvtot;

#ifdef BUILD_WITH_VCC
	err = WSAStartup (0x0101, &WsaData);
	if (err == SOCKET_ERROR) {
		fprintf (stdout, "WSAStartup Failed\n");
		return(-1);
	}
#endif

	SetConsoleTitle(ServerTitleBar);

	// Open a socket to listen for incoming connections.
	listener = socket (AF_INET, SOCK_DGRAM, 0);
	if (SOCKET_INVALID(listener)) {
		fprintf (stdout, "Socket Create Failed\n");
		return(-1);
	}

	// Bind our server to the TFTP port number.
	memset ((char *)&localAddr, 0, sizeof (localAddr));
	localAddr.sin_port = htons (tftpPort);
	localAddr.sin_family = AF_INET;

	err = bind (listener, (PSOCKADDR) & localAddr, sizeof (localAddr));
	if (SOCKET_INVALID(err)) {
		fprintf (stderr,"Socket Bind Failed\n");
#ifdef BUILD_WITH_VCC
		if (WSAGetLastError () == WSAEADDRINUSE)
			fprintf (stderr,"The port number may already be in use.\n");
#endif
		return(-1);
	}
	TftpState = TFTP_IDLE;
	RRQfd = WRQfd = msgCount = 0;
	showMyIp();

#ifdef USE_SELECT
	FD_ZERO(&rset);
#endif

	while(1) {

#ifdef USE_SELECT
		tmout = 0;
		FD_SET(listener, &rset);
		t.tv_sec = 1;
		t.tv_usec = 0;
select_more:
		rc = select(listener+1, &rset, 0, 0, &t);

		if (rc <= 0) {
			if (rc < 0) {
				perror("select");
				break;
			}
			else {
				if (TftpState != TFTP_IDLE) {
					if ((tmout == 0) &&
						(*(ushort *)sndmsg == htons(TFTP_DAT))) {
						/* If we're here, then we sent a DAT packet, but 
						 * did not get an ACK after one second, so we
						 * resend just once...
						 */
						tmout = 1;
//						sysTrace(TFTP_TRACE,"TFTP ackrcv tmout, resending");
						if (sendto(listener,sndmsg,len,0,
							(struct sockaddr *)&localAddr,
							sizeof(struct sockaddr)) < 0) {
								perror("sendto5() failed");
						}
						FD_SET(listener, &rset);
						t.tv_sec = 5;
						t.tv_usec = 0;
						goto select_more;
					}
					else if (tmout == 1) {
						FD_SET(listener, &rset);
						t.tv_sec = 5;
						t.tv_usec = 0;
						goto select_more;
					}
//					else {
//						sysTrace(TFTP_TRACE,"TFTP SERVER timeout");
//					}
				}
			}
			if (RRQfd > 0) {
				close(RRQfd);
				RRQfd = 0;
			}
			if (WRQfd > 0) { 
				close(WRQfd);
				WRQfd = 0;
			}
			TftpState = TFTP_IDLE;
			continue;
		}
		else {
			localAddr_len = sizeof(struct sockaddr);
			rcvlen = recvfrom(listener,rcvmsg,sizeof(rcvmsg),0,
				(struct sockaddr *)&localAddr,&localAddr_len);
			
			if (rcvlen <= 0)
				continue;
		}
		
#else
		localAddr_len = sizeof(struct sockaddr);
		rcvlen = recvfrom(listener,rcvmsg,sizeof(rcvmsg),0,
			(struct sockaddr *)&localAddr,&localAddr_len);
#endif
		msgCount++;
		opcode = ntohs(*(ushort *)rcvmsg);
		switch(opcode) {
		case TFTP_RRQ:
			printf("\nTFTP_RRQ from %s...\n",inet_ntoa(localAddr.sin_addr));
			test_OpcodePassCount = 0;
			strcpy(RRQfile,&rcvmsg[2]);
			strcpy(RRQmode,&rcvmsg[strlen(RRQfile)+3]);
			if (TftpState == TFTP_BUSY) {
				printf("TFTP_RRQ rqst: %s (%s) failed, srvr busy\n",
					RRQfile,RRQmode);
				*(ushort *)sndmsg = htons(TFTP_ERR);
				*(ushort *)&sndmsg[2] = htons(2);
				strcpy(&sndmsg[4],"Server is busy");
				len = 19;
				blockno = 0;
				goto tag1;
			}
			RRQfd = open(RRQfile,OPEN_READ_FLAGS);
			if (RRQfd == -1) {
				printf("TFTP_RRQ rqst: %s (%s) failed, file not found\n",
					RRQfile,RRQmode);
				*(ushort *)sndmsg = htons(TFTP_ERR);
				*(ushort *)&sndmsg[2] = htons(1);
				strcpy(&sndmsg[4],"File not found");
				len = 19;
				blockno = 0;
			}
			else {
				printf("TFTP transferring: %s (%s)\n",RRQfile,RRQmode);
				sprintf(title,"TFTP_RRQ: %s",RRQfile);
				SetConsoleTitle(title);

				blockno = 1;
				*(ushort *)sndmsg = htons(TFTP_DAT);
				*(ushort *)&sndmsg[2] = htons(blockno);
				len = read(RRQfd,&sndmsg[4],512);
				if (len < 512) {
					close(RRQfd);
					RRQfd = 0;
					printf("TFTP %s transfer complete\n",RRQfile);
					SetConsoleTitle(ServerTitleBar);
					TftpState = TFTP_GETLASTACK;
				}
				else {
					TftpState = TFTP_BUSY;
					startTimer();
				}
				if (len >= 0)
					len += 4;
				else
					len = 4;
			}
tag1:
			testTftp(TFTP_RRQ,sndmsg,len);
			if (sendto(listener,sndmsg,len,0,(struct sockaddr *)&localAddr,
			  sizeof(struct sockaddr)) < 0) {
				perror("sendto failed");
				return(EXIT_ERROR);
			}
			break;
		case TFTP_ACK:
			if (RRQfd == -1)
				break;
			if (TftpState == TFTP_GETLASTACK) {
				TftpState = TFTP_IDLE;
				break;
			}
			if (tftpVerbose)
				printf("Rcvd TFTP_ACK blk#%d\n",htons(*(ushort *)&rcvmsg[2]));
			if (*(ushort *)&rcvmsg[2] == htons(blockno)) {
				blockno++;
				*(ushort *)sndmsg = htons(TFTP_DAT);
				*(ushort *)&sndmsg[2] = htons(blockno);
				len = read(RRQfd,&sndmsg[4],512);
				if (len < 512) {
					close(RRQfd);
				}
				if (len >= 0)
					len += 4;
				else {
					TftpState = TFTP_IDLE;
					printf("TFTP %s transfer completed\n",RRQfile);
					SetConsoleTitle(ServerTitleBar);
					RRQfd = 0;
					stopTimer();
					continue;
				}
			}
			else {
				*(ushort *)sndmsg = htons(TFTP_ERR);
				*(ushort *)&sndmsg[2] = htons(2);
				strcpy(&sndmsg[4],"Ack confused");
				len = 17;
				TftpState = TFTP_IDLE;
				close(RRQfd);
				RRQfd = WRQfd = 0;
				stopTimer();
			}
			testTftp(TFTP_ACK,sndmsg,len);
			if (sendto(listener,sndmsg,len,0,(struct sockaddr *)&localAddr,
			  sizeof(struct sockaddr)) < 0) {
				perror("sendto failed");
				return(EXIT_ERROR);
			}
			break;
		case TFTP_WRQ:
			printf("\nTFTP_WRQ from %s...\n",inet_ntoa(localAddr.sin_addr));

			test_OpcodePassCount = 0;
			strcpy(WRQfile,&rcvmsg[2]);
			WRQmode = &rcvmsg[2];
			while(*WRQmode) WRQmode++;
			WRQmode++;
			if (TftpState == TFTP_BUSY) {
				printf("TFTP_WRQ rqst: %s (%s) failed, srvr busy\n",
					WRQfile,WRQmode);
				*(ushort *)sndmsg = htons(TFTP_ERR);
				*(ushort *)&sndmsg[2] = htons(2);
				strcpy(&sndmsg[4],"Server is busy");
				len = 19;
				blockno = 0;
				goto tag2;
			}
			if (strcmp(WRQmode,"octet")) {
				fprintf(stderr,"%s: %s mode not supported\n",WRQfile,WRQmode);
				break;
			}
			printf("TFTP receiving: '%s'\n",WRQfile);
			unlink(WRQfile);
			if ((WRQfd = open(WRQfile,OPEN_WRITE_FLAGS,0777))==-1) {
				perror(WRQfile);
				break;
			}
			TftpState = TFTP_BUSY;
			sprintf(title,"TFTP_WRQ: %s",WRQfile);
			SetConsoleTitle(title);
			startTimer();
			rcvtot = 0;
			*(ushort *)sndmsg = htons(TFTP_ACK);
			*(ushort *)&sndmsg[2] = 0;
			len = 4;
tag2:
			testTftp(TFTP_WRQ,sndmsg,len);
			if (sendto(listener,sndmsg,len,0,(struct sockaddr *)&localAddr,
			  sizeof(struct sockaddr)) < 0) {
				perror("sendto (TFTP_ACK) failed");
				return(EXIT_ERROR);
			}
			break;
		case TFTP_DAT:
			if (tftpVerbose)
				printf("Rcvd TFTP_DAT (pktsiz=%d,rcvtot=%d)\n",rcvlen,rcvtot);
			datlen = rcvlen - 4;
			if (datlen >= 0) {
				if (datlen > 0) {
					if (write(WRQfd,&rcvmsg[4],datlen) != datlen) {
						perror("write (TFTP_DAT) failed");
						return(EXIT_ERROR);
					}
				}
				rcvtot += datlen;
				if (datlen != 512) {
					TftpState = TFTP_IDLE;
					close(WRQfd);
					printf("Receive completed (%d bytes).\n",rcvtot);
					SetConsoleTitle(ServerTitleBar);
					stopTimer();
				}
			}
			*(ushort *)sndmsg = htons(TFTP_ACK);
			sndmsg[2] = rcvmsg[2];	/* Copy blockno for ack. */
			sndmsg[3] = rcvmsg[3];
			len = 4;
			testTftp(TFTP_DAT,sndmsg,len);
			if (sendto(listener,sndmsg,len,0,(struct sockaddr *)&localAddr,
			  sizeof(struct sockaddr)) < 0) {
				perror("sendto (TFTP_ACK) failed");
				return(EXIT_ERROR);
			}
			if ((datlen >= 0) && (datlen != 512) && tftpExitAfterRcv)
				return(EXIT_SUCCESS);
			break;
		case TFTP_ERR:
			testTftp(TFTP_ERR,0,0);
			errcode = htons(*(ushort *)&rcvmsg[2]);
			errmsg = &rcvmsg[4];
			if (RRQfd != 0) {
				close(RRQfd);
				RRQfd = 0;
			}
			if (WRQfd != 0) {
				close(WRQfd);
				WRQfd = 0;
			}
			TftpState = TFTP_IDLE;
			stopTimer();
			printf("Rcv'd ERROR %d <%s>, transaction terminated by server.\n",
				errcode, errmsg);
//errcode != 0 ? errmsg : "???");
			SetConsoleTitle(ServerTitleBar);
			break;
		}
	}
	return(EXIT_SUCCESS);
}

// TimerThread:
// This is used to kick off a timer that will keep track of the msgCount
// value and the TftpState variable.  It is started when WRQ or RRQ is
// received; then, every 5 seconds TftpState is polled.  If the state is
// busy, and tftpSrvrTimeout seconds have elapsed without a change in
// msgCount, then assume that the client went away and clear the busy state.
// If not busy, then assume the transaction has completed and terminate.
//
#ifdef BUILD_WITH_VCC
DWORD WINAPI timerThread(LPVOID notusedhere)
#else
void *timerThread(void *notusedhere)
#endif
{
	int	lastcount, no_activity_time;

	/* Don't use the timer thread if a test is active. */
	if (test_Opcode)
		return(0);

	no_activity_time = 0;
	while(1) {
		sleep(5);
		if (TftpState == TFTP_BUSY) {
			if (lastcount == msgCount) {
				no_activity_time += 5;
				if (no_activity_time == 5)
					fprintf(stderr,"TFTP server waiting for response");
				else
					fprintf(stderr,".");
			}
			else {
				lastcount = msgCount;
				no_activity_time = 0;
			}
			if (no_activity_time >= tftpSrvrTimeout) {
				fprintf(stderr,"\nTFTP timeout");
				if (RRQfd > 0) {
					close(RRQfd);
					fprintf(stderr,", RRQ of %s aborted",RRQfile);
				}
				if (WRQfd > 0) {
					close(WRQfd);
					fprintf(stderr,
						", WRQ of %s aborted (file prematurely truncated)",
						WRQfile);
				}
				fprintf(stderr,".\n");
				TftpState = TFTP_IDLE;
				SetConsoleTitle(ServerTitleBar);
				stopTimer();	/* Doesn't return, it kills this thread. */
			}
		}
		else {
			if (no_activity_time > 0)
				fprintf(stderr,"\n");
			stopTimer();	/* Doesn't return, it kills this thread. */
		}
	}
	return(0);				/* Should never get here. */
}

#ifdef USE_SELECT
void
startTimer(void)
{}

void
stopTimer(void)
{}

#else

#ifdef BUILD_WITH_VCC
void
startTimer(void)
{
	DWORD	tid;

	/* Don't use the timer thread if a test is active. */
	if (test_Opcode)
		return;

	hdlTimer = CreateThread(NULL,0,timerThread,(LPVOID)0,CREATE_SUSPENDED,&tid);
	if (!hdlTimer) {
		perror("CreatThread()");
		return;
	}
	ResumeThread(hdlTimer);
}

void
stopTimer(void)
{
	/* Don't use the timer thread if a test is active. */
	if (test_Opcode)
		return;

	if (hdlTimer) {
		if (TerminateThread(hdlTimer,1) == FALSE) {
			fprintf(stderr,"Thread termination failed\n");
			return;
		}
	}
}
#else
void
startTimer(void)
{
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr,0x8000);
	pthread_create(&tidTimer,&attr,timerThread,(void *)0);
}

void
stopTimer(void)
{
	if (tidTimer != (pthread_t)NULL) {
		pthread_cancel(tidTimer);
		tidTimer = (pthread_t)NULL;
	}
}
#endif
#endif
