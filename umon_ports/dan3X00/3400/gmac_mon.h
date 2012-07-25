/*
All files except if stated otherwise in the begining of the file are under the ISC license:
-----------------------------------------------------------------------------------

Copyright (c) 2010-2012 Design Art Networks Ltd.

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

-----------------------------------------------------------------------------------
*/
/**********************************************************************\
 Library    :  uMON (Micro Monitor)
 Filename   :  gmac_mon.h
 Purpose    :  DAN3400 GMAC Ethernet driver for uMON only
 Owner		:  Sergey Krasnitsky
 Created    :  21.11.2010
 Note		:  Based on the original driver: Drivers/gmac/src/gmac.c
\**********************************************************************/


/**********************************************************************\
							GMAC addresses
\**********************************************************************/

#ifdef DAN2400
#define GMAC_BASE				0x20006000
#define GMAC_BUF_ADDR			(uint32)(GMAC_BUFs)
#define GMAC_DESC_ADDR			(uint32)(GMAC_DESCs)
#else
#define USE_GMAC_0
#ifdef USE_GMAC_1
#define GMAC_BASE				0xe577E000
#else
#define GMAC_BASE				0xe5738000
#endif
#ifndef USE_NPU_CB_ONLY
#define GMAC_BUF_ADDR			((uint32)(GMAC_BUFs) | 0x80000000)
#define GMAC_DESC_ADDR			((uint32)(GMAC_DESCs) | 0x80000000)
#else
#define GMAC_BUF_ADDR			((uint32)(GMAC_BUFs) | 0x60400000)
#define GMAC_DESC_ADDR			((uint32)(GMAC_DESCs) | 0x60400000)
#endif
#endif
#define GMAC_DMA_OFFSET			0x1000
#define GMAC_BUF_NUM			7
#define GMAC_BUF_SIZE			2048
#define GMAC_RX_DESC_NUM		3
#define GMAC_TX_DESC_NUM		2

#if 0
#define PHY_CRT_EXT_REG_TX_DELAY		(1<<1)
#define PHY_CRT_EXT_REG_RX_DELAY		(1<<7)
#define GMAC_PHY_CRT_EXT_REG_OFFSET		(20)
#else
#define PHY_CRT_EXT_REG_TX_DELAY		(1<<14)
#define PHY_CRT_EXT_REG_RX_DELAY		(1<<15)
#define GMAC_PHY_CRT_EXT_REG_OFFSET		(1)
#endif
#define PHY_RETRIES						(1000)
#define PHY_DEV_ADDR					(0x15)

/**********************************************************************\
					GMAC configuration registers 
\**********************************************************************/

/* 
 GMAC registers, base address is BAR+GmacRegistersBase
*/
enum GmacRegisters
{
	GmacConfig         = 0x00,    // config 
	GmacFrameFilter    = 0x04,    // frame filter 
	GmacHashHigh       = 0x08,    // multi-cast hash table high 
	GmacHashLow        = 0x0C,    // multi-cast hash table low 
	GmacGmiiAddr       = 0x10,    // GMII address 
	GmacGmiiData       = 0x14,    // GMII data 
	GmacFlowControl    = 0x18,    // Flow control 
	GmacVlan           = 0x1C,    // VLAN tag 
	GmacAddr0High      = 0x40,    // address0 high 
	GmacAddr0Low       = 0x44,    // address0 low 
	GmacAddr1High      = 0x48,    // address1 high 
	GmacAddr1Low       = 0x4C,    // address1 low 
	GmacAddr2High      = 0x50,    // address2 high 
	GmacAddr2Low       = 0x54,    // address2 low 
	GmacAddr3High      = 0x58,    // address3 high 
	GmacAddr3Low       = 0x5C,    // address3 low 
	GmacAddr4High      = 0x60,    // address4 high 
	GmacAddr4Low       = 0x64,    // address4 low 
	GmacAddr5High      = 0x68,    // address5 high 
	GmacAddr5Low       = 0x6C,    // address5 low 
	GmacAddr6High      = 0x70,    // address6 high 
	GmacAddr6Low       = 0x74,    // address6 low 
	GmacAddr7High      = 0x78,    // address7 high 
	GmacAddr7Low       = 0x7C,    // address7 low 
	GmacAddr8High      = 0x80,    // address8 high 
	GmacAddr8Low       = 0x84,    // address8 low 
	GmacAddr9High      = 0x88,    // address9 high 
	GmacAddr9Low       = 0x8C,    // address9 low 
	GmacAddr10High     = 0x90,    // address10 high 
	GmacAddr10Low      = 0x94,    // address10 low 
	GmacAddr11High     = 0x98,    // address11 high 
	GmacAddr11Low      = 0x9C,    // address11 low 
	GmacAddr12High     = 0xA0,    // address12 high 
	GmacAddr12Low      = 0xA4,    // address12 low 
	GmacAddr13High     = 0xA8,    // address13 high 
	GmacAddr13Low      = 0xAC,    // address13 low 
	GmacAddr14High     = 0xB0,    // address14 high 
	GmacAddr14Low      = 0xB4,    // address14 low 
	GmacAddr15High     = 0xB8,    // address15 high 
	GmacAddr15Low      = 0xBC,    // address15 low 
};


