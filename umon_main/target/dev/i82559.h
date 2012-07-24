//=============================================================================
//
//      82559.h
//
//      Intel 82559/er defines
//
// Author(s):    Michael Kelly, Cogent Computer Systems, Inc.
// Date:         2002-02-10
// Description:  This file contains the defines for the Intel 82559 and 82559er
//
//
//=============================================================================

// Error returns
#define SCB_PORT_ST_ERR			1
#define CBL_CMD_CFG_TIMEOUT		2
#define CBL_CMD_CFG_ERR			3
#define CBL_CMD_TX_TIMEOUT		4
#define CBL_CMD_TX_ERR			5
#define RU_CMD_TIMEOUT 			6
#define CU_CMD_TIMEOUT 			7
#define CU_STAT_TIMEOUT			8

// PCI Vendor and Device ID's
#define I82559_VENID			0x8086
#define I82559_DEVID			0x1229	// also the same as the i82558
#define I82559ER_DEVID			0x1209

// ------------------------------------------------------------------------
// SCB Offsets
// ------------------------------------------------------------------------
#define SCB_STAT				0x00	// SCB Status Word
#define SCB_CMD         		0x02	// SCB Command Word
#define SCB_PTR      			0x04	// SCB General Purpose Pointer
#define SCB_PORT				0x08	// SCB Port
#define SCB_FCR					0x0c	// Flash Control Register
#define SCB_ECR					0x0e	// EEPROM Control Register
#define SCB_MDI					0x10	// MDI Control Register
#define SCB_RXDMA				0x14	// Receive DMA Byte Count
#define SCB_FLOW_THR			0x19	// Flow Control Threshhold Register
#define SCB_FLOW_CMD			0x1a	// Flow Control Command Register
#define SCB_PMDR				0x1b	// Power Management Driver Registers
#define SCB_GEN_CTL				0x1c	// General Control
#define SCB_GEN_STAT			0x1d	// General Status

//------------------------------------------------------------------------------
// SCB Status
//------------------------------------------------------------------------------
#define SCB_STAT_CX   			0x8000	// CU Command Complete
#define SCB_STAT_FR   			0x4000  // Frame Received
#define SCB_STAT_CNA  			0x2000  // CU Not Active
#define SCB_STAT_RNR  			0x1000  // Receiver Not Ready
#define SCB_STAT_MDI  			0x0800  // MDI Cycle Complete
#define SCB_STAT_SWI  			0x0400  // Software Interrupt
#define SCB_STAT_FCP  			0x0100  // Flow Control Pause Interrupt

// CU Status 
#define SCB_STAT_CU_MASK		0x00c0	// Command Unit Status Mask
#define SCB_STAT_CU_SHIFT		6
#define CUS_IDLE				0x00	// Idle
#define CUS_SUSP				0x01	// Suspended
#define CUS_LPQ					0x02	// Low Priority Queue Active
#define CUS_HPQ					0x03	// High Priority Queue Active

#define SCB_STAT_RU_MASK		0x003c	// Receive Unit Status Mask
#define SCB_STAT_RU_SHIFT		2
#define RUS_IDLE  				0x00	// Idle
#define RUS_SUSP   				0x01	// Suspended
#define RUS_NORES 				0x02	// No Resources
#define RUS_READY 				0x04	// Ready
#define RUS_SUSP_NO_RBD   		0x09	// Suspended with No RBD's
#define RUS_NO_RES_NO_RBD 		0x0a	// No Resources, No RBD's
#define RUS_READY_NO_RBD 		0x0c	// Ready with No RBD's

