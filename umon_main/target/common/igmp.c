/* igmp.c:
 *	Support some basic IGMP stuff.
 *	I wrote this code as an exercise for getting to know basic IGMP.
 *  This code doesn't do much.  It will successfully send a JOIN or LEAVE
 *	but the monitor is not prepared to deal with multicast, so it doesn't
 *	add any real functionality except for the ability to observe IGMP.
 *
 *	General notice:
 *	This code is part of a boot-monitor package developed as a generic base
 *	platform for embedded system designs.  As such, it is likely to be
 *	distributed to various projects beyond the control of the original
 *	author.  Please notify the author of any enhancements made or bugs found
 *	so that all may benefit from the changes.  In addition, notification back
 *	to the author will allow the new user to pick up changes that may have
 *	been made by other users after this version of the code was distributed.
 *
 *	Note1: the majority of this code was edited with 4-space tabs.
 *	Note2: as more and more contributions are accepted, the term "author"
 *		   is becoming a mis-representation of credit.
 *
 *	Original author:	Ed Sutter
 *	Email:				esutter@lucent.com
 *	Phone:				908-582-2351
 */
#include "config.h"
#if INCLUDE_IGMP
#include "endian.h"
#include "genlib.h"
#include "ether.h"
#include "stddefs.h"
#include "cli.h"

int SendIGMP(int type,uchar *groupip);

char *IgmpHelp[] = {
	"IGMP interface",
#if INCLUDE_VERBOSEHELP
	"{operation} [args]",
	"Operations...",
	" join  {IP}",
	" leave {IP}",
#endif
	0,
};

int
Igmp(int argc,char *argv[])
{
	int	opt, verbose = 0;
	uchar binip[8];
	char *operation, *ipadd;

	while ((opt=getopt(argc,argv,"v")) != -1) {
		switch(opt) {
		case 'v':
			verbose = 1;
			break;
		default:
			return(CMD_PARAM_ERROR);
		}
	}

	if (argc != optind + 2)
		return(CMD_PARAM_ERROR);

	operation = argv[optind];
	ipadd = argv[optind+1];
	
	/* Convert IP address to binary: */
	if (IpToBin(ipadd,binip) < 0)
		return(CMD_SUCCESS);

	/* If time or echo, do the common up-front stuff here... */
	if (!strcmp(operation,"join")) { 
		SendIGMP(IGMPTYPE_JOIN,binip);
	}
	else if (!strcmp(operation,"leave")) { 
		SendIGMP(IGMPTYPE_LEAVE,binip);
	}
	else {
		printf("Unrecognized IGMP op: %s\n",operation);
		return(CMD_FAILURE);
	}
	return(CMD_SUCCESS);
}

int
SendIGMP(int type,uchar *groupip)
{
	int		i;
	ushort	*sp;
	struct	ip *ti;
	struct	Igmphdr *tigmp;
	struct	ether_header *te;
	ulong	csum, *router_alert_opt;

	/* Retrieve an ethernet buffer from the driver and populate the */
	/* ethernet level of packet: */
	te = (struct ether_header *) getXmitBuffer();

	/* Source MAC is this target's MAC address...
	 */
	memcpy((char *)&te->ether_shost,(char *)BinEnetAddr,6);
	/* Destination MAC is multicast, so it is derived from the
	 * group IP address...
	 * The upper 24 bits are 0x01005e.
	 * For JOIN, the lower 24 bits are masked with the groupIP and 0x7fffff.
	 * For LEAVE, the message is sent to the All_Routers_Multicast_Group
	 *  (224.0.0.2), so the lower three bytes are 0,0,2.
	 */
	te->ether_dhost.ether_addr_octet[0] = 0x01;
	te->ether_dhost.ether_addr_octet[1] = 0x00;
	te->ether_dhost.ether_addr_octet[2] = 0x5e;

	if (type == IGMPTYPE_LEAVE) {
		te->ether_dhost.ether_addr_octet[3] = 0x00;
		te->ether_dhost.ether_addr_octet[4] = 0x00;
		te->ether_dhost.ether_addr_octet[5] = 0x02;
	}
	else {
		te->ether_dhost.ether_addr_octet[3] = groupip[1] & 0x7f;
		te->ether_dhost.ether_addr_octet[4] = groupip[2];
		te->ether_dhost.ether_addr_octet[5] = groupip[3];
	}
	te->ether_type = ecs(ETHERTYPE_IP);

	/* Move to the IP portion of the packet and populate it appropriately: */
	ti = (struct ip *) (te + 1);
	ti->ip_vhl = ((IP_VER<<4) | ((sizeof(struct ip)+sizeof(ulong)) >> 2));
	ti->ip_tos = 0;
	ti->ip_len = sizeof(struct ip) + sizeof(ulong) + sizeof(struct Igmphdr);
	ti->ip_id = ipId();
	ti->ip_off = 0;
	ti->ip_ttl = 1;
	ti->ip_p = IP_IGMP;
	memcpy((char *)&ti->ip_src.s_addr,(char *)BinIpAddr,4);
	if (type == IGMPTYPE_LEAVE)
		ti->ip_dst.s_addr = ALL_MULTICAST_ROUTERS;
	else
		memcpy((char *)&ti->ip_dst.s_addr,(char *)groupip,4);

	self_ecs(ti->ip_len);
	self_ecs(ti->ip_off);
	self_ecs(ti->ip_id);

	router_alert_opt = (ulong *)(ti+1);
	*router_alert_opt = 0x94040000;		/* see RFC2113 */
	
	ipChksum(ti);		/* Compute csum of ip hdr */

	/* Move to the IGMP portion of the packet and populate it appropriately: */
	tigmp = (struct Igmphdr *)(router_alert_opt+1);
	tigmp->type = type;
	tigmp->mrt = 0;
	tigmp->csum = 0;
	memcpy((char *)&tigmp->group,(char *)groupip,4);

	/* Calculate checksum of IGMP portion of the header.
	 * This uses the same method as is used with ipChksum()...
	 */
	sp = (ushort *)tigmp;
	csum = 0;
	for (i=0;i<4;i++,sp++) {
		csum += *sp;
		if (csum & 0x80000000)
			csum = (csum & 0xffff) + (csum >> 16);
	}
	while(csum >> 16)
		csum = (csum & 0xffff) + (csum >> 16);
	tigmp->csum = ~csum;

	sendBuffer(IGMP_JOINRQSTSIZE);
	return(0);
}

int
MacIsMulticast(uchar *mac)
{
	if (*mac != 0x01)
		return(0);
	mac++;
	if (*mac != 0x00)
		return(0);
	mac++;
	if (*mac != 0x5e)
		return(0);
	mac++;
	if (*mac & 0x80)
		return(0);
	return(1);
}

#endif
