/* tftp.c:
 *	This code supports the monitor's TFTP server.
 *	TFTP is covered under RFC 783.
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
#if INCLUDE_TFTP
#include "endian.h"
#include "genlib.h"
#include "ether.h"
#include "stddefs.h"
#include "flash.h"
#include "cli.h"
#include "timer.h"

static int SendTFTPErr(struct ether_header *,short,char *,int);
static int SendTFTPAck(struct ether_header *,ushort);
static int SendTFTPRRQ(uchar *,uchar *,char *,uchar *);


static	struct elapsed_tmr tftpTmr;

static ushort	tftpPrevBlock;	/* Keeps the value of the block number of
								 * the previous TFTP transaction.
								 */
static int TftpLastPktSize;
static uchar TftpLastPkt[TFTP_PKTOVERHEAD+TFTP_DATAMAX+4];
								/* Storage of last packet sent.  This is
								 * used if it is determined that the packet
								 * most recently sent must be sent again.
								 */

static long	TftpRetryTimeout;	/* Count used during a TFTP transfer to
								 * kick off a retransmission of a packet.
								 */
static ushort TftpRmtPort;		/* Remote port of tftp transfer */
static uchar *TftpAddr;			/* Current destination address used for tftp
								 * file transfer into local memory.
								 */
static int	TftpCount;			/* Running total number of bytes transferred
								 * for a particular tftp transaction.
								 */
static int	TftpState;			/* Used to keep track of the current state
								 * of a tftp transaction.
								 */
static int	TftpTurnedOff;		/* If non-zero, then tftp is disabled. */
static char TftpErrString[32];	/* Used to post a tftp error message. */

static int
tftpGotoState(int state)
{
	int	ostate;

	ostate = TftpState;
	TftpState = state;
	return(ostate);
}

/* tftpGet():
 *	Return size of file if successful; else 0.
 */
int
tftpGet(ulong addr,char *tftpsrvr,char *hostfile)
{
	int		done;
	uchar	binip[8], binenet[8], *enetaddr;

	printf("TFTP xfer '%s:%s' to 0x%lx...\n",tftpsrvr,hostfile,addr);

	/* Convert IP address to binary: */
	if (IpToBin(tftpsrvr,binip) < 0)
		return(0);

	/* Get the ethernet address for the IP: */
	/* Give ARP the same verbosity (if any) set up for TFTP: */
	enetaddr = ArpEther(binip,binenet);
	if (!enetaddr) {
		printf("ARP fail %s\n",tftpsrvr);
		return(0);
	}

	/* Send the TFTP RRQ to initiate the transfer. */
	if (SendTFTPRRQ(binip,binenet,hostfile,(uchar *)addr) < 0) {
		printf("RRQ failed\n");
		return(0);
	}

	/* Wait for TftpState to indicate that the transaction has completed... */
	done = 0;
	while(!done) {
		pollethernet();
		switch(TftpState) {
			case TFTPIDLE:
				printf("Rcvd %d bytes",TftpCount);
				done = 1;
				break;
			case TFTPERROR:
				printf("Host err: %s\n",TftpErrString);
				done = 2;
				break;
			case TFTPTIMEOUT:
				printf("Timing out\n");
				done = 2;
				break;
			default:
				break;
		}
	}
	if (done == 2)
		return(0);

	printf("\n");
	return(TftpCount);
}

/* tftpInit():
 *	Called by the ethenet initialization to initialize state variables.
 */
void
tftpInit()
{
	TftpCount = -1;
	TftpRmtPort = 0;
	TftpTurnedOff = 0;
	tftpGotoState(TFTPIDLE);
	TftpAddr = (uchar *)0;
}

/* storePktAndSend():
 *	The final stage in sending a TFTP packet...
 *	1. Compute IP and UDP checksums;
 *	2. Copy ethernet packet to a buffer so that it can be resent
 *		if necessary (by tftpStateCheck()).
 *	3. Store the size of the packet; 
 *	4. Send the packet out the interface.
 *	5. Reset the timeout count and re-transmission delay variables.
 */
