//
// lan91c11x.h:
//
// Derived from Ed Sutter's smc91c94.h by Michael Kelly, Cogent Computer Systems, Inc.
// 03/16/03
//
// All registers are defined for 16-bit access, little endian only.
//
#define LAN_BANK				(0xe * LAN_STEP)		// Bank Select Register (all banks)

// bank registers are defined as offsets in the bank with the
// bank number encoded into the upper byte.  The generic LAN 
// read and write routines will use this information to set the
// desired bank

// Bank 0 registers:
#define LAN_TCR							(0x00 * LAN_STEP)		// Transmit Control
#define LAN_EPH							(0x02 * LAN_STEP)		// EPH Status
#define LAN_RCR							(0x04 * LAN_STEP)		// Receive Control
#define LAN_COUNTER						(0x06 * LAN_STEP)		// Counter
#define LAN_MIR							(0x08 * LAN_STEP)		// Memory Information
#define LAN_RPCR						(0x0a * LAN_STEP)		// Receive/PHY Control

// Bank 1 registers:
#define LAN_CONFIG						(0x10 * LAN_STEP)		// Configuration
#define LAN_BASE						(0x12 * LAN_STEP)		// Base Address
#define LAN_IA01						(0x14 * LAN_STEP)		// Individual Address 0-1
#define LAN_IA23						(0x16 * LAN_STEP)		// Individual Address 2-3
#define LAN_IA45						(0x18 * LAN_STEP)		// Individual Address 4-5
#define LAN_GENERAL						(0x1a * LAN_STEP)		// General Purpose
#define LAN_CTRL						(0x1c * LAN_STEP)		// Control

// Bank 2 registers:
#define LAN_MMU_CMD						(0x20 * LAN_STEP)		// MMU Command
#define LAN_PNRARR						(0x22 * LAN_STEP)		// Pkt Number/Allocation Rslt
#define LAN_FIFOPORT					(0x24 * LAN_STEP)		// FIFO Ports
#define LAN_PTR							(0x26 * LAN_STEP)		// Pointer
#define LAN_DATA1						(0x28 * LAN_STEP)		// Data Port 1
#define LAN_DATA2						(0x2a * LAN_STEP)		// Data Port 2
#define LAN_INT							(0x2c * LAN_STEP)		// Interrupt Control

// Bank 3 registers:
#define LAN_MT01						(0x30 * LAN_STEP)		// Multicast Tbl 0-1
#define LAN_MT23						(0x32 * LAN_STEP)		// Multicast Tbl 2-3
#define LAN_MT45						(0x34 * LAN_STEP)		// Multicast Tbl 4-5
#define LAN_MT67						(0x36 * LAN_STEP)		// Multicast Tbl 6-7
#define LAN_MGMT						(0x38 * LAN_STEP)		// MII Management Register
#define LAN_REV							(0x3a * LAN_STEP)		// Revision
#define LAN_ERCV						(0x3c * LAN_STEP)		// Early Receive

// Bit masks for registers:
// Note that bytes are swapped

// LAN_TCR bit defines
#define	LAN_TCR_SWFDUP					0x8000	// 1 = enabled switched full duplex mode
#define	LAN_TCR_EPH_LOOP				0x2000	// 1 = enable internal MAC loopback
#define	LAN_TCR_STP_SQET				0x1000	// 1 = stop sransmission on SQE test errors
#define	LAN_TCR_FDUPLX					0x0800	// 1 = receive frames sent from ourself
#define	LAN_TCR_MON_CSN					0x0400	// 1 = monitor our carrier signal
#define	LAN_TCR_NOCRC					0x0100	// 1 = do not insert CRC
#define	LAN_TCR_PAD_EN					0x0080	// 1 = insert 0x00 pads for frames < 64 bytes
#define	LAN_TCR_FORCOL					0x0004	// 1 = force a collision
#define	LAN_TCR_LOOP					0x0002	// 1 = enable PHY loopback
#define	LAN_TCR_TXENA					0x0001	// 1 = enable transmit

