/* etherdev.c:
 * Hooks needed between uuMon and BF537's ethernet MAC.
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
#if INCLUDE_ENETCORE
#include "genlib.h"
#include "stddefs.h"
#include "ether.h"
#include "phy.h"
#include "cdefBF537.h"
#include "timer.h"

#define ENET_RX_ERR (RX_LONG | RX_ALIGN | RX_CRC | RX_LEN | RX_FRAG | \
					 RX_ADDR | RX_DMAO | RX_PHY | RX_LATE | RX_RANGE)

#define PHY_VERBOSE
#define AUTO_RX_CKSUM	0	/* If set to 1, Blackfin will compute incoming UDP
							 * and TCP checksums (need to add code to take
							 * advantage of this); otherwise set to 0.
							 */
#define PHYAddr 		1
#define MDCFREQ			2000000

#define RXTOT			8
#define TBUFSIZE		1560 
#define RBUFSIZE		1560 

/* Structure from HW Reference Manual...
 */
struct dma_descriptor {
	struct dma_descriptor		*next;
	ulong						*startaddr;
	ushort						config;
};

/* Transmit & receive data buffers:
 * TODO: consider using L1-resident ethernet buffers.
 */

int rxidx;


#ifdef TX_STAT_BASE
vulong *pTxStat = (vulong *)TX_STAT_BASE;
vulong *pTxData = (vulong *)TX_DATA_BASE;
vulong (*pRxStat)[2] = RX_STAT_BASE;
vulong (*pRxData)[(RBUFSIZE/sizeof(long))] = RX_DATA_BASE;
#else
vulong tx_stat;
vulong tx_data[(TBUFSIZE/sizeof(long))];
vulong rx_stat[RXTOT][2];
vulong rx_data[RXTOT][(RBUFSIZE/sizeof(long))];
vulong *pTxStat = &tx_stat;
vulong *pTxData = tx_data;
vulong (*pRxStat)[2] = rx_stat;
vulong (*pRxData)[(RBUFSIZE/sizeof(long))] = rx_data;
#endif

#ifdef TX_DMA_DESCRIPTOR_BASE
struct dma_descriptor *txd = (struct dma_descriptor *)TX_DMA_DESCRIPTOR_BASE;
#else
struct dma_descriptor txd[2];
#endif

#ifdef RX_DMA_DESCRIPTOR_BASE
struct dma_descriptor *rxd = (struct dma_descriptor *)RX_DMA_DESCRIPTOR_BASE;
#else
struct dma_descriptor rxd[RXTOT*2];
#endif

int linkcheck(void);

/*
 * enreset():
 *	Reset the PHY and MAC.
 */
void
enreset(void)
{
	phy_reset(0);

	*pEMAC_OPMODE = 0x00000000;
	*pDMA1_CONFIG = 0x0000;
	*pDMA2_CONFIG = 0x0000;
}

/*
 * eninit():
 * Initialize the PHY and MAC.
 * These steps are based on the section titled "Programming Model"
 * in chapter 8 (Ethernet MAC) of the hardware reference manual.
 *
 * Return 0 if successful; else -1.
 */
