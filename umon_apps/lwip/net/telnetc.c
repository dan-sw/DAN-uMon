#include "lwip/debug.h"
#include "lwip/stats.h"
#include "telnet.h"
#include "lwip/tcp.h"
#include "monlib.h"

typedef unsigned char uchar;

static struct tcp_pcb *tnetc_pcb;
static int tnetc_state, tnetc_debug, tnetc_connected;
static int tnetc_le;

//#define DBGPRINT(a)	mon_printf a
#define DBGPRINT(a)

int
telnetc_isactive(void)
{
	return(tnetc_state);
}

void
telnetc_close(void)
{
	tcp_arg(tnetc_pcb, NULL);
	tcp_sent(tnetc_pcb, NULL);
	tcp_recv(tnetc_pcb, NULL);
	tcp_err(tnetc_pcb, NULL);
	tcp_poll(tnetc_pcb, NULL, 0);
	tcp_close(tnetc_pcb);
	tnetc_state = 0;
}

err_t
telnetc_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
	struct pbuf *q;

	if (err != ERR_OK) {
		mon_printf("\nTELNETC: recv failed (err=%d)\n",err);
		telnetc_close();
		return(err);
	}
	if (p == NULL) {
		/* If we're here, the remote host closed the connection.
		 */
		telnetc_close();
		mon_printf("\nTELNETC: Server closed connection\n");
		return(ERR_OK);
	}
	for(q = p; q != NULL; q = q->next) {
		int i;
		char *cp = (char *)q->payload;

		for(i=0;i<q->len;i++) {
			if (*cp == TELNET_IAC) {
				int	tstate;
				char resp[32], c;

				tstate = TELNET_IAC;
				while(tstate != TELNET_BRK) {
					if (i >= q->len)
						mon_printf("TELNETC: OOPS\n");
					cp++; i++;
					c = *cp;
					DBGPRINT(("recv %d (%02x)",(uchar)c,(uchar)c));

					if (tstate == TELNET_IAC) {
						switch(c) {
							case TELNET_IAC:
								tstate = TELNET_BRK;
								break;
							case TELNET_DO:
								cp++; i++;
								c = *cp;
								resp[0] = TELNET_IAC;
								if (c == TELNET_OPTION_TERMTYPE) {
									DBGPRINT(("TELNET_DO TERMTYPE"));
									resp[1] = TELNET_WILL;
								}
								else if (c == TELNET_OPTION_ECHO) {
									DBGPRINT(("TELNET_DO ECHO"));
									resp[1] = TELNET_WILL;
									tnetc_le= 1;
								}
								else {
									DBGPRINT(("TELNET_DO %d",(uchar)c));
									resp[1] = TELNET_WONT;
								}
								resp[2] = c;
								tcp_write(tnetc_pcb,resp,3,TCP_WRITE_FLAG_COPY);
								tstate = TELNET_BRK;
								break;
							case TELNET_DONT:
								cp++; i++;
								c = *cp;
								DBGPRINT(("TELNET_DONT %d",(uchar)c));
								tstate = TELNET_BRK;
								break;
							case TELNET_WILL:
								cp++; i++;
								c = *cp;
								resp[0] = TELNET_IAC;
								if (c == TELNET_OPTION_ECHO) {
									DBGPRINT(("TELNET_WILL ECHO"));
									resp[1] = TELNET_DO;
									tnetc_le= 0;
								}
								else if (c == TELNET_OPTION_TERMTYPE) {
									DBGPRINT(("TELNET_WILL TERMTYPE"));
									resp[1] = TELNET_DO;
								}
								else if (c == TELNET_OPTION_SUPPGOAHEAD) { 
									DBGPRINT(("TELNET_WILL SUPPGOAHEAD"));
									resp[1] = TELNET_DO;
								}
								else {
									DBGPRINT(("TELNET_WILL %d",c));
									resp[1] = TELNET_DONT;
								}
								resp[2] = c;
								tcp_write(tnetc_pcb,resp,3,TCP_WRITE_FLAG_COPY);
								tstate = TELNET_BRK;
								break;
							case TELNET_WONT:
								cp++; i++;
								c = *cp;
								DBGPRINT(("TELNET_WONT %d",(uchar)c));
								tstate = TELNET_BRK;
								break;
							case TELNET_SB:
								tstate = TELNET_SB;
								DBGPRINT(("TELNET_SB"));
								break;
							default:
								tstate = TELNET_BRK;
								break;
						}
					}
					else if (tstate == TELNET_SB) {
						if (c == TELNET_IAC) {
							cp++; i++;
							c = *cp;
							if (c == TELNET_SE) {
								tstate = TELNET_BRK;
								DBGPRINT(("TELNET_SE"));
							}
							else {
								DBGPRINT(("TELNET_SB %d",(uchar)c));
							}
						}
						else if (c == TELNET_OPTION_TERMTYPE) {
							cp++; i++;
							c = *cp;
							DBGPRINT(("TELNET_SB TERMTYPE"));
							resp[0] = TELNET_IAC;
							resp[1] = TELNET_SB;
							resp[2] = TELNET_OPTION_TERMTYPE;
							resp[3] = 'v';
							resp[4] = 't';
							resp[5] = '1';
							resp[6] = '0';
							resp[7] = '0';
							resp[8] = TELNET_IAC;
							resp[9] = TELNET_SE;
							DBGPRINT(("TERMTYPE = VT100"));
							tcp_write(tnetc_pcb,resp,10,TCP_WRITE_FLAG_COPY);
						}
					}
				}
				DBGPRINT(("done\n"));
			}
			else
				mon_putchar(*cp);
			cp++;
		}
	}
	tcp_recved(tpcb,p->tot_len);
	pbuf_free(p);
	return(ERR_OK);
}