static void
storePktAndSend(struct ip *ipp, struct ether_header *epkt,int size)
{
	ipChksum(ipp);							/* Compute csum of ip hdr */
	udpChksum(ipp);							/* Compute UDP checksum */
											/* Copy packet to static buffer */
	memcpy((char *)TftpLastPkt,(char *)epkt,size);
	TftpLastPktSize = size;					/* Copy size to static location */
	sendBuffer(size);						/* Send buffer out ethernet i*/

	/* Re-initialize the re-transmission delay variables.
	 */
	TftpRetryTimeout = RetransmitDelay(DELAY_INIT_TFTP);
	startElapsedTimer(&tftpTmr,TftpRetryTimeout * 1000);
}

/* getTftpSrcPort():
 *	Each time a TFTP RRQ goes out, use a new source port number.
 *	Cycle through a block of 256 port numbers...
 */
static ushort
getTftpSrcPort(void)
{
	if (TftpSrcPort < (IPPORT_TFTPSRC+256))
		TftpSrcPort++;
	else
		TftpSrcPort = IPPORT_TFTPSRC;
	return(TftpSrcPort);
}

/* tftpStateCheck():
 *	Called by the pollethernet function to support the ability to retry
 *	on a TFTP transmission that appears to have terminated prematurely
 *	due to a lost packet.  If a packet is sent and the response is not
 *	received within about 1-2 seconds, the packet is re-sent.  The retry
 *	will repeat 8 times; then give up and set the TFTP state to idle.
 *
 *	Taken from RFC 1350 section 2 "Overview of the Protocol"...
 *	... If a packet gets lost in the network, the intended recipient will
 *	timeout and may retransmit his last packet (which may be data or an
 *	acknowledgement), thus causing the sender of the lost packet to retransmit
 *	the lost packet.
 *
 *	Taken from RFC 1123 section 4.2.3.2 "Timeout Algorithms"...
 *	... a TFTP implementation MUST use an adaptive timeout ...
 */

void
tftpStateCheck(void)
{
	uchar	*buf;
	long	delay;
	ushort	tftp_opcode;
	struct	ip *ihdr;
	struct	Udphdr *uhdr;
	struct	ether_header *ehdr;

	switch(TftpState) {
		case TFTPIDLE:
		case TFTPTIMEOUT:
		case TFTPERROR:
			return;
		default:
			break;
	}

	/* If timeout occurs, re-transmit the packet...
	 */
	if (!msecElapsed(&tftpTmr))
		return;

	delay = RetransmitDelay(DELAY_INCREMENT);
	if (delay == RETRANSMISSION_TIMEOUT) {
		printf("  TFTP giving up\n");
		tftpGotoState(TFTPTIMEOUT);
//		enableBroadcastReception();
		return;
	}

	/* Get a transmit buffer and copy the packet that was last sent.
	 * Insert a new IP ID, recalculate the checksums and send it again...
	 * If the opcode of the packet to be re-transmitted is RRQ, then
	 * use a new port number.
	 */
	buf = (uchar *)getXmitBuffer();
	memcpy((char *)buf,(char *)TftpLastPkt,TftpLastPktSize);
	ehdr = (struct ether_header *)buf;
	ihdr = (struct ip *)(ehdr + 1);
	uhdr = (struct Udphdr *)(ihdr + 1);
	tftp_opcode = *(ushort *)(uhdr + 1);
	ihdr->ip_id = ipId();
	if (tftp_opcode == ecs(TFTP_RRQ)) {
		uhdr->uh_sport = getTftpSrcPort();
		self_ecs(uhdr->uh_sport);
	}
	ipChksum(ihdr);
	udpChksum(ihdr);
	sendBuffer(TftpLastPktSize);

	TftpRetryTimeout = delay;
	startElapsedTimer(&tftpTmr,TftpRetryTimeout * 1000);
	return;
}

/* tftpTransferComplete():
 * Print a message indicating that the transfer has completed.
 */