static int
eninit(void)
{
	ushort sval, phyreg;
	ulong lval, tmp;

	*pVR_CTL |= CLKBUFOE;		/* CLKBUF */

	tmp = *pPORTH_FER;			/* Read it once (bug work-around) */
	*pPORTH_FER = 0xffff;		/* GPIO pin multiplexing (write it twice) */
	*pPORTH_FER = 0xffff;
	
	/* Load MAC address into device...
	 */
	memcpy((char *)&lval,(char *)BinEnetAddr,4);
	*pEMAC_ADDRLO = lval;
	memcpy((char *)&sval,(char *)BinEnetAddr+4,2);
	*pEMAC_ADDRHI = sval;

	/* Initialize SYSCTL:
	 */
#if AUTO_RX_CKSUM
	*pEMAC_SYSCTL = (((SYSCLKFREQ/MDCFREQ)/2 - 1) << 8) | (RXDWA | RXCKS);
#else
	*pEMAC_SYSCTL = (((SYSCLKFREQ/MDCFREQ)/2 - 1) << 8) | RXDWA;
#endif

	/* PHY setup:
	 * The PHY is an SMSC LAN83C185 (on the board used for this port),
	 * the PHY ID should be 0x0007c0aX (X = revision).
	 */
	if (phy_id(0,&lval) == -1) {
		printf("phy access failure\n");
		return(-1);
	}

	/* Can't get proper link status out of the PHY immediately after reset.
	 * So, always invoke auto-negotiation...
	 */
	phy_autoneg(0);

	/* Setup MAC:
	 * If PHY is configured full duplex, then we wanna setup the
	 * MAC for full duplex...
	 */
	phy_read(0,PHY_CTRL,&phyreg);
	if (phyreg & PHY_CTRL_FDX)
		*pEMAC_OPMODE = ASTP | DRO | FDMODE;
	else
		*pEMAC_OPMODE = ASTP | DRO;

	/* Establish/reset management counters:
	 */
	*pEMAC_MMC_CTL = MMCE | CCOR | RSTC;
	*pEMAC_MMC_CTL = MMCE | CCOR;
	
	/* Establish transmit and receive descriptors:
	 */
	*pTxStat = 0;
	txd[0].next = &txd[1];
	txd[0].startaddr = (ulong *)pTxData;
	txd[0].config = DMAEN | NDSIZE_5 | FLOW_LARGE | WDSIZE_32;
	txd[1].next = &txd[0];
	txd[1].startaddr = (ulong *)pTxStat;
	txd[1].config = DMAEN | DI_EN | WNR | WDSIZE_32;
	
	for(rxidx=0;rxidx<RXTOT;rxidx++) {
		pRxStat[rxidx][0] = 0;
		pRxStat[rxidx][1] = 0;
	}
	for(rxidx=0;rxidx<RXTOT*2;rxidx+=2) {
		rxd[rxidx].next = &rxd[rxidx+1];
		rxd[rxidx].startaddr = (ulong *)pRxData[rxidx/2];
		rxd[rxidx].config = WNR | DMAEN | NDSIZE_5 | FLOW_LARGE | WDSIZE_32;
		rxd[rxidx+1].next = &rxd[rxidx+2];
		rxd[rxidx+1].startaddr = (ulong *)&pRxStat[rxidx/2][0];
		rxd[rxidx+1].config =
			WNR | DMAEN | DI_EN | NDSIZE_5 | FLOW_LARGE | WDSIZE_32;
	}
	rxd[(RXTOT*2)-1].next = &rxd[0];
	rxidx = 0;

	/* Init DMA:
	 * DMA_1 is for ethernet receive
	 * DMA_2 is for ethernet transmit
	 */
	*pDMA1_X_COUNT  = 0;
	*pDMA1_X_MODIFY = 4;
	*pDMA1_Y_COUNT  = 0;
	*pDMA1_Y_MODIFY = 0;
	*pDMA2_X_COUNT  = 0;
	*pDMA2_X_MODIFY = 4;
	*pDMA2_Y_COUNT  = 0;
	*pDMA2_Y_MODIFY = 0;
	*pDMA1_NEXT_DESC_PTR = rxd;
	*pDMA1_CONFIG = DMAEN | WNR | NDSIZE_5 | FLOW_LARGE | WDSIZE_32;

	/* Once everything else is initialized, enable xmt & rcv...
	 */
	*pEMAC_OPMODE |= (TE | RE);
	return (0);
}

int
EtherdevStartup(int verbose)
{
	static char beenhere;

	/* On the very first pass through this function we don't bring up
	 * the MAC.  Later in the initialization, polletherdev() will be
	 * called and it will detect the link-up status and automatically
	 * call eninit() to invoke auto-negotiation.
	 */
	if (!beenhere) {
		beenhere = 1;
		return(0);
	}

	/* Put ethernet controller in reset: */
	enreset();

	/* Initialize controller and return the value returned by
	 * eninit().
	 */
	return(eninit());
}

void
disableMulticastReception(void)
{
	*pEMAC_OPMODE &= ~PAM;
}

void
enableMulticastReception(void)
{
	*pEMAC_OPMODE |= PAM;
}

/* disablePromiscuousReception():
 * Provide the code that disables promiscuous reception.
 */
void
disablePromiscuousReception(void)
{
	*pEMAC_OPMODE &= ~PR;
}

/* enablePromiscuousReception():
 * Provide the code that enables promiscuous reception.
 */