/* 
  GMAC Config register layout
*/
enum GmacConfigReg      
{											   // Bit description                      R/W   Reset value 
	GmacCfgTransmEnable      = 0x01000000,     // Enable Configuration transmit        RW                
	GmacCfgTransmDisable     = 0,              // Disable Configuration transmit                 0       

	GmacWatchdogDisable      = 0x00800000,     // Disable watchdog timer               RW                
	GmacWatchdogEnable       = 0,              // Enable watchdog timer                          0       

	GmacJabberDisable        = 0x00400000,     // Disable jabber timer                 RW                
	GmacJabberEnable         = 0,              // Enable jabber timer                            0       

	GmacFrameBurstEnable     = 0x00200000,     // Enable frame bursting                RW                
	GmacFrameBurstDisable    = 0,              // Disable frame bursting                         0       

	GmacJumboFrameEnable     = 0x00100000,     // Enable jumbo frame                   RW                
	GmacJumboFrameDisable    = 0,              // Disable jumbo frame                            0       

	// CHANGE: Added on 07/28 SNPS
	GmacInterFrameGap7       = 0x000E0000,     // IFG Config7 - 40 bit times           RW        000     
	GmacInterFrameGap6       = 0x000C0000,     // IFG Config6 - 48 bit times                             
	GmacInterFrameGap5       = 0x000A0000,     // IFG Config5 - 56 bit times                             
	GmacInterFrameGap4       = 0x00080000,     // IFG Config4 - 64 bit times                             
	GmacInterFrameGap3       = 0x00040000,     // IFG Config3 - 72 bit times                             
	GmacInterFrameGap2       = 0x00020000,     // IFG Config2 - 80 bit times                             
	GmacInterFrameGap1       = 0x00010000,     // IFG Config1 - 88 bit times                             
	GmacInterFrameGap0       = 000,            // IFG Config0 - 96 bit times                             

	GmacSelectMii            = 0x00008000,     // Select MII mode                      RW                
	GmacSelectGmii           = 0,              // Select GMII mode                               0       

	GmacSelect100M           = 0x00004000,     // Select 100M mode                     RW                
	GmacSelect10M            = 0,              // Select 10M mode                                0       

	//  CHANGE: Commented as Endian mode is not register configurable
	//  GmacBigEndian            = 0x00004000,     // Big endian mode                    RW                
	//  GmacLittleEndian         = 0,              // Little endian                                0       

	GmacDisableRxOwn         = 0x00002000,     // Disable receive own packets          RW                
	GmacEnableRxOwn          = 0,              // Enable receive own packets                     0       

	GmacLoopbackOn           = 0x00001000,     // Loopback mode                        RW                
	GmacLoopbackOff          = 0,              // Normal mode                                    0       

	GmacFullDuplex           = 0x00000800,     // Full duplex mode                     RW                
	GmacHalfDuplex           = 0,              // Half duplex mode                               0       

	GmacRetryDisable         = 0x00000200,     // Disable retransmission               RW                
	GmacRetryEnable          = 0,              // Enable retransmission                          0       

	GmacLinkUp               = 0x00000100,     // Put Link UP                          RW                
	GmacLinkDown             = 0,              // Put Link DOWN                                  0       

	// CHANGE: 07/28 renamed GmacCrcStrip* GmacPadCrcStrip*
	GmacPadCrcStripEnable    = 0x00000080,     // Pad / Crc stripping enable           RW                
	GmacPadCrcStripDisable   = 0,              // Pad / Crc stripping disable                    0       

	GmacBackoffLimit3        = 0x00000060,     // Back-off limit                       RW                
	GmacBackoffLimit2        = 0x00000040,     //                                                        
	GmacBackoffLimit1        = 0x00000020,     //                                                        
	GmacBackoffLimit0        = 00,             //                                                00      

	GmacDeferralCheckEnable  = 0x00000010,     // Deferral check enable                RW                
	GmacDeferralCheckDisable = 0,              // Deferral check disable                         0       

	GmacTxEnable             = 0x00000008,     // Transmitter enable                   RW                
	GmacTxDisable            = 0,              // Transmitter disable                            0       

