#include "config.h"
#if INCLUDE_ETHERNET
#include "genlib.h"
#include "stddefs.h"
#include "phy.h"

/* phy_dump():
 * Dump the base set of PHY registers..
 */
int
phy_dump(int phy)
{
	int i;
	ushort data;

	for(i=0; i<0x1A; i++) { 
		if(phy_read(phy, i, &data) == -1) {
			printf("read error for reg 0x%02x\n",i);
			return(-1);
		}  
		printf("Phy_%d reg 0x%02x ==> 0x%x\n",phy,i,data); 
	}
	return(0);
}

/* phy_stat():
 * If verbose is set, then dump the state of the link.
 * Return 1 if link is up, 0 if link is down, -1 if the
 * phy access fails.
 */
int
phy_stat(int phy, int verbose)
{
	int speed;
	char *duplex;
	ushort axcs, stat;

	/* We give the link state bit two chances because on the first
	 * read of the status register it may indicate link down only to
	 * inform the user that at some time since the last read of the
	 * status register the link temporarily went down.  The second
	 * read will properly indicate the current link status.
	 */
	if (phy_read(phy,PHY_STAT,&stat) < 0)
		return(-1);
	if ((stat & PHY_STAT_LINKUP) == 0) {
		if (phy_read(phy,PHY_STAT,&stat) < 0)
			return(-1);
	}
	if (stat & PHY_STAT_LINKUP) {
		if (verbose == 0)
			return(1);

		if (phy_read(phy,PHY_AXCS,&axcs) < 0)
			return(-1);

//		if ((axcs & PHY_AXCS_ANI) == 0)
//			printf("Auto-negotiation is not enabled\n");

		if (axcs & PHY_AXCS_FDXI)
			duplex = "full";
		else
			duplex = "half";
		if (axcs & PHY_AXCS_SPEED100)
			speed = 100;
		else
			speed = 10;
		
		printf("Link is up (%dMbps %s duplex)\n",speed,duplex);
		return(1);
	}
	else {
		if (verbose) 
			printf("Link is down\n");
		
		return(0);
	}
}

/* phy_duplex():
 * Return HALF_DUPLEX or FULL_DUPLEX if link is up.
 * Return 0 if link is down.
 * Return -1 if phy access failure.
 */
int
phy_duplex(int phy)
{
	ushort axcs, stat;

	/* See notes in phy_stat() regarding retry for link status.
	 */
	if (phy_read(phy,PHY_STAT,&stat) < 0)
		return(-1);
	if ((stat & PHY_STAT_LINKUP) == 0) {
		if (phy_read(phy,PHY_STAT,&stat) < 0)
			return(-1);
	}
	if (stat & PHY_STAT_LINKUP) {
		if (phy_read(phy,PHY_AXCS,&axcs) < 0)
			return(-1);

		if (axcs & PHY_AXCS_FDXI)
			return(FULL_DUPLEX);
		else
			return(HALF_DUPLEX);
	}
	return(0);
}


/* phy_speed():
 * If link is up return 10, 100.
 * If link is down return 0.
 * If phy access fails return -1.
 */
int
phy_speed(int phy)
{
	ushort axcs, stat;

	/* See notes in phy_stat() regarding retry for link status.
	 */
	if (phy_read(phy,PHY_STAT,&stat) < 0)
		return(-1);
	if ((stat & PHY_STAT_LINKUP) == 0) {
		if (phy_read(phy,PHY_STAT,&stat) < 0)
			return(-1);
	}
	if (stat & PHY_STAT_LINKUP) {
		if (phy_read(phy,PHY_AXCS,&axcs) < 0)
			return(-1);

		if (axcs & PHY_AXCS_SPEED100)
			return(100);
		else
			return(10);
		
	}
	return(0);
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
	return(0);
}

/* phy_autoneg():
 * Go through the auto-negotiation process...
 */