void
enablePromiscuousReception(void)
{
	*pEMAC_OPMODE |= PR;
}

/* disableBroadcastReception():
 * Provide the code that disables broadcast reception.
 */
void
disableBroadcastReception(void)
{
	*pEMAC_OPMODE |= DBF;
}

/* enableBroadcastReception():
 * Provide the code that enables broadcast reception.
 */
void
enableBroadcastReception(void)
{
	*pEMAC_OPMODE &= ~DBF;
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
	printf("Self-test not yet available for this driver.\n");
	return(1);
}

/* ShowEtherdevStats():
 * This function is used to display device-specific stats (error counts
 * usually).
 */
void
ShowEtherdevStats(void)
{
	ushort phyreg;

	/* Read the register twice because the STAT bit is sticky,
	 * and we want to make sure that we are showing the current status.
	 */
	phy_read(0,PHY_STAT,&phyreg);
	phy_read(0,PHY_STAT,&phyreg);
	if (phyreg & PHY_STAT_LINKUP) {
		phy_read(0,PHY_CTRL,&phyreg);
		printf("PHY: %dMbps, %s-duplex\n",
			phyreg & PHY_CTRL_100MB ? 100 : 10,
			phyreg & PHY_CTRL_FDX ? "Full" : "Half");
	}
	else {
		printf("PHY: link is down\n");
	}
}

/* getXmitBuffer():
 * Return a pointer to the buffer that is to be used for transmission of
 * the next packet.  The monitor will never need more than one packet at
 * a time; hence, we just keep giving out the same pointer.
 * Note that for the BF537, the first two bytes of the buffer are used
 * by the DMA engine for the length of the packet; so this function actually
 * hands the requestor a pointer incremented by two from the base of tx_data.
 */
uchar *
getXmitBuffer(void)
{
	uchar *cp;
    struct elapsed_tmr tmr;

	/* Since we're only using one transmit buffer, we need to make sure
 	 * that the previous packet has been sent prior to giving the pointer
	 * up for re-use...
	 */
    startElapsedTimer(&tmr,5000);
	while(*pDMA2_IRQ_STATUS & DMA_RUN) {
    	if(msecElapsed(&tmr)) {
			printf("Wait-for-DMA_RUN timeout\n");
			break;
		}
	}
	cp = (uchar *)pTxData;
	cp += 2;
	return(cp);
}

/* sendBuffer():
 * Send out the packet assumed to be built in the buffer returned by the
 * previous call to getXmitBuffer() above.
 *
 * This driver uses register based DMA for ethernet transmit simply
 * because uMon only needs one transmit buffer; hence, there's really
 * no need for the added complexity of the descriptor model.
 * Note that DMA_2 is for ethernet transmit.
 */
int
sendBuffer(int length)
{
	struct elapsed_tmr tmr;
	int linkcheck(void);
	static char beenhere;
	int sentlen;
	ushort *sp;

	if (!beenhere) {
		beenhere = 1;
		if (linkcheck() == 0)
				return(0);
		}

	sp = (ushort *)pTxData;

	asm("ssync;");
#if INCLUDE_ETHERVERBOSE
	if (EtherVerbose &  SHOW_OUTGOING)
		printPkt((struct ether_header *)(sp+1),length,ETHER_OUTGOING);
#endif

	*pTxStat = 0;
	asm("ssync;");
	*sp = length;
	asm("ssync;");
	*pDMA2_NEXT_DESC_PTR = txd;
	asm("ssync;");
	*pDMA2_CONFIG = DMAEN | NDSIZE_5 | FLOW_LARGE | WDSIZE_32;
	asm("ssync;");

    startElapsedTimer(&tmr,2500);
	while((*pTxStat & TX_COMP) == 0) {
		asm("ssync;");
    	if(msecElapsed(&tmr)) {
			printf("EMAC: TX_COMP timeout\n");
			return(0);
		}
	}
	asm("ssync;");
//	if ((*pTxStat & TX_OK) == 0) 
//		printf("EMAC txerr 0x%lx\n",*pTxStat);

	sentlen = (*pTxStat & TX_FRLEN) >> 16;
	
	asm("ssync;");
	if (sentlen < length) {
		printf("EMAC warning: sent count (%d) < send request (%d)\n",
			sentlen,length);
	}

	asm("ssync;");
	*pTxStat = 0;
	return(0);
}

