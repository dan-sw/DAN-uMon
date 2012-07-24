//==========================================================================
//
// at91sam9263_emac.h
//
// Author(s):    Michael Kelly, Cogent Computer Systems, Inc.
// Contributors: 
// Date:         6/20/2003
// Description:  This file contains the register offsets and bit
//				 defines for the AT91SAM9263 Ethernet MAC
//

#ifndef AT91SAM9263_EMAC_H
#define AT91SAM9263_EMAC_H

#include "bits.h"

//Register offsets
#define EMAC_CTL  				0x00 		// Network Control Register
#define EMAC_CFG  				0x04 		// Network Configuration Register
#define EMAC_SR   				0x08 		// Network Status Register
//#define EMAC_TAR  				0x0C 		// Transmit Address Register
//#define EMAC_TCR  				0x10 		// Transmit Control Register
#define EMAC_TSR  				0x14 		// Transmit Status Register
#define EMAC_RBQP 				0x18 		// Receive Buffer Queue Pointer
#define EMAC_TBQP 				0x1C 		// Transmit Buffer Queue Pointer
#define EMAC_RSR  				0x20 		// Receive Status Register
#define EMAC_ISR  				0x24 		// Interrupt Status Register
#define EMAC_IER  				0x28 		// Interrupt Enable Register
#define EMAC_IDR  				0x2C 		// Interrupt Disable Register
#define EMAC_IMR  				0x30 		// Interrupt Mask Register
#define EMAC_MAN  				0x34 		// PHY Maintenance Register
#define EMAC_PTR  				0x38 		// Pause Time Register
#define EMAC_PFR  				0x3C 		// Pause Frames Received Register
#define EMAC_FRA  				0x40 		// Frames Transmitted OK Register
#define EMAC_SCOL 				0x44 		// Single Collision Frame Register
#define EMAC_MCOL 				0x48 		// Multiple Collision Frame Register
#define EMAC_OK   				0x4C 		// Frames Received OK Register
#define EMAC_SEQE 				0x50 		// Frame Check Sequence Error Register
#define EMAC_ALE  				0x54 		// Alignment Error Register
#define EMAC_DTE  				0x58 		// Deferred Transmission Frame Register
#define EMAC_LCOL 				0x5C 		// Late Collision Register
#define EMAC_ECOL 				0x60 		// Excessive Collision Register
#define EMAC_TUE  				0x64 		// Transmit Underrun Error Register
#define EMAC_CSE  				0x68 		// Carrier Sense Error Register
#define EMAC_RRE  				0x6C 		// Receive Resource Error Register
#define EMAC_ROV  				0x70 		// Receive Overrun Error Register
#define EMAC_RSE  				0x74 		// Receive Symbol Error Register
#define EMAC_ELR  				0x78 		// Excessive Length Error Register
#define EMAC_RJB  				0x7C 		// Receive Jabber Register
#define EMAC_USF  				0x80 		// Undersize Frame Register
#define EMAC_SQEE 				0x84 		// SQE Test Error Register
#define EMAC_RLE 				0x88 		// Receive Length Field Mismatch Register
#define EMAC_HSL  				0x90 		// Hash Address Low[31:0]
#define EMAC_HSH  				0x94 		// Hash Address High[63:32]
#define EMAC_SA1L 				0x98 		// Specific Address 1 Bottom
#define EMAC_SA1H 				0x9C 		// Specific Address 1 Top
#define EMAC_SA2L 				0xA0 		// Specific Address 2 Bottom
#define EMAC_SA2H 				0xA4 		// Specific Address 2 Top
#define EMAC_SA3L 				0xA8 		// Specific Address 3 Bottom
#define EMAC_SA3H 				0xAC 		// Specific Address 3 Top
#define EMAC_SA4L 				0xB0 		// Specific Address 4 Bottom
#define EMAC_SA4H 				0xB4 		// Specific Address 4 Top
#define EMAC_TID 				0xB8 		// Type ID Checking Register
#define EMAC_USRIO 				0xC0 		// User I/O Register

// Control Register, EMAC_CTL, Offset 0x0
#define EMAC_CTL_LB         	BIT0 		// 1 = Set Loopback output signal
#define EMAC_CTL_LBL        	BIT1 		// 1 = Loopback local. 
#define EMAC_CTL_RE         	BIT2 		// 1 = Receive enable. 
#define EMAC_CTL_TE         	BIT3 		// 1 = Transmit enable. 
#define EMAC_CTL_MPE        	BIT4 		// 1 = Management port enable. 
#define EMAC_CTL_CSR        	BIT5 		// Write 1 to clear statistics registers. 
#define EMAC_CTL_ISR        	BIT6 		// Write to increment all statistics registers for test use. 
#define EMAC_CTL_WES        	BIT7 		// 1 = Enable writing to statistics registers for test use.
#define EMAC_CTL_BP         	BIT8 		// 1 = Force collision on all RX frames in HDX mode.
#define EMAC_CTL_TST         	BIT9 		// 1 = Starts Transmission.
#define EMAC_CTL_THLT         	BIT10 		// 1 = Halts Transmission.

