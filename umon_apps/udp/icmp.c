#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "monlib.h"
#include "unet.h"

typedef unsigned long ulong;
typedef unsigned char uchar;
typedef unsigned short ushort;

/* SendEchoResp():
 * 	Called in response to an ICMP ECHO REQUEST.  Typically used as
 *	a response to a PING.
 */
int
SendEchoResp(struct ether_header *re)
{
	int i, icmp_len, datalen, oddlen;
	ulong	t;
	ushort	*sp, ip_len;
	struct ether_header *te;
	struct ip *ti, *ri;
	struct icmp_echo_hdr *ticmp, *ricmp;
	static ulong icmppkt[64];

	ri = (struct ip *) (re + 1);
	datalen = ecs(ri->ip_len) - ((ri->ip_vhl & 0x0f) * 4);

	te = (struct ether_header *) icmppkt;
	memcpy((char *)&(te->ether_shost), (char *)thismac.ether_addr_octet,6);
	memcpy((char *)&(te->ether_dhost),(char *)&(re->ether_shost),6);
	te->ether_type = ecs(ETHERTYPE_IP);

	ti = (struct ip *) (te + 1);
	ti->ip_vhl = ri->ip_vhl;
	ti->ip_tos = ri->ip_tos;
	ti->ip_len = ri->ip_len;
	ti->ip_id = ipId();
	ti->ip_off = 0;
	ti->ip_ttl = UDP_TTL;
	ti->ip_p = IP_ICMP;
	memcpy((char *)&(ti->ip_src.s_addr),(char *)&(ri->ip_dst.s_addr),
		sizeof(struct in_addr));
	memcpy((char *)&(ti->ip_dst.s_addr),(char *)&(ri->ip_src.s_addr),
		sizeof(struct in_addr));

	ticmp = (struct icmp_echo_hdr *) (ti + 1);
	ricmp = (struct icmp_echo_hdr *) (ri + 1);
	ticmp->type = ICMP_ECHOREPLY;
	ticmp->code = 0;
	ticmp->cksum = 0;
	ticmp->id = ricmp->id;
	ticmp->seq = ricmp->seq;
	memcpy((char *)(ticmp+1),(char *)(ricmp+1),datalen-8);

	ip_len = ecs(ti->ip_len);
	if (ip_len & 1) {		/* If size is odd, temporarily bump up ip_len by */
		oddlen = 1;			/* one and add append a null byte for csum. */ 
		((char *)ti)[ip_len] = 0;
		ip_len++;
	}
	else
		oddlen = 0;
	icmp_len = (ip_len - sizeof(struct ip))/2;

	ipChksum(ti);

	/* compute checksum of icmp message: (3rd Edition Comer pg 126) */
	ticmp->cksum = 0;
	sp = (ushort *) ticmp;
	for (i=0,t=0;i<icmp_len;i++,sp++)
		t += ecs(*sp);
	t = (t & 0xffff) + (t >> 16);
	ticmp->cksum = ~t;

	self_ecs(ticmp->cksum);

	if (oddlen)
		ip_len--;

	mon_sendenetpkt((char *)icmppkt,(sizeof(struct ether_header) + ip_len));
	return(0);
}

int
processICMP(struct ether_header *ehdr,ushort size)
{
	int		len;
	struct	ip *ipp;
	struct	icmp_hdr *icmpp;

	ipp = (struct ip *)(ehdr + 1);

	/* Compute size of ICMP message (IP len - size of IP header): */
	len = ipp->ip_len - ((ipp->ip_vhl & 0x0f) * 4);

	icmpp = (struct icmp_hdr *)((char *)ipp+IP_HLEN(ipp));
	if (icmpp->type == ICMP_ECHOREQUEST) {
		SendEchoResp(ehdr);
		return(1);
	}
	return(0);
}