static void
tftpTransferComplete(void)
{
}

/* processTFTP():
 *	This function handles the majority of the TFTP requests that a
 *	TFTP server must be able to handle.  There is no real robust 
 *	error handling, but it seems to work pretty well.
 *	Refer to Stevens' "UNIX Network Programming" chap 12 for details
 *	on TFTP.
 *	Note: During TFTP, promiscuity, broadcast & multicast reception
 *	are all turned off.  This is done to speed up the file transfer.
 */
int
processTFTP(struct ether_header *ehdr,ushort size)
{
	static	uchar	*oaddr;
	struct	ip *ihdr;
	struct	Udphdr *uhdr;
	uchar	*data;
	int		count;
	ushort	opcode, block, errcode;
	char	*errstring, *tftpp;

	ihdr = (struct ip *)(ehdr + 1);
	uhdr = (struct Udphdr *)((char *)ihdr + IP_HLEN(ihdr));
	tftpp = (char *)(uhdr + 1);
	opcode = *(ushort *)tftpp;

	switch (opcode) {
	case ecs(TFTP_DAT):
		block = ecs(*(ushort *)(tftpp+2));
		count = ecs(uhdr->uh_ulen) - (sizeof(struct Udphdr)+4);

		if (TftpState == TFTPSENTRRQ) {		/* See notes in SendTFTPRRQ() */
			tftpPrevBlock = 0;
			if (block == 1) {
				TftpRmtPort = ecs(uhdr->uh_sport);
				tftpGotoState(TFTPACTIVE);
			}
			else {
				SendTFTPErr(ehdr,0,"invalid block",1);
				return(0);
			}
		}
		/* Since we don't ACK the final TFTP_DAT from the server until after
		 * the file has been written, it is possible that we will receive
		 * a re-transmitted TFTP_DAT from the server.  This is ignored by
		 * Sending another ACK...
		 */
		else if ((TftpState == TFTPIDLE) && (block == tftpPrevBlock)) {
			SendTFTPAck(ehdr,block);	
			return(0);				
		}
		else if (TftpState != TFTPACTIVE) {
			SendTFTPErr(ehdr,0,"invalid state",1);
			return(0);
		}

		if (ecs(uhdr->uh_sport) != TftpRmtPort) {
			SendTFTPErr(ehdr,0,"invalid source port",0);
			return(0);
		}
		if (block == tftpPrevBlock) {	/* If block didn't increment, assume */
			SendTFTPAck(ehdr,block);	/* retry.  Ack it and return here.	*/
			return(0);			/* Otherwise, if block != tftpPrevBlock+1, */
		}						/* return an error, and quit now.	*/
		else if (block != tftpPrevBlock+1) {
			SendTFTPErr(ehdr,0,"Unexpected block number",1);
			TftpCount = -1;
			return(0);
		}
		TftpCount += count;
		oaddr = TftpAddr;
		tftpPrevBlock = block;
		data = (uchar *)(tftpp+4);

		/* If count is less than TFTP_DATAMAX, this must be the last
		 * packet of the transfer, so clean up state here.
		 */
		if (count < TFTP_DATAMAX) {
//			enableBroadcastReception();
			tftpGotoState(TFTPIDLE);
		}

		/* Copy data from enet buffer to TftpAddr location.
		 */
		memcpy((char *)TftpAddr,(char *)data,count);
		TftpAddr += count;

		/* Check for transfer complete (count < TFTP_DATAMAX)... */
		if (count < TFTP_DATAMAX) 
			tftpTransferComplete();
		if ((block & 0xf) == 0xf)
			ticktock();
		break;
	case ecs(TFTP_ERR):
		errcode = ecs(*(ushort *)(tftpp+2));
		errstring = tftpp+4;
		printf("  TFTP_ERR #%d (%s)\n",errcode,errstring);
		TftpCount = -1;
		tftpGotoState(TFTPERROR);
		strncpy(TftpErrString,errstring,sizeof(TftpErrString)-1);
		TftpErrString[sizeof(TftpErrString)-1] = 0;
		return(0);
	default:
		printf("  ??? <%04x> opcode\n", opcode);
		TftpCount = -1;
		return(-1);
	}
	SendTFTPAck(ehdr,block);
	return(0);
}

