/* dhcp_02.c:
 *	This is the PPA-specific code used for Time-Warner Market.
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
#include "cpuio.h"
#include "ether.h"
#include "tfs.h"
#include "tfsprivate.h"
#include "string.h"
#include "stddefs.h"

/* ValidDHCPOffer():
 *	Target issued the DISCOVER, the incoming packet is the server's
 *	OFFER reply.  If the offer contains the string PPADHCPSRVR in the
 *	bootfile string, then accept the offer (return 1); else reject it.
 */
ValidDHCPOffer(struct	dhcphdr *dhdr)
{
	if (strcmp(dhdr->bootfile,"PPADHCPSRVR"))
		return(0);
	return(1);
}

/* buildDhcpHdr():
 *	Called by dhcpboot.c to allow application-specific header stuff to
 *	be added to header.  Return 0 if generic stuff in dhcpboot.c is to be
 *	used; else return 1 and the calling code will assume this function is
 *	dealing with it (see dhcpboot.c for basic idea).
 */
int
buildDhcpHdr(struct dhcphdr *dhcpdata)
{
	return(0);
}

/* DhcpBootpDone():
 *	Called at the end of the Bootp or Dhcp transaction.
 *	Input...
 *	bootp:	1 if BOOTP; else DHCP.
 *	dhdr:	pointer to dhcp or bootp header.
 *	vsize:	size of vendor specific area (for bootp this is fixed at 64, 
 *			but for dhcp it is variable).
 */
void
DhcpBootpDone(int bootp, struct dhcphdr *dhdr, int vsize)
{
	return;
}

/* DhcpVendorSpecific():
 *	Process vendor specific stuff within the incoming dhcp header.
 */
void
DhcpVendorSpecific(struct	dhcphdr *dhdr)
{
}

/* printDhcpVSopt():
 *	Print vendor specific stuff within the incoming dhcp header.
 */
int
printDhcpVSopt(int vsopt, int vsoptlen, char *options)
{
	return(0);
}
