#define __PACKED__	__attribute__ ((packed))

#ifdef CPU_LE
#define ecs(a) (((a & 0x00ff) << 8) | ((a & 0xff00) >> 8))
#define ecl(a) (((a & 0x000000ff) << 24) | ((a & 0x0000ff00) << 8) | \
		  ((a & 0x00ff0000) >> 8) | ((a & 0xff000000) >> 24))
#define self_ecs(a)	(a = ecs(a))
#define self_ecl(a)	(a = ecl(a))
#define ntohl	ecl
#define ntohs	ecs
#define htonl	ecl
#define htons	ecs
#else
#define ecs(a)  a
#define ecl(a)  a
#define self_ecs(a)
#define self_ecl(a)
#define ntohl(a)	a
#define ntohs(a)	a
#define htonl(a)	a
#define htons(a)	a
#endif

   
struct ether_addr {
	unsigned char  ether_addr_octet[6];
} __PACKED__ ;
 
struct  ether_header {
	struct  ether_addr ether_dhost;
	struct  ether_addr ether_shost;
	unsigned short ether_type;
} __PACKED__ ;


struct in_addr {
	unsigned long s_addr;
} __PACKED__ ;

struct arphdr {
	unsigned short	hardware;
	unsigned short	protocol;
	unsigned char	hlen;
	unsigned char	plen;
	unsigned short	operation;	
	unsigned char	senderha[6];
	unsigned char	senderia[4];
	unsigned char	targetha[6];
	unsigned char	targetia[4];
} __PACKED__ ;

struct ip {
	unsigned char  ip_vhl;		/* version & header length */
	unsigned char  ip_tos;		/* type of service */
	short  ip_len;				/* total length */
	unsigned short ip_id;		/* identification */
   	short  ip_off;				/* fragment offset field */
	unsigned char  ip_ttl;		/* time to live */
	unsigned char  ip_p;		/* protocol */
	unsigned short ip_sum;		/* checksum */
	struct in_addr ip_src;		/* source address */
	struct in_addr ip_dst;		/* dest address */
} __PACKED__ ;

struct icmp_hdr {
	unsigned char	type;		/* type of message */
	unsigned char	code;		/* type subcode */
	unsigned short	cksum;		/* ones complement cksum of struct */
} __PACKED__ ;

struct icmp_echo_hdr {
	unsigned char	type;		/* type of message */
	unsigned char	code;		/* type subcode */
	unsigned short	cksum;		/* ones complement cksum of struct */
	unsigned short	id;			/* identifier */
	unsigned short	seq;		/* sequence number */
} __PACKED__ ;

/* UDP protocol header.
 */
struct Udphdr {
	unsigned short	uh_sport;		/* source port */
	unsigned short	uh_dport;		/* destination port */
	unsigned short	uh_ulen;		/* udp length */
	unsigned short	uh_sum;			/* udp checksum */
} __PACKED__ ;


/* UDP pseudo header.
 */
struct UdpPseudohdr {
	struct in_addr ip_src;		/* source address */
	struct in_addr ip_dst;		/* dest address */
	unsigned char	zero;		/* fixed to zero */
	unsigned char	proto;		/* protocol */
	unsigned short	ulen;		/* udp length */
} __PACKED__ ;

struct udpinfo {
	struct in_addr	sip;		/* Source IP address */
	struct in_addr	dip;		/* Destination IP address */
	short			sport;		/* Source port */
	short			dport;		/* Destination port */
	char			*packet;	/* Pointer to packet buffer to be used */
	char			*udata;		/* Pointer to UDP data */
	int				plen;		/* Size of buffer pointed to by packet */
	int				ulen;		/* Size of udp data */
};

struct uneterr {
	int		errno;
	char	*errdesc;
};

struct arpcache {
	unsigned char	ip[4];
	unsigned char	ether[6];
};

#define SIZEOFARPCACHE	8

#define ETHERSIZE		sizeof(struct ether_header)
#define ARPSIZE 		(sizeof(struct ether_header) + sizeof(struct arphdr))
#define UDPSIZE			sizeof(struct Udphdr)

#define ETHERTYPE_IP	0x0800          /* IP protocol */
#define ETHERTYPE_ARP	0x0806          /* ARP protocol */
#define ARP_REQUEST		1
#define ARP_RESPONSE	2

#define UDP_TTL			0x3c
#define getIP_V(x)		((x)>>4) 
#define getIP_HL(x)		((x)&0xf)
#define IP_DONTFRAG		0x4000			/* dont fragment flag */
#define IP_MOREFRAGS	0x2000			/* more fragments flag */
#define IP_VER			0x4
#define IP_HDR_LEN		(sizeof(struct ip)>>2)
#define IP_HDR_VER_LEN	((IP_VER<<4)|IP_HDR_LEN)
#define IP_HLEN(ihdr)	((ihdr->ip_vhl&0x0f)<<2)
#define IPSIZE			sizeof(struct ip)
#define IP_IP   		0
#define IP_UDP  		17
#define IP_ICMP  		1

#define ANY_UDP_PORT	0

#define ICMP_ECHOREPLY		0
#define ICMP_ECHOREQUEST	8

#if CPU_LE
#define IP(a,b,c,d)		((a)|(b<<8)|(c<<16)|(d<<24))
#define IP1(a)			(a & 0xff)
#define IP2(a)			((a>>8) & 0xff)
#define IP3(a)			((a>>16) & 0xff)
#define IP4(a)			((a>>24) & 0xff)
#else
#define IP(a,b,c,d)		((a<<24)|(b<<16)|(c<<8)|(d))
#define IP1(a)			((a>>24) & 0xff)
#define IP2(a)			((a>>16) & 0xff)
#define IP3(a)			((a>>8) & 0xff)
#define IP4(a)			(a & 0xff)
#endif

#define	UNETERR_NOENV		-1
#define	UNETERR_BADENV		-2
#define	UNETERR_ARPFAIL		-3
#define UNETERR_IPGIPMASK	-4

#define UNETACT_PRINT	(1<<0)
#define UNETACT_TRACE	(2<<0)
#define UNETACT_EXIT	(3<<0)
#define UNETACT_ALL		0xffffffff

extern struct ether_addr thismac;
extern struct arpcache ArpCache[];
extern unsigned char BroadcastAddr[];
extern struct in_addr thisip, thisnm, thisgip;

extern int IpIsOnThisNet(unsigned char *ip);
extern void unetShowThis(char * header);
extern unsigned short ipId(void);
extern void ipChksum(struct ip *ihdr);
extern char *udpErr2Str(int err);
extern void unetStart(void);
extern void unetStop(void);
extern int udpSendTo(struct udpinfo *u);
extern int udpRecvFrom(struct udpinfo *u);
extern void unetError(char *msg, int errno, int action);
extern int IpToBin(char *ascii,unsigned char *binary);
extern unsigned char * EtherFromCache(unsigned char *ip);
extern int ArpStore(unsigned char *ip,unsigned char *ether);
extern unsigned char *ArpEther(struct udpinfo *u, unsigned char *ecpy);
extern int SendArpResp(struct ether_header *re);
extern int processARP(struct ether_header *ehdr,unsigned short size);
extern int processICMP(struct ether_header *ehdr,unsigned short size);