	GmacRxEnable             = 0x00000004,     // Receiver enable                      RW                
	GmacRxDisable            = 0,              // Receiver disable                               0       
};


/* 
  GMAC frame filter register layout
*/
enum GmacFrameFilterReg 
{
	GmacFilterOff            = 0x80000000,     // Receive all incoming packets         RW                
	GmacFilterOn             = 0,              // Receive filtered packets only                  0       

	// CHANGE: Added on 07/28 SNPS
	GmacSrcAddrFilterEnable  = 0x00000200,     // Source Address Filter enable         RW                
	GmacSrcAddrFilterDisable = 0,              //                                                0       

	// CHANGE: Added on 07/28 SNPS
	GmacSrcInvAddrFilterEn   = 0x00000100,     // Inverse Source Address Filter enable RW                
	GmacSrcInvAddrFilterDis  = 0,              //                                                0       

	// CHANGE: Changed the control frame config (07/28)
	GmacPassControl3         = 0x000000C0,     // Forwards control frames that pass AF RW                
	GmacPassControl2         = 0x00000080,     // Forwards all control frames                            
	GmacPassControl1         = 0x00000040,     // Does not pass control frames                           
	GmacPassControl0         = 00,             // Does not pass control frames                   00      

	GmacBroadcastDisable     = 0x00000020,     // Disable reception of broadcast frames RW               
	GmacBroadcastEnable      = 0,              // Enable broadcast frames                        0       

	GmacMulticastFilterOff   = 0x00000010,     // Pass all multicast packets           RW                
	GmacMulticastFilterOn    = 0,              // Pass filtered multicast packets                0       

	// CHANGE: Changed to Dest Addr Filter Inverse (07/28)
	GmacDestAddrFilterInv    = 0x00000008,     // Inverse filtering for DA             RW                
	GmacDestAddrFilterNor    = 0,              // Normal filtering for DA                        0       

	// CHANGE: Changed to Multicast Hash filter (07/28)
	GmacMcastHashFilterOn    = 0x00000004,     // perform multicast hash filtering     RW                
	GmacMcastHashFilterOff   = 0,              // perfect filtering only                         0       

	// CHANGE: Changed to Unicast Hash filter (07/28)
	GmacUcastHashFilterOn    = 0x00000002,     // Unicast Hash filtering only          RW                
	GmacUcastHashFilterOff   = 0,              // perfect filtering only                         0       

	GmacPromiscuousModeOn    = 0x00000001,     // Receive all valid packets            RW                
	GmacPromiscuousModeOff   = 0,              // Receive filtered packets only                  0       
};


/* 
  GMII address register layout
*/
enum GmacGmiiAddrReg      
{
	GmiiDevMask    = 0x0000F800,     // GMII device address 
	GmiiDevShift   = 11,

	GmiiRegMask    = 0x000007C0,     // GMII register 
	GmiiRegShift   = 6,

	// CHANGED: 3-bit config instead of older 2-bit (07/28)
	GmiiAppClk5    = 0x00000014,     // Application Clock Range 250-300 MHz 
	GmiiAppClk4    = 0x00000010,     //                         150-250 MHz 
	GmiiAppClk3    = 0x0000000C,     //                         35-60 MHz 
	GmiiAppClk2    = 0x00000008,     //                         20-35 MHz 
	GmiiAppClk1    = 0x00000004,     //                         100-150 MHz 
	GmiiAppClk0    = 00,             //                         60-100 MHz 

	GmiiWrite      = 0x00000002,     // Write to register 
	GmiiRead       = 0,              // Read from register 

	GmiiBusy       = 0x00000001,     // GMII interface is busy 
};


/* 
  GMII address register layout
*/
enum GmacGmiiDataReg
{
	GmiiDataMask = 0x0000FFFF,     // GMII Data 
};


/* 
  GMAC flow control register layout
*/
enum GmacFlowControlReg  
{                                            // Bit description                        R/W   Reset value 
	GmacPauseTimeMask        = 0xFFFF0000,   // PAUSE TIME field in the control frame  RW      0000      
	GmacPauseTimeShift       = 16,

	// CHANGED: Added on (07/28)
	GmacPauseLowThresh3      = 0x00000030,   // threshold for pause tmr 256 slot time  RW        00      
	GmacPauseLowThresh2      = 0x00000020,   //                         144 slot time                    
	GmacPauseLowThresh1      = 0x00000010,   //                         28  slot time                    
	GmacPauseLowThresh0      = 00,           //                         4   slot time                    

