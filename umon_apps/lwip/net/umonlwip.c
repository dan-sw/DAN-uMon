/* 
 * umonlwip.c:
 * This is the code that connects a simple uMon demo application
 * to LWIP.  This code was started based on LWIP 1.3, referring to
 * the LWIP Wiki, the document lwip/docs/rawapi.txt.
 */

#include <string.h>
#include <stdlib.h>
#include "monlib.h"
#include "tfs.h"
#include "cfg.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "netif/etharp.h"
#include "lwip/init.h"
#include "lwip/ip.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "lwip/inet.h"
#include "lwip/tcpip.h"
#include "lwip/netif.h"
#include "lwip/stats.h"
#include "umonlwip.h"
#if !CLOCK_MS
#include "timer.h"
#endif

struct ethernetif {
  struct eth_addr *ethaddr;
};

extern unsigned long clock_time(void);

unsigned long AppStack[APPSTACKSIZE/4];

long opacket[1600/4], ipacket[1600/4];

struct netif mynetif, *mynetifp;

#if !CLOCK_MS
static unsigned long msec250;	/* Number of timer ticks equivalent to
								 * 250 milliseconds.
								 */
#endif

/* EtherToBin():
 *  Convert ascii MAC address string to binary.
 */
int
EtherToBin(char *ascii,unsigned char *binary)
{
    int i, digit;
    char    *acpy;

    acpy = ascii;
    for(i=0;i<6;i++) {
        digit = (int)strtol(acpy,&acpy,16);
        if (((i != 5) && (*acpy++ != ':')) ||
            ((i == 5) && (*acpy != 0)) ||
            (digit < 0) || (digit > 255)) {
            mon_printf("Misformed ethernet addr at: 0x%lx\n",(long)ascii);
            return(-1);
        }
        binary[i] = (unsigned char)digit;
    }
    return(0);
}

static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
  int plen = 0;
  struct pbuf *q;
  u8_t *ebase, *ebuf;

  /* initiate transfer... */
  ebuf = ebase = (u8_t *)opacket;

#if ETH_PAD_SIZE
  pbuf_header(p, -ETH_PAD_SIZE);            /* drop the padding word */
#endif

  for(q = p; q != NULL; q = q->next) {
    /* Send the data from the pbuf to the interface, one pbuf at a
       time. The size of the data in each pbuf is kept in the ->len
       variable. */

    memcpy((char *)ebuf,(char *)q->payload,(int)q->len);
    plen += q->len;
    ebuf += q->len;
  }

  /* signal that packet should be sent... */
  mon_sendenetpkt((char *)opacket,plen);

#if ETH_PAD_SIZE
  pbuf_header(p, ETH_PAD_SIZE);         /* reclaim the padding word */
#endif

//#if LINK_STATS
//  lwip_stats.link.xmit++;
//#endif /* LINK_STATS */

  return ERR_OK;
}

err_t
my_netif_init(struct netif *netif)
{
  struct ethernetif *ethernetif;

  if (EtherToBin(mon_getenv("ETHERADD"),netif->hwaddr) < 0)
		mon_appexit(1);

  netif->hwaddr_len = 6;
  LWIP_ASSERT("netif != NULL", (netif != NULL));

  ethernetif = mem_malloc(sizeof(struct ethernetif));
  if (ethernetif == NULL) {
    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
    return ERR_MEM;
  }

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
  NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, ???);
   */

  netif->state = ethernetif;
  netif->name[0] = 'b';
  netif->name[1] = 'f';

  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  netif->output = etharp_output;
  netif->linkoutput = low_level_output;

  ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);

  return ERR_OK;
}

/* lwip_umon_startup():
 * This should be the first lwip function called.  It hooks up the
 * LWIP environment to MicroMonitor, plus it does all the initial
 * LWIP initialization.
 */