/* SendTFTPRRQ():
 *	Pass the ether and ip address of the TFTP server, along with the
 *	filename and mode to start up a target-initiated TFTP download.
 *	The initial TftpState value is TFTPSENTRRQ, this is done so that incoming
 *	TFTP_DAT packets can be verified...
 *	 - If a TFTP_DAT packet is received and TftpState is TFTPSENTRRQ, then
 *	   the block number should be 1.  If this is true, then that server's
 *	   source port is stored away in TftpRmtPort so that all subsequent
 *	   TFTP_DAT packets will be compared to the initial source port.  If no
 *	   match, then respond with a TFTP error or ICMP PortUnreachable message.
 *	 - If a TFTP_DAT packet is received and TftpState is TFTPSENTRRQ, then
 *	   if the block number is not 1, generate a error.
 */
static int
SendTFTPRRQ(uchar *ipadd,uchar *eadd,char *filename,uchar *loc)
{
	static char *mode = "octet";
	uchar *tftpdat;
	ushort ip_len;
	struct ether_header *te;
	struct ip *ti;
	struct Udphdr *tu;

	tftpGotoState(TFTPSENTRRQ);
	TftpAddr = loc;
	TftpCount = 0;

	/* Retrieve an ethernet buffer from the driver and populate the
	 * ethernet level of packet:
	 */
	te = (struct ether_header *) getXmitBuffer();
	memcpy((char *)&te->ether_shost,(char *)BinEnetAddr,6);
	memcpy((char *)&te->ether_dhost,(char *)eadd,6);
	te->ether_type = ecs(ETHERTYPE_IP);

	/* Move to the IP portion of the packet and populate it appropriately: */
	ti = (struct ip *) (te + 1);
	ti->ip_vhl = IP_HDR_VER_LEN;
	ti->ip_tos = 0;
	ip_len = sizeof(struct ip) +
	    sizeof(struct Udphdr) + strlen(filename) + strlen(mode) + 4;
	ti->ip_len = ecs(ip_len);
	ti->ip_id = ipId();
	ti->ip_off = 0;
	ti->ip_ttl = UDP_TTL;
	ti->ip_p = IP_UDP;
	memcpy((char *)&ti->ip_src.s_addr,(char *)BinIpAddr,4);
	memcpy((char *)&ti->ip_dst.s_addr,(char *)ipadd,4);

	/* Now udp... */
	tu = (struct Udphdr *) (ti + 1);
	tu->uh_sport = getTftpSrcPort();
	self_ecs(tu->uh_sport);
	tu->uh_dport = ecs(TftpPort);
	tu->uh_ulen = ecs((ushort)(ip_len - sizeof(struct ip)));

	/* Finally, the TFTP specific stuff... */
	tftpdat = (uchar *)(tu+1);
	*(ushort *)(tftpdat) = ecs(TFTP_RRQ);
	strcpy((char *)tftpdat+2,filename);
	strcpy((char *)tftpdat+2+strlen(filename)+1,mode);

	storePktAndSend(ti, te,TFTPACKSIZE+strlen(filename)+strlen(mode));

	return(0);
}

/* SendTFTPAck():
 */
