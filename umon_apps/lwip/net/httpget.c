#include "lwip/debug.h"
#include "lwip/stats.h"
#include "httpd.h"
#include "lwip/tcp.h"
#include "monlib.h"

static int rcvlen, get_state;
static struct tcp_pcb *get_pcb;
static char *get_dest, get_line[128];
static int debug;

/* This code is based on a few different lwip snippets I found...
 * 1. contrib/ports/unix/proj/minimal/echo.c 
 * 2. http://www.mail-archive.com/lwip-users@nongnu.org/msg04160.html
 * 3. http://www.mail-archive.com/lwip-users@nongnu.org/msg04689.html
 */

int
httpget_isactive(void)
{
	return(get_state);
}

void
httpget_close(void)
{
	tcp_arg(get_pcb, NULL);
	tcp_sent(get_pcb, NULL);
	tcp_recv(get_pcb, NULL);
	tcp_err(get_pcb, NULL);
	tcp_poll(get_pcb, NULL, 0);
	tcp_close(get_pcb);
	get_state = 0;
}

err_t
httpget_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
	struct pbuf *q;
	extern void ticktock(void);

	if (err != ERR_OK) {
		mon_printf("\nHTTPGET: recv failed (err=%d)\n",err);
		httpget_close();
		return(err);
	}
	if (p == NULL) {
		char buf[16];

		/* If we're here, the remote host closed the connection.
		 */
		httpget_close();
		mon_printf("\nServer closed connection, total bytes rcvd: %d\n",
			rcvlen);
		mon_sprintf(buf,"%d",rcvlen);
		mon_setenv("HTTPGET",buf);
		return(ERR_OK);
	}
	for(q = p; q != NULL; q = q->next) {
		memcpy((char *)get_dest,(char *)q->payload,(int)q->len);
		rcvlen += q->len;
		get_dest += q->len;
	}
	tcp_recved(tpcb,p->tot_len);
	pbuf_free(p);
	if (debug)
		mon_printf("HTTPGET: recv tot: %d\n",rcvlen);
	else 
		mon_printf("%08d\b\b\b\b\b\b\b\b",rcvlen);
	return(ERR_OK);
}


err_t
httpget_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	if (debug)
		mon_printf("HTTPGET: msg sent (size=%d)\n",len);
	return(ERR_OK);
}

err_t
httpget_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
	if (err != ERR_OK) {
		mon_printf("HTTPGET: connect failed (err=%d)\n",err);
		httpget_close();
	}
	else {
		int size, maxsize;

		if (debug)
			mon_printf("HTTPGET: connected\n");

		maxsize = tcp_sndbuf(tpcb);
		size = strlen(get_line);
		if (debug)
			mon_printf("MSG(%d): %s \n",size,get_line);
		if (size > maxsize) {
			mon_printf("HTTPGET: msg too big\n");
			return(ERR_BUF);
		}
		else {
			err = tcp_write(tpcb, get_line, size, 0);
			if (err != ERR_OK)
				mon_printf("HTTPGET: tcp_write failed\n");
			else
				tcp_output(tpcb);
		}
	}
	return(err);
}

void
httpget_err(void *arg, err_t err)
{
	mon_printf("\nHTTPGET: error %d\n",err);
	httpget_close();
}

/* Regarding a proxy...
 * When a proxy is involved, all outgoing client requests must pass
 * through that proxy.  So, the connection request must be made to
 * the proxy, and the path of the file to be retrieved must be 
 * preceded by the full network path of the server containing
 * the file.  For example...
 *
 *  GET http://www.yada.com/fullpath.html HTTP/1.0
 *
 */
int
httpget_init(char *srvrip, char *file, char *addr)
{
	struct ip_addr ipaddr;

	mon_setenv("HTTPGET",0);

	if ((strlen(file) + 32) >= sizeof(get_line)) {
		mon_printf("get_line[] overflow\n");
		return(-1);
	}

	rcvlen = 0;
	get_state = 1;
	get_dest = addr;
	mon_sprintf(get_line,"GET %s HTTP/1.0\n\n",file);
	inet_aton((const char *)srvrip,(struct in_addr *)&ipaddr);

	if ((get_pcb = tcp_new()) == 0) {
		mon_printf("tcp_new() returned NULL\n");
		return(-1);
	}

	if (mon_getenv("HTTPGET_DEBUG"))
		debug = 1;
	else
		debug = 0;

	tcp_arg(get_pcb,NULL);
	tcp_err(get_pcb, httpget_err);
	tcp_sent(get_pcb, httpget_sent);
	tcp_recv(get_pcb, httpget_recv);
	tcp_connect(get_pcb, &ipaddr, 80, httpget_connected);
	return(0);
}