	GmacUnicastPauseFrameOn  = 0x00000008,   // Detect pause frame with unicast addr.  RW                
	GmacUnicastPauseFrameOff = 0,            // Detect only pause frame with multicast addr.     0       

	GmacRxFlowControlEnable  = 0x00000004,   // Enable Rx flow control                 RW                
	GmacRxFlowControlDisable = 0,            // Disable Rx flow control                          0       

	GmacTxFlowControlEnable  = 0x00000002,   // Enable Tx flow control                 RW                
	GmacTxFlowControlDisable = 0,            // Disable flow control                             0       

	GmacSendPauseFrame       = 0x00000001,   // send pause frame                       RW        0       
};


/* 
  DMA engine registers, base address is BAR+DmaRegistersBase
*/
enum DmaRegisters             
{
	DmaBusMode        = 0x00,    // CSR0 - Bus Mode 
	DmaTxPollDemand   = 0x04,    // CSR1 - Transmit Poll Demand 
	DmaRxPollDemand   = 0x08,    // CSR2 - Receive Poll Demand 
	DmaRxBaseAddr     = 0x0C,    // CSR3 - Receive list base address 
	DmaTxBaseAddr     = 0x10,    // CSR4 - Transmit list base address 
	DmaStatus         = 0x14,    // CSR5 - Dma status 
	DmaControl        = 0x18,    // CSR6 - Dma control 
	DmaInterrupt      = 0x1C,    // CSR7 - Interrupt enable 
	DmaMissedFr       = 0x20,    // CSR8 - Missed Frame Counter 
	DmaTxCurrAddr     = 0x50,    // CSR20 - Current host transmit buffer address 
	DmaRxCurrAddr     = 0x54,    // CSR21 - Current host receive buffer address 
};


/* 
  DMA Status register
*/
enum DmaStatusReg         
{                                           // Bit description                        R/W   Reset value 
	// CHANGED: Added on 07/28
	DmaLineIntfIntr         = 0x04000000,   // Line interface interrupt               R         0       
	// CHANGED: Added on 07/28
	DmaErrorBit2            = 0x02000000,   // err. 0-data buffer, 1-desc. access     R         0       
	DmaErrorBit1            = 0x01000000,   // err. 0-write trnsf, 1-read transfr     R         0       
	DmaErrorBit0            = 0x00800000,   // err. 0-Rx DMA, 1-Tx DMA                R         0       

	DmaTxState              = 0x00700000,   // Transmit process state                 R         000     
	DmaTxStopped            = 0x00000000,   // Stopped                                                  
	DmaTxFetching           = 0x00100000,   // Running - fetching the descriptor                        
	DmaTxWaiting            = 0x00200000,   // Running - waiting for end of transmission                
	DmaTxReading            = 0x00300000,   // Running - reading the data from memory                   
	DmaTxSuspended          = 0x00600000,   // Suspended                                                
	DmaTxClosing            = 0x00700000,   // Running - closing descriptor                             

	DmaRxState              = 0x000E0000,   // Receive process state                  R         000     
	DmaRxStopped            = 0x00000000,   // Stopped                                                  
	DmaRxFetching           = 0x00020000,   // Running - fetching the descriptor                        
	// CHANGED: Commented as not applicable (07/28)
	//  DmaRxChecking           = 0x00040000,   // Running - checking for end of packet                   
	DmaRxWaiting            = 0x00060000,   // Running - waiting for packet                             
	DmaRxSuspended          = 0x00080000,   // Suspended                                                
	DmaRxClosing            = 0x000A0000,   // Running - closing descriptor                             
	// CHANGED: Commented as not applicable (07/28)
	//  DmaRxFlushing           = 0x000C0000,   // Running - flushing the current frame                   
	DmaRxQueuing            = 0x000E0000,   // Running - queuing the recieve frame into host memory     

	DmaIntNormal            = 0x00010000,   // Normal interrupt summary               RW        0       
	DmaIntAbnormal          = 0x00008000,   // Abnormal interrupt summary             RW        0       