// LAN_EPHSTAT bit defines
#define LAN_EPH_TXUNRN					0x8000	// 1 = transmit under run
#define LAN_EPH_LINK_OK					0x4000	// 1 = link ok bit from PHY
#define LAN_EPH_CTR_ROL					0x1000	// 1 = counter roll over
#define LAN_EPH_EXC_DEF					0x0800	// 1 = excessive deferral
#define LAN_EPH_LOST_CARR				0x0400	// 1 = lost carrier sense
#define LAN_EPH_LATECOL					0x0200	// 1 = late collision
#define LAN_EPH_TX_DEFR					0x0080	// 1 = transmir deferred
#define LAN_EPH_LTX_BRD					0x0040	// 1 = last transmit was a broadcast
#define LAN_EPH_SQET					0x0020	// 1 = signal quality error test
#define LAN_EPH_16COL					0x0010	// 1 = 16 collisions reached
#define LAN_EPH_LTX_MULT				0x0008	// 1 = last transmit was a multicast
#define LAN_EPH_MULCOL					0x0004	// 1 = multiple collisions detected
#define LAN_EPH_SNGLCOL					0x0002	// 1 = single collision detected
#define LAN_EPH_TX_SUC					0x0001	// 1 = last transmit was successful

// LAN_RCR bit defines
#define	LAN_RCR_SOFT_RST				0x8000	// 1 = software reset of lan91c11x
#define	LAN_RCR_FILT_CAR				0x4000	// 1 = filter carrier
#define	LAN_RCR_ABORT_ENAB				0x0200	// 1 = abort receive packet on collision
#define	LAN_RCR_STRIP_CRC				0x0200	// 1 = strip crc from received frames
#define	LAN_RCR_RXEN					0x0100	// 1 = enable receiver
#define	LAN_RCR_ALMUL					0x0004	// 1 = accept all multicast frames
#define	LAN_RCR_PRMS					0x0002	// 1 = promiscuous - accept all frames
#define	LAN_RCR_RX_ABORT				0x0001	// 1 = reception aborted, length > 2K bytes

// LAN_MIR bit macros for returning memory size and free memory
// LAN91C11x reports this information in 2K byte units
#define	LAN_MIR_FREEMEM()				(((LAN_Read(LAN_MIR) & 0xff00) >> 8) * 2048)
#define	LAN_MIR_MEMSIZE()				((LAN_Read(LAN_MIR) & 0x00ff) * 2048)

// LAN_RPCR bit defines
#define LAN_RPCR_SPEED					0x2000	// 1 = select 100Mbit speed when ANEG = 0
#define LAN_RPCR_DPLX					0x1000	// 1 = select full duplex mode when ANEG = 0
#define LAN_RPCR_ANEG					0x0800	// 1 = select auto-negotiate mode
// LEDA select
#define LAN_RPCR_LEDA_LINK				(0 << 5)	// LEDA is active for 10M or 100M link
#define LAN_RPCR_LEDA_10M				(2 << 5)	// LEDA is active for 10M link only 
#define LAN_RPCR_LEDA_FDX				(3 << 5)	// LEDA is active for full duplex mode
#define LAN_RPCR_LEDA_TXRX				(4 << 5)	// LEDA is active for transmit or receive
#define LAN_RPCR_LEDA_100M				(5 << 5)	// LEDA is active for 100M link only
#define LAN_RPCR_LEDA_RX				(6 << 5)	// LEDA is active for receive only
#define LAN_RPCR_LEDA_TX				(7 << 5)	// LEDA is active for transmit only
// LEDB select
#define LAN_RPCR_LEDB_LINK				(0 << 2)	// LEDB is active for 10M or 100M link
#define LAN_RPCR_LEDB_10M				(2 << 2)	// LEDB is active for 10M link only 
#define LAN_RPCR_LEDB_FDX				(3 << 2)	// LEDB is active for full duplex mode
#define LAN_RPCR_LEDB_TXRX				(4 << 2)	// LEDB is active for transmit or receive
#define LAN_RPCR_LEDB_100M				(5 << 2)	// LEDB is active for 100M link only
#define LAN_RPCR_LEDB_RX				(6 << 2)	// LEDB is active for receive only
#define LAN_RPCR_LEDB_TX				(7 << 2)	// LEDB is active for transmit only

