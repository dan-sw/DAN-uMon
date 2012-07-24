/* bootp.c:
 *	Chopped up version of dhcpboot.c that supports only bootp.
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
#include "ether.h"
#include "genlib.h"
#include "stddefs.h"
#include "cli.h"
#include "timer.h"

#if INCLUDE_BOOTP

int		BOOTPStartup(short);
void	BootpLoadVSA(uchar *vsabin, int vsize);

unsigned short	DHCPState;

static struct elapsed_tmr dhcpTmr;
static int		BOOTPCommandIssued;
static ulong	BOOTPTransactionId;

/* Variable to keep track of elapsed seconds since DHCP started: */
static short	BOOTPElapsedSecs;

char *BootpHelp[] = {
	"Issue a BOOTP request",
	"",
		0,
};

int
Bootp(int argc,char *argv[])
{
	if (!enetInitialized())
		return(CMD_FAILURE);

	startElapsedTimer(&dhcpTmr,RetransmitDelay(DELAY_INIT_DHCP)*1000);

	DHCPState = BOOTPSTATE_INITIALIZE;
	BOOTPStartup(0);

	while(DHCPState != BOOTPSTATE_COMPLETE) {
		pollethernet();
		if (gotachar()) {
			DHCPState = BOOTPSTATE_COMPLETE;
			break;
		}
	}
	return(CMD_SUCCESS);
}

void
dhcpDisable()
{
	DHCPState = DHCPSTATE_NOTUSED;
}

/* BOOTPStartup()
 *	The DHCPDISCOVER is issued as an ethernet broadcast.  IF the bootp
 *	flag is non-zero then just do a bootp request (a subset of the 
 *	DHCPDISCOVER stuff).
 */
int
BOOTPStartup(short seconds)
{
	struct	dhcphdr *dhcpdata;
	struct	bootphdr *bootpdata;
	struct	ether_header *te;
	struct	ip *ti;
	struct	Udphdr *tu;
	ushort	uh_ulen;
	int		optlen;

	/* Retrieve an ethernet buffer from the driver and populate the
	 * ethernet level of packet:
	 */
	te = (struct ether_header *) getXmitBuffer();
	memcpy((char *)&te->ether_shost,(char *)BinEnetAddr,6);
	memcpy((char *)&te->ether_dhost,(char *)BroadcastAddr,6);
	te->ether_type = ecs(ETHERTYPE_IP);

	/* Move to the IP portion of the packet and populate it appropriately: */
	ti = (struct ip *) (te + 1);
	ti->ip_vhl = IP_HDR_VER_LEN;
	ti->ip_tos = 0;
	ti->ip_id = 0;
	ti->ip_off = ecs(0x4000);	/* No fragmentation allowed */
	ti->ip_ttl = UDP_TTL;
	ti->ip_p = IP_UDP;
	memset((char *)&ti->ip_src.s_addr,0,4);
	memset((char *)&ti->ip_dst.s_addr,0xff,4);

	/* Now udp... */
	tu = (struct Udphdr *) (ti + 1);
	tu->uh_sport = ecs(DhcpClientPort);
	tu->uh_dport = ecs(DhcpServerPort);

	/* First the stuff that is the same for BOOTP or DHCP... */
	bootpdata = (struct bootphdr *)(tu+1);
	dhcpdata = (struct dhcphdr *)(tu+1);
	dhcpdata->op = DHCPBOOTP_REQUEST;
	dhcpdata->htype = 1;
	dhcpdata->hlen = 6;
	dhcpdata->hops = 0;
	dhcpdata->seconds = ecs(seconds);
	memset((char *)dhcpdata->bootfile,0,
		sizeof(dhcpdata->bootfile));
	memset((char *)dhcpdata->server_hostname,0,
		sizeof(dhcpdata->server_hostname));

	/* For the first request issued, establish a transaction id based
	 * on a crc32 of the mac address.  For each request after that,
	 * just increment.
	 */
	if (!BOOTPTransactionId)
		BOOTPTransactionId = BinEnetAddr[5];
	else
		BOOTPTransactionId++;

	memcpy((char *)&dhcpdata->transaction_id,(char *)&BOOTPTransactionId,4);
	memset((char *)&dhcpdata->client_ip,0,4);
	memset((char *)&dhcpdata->your_ip,0,4);
	memset((char *)&dhcpdata->server_ip,0,4);
	memset((char *)&dhcpdata->router_ip,0,4);
	memcpy((char *)dhcpdata->client_macaddr,(char *)BinEnetAddr,6);
	dhcpdata->flags = 0;

	self_ecs(dhcpdata->flags);

	/* Finally, the DHCP or BOOTP specific stuff...
	 * Based on RFC1534 (Interoperation Between DHCP and BOOTP), any message
	 * received by a DHCP server that contains a 'DHCP_MESSAGETYPE' option
	 * is assumed to have been sent by a DHCP client.  A message without the
	 * DHCP_MESSAGETYPE option is assumed to have been sent by a BOOTP
	 * client.
	 */
	uh_ulen = optlen = 0;
	memset((char *)bootpdata->vsa,0,sizeof(bootpdata->vsa));
	uh_ulen = sizeof(struct Udphdr) + sizeof(struct bootphdr);
	tu->uh_ulen = ecs(uh_ulen);
	ti->ip_len = ecs((sizeof(struct ip) + uh_ulen));

	ipChksum(ti);	/* Compute checksum of ip hdr */
	udpChksum(ti);	/* Compute UDP checksum */

	DHCPState = BOOTPSTATE_REQUEST;
	sendBuffer(BOOTPSIZE);

	return(0);
}