int
lwip_umon_startup(void)
{
	char *envgw, *envip, *envnm;
	struct ip_addr ip_ipa, ip_nm, ip_gw;
	struct in_addr in_ipa, in_nm, in_gw;
#if !CLOCK_MS 
	struct elapsed_tmr tmr;
#endif

	/* Tell uMon's underlying ethernet driver not to poll the
	 * interface; thus allowing this application (only) to do
	 * the polling.
	 */
	mon_sendenetpkt(0,0);

	/* Set up basic network information as it exists in the environment
	 * already set up in uMon...
	 */
    envgw = mon_getenv("GIPADD");
    envip = mon_getenv("IPADD");
    envnm = mon_getenv("NETMASK");
	if ((envgw == (char *)0) || (envip == (char *)0) || (envnm == (char *)0)) {
        mon_printf("Network parameters unavailable\n");
        return(-1);
	}
	if (!inet_aton(envgw,&in_gw)) {
		mon_printf("GIPADD bad syntax: '%s'\n",envgw);
		return(-1);
	}

	if (!inet_aton(envip,&in_ipa)) {
		mon_printf("IPADD bad syntax: '%s'\n",envip);
		return(-1);
	}

	if (!inet_aton(envnm,&in_nm)) {
		mon_printf("NETMASK bad syntax: '%s'\n",envnm);
		return(-1);
	}

	ip_ipa.addr = in_ipa.s_addr;
	ip_nm.addr = in_nm.s_addr;
	ip_gw.addr = in_gw.s_addr;

	/* Following the recommendations of the rawapi.txt document the
	 * following init calls are made.  As of LWIP 1.3.0 (from which
	 * this demo was originally built) most of these are empty stubs;
	 * however, text in rawapi.txt recommends that they be made here
	 * for future compatibility.
	 */
	stats_init()
	sys_init();
	mem_init();
	memp_init();
	pbuf_init();
	etharp_init();
	ip_init();
	udp_init();
	tcp_init();

	mynetifp = netif_add(&mynetif,&ip_ipa,&ip_nm,&ip_gw,0,
		my_netif_init,ethernet_input);

	netif_set_default(mynetifp);
	netif_set_up(mynetifp);
//	dhcp_start(mynetifp);

	mon_printf("Network interface configured...\n");
	mon_printf("  IP: %s\n",envip);
	mon_printf("  GW: %s\n",envgw);
	mon_printf("  NM: %s\n",envnm);

#if !CLOCK_MS 
	mon_timer(TIMER_QUERY,&tmr);
	msec250 = tmr.tpm * 250;
#endif

	return(0);
}

int
uMonRand(void)
{
#if CLOCK_MS
	return((int)clock_time());
#else
	struct elapsed_tmr tmr;
	mon_timer(TIMER_QUERY,&tmr);
	return((int)tmr.currenttmrval);
#endif
}

/* lwip_umon_shutdown():
 * When this app started using uMon's underlying ethernet interface, 
 * it had to start off by disabling uMon's underlying mechanism to poll
 * the ethernet interface (so that this application can do it).
 * 
 * This function must be called prior to exiting the the application (back
 * to the monitor) so that ethernet polling intrinsically done by the monitor
 * is turned back on.
 */
void
lwip_umon_shutdown(void)
{
	mon_sendenetpkt(0,-1);
}

/* lwip_poll():
 * The recommended "OS-less" code, based on text in the LWIP wiki page...
 *  http://lwip.scribblewiki.com/LwIP_with_or_without_an_operating_system
 */
void
lwip_poll(void)
{
	int len;
	char *ipkt;
	struct pbuf *p, *q;
	static unsigned long last_time = 0;
#if !CLOCK_MS 
	struct elapsed_tmr tmr, tmr1;
#endif

	ipkt = (char *)ipacket;
	len = mon_recvenetpkt((char *)ipacket,sizeof(ipacket));
  	if (len) {
#if ETH_PAD_SIZE
		len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif
		p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
		if (p) {
#if ETH_PAD_SIZE
			pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif
    		for(q = p; q != NULL; q = q->next) {
				memcpy(q->payload,ipkt, q->len);
				ipkt += q->len;
			}
#if ETH_PAD_SIZE
			pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif
			ethernet_input(p, mynetifp);
			LINK_STATS_INC(link.recv);
		}
		else {
			LINK_STATS_INC(link.memerr);
			LINK_STATS_INC(link.drop);
		}
	}
#if CLOCK_MS
	if((clock_time() - last_time) >= (250 * CLOCK_MS)) {
#else
	mon_timer(TIMER_QUERY,&tmr);
	if((tmr.currenttmrval - last_time) >= msec250) {
#endif
		static int count = 0;
		count++;
		if (count == 20) {	/* Call etharp_tmr() every 5 seconds. */
			etharp_tmr();
			count = 0;
		}
		tcp_tmr();
#if CLOCK_MS
		last_time = clock_time();
#else
		mon_timer(TIMER_QUERY,&tmr1);
		last_time = tmr1.currenttmrval;
#endif
	}    
}
