#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "monlib.h"
#include "unet.h"

typedef unsigned long ulong;
typedef unsigned char uchar;
typedef unsigned short ushort;

struct	in_addr thisip, thisgip, thisnm;
struct	ether_addr thismac;
struct	arpcache ArpCache[SIZEOFARPCACHE];
unsigned char BroadcastAddr[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

struct uneterr unet_errtbl[] = {
	{ UNETERR_NOENV,		"Network shell variable not set" },

	{ UNETERR_BADENV,		"Network shell variable invalid" },

	{ UNETERR_ARPFAIL,		"ARP did not succeed" },

	{ UNETERR_IPGIPMASK,	"IP/GIP not on same subnet" },

	{ 0, 0 }
};

char *
unetErr2Str(int errno)
{
	struct uneterr *ue;

	ue = unet_errtbl;
	while(ue->errno != 0) {
		if (errno == ue->errno)
			return(ue->errdesc);
		ue++;
	}
	return("???");
}

/* unetError():
 * Use the unetErr2Str() function to process the incoming udp
 * error number then just exit.
 */
void
unetError(char *msg, int errno, int action)
{
	if (action & UNETACT_TRACE)
		mon_memtrace("%s: %s\n",msg ? msg : "", unetErr2Str(errno));
	if ((action & UNETACT_PRINT) || (action & UNETACT_EXIT))
		mon_printf("%s: %s\n",msg ? msg : "", unetErr2Str(errno));
	if (action & UNETACT_EXIT)
		mon_appexit(errno);
}

ushort
ipId(void)
{
	static unsigned short ipid = 0;

	return(++ipid);
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

	ihdr->ip_sum = 0;

	csum = 0;
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

/* EtherToBin():
 *	Convert ascii MAC address string to binary. 
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
			return(-1);
		}
		binary[i] = (uchar)digit;
	}
	return(0);
}

/* IpToBin():
 *	Convert ascii IP address string to binary. 
 */
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
			return(-1);
		}
		binary[i] = (uchar)digit;
	}
	return(0);
}

/* unetGetThis():
 * The structures "thisip/gip/nm" and "thismac" hold the NETWORK and MAC
 * addresses for this board.  This function must be called initially to
 * pull that information out of the corresponding uMon shell variables.
 * Note it is done when unetStart() is called so it need not be public.
 */
static int
unetGetThis(void)
{
	char *ipenv, *macenv, *gipenv, *nmenv;
	int rc1, rc2, rc3, rc4;

	ipenv = mon_getenv("IPADD");
	gipenv = mon_getenv("GIPADD");
	nmenv = mon_getenv("NETMASK");
	macenv = mon_getenv("ETHERADD");
	if ((ipenv == 0) || (macenv == 0) ||
		(gipenv == 0) || (nmenv == 0)) {
		unetError("unetGetThis",UNETERR_NOENV,UNETACT_EXIT);
	}

	rc1 = IpToBin(ipenv,(uchar *)&thisip.s_addr);
	rc2 = IpToBin(gipenv,(uchar *)&thisgip.s_addr);
	rc3 = IpToBin(nmenv,(uchar *)&thisnm.s_addr);
	rc4 = EtherToBin(macenv,thismac.ether_addr_octet);

	if ((rc1 < 0) || (rc2 < 0) || (rc3 < 0) || (rc4 < 0))
		unetError("unetGetThis",UNETERR_BADENV,UNETACT_EXIT);
	
	if (!IpIsOnThisNet((uchar *)&thisgip.s_addr)) {
		unetError("unetGetThis",UNETERR_IPGIPMASK,UNETACT_EXIT);
	}
	return(0);
}

void
unetShowThis(char *header)
{
	if (header)
		mon_printf("%s:\n",header);
	else
		mon_printf("This system's network info:\n");
	mon_printf("  IP:   %d.%d.%d.%d\n",
		IP1(thisip.s_addr), IP2(thisip.s_addr),
		IP3(thisip.s_addr), IP4(thisip.s_addr));
	mon_printf("  GIP:  %d.%d.%d.%d\n",
		IP1(thisgip.s_addr), IP2(thisgip.s_addr),
		IP3(thisgip.s_addr), IP4(thisgip.s_addr));
	mon_printf("  MASK: %d.%d.%d.%d\n",
		IP1(thisnm.s_addr), IP2(thisnm.s_addr),
		IP3(thisnm.s_addr), IP4(thisnm.s_addr));
	mon_printf("  MAC:  %02x:%02x:%02x:%02x:%02x:%02x\n",
		thismac.ether_addr_octet[0], thismac.ether_addr_octet[1],
		thismac.ether_addr_octet[2], thismac.ether_addr_octet[3],
		thismac.ether_addr_octet[4], thismac.ether_addr_octet[5]);
}

/* unetStop() & unetStart():
 * These functions are needed so that the application can tell
 * the underlying uMon code to not do any ethernet polling when facilities
 * within uMon are called.
 */
void
unetStop(void)
{
	/* Tell uMon's underlying ethernet driver restore polling
	 * of the interface.
	 */
	mon_sendenetpkt(0,-1);
}

void
unetStart(void)
{
	/* Tell uMon's underlying ethernet driver not to poll the
	 * interface; thus allowing this application (only) to do 
	 * the polling.
	 */
	mon_sendenetpkt(0,0);
	unetGetThis();
}
