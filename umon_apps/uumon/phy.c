#include "config.h"
#if INCLUDE_PHY
#include "genlib.h"
#include "stddefs.h"
#include "phy.h"

/* phy_autoneg():
 * Go through the auto-negotiation process...
 */
int
phy_autoneg(int phy)
{
	volatile int retries;
	ushort ctrl, stat, autoneg;
	int is100Mbps, isFullDuplex;

	retries = 100000;
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
		printf("Phy can't aneg\n");
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
			printf("Aneg: rmt flt\n");
			return -1;
		}
		retries -= 1;                      /* Decrement retry count */
	}
	while((retries != 0) && ((stat & PHY_STAT_ANDONE) == 0));

	phy_read(phy,PHY_ANLPAR,&autoneg);

	if((autoneg & PHY_ANLP_REMFAULT) != 0) {
		printf("Aneg: rmt flt\n");
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
		printf("Aneg: tmout\n");
		return -1;
	}
	return 0;
}

#endif