//------------------------------------------------------------------------
// SCB Commands (Note - The upper 8 bits are the interrupt mask bits.
//               See SCB_STAT for definitions)
//------------------------------------------------------------------------
// Command Unit
#define SCB_CMD_CU_MASK			0x00f0	// Command Unit Command Mask
#define SCB_CMD_CU_SHIFT		4
#define SCB_CU_NOP          	0x0000	// No Op
#define	SCB_CU_START        	0x0010	// Start the Command in the CBL
#define	SCB_CU_RESUME       	0x0020	// Execute Next Command (if any)
#define	SCB_CU_START_HPQ		0x0030	// Start the Command in the High Priority CBL
#define	SCB_CU_DUMP_ADD	    	0x0040	// Load Dump Counters Address
#define	SCB_CU_DUMP_STATS    	0x0050	// Dump Statistical Counters
#define	SCB_CU_BASE_ADD			0x0060	// Load CU Base Address from SCB_PTR
#define	SCB_CU_DUMP_STATS_RST	0x0070	// Dump Statistical Counters, then Reset them
#define SCB_CU_STATIC_RESUME	0x00a0	// Execute Next Command (if any) even if Suspended
#define SCB_CU_HPQ_RESUME		0x00b0	// Execute Next High Priority Command (if any)

// Receive Unit
#define SCB_CMD_RU_MASK			0x0003	// Receive Unit Command Mask
#define SCB_CMD_RU_SHIFT		0
#define SCB_RU_NOP         		0x0000	// No Op
#define	SCB_RU_START       		0x0001	// Start Receiving into RFA pointed to by SCB_PTR
#define	SCB_RU_RESUME      		0x0002	// Resume Receiving From Suspended State
#define	SCB_RU_RESUME_DMA  		0x0003	// Resume Receiving From Suspended State if in DMA Mode
#define SCB_RU_ABORT       		0x0004	// Abort Current Receive Operation
#define SCB_RU_LOAD_HDS       	0x0005	// Load the Header Data Size for DMA or HDS Mode
#define	SCB_RU_BASE_ADD  		0x0006  // Load RU Base Address from SCB_PTR
#define SCB_RU_RESUME_RBD    	0x0007	// Resume from RUS_NO_RES_NO_RBD or RUS_SUSP_NO_RBD states only

// Interrupt Mask Bits
#define SCB_IM_M				0x0100	// When set, masks all interrupts
#define SCB_IM_SI				0x0200	// When set, forces an interrupt
#define SCB_IM_FCP				0x0400	// When set, masks Flow Control Pause interrupts
#define SCB_IM_ER				0x0800	// When set, masks 
#define SCB_IM_RNR				0x1000	// When set, masks Receiver Not Ready interrupts
#define SCB_IM_CNA				0x2000	// When set, masks Command Unit Not Active interrupts
#define SCB_IM_FR				0x4000	// When set, masks Frame Received interrupts
#define SCB_IM_CX				0x8000	// When set, masks Command Complete interrupts

//------------------------------------------------------------------------
// PORT Commands
//------------------------------------------------------------------------
#define SCB_PORT_RST            0x0000 	// Software Reset
#define SCB_PORT_ST         	0x0001 	// 82559 Self Test
#define SCB_PORT_SEL_RST  		0x0002	// Selective Reset
#define SCB_PORT_DUMP           0x0003	// Dump Statistics
#define SCB_PORT_DUMP_WU      	0x0007	// Dump Statistics, then Wakeup

// Self Test Status Offsets
#define PORT_ST_CHECKT			0x00	// ROM Checksum
#define PORT_ST_STAT			0x04	// Self Test Status Bits

// Self Test Status Bits - Set means Failure
#define PORT_ST_GEN				0x0800	// General Self Test
#define PORT_ST_DIAG			0x0040	// Internal Diagnostics
#define PORT_ST_REG				0x0004	// Register Diagnostics
#define PORT_ST_ROM				0x0002	// Internal ROM Contents

//------------------------------------------------------------------------
// Command Block List
//------------------------------------------------------------------------
// Offsets - All commands
#define CBL_STAT				0x00
#define CBL_CMD					0x02
#define CBL_LINK				0x04
#define CBL_DATA				0x08	// Start of individual command data field

// Control Bits - All Commands
#define CBL_CMD_EL				0x8000	// End of List when set
#define CBL_CMD_S				0x4000	// Suspend upon command completion when set
#define CBL_CMD_I				0x2000	// Generate Interrupt upon command completion when set

