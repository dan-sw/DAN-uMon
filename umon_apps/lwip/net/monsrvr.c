/* monsrvr.c:
 *
 * This code is derived from a udp echo server snippet of code
 * from the LWIP mail archive.  It's a server sitting on port
 * 777 waiting for incoming UDP commands.  For each incoming line,
 * it passes that line to mon_docommand() and while mon_docommand()
 * is active, the output from uMon is redirected to this UDP session.
 */
#include "lwip/udp.h"
#include "lwip/pbuf.h"
#include "monlib.h"

static int monsrvrLidx;
static char monsrvrCmd[64];
static u16_t monsrvrRespPort;
static struct udp_pcb *monsrvrPcb;
static struct ip_addr monsrvrRespAddr;
static char monsrvrRespLine[128];

int
sendudp(char *data, int len, struct udp_pcb *pcb, struct ip_addr *addr, u16_t port)
{
	char *bp;
	int i, j;
	err_t err;
	struct pbuf *p, *q;

	p = pbuf_alloc(PBUF_TRANSPORT,len,PBUF_POOL);
	if (p == NULL) {
		mon_printf("sendudp: pbuf_alloc failed\n");
		return(-1);
	}

	j = 0;
	for(q = p; q != NULL; q = q->next) {
		bp = (char *)(q->payload);
		for(i=0;i<q->len;i++) {
			if (j >= len) {
				mon_printf("sendudp: length confusion\n");
				break;
			}
			bp[i] = data[j++];
		}
	}

	err = udp_sendto(pcb, p, addr, port);
	pbuf_free(p);

	if (err != ERR_OK) {
		mon_printf("sendudp: udp_sendto failed (%d)\n",err);
		return(-1);
	}
	return(0);
}

/* udp_putchar():
 * While mon_docommand() is active, this function is used by uMon as the
 * putchar replacement. It simply queues up characters until a newline
 * is received, then it sends that entire line out the UDP connection
 * established by the monsrvr variables.
 */
int
udp_putchar(char c)
{
	monsrvrRespLine[monsrvrLidx++] = c;
	if ((monsrvrLidx == sizeof(monsrvrRespLine)) || (c == '\n')) {
		sendudp(monsrvrRespLine,monsrvrLidx,monsrvrPcb,&monsrvrRespAddr,monsrvrRespPort);
		monsrvrLidx = 0;
	}
	return(1);
}

/* monsrvr_recv():
 * This is the function that is called when the stack receives a UDP
 * packet destined for port 777.  It is treated as a command line destined
 * for execution by mon_docommand.
 */
void
monsrvr_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, struct ip_addr *addr, u16_t port)
{
    int i, j;
    struct pbuf *q;

    if (p != NULL) {
    	j = 0;
		monsrvrRespAddr = *addr;

		/* Pull the message off the pbuf, then send it to
		 * mon_docommand()...
		 */
        for(q = p; q != NULL; q = q->next) {
          for(i=0;i<q->len;i++) {
			if (j < sizeof(monsrvrCmd)-3)
            	monsrvrCmd[j++] = ((char *)(q->payload))[i];
		  }
        }
		pbuf_free(p);

		monsrvrCmd[j] = 0;
		mon_printf("REMOTE_CMD (from %d.%d.%d.%d): '%s'\n", ip4_addr1(addr),
			ip4_addr2(addr), ip4_addr3(addr), ip4_addr4(addr),monsrvrCmd);

		monsrvrPcb = pcb;
		monsrvrRespPort = port;
		mon_com(CHARFUNC_PUTCHAR,udp_putchar,0,0);
		mon_docommand(monsrvrCmd,1);
		mon_com(CHARFUNC_PUTCHAR,0,0,0);
		sendudp("",1,pcb,&monsrvrRespAddr,port);
    }
}


/* monsrvr_init():
 * This does the equivalent of setting up a UDP server on port 777.
 * The function monsrvr_recv() is the callback function that LWIP
 * notifies each time an incoming packet is received on port 777.
 */
void
monsrvr_init(void)
{
    /* get new pcb */
    monsrvrPcb = udp_new();
    if (monsrvrPcb == NULL) {
        mon_printf("monsrvr_init: udp_new failed!\n");
        return;
    }

    /* bind to any IP address on port 777 */
    if (udp_bind(monsrvrPcb, IP_ADDR_ANY, 777) != ERR_OK) {
        mon_printf("monsrvr_init: udp_bind failed!\n");
        return;
    }

	monsrvrLidx = 0;

    /* set monsrvr_recv() as callback function for received packets */
    udp_recv(monsrvrPcb, monsrvr_recv, NULL);
}
