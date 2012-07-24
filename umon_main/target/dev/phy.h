/* phy.h:
 * Definitions of the common PHY registers and their bits.
 */
extern int phy_read(int phy, int reg, unsigned short *value);
extern int phy_write(int phy, int reg, unsigned short value);
extern int phy_stat(int phy, int verbose);
extern int phy_dump(int phy);
extern int phy_reset(int phy);
extern int phy_id(int phy, unsigned long *id);
extern int phy_autoneg(int phy);
extern int phy_speed(int phy);
extern int phy_duplex(int phy);

#define HALF_DUPLEX 0x10
#define FULL_DUPLEX 0x11

/***************************
 *
 * Various PHY IDs (returned by the phy_id() function):
 */
#define PHYID_SMSC83C180	0x02821c55
#define PHYID_LXT971		0x001378e0	// Least significant nibble is revno

/***************************
 *
 * PHY Reset register:
 */
#define PHY_CTRL	0

#define PHY_CTRL_RST		(1<<15)	// 1= Phy Reset
#define PHY_CTRL_LBK		(1<<14)	// 1= Loopback mode
#define PHY_CTRL_FSS		(1<<13)	// 1= 100, 0=10 (forced speed selection)
#define PHY_CTRL_100MB		(1<<13)	// 1= 100
#define PHY_CTRL_10MB		(0)		// 0= 10
#define PHY_CTRL_ANE		(1<<12)	// 1= Autonegotiation enable
#define PHY_CTRL_PWRDN		(1<<11)	// 0= Normal operation (read only)
#define PHY_CTRL_ISOLATE	(1<<10)	// 1= Electrically isolate phy from mii
#define PHY_CTRL_ANRESTART	(1<<9)	// 1= Restart auto-negotiation process
#define PHY_CTRL_DUPLEX		(1<<8)	// 1= Full-duplex, 0=half-duplex
#define PHY_CTRL_FDX		(1<<8)	// 1= Full-duplex
#define PHY_CTRL_HDX		(0)		// 0= Half-duplex
#define PHY_CTRL_COLTST		(1<<7)	// 1= Enable collision test mode

/***************************
 *
 * PHY Status register:
 */
#define PHY_STAT	1

#define PHY_STAT_100BT4		(1<<15)	// 0= Not 100Base-T4 capable
#define PHY_STAT_100FDX		(1<<14)	// 1= 100Base-T full duplex capable
#define PHY_STAT_100HDX		(1<<13)	// 1= 100Base-T half duplex capable
#define PHY_STAT_10FDX		(1<<12)	// 1= 10Base-T full duplex capable
#define PHY_STAT_10HDX		(1<<11)	// 1= 10Base-T half duplex capable
#define PHY_STAT_MFPRESUP	(1<<6)	// 1= Preamble may be suppressed (RW)
#define PHY_STAT_ANDONE		(1<<5)	// 1= Auto-negotiation done
#define PHY_STAT_REMFAULT	(1<<4)	// 1= Far-end fault condition detected
#define PHY_STAT_ANABLE		(1<<3)	// 1= Auto-negotiate capable
#define PHY_STAT_LINKUP		(1<<2)	// 1= Link is up
#define PHY_STAT_JABBER		(1<<1)	// 1= Jabber condition detected
#define PHY_STAT_EXABLE		(1<<0)	// 1= Extended register capable

#define PHY_IDHI	2
#define PHY_IDLO	3

/***************************
 *
 * PHY Auto-negotiation advertise register:
 */
#define PHY_ANAD	4

#define PHY_ANAD_NXTPG		(1<<15)	// 1= Enable next page ability
#define PHY_ANAD_REMFAULT	(1<<13)	// 1= Transmit remote fault
#define PHY_ANAD_FDPAUSE	(1<<10)	// 1= Pause operation for full duplex
#define PHY_ANAD_100BT4		(1<<9)	// 0= Do not advertise T4 capability
#define PHY_ANAD_100FDX		(1<<8)	// 1= Advertise 100Base-T full duplex
#define PHY_ANAD_100HDX		(1<<7)	// 1= Advertise 100Base-T half duplex
#define PHY_ANAD_10FDX		(1<<6)	// 1= Advertise 10Base-T full duplex
#define PHY_ANAD_10HDX		(1<<5)	// 1= Advertise 10Base-T half duplex
#define PHY_ANAD_802_3      (1<<0)
#define PHY_ANAD_802_3_16T  (1<<1)
#define PHY_ANAD_ADSEL		0x1f	// Advertise selector field

/***************************
 *
 * PHY Auto-negotiation link partner ability register
 */
#define PHY_ANLPAR	5

#define PHY_ANLP_NXTPG		(1<<15)	// Link partner next page bit
#define PHY_ANLP_ACK		(1<<14)	// Link partner acknowledge bit
#define PHY_ANLP_REMFAULT	(1<<13)	// Link partner remote fault indicator
#define PHY_ANLP_PAUSE		(1<<10)	// Link partner is pause capable
#define PHY_ANLP_100BT4		(1<<9)	// Link partner is 100Base-T4 capable
#define PHY_ANLP_100FDX		(1<<8)	// Link partner is 100Base-TX FD capable
#define PHY_ANLP_100HDX		(1<<7)	// Link partner is 100Base-TX capable
#define PHY_ANLP_10FDX		(1<<6)	// Link partner is 10Base-T FD capable
#define PHY_ANLP_10HDX		(1<<5)	// Link partner is 10Base-T capable
#define PHY_ANLP_LPSEL		0x1f	// Link partner selector field


/***************************
 *
 * PHY Auxilliary control/status register
 */
#define PHY_AXCS	24

#define PHY_AXCS_JABBER		(1<<15)	// 1=Jabber function disabled
#define PHY_AXCS_FORCE		(1<<14)	// 1=Force link pass
#define PHY_AXCS_10TPM		(1<<8)	// 1=Full power mode 0=low power
									// Squelch bits:
#define PHY_AXCS_SQNORM		0x0000	// 00=normal squelch
#define PHY_AXCS_SQLO		0x0040	// 01=low squelch
#define PHY_AXCS_SQHI		0x0080	// 10=high squelch
#define PHY_AXCS_SQNA		0x00c0	// 11= squelch not allowed
									// Edge rate bits:
#define PHY_AXCS_EDGE1NS	0x0000	// 00=1 ns
#define PHY_AXCS_EDGE2NS	0x0010	// 01=2 ns
#define PHY_AXCS_EDGE3NS	0x0020	// 10=3 ns
#define PHY_AXCS_EDGE4NS	0x0030	// 11=4 ns

#define PHY_AXCS_ANI		(1<<3)	// 1=Auto-negotiateion activated
									// 0=Speed forced manually
#define PHY_AXCS_FSPEED		(1<<2)	// 1=Speed forced to 100Base-TX
									// 0=Speed forced to 10Baset-T
#define PHY_AXCS_SPEED100	(1<<1)	// 1=100Base-Tx
									// 0=10Base-T
#define PHY_AXCS_FDXI		(1<<0)	// 1=Full-duplex active
									// 0=Full-duplex not active