/* dhcpStateCheck():
 *	Called by pollethernet() to monitor the progress of DHCPState.
 *	The retry rate is "almost" what is specified in the RFC...
 *	Refer to the RetransmitDelay() function for details.
 *
 *	Regarding timing...
 *	The DHCP startup may be running without an accurate measure of elapsed
 *	time.  The value of LoopsPerMillisecond is used as an approximation of
 *	the number of times this function must be called for one second to 
 *	pass (dependent on network traffic, etc...).  RetransmitDelay() is
 *	called to retrieve the number of seconds that must elapse prior to 
 *	retransmitting the last DHCP message.  The static variables in this
 *	function are used to keep track of that timeout.
 */
void
dhcpStateCheck(void)
{
	int delaysecs;

	/* If the DHCP command has been issued, it is assumed that the script
	 * is handling retries...
	 */
	if (BOOTPCommandIssued)
		return;

	/* Return, restart or fall through; depending on DHCPState... */
	switch(DHCPState) {
		case DHCPSTATE_NOTUSED:
		case BOOTPSTATE_COMPLETE:
			return;
		case BOOTPSTATE_RESTART:
			BOOTPStartup(0);
			return;
		case BOOTPSTATE_INITIALIZE:
			DHCPState = BOOTPSTATE_INITDELAY;
			return;
		case BOOTPSTATE_INITDELAY:
			BOOTPElapsedSecs = 0;
			startElapsedTimer(&dhcpTmr,
				RetransmitDelay(DELAY_INIT_DHCP)*1000);
			BOOTPStartup(0);
			return;
		default:
			break;
	}

	if (msecElapsed(&dhcpTmr)) {
		int lastdelay;
		
		lastdelay = RetransmitDelay(DELAY_RETURN);
		delaysecs = RetransmitDelay(DELAY_INCREMENT);
		
		if (delaysecs != RETRANSMISSION_TIMEOUT) {
			BOOTPElapsedSecs += delaysecs;
			startElapsedTimer(&dhcpTmr,delaysecs*1000);

			BOOTPStartup(BOOTPElapsedSecs);
			printf("  BOOTP retry (%d secs)\n",lastdelay);
		}
		else {
			printf("  BOOTP giving up\n");
		}
	}
}

/* xidCheck():
 *	Common function used for DHCP and BOOTP to verify incoming transaction
 *	id...
 */
int
xidCheck(char *id,int bootp)
{
	if (memcmp(id,(char *)&BOOTPTransactionId,4)) {
		printf("%s ignored: unexpected transaction id.\n",
			bootp ? "BOOTP":"DHCP");
		return(-1);
	}
	return(0);
}

int
loadBootFile(char *bootfile,char *tftpsrvr)
{
	char *comma;
	ulong	addr;
	void	(*func)(void);

	/* Default to no entrypoint and a load address of AppRamStart.
	 * but allow these to be overwritten by comma delimited fields
	 * that may be specified in the bootfilename...
	 *
	 * Syntax: "filename[,LOADADDRESS][,ENTRYPOINT]"
	 */
	func = 0;
	addr = APPLICATION_RAMSTART;

	comma = strchr(bootfile,',');
	if (comma) {
		*comma = 0;
		addr = strtol(comma+1,0,0);
		comma = strchr(comma+1,',');
		if (comma) 
			func = (void(*)(void))strtol(comma+1,0,0);
	}

	/* If the TFTP transfer succeeds, assume the boot file image was
	 * loaded into the base of AppRamStart and jump there...
	 */
	if (tftpGet(addr,tftpsrvr,bootfile) > 0) {
		DHCPState = BOOTPSTATE_COMPLETE;

		if (func)
			func();
	}
	else {
		tftpInit();
		RetransmitDelay(DELAY_INIT_TFTP);
		DHCPState = DHCPSTATE_RESTART;
	}
	return(0);
}

/* processDHCP():
 * Actually this is processBOOTP, but we call it processDHCP here so that
 * the same code in enetcore.c can be used to call either function (depending
 * on what has been configured into the uMon build).
 */