// LAN_CONFIG bit defines
#define LAN_CONFIG_EPH_PWREN			0x8000	// 1 = Power enable, 0 = low power mode 
#define LAN_CONFIG_NO_WAIT				0x1000	// 1 = no wait mode except for data not ready
#define LAN_CONFIG_GPCNTRL				0x0400	// directly controls nCNTRL inverted
#define LAN_CONFIG_EXT_PHY				0x0200	// 1 = external PHY enabled


// LAN_CTRL bit defines
#define	LAN_CTRL_RCV_BAD				0x4000	// 1 = accept receive packets with bad CRC
#define	LAN_CTRL_AUTO_REL				0x0800	// 1 = release transmit memory upon good completion
#define	LAN_CTRL_LE_ENABLE				0x0080	// 1 = link error interrupt enable
#define	LAN_CTRL_CR_ENABLE				0x0040	// 1 = counter roll over interrupt enable
#define	LAN_CTRL_TE_ENABLE				0x0020	// 1 = transmit error interrupt enable
#define	LAN_CTRL_EEPROM_SEL				0x0004	// 1 = only load GPR from EEPROM, 0 = load all
#define	LAN_CTRL_RELOAD					0x0002	// 1 = reload from EEPROM
#define	LAN_CTRL_STORE					0x0001	// 1 = store to EEPROM

// MMU COMMAND bits:
#define	LAN_MMU_BUSY					0x0001
#define	LAN_MMU_CMD_NOOP				(0 << 5)	// NO OP
#define	LAN_MMU_CMD_ALLOCTXMEM			(1 << 5)	// Allocate Memory for TX
#define LAN_MMU_CMD_RESETMMU			(2 << 5)	// Reset MMU to initial state
#define	LAN_MMU_CMD_RMRXFRAME			(3 << 5)	// Remove frame from top of RX fifo
#define	LAN_MMU_CMD_RMRELRXFRAME		(4 << 5)	// Remove and release top of RX fifo
#define	LAN_MMU_CMD_RELEASEPKT			(5 << 5)	// Release specific packet
#define	LAN_MMU_CMD_ENQUEUETXPKT		(6 << 5)	// Enqueue packet into TX fifo
#define	LAN_MMU_CMD_RESETTXFIFOS		(7 << 5)	// Reset TX fifos

// Receive frame status word bits:
#define LAN_RFSW_ALGN_ERR				0x8000
#define LAN_RFSW_BROADCAST				0x4000
#define LAN_RFSW_BADCRC					0x2000
#define LAN_RFSW_ODDFRM					0x1000
#define LAN_RFSW_TOOLNG					0x0800
#define LAN_RFSW_TOOSHORT				0x0400
#define LAN_RFSW_MULT_CAST				0x0001

// Interrupt Register bits:
#define LAN_INT_ERCV					0x0040
#define LAN_INT_EPH						0x0020
#define LAN_INT_RXOVRN					0x0010
#define LAN_INT_ALLOC					0x0008
#define LAN_INT_TXEMPTY					0x0004
#define LAN_INT_TX						0x0002
#define LAN_INT_RCV						0x0001

// FifoPort bits:
#define LAN_FIFOPORT_RCVFIFO_EMPTY		0x8000
#define LAN_FIFOPORT_XMTFIFO_EMPTY		0x0080

// Pointer Register bits:
#define LAN_PTR_RCV						0x8000
#define LAN_PTR_AUTOINC					0x4000
#define LAN_PTR_READ					0x2000
#define LAN_PTR_ETEN					0x1000

