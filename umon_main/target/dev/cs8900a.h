//==========================================================================
//
//      cs8900a.h
//
//
// Author(s):    Michael Kelly, Cogent Computer Systems, Inc.
// Contributors: 
// Date:         05-26-2002
// Description:  This file contains defines and access macros for the
//				 cs8900a ethernet controller found on some CSB's.
//
//--------------------------------------------------------------------------

// ------------------------------------------------------------------------
// cpuio.h must provide the CS8900A_BASE_ADDRESS and the CS8900A_STEP
// defines
#define CS_REG(_x_)				*(vushort *)(CS8900A_BASE_ADDRESS + _x_)

// ------------------------------------------------------------------------
// Directly visible registers. 
#define CS_RTDATA 				CS_REG((0x00 * CS8900A_STEP))
#define CS_TxCMD  				CS_REG((0x02 * CS8900A_STEP))
#define CS_TxLEN  				CS_REG((0x03 * CS8900A_STEP))
#define CS_ISQ    				CS_REG((0x04 * CS8900A_STEP))
#define CS_PADD   				CS_REG((0x05 * CS8900A_STEP))
#define CS_PDAT  				CS_REG((0x06 * CS8900A_STEP))

#define ISQ_RxEvent     		0x04
#define ISQ_TxEvent     		0x08
#define ISQ_BufEvent    		0x0C
#define ISQ_RxMissEvent 		0x10
#define ISQ_TxColEvent  		0x12
#define ISQ_EventMask   		0x3F

// ------------------------------------------------------------------------
// Registers available via "page pointer" (indirect access)
#define CS_ChipID    			0x0000  // Chip identifier - must be 0x630E
#define CS_ChipRev   			0x0002  // Chip revision, model codes

#define CS_IntReg    			0x0022  // Interrupt configuration
#define CS_IntReg_IRQ0         	0x0000  // Use INTR0 pin
#define CS_IntReg_IRQ1         	0x0001  // Use INTR1 pin
#define CS_IntReg_IRQ2         	0x0002  // Use INTR2 pin
#define CS_IntReg_IRQ3         	0x0003  // Use INTR3 pin

#define CS_RxCFG     			0x0102  // Receiver configuration
#define CS_RxCFG_Skip1         	0x0040  // Skip (i.e. discard) current frame
#define CS_RxCFG_Stream        	0x0080  // Enable streaming mode
#define CS_RxCFG_RxOK          	0x0100  // RxOK interrupt enable
#define CS_RxCFG_RxDMAonly     	0x0200  // Use RxDMA for all frames
#define CS_RxCFG_AutoRxDMA     	0x0400  // Select RxDMA automatically
#define CS_RxCFG_BufferCRC     	0x0800  // Include CRC characters in frame
#define CS_RxCFG_CRC           	0x1000  // Enable interrupt on CRC error
#define CS_RxCFG_RUNT          	0x2000  // Enable interrupt on RUNT frames
#define CS_RxCFG_EXTRA         	0x4000  // Enable interrupt on frames with extra data

#define CS_RxCTL     			0x0104  // Receiver control
#define CS_RxCTL_IAHash        	0x0040  // Accept frames that match hash
#define CS_RxCTL_Promiscuous   	0x0080  // Accept any frame
#define CS_RxCTL_RxOK          	0x0100  // Accept well formed frames
#define CS_RxCTL_Multicast     	0x0200  // Accept multicast frames
#define CS_RxCTL_MAC           	0x0400  // Accept frame that matches MAC
#define CS_RxCTL_Broadcast     	0x0800  // Accept broadcast frames
#define CS_RxCTL_CRC           	0x1000  // Accept frames with bad CRC
#define CS_RxCTL_RUNT          	0x2000  // Accept runt frames
#define CS_RxCTL_EXTRA         	0x4000  // Accept frames that are too long

#define CS_TxCFG     			0x0106  // Transmit configuration
#define CS_TxCFG_CRS           	0x0040  // Enable interrupt on loss of carrier
#define CS_TxCFG_SQE           	0x0080  // Enable interrupt on Signal Quality Error
#define CS_TxCFG_TxOK          	0x0100  // Enable interrupt on successful xmits
#define CS_TxCFG_Late          	0x0200  // Enable interrupt on "out of window" 
#define CS_TxCFG_Jabber        	0x0400  // Enable interrupt on jabber detect
#define CS_TxCFG_Collision     	0x0800  // Enable interrupt if collision
#define CS_TxCFG_16Collisions  	0x8000  // Enable interrupt if > 16 collisions

#define CS_TxCmd     			0x0108  // Transmit command status
#define CS_TxCmd_TxStart_5     	0x0000  // Start after 5 bytes in buffer
#define CS_TxCmd_TxStart_381   	0x0040  // Start after 381 bytes in buffer
#define CS_TxCmd_TxStart_1021  	0x0080  // Start after 1021 bytes in buffer
#define CS_TxCmd_TxStart_Full  	0x00C0  // Start after all bytes loaded
#define CS_TxCmd_Force         	0x0100  // Discard any pending packets
#define CS_TxCmd_OneCollision  	0x0200  // Abort after a single collision
#define CS_TxCmd_NoCRC         	0x1000  // Do not add CRC
#define CS_TxCmd_NoPad         	0x2000  // Do not pad short packets