// Command Field
#define CBL_CMD_IAS				0x0001	// Individual Address Setup
#define CBL_CMD_CFG				0x0002	// Configure
#define CBL_CMD_MCAS			0x0003	// Multicast Address Setup
#define CBL_CMD_TX				0x0004	// Transmit
#define CBL_CMD_MICRO			0x0005	// Load Micro Code
#define CBL_CMD_DUMP			0x0006	// Dump Internal Registers
#define CBL_CMD_DIAG			0x0007	// CSMA/CD Subsystem Diagnotic Test

// Status Bits
#define CBL_STAT_C				0x8000	// Command Complete
#define CBL_STAT_OK				0x2000	// Completion Status OK

// TX Command Additional Offsets
#define CBL_TxCB_TBD			0x08	// Transmit Buffer Descriptor Starting Address - Flexible Mode only
#define CBL_TxCB_CNT			0x0c	// Transmit Byte Count
#define CBL_TxCB_THRESH			0x0e	// Transmit Threshhold
#define CBL_TxCB_TBD_NUM		0x0f	// TBD Number - Flexible Mode Only
#define CBL_TxCB_DATA			0x10	// Start of data field in simplified mode

// TX Command Additional bits
#define CBL_CMD_SF				0x0008	// Simple(0)/Flexible(1) mode bit
#define CBL_CMD_NC				0x0010	// No-CRC when set

// TX EOF Flag in COUNT
#define CBL_TxCB_CNT_EOF		0x8000	// Must be set for simplified mode


//------------------------------------------------------------------------
// Receive Frame Descriptors
//------------------------------------------------------------------------
// Offsets
#define RFD_STAT       		0x00
#define RFD_CMD				0x02
#define RFD_LINK         	0x04
#define RFD_RDB_ADDR     	0x08
#define RFD_CNT       		0x0c
#define RFD_SIZE        	0x0e
#define RFD_DATA      		0x10

// Command control bits
#define RFD_CMD_EL   		0x8000       // 1=last RFD in RFA
#define RFD_CMD_S    		0x4000       // 1=suspend RU after receiving frame
#define RFD_CMD_H    		0x0010       // 1=RFD is a header RFD
#define RFD_CMD_SF   		0x0008       // 0=simplified, 1=flexible mode

// Command Status Bits
#define RFD_STAT_C    		0x8000       // completion of received frame
#define RFD_STAT_OK   		0x2000       // frame received with no errors

// Count Status
#define RFD_CNT_MASK     	0x3fff
#define RFD_CNT_F        	0x4000
#define RFD_CNT_EOF      	0x8000

// Packet Status Bits      
#define RFD_RX_CRC          0x0800  // crc error
#define RFD_RX_ALIGNMENT    0x0400  // alignment error
#define RFD_RX_RESOURCE     0x0200  // out of space, no resources
#define RFD_RX_DMA_OVER     0x0100  // DMA overrun
#define RFD_RX_SHORT        0x0080  // short frame error
#define RFD_RX_LENGTH       0x0020  //
#define RFD_RX_ERROR        0x0010  // receive error
#define RFD_RX_NO_ADR_MATCH 0x0004  // no address match
#define RFD_RX_IA_MATCH     0x0002  // individual address does not match
#define RFD_RX_TCO          0x0001  // TCO indication


extern unsigned long i82559_init(void);
extern unsigned long i82559_cbl_cfg(unsigned long promisc);
extern unsigned long i82559_cbl_ias(unsigned char *mac_addr);
extern unsigned long i82559_send(unsigned char *txbuf,
	unsigned long length);
extern unsigned short i82559_rx_get(volatile unsigned char *pktbuf);
extern void i82559_rx_init(void);
extern unsigned long i82559_wait_for_cu_cmd(void);
extern unsigned long i82559_wait_for_ru_cmd(void);
extern void i82559_reset(void);