static int
SendTFTPAck(struct ether_header *re,ushort block)
{
	uchar *tftpdat;
	ushort ip_len;
	struct ether_header *te;
	struct ip *ti, *ri;
	struct Udphdr *tu, *ru;

	te = EtherCopy(re);

	ti = (struct ip *) (te + 1);
	ri = (struct ip *) (re + 1);
	ti->ip_vhl = ri->ip_vhl;
	ti->ip_tos = ri->ip_tos;
	ip_len = sizeof(struct ip) + sizeof(struct Udphdr) + 4;
	ti->ip_len = ecs(ip_len);
	ti->ip_id = ipId();
	ti->ip_off = ri->ip_off;
	ti->ip_ttl = UDP_TTL;
	ti->ip_p = IP_UDP;
	memcpy((char *)&(ti->ip_src.s_addr),(char *)&(ri->ip_dst.s_addr),
		sizeof(struct in_addr));
	memcpy((char *)&(ti->ip_dst.s_addr),(char *)&(ri->ip_src.s_addr),
		sizeof(struct in_addr));

	tu = (struct Udphdr *) (ti + 1);
	ru = (struct Udphdr *) (ri + 1);
	tu->uh_sport = ru->uh_dport;
	tu->uh_dport = ru->uh_sport;
	tu->uh_ulen = ecs((ushort)(ip_len - sizeof(struct ip)));

	tftpdat = (uchar *)(tu+1);
	*(ushort *)(tftpdat) = ecs(TFTP_ACK);
	*(ushort *)(tftpdat+2) = ecs(block);

	storePktAndSend(ti,te,TFTPACKSIZE);

	return(0);
}

/* SendTFTPErr():
 */
static int
SendTFTPErr(struct ether_header *re,short errno,char *errmsg,int changestate)
{
	short len, tftplen, hdrlen;
	uchar *tftpmsg;
	struct ether_header *te;
	struct ip *ti, *ri;
	struct Udphdr *tu, *ru;

	if (changestate)
		tftpGotoState(TFTPERROR);

	tftplen = strlen(errmsg) + 1 + 4;
	hdrlen = sizeof(struct ip) + sizeof(struct Udphdr);
	len = tftplen + hdrlen ;

	te = EtherCopy(re);

	ti = (struct ip *) (te + 1);
	ri = (struct ip *) (re + 1);
	ti->ip_vhl = ri->ip_vhl;
	ti->ip_tos = ri->ip_tos;
	ti->ip_len = ecs(len);
	ti->ip_id = ipId();
	ti->ip_off = ri->ip_off;
	ti->ip_ttl = UDP_TTL;
	ti->ip_p = IP_UDP;
	memcpy((char *)&(ti->ip_src.s_addr),(char *)&(ri->ip_dst.s_addr),
		sizeof(struct in_addr));
	memcpy((char *)&(ti->ip_dst.s_addr),(char *)&(ri->ip_src.s_addr),
		sizeof(struct in_addr));

	tu = (struct Udphdr *) (ti + 1);
	ru = (struct Udphdr *) (ri + 1);
	tu->uh_sport = ru->uh_dport;
	tu->uh_dport = ru->uh_sport;
	tu->uh_ulen = sizeof(struct Udphdr) + tftplen;
	self_ecs(tu->uh_ulen);

	tftpmsg = (uchar *)(tu+1);
	*(ushort *)(tftpmsg) = ecs(TFTP_ERR);
	* (ushort *)(tftpmsg+2) = ecs(errno);
	strcpy((char *)tftpmsg+4,errmsg);

	storePktAndSend(ti,te,TFTPACKSIZE + strlen(errmsg) + 1);

	return(0);
}

/* Tftp():
 *	Initiate a tftp transfer at the target (target is client).
 *	Command line:
 *		tftp [options] {IP} {get|put} {file|addr} [len]...
 *		tftp [options] {IP} get file dest_addr
 *		tftp [options] {IP} put addr dest_file len
 *	Currently, only "get" is supported.
 */

char *TftpHelp[] = {
	"TFTP get",
	"{srvr_ip} {hostfile} {address}" ,
	0,
};


int
Tftp(int argc,char *argv[])
{
	ulong	addr;

	if (!enetInitialized())
		return(CMD_FAILURE);

	if (argc != 4)
		return(CMD_PARAM_ERROR);

	addr = (ulong)strtol(argv[3],0,0);
	tftpGet(addr,argv[1],argv[2]);

	return(CMD_SUCCESS);
}

void
ShowTftpStats()
{
}

#endif
