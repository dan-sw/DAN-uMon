/* enetcore.c:
 *	This code supports basic ethernet connectivity of MicroMonitor's
 *  ethernet facility.  Refer to ethernet.c for more information on how
 *  to configure the subsection of ethernet within MicroMonitor.
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
#include "endian.h"
#include "stddefs.h"
#include "genlib.h"

#if INCLUDE_ENETCORE
#include "ether.h"
#include "cli.h"

void ShowEthernetStats(void);

#if !INCLUDE_BOOTP
#define dhcpStateCheck()
#define	dhcpDisable()
#define	ShowDhcpStats()
unsigned short	DHCPState;
#endif

#if !INCLUDE_TFTP
#define	tftpStateCheck()
#define	tftpInit()
#define	ShowTftpStats()
#endif

uchar BinGipAddr[4];		/* Space for binary GIP address */
uchar BinNetMask[4];		/* Space for binary NetMask */
uchar BinIpAddr[4];			/* Space for binary IP address */
uchar BinEnetAddr[6];		/* Space for binary MAC address */
uchar EtherIsActive;		/* Non-zero if ethernet is up. */
uchar EtherPollNesting;		/* Incremented when pollethernet() is called. */
uchar MaxEtherPollNesting;	/* High-warter mark of EtherPollNesting. */
ushort	UniqueIpId;

/* Ports used by the monitor have defaults, but can be redefined using
 * shell variables:
 */
#if INCLUDE_BOOTP
ushort	DhcpClientPort;		/* shell var: DCLIPORT */
ushort	DhcpServerPort;		/* shell var: DSRVPORT */
#endif
#if INCLUDE_TFTP
ushort	TftpPort;			/* shell var: TFTPPORT */
ushort	TftpSrcPort;		/* shell var: TFTPPORT */
#endif

