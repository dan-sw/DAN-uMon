//==========================================================================
//
// at91sam9263_mem.h
//
// Author(s):    Michael Kelly, Cogent Computer Systems, Inc.
// Contributors: 
// Date:         7/240/2003
// Description:  This file contains the register offsets and bit defines for 
//				 the AT91SAM9263 Static and SDRAM Memory Controllers as well
//				 the EBI/MC section

#ifndef AT91SAM9263_MEM_H
#define AT91SAM9263_MEM_H

// *****************************************************************************
// Chip Configuration User Interface - External Bus Interface Unit
// *****************************************************************************
// Register Offsets
#define MATRIX_TCR	  				0x04	// Bus Matrix TCM Configuration Register
#define CCFG_EBI0_CSA				0x10 	// EBI0 Chip Select Assignment Register
#define CCFG_EBI1_CSA				0x14 	// EBI1 Chip Select Assignment Register

// Bit Defines
// EBI0_CSA - Chip Select Assignment Register
#define CCFG_EBI0_CSA_MVS		   	BIT16	// 1 = Memories are 3.3V, 0 = Memories are 1.8V
#define CCFG_EBI0_CSA_DBPU			BIT8	// 1 = Disable EBI0 D0-15 pullups
#define CCFG_EBI0_CSA_CS5_CF2	   	BIT5	// 1 = CS5 is assigned to SMC and Compact Flash (second slot) is Activated, 0 = Chip Select
#define CCFG_EBI0_CSA_CS4_CF1	   	BIT4	// 1 = CS4 is assigned to SMC and Compact Flash (first slot) is Activated, 0 = Chip Select
#define CCFG_EBI0_CSA_CS3_NAND		BIT3	// 1 = CS3 is assigned to SMC and SmartMedia (NAND) is Activated, 0 = Chip Select
#define CCFG_EBI0_CSA_CS1_SDRAM		BIT1	// 1 = CS1 is assigned to SDRAM, 0 = Chip Select

// Bit Defines
// EBI1_CSA - Chip Select Assignment Register
#define CCFG_EBI1_CSA_MVS		   	BIT16	// 1 = Memories are 3.3V, 0 = Memories are 1.8V
#define CCFG_EBI1_CSA_DBPU			BIT8	// 1 = Disable EBI1 D0-15 pullups
#define CCFG_EBI1_CSA_CS3_NAND		BIT3	// 1 = CS3 is assigned to SMC and SmartMedia (NAND) is Activated, 0 = Chip Select
#define CCFG_EBI1_CSA_CS1_SDRAM		BIT1	// 1 = CS1 is assigned to SDRAM, 0 = Chip Select

// *****************************************************************************
// Static Memory Interface Unit
// *****************************************************************************
#define SMC_SETUP0 				0x00 	// Chip Select 0 Setup Register 
#define SMC_SETUP1 				0x10 	// Chip Select 1 Setup Register 
#define SMC_SETUP2 				0x20 	// Chip Select 2 Setup Register 
#define SMC_SETUP3 				0x30 	// Chip Select 3 Setup Register 
#define SMC_SETUP4 				0x40 	// Chip Select 4 Setup Register 
#define SMC_SETUP5 				0x50 	// Chip Select 5 Setup Register 

#define SMC_PULSE0 				0x04 	// Chip Select 0 Pulse Register 
#define SMC_PULSE1 				0x14 	// Chip Select 1 Pulse Register 
#define SMC_PULSE2 				0x24 	// Chip Select 2 Pulse Register 
#define SMC_PULSE3 				0x34 	// Chip Select 3 Pulse Register 
#define SMC_PULSE4 				0x44 	// Chip Select 4 Pulse Register 
#define SMC_PULSE5 				0x54 	// Chip Select 5 Pulse Register 

#define SMC_CYCLE0 				0x08 	// Chip Select 0 Cycle Register 
#define SMC_CYCLE1 				0x18 	// Chip Select 1 Cycle Register 
#define SMC_CYCLE2 				0x28 	// Chip Select 2 Cycle Register 
#define SMC_CYCLE3 				0x38 	// Chip Select 3 Cycle Register 
#define SMC_CYCLE4 				0x48 	// Chip Select 4 Cycle Register 
#define SMC_CYCLE5 				0x58 	// Chip Select 5 Cycle Register 

#define SMC_MODE0 				0x0C 	// Chip Select 0 Mode Register 
#define SMC_MODE1 				0x1C 	// Chip Select 1 Mode Register 
#define SMC_MODE2 				0x2C 	// Chip Select 2 Mode Register 
#define SMC_MODE3 				0x3C 	// Chip Select 3 Mode Register 
#define SMC_MODE4 				0x4C 	// Chip Select 4 Mode Register 
#define SMC_MODE5 				0x5C 	// Chip Select 5 Mode Register 