// Allocation Result Register bits:
#define LAN_PNRARR_ALLOC_FAILED			0x0080
#define LAN_PNRARR_ARRTOPNR(arr)		((arr << 8) & 0xff00)

// Frame Control Byte bits:
#define LAN_CTL_BYTE_ODD_LENGTH			0x0020

// Receive Frame Status Register bits:
#define LAN_RFSW_ODD_LEN				0x0010
#define LAN_RFSW_BCAST					0x0040

// chip id and rev:
#define LAN_REV_LAN91C11x				0x3390	// same for LAN91C111 and LAN91C113

#define LAN_REV_REV_MASK				0x000f
#define LAN_REV_CHIP_MASK				0xfff0

// MII Interface Register
#define LAN_MGMT_CRS100					0x4000	// Disable CRS100 detection in half duplex mode
#define LAN_MGMT_MDOE					0x0008	// 0 = MDO pin is tristated, 1 = driven
#define LAN_MGMT_MCLK					0x0004	// directly control MCLK pin
#define LAN_MGMT_MDI					0x0002	// state if MDI pin
#define LAN_MGMT_MDO					0x0001	// Direclty controls MDO pin (when MDOE = 1)

// Internal LAN91C11x PHY defines
// PHY Register Addresses (LAN91C111 Internal PHY)

// PHY Register Offsets
#define PHY_CNTL						0x00	// PHY Control 
#define PHY_STAT						0x01	// PHY Status
#define PHY_ID1							0x02	// PHY Identifier 1
#define PHY_ID2							0x03	// PHY Identifier 2
#define PHY_AD							0x04	// PHY Auto-negotiate Control
#define PHY_RMT							0x05	// PHY Auto-negotiation Remote End Capability Register
#define PHY_CFG1						0x10	// PHY Configuration 1
#define PHY_CFG2						0x11	// PHY Configuration 2
#define PHY_INT							0x12	// Status Output (Interrupt Status)
#define PHY_MASK						0x13	// Interrupt Mask

// PHY Control Register Bit Defines
#define PHY_CNTL_RST					0x8000	// 1 = PHY Reset
#define PHY_CNTL_LPBK					0x4000	// 1 = PHY Loopback
#define PHY_CNTL_SPEED					0x2000	// 1 = 100Mbps, 0=10Mpbs
#define PHY_CNTL_ANEG_EN				0x1000	// 1 = Enable Auto negotiation
#define PHY_CNTL_PDN					0x0800	// 1 = PHY Power Down mode
#define PHY_CNTL_MII_DIS				0x0400	// 1 = MII 4 bit interface disabled
#define PHY_CNTL_ANEG_RST				0x0200	// 1 = Reset Auto negotiate
#define PHY_CNTL_DPLX					0x0100	// 1 = Full Duplex, 0=Half Duplex
#define PHY_CNTL_COLTST					0x0080	// 1 = MII Colision Test

// PHY Status Register Bit Defines
#define PHY_STAT_CAP_T4					0x8000	// 1 = 100Base-T4 capable
#define PHY_STAT_CAP_TXF				0x4000	// 1 = 100Base-X full duplex capable
#define PHY_STAT_CAP_TXH				0x2000	// 1 = 100Base-X half duplex capable
#define PHY_STAT_CAP_TF					0x1000	// 1 = 10Mbps full duplex capable
#define PHY_STAT_CAP_TH					0x0800	// 1 = 10Mbps half duplex capable
#define PHY_STAT_CAP_SUPR				0x0040	// 1 = recv mgmt frames with not preamble
#define PHY_STAT_ANEG_ACK				0x0020	// 1 = ANEG has completed
#define PHY_STAT_REM_FLT				0x0010	// 1 = Remote Fault detected
#define PHY_STAT_CAP_ANEG				0x0008	// 1 = Auto negotiate capable
#define PHY_STAT_LINK					0x0004	// 1 = valid link
#define PHY_STAT_JAB					0x0002	// 1 = 10Mbps jabber condition
#define PHY_STAT_EXREG					0x0001	// 1 = extended registers implemented

