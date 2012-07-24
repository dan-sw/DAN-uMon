#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "monlib.h"
#include "unet.h"

typedef unsigned long ulong;
typedef unsigned char uchar;
typedef unsigned short ushort;

/*	udpChksum():
 *	Compute the checksum of the UDP packet.
 *	The incoming pointer is to an ip header, the udp header after that ip
 *	header is directly populated with the result.
 *	Got part of this code out of Steven's TCP/IP Illustrated Volume 2.
 */
void
udpChksum(struct ip *ihdr)
{
	register int	i;
	register ushort	*datap;
	register long	sum;
	int	 	len;
	struct	Udphdr *uhdr;
	struct	UdpPseudohdr	pseudohdr;

	uhdr = (struct Udphdr *)(ihdr+1);
	uhdr->uh_sum = 0;

	/* Note that optionally, the checksum can be forced to zero here,
	 * so a return could replace the remaining code.
	 * That would be kinda dangerous, but it is an option according to
	 * the spec.
	 */
#ifdef UDPCHKSUM_ZERO
	return;
#endif

	/* Start with the checksum of the pseudo header:
	 * Note that we have to use memcpy because we don't know if the incoming
	 * stream is aligned properly.
	 */
	memcpy((char *)&pseudohdr.ip_src.s_addr,(char *)&ihdr->ip_src.s_addr,4);
	memcpy((char *)&pseudohdr.ip_dst.s_addr,(char *)&ihdr->ip_dst.s_addr,4);
	pseudohdr.zero = 0;
	pseudohdr.proto = ihdr->ip_p;
	pseudohdr.ulen = uhdr->uh_ulen;

	/* Get checksum of pseudo header: */
	sum = 0;
	datap = (ushort *) &pseudohdr;
	for (i=0;i<(sizeof(struct UdpPseudohdr)/sizeof(ushort));i++) {
		sum += *datap++;
		if (sum & 0x80000000)
			sum = (sum & 0xffff) + (sum >> 16);
	}

	len = ecs(uhdr->uh_ulen);
	datap = (ushort *) uhdr;

	/* If length is odd, pad with zero and add 1... */
	if (len & 1) {
		uchar	*ucp;
		ucp = (uchar *)uhdr;
		ucp[len] = 0;
		len++;
	}

	while(len) {
		sum += *datap++;
		if (sum & 0x80000000)
			sum = (sum & 0xffff) + (sum >> 16);
		len -= 2;
	}

	while(sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	uhdr->uh_sum = ~sum;
}

/* udpSendTo():
 * Send a UDP packet.
 * The incoming udpinfo structure is assumed to have...
 *  u->dport loaded with the destination port; 
 *	u->dip.s_addr loaded with the destination IP address;
 *	u->packet pointing to the buffer to be used to build the packet;
 *	u->plen size of the packet buffer;
 *	u->udata pointing to the udp data;
 *	u->ulen loaded with the size of the udp data pointed to be u->data.
 *
 * If successful, a positive number is returned, and it represents the
 * size of the full ethernet packet sent out.  If unsuccessful, then
 * a negative value is returned, and it represents some UDP error code
 * that can be displayed using the udpErr2Str() function above.
 */
int
udpSendTo(struct udpinfo *u)
{
	uchar	*ep;
	int		pktlen;
	struct	ip *ipp;
	struct	Udphdr *udpp;
	ushort	ip_len, sport;
	struct	ether_addr thatmac;
	struct	ether_header *enetp;

	/* Get the ethernet address for the destination IP:
	 * First look in the local arp cache, then if not found there, issue
	 * an ARP request...
	 */
	ep = EtherFromCache((uchar *)&u->dip.s_addr);
	if (ep) {
		memcpy((char *)thatmac.ether_addr_octet,ep,6);
	}
	else {
		if (ArpEther(u,(uchar *)thatmac.ether_addr_octet) < 0)
			return(UNETERR_ARPFAIL);
	}

	/* Populate the ethernet level of packet:
	 */
	enetp = (struct ether_header *)u->packet;
	memcpy((char *)&enetp->ether_shost,(char *)thismac.ether_addr_octet,6);
	memcpy((char *)&enetp->ether_dhost,(char *)thatmac.ether_addr_octet,6);
	enetp->ether_type = ecs(ETHERTYPE_IP);

	/* Move to the IP portion of the packet and populate it
	 * appropriately:
	 */
	ipp = (struct ip *) (enetp + 1);
	ipp->ip_vhl = IP_HDR_VER_LEN;
	ipp->ip_tos = 0;
	ipp->ip_id = ipId();
	ip_len = sizeof(struct ip) + sizeof(struct Udphdr) + u->ulen;
	ipp->ip_len = ecs(ip_len);
	ipp->ip_off = 0;
	ipp->ip_ttl = UDP_TTL;
	ipp->ip_p = IP_UDP;
	ipp->ip_src.s_addr = thisip.s_addr;
	ipp->ip_dst.s_addr = ecl(u->dip.s_addr);

	/* Now UDP header...
	 */
	sport = u->dport+1;
	udpp = (struct Udphdr *) (ipp + 1);
	udpp->uh_sport = ecs(sport);
	udpp->uh_dport = ecs(u->dport);
	udpp->uh_ulen = ecs((ushort)(ip_len - sizeof(struct ip)));

	/* Finally, the  UDP data...
	 */
	memcpy((char *)(udpp+1),u->udata,u->ulen);

	ipChksum(ipp);			/* Compute csum of ip hdr */
	udpChksum(ipp);			/* Compute UDP checksum */

	pktlen = u->ulen + ETHERSIZE + IPSIZE + UDPSIZE;

	mon_sendenetpkt((char *)u->packet,pktlen);
	return(pktlen);
}

/* udpRecvFrom():
 * This function retrieves the next incoming packet from ethernet.
 * If the incoming packet is an ARP request for this board, then it
 * will respond with the appropriate ARP response.
 *
 * The incoming udpinfo structure is assumed to have...
 *  u->dport loaded with the port that is to receive the data
 *		(if this is zero, then the incoming port is ignored);
 *	u->packet pointing to a buffer into which the packet can be placed;
 *	u->plen loaded with the size of the udp data buffer;
 *
 * Upon completion the following data is returned...
 *  u->ulen will contain the size of the incoming UDP data;
 *  u->data will point to the udp data area of the received packet;
 *  u->sport is loaded with the port of the sender of the data;
 *  u->sip is loaded with the ip address of the sender of the data;
 *
 * This function does not block.  The return value for this function is
 * zero if no packet is received; negative if there is an error and 
 * positive if a packet is received.  In which case, the positive value
 * also contains the size of the data.
 */
int
udpRecvFrom(struct udpinfo *u)
{
	ushort	*datap;
	ulong	csum, ip;
	struct	ip *ihdr;
	struct	Udphdr *uhdr;
	struct	ether_header *ehdr;
	int		i, size, udplen;

	/* See if there is an incoming packet.  If not, just return 0.
	 * If yes, then parse the packet to see if it is what the caller
	 * is waiting for.
	 */
top:
	size = mon_recvenetpkt(u->packet,u->plen);
	if (size == 0)
		return(0);

	ehdr = (struct ether_header *)u->packet;

	/* If the incoming packet's source MAC is this board, then assume
	 * we're just receiving our own full-duplex transmission and allow
	 * for one more attempt...
	 */
	if (!memcmp(ehdr->ether_shost.ether_addr_octet,thismac.ether_addr_octet,6))
		goto top;

	/* Check if the incoming packet is ARP.  If yes, then repsond to it
	 * if it is an ARP to our IP...
	 */
	if (ehdr->ether_type == ecs(ETHERTYPE_ARP)) {
		struct	arphdr *arpp;

		arpp = (struct arphdr *)(ehdr+1);
		if (arpp->operation == ecs(ARP_REQUEST)) {
			if (memcmp(arpp->targetia,(char *)&thisip.s_addr,4) == 0) 
				processARP(ehdr,size);
		}
		return(0);
	}

	if (ehdr->ether_type != ecs(ETHERTYPE_IP))
		return(0);

	ihdr = (struct ip *) (ehdr + 1);

	/* If not version 4 or if not UDP or ICMP, return now... */
	if ((getIP_V(ihdr->ip_vhl) != 4) ||
			((ihdr->ip_p != IP_UDP) && (ihdr->ip_p != IP_ICMP))) {
		return(0);
	}

	/* Verify destination and (possibly) source IP addresses...
	 */
	if (ihdr->ip_dst.s_addr != thisip.s_addr)
		return(0);

	/* Verify incoming IP header checksum...
	 */
	csum = 0;
	datap = (ushort *) ihdr;
	for (i=0;i<(sizeof(struct ip)/sizeof(ushort));i++,datap++)
		csum += *datap;
	csum = (csum & 0xffff) + (csum >> 16);
	if (csum != 0xffff) 
		return(-2);

	if (ihdr->ip_p == IP_ICMP) {
		processICMP(ehdr,size);
		return(0);
	}

	uhdr = (struct Udphdr *)(ihdr+1);

	/* Verify incoming port number...
	 * Note that if the u->dport passed into this function is ANY_UDP_PORT,
	 * then we store the incoming port number and process the incoming packet
	 * regardless of the port number.
	 */
	if (u->dport == ANY_UDP_PORT) {
		u->dport = ecs(uhdr->uh_dport);
	}
	else if (uhdr->uh_dport != ecs(u->dport)) {
		return(0);
	}

	udplen = ecs(uhdr->uh_ulen);

	/* If non-zero, verify incoming UDP packet checksum...
	 */
	if (uhdr->uh_sum) {
		int	 	len;
		struct	UdpPseudohdr	pseudohdr;

		memcpy((char *)&pseudohdr.ip_src.s_addr,(char *)&ihdr->ip_src.s_addr,4);
		memcpy((char *)&pseudohdr.ip_dst.s_addr,(char *)&ihdr->ip_dst.s_addr,4);
		pseudohdr.zero = 0;
		pseudohdr.proto = ihdr->ip_p;
		pseudohdr.ulen = uhdr->uh_ulen;

		csum = 0;
		datap = (ushort *) &pseudohdr;
		for (i=0;i<(sizeof(struct UdpPseudohdr)/sizeof(ushort));i++)
			csum += *datap++;

		/* If length is odd, pad and add one. */
		len = udplen;
		if (len & 1) {
			uchar	*ucp;
			ucp = (uchar *)uhdr;
			ucp[len] = 0;
			len++;
		}
		len >>= 1;

		datap = (ushort *) uhdr;
		for (i=0;i<len;i++)
			csum += *datap++;
		csum = (csum & 0xffff) + (csum >> 16);
		if (csum != 0xffff) 
			return(0);
	}
	udplen -= sizeof(struct Udphdr);

	ip = ecl(ihdr->ip_src.s_addr);

	ArpStore((uchar *)&ip,ehdr->ether_shost.ether_addr_octet);

	/* All done checking out the packet, now just copy the UDP payload
	 * into the message buffer and return the size...
	 */
	u->ulen = udplen;
	u->sport = ecs(uhdr->uh_sport);
	u->sip.s_addr = ecl(ihdr->ip_src.s_addr);
	u->udata = (char *)(uhdr+1);
	return(u->ulen);
}