int
processDHCP(struct ether_header *ehdr,ushort size)
{
	char	getbootfile = 0;
	struct	ip *ihdr;
	struct	Udphdr *uhdr;
	struct	bootphdr *bhdr;
	ulong	ip, temp_ip, cookie;
	uchar	buf[16], bootsrvrip[16], *op;

	ihdr = (struct ip *)(ehdr + 1);
	uhdr = (struct Udphdr *)((char *)ihdr + IP_HLEN(ihdr));
	bhdr = (struct bootphdr *)(uhdr+1);

	/* Verify incoming transaction id matches the previous outgoing value: */
	if (xidCheck((char *)&bhdr->transaction_id,1) < 0)
		return(-1);

	/* If bootfile is nonzero, store it into BOOTFILE shell var: */
	if (bhdr->bootfile[0]) 
		getbootfile++;


	/* Assign IP "server_ip" to the BOOTSRVR shell var (if non-zero): */
	memcpy((char *)&temp_ip,(char *)&bhdr->server_ip,4);
	if (temp_ip) 
		getbootfile++;
	IpToString(temp_ip,(char *)bootsrvrip);

	/* Assign IP address loaded in "your_ip" to the IPADD shell var: */
	memcpy((char *)BinIpAddr,(char *)&bhdr->your_ip,4);
	memcpy((char *)&temp_ip,(char *)&bhdr->your_ip,4);
	printf("IP: %s\n",IpToString(temp_ip,(char *)buf));

	/* If STANDARD_MAGIC_COOKIE exists, then process options... */
	memcpy((char *)&cookie,(char *)bhdr->vsa,4);
	if (cookie == ecl(STANDARD_MAGIC_COOKIE)) {
		/* Assign subnet mask option to NETMASK shell var (if found): */
		op = DhcpGetOption(DHCPOPT_SUBNETMASK,&bhdr->vsa[4]);
		if (op) {
			memcpy((char *)BinNetMask,(char *)op+2,4);
			memcpy((char *)&ip,(char *)op+2,4);
			printf("NETMASK: %s\n",IpToString(ip,(char *)buf));
		}
		/* Assign first router option to GIPADD shell var (if found): */
		/* (the router option can have multiple entries, and they are */
		/* supposed to be in order of preference, so use the first one) */
		op = DhcpGetOption(DHCPOPT_ROUTER,&bhdr->vsa[4]);
		if (op) {
			memcpy((char *)BinGipAddr,(char *)op+2,4);
			memcpy((char *)&ip,(char *)op+2,4);
			printf("GIPADD: %s\n",IpToString(ip,(char *)buf));
		}
	}

	/* Call loadBootFile() which will then kick off a tftp client
	 * transfer if both BOOTFILE and BOOTSRVR shell variables are
	 * loaded; otherwise, we are done.
	 */
	/* If the bootp server gave us the bootfile and boot server IP, 
	 * then call loadBootFile()...
	 */
	if (getbootfile == 2)
		loadBootFile((char *)bhdr->bootfile,(char *)bootsrvrip);
	else
		DHCPState = BOOTPSTATE_COMPLETE;

	return(0);
}

/* DhcpGetOption():
 *	Based on the incoming option pointer and a specified option value,
 *	search through the options list for the value and return a pointer
 *	to that option.
 */
uchar *
DhcpGetOption(unsigned char optval,unsigned char *options)
{
	int		safety;

	safety = 0;
	while(*options != 0xff) {
		if (safety++ > 1000)
			break;
		if (*options == 0) {	/* Skip over padding. */
			options++;
			continue;
		}
		if (*options == optval)
			return(options);
		options += ((*(options+1)) + 2);
	}
	return(0);
}

int
DhcpIPCheck(char *ipadd)
{
	/* Ifthe incoming IP address is not understood, then default
	 * to bootp startup...
	 */
	if (IpToBin(ipadd,BinIpAddr) < 0) {
		DHCPState = BOOTPSTATE_INITIALIZE;
		memset((char *)BinIpAddr,0,4);
	}
	else {
		DHCPState = DHCPSTATE_NOTUSED;
	}
	return(0);
}

char *
bootpStringState(int state)
{
	switch(state) {
		case BOOTPSTATE_INITIALIZE:
			return("INITIALIZE");
		case BOOTPSTATE_REQUEST:
			return("REQUEST");
		case BOOTPSTATE_RESTART:
			return("RESTART");
		case BOOTPSTATE_COMPLETE:
			return("COMPLETE");
		case DHCPSTATE_NOTUSED:
			return("NOTUSED");
		default:
			return("???");
	}
}

void
ShowDhcpStats()
{
	printf("Current BOOTP State: BOOTP_%s\n",bootpStringState(DHCPState));
}

#endif
