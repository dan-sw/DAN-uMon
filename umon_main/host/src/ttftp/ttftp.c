/* TTFTP:
	An optional replacement for tftp on sun and pc.

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
#include <ctype.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include "ttftp.h"
#ifdef BUILD_WITH_VCC
#include <io.h>
#include <winsock2.h>
#define uint32_t unsigned long
#else
#define Sleep(a)	sleep(a/1000)
#include <unistd.h>
#include <stdint.h>
#ifndef O_BINARY
#define O_BINARY 0
#endif
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#ifndef IPPORT_TFTP
#define IPPORT_TFTP 69
#endif

#ifdef BUILD_WITH_VCC
typedef unsigned short ushort;
#else
#define INVALID_SOCKET	-1
#endif

extern	void usage(char *);
char	consoleTitle[256];
short	tftpPort;
int		tftpVerbose, tftpQuiet, RetryCount, tftpSrvrTimeout, tftpPpd;
int		tftpRFC2349TsizeEnabled, tftpExitAfterRcv;

void
err(char *msg)
{
	if (msg)
		fprintf(stderr,"%s, ",msg);
#ifdef BUILD_WITH_VCC
	fprintf(stderr,"err=%d\n",WSAGetLastError());
#else
	perror("");
#endif
	exit(EXIT_ERROR);
}

void
ttftp_init(void)
{
	tftpPort = IPPORT_TFTP;
	tftpPpd = 0;
	tftpQuiet = 0;
	RetryCount = 0;
	tftpVerbose = 0;
	tftpSrvrTimeout = 60;
	tftpRFC2349TsizeEnabled = 1;
	tftpExitAfterRcv = 0;
}

void
tick(void)
{
	static char *hand[] = { "|\b", "/\b", "-\b", "\\\b" };
	static int tock;
	
	if (tftpQuiet < 2) {
		if (tock > 3)
			tock = 0;
		write(1,hand[tock++],2);
	}
}

void
showerr(char *msg,int len)
{
	ushort	errnum;

	errnum = ntohs(*(ushort *)(msg+2));
	if (len > 5)
		printf("\nTFTP ERROR %d: %s\n",errnum,&msg[4]);
	else 
		printf("\nTFTP ERROR %d\n",errnum);
}

void
SendTo(int sfd,char *msg,int len,int flags,
	struct sockaddr *to,int tolen,char *desc,int pass)
{
	static	int	Sfd, MsgLen, Flags, ToLen;
	static	char 	Msg[1024], Desc[128];
	static	struct sockaddr	Saddr;

	/* If this is pass zero, then copy the data to static space in */
	/* case it is needed for retry. */
	if (pass == 0) {
		RetryCount = 0;
		Sfd = sfd;
		MsgLen = len;
		Flags = flags;
		ToLen = tolen;
		if ((len > 1024) || (strlen(desc) > 128)) {
			fprintf(stderr,"too big\n");
			exit(EXIT_ERROR);
		}
		memcpy(Msg,msg,len);
		strcpy(Desc,desc);
		Saddr = *to;
	}
	if (tftpVerbose)
		fprintf(stderr,"Sending %s...\n",Desc);

	if (sendto(Sfd,Msg,MsgLen,Flags,&Saddr,sizeof(Saddr)) < 0)
		err("sendto failed");
}

char *
strtolower(char *string)
{
	char *base, *cp, *sp;

	sp = string;
	base = cp = malloc(strlen(string) + 1);
	while(*sp) 
		*cp++ = tolower(*sp++);
	*cp = 0;
	return(base);
}

#ifdef BUILD_WITH_VCC
/* perror():
 * Implementation of a discussion in VC++ documentation for displaying
 * the error string that corresponds to the return value of GetLastError().
 * A win32 implementation of perror()...
 */
void
perror(const char *msg)
{
	int		err;
	LPVOID	lpMessageBuffer;

	err = GetLastError();
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMessageBuffer, 0, NULL);
	if (!msg)
		msg = "";
	fprintf(stderr,"Error:\t %s: %s",msg,lpMessageBuffer);
	LocalFree(lpMessageBuffer);
}
#endif