	DmaIntEarlyRx           = 0x00004000,   // Early receive interrupt (Normal)       RW        0       
	DmaIntBusError          = 0x00002000,   // Fatal bus error (Abnormal)             RW        0       
	DmaIntEarlyTx           = 0x00000400,   // Early transmit interrupt (Abnormal)    RW        0       
	DmaIntRxWdogTO          = 0x00000200,   // Receive Watchdog Timeout (Abnormal)    RW        0       
	DmaIntRxStopped         = 0x00000100,   // Receive process stopped (Abnormal)     RW        0       
	DmaIntRxNoBuffer        = 0x00000080,   // Receive buffer unavailable (Abnormal)  RW        0       
	DmaIntRxCompleted       = 0x00000040,   // Completion of frame reception (Normal) RW        0       
	DmaIntTxUnderflow       = 0x00000020,   // Transmit underflow (Abnormal)          RW        0       
	// CHANGED: Added on 07/28
	DmaIntRcvOverflow       = 0x00000010,   // Receive Buffer overflow interrupt      RW        0       
	DmaIntTxJabberTO        = 0x00000008,   // Transmit Jabber Timeout (Abnormal)     RW        0       
	DmaIntTxNoBuffer        = 0x00000004,   // Transmit buffer unavailable (Normal)   RW        0       
	DmaIntTxStopped         = 0x00000002,   // Transmit process stopped (Abnormal)    RW        0       
	DmaIntTxCompleted       = 0x00000001,   // Transmit completed (Normal)            RW        0       
};


/* 
  DMA bus mode register
*/
enum DmaBusModeReg         
{                                           // Bit description                        R/W   Reset value 
	// CHANGED: Commented as not applicable (07/28)
	//  DmaBigEndianDesc        = 0x00100000,   // Big endian data buffer descriptors   RW                
	//  DmaLittleEndianDesc     = 0,            // Little endian data descriptors                 0       

	// CHANGED: Added on 07/28
	DmaFixedBurstEnable     = 0x00010000,   // Fixed Burst SINGLE, INCR4, INCR8 or INCR16               
	DmaFixedBurstDisable    = 0,            //             SINGLE, INCR                         0       

	DmaBurstLength32        = 0x00002000,   // Dma burst length = 32                  RW                
	DmaBurstLength16        = 0x00001000,   // Dma burst length = 16                                    
	DmaBurstLength8         = 0x00000800,   // Dma burst length = 8                                     
	DmaBurstLength4         = 0x00000400,   // Dma burst length = 4                                     
	DmaBurstLength2         = 0x00000200,   // Dma burst length = 2                                     
	DmaBurstLength1         = 0x00000100,   // Dma burst length = 1                                     
	DmaBurstLength0         = 0x00000000,   // Dma burst length = 0                             0       

	// CHANGED: Commented as not applicable (07/28)
	//  DmaBigEndianData        = 0x00000080,   // Big endian data buffers              RW                
	//  DmaLittleEndianData     = 0,            // Little endian data buffers                     0       

	DmaDescriptorSkip16     = 0x00000040,   // number of dwords to skip               RW                
	DmaDescriptorSkip8      = 0x00000020,   // between two unchained descriptors                        
	DmaDescriptorSkip4      = 0x00000010,   //                                                          
	DmaDescriptorSkip2      = 0x00000008,   //                                                          
	DmaDescriptorSkip1      = 0x00000004,   //                                                          
	DmaDescriptorSkip0      = 0,            //                                                  0       

	DmaResetOn              = 0x00000001,   // Reset DMA engine                       RW                
	DmaResetOff             = 0,            //                                                  0       
};


/* 
  DMA interrupt enable register
*/
enum  DmaInterruptReg     
{												  // Bit description                        R/W   Reset value 
	DmaIeNormal            = DmaIntNormal     ,   // Normal interrupt enable                 RW        0       
	DmaIeAbnormal          = DmaIntAbnormal   ,   // Abnormal interrupt enable               RW        0       

	DmaIeEarlyRx           = DmaIntEarlyRx    ,   // Early receive interrupt enable          RW        0       
	DmaIeBusError          = DmaIntBusError   ,   // Fatal bus error enable                  RW        0       
	DmaIeEarlyTx           = DmaIntEarlyTx    ,   // Early transmit interrupt enable         RW        0       
	DmaIeRxWdogTO          = DmaIntRxWdogTO   ,   // Receive Watchdog Timeout enable         RW        0       
	DmaIeRxStopped         = DmaIntRxStopped  ,   // Receive process stopped enable          RW        0       
	DmaIeRxNoBuffer        = DmaIntRxNoBuffer ,   // Receive buffer unavailable enable       RW        0       
	DmaIeRxCompleted       = DmaIntRxCompleted,   // Completion of frame reception enable    RW        0       
	DmaIeTxUnderflow       = DmaIntTxUnderflow,   // Transmit underflow enable               RW        0       
	// CHANGED: Added on 07/28
	DmaIeRxOverflow        = DmaIntRcvOverflow,   // Receive Buffer overflow interrupt       RW        0       
	DmaIeTxJabberTO        = DmaIntTxJabberTO ,   // Transmit Jabber Timeout enable          RW        0       
	DmaIeTxNoBuffer        = DmaIntTxNoBuffer ,   // Transmit buffer unavailable enable      RW        0       
	DmaIeTxStopped         = DmaIntTxStopped  ,   // Transmit process stopped enable         RW        0       
	DmaIeTxCompleted       = DmaIntTxCompleted,   // Transmit completed enable               RW        0       
};


