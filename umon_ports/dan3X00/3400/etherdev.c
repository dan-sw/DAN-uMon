/**********************************************************************\
 Library    :  uMON (Micro Monitor)
 Filename   :  etherdev.c
 Purpose    :  Ethernet driver port for DAN3400 GMAC 
 Owner		:  Sergey Krasnitsky
 Created    :  21.11.2010

 Note :
	This file is written based on template_etherdev.c, the template file 
	for building an Ethernet driver for the uMON. Refer to the original 
	umon_main\target\misc\etherdev_template.c for naming conventions and 
	other details regarding uMon's Ethernet device driver interface.
\**********************************************************************/

#include "config.h"
#include "genlib.h"
#include "stddefs.h"
#include "ether.h"
#include "gmac_mon.h"


#if INCLUDE_ETHERNET

// Dummy MAC addr needed by target\common\ethernet.c compilation
uint8 etheraddr[] = { 0xff };

uint8* ethLastTxPacket;

/*
 *	Reset the PHY and MAC.
 */
void enreset (void)
{
	/* ADD_CODE_HERE */
}


/*
 * Initialize the PHY and MAC.
 * This would include establishing buffer descriptor tables and
 * all the support code that will be used by the ethernet device.
 *
 * It can be assumed at this point that the array uchar BinEnetAddr[6]
 * contains the 6-byte MAC address.
 *
 * Return 0 if successful; else -1.
 */
int eninit (void)
{
	gmac_Init(0,BinEnetAddr);
	return 0;
}


int EtherdevStartup (int verbose)
{
	/* Initialize local device error counts (if any) here. */
	/* OPT_ADD_CODE_HERE */

	/* Put ethernet controller in reset: */
	enreset();

	/* Initialize controller: */
	eninit();

	return 0;
}


/* Provide the code that disables promiscuous reception */
void disablePromiscuousReception (void)
{
	/* OPT_ADD_CODE_HERE */
}


/* Provide the code that enables promiscuous reception */
void enablePromiscuousReception (void)
{
	/* OPT_ADD_CODE_HERE */
}


/* Provide the code that disables broadcast reception */
void disableBroadcastReception (void)
{
	/* ADD_CODE_HERE */
}


/* Provide the code that enables broadcast reception */
void enableBroadcastReception (void)
{
	/* ADD_CODE_HERE */
}

/* 
 * Run a self test of the ethernet device(s).  
 * This can be stubbed with a return(1).
 * Return 1 if success; else -1 if failure.
 */
int enselftest (int verbose)
{
	return 1;
}


/* This function is used to display device-specific stats (error counts usually) */
void ShowEtherdevStats (void)
{
	/* OPT_ADD_CODE_HERE */
}


/* Return a pointer to the buffer that is to be used for transmission of
 * the next packet.  Since the monitor's driver is EXTREMELY basic,
 * there will only be one packet ever being transmitted.  No need to queue
 * up transmit packets.
 */
uchar* getXmitBuffer (void)
{
	return (ethLastTxPacket = gmac_PacketAlloc());
}


/* Send out the packet assumed to be built in the buffer returned by the
 * previous call to getXmitBuffer() above.
 */
int sendBuffer (int length)
{
	if (EtherVerbose & SHOW_OUTGOING)
		printPkt ((struct ether_header*)ethLastTxPacket, length, ETHER_OUTGOING);

	/* Bump up the packet length to a minimum of 64 bytes */
	if (length < 64)
		length = 64;

	/* Add the code that will tickle the device into sending out the
	 * buffer that was previously returned by getXmitBuffer() above... */
	gmac_Send (0, (uint8*) ethLastTxPacket, length);

	EtherXFRAMECnt++;
	return(0);
}


/* Fine as it is... */
void DisableEtherdev (void)
{
	enreset();
}


/* If there was some external mechanism (other than just using the
 * IPADD shell variable established in the monrc file) for retrieval of
 * the board's IP address, then do it here...
 */
char* extGetIpAdd (void)
{
	return 0;
}


/* If there was some external mechanism (other than just using the
 * ETHERADD shell variable established in the monrc file) for retrieval of
 * the board's MAC address, then do it here...
 */
char* extGetEtherAdd (void)
{
	return 0;
}


/*
 * Called continuously by the monitor (ethernet.c) to determine if there
 * is any incoming ethernet packets.
 * NOTE: this function must be reentrant.
 */
int polletherdev (void)
{
	int pktcnt = 0;

	while (1)
	{
		/* If there is a packet, then pull it out of the device and feed it
		   to processPACKET(); else break */
		unsigned pktlen;
		uchar   *pktbuf = gmac_Recv (0, &pktlen);
		if (!pktbuf)
			break;
		pktcnt++;
		EtherRFRAMECnt++;
		processPACKET ((struct ether_header*)pktbuf, pktlen);
		gmac_PacketFree(pktbuf);
	}
	return pktcnt;
}


#endif // INCLUDE_ETHERNET