// Bit Defines
// SMC CS0-5 Setup Register
#define	SMC_NCS_RD_SETUP(_x_)		((_x_ & 0x3F) << 24)	// NCS setup length = (128* NCS_RD_SETUP[5] + NCS_RD_SETUP[4:0]) clock cycles
#define	SMC_NRD_SETUP(_x_)			((_x_ & 0x3F) << 16)	// NRD setup length = (128* NRD_SETUP[5] + NRD_SETUP[4:0]) clock cycles
#define	SMC_NCS_WR_SETUP(_x_)		((_x_ & 0x3F) << 8)		// NCS setup length = (128* NCS_WR_SETUP[5] + NCS_WR_SETUP[4:0]) clock cycles
#define	SMC_NWE_SETUP(_x_)			((_x_ & 0x3F) << 0)		// NWE setup length = (128* NWE_SETUP[5] + NWE_SETUP[4:0]) clock cycles

// Bit Defines
// SMC CS0-5 Pulse Register
#define	SMC_NCS_RD_PULSE(_x_)		((_x_ & 0x7F) << 24)	// NCS pulse length = (256* NCS_RD_PULSE[6] + NCS_RD_PULSE[5:0]) clock cyc
#define	SMC_NRD_PULSE(_x_)			((_x_ & 0x7F) << 16)	// NRD pulse length = (256* NRD_PULSE[6] + NRD_PULSE[5:0]) clock cycles
#define	SMC_NCS_WR_PULSE(_x_)		((_x_ & 0x7F) << 8)		// NCS pulse length = (256* NCS_WR_PULSE[6] + NCS_WR_PULSE[5:0]) clock cycles
#define	SMC_NWE_PULSE(_x_)			((_x_ & 0x7F) << 0)		// NWE pulse length = (256* NWE_PULSE[6] + NWE_PULSE[5:0]) clock cycles

// Bit Defines
// SMC CS0-5 Cycle Register
#define	SMC_NRD_CYCLE(_x_)			((_x_ & 0x1FF) << 16)	// Read cycle length = (NRD_CYCLE[8:7]*256 + NRD_CYCLE[6:0]) clock cycles
#define	SMC_NWE_CYCLE(_x_)			((_x_ & 0x1FF) << 0)	// Write cycle length = (NWE_CYCLE[8:7]*256 + NWE_CYCLE[6:0]) clock cycles

// Bit Defines
// SMC CS0-5 Mode Register
#define SMC_MODE_PMEN_EN		BIT24					// 1 = Page Mode Enabled, 0 = Standard Read
#define SMC_MODE_TDF_EN			BIT20					// 1 = TDF Optimization Enabled, 0 = Disabled
#define SMC_MODE_TDF(_x_)		((_x_ & 0xf) << 16)		// Data Float Time
#define SMC_MODE_DBW_8			(0 << 12)				// CS DataBus Width = 8 Bits
#define SMC_MODE_DBW_16			(1 << 12)				// CS DataBus Width = 16 Bits
#define SMC_MODE_DBW_32			(2 << 12)				// CS DataBus Width = 32 Bits
#define SMC_MODE_BAT_BE			0						// Writes and Reads are controlled by BE's
#define SMC_MODE_BAT_WE			BIT8					// Writes are controlled by WE's
#define SMC_MODE_EXNW_DIS		(0 << 4)				// NWAIT signal used to extend the read and write signal = Disabled
#define SMC_MODE_EXNW_FRZ		(2 << 4)				// NWAIT signal used to extend the read and write signal = Frozen
#define SMC_MODE_EXNW_RDY		(3 << 4)				// NWAIT signal used to extend the read and write signal = Ready
#define SMC_MODE_WRITE_NCS		0						// The write operation is controlled by the NCS signal
#define SMC_MODE_WRITE_NWE		BIT1					// The write operation is controlled by the NWE signal
#define SMC_MODE_READ_NCS 		0						// The read operation is controlled by the NCS signal
#define SMC_MODE_READ_NRD 		BIT0					// The read operation is controlled by the NRD signal

// *****************************************************************************
// SDRAM Memory Interface Unit
// *****************************************************************************
#define SDRC_MR   				0x00 		// Mode Register
#define SDRC_TR   				0x04 		// Refresh Timer Register
#define SDRC_CR   				0x08 		// Configuration Register
#define SDRC_LPR  				0x10 		// Low Power Register
#define SDRC_IER  				0x14 		// Interrupt Enable Register
#define SDRC_IDR  				0x18 		// Interrupt Disable Register
#define SDRC_IMR  				0x1C 		// Interrupt Mask Register
#define SDRC_ISR  				0x20 		// Interrupt Status Register
#define SDRC_MDR  				0x20 		// Memory Device Register