/* 
  Status word of DMA descriptor
*/
enum DmaDescriptorStatus    
{
	DescOwnByDma          = 0x80000000,   // Descriptor is owned by DMA engine  
	// CHANGED: Added on 07/29
	DescDAFilterFail      = 0x40000000,   // Rx - DA Filter Fail for the received frame        E  
	DescFrameLengthMask   = 0x3FFF0000,   // Receive descriptor frame length 
	DescFrameLengthShift  = 16,

	TxDescIntEnable       = 0x40000000,   // Tx - interrupt on completion                         
	TxDescLast            = 0x20000000,   // Tx - Last segment of the frame                       
	TxDescFirst           = 0x10000000,   // Tx - First segment of the frame                      
	TxDescEndOfRing       = 0x00200000,   // End of TX descriptors ring                           
	TxDescChain           = 0x00100000,   // TX: Second buffer address is chain address           

	DescError             = 0x00008000,   // Error summary bit  - OR of the following bits:    v  

	DescRxTruncated       = 0x00004000,   // Rx - no more descriptors for receive frame        E  
	// CHANGED: Added on 07/29
	DescSAFilterFail      = 0x00002000,   // Rx - SA Filter Fail for the received frame        E  
	// added by reyaz 
	DescRxLengthError	= 0x00001000,   // Rx - frame size not matching with length field    E  
	DescRxDamaged         = 0x00000800,   // Rx - frame was damaged due to buffer overflow     E  
	// CHANGED: Added on 07/29
	DescRxVLANTag         = 0x00000400,   // Rx - received frame is a VLAN frame               I  
	DescRxFirst           = 0x00000200,   // Rx - first descriptor of the frame                I  
	DescRxLast            = 0x00000100,   // Rx - last descriptor of the frame                 I  
	DescRxLongFrame       = 0x00000080,   // Rx - frame is longer than 1518 bytes              E  
	DescRxCollision       = 0x00000040,   // Rx - late collision occurred during reception     E  
	DescRxFrameEther      = 0x00000020,   // Rx - Frame type - Ethernet, otherwise 802.3          
	DescRxWatchdog        = 0x00000010,   // Rx - watchdog timer expired during reception      E  
	DescRxMiiError        = 0x00000008,   // Rx - error reported by MII interface              E  
	DescRxDribbling       = 0x00000004,   // Rx - frame contains non-integer multiple of 8 bits    
	DescRxCrc             = 0x00000002,   // Rx - CRC error                                    E  

	DescTxTimeout         = 0x00004000,   // Tx - Transmit jabber timeout                      E  
	// CHANGED: Added on 07/29
	DescTxFrameFlushed    = 0x00002000,   // Tx - DMA/MTL flushed the frame due to SW flush    I  
	DescTxLostCarrier     = 0x00000800,   // Tx - carrier lost during transmission             E  
	DescTxNoCarrier       = 0x00000400,   // Tx - no carrier signal from the transmitter       E  
	DescTxLateCollision   = 0x00000200,   // Tx - transmission aborted due to collision        E  
	DescTxExcCollisions   = 0x00000100,   // Tx - transmission aborted after 16 collisions     E  
	DescTxVLANFrame       = 0x00000080,   // Tx - VLAN-type frame                                 
	DescTxCollMask        = 0x00000078,   // Tx - Collision count                                 
	DescTxCollShift       = 3,
	DescTxExcDeferral     = 0x00000004,   // Tx - excessive deferral                           E  
	DescTxUnderflow       = 0x00000002,   // Tx - late data arrival from the memory            E  
	DescTxDeferred        = 0x00000001,   // Tx - frame transmission deferred                      
};


/* 
  DMA control register
*/
enum DmaControlReg        
{                                           // Bit description                        R/W   Reset value 
	DmaRxStoreAndForward    = 0x02000000,   // Rx Store and forward                   RW        0       
	DmaTxStoreAndForward    = 0x00200000,   // Tx Store and forward                   RW        0       
	DmaTxStart              = 0x00002000,   // Start/Stop transmission                RW        0       
	// CHANGED: Added on 07/28
	DmaFwdErrorFrames       = 0x00000080,   // Forward error frames                   RW        0       
	DmaFwdUnderSzFrames     = 0x00000040,   // Forward undersize frames               RW        0       
	DmaTxSecondFrame        = 0x00000004,   // Operate on second frame                RW        0       
	DmaRxStart              = 0x00000002,   // Start/Stop reception                   RW        0       
};


