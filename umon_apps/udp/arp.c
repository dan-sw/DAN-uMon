#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "monlib.h"
#include "unet.h"

typedef unsigned long ulong;
typedef unsigned char uchar;
typedef unsigned short ushort;

/* EtherFromCache():
 *	Called with a binary (4-byte) ip address.  If a match is found
 *	in the cache, return a pointer to that ethernet address; else
 *	return NULL.
 */
uchar *
EtherFromCache(uchar *ip)
{
	int	i;

	for(i=0;i<SIZEOFARPCACHE;i++) {
		if (!memcmp((char *)ArpCache[i].ip, (char *)ip,4))
			return(ArpCache[i].ether);
	}
	return(0);
}

/* ArpStore():
 *	Called with binary ip and ethernet addresses.
 *	It will store that set away in the cache.
 *	Nothing fancy about the arpcache.  Just an indexed ciruclar buffer.
 */
int
ArpStore(uchar *ip,uchar *ether)
{
	static int idx = 0;

	if (EtherFromCache(ip))
		return(0);

	if (++idx >= SIZEOFARPCACHE)
		idx=0;

	memcpy((char *)ArpCache[idx].ip,(char *)ip,4);
	memcpy((char *)ArpCache[idx].ether,(char *)ether,6);
	return(0);
}

/* IpIsOnThisNet():
 *	Return 1 if the incoming ip is on this subnet; else 0.
 *	For each bit in the netmask that is set to 1...
 *	If the corresponding bits in the incoming IP and this board's IP
 *	do not match, return 0; else return 1.
 */
int
IpIsOnThisNet(uchar *ip)
{
	int		i;
	uchar	*nm, *myip;

	nm = (uchar *)&thisnm.s_addr;
	myip = (uchar *)&thisip.s_addr;

	for(i=0;i<4;i++) {
		if ((ip[i] & nm[i]) != (myip[i] & nm[i])) {
			return(0);
		}
	}
	return(1);
}

/* SendArpResp():
 *	Called in response to an ARP REQUEST.  The incoming ethernet
 *	header pointer points to the memory area that contains the incoming
 *	ethernet packet that this function is called in response to.
 *	In other words, it is used to fill the majority of the response
 *	packet fields.
 */
int
SendArpResp(struct ether_header *re)
{
	struct arphdr *ta, *ra;
	struct ether_header *te;
	unsigned long arppkt[ARPSIZE/2];

	te = (struct ether_header *) arppkt;
	memcpy((char *)&(te->ether_shost), (char *)thismac.ether_addr_octet,6);
	memcpy((char *)&(te->ether_dhost),(char *)&(re->ether_shost),6);
	te->ether_type = ecs(ETHERTYPE_ARP);

	ta = (struct arphdr *) (te + 1);
	ra = (struct arphdr *) (re + 1);
	ta->hardware = ra->hardware;
	ta->protocol = ra->protocol;
	ta->hlen = ra->hlen;
	ta->plen = ra->plen;
	ta->operation = ARP_RESPONSE;
	memcpy((char *)ta->senderha, (char *)thismac.ether_addr_octet,6);
	memcpy((char *)ta->senderia, (char *)ra->targetia,4);
	memcpy((char *)ta->targetha, (char *)ra->senderha,6);
	memcpy((char *)ta->targetia, (char *)ra->senderia,4);
	self_ecs(ta->hardware);
	self_ecs(ta->protocol);
	self_ecs(ta->operation);
	mon_sendenetpkt((char *)arppkt,ARPSIZE);
	return(0);
}

/* SendArpRequest():
 */
