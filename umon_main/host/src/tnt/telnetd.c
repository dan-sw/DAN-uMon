/* telnetd.c:
 * This simple server provides 'tc' with the ability to connect
 * multiple telnet clients to the same TTY port at the same time.
 * Each connection's (up to 32) interaction with the COM port will
 * be seen by every client that is attached.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "telnetd.h"
#include "utils.h"
#include "tnt.h"

#define INVALID_SOCKET	-1	
#define PORT_RETRY_MAX	10
#define PASSWORDPROMPT	"Password: "

static int	listener;
static int	telnetReadOnly;
static char telnetWelcome[128];
static char telnetPassword[128];

extern int runasdaemon;
extern void terminate_message(void);

/* net_write():
 * This function writes the same buffer to each of the sockets that
 * are currently opened to telnet clients.
 */
int
net_write(char *buffer, int len)
{
	int	i;

	for (i=0;i<(sizeof(SockTbl)/sizeof(struct sockinfo));i++) {
		if (SockTbl[i].socket != INVALID_SOCKET) {
			send(SockTbl[i].socket,buffer,len,0);
		}
	}
	return(len);
}

/* processIAC():
 * This is pretty dumb.  All it does is ignore the telnet IAC 
 * (interpret as command) stuff from the client.
 * This way the data is not dumped to the 
 * COM port.
 */
void
processIAC(int socket)
{
	int	tstate;
	char c;

	tstate = TELNET_IAC;
	while(tstate != TELNET_BRK) {
		if (recv(socket,&c,1,0) <= 0) {
			break;
		}
		if (tstate == TELNET_IAC) {
			switch(c) {
				case TELNET_IAC:
					tstate = TELNET_BRK;
					break;
				case TELNET_DO:
				case TELNET_DONT:
				case TELNET_WILL:
				case TELNET_WONT:
					recv(socket,&c,1,0);
					tstate = TELNET_BRK;
					break;
				case TELNET_SB:
					tstate = TELNET_SB;
					break;
				default:
					break;
			}
		}
		else if (tstate == TELNET_SB) {
			if (c == TELNET_IAC) {
				recv(socket,&c,1,0);
				if (c == TELNET_SE) {
					tstate = TELNET_BRK;
				}
			}
		}
	}
}

void *
serverSubThread(void *id)
{
	struct sockinfo *sptr;
	char	c, crlf, gotcrlf, msg[80];

	sptr = &SockTbl[(int)id];

	/* Option setup...
	 * LINEMODE tells the client to send each character as it is typed.
	 * ECHO tells the client that the server will echo the characters.
	 */
	msg[0] = (char)TELNET_IAC;
	msg[1] = (char)TELNET_DO;
	msg[2] = (char)TELNET_OPTION_LINEMODE;	/* See RFC 1116 */
	msg[3] = (char)TELNET_IAC;
	msg[4] = (char)TELNET_WILL;
	msg[5] = (char)TELNET_OPTION_ECHO;		/* See RFC 857 */
	send(sptr->socket,msg,6,0);

	if (telnetPassword[0]) {
		int		pidx;
		char	password[32];

		socprintf(sptr->socket,"\n\n%s",PASSWORDPROMPT);

		for(pidx=0;pidx<sizeof(password);pidx++) {
			if (recv(sptr->socket,&c,1,0) <= 0)
				goto done;

			if (c == 0) {
				pidx--;
				continue;
			}
			if (c == TELNET_IAC) {
				processIAC(sptr->socket);
				pidx--;
				continue;
			}
			send(sptr->socket," ",1,0);
			password[pidx] = c;
			if ((c == '\n') || (c == '\r')) {
				password[pidx] = 0;
				if (strcmp(password,telnetPassword))
					goto done;
				send(sptr->socket,"\r\n",2,0);
				break;
			}
		}
		if (pidx == sizeof(password))
			goto done;
	}

	if (telnetWelcome[0])
		socprintf(sptr->socket,"\n\n%s\n\n",telnetWelcome);
	else
		socprintf(sptr->socket,"\n\nTNT connected\n\n");

	gotcrlf = crlf = 0;
	while(1) {

		if (recv(sptr->socket,&c,1,0) <= 0)
			break;

		if (c == 0)
			continue;

		if (c == TELNET_IAC) {
			processIAC(sptr->socket);
			continue;
		}

		/* As a Network Virtual Terminal (NVT) the client is supposed to
		 * map the ENTER (or RETURN) key of the user to CR-LF (see Comer
		 * pg411).  If CCP->telnetCRLFProc is set, then we strip the 
		 * carriage return and only pass the line feed.
		 */
		if ((c == '\n') || (c == '\r')) {
			gotcrlf = 1;
			if ((crlf) && (crlf != c)) {
				crlf = 0;
				continue;
			}
			crlf = c;
		}
		else {
			gotcrlf = 0;
			crlf = 0;
		}

		if (telnetReadOnly == 0)
			ttywrite(&c,1);
	}

done:
	close(sptr->socket);
	sptr->socket = INVALID_SOCKET;
	sptr->warning = 0;
	return(0);
}