/* 
  Length word of DMA descriptor
*/
enum DmaDescriptorLength    
{
	DescTxIntEnable       = 0x80000000,   // Tx - interrupt on completion                         
	DescTxLast            = 0x40000000,   // Tx - Last segment of the frame                       
	DescTxFirst           = 0x20000000,   // Tx - First segment of the frame                      
	DescTxDisableCrc      = 0x04000000,   // Tx - Add CRC disabled (first segment only)           

	DescEndOfRing         = 0x02000000,   // End of descriptors ring                              
	DescChain             = 0x01000000,   // Second buffer address is chain address               
	DescTxDisablePadd	  = 0x00800000,   // disable padding	

	RxDescEndOfRing       = 0x00008000,   // End of RX descriptors ring                           
	RxDescChain           = 0x00004000,   // RX: Second buffer address is chain address           

	DescSize2Mask         = 0x003FF800,   // Buffer 2 size                                        
	DescSize2Shift        = 11,
	DescSize1Mask         = 0x000007FF,   // Buffer 1 size                                        
	DescSize1Shift        = 0,
};


/* 
  Initial register values
*/
enum GmacInitialRegValues
{
  GmacConfigInitFdx1000       // Full-duplex mode with perfect filter on 
                          = GmacWatchdogEnable | GmacJabberEnable		  | GmacJumboFrameDisable | GmacLinkUp
                          | GmacSelectGmii     | GmacEnableRxOwn          | GmacLoopbackOff		  | GmacCfgTransmEnable
                          | GmacFullDuplex     | GmacRetryEnable          | GmacPadCrcStripDisable
                          | GmacBackoffLimit0  | GmacDeferralCheckDisable | GmacTxEnable          | GmacRxEnable,

  GmacConfigInitFdx110       // Full-duplex mode with perfect filter on 
                          = GmacWatchdogEnable | GmacJabberEnable		  | GmacJumboFrameDisable | GmacLinkUp
                          | GmacSelectMii      | GmacEnableRxOwn          | GmacLoopbackOff       | GmacCfgTransmEnable
                          | GmacFullDuplex     | GmacRetryEnable          | GmacPadCrcStripDisable| GmacSelect100M
                          | GmacBackoffLimit0  | GmacDeferralCheckDisable | GmacTxEnable          | GmacRxEnable,

  GmacFrameFilterInitFdx  // Full-duplex mode 
                          = GmacFilterOn          | GmacPassControl0	  | GmacBroadcastEnable |  GmacSrcAddrFilterDisable
                          | GmacMulticastFilterOn | GmacDestAddrFilterNor | GmacMcastHashFilterOff
                          | GmacPromiscuousModeOff | GmacUcastHashFilterOff,

  GmacFlowControlInitFdx  // Full-duplex mode 
                          = GmacUnicastPauseFrameOff | GmacRxFlowControlEnable | GmacTxFlowControlEnable,

  GmacGmiiAddrInitFdx     // Full-duplex mode 
                          = GmiiAppClk2,

  GmacConfigInitHdx1000       // Half-duplex mode with perfect filter on 
                          = GmacWatchdogEnable | GmacJabberEnable         | GmacFrameBurstEnable  | GmacJumboFrameDisable
                          | GmacSelectGmii     | GmacDisableRxOwn         | GmacLoopbackOff
                          | GmacHalfDuplex     | GmacRetryEnable          | GmacPadCrcStripDisable
                          | GmacBackoffLimit0  | GmacDeferralCheckDisable | GmacTxEnable          | GmacRxEnable,

  GmacConfigInitHdx110       // Half-duplex mode with perfect filter on 
                          = GmacWatchdogEnable | GmacJabberEnable         | GmacFrameBurstEnable  | GmacJumboFrameDisable
                          | GmacSelectMii      | GmacDisableRxOwn         | GmacLoopbackOff
                          | GmacHalfDuplex     | GmacRetryEnable          | GmacPadCrcStripDisable
                          | GmacBackoffLimit0  | GmacDeferralCheckDisable | GmacTxEnable          | GmacRxEnable,

  GmacFrameFilterInitHdx  // Half-duplex mode 
                          = GmacFilterOn          | GmacPassControl0        | GmacBroadcastEnable | GmacSrcAddrFilterDisable
                          | GmacMulticastFilterOn | GmacDestAddrFilterNor   | GmacMcastHashFilterOff
                          | GmacUcastHashFilterOff| GmacPromiscuousModeOff,

  GmacFlowControlInitHdx  // Half-duplex mode 
                          = GmacUnicastPauseFrameOff | GmacRxFlowControlDisable | GmacTxFlowControlDisable,