/* DisableEtherdev():
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
#if PLATFORM_PCORE537
	char *RetVal = DEFAULT_ETHERADD;				// To return from.
	static char MacAddress[18];

	eeprom_init();
	eeprom_getMAC(BinEnetAddr);
	EtherToString(BinEnetAddr, MacAddress);
	RetVal = MacAddress;
	return RetVal;
#else
	return((char *)0);
#endif
}

int
linkcheck(void)
{
	ushort tmp;
	static ushort phystat;

	phy_read(0,PHY_STAT,&tmp);
	if ((tmp & PHY_STAT_LINKUP) && ((phystat & PHY_STAT_LINKUP) == 0)) {
#if INCLUDE_ETHERVERBOSE
		if (EtherVerbose &  SHOW_DRIVER_DEBUG)
			printf("\nEMAC: link up\n");
#endif
		enreset();
		eninit();
		phystat = tmp;
	}
	else if (((tmp & PHY_STAT_LINKUP) == 0) && (phystat & PHY_STAT_LINKUP)) {
#if INCLUDE_ETHERVERBOSE
		if (EtherVerbose &  SHOW_DRIVER_DEBUG)
			printf("\nEMAC: link down\n");
#endif
		phystat = tmp;
	}

	/* Return non-zero if link is up...
	 */
	return(phystat & PHY_STAT_LINKUP);
}

/* rxbuf_check():
 * This function was written to help diagnose a problem with this driver
 * that appears to occur when an input buffer overflow occurs.
 */
void
rxbuf_check(void)
{
	int i, idx;
	volatile ulong stat;

#ifdef RXCHECK_VERBOSE
	static int tag;

	tag++;
#endif

	idx = rxidx+1;
	if (idx == RXTOT)
		idx = 0;

	for(i=0;i<RXTOT;i++) {
		stat = pRxStat[idx][AUTO_RX_CKSUM];
		if (stat & RX_COMP) {
#ifdef RXCHECK_VERBOSE
			printf("(%08x/%d) rx_comp: %d\n",tag,rxidx,idx);
#endif
			if (idx != rxidx) {
				if (++rxidx == RXTOT)
					rxidx = 0;
				return;
			}
		}
#ifdef RXCHECK_VERBOSE
		if (stat & ENET_RX_ERR) 
			printf("(%08x/%d) STAT (%d): 0x%lx\n",tag,rxidx,stat);
#endif
		if (++idx == RXTOT)
			idx = 0;
	}
}

/*
 * polletherdev():
 * Called continuously by the monitor (ethernet.c) to determine if there
 * is any incoming ethernet packets.
 *
 * NOTES:
 * 1. This function must be reentrant, because there are a few cases in
 *    processPACKET() where pollethernet() may be called.
 * 2. It should only process one packet per call.  This is important
 *    because if allowed to stay here to flush all available packets,
 *    it may starve the rest of the system (especially in cases of heavy
 *    network traffic).
 * 3. There are cases in the monitor's execution that may cause the
 *    polling polletherdev() to cease for several seconds.  Depending on
 *    network traffic, this may cause the input buffering mechanism on
 *    the ethernet device to overflow.  A robust polletherdev() function
 *    should support this gracefully (i.e. when the error is detected,
 *    attempt to pass all queued packets to processPACKET(), then do what
 *    is necessary to clear the error).
 *
 * KNOWN PROBLEM with BF537:
 * Apparently the DMA engine behind the EMAC does not deal with a buffer
 * overflow condition on this device.  After several conversations (email)
 * with the processor tech support at analog devices, I've been told that
 * the DMA engine will just fill up the buffers pointed to by the circular
 * queue of buffer descriptors, and then blindly overwrite a buffer if
 * a burst of packets come in prior to the firmware being able to flush
 * them from the queue.  IMHO, this is a poor design.  Typically, there is
 * some kind of "in-use" bit in the buffer descriptor structure that will
 * let the backend DMA engine know that it should not overwrite the buffer
 * that the current descriptor is pointing to.
 * Not the case with this device, so all we can do to reduce the likelihood
 * that an overflow will occur is to increase the size of RXTOT (above).
 */