int
SendArpRequest(uchar *ip)
{
	struct arphdr *ta;
	struct ether_header *te;
	unsigned long arppkt[ARPSIZE/2];

	/* Populate the ethernet header: */
	te = (struct ether_header *) arppkt;
	memcpy((char *)&(te->ether_shost), (char *)thismac.ether_addr_octet,6);
	memcpy((char *)&(te->ether_dhost), (char *)BroadcastAddr,6);
	te->ether_type = ecs(ETHERTYPE_ARP);

	/* Populate the arp header: */
	ta = (struct arphdr *) (te + 1);
	ta->hardware = ecs(1);		/* 1 for ethernet */
	ta->protocol = ecs(ETHERTYPE_IP);
	ta->hlen = 6;		/* Length of hdware (ethernet) address */
	ta->plen = 4;		/* Length of protocol (ip) address */
	ta->operation = ecs(ARP_REQUEST);
	memcpy((char *)ta->senderha, (char *)thismac.ether_addr_octet,6);
	memcpy((char *)ta->senderia, (char *)&thisip.s_addr,4);
	memcpy((char *)ta->targetha, (char *)BroadcastAddr,6);
	memcpy((char *)ta->targetia, (char *)ip,4);
	mon_sendenetpkt((char *)arppkt,ARPSIZE);
	return(0);
}

/* ArpEther():
 *	Retrieve the ethernet address that corresponds to the incoming IP 
 *	address.  First check the local ArpCache[], then if not found issue
 *	an ARP request... If the incoming IP is on this net, then issue the
 *	request for the MAC address of that IP; otherwise, issue the request
 *	for this net's GATEWAY.
 */

uchar *
ArpEther(struct udpinfo *u, uchar *ecpy)
{
	struct 	ether_header *ehdr;
	uchar	*binip, *gbinip, *Ip, *ep;
	int		size, retry, timeoutsecs;

	binip = (uchar *)&u->dip.s_addr;
	gbinip = (uchar *)&thisgip.s_addr;

	/* First check local cache. If found, return with pointer to MAC. */
	ep = EtherFromCache(binip);
	if (ep) {
		if (ecpy) {
			memcpy((char *)ecpy, (char *)ep,6);
			ep = ecpy;
		}
		return(ep);
	}

	retry = 0;
	while(1) {
		/* If IP is not on this net, the get the GATEWAY IP address.
		 */
		if (!IpIsOnThisNet(binip)) {
			ep = EtherFromCache(gbinip);
			if (ep) {
				if (ecpy) {
					memcpy((char *)ecpy, (char *)ep,6);
					ep = ecpy;
				}
				return(ep);
			}
			SendArpRequest(gbinip);
			Ip = gbinip;
		}
		else {
			SendArpRequest(binip);
			Ip = binip;
		}

		/* Now that the request has been issued, wait for a while to see if
		 * the ARP cache is loaded with the result of the request.
		 */
		for(timeoutsecs=0;timeoutsecs<1000000;timeoutsecs++) {
			ep = EtherFromCache(Ip);
			if (ep)
				break;
			size = mon_recvenetpkt(u->packet,u->plen);
			if (size == 0)
				continue;
			ehdr = (struct ether_header *)u->packet;
			if (!memcmp(ehdr->ether_shost.ether_addr_octet,
				thismac.ether_addr_octet,6)) {
				continue;
			}
			if (ehdr->ether_type == ecs(ETHERTYPE_ARP)) {
				processARP(ehdr,size);
			}
		}
		if (ep) {
			if (ecpy) {
				memcpy((char *)ecpy, (char *)ep,6);
				ep = ecpy;
			}
			return(ep);
		}
		retry++;
	}
	return(0);
}

/* processARP();
 */
int
processARP(struct ether_header *ehdr,ushort size)
{
	struct	arphdr *arpp;

	arpp = (struct arphdr *)(ehdr+1);
	self_ecs(arpp->hardware);
	self_ecs(arpp->protocol);
	self_ecs(arpp->operation);

	switch(arpp->operation) {
	case ARP_REQUEST:
		if (!memcmp((char *)arpp->targetia, (char *)&thisip.s_addr,4)) {
			ArpStore(arpp->senderia,arpp->senderha);
			SendArpResp(ehdr);
		}
		break;
	case ARP_RESPONSE:
		if (!memcmp((char *)arpp->targetia, (char *)&thisip.s_addr,4)) {
			ArpStore(arpp->senderia,arpp->senderha);
		}
		break;
	default:
		mon_printf("  Invalid ARP operation: 0x%x\n",arpp->operation);
		return(0);
	}
	return(1);
}