  GmacGmiiAddrInitHdx     // Half-duplex mode 
                          = GmiiAppClk2,

  DmaBusModeInit          // Little-endian mode 
                          = DmaFixedBurstEnable |   DmaBurstLength8   | DmaDescriptorSkip4       | DmaResetOff,

  DmaControlInit1000      // 1000 Mb/s mode 
                          = DmaRxStoreAndForward | DmaTxStoreAndForward | DmaTxSecondFrame | DmaFwdUnderSzFrames,

  DmaControlInit100       // 100 Mb/s mode 
                          = DmaRxStoreAndForward | DmaTxStoreAndForward | DmaFwdUnderSzFrames,

  DmaControlInit10        // 10 Mb/s mode 
                          = DmaRxStoreAndForward | DmaTxStoreAndForward | DmaFwdUnderSzFrames,

                          // Interrupt groups 
  DmaIntErrorMask         = DmaIntBusError,           // Error 
  DmaIntRxAbnMask         = DmaIntRxNoBuffer,         // receiver abnormal interrupt 
  DmaIntRxNormMask        = DmaIntRxCompleted,        // receiver normal interrupt   
  DmaIntRxStoppedMask     = DmaIntRxStopped,          // receiver stopped 
  DmaIntTxAbnMask         = DmaIntTxUnderflow,        // transmitter abnormal interrupt 
  DmaIntTxNormMask        = DmaIntTxCompleted,        // transmitter normal interrupt 
  DmaIntTxStoppedMask     = DmaIntTxStopped,          // receiver stopped 

  DmaIntEnable            = DmaIeNormal     | DmaIeAbnormal    | DmaIntErrorMask
                          | DmaIntRxAbnMask | DmaIntRxNormMask | DmaIntRxStoppedMask
                          | DmaIntTxAbnMask | DmaIntTxNormMask | DmaIntTxStoppedMask
                          | DmaIeRxWdogTO | DmaIeRxOverflow,
  DmaIntDisable           = 0,
};


/**********************************************************************\
						GMAC data structures
\**********************************************************************/

/*
  This structure is common for both receive and transmit descriptors
*/
typedef struct
{
	uint32   status;	// Status 
	uint32   length;	// Buffer length 
	uint32   buffer1;	// Buffer 1 pointer 
	uint32   buffer2;	// Buffer 2 pointer or next descriptor pointer in chain structure 
	uint32   data1;		// driver data, are not used by DMA engine,                       
	uint32   data2;		//   set DmaDescriptorSkip4 in DmaBusModeInit to skip these words 
	uint32   data3;		//   set DmaDescriptorSkip4 in DmaBusModeInit to skip these words 
	uint32   data4;		//   set DmaDescriptorSkip4 in DmaBusModeInit to skip these words 
} GmacDmaDesc;


/*
  GMAC_process receive callback routine prototype
*/
typedef void (*GmacRcvCb) (uint8 *buf, int len);



/**********************************************************************\
							Public functions
\**********************************************************************/

/*
 * ----------------------------------------------------------------------
 * Description:	Init GMAC Ethernet controller
 * Input:		gmacid  - ID of the GMAC used (0,1)
 * 				macaddr - Ethernet MAC address
 * ----------------------------------------------------------------------
 */
void gmac_Init(uint8 gmacid, const uint8 macaddr[6]);


/*
 * ----------------------------------------------------------------------
 * Description:	Send Ethernet frame, consists from one or more buffers
 * Input:		gmacid - ID of the GMAC used (0,1)
 * 				buf	- Pointer to the first available byte of the buffer
 * 				len - the length of the Ethernet frame to send
 * ----------------------------------------------------------------------
 */
void gmac_Send (uint8 gmacid, uint8 *buf, int len);


/*
 * ----------------------------------------------------------------------
 * Description:	Poll the ethernet for possibly received buffers
 * Input:		gmacid - GMAC id (0,1)
 * 				retlen - pointer for a packet length to be filled 
 * Return:		Received packet or 0 if nothing to receive
 * ----------------------------------------------------------------------
 */
uint8* gmac_Recv (uint8 gmacid, unsigned * retlen);


/*
 * ----------------------------------------------------------------------
 * Description:	Allocate a packet for subsequent send.
 * Return:		Pointer to a buffer of GMAC_BUF_SIZE
 * ----------------------------------------------------------------------
 */
uint8 *gmac_PacketAlloc(void);


/*
 * ----------------------------------------------------------------------
 * Description:	Release previously allocated packet buffer or a packet 
 * 				received by gmac_Recv.
 * Return:		None
 * ----------------------------------------------------------------------
 */
void gmac_PacketFree (uint8 *buf);