// Bit Defines
// SDRC_MR - Mode Register
#define SDRC_MR_NORM			(0 << 0)	// Normal Mode - All accesses to SDRAM are decoded normally
#define SDRC_MR_NOP				(1 << 0)	// NOP Command is sent to SDRAM
#define SDRC_MR_PRE				(2 << 0)	// Precharge All Command is sent to SDRAM
#define SDRC_MR_MRS				(3 << 0)	// Mode Register Set Command is sent to SDRAM
#define SDRC_MR_REF				(4 << 0)	// Refresh Command is sent to SDRAM
#define SDRC_MR_EMRS			(5 << 0)	// Extended Mode Register Set Command is sent to SDRAM
#define SDRC_MR_DPD				(6 << 0)	// Deep Power-Down Mode

// SDRC_TR - Refresh Timer Register
#define SDRC_TR_COUNT(_x_)		((_x_ & 0xfff) << 0)

// SDRC_CR - Configuration Register
#define SDRC_CR_TXSR(_x_)		((_x_ & 0xf) << 28)	// CKE to ACT Time
#define SDRC_CR_TRAS(_x_)		((_x_ & 0xf) << 24)	// ACT to PRE Time
#define SDRC_CR_TRCD(_x_)		((_x_ & 0xf) << 20)	// RAS to CAS Time
#define SDRC_CR_TRP(_x_)		((_x_ & 0xf) << 16)	// PRE to ACT Time
#define SDRC_CR_TRC(_x_)		((_x_ & 0xf) << 12)	// REF to ACT Time
#define SDRC_CR_TWR(_x_)		((_x_ & 0xf) << 8)	// Write Recovery Time
#define SDRC_CR_DBW_16			BIT7				// 1 = SDRAM is 16-bits wide, 0 = 32-bits
#define SDRC_CR_CAS_1			(1 << 5)			// Cas Delay = 1
#define SDRC_CR_CAS_2			(2 << 5)			// Cas Delay = 2
#define SDRC_CR_CAS_3			(3 << 5)			// Cas Delay = 3
#define SDRC_CR_NB_2			0					// 2 Banks per device
#define SDRC_CR_NB_4			BIT4				// 4 Banks per device
#define SDRC_CR_NR_11			(0 << 2)			// Number of rows = 11
#define SDRC_CR_NR_12			(1 << 2)			// Number of rows = 12
#define SDRC_CR_NR_13			(2 << 2)			// Number of rows = 13
#define SDRC_CR_NC_8			(0 << 0)			// Number of columns = 8
#define SDRC_CR_NC_9			(1 << 0)			// Number of columns = 9
#define SDRC_CR_NC_10			(2 << 0)			// Number of columns = 10
#define SDRC_CR_NC_11			(3 << 0)			// Number of columns = 11

// SDRC_LPR - Low Power Register
#define SDRC_LPR_TIMEOUT		(0 << 12)			// Low-Power Mode active immediately after end of last transfer
#define SDRC_LPR_TIMEOUT64		(1 << 12)			// Low-Power Mode active 64 cycles after end of last transfer
#define SDRC_LPR_TIMEOUT128		(2 << 12)			// Low-Power Mode active 128 cycles after end of last transfer
#define SDRC_LPR_DS(_x_)		((_x_ & 0x3) << 10)	// Drive Strength (only for low-power SDRAM)
#define SDRC_LPR_TCSR(_x_)		((_x_ & 0x3) << 8)	// Temperature Compensated Self-Refresh (only for low-power SDRAM)
#define SDRC_LPR_PASR(_x_)		((_x_ & 0x7) << 4)	// Partial Array Self-refresh (only for low-power SDRAM)
#define SDRC_LPR_LPCB			(0 << 0)			// Low Power Feature is inhibited
#define SDRC_LPR_LPCB_SR 		(1 << 0)			// Self-Refresh Command, SDCLK deactivated, SDCKE set low
#define SDRC_LPR_LPCB_PD 		(2 << 0)			// Power-Down Command, SDCKE set low
#define SDRC_LPR_LPCB_DPD		(3 << 0)			// Deep Power-Down Command

// SDRC_IER - Interrupt Enable Register
// SDRC_IDR - Interrupt Disable Register
// SDRC_ISR - Interrupt Mask Register
// SDRC_IMR - Interrupt Mask Register
#define SDRC_INT_RES			BIT0		// Refresh Error Status

//SDRC_MDR - Memory Device Register
#define SDRC_MDR_SDRAM			(0 << 0)			// Device Type is SDRAM
#define SDRC_MDR_LPSDRAM		(1 << 0)			// Device Type is Low-Power SDRAM

#endif