int
ttftp(char *sysname, char *getorput, char *srcfile,
	char *destfile, char *xfermode)
{
	unsigned sfd;
	ushort	opcode;
	unsigned msglen;
	struct	stat mstat;
	uint32_t inaddr;
	ushort	blockno, lastblockno;
	struct	hostent	*hp, host_info;
	struct	sockaddr_in server, resp;
	int		xferlen, filemode, ffd, done, len;
	char	rcvmsg[1024], sndmsg[1024], *openfile;
#ifdef BUILD_WITH_VCC
	WSADATA WsaData;
#endif

	if (!tftpQuiet)
		printf("ttftp(%s %s %s %s %s)\n",
			sysname,getorput,srcfile,destfile,xfermode);

	if (strcmp(strtolower(getorput),"get") == 0) {
		opcode = htons(TFTP_RRQ);
		filemode = O_WRONLY | O_BINARY | O_CREAT | O_TRUNC;
		openfile = destfile; 
		ffd = 0;
		/* Don't open the file till we make sure that it exists on
		 * the system we're going to retrieve it from.
		 */
	}
	else if (strcmp(strtolower(getorput),"put") == 0) {
		opcode = htons(TFTP_WRQ);
		filemode = O_RDONLY | O_BINARY;
		openfile = srcfile; 

		/* Open the file now and if it doesn't exist, just exit.
		 */
		ffd = open(openfile,filemode,0664);
		if (ffd == -1) {
			perror(openfile);
			exit(EXIT_ERROR);
		}
	}
	else
		usage("specify 'get or put'");


	fstat(ffd,&mstat);
	if (opcode == htons(TFTP_WRQ))
		printf("File %s size: %d bytes\n",openfile,mstat.st_size);

	xferlen = mstat.st_size;

#ifdef BUILD_WITH_VCC
	if (WSAStartup (0x0101, &WsaData) == SOCKET_ERROR)
		err("WSAStartup Failed");
#endif

	/* Accept server name as string or internet dotted-decimal address: */
	memset((char *)&server,0,sizeof(struct sockaddr));
	if ((inaddr = inet_addr(sysname)) != INADDR_NONE) {
		memcpy((char *)&server.sin_addr,(char *)&inaddr,sizeof(inaddr));
		host_info.h_name = NULL;
	}
	else {
		hp = gethostbyname(sysname);
		if (hp == NULL)
			err("gethostbyname failed");
		host_info = *hp;
		memcpy((char *)&server.sin_addr,hp->h_addr,hp->h_length);
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(tftpPort);

	sfd = socket(AF_INET,SOCK_DGRAM,0);
	if (sfd == INVALID_SOCKET)
		err("socket failed");

	/* Build the TFTP request: */
	memcpy(sndmsg,&opcode,2);
	
	if (opcode == htons(TFTP_WRQ)) {
		strcpy(&sndmsg[2],destfile);
		msglen = strlen(destfile) + 2;
	}
	else {
		strcpy(&sndmsg[2],srcfile);
		msglen = strlen(srcfile) + 2;
	}
	strcpy(&sndmsg[msglen+1],xfermode);
	msglen += (strlen(xfermode) + 2);
	
	if ((tftpRFC2349TsizeEnabled) && (opcode == htons(TFTP_WRQ))) {
		strcpy(sndmsg+msglen,"tsize");
		msglen += 6;
		msglen += sprintf(sndmsg+msglen,"%d",mstat.st_size);
		msglen++;
	}

	/* Start off the transfer... */
	SendTo(sfd,sndmsg,msglen,0,(struct sockaddr *)&server,
		sizeof(server),"RRQ/WRQ",0);

	done = 0;
	blockno = lastblockno = 0;
	while(!done) {
		fd_set	fd_read;
		int		sval, rcvsz;
		struct	timeval tval;

		tval.tv_sec = 3;
		tval.tv_usec = 0;

		FD_ZERO(&fd_read);
		FD_SET(sfd,&fd_read);
		sval = select(sfd+1,&fd_read,0,0,&tval);
		
		if (sval == 1) {		/* Data ready */
			msglen = sizeof(struct sockaddr);
			rcvsz = recvfrom(sfd,rcvmsg,sizeof(rcvmsg),0,
				(struct sockaddr *)&resp,&msglen);
		}
		else if (sval == 0) {	/* Timeout */
			if (++RetryCount >= RETRY_MAX) {
				fprintf(stderr,"Retries exhausted, giving up!\n");
				exit(EXIT_ERROR);
			}
			tick();
			SendTo(0,0,0,0,0,0,0,1);
			continue;
		}
		else {					/* Error */
			err("select failed");
		}

		if (rcvsz <= 0) {
			fprintf(stderr,"recvfrom size: %d\n",rcvsz);
			err("recvfrom error");
		}

		opcode = ntohs(*(ushort *)rcvmsg);

		if (tftpVerbose)
			printf("Rcvd opcode: 0x%x\n",opcode);

		switch(opcode) {
		case TFTP_DAT:
			if (tftpVerbose)
				printf("Rcvd TFTP_DAT\n");
			else if (tftpQuiet == 0)
				write(1,".",1);
			else if (tftpQuiet == 1)
				tick();
			rcvsz -= 4;
			if (rcvsz) {
				if (ffd == 0) {
					/* Now that we know the file exists, we can open the
					 * file here...
					 */
					ffd = open(openfile,filemode,0664);
					if (ffd == -1) {
						perror(openfile);
						exit(EXIT_ERROR);
					}
				}
				if (write(ffd,&rcvmsg[4],rcvsz) < 0) {
					fprintf(stderr,"\nwrite size=%d\n",rcvsz);
					err("write failed");
				}
			}
			*(ushort *)sndmsg = htons(TFTP_ACK);
			sndmsg[2] = rcvmsg[2];
			sndmsg[3] = rcvmsg[3];
//#ifdef BUILD_WITH_VCC
			testTftp(TFTP_DAT,sndmsg,4);
//#endif
			SendTo(sfd,sndmsg,4,0,(struct sockaddr *)&resp,
				sizeof(server),"ACK",0);
			if (rcvsz < TFTP_DATAMAX)
				done = 1;
			break;
		case TFTP_ACK:
		case TFTP_OACK:
			if (opcode == TFTP_ACK) {
				blockno = ntohs((*(ushort *)(rcvmsg+2)));
				if (tftpVerbose)
					printf("Rcvd TFTP_ACK blk#%d\n",blockno);
			}
			else {
				blockno = 0;
				if (tftpVerbose)
					printf("Rcvd TFTP_OACK\n");
			}
//#ifdef BUILD_WITH_VCC
			if (tftpPpd)
				Sleep(tftpPpd);
//#endif
			
			if (xferlen == -1) {
				done = 1;
				break;
			}
			*(ushort *)sndmsg = htons(TFTP_DAT);
			if (!blockno || (blockno == lastblockno + 1)) {
				if (!tftpVerbose) {
					if (tftpQuiet == 0)
						write(1,".",1);
					else if (tftpQuiet == 1)
						tick();
				}
				if (xferlen > TFTP_DATAMAX) {
					len = TFTP_DATAMAX;
					xferlen -= TFTP_DATAMAX;
				}
				else if (xferlen == TFTP_DATAMAX) {
					len = TFTP_DATAMAX;
					xferlen = 0;
				}
				else {
					if (xferlen > 0)
						len = xferlen;
					else
						len = 0;
					xferlen = -1;
				}
				*(ushort *)(sndmsg+2) = htons((ushort)(blockno+1));
				if (len) {
					int n;
					n = read(ffd,&sndmsg[4],len);
					if (n != len) {
						fprintf(stderr,"read=%d,not%d\n",n,len);
						err("read failed");
					}
				}
			}
			else {
				if (tftpVerbose) {
					fprintf(stderr,"retry blkno=%d\n",
					blockno);
				}
			}
//#ifdef BUILD_WITH_VCC
			testTftp(TFTP_ACK,sndmsg,len+4);
//#endif
			SendTo(sfd,sndmsg,len+4,0,(struct sockaddr *)&resp,
				sizeof(server),"DAT",0);
			lastblockno = blockno;
			break;
		case TFTP_ERR:
			showerr(rcvmsg,rcvsz);
			done = -1;
			break;
		case TFTP_WRQ:
			if (tftpVerbose)
				fprintf(stderr,"Rcvd TFTP_WRQ\n");
			break;
		case TFTP_RRQ:
			if (tftpVerbose)
				fprintf(stderr,"Rcvd TFTP_RRQ\n");
			break;
		default:
			if (tftpVerbose)
				fprintf(stderr,"Rcvd 0x%x ????\n",opcode);
			done = 1;
			break;
		}
	}
	if ((tftpVerbose) || (!tftpQuiet))
		printf("\n");
	close(sfd);
	close(ffd);
	if (done == -1)
		return(EXIT_ERROR);
	else
		return(EXIT_SUCCESS);
}