// PHY Auto-Negotiation Advertisement Register Bit Defines
#define PHY_AD_NP						0x8000	// 1 = PHY requests exchange of Next Page
#define PHY_AD_ACK						0x4000	// 1 = got link code word from remote
#define PHY_AD_RF						0x2000	// 1 = advertise remote fault
#define PHY_AD_T4						0x0200	// 1 = PHY is capable of 100Base-T4
#define PHY_AD_TX_FDX					0x0100	// 1 = PHY is capable of 100Base-TX FDPLX
#define PHY_AD_TX_HDX					0x0080	// 1 = PHY is capable of 100Base-TX HDPLX
#define PHY_AD_10_FDX					0x0040	// 1 = PHY is capable of 10Base-T FDPLX
#define PHY_AD_10_HDX					0x0020	// 1 = PHY is capable of 10Base-T HDPLX
#define PHY_AD_CSMA						0x0001	// 1 = PHY is capable of 802.3 CMSA

// PHY Auto-negotiation Remote End Capability Register Bit Defines
// Uses same bit definitions as PHY_AD_REG

// PHY Configuration Register 1 Bit Defines
#define PHY_CFG1_LNKDIS					0x8000	// 1 = Rx Link Detect Function disabled
#define PHY_CFG1_XMTDIS					0x4000	// 1 = TP Transmitter Disabled
#define PHY_CFG1_XMTPDN					0x2000	// 1 = TP Transmitter Powered Down
#define PHY_CFG1_BYPSCR					0x0400	// 1 = Bypass scrambler/descrambler
#define PHY_CFG1_UNSCDS					0x0200	// 1 = Unscramble Idle Reception Disable
#define PHY_CFG1_EQLZR					0x0100	// 1 = Rx Equalizer Disabled
#define PHY_CFG1_CABLE					0x0080	// 1 = STP(150ohm), 0=UTP(100ohm)
#define PHY_CFG1_RLVL0					0x0040	// 1 = Rx Squelch level reduced by 4.5db
#define PHY_CFG1_TLVL_SHIFT				2		// Transmit Output Level Adjust
#define PHY_CFG1_TLVL_MASK				0x003C
#define PHY_CFG1_TRF_MASK				0x0003	// Transmitter Rise/Fall time

// PHY Configuration Register 2 Bit Defines
#define PHY_CFG2_APOLDIS				0x0020	// 1 = Auto Polarity Correction disabled
#define PHY_CFG2_JABDIS					0x0010	// 1 = Jabber disabled
#define PHY_CFG2_MREG					0x0008	// 1 = Multiple register access (MII mgt)
#define PHY_CFG2_INTMDIO				0x0004	// 1 = Interrupt signaled with MDIO pulseo

// PHY Status Output (and Interrupt status) Register Bit Defines
#define PHY_INT_INT						0x8000	// 1 = bits have changed since last read
#define	PHY_INT_LNKFAIL					0x4000	// 1 = Link Not detected
#define PHY_INT_LOSSSYNC				0x2000	// 1 = Descrambler has lost sync
#define PHY_INT_CWRD					0x1000	// 1 = Invalid 4B5B code detected on rx
#define PHY_INT_SSD						0x0800	// 1 = No Start Of Stream detected on rx
#define PHY_INT_ESD						0x0400	// 1 = No End Of Stream detected on rx
#define PHY_INT_RPOL					0x0200	// 1 = Reverse Polarity detected
#define PHY_INT_JAB						0x0100	// 1 = Jabber detected
#define PHY_INT_SPDDET					0x0080	// 1 = 100Base-TX mode, 0=10Base-T mode
#define PHY_INT_DPLXDET					0x0040	// 1 = Device in Full Duplex

// PHY Interrupt/Status Mask Register
// Uses the same bit definitions as PHY_INT_REG