int
polletherdev(void)
{
	char *cp;
	volatile ulong stat;
	int	pktcnt = 0, this_rxidx;

	stat = pRxStat[rxidx][AUTO_RX_CKSUM];

	/* These error states are queried, but I don't expect them to occur;
	 * nevertheless, I need to add error-handler code.
	 */
	if (*pDMA1_IRQ_STATUS & DMA_ERR)
		printf("polletherdev DMAERROR\n");

	if (*pEMAC_SYSTAT &  (RXDMAERR | TXDMAERR))
		printf("polletherdev EMAC_SYSTAT: 0x%lx\n",*pEMAC_SYSTAT);

	if (stat & ENET_RX_ERR) 
		printf("polletherdev STAT: 0x%lx\n",stat);
	
	/* Query for an incoming packet...
	 */
	if (stat & RX_COMP) {
		if (stat & RX_OK) {
			cp = (char *)pRxData[rxidx];
			cp += 2;

#ifdef RXCHECK_VERBOSE
			printf("rx_ok: %d (cdp=0x%lx,ndp=0x%lx)\n",
				rxidx, *pDMA1_CURR_DESC_PTR, *pDMA1_NEXT_DESC_PTR);
#endif

			/* FIX (hopefully): rxidx is incremented prior to calling
			 * processPACKET() so that nested calls to polletherdev()
			 * will deal with the correct rxbuffer index.
			 */
			this_rxidx = rxidx;
			if (++rxidx == RXTOT)
				rxidx = 0;

			processPACKET((struct ether_header *)cp, (stat & RX_FRLEN));

			*pDMA1_IRQ_STATUS |= (DMA_DONE | DMA_ERR);
			pRxStat[this_rxidx][AUTO_RX_CKSUM] = 0;

			pktcnt++;
		}
		else {
			printf("polletherdev COMPnotOK: 0x%lx\n",stat);

			*pDMA1_IRQ_STATUS |= (DMA_DONE | DMA_ERR);
			pRxStat[rxidx][AUTO_RX_CKSUM] = 0;

			if (++rxidx == RXTOT)
				rxidx = 0;
		}
	}
	else {
		rxbuf_check();
	}

	return(pktcnt);
}

/*************************************************************************
 * PHY interface:
 *************************************************************************
 */

/* phy_busy_poll():
 * Wait in loop (with timeout) until PHY isn't busy.
 */
int
phy_busy_poll(void)
{
    struct elapsed_tmr tmr, tmr1;

	/* Wait till previous MDC/MDIO transaction has completed.
     */
    startElapsedTimer(&tmr,5000);
	while(*(vushort *)EMAC_STAADD & STABUSY) {
    	if(msecElapsed(&tmr)) {
			printf("phy_busy_poll timeout\n");
			return(-1);
		}
    	startElapsedTimer(&tmr1,10);
    	while(!msecElapsed(&tmr1));
	}
	return(0);
}

/* phy_read():
 * Read an off-chip register in a PHY through the MDC/MDIO port:
 */
int
phy_read(int phy, int reg, unsigned short *value)
{
	if (phy_busy_poll() < 0)
		return(-1);

	*(vulong *)EMAC_STAADD = (SET_PHYAD(PHYAddr) | SET_REGAD(reg) | STABUSY);

	if (phy_busy_poll() < 0)
		return(-1);

	*value = (ushort) *(vulong *)EMAC_STADAT;
	return(0);
}


/* phy_write():
 * Write an off-chip register in a PHY through the MDC/MDIO port:
 */
int
phy_write(int phy, int reg, unsigned short value)
{
	if (phy_busy_poll() < 0)
		return(-1);

	*(vulong *)EMAC_STADAT = value;

	*(vulong *)EMAC_STAADD =
		(SET_PHYAD(PHYAddr) | SET_REGAD(reg) | STAOP | STABUSY);

	return(0);
}

/* phy_reset():
 * Reset the phy.
 */
int
phy_reset(int phy)
{
	ushort ctrl;
    struct elapsed_tmr tmr;

	phy_read(phy,PHY_CTRL,&ctrl);
	ctrl |= PHY_CTRL_RST;
	phy_write(phy,PHY_CTRL,ctrl);
    startElapsedTimer(&tmr,10);
	while(1) {
		if (phy_read(phy,PHY_CTRL,&ctrl) < 0)
			return(-1);
		if ((ctrl & PHY_CTRL_RST) == 0)
			break;
    	if (msecElapsed(&tmr)) {
			printf("phy reset timeout\n");
			return(-1);
		}
    }
	return(0);
}