uchar BroadcastAddr[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

/* DisableEthernet():
 * Shut down the interface, and return the state of the
 * interface prior to forcing the shut down.
 */
int
DisableEthernet(void)
{
	int	eia;

	eia = EtherIsActive;
	EtherIsActive = 0;
	DisableEtherdev();
	return(eia);
}

int
EthernetStartup(int verbose, int justreset)
{
	/* Initialize the retransmission delay calculator: */
	RetransmitDelay(DELAY_INIT_DHCP);

	EtherPollNesting = 0;
	MaxEtherPollNesting = 0;
	DHCPState = DHCPSTATE_NOTUSED;

	TftpPort = IPPORT_TFTP;	
	TftpSrcPort = IPPORT_TFTPSRC;
#if INCLUDE_BOOTP
	DhcpClientPort = IPPORT_DHCP_CLIENT;
	DhcpServerPort = IPPORT_DHCP_SERVER;
#endif

	UniqueIpId = (ushort)BinEnetAddr[5];

	/* Call device specific startup code: */
	if (EtherdevStartup(verbose) < 0)
		return(-1);

	/* Initialize some TFTP state... */
	tftpInit();

#if INCLUDE_BOOTP
	/* If EthernetStartup is called as a result of anything other than a
	 * target reset, don't startup any DHCP/BOOTP transaction...
	 */
	if (!justreset)
		dhcpDisable();
#endif
	EtherIsActive = 1;
	sendGratuitousArp();
	return(0);
}

/* pollethernet():
 *	Called at a few critical points in the monitor code to poll the
 *	ethernet device and keep track of the state of DHCP and TFTP.
 */
int
pollethernet(void)
{
	int	pcnt;

	if ((!EtherIsActive) || (EtherPollNesting > 4))
		return(0);

	EtherPollNesting++;
	if (EtherPollNesting > MaxEtherPollNesting)
		MaxEtherPollNesting = EtherPollNesting;

	pcnt = polletherdev();

	dhcpStateCheck();
	tftpStateCheck();

	EtherPollNesting--;
	return(pcnt);
}


/* processPACKET():
 *	This is the top level of the message processing after a complete
 *	packet has been received over ethernet.  It's all just a lot of
 *	parsing to determine whether the message is for this board's IP
 *	address (broadcast reception may be enabled), and the type of
 *	incoming protocol.  Once that is determined, the packet is either
 *	processed (TFTP, DHCP, ARP, ICMP-ECHO, etc...) or discarded.
 */
void
processPACKET(struct ether_header *ehdr, ushort size)
{
	int	i;
	ushort	*datap, udpport;
	ulong	csum;
	struct ip *ihdr;
	struct Udphdr *uhdr;

	if (ehdr->ether_type == ecs(ETHERTYPE_ARP)) {
		processARP(ehdr,size);
		return;
	}
	else if (ehdr->ether_type != ecs(ETHERTYPE_IP)) {
		return;
	}

	/* If source MAC address is this board, then assume we received our
	 * own outgoing broadcast message...
	 */
	if (!memcmp((char *)&(ehdr->ether_shost),(char *)BinEnetAddr,6)) {
		return;
	}

	ihdr = (struct ip *) (ehdr + 1);

	/* If not version # 4, return now... */
	if (getIP_V(ihdr->ip_vhl) != 4) {
		return;
	}

	/* IP address filtering:
	 * At this point, the only packets accepted are those destined for this
	 * board's IP address or broadcast to the subnet, plus DHCP, if active,
	 */
	if (memcmp((char *)&(ihdr->ip_dst),(char *)BinIpAddr,4)) {
		long net_mask, sub_net_addr;

		memcpy((char *)&net_mask,(char *)BinNetMask,4);
		sub_net_addr = ihdr->ip_dst.s_addr & ~net_mask;	/* x.x.x.255 */
		uhdr = (struct Udphdr *)(ihdr+1);

		if ( ihdr->ip_p != IP_UDP || sub_net_addr != ~net_mask ) {
#if INCLUDE_BOOTP	
			if (DHCPState == DHCPSTATE_NOTUSED)
				return;
			if (ihdr->ip_p != IP_UDP)
				return;
			uhdr = (struct Udphdr *)(ihdr+1);
			if (uhdr->uh_dport != ecs(DhcpClientPort)) {
				return;
			}
#else
			return;
#endif
		}
	}

	/* Verify incoming IP header checksum...
	 * Refer to section 3.2 of TCP/IP Illustrated, Vol 1 for details.
	 */
	csum = 0;
	datap = (ushort *) ihdr;
	for (i=0;i<(sizeof(struct ip)/sizeof(ushort));i++,datap++)
		csum += *datap;
	csum = (csum & 0xffff) + (csum >> 16);
	if (csum != 0xffff) {
		return;
	}
	
#if INCLUDE_ICMP
	if (ihdr->ip_p == IP_ICMP) {
		processICMP(ehdr,size);
		return;
	}
	else
#endif
	if (ihdr->ip_p != IP_UDP) {
#if INCLUDE_ICMP
		SendICMPUnreachable(ehdr,ICMP_UNREACHABLE_PROTOCOL);
#endif
		return;
	}

	uhdr = (struct Udphdr *)(ihdr+1);

	/* If non-zero, verify incoming UDP packet checksum...
	 * Refer to section 11.3 of TCP/IP Illustrated, Vol 1 for details.
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
		len = ecs(uhdr->uh_ulen);
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
		if (csum != 0xffff) {
			return;
		}
	}
	udpport = ecs(uhdr->uh_dport);

#if INCLUDE_BOOTP
	if (udpport == DhcpClientPort) {
		processDHCP(ehdr,size);
		return;
	}
#endif

#if INCLUDE_TFTP
	if ((udpport == TftpPort) || (udpport == TftpSrcPort)) {
		processTFTP(ehdr,size);
		return;
	}
#endif

#if INCLUDE_ICMP
	SendICMPUnreachable(ehdr,ICMP_UNREACHABLE_PORT);
#endif
}

int
IpToBin(char *ascii,uchar *binary)
{
	int	i, digit;
	char	*acpy;

	acpy = ascii;
	for(i=0;i<4;i++) {
		digit = (int)strtol(acpy,&acpy,10);
		if (((i != 3) && (*acpy++ != '.')) ||
			((i == 3) && (*acpy != 0)) ||
			(digit < 0) || (digit > 255)) {
			printf("Misformed IP addr: %s\n",ascii);
			return(-1);
		}
		binary[i] = (uchar)digit;
	}
	return(0);
}

/* ipChksum():
 *	Compute the checksum of the incoming IP header.  The size of
 *	the header is variable because of the possibility of there 
 *	being options; hence, the size of the header is taken from
 *	the ip_vhl field instead of assuming sizeof(struct ip).
 *	The incoming pointer to an IP header is directly populated with
 *	the result.
 */
void
ipChksum(struct ip *ihdr)
{
	register int	i, stot;
	register ushort	*sp;
	register long	csum;

	csum = 0;
	ihdr->ip_sum = 0;
	stot = (((ihdr->ip_vhl & 0x0f) << 2) / (int)sizeof(ushort));
	sp = (ushort *) ihdr;
	for (i=0;i<stot;i++,sp++) {
		csum += *sp;
		if (csum & 0x80000000)
			csum = (csum & 0xffff) + (csum >> 16);
	}
	while(csum >> 16)
		csum = (csum & 0xffff) + (csum >> 16);
	ihdr->ip_sum = ~csum;
}

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

struct	ether_header *
EtherCopy(struct ether_header *re)
{
	struct	ether_header *te;

	te = (struct ether_header *) getXmitBuffer();
	memcpy((char *)&(te->ether_shost),(char *)BinEnetAddr,6);
	memcpy((char *)&(te->ether_dhost),(char *)&(re->ether_shost),6);
	te->ether_type = re->ether_type;
	return(te);
}

ushort
ipId(void)
{
	return(++UniqueIpId);
}


/* RetransmitDelay():
 *	This function provides a common point for retransmission delay
 *	calculation.  It is an implementation "similar" to the recommendation
 *	made in the RFC 2131 (DHCP) section 4.1 paragraph #8...
 *
 *	The delay before the first retransmission is 4 seconds randomized by
 *	the value of a uniform random number chosen from the range -1 to +2.
 *	The delay before the next retransmission is 8 seconds randomized by
 *	the same number as previous randomization.  Each subsequent retransmission
 *	delay is doubled up to a maximum of 66 seconds.  Once a delay of 66
 *	seconds is reached, return that value for 6 subsequent delay
 *	requests, then return RETRANSMISSION_TIMEOUT (-1) indicating that the
 *	requestor should give up.
 *
 *	The value of randomdelta will be 2, 1, 0 or -1 depending on the target's
 *	IP address.
 *
 *	The return values will be...
 *	4+randomdelta, 8+randomdelta, 16+randomdelta, etc... up to 64+randomdelta.
 *	Then after returning 64+randomdelta 6 times, return RETRANSMISSION_TIMEOUT.
 *
 *	NOTE: if DELAY_RETURN is the opcode, then RETRANSMISSION_TIMEOUT is
 *		  never returned, once the max is reached, it is always the value
 *		  returned;
 *		  if DELAY_OR_TIMEOUT_RETURN is the opcode, then once maxoutcount
 *		  reaches 6, RETRANSMISSION_TIMEOUT is returned.
 *
 *	NOTE1: this function supports the ability to modify the above-discussed
 *		   parameters.  Start with DELAY_INIT_DHCP to set up the parameters
 *		   discussed above; start with DELAY_INIT_XXXX for others.
 */
int
RetransmitDelay(int opcode)
{
	static int randomdelta;		/* Used to slightly randomize the delay.
								 * Taken from the 2 least-significant-bits
								 * of the IP address (range = -1 to 2).
								 */
	static int rexmitdelay;		/* Doubled each time DELAY_INCREMENT
								 * is called until it is greater than the
								 * value stored in rexmitdelaymax.
								 */
	static int rexmitdelaymax;	/* See rexmitdelay. */
	static int maxoutcount;		/* Number of times the returned delay has
								 * reached its max.
								 */
	static int giveupcount;		/* Once maxoutcount reaches this value, we
								 * give up and return TIMEOUT.
								 */
	int		rexmitstate;

	rexmitstate = RETRANSMISSION_ACTIVE;
	switch(opcode) {
		case DELAY_INIT_DHCP:		
			rexmitdelay = 4;
			giveupcount = 6;
			rexmitdelaymax = 64;
			maxoutcount = 0;
			randomdelta = (int)(BinIpAddr[3] & 3) - 1;
			break;
		case DELAY_INIT_TFTP:
			rexmitdelay = 4;
			giveupcount = 3;
			rexmitdelaymax = 32;
			maxoutcount = 0;
			randomdelta = (int)(BinIpAddr[3] & 3) - 1;
			break;
		case DELAY_INIT_ARP:
			rexmitdelay = 1;	
			giveupcount = 0;
			rexmitdelaymax = 4;
			maxoutcount = 0;
			randomdelta = 0;
			break;
		case DELAY_INCREMENT:
			if (rexmitdelay < rexmitdelaymax)
				rexmitdelay <<= 1;		/* double it. */
			else 
				maxoutcount++;

			if (maxoutcount > giveupcount)
				rexmitstate = RETRANSMISSION_TIMEOUT;
			break;
		case DELAY_OR_TIMEOUT_RETURN:
			if (maxoutcount > giveupcount)
				rexmitstate = RETRANSMISSION_TIMEOUT;
			break;
		case DELAY_RETURN:
			break;
		default:
			printf("\007TimeoutAlgorithm error 0x%x.\n",opcode);
			rexmitstate = RETRANSMISSION_TIMEOUT;
			break;
	}
	if (rexmitstate == RETRANSMISSION_TIMEOUT)
		return(RETRANSMISSION_TIMEOUT);
	else
		return(rexmitdelay+randomdelta);
}

/* monSendEnetPkt() & monRecvEnetPkt():
 * These two functions allow the monitor to provide a primitive
 * connect to the ethernet interface for an application (using
 * mon_sendenetpkt() and mon_recvenetpkt()).
 *
 * Note: These are obviously not very sophisticated; however, they
 * provide an immediate mechanism for LWIP to access the raw driver.
 * without the application even being aware of the type of ethernet
 * device.
 */
int
monSendEnetPkt(char *pkt, int pktlen)
{
	/* Copy the incoming packet into a packet that has been allocated
	 * by the monitor's packet allocator for this ethernet device:
	 */
	memcpy((char *)getXmitBuffer(),pkt,pktlen);
	sendBuffer(pktlen);
	return(pktlen);
}

int
monRecvEnetPkt(char *pkt, int pktlen)
{
	int	pcnt, len;

	pcnt = polletherdev();
	if (pcnt == 0) {
		len = 0;
	}
	else {
		if (pcnt > 1)
			len = -pktlen;
		else
			len = pktlen;
	}
	return(len);
}

/* Enet():
 *	Used in "mini" mode to establish MAC and optionally the IP address
 *  of the target.
 */

char *EnetHelp[] = {
	"Ethernet setup",
	"{mac} [ip nm gip]",
	0,
};

int
Enet(int argc,char *argv[])
{
	if (argc == 2) {
		if (EtherToBin(argv[1],BinEnetAddr) == -1)
			return(-1);
	}
	else if (argc == 5) {
		if (EtherToBin(argv[1],BinEnetAddr) == -1)
			return(-1);
		if (IpToBin(argv[2],BinIpAddr) == -1)
			return(-1);
		if (IpToBin(argv[3],BinNetMask) == -1)
			return(-1);
		if (IpToBin(argv[4],BinGipAddr) == -1)
			return(-1);
	}
	else
		return(CMD_PARAM_ERROR);


	EthernetStartup(0,1);
	return(CMD_SUCCESS);
}

int
enetInitialized(void)
{
	int i;
	uchar *bp = BinEnetAddr;

	for(i=0;i<6;i++) {
		if (*bp++ != 0)
			return(1);
	}
	printf("Ethernet not initialized\n");
	return(0);
}

/* EtherToBin():
 *	Convert ascii MAC address string to binary.  Note that this is outside
 *	the #if INCLUDE_ETHERNET because it is used by password.c.  This correctly
 *	implies that if there is no ethernet interface, then we need a different
 *	solution for the password backdoor!.
 */
int
EtherToBin(char *ascii,uchar *binary)
{
	int	i, digit;
	char	*acpy;

	acpy = ascii;
	for(i=0;i<6;i++) {
		digit = (int)strtol(acpy,&acpy,16);
		if (((i != 5) && (*acpy++ != ':')) ||
			((i == 5) && (*acpy != 0)) ||
			(digit < 0) || (digit > 255)) {
			printf("Misformed ethernet addr: %s\n",ascii);
			return(-1);
		}
		binary[i] = (uchar)digit;
	}
	return(0);
}

/* IpToString():
 *	Incoming ascii pointer is assumed to be pointing to at least 16
 *	characters of available space.  Conversion from long to ascii is done
 *	and string is terminated with NULL.  The ascii pointer is returned.
 */
char *
IpToString(ulong ipadd,char *ascii)
{
	uchar	*cp;

	cp = (uchar *)&ipadd;
	sprintf(ascii,"%d.%d.%d.%d",
	    (int)cp[0],(int)cp[1],(int)cp[2],(int)cp[3]);
	return(ascii);
}

#endif	/* INCLUDE_ENETCORE */