#define CS_BufCFG    			0x010A  // Buffer configuration
#define CS_BufCFG_SWI          	0x0040  // Force interrupt via software
#define CS_BufCFG_RxDMA        	0x0080  // Enable interrupt on Rx DMA
#define CS_BufCFG_TxRDY        	0x0100  // Enable interrupt when ready for Tx
#define CS_BufCFG_TxUE         	0x0200  // Enable interrupt in Tx underrun
#define CS_BufCFG_RxMiss       	0x0400  // Enable interrupt on missed Rx packets
#define CS_BufCFG_Rx128        	0x0800  // Enable Rx interrupt after 128 bytes
#define CS_BufCFG_TxCol        	0x1000  // Enable int on Tx collision ctr overflow
#define CS_BufCFG_Miss         	0x2000  // Enable int on Rx miss ctr overflow
#define CS_BufCFG_RxDest       	0x8000  // Enable int on Rx dest addr match

#define CS_LineCTL   			0x0112  // Line control
#define CS_LineCTL_Rx          	0x0040  // Enable receiver
#define CS_LineCTL_Tx          	0x0080  // Enable transmitter

#define CS_SelfCtl   			0x0114  // Chip control
#define CS_SelfCtl_Reset       	0x0040  // Self-clearing reset

#define CS_BusCtl    			0x0116  // Bus control
#define CS_BusCtl_ResetRxDMA   	0x0040  // Reset receiver DMA engine
#define CS_BusCtl_DMAextend    	0x0100
#define CS_BusCtl_UseSA        	0x0200
#define CS_BusCtl_Memory      	0x0400  // Enable "memory mode"
#define CS_BusCtl_DMAbust     	0x0800
#define CS_BusCtl_IOCH_RDYE    	0x1000
#define CS_BusCtl_RxDMAsize    	0x2000
#define CS_BusCtl_EnableIRQ    	0x8000  // Enable interrupts

#define CS_TestCtl    			0x0118  // Test Control
#define CS_TestCtl_FDX			0x4000  // Full Duplex

#define CS_RER       			0x0124  // Receive event
#define CS_RER_IAHash          	0x0040  // Frame hash match
#define CS_RER_Dribble         	0x0080  // Frame had 1-7 extra bits after last byte
#define CS_RER_RxOK            	0x0100  // Frame received with no errors
#define CS_RER_Hashed          	0x0200  // Frame address hashed OK
#define CS_RER_IA              	0x0400  // Frame address matched IA
#define CS_RER_Broadcast       	0x0800  // Broadcast frame
#define CS_RER_CRC             	0x1000  // Frame had CRC error
#define CS_RER_RUNT            	0x2000  // Runt frame
#define CS_RER_EXTRA           	0x4000  // Frame was too long

#define CS_LineStat  			0x0134  // Line status
#define CS_LineStat_LinkOK     	0x0080  // Line is connected and working
#define CS_LineStat_AUI        	0x0100  // Connected via AUI
#define CS_LineStat_10BT       	0x0200  // Connected via twisted pair
#define CS_LineStat_Polarity   	0x1000  // Line polarity OK (10BT only)
#define CS_LineStat_CRS        	0x4000  // Frame being received

#define CS_SelfStat  			0x0136  // Chip status
#define CS_SelfStat_InitD      	0x0080  // Chip initialization complete
#define CS_SelfStat_SIBSY      	0x0100  // EEPROM is busy
#define CS_SelfStat_EEPROM     	0x0200  // EEPROM present
#define CS_SelfStat_EEPROM_OK  	0x0400  // EEPROM checks out
#define CS_SelfStat_ELPresent  	0x0800  // External address latch logic available
#define CS_SelfStat_EEsize     	0x1000  // Size of EEPROM

#define CS_BusStat   			0x0138  // Bus status
#define CS_BusStat_TxBid       	0x0080  // Tx error
#define CS_BusStat_TxRDY       	0x0100  // Ready for Tx data

#define CS_LAF       			0x0150  // Logical address filter (6 bytes)
#define CS_MAC        			0x0158  // Individual Address (MAC)

#define CS_TER       			0x0218	// Transmit event
#define CS_TER_CRS             	0x0040	// Carrier lost
#define CS_TER_SQE             	0x0080  // Signal Quality Error
#define CS_TER_TxOK            	0x0100  // Packet sent without error
#define CS_TER_Late            	0x0200  // Out of window
#define CS_TER_Jabber          	0x0400  // Stuck transmit?
#define CS_TER_NumCollisions   	0x7800  // Number of collisions
#define CS_TER_16Collisions    	0x8000  // > 16 collisions

extern unsigned short cs8900a_page_rd(unsigned short reg);
extern void cs8900a_page_wr(unsigned short reg, unsigned short data);
extern int cs8900a_init(void);
extern int cs8900a_reset();
extern void cs8900a_set_mac(void);
extern int cs8900a_tx(unsigned char *txbuf, unsigned long length);
extern int cs8900a_rx(uchar *pktbuf);
