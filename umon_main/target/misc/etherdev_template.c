/* template_etherdev.c:
 * This is a "starter" file for the ethernet driver interface used by
 * the monitor.  The functions are described, but empty.
 * At a minimum, code must be added in all places marked ADD_CODE_HERE.
 * Additional OPT_ADD_CODE_HERE tags indicate locations that can be
 * omitted, but would be nice to have additional facilities.
 *
 * General notice:
 * This code is part of a boot-monitor package developed as a generic base
 * platform for embedded system designs.  As such, it is likely to be
 * distributed to various projects beyond the control of the original
 * author.  Please notify the author of any enhancements made or bugs found
 * so that all may benefit from the changes.  In addition, notification back
 * to the author will allow the new user to pick up changes that may have
 * been made by other users after this version of the code was distributed.
 *
 * Author:	Ed Sutter
 * email:	esutter@lucent.com
 * phone:	908-582-2351
 */

#include "config.h"
#include "genlib.h"
#include "stddefs.h"
#include "ether.h"

#if INCLUDE_ETHERNET

uchar *tx_buff;

/*
 * enreset():
 *	Reset the PHY and MAC.
 */
void
enreset(void)
{
	/* ADD_CODE_HERE */
}

/*
 * eninit():
 * Initialize the PHY and MAC.
 * This would include establishing buffer descriptor tables and
 * all the support code that will be used by the ethernet device.
 *
 * It can be assumed at this point that the array uchar BinEnetAddr[6]
 * contains the 6-byte MAC address.
 *
 * Return 0 if successful; else -1.
 */
int
eninit(void)
{
	/* ADD_CODE_HERE */
	return (0);
}

int
EtherdevStartup(int verbose)
{
	/* Initialize local device error counts (if any) here. */
	/* OPT_ADD_CODE_HERE */

	/* Put ethernet controller in reset: */
	enreset();

	/* Initialize controller: */
	eninit();

	return(0);
}

/* disablePromiscuousReception():
 * Provide the code that disables promiscuous reception.
 */
void
disablePromiscuousReception(void)
{
	/* OPT_ADD_CODE_HERE */
}

/* enablePromiscuousReception():
 * Provide the code that enables promiscuous reception.
 */
void
enablePromiscuousReception(void)
{
	/* OPT_ADD_CODE_HERE */
}

/* disableBroadcastReception():
 * Provide the code that disables broadcast reception.
 */
void
disableBroadcastReception(void)
{
	/* ADD_CODE_HERE */
}

/* enableBroadcastReception():
 * Provide the code that enables broadcast reception.
 */
void
enableBroadcastReception(void)
{
	/* ADD_CODE_HERE */
}

/* 
 * enselftest():
 *	Run a self test of the ethernet device(s).  This can be stubbed
 *	with a return(1).
 *	Return 1 if success; else -1 if failure.
 */
int
enselftest(int verbose)
{
	return(1);
}

/* ShowEtherdevStats():
 * This function is used to display device-specific stats (error counts
 * usually).
 */
void
ShowEtherdevStats(void)
{
	/* OPT_ADD_CODE_HERE */
}

/* getXmitBuffer():
 * Return a pointer to the buffer that is to be used for transmission of
 * the next packet.  Since the monitor's driver is EXTREMELY basic,
 * there will only be one packet ever being transmitted.  No need to queue
 * up transmit packets.
 */
uchar *
getXmitBuffer(void)
{
	/* ADD_CODE_HERE */
	return(tx_buff);
}

/* sendBuffer():
 * Send out the packet assumed to be built in the buffer returned by the
 * previous call to getXmitBuffer() above.
 */
int
sendBuffer(int length)
{
	if (EtherVerbose &  SHOW_OUTGOING)
		printPkt((struct ether_header *)tx_buff,length,ETHER_OUTGOING);

	/* Bump up the packet length to a minimum of 64 bytes.
	 */
	if (length < 64)
		length = 64;

	/* Add the code that will tickle the device into sending out the
	 * buffer that was previously returned by getXmitBuffer() above...
	 */
	/* ADD_CODE_HERE */

	EtherXFRAMECnt++;
	return(0);
}

/* DisableEtherdev():
 * Fine as it is...
 */
void
DisableEtherdev(void)
{
	enreset();
}

/* extGetIpAdd():
 * If there was some external mechanism (other than just using the
 * IPADD shell variable established in the monrc file) for retrieval of
 * the board's IP address, then do it here...
 */
char *
extGetIpAdd(void)
{
	return((char *)0);
}

/* extGetEtherAdd():
 * If there was some external mechanism (other than just using the
 * ETHERADD shell variable established in the monrc file) for retrieval of
 * the board's MAC address, then do it here...
 */
char *
extGetEtherAdd(void)
{
	return((char *)0);
}

/*
 * polletherdev():
 * Called continuously by the monitor (ethernet.c) to determine if there
 * is any incoming ethernet packets.
 *
 * NOTE: this function must be reentrant.
 */
int
polletherdev(void)
{
	uchar *pktbuf = (char *)0;
	int	pktlen = 0, pktcnt = 0;

	while(1)
	{
		/* ADD_CODE_HERE */
		/* If there is a packet, then pull it out of the device and feed it
		 * to processPACKET(); else break.
		 */
		pktcnt++;
		EtherRFRAMECnt++;
		processPACKET((struct ether_header *)pktbuf, pktlen);
	}
	return(pktcnt);
}

#endif
