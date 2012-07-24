/*
 * Copyright (c) 2001, Adam Dunkels.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Adam Dunkels.
 * 4. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * $Id: main.c,v 1.3 2008-04-11 16:05:25 umon Exp $
 *
 */


#include <stdlib.h>
#include "uip.h"
#include "uip_arp.h"
#include "monlib.h"

#include "timer.h"

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

#ifndef NULL
#define NULL (void *)0
#endif /* NULL */

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
            printf("Misformed ethernet addr at: 0x%lx\n",(long)ascii);
            return(-1);
        }
        binary[i] = (unsigned char)digit;
    }
    return(0);
}

/* IpToDigits():
 *  Convert ascii IP address to 4 individual integers.
 */
int
IpToDigits(char *ascii,int *digits)
{
    int i, digit;
    char    *acpy;

    acpy = ascii;
    for(i=0;i<4;i++) {
        digit = (int)strtol(acpy,&acpy,10);
        if (((i != 3) && (*acpy++ != '.')) ||
            ((i == 3) && (*acpy != 0)) ||
            (digit < 0) || (digit > 255)) {
            printf("Misformed IP addr at 0x%lx\n",(long)ascii);
            return(-1);
        }
        digits[i] = digit;
    }
    return(0);
}

/* devicedriver_send()/ devicedriver_recv():
 * Use uMon's packet interface API (mon_recvenetpkt() and mon_sendenetpkt())
 * to receive and send packets as needed by uIP...
 */
void
devicedriver_send(void)
{
/* The uIP documentation seems to indicate that I should break up the
 * outgoing send calls because the outgoing data may not all be in
 * uip_buf; however, when I do that (as opposed to just sending it 
 * all from uip_buf) things don't work.  So for now, we just call
 * mon_sendenetpkt(uip_buf,uip_len) once (till I understand what's 
 * the right thing to do)...
 * Refer to section 6.5.3.1 of the uIP documentation for info on this.
 */
#if 1
  mon_sendenetpkt((char *)uip_buf,uip_len);
#else
  mon_sendenetpkt((char *)&uip_buf[0], UIP_LLH_LEN);
  if(uip_len <= UIP_LLH_LEN + UIP_TCPIP_HLEN) {
    mon_sendenetpkt((char *)&uip_buf[UIP_LLH_LEN], uip_len - UIP_LLH_LEN);
  } else {
    mon_sendenetpkt((char *)&uip_buf[UIP_LLH_LEN], UIP_TCPIP_HLEN);
    mon_sendenetpkt(uip_appdata, uip_len - UIP_TCPIP_HLEN - UIP_LLH_LEN);
  }
#endif
}

int
devicedriver_recv(void)
{
  return(mon_recvenetpkt((char *)uip_buf,UIP_BUFSIZE));
}