/* phy_autoneg():
 * Go through the auto-negotiation process...
 */
int
phy_autoneg(int phy)
{
	volatile int retries;
	ushort ctrl, stat, autoneg, par;

#if INCLUDE_ETHERVERBOSE
	if (EtherVerbose &  SHOW_DRIVER_DEBUG)
		printf("\nPHY Autonegotiate... ");
#endif

	retries = 1000;
	autoneg = PHY_ANAD_802_3;

	phy_read(phy,PHY_STAT,&stat);
	if((stat & PHY_STAT_ANABLE) == 0) {
		printf("phy does not support autoneg\n");
		return (-1);
	}
	
	/* Set appropriate advertisement bits based on this
	 * phy's capabilities...
	 */
	if (stat & PHY_STAT_100FDX)
		autoneg |= PHY_ANAD_100FDX;
	else if (stat & PHY_STAT_10FDX)
		autoneg |= PHY_ANAD_10FDX;
	else if (stat & PHY_STAT_100HDX)
		autoneg |= PHY_ANAD_100HDX;
	else if (stat & PHY_STAT_10HDX)
		autoneg |= PHY_ANAD_10HDX;
	phy_write(phy,PHY_ANAD, autoneg);

	phy_read(phy,PHY_CTRL,&ctrl);
	ctrl &= ~(PHY_CTRL_LBK | PHY_CTRL_PWRDN);
	ctrl &= ~(PHY_CTRL_ISOLATE | PHY_CTRL_COLTST);
	ctrl |= (PHY_CTRL_ANE);

	phy_write(phy,PHY_CTRL, ctrl);
	ctrl |= PHY_CTRL_ANRESTART; 
	phy_write(phy,PHY_CTRL, ctrl);

	do {
		phy_read(phy,PHY_STAT,&stat);
		if((stat & PHY_STAT_REMFAULT) != 0) {
			printf("phy: remote fault\n");
			return -1;
		}
		if (--retries <= 0) {
#if INCLUDE_ETHERVERBOSE
			if (EtherVerbose &  SHOW_DRIVER_DEBUG)
				printf("phy: AN timeout\n");
#endif
			return -1;
		}
	} while((stat & PHY_STAT_ANDONE) == 0);

	phy_read(phy,PHY_ANLPAR,&par);

	if((par & PHY_ANLP_REMFAULT) != 0) {
		printf("phy: remote fault\n");
		return -1;
	}

	phy_read(phy,PHY_CTRL,&ctrl);
	ctrl &= (~PHY_CTRL_FDX | PHY_CTRL_100MB);

	phy_read(phy,PHY_STAT,&stat);
	if ((stat & PHY_STAT_100FDX) && (par & PHY_ANLP_100FDX))
		ctrl |= (PHY_CTRL_100MB | PHY_CTRL_FDX);
	else if ((stat & PHY_STAT_10FDX) && (par & PHY_ANLP_10FDX))
		ctrl |= PHY_CTRL_FDX;
	else if ((stat & PHY_STAT_100HDX) && (par & PHY_ANLP_100HDX))
		ctrl |= PHY_CTRL_100MB;

	phy_write(phy,PHY_CTRL, ctrl);

#if INCLUDE_ETHERVERBOSE
	if (EtherVerbose &  SHOW_DRIVER_DEBUG) {
		printf("%dMbps %s-duplex\n",
			ctrl & PHY_CTRL_100MB ? 100 : 10,
			ctrl & PHY_CTRL_FDX ? "Full" : "Half");
	}
#endif
	return 0;
}

/* phy_id():
 * Return the concatenation of the hi and lo phy ID registers (16 bits each)
 * as a single 32-bit value.
 */
int
phy_id(int phy, ulong *id)
{
	ushort idhi, idlo;

	if (phy_read(phy,PHY_IDHI,&idhi) < 0)
		return(-1);
	if (phy_read(phy,PHY_IDLO,&idlo) < 0)
		return(-1);
	*id = (((ulong)idhi << 16) | idlo);
	//printf("phyid: 0x%lx\n",*id);
	return(0);
}

#endif