err_t
telnetc_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	if (tnetc_debug)
		mon_printf("TELNETC: msg sent (size=%d)\n",len);
	return(ERR_OK);
}

int
telnetc_putchar(char c)
{
	err_t err;

	if (!tnetc_connected)
		return((int)ERR_CONN);
		
	if (c == 0x1d)	{ /* ctrl-] */
		telnetc_close();
		tnetc_connected = 1;
		return(-1);
	}

	err = tcp_write(tnetc_pcb, &c, 1, TCP_WRITE_FLAG_COPY);
	if (err != ERR_OK)  {
		mon_printf("TELNETC: tcp_write failed\n");
		telnetc_close();
	}
	else
		tcp_output(tnetc_pcb);

	return((int)err);
}

err_t
telnetc_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
	if (err != ERR_OK) {
		mon_printf("TELNETC: connect failed (err=%d)\n",err);
		telnetc_close();
	}
	else {
		if (tnetc_debug)
			mon_printf("TELNETC: connected\n");
		tnetc_connected = 1;
	}
	return(err);
}

void
telnetc_err(void *arg, err_t err)
{
	mon_printf("\nTELNETC: error %d\n",err);
	telnetc_close();
}

int
telnetc_init(char *srvrip, short port)
{
	struct ip_addr ipaddr;

	tnetc_state = 1;
	tnetc_le = 0;
	tnetc_connected = 0;
	inet_aton((const char *)srvrip,(struct in_addr *)&ipaddr);

	if ((tnetc_pcb = tcp_new()) == 0) {
		mon_printf("TELNETC: tcp_new() returned NULL\n");
		return(-1);
	}

	if (mon_getenv("TELNETC_DEBUG"))
		tnetc_debug = 1;
	else
		tnetc_debug = 0;

	tcp_arg(tnetc_pcb,NULL);
	tcp_err(tnetc_pcb, telnetc_err);
	tcp_sent(tnetc_pcb, telnetc_sent);
	tcp_recv(tnetc_pcb, telnetc_recv);
	tcp_connect(tnetc_pcb, &ipaddr, port, telnetc_connected);
	return(0);
}