int
telnetd(char *password, char *welcome, short port, int readonly)
{
	int		rc, i, retry;
	struct	sockaddr_in serv_addr;

	telnetReadOnly = readonly;
	if (password) 
		strcpy(telnetPassword,password);
	else
		telnetPassword[0] = 0;
	if (welcome) 
		strcpy(telnetWelcome,welcome);
	else
		telnetWelcome[0] = 0;

	for (i=0;i<(sizeof(SockTbl)/sizeof(struct sockinfo));i++) {
		SockTbl[i].handle = 0;
		SockTbl[i].warning = 0;
		SockTbl[i].socket = INVALID_SOCKET;
	}

	// Open a socket to listen for incoming connections.
	listener = socket (AF_INET, SOCK_STREAM, 0);
	if (listener < 0) {
		err("socket() error");
		return(-1);
	}

	/* Bind a local address so the client can send to us. */
	bzero((char *)&serv_addr,sizeof(struct sockaddr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);

				
	/* Since multiple uCon sessions may be running on a single machine,
	 * if the bind fails, we assume that the port is being used by some
	 * other uCon session already up, so just increment the port number
	 * and try again (for a while)...
	 */
	for(retry=0;retry < PORT_RETRY_MAX;retry++) {
		rc = bind(listener,(struct sockaddr *)&serv_addr,
				sizeof(struct sockaddr));
		if (rc >= 0)
			break;
		serv_addr.sin_port = htons(++port);
	}
	if (retry == PORT_RETRY_MAX) {
		perror("bind failed, server disabled");
		close(listener);
		return(-1);
	}

	/* Prepare to accept client connections.  Allow up to 5
	 * pending connections.
	 */
	rc = listen (listener, 5);
	if (rc < 0) {
		err("listen() failed");
		return(-1);
	}

	if (runasdaemon == 0) {
		fprintf(stderr,"Telnet: port %d\n",port);
		terminate_message();
	}

	while(1) {
		pthread_attr_t attr;
		int		sidx;
		socklen_t fromlen;
		struct	sockaddr_in	from;
		pthread_t tid = (pthread_t)0;

		for (sidx=0;sidx<(sizeof(SockTbl)/sizeof(struct sockinfo));sidx++) {
			if (SockTbl[sidx].socket == INVALID_SOCKET) {
				break;
			}
		}

		/* If there are no available slots, then wait a few seconds and retry
		 * with the hope that one of the connected clients will disconnect.
		 */
		if (sidx >= (sizeof(SockTbl)/sizeof(struct sockinfo))) {
			sleep(1);
			continue;
		}

		fromlen = sizeof(from);
		SockTbl[sidx].socket =
			accept(listener,(struct sockaddr *)&from,&fromlen);

		SockTbl[sidx].from = from;

		if (SockTbl[sidx].socket == INVALID_SOCKET) {
			perror("accept() failed");
			return(-1);
		}

		pthread_attr_init(&attr);
		pthread_attr_setstacksize(&attr,0x8000);
		pthread_create(&tid,&attr,serverSubThread,(void *)sidx);
		SockTbl[sidx].handle = tid;
	}
	return(0);
}