// Configuration Register, EMAC_CFG, Offset 0x4
#define EMAC_CFG_SPD        	BIT0 		// 1 = 10/100 Speed (no functional effect)
#define EMAC_CFG_FD         	BIT1 		// 1 = Full duplex. 
#define EMAC_CFG_JFRAME        	BIT3 		// 1 = Jumbo Frames up to 10240 bytes accepted 
#define EMAC_CFG_CAF        	BIT4 		// 1 = accept all frames
#define EMAC_CFG_NBC        	BIT5 		// 1 = disable reception of broadcast frames
#define EMAC_CFG_MTI        	BIT6 		// 1 = Multicast hash enable
#define EMAC_CFG_UNI        	BIT7 		// 1 = Unicast hash enable. 
#define EMAC_CFG_BIG        	BIT8 		// 1 = enable reception of 802.3 1522 byte frames
#define EMAC_CFG_CLK_8   		(0 << 10)	// MII Clock = HCLK divided by 8
#define EMAC_CFG_CLK_16  		(1 << 10)	// MII Clock = HCLK divided by 16
#define EMAC_CFG_CLK_32  		(2 << 10)	// MII Clock = HCLK divided by 32
#define EMAC_CFG_CLK_64  		(3 << 10)	// MII Clock = HCLK divided by 64
#define EMAC_CFG_RTY        	BIT12 		// Retry Test Mode - Must be 0 
#define EMAC_CFG_PAE   	    	BIT13 		// Reduced MII Mode Enable
#define EMAC_CFG_RBOF0       	(0 << 14)	// No Offset from start of receive buffer
#define EMAC_CFG_RBOF1       	(1 << 14)	// 1 byte Offset from start of receive buffer
#define EMAC_CFG_RBOF2       	(2 << 14)	// 2 byte Offset from start of receive buffer
#define EMAC_CFG_RBOF3       	(3 << 14)	// 3 byte Offset from start of receive buffer
#define EMAC_CFG_RLCE       	BIT16 		// Receive Length field Checking Enable
#define EMAC_CFG_DRFCS       	BIT17 		// Discard Receive FCS
#define EMAC_CFG_EFRHD       	BIT18 		// Enable Frames be received in HPM while transmitting
#define EMAC_CFG_IRXFCS       	BIT19 		// Ignore RX FCS

// Status Register, EMAC_SR, Offset 0x8
#define EMAC_MDIO       		BIT1 		// Real Time state of MDIO pin
#define EMAC_IDLE       		BIT2 		// 0 = PHY Logic is idle, 1 = PHY Logic is running

// Transmit Control Register, EMAC_TCR, Offset 0x10
//#define EMAC_TCR_LEN(_x_)       ((_x_ & 0x7FF) <<  0) 	// Transmit frame length minus CRC length (if any)
//#define EMAC_TCR_NCRC       	BIT15					// Do not append CRC on Transmit

// Transmit Status Register, EMAC_TSR, Offset 0x14
#define EMAC_TSR_UBR        	BIT0		// 1 = Used Bit Read
#define EMAC_TSR_COL        	BIT1 		// 1 = Collision occured
#define EMAC_TSR_RLE        	BIT2 		// 1 = Retry lmimt exceeded
#define EMAC_TSR_TGO	     	BIT3 		// 1 = Transmitter Go
#define EMAC_TSR_BEX        	BIT4 		// 1 = Buffer exhausted mid frame
#define EMAC_TSR_COMP       	BIT5 		// 1 = Transmit complete
#define EMAC_TSR_UND        	BIT6 		// 1 = Transmit underrun
 
// Receive Status Register, EMAC_RSR, Offset 0x20
#define EMAC_RSR_BNA        	BIT0 		// 1 = Buffer not available
#define EMAC_RSR_REC        	BIT1 		// 1 = Frame received
#define EMAC_RSR_OVR			BIT2 		// 1 = Receive overrun

// Interrupt Status Register, EMAC_ISR, Offsen 0x24
// Interrupt Enable Register, EMAC_IER, Offset 0x28
// Interrupt Disable Register, EMAC_IDR, Offset 0x2c
// Interrupt Mask Register, EMAC_IMR, Offset 0x30
#define EMAC_INT_DONE       	BIT0 		// Phy management done 
#define EMAC_INT_RCOM       	BIT1 		// Receive complete
#define EMAC_INT_RUBR       	BIT2 		// Receive used bit read
#define EMAC_INT_TUBR       	BIT3 		// Transmit used bit read
#define EMAC_INT_TUND       	BIT4 		// Transmit buffer underrun
#define EMAC_INT_RTRY       	BIT5 		// Transmit Retry limt
#define EMAC_INT_TERR       	BIT6 		// Transmit buffer register error
#define EMAC_INT_TCOM       	BIT7 		// Transmit complete
#define EMAC_INT_ROVR       	BIT10 		// Receive overrun
#define EMAC_INT_HRESP     		BIT11 		// Hresp not OK - DMA block sees a bus error
#define EMAC_INT_PFR     		BIT12 		// Pause Frame Received
#define EMAC_INT_PTZ     		BIT13 		// Pause Time Zero

// PHY Maintenance Register, EMAC_MAN, Offset 0x34
#define EMAC_MAN_DATA(_x_)     	((_x_ & 0xFFFF) <<  0) 	// PHY data register
#define EMAC_MAN_CODE       	(0x3 << 16)				// IEEE Code
#define EMAC_MAN_REGA(_x_)     	((_x_ & 0x1F) << 18) 	// PHY register address
#define EMAC_MAN_PHYA(_x_)     	((_x_ & 0x1F) << 23)	// PHY address
#define EMAC_MAN_WRITE         	(0x1 << 28)				// Transfer is a write
#define EMAC_MAN_READ         	(0x2 << 28)				// Transfer is a read
#define EMAC_MAN_SOF	       	(0x2 << 30)				// Must be written 01 for valid frame

// User Input/Output Register
#define EMAC_USRIO_RMII       	BIT0 		// set to 1 for RMII mode, 0 = MII 
#define EMAC_USRIO_CLKEN       	BIT1 		// set to 1 to enable transeiver input clock

#endif // #ifndef AT91SAM9263_EMAC_H