int
phy_autoneg(int phy)
{
	ushort ctrl, stat, autoneg;
	int is100Mbps, isFullDuplex, retries;

	retries = 10000;
	is100Mbps = isFullDuplex = 0;

	autoneg = PHY_ANAD_802_3;
	phy_read(phy,PHY_CTRL,&ctrl);
	ctrl |= PHY_CTRL_RST;
	phy_write(phy,PHY_CTRL,ctrl);
	while(1) {
		phy_read(phy,PHY_CTRL,&ctrl);
		if ((ctrl & PHY_CTRL_RST) == 0)
			break;
    }

	ctrl &= ~(PHY_CTRL_LBK | PHY_CTRL_PWRDN);
	ctrl &= ~(PHY_CTRL_ISOLATE | PHY_CTRL_COLTST);
	ctrl |= (PHY_CTRL_ANE);

	phy_write(phy,PHY_CTRL, ctrl);
	phy_read(phy,PHY_STAT,&stat);

	/* This driver assumes the PHY is capable of auto-negotiation
	 */
	if((stat & PHY_STAT_ANABLE) == 0) {
		printf("AutoNegotiate: PHY does not support AUTO Negotiation\n");
		return (-1);
	}

	/* Determine PHY capabilities to be advertised during auto-negotiation
	 */
	if((stat & PHY_STAT_100FDX) && (is100Mbps)) {
		autoneg |= PHY_ANAD_100HDX;
		if((stat & PHY_STAT_100FDX) && (isFullDuplex))
			autoneg |= PHY_ANAD_100FDX;
    }

	if (stat & PHY_STAT_10HDX) {
		autoneg |= PHY_ANAD_10HDX;
		if((stat & PHY_STAT_10FDX) && (isFullDuplex))
			autoneg |= PHY_ANAD_10FDX;
	}

	phy_write(phy,PHY_ANAD, autoneg);

	/* Restart auto-negotiation process
	 */
	phy_read(phy,PHY_CTRL,&ctrl);
	ctrl |= PHY_CTRL_ANRESTART; 
	phy_write(phy,PHY_CTRL, ctrl);

	do {
		phy_read(phy,PHY_STAT,&stat);

		if((stat & PHY_STAT_REMFAULT) != 0) {
			printf("Autonegotiate: remote fault\n");
			return -1;
		}
		retries -= 1;                      /* Decrement retry count */
	}
	while((retries != 0) && ((stat & PHY_STAT_ANDONE) == 0));

	phy_read(phy,PHY_ANLPAR,&autoneg);

	if((autoneg & PHY_ANLP_REMFAULT) != 0) {
		printf("Autonegotiate: remote fault\n");
		return -1;
	}

	phy_read(phy,PHY_CTRL,&ctrl);

	/* Select 100 Mbps if this PHY is 100Base-TX capable, the driver asked for
	 * 100Mbps and not only 10Mbps, and the link partner responded with
	 * 100Base-TX or did not respond at all. The "no response" behavior is
	 * done to allow a wire to be plugged in later and have the PHY negotiate
	 * the best speed.
	 */
	if((stat & PHY_STAT_100FDX) && (is100Mbps == 1) &&
		((autoneg & PHY_ANLP_100HDX) || (!(autoneg & PHY_ANLP_ACK))))
	{
		ctrl |= PHY_CTRL_100MB;
		is100Mbps = 1;
	}
	else	/* Use only 10Mbps, because of options or link partner */
	{
		ctrl &= (~(PHY_CTRL_100MB));
		is100Mbps = 0;
	}

	if ((autoneg & PHY_ANLP_ACK) && (isFullDuplex) &&
		(((is100Mbps) && (stat & PHY_STAT_100FDX) &&
		 (autoneg & PHY_ANLP_100FDX)) ||
		((is100Mbps == 0) && (stat & PHY_STAT_10FDX) &&
		 (autoneg & PHY_ANLP_10FDX))))
	{
		/* Select full duplex if link partner responded and both the link
		 * partner and this PHY are full duplex capable
		 */
		ctrl |= PHY_CTRL_FDX;
		isFullDuplex = 1;
	}
	else	/* Use only half duplex, because of options or link partner */
	{
		ctrl &= (~(PHY_CTRL_FDX));
		isFullDuplex = 0;
	}

	phy_write(phy,PHY_CTRL, ctrl);

	if(retries == 0) {
		printf("Autonegotiate: Timeout\n");
		return -1;
	}
	return 0;
}

#endif