/*---------------------------------------------------------------------------*/
int
main(void)
{
  char *appname;
  int i, digits[4];
  struct uip_eth_addr eaddr;
  uip_ipaddr_t ipaddr;
  struct timer periodic_timer, arp_timer;

  timer_set(&periodic_timer, CLOCK_SECOND / 2);
  timer_set(&arp_timer, CLOCK_SECOND * 10);
  
  /* Tell uMon's underlying ethernet driver not to poll the
   * interface; thus allowing this application (only) to do
   * the polling.
   */
  mon_sendenetpkt(0,0);

  uip_init();

  /* Give uIP the target MAC address...
   */
  if (EtherToBin(mon_getenv("ETHERADD"),eaddr.addr) < 0)
		mon_appexit(1);
  uip_setethaddr(eaddr);

  /* Use the low two bytes of the MAC address for establishing
   * the target's ipid...
   */
  uip_setipid((eaddr.addr[4] << 8) || (eaddr.addr[5]));

  /* Give uIP the target IP address...
   */
  if (IpToDigits(mon_getenv("IPADD"),digits) < 0)
		mon_appexit(1);
  uip_ipaddr(ipaddr, digits[0],digits[1],digits[2],digits[3]);
  uip_sethostaddr(ipaddr);

  /* Give uIP the target Gateway IP address...
   */
  if (IpToDigits(mon_getenv("GIPADD"),digits) < 0)
		mon_appexit(1);
  uip_ipaddr(ipaddr, digits[0],digits[1],digits[2],digits[3]);
  uip_setdraddr(ipaddr);

  /* Give uIP the target NETMASK...
   */
  if (IpToDigits(mon_getenv("NETMASK"),digits) < 0)
		mon_appexit(1);
  uip_ipaddr(ipaddr, digits[0],digits[1],digits[2],digits[3]);
  uip_setnetmask(ipaddr);

#ifdef WEBSERVER_APP
  appname = "webserver";
#elif TELNETD_APP
  appname = "telnetd";
#elif HELLOWORLD_APP
  appname = "hello-world";
#else
#error: Unexpected APP
#endif

  /* Print this banner before calling app-specific init function because
   * in some cases (telnetd) the app will take over the output.
   */
  mon_printf("Running the uIP '%s' application\n",appname);
  mon_printf("Hit any character to terminate this application...\n");

#ifdef WEBSERVER_APP
  httpd_init();
#elif TELNETD_APP
  telnetd_init();
#elif HELLOWORLD_APP
  hello_world_init();
#endif

/*  {
 *     u8_t mac[6] = {1,2,3,4,5,6};
 *     dhcpc_init(&mac, 6);
 *  }
 */

/*  uip_ipaddr(ipaddr, 127,0,0,1);
 *  smtp_configure("localhost", ipaddr);
 *  SMTP_SEND("adam@sics.se", NULL, "uip-testing@example.com",
 *	    "Testing SMTP from uIP",
 *	    "Test message sent by uIP\r\n");
 */

/*
 *    webclient_init();
 *    resolv_init();
 *    uip_ipaddr(ipaddr, 195,54,122,204);
 *    resolv_conf(ipaddr);
 *    resolv_query("www.sics.se");
 */

  
  /* Top-level loop for processing incoming packets:
   */
  while(1) {
    /* Provide an 'escape' back to the uMon prompt...
     */
    if (mon_gotachar())
      mon_appexit(0);

    uip_len = devicedriver_recv();
    if(uip_len > 0) {
      if(BUF->type == htons(UIP_ETHTYPE_IP)) {
	uip_arp_ipin();
	uip_input();
	if(uip_len > 0) {
	  uip_arp_out();
          devicedriver_send();
	}
      } else if(BUF->type == htons(UIP_ETHTYPE_ARP)) {
	uip_arp_arpin();
	if(uip_len > 0) {
          devicedriver_send();
	}
      }

    } else if(timer_expired(&periodic_timer)) {
      timer_reset(&periodic_timer);
      for(i = 0; i < UIP_CONNS; i++) {
	uip_periodic(i);
	if(uip_len > 0) {
	  uip_arp_out();
          devicedriver_send();
	}
      }

#if UIP_UDP
      for(i = 0; i < UIP_UDP_CONNS; i++) {
	uip_udp_periodic(i);
	if(uip_len > 0) {
	  uip_arp_out();
          devicedriver_send();
	}
      }
#endif /* UIP_UDP */
      
      /* Call the ARP timer function every 10 seconds. */
      if(timer_expired(&arp_timer)) {
	timer_reset(&arp_timer);
	uip_arp_timer();
      }
    }
  }
  return 0;
}

/*---------------------------------------------------------------------------*/
void
uip_log(char *m)
{
  printf("uIP log message: %s\n", m);
}

void
resolv_found(char *name, u16_t *ipaddr)
{
  if(ipaddr == NULL) {
    printf("Host '%s' not found.\n", name);
  } else {
    printf("Found name '%s' = %d.%d.%d.%d\n", name,
	   htons(ipaddr[0]) >> 8,
	   htons(ipaddr[0]) & 0xff,
	   htons(ipaddr[1]) >> 8,
	   htons(ipaddr[1]) & 0xff);
    /*    webclient_get("www.sics.se", 80, "/~adam/uip");*/
  }
}

#ifdef __DHCPC_H__
void
dhcpc_configured(const struct dhcpc_state *s)
{
  uip_sethostaddr(s->ipaddr);
  uip_setnetmask(s->netmask);
  uip_setdraddr(s->default_router);
  resolv_conf(s->dnsaddr);
}
#endif /* __DHCPC_H__ */
void
smtp_done(unsigned char code)
{
  printf("SMTP done with code %d\n", code);
}
void
webclient_closed(void)
{
  printf("Webclient: connection closed\n");
}
void
webclient_aborted(void)
{
  printf("Webclient: connection aborted\n");
}
void
webclient_timedout(void)
{
  printf("Webclient: connection timed out\n");
}
void
webclient_connected(void)
{
  printf("Webclient: connected, waiting for data...\n");
}
void
webclient_datahandler(char *data, u16_t len)
{
  printf("Webclient: got %d bytes of data.\n", len);
}

#if CPU_IS_PPC
clock_time_t
clock_time()
{
    volatile unsigned register tbl;

    // Retrieve the current value of the TBL register:
    asm volatile ("mfspr %0, 0x10c" : "=r" (tbl));

    return((clock_time_t)tbl);
}
#elif CPU_IS_MIPS
#error: Gotta write clock_time for MIPS.
#elif CPU_IS_ARM
#error: Gotta write clock_time for ARM.
#elif CPU_IS_BFIN
/* clock_time is in assembler */
#elif CPU_IS_MICROBLAZE
#error: Gotta write clock_time for MICROBLAZE.
#elif CPU_IS_68K
#else
//KS: put here clock_time() implementation
//#error: Invalid CPU specification.
clock_time_t clock_time()
{
    return((clock_time_t)0);
}
#endif

void
tcpip_output()
{
	/* Not sure what this function is for in uip */
	mon_printf("tcpip_output()\n");
}

/*---------------------------------------------------------------------------*/
