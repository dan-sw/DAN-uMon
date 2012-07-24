//==========================================================================
//
// at91sam9263.h
//
// Author(s):    Michael Kelly, Cogent Computer Systems, Inc.
// Contributors: 
// Date:         6/20/2003
// Description:  This file contains the AT91SAM9263 register defines
//				 and access macros.  Note that some register offsets
//               and most bit defines are located in separate, function
//				 specific files.


#ifndef AT91SAM9263_H
#define AT91SAM9263_H

// *****************************************************************************
// Advanced Interrupt Controller
// *****************************************************************************

// Source Mode Register - 32 of them
#define AIC_SMR_BASE		0xFFFFF000
#define AIC_SMR_REG(_x_)	*(vulong *)(AIC_SMR_BASE + (_x_ & 0x1f))

// Source Vector Register - 32 of them
#define AIC_SVR_BASE		0xFFFFF080
#define AIC_SVR_REG(_x_)	*(vulong *)(AIC_SVR_BASE + (_x_ & 0x1f))

// Control Register - 32 of them
#define AIC_CTL_BASE		0xFFFFF100	
#define AIC_CTL_REG(_x_)	*(vulong *)(AIC_SVR_BASE + (_x_ & 0x1f))

// Register Offsets
#define AIC_IVR 	0x00	// IRQ Vector Register
#define AIC_FVR 	0x04	// FIQ Vector Register
#define AIC_ISR 	0x08	// Interrupt Status Register
#define AIC_IPR 	0x0C	// Interrupt Pending Register
#define AIC_IMR 	0x10	// Interrupt Mask Register
#define AIC_CISR 	0x14	// Core Interrupt Status Register
#define AIC_IECR 	0x20	// Interrupt Enable Command Register
#define AIC_IDCR 	0x24	// Interrupt Disable Command Register
#define AIC_ICCR 	0x28	// Interrupt Clear Command Register
#define AIC_ISCR 	0x2C	// Interrupt Set Command Register
#define AIC_EOICR 	0x30	// End of Interrupt Command Register
#define AIC_SPU 	0x34	// Spurious Vector Register
#define AIC_DCR 	0x38	// Debug Control Register (Protect)
#define AIC_FFER 	0x40	// Fast Forcing Enable Register
#define AIC_FFDR 	0x44	// Fast Forcing Disable Register
#define AIC_FFSR 	0x48	// Fast Forcing Status Register

// *****************************************************************************
// Debug Unit
// *****************************************************************************
#define DBGU_BASE		0xFFFFEE00
#define DBGU_REG(_x_)	*(vulong *)(DBGU_BASE + _x_)

// Register Offsets
#define DBGU_CR 	0x00	// Control Register
#define DBGU_MR 	0x04	// Mode Register
#define DBGU_IER 	0x08	// Interrupt Enable Register
#define DBGU_IDR 	0x0C	// Interrupt Disable Register
#define DBGU_IMR 	0x10	// Interrupt Mask Register
#define DBGU_CSR 	0x14	// Channel Status Register
#define DBGU_RHR 	0x18	// Receiver Holding Register
#define DBGU_THR 	0x1C	// Transmitter Holding Register
#define DBGU_BRGR 	0x20	// Baud Rate Generator Register
#define DBGU_C1R 	0x40	// Chip ID1 Register
#define DBGU_C2R 	0x44	// Chip ID2 Register
#define DBGU_FNTR 	0x48	// Force NTRST Register


// *****************************************************************************
// Peripheral Data Control (DMA)
// Note that each of the following peripherals has it's own
// set of these registers starting at offset 0x100 from it's
// base address: DBGU, SPI, USART and SSC
// To access the DMA for a peripheral, use the macro for that 
// peripheral but with these register offsets
// *****************************************************************************
// Register Offsets
#define PDC_RPR 	0x100	// Receive Pointer Register
#define PDC_RCR 	0x104	// Receive Counter Register
#define PDC_TPR 	0x108	// Transmit Pointer Register
#define PDC_TCR 	0x10c	// Transmit Counter Register
#define PDC_RNPR 	0x110	// Receive Next Pointer Register
#define PDC_RNCR 	0x114	// Receive Next Counter Register
#define PDC_TNPR 	0x118	// Transmit Next Pointer Register
#define PDC_TNCR 	0x11c	// Transmit Next Counter Register
#define PDC_PTCR 	0x120	// PDC Transfer Control Register
#define PDC_PTSR 	0x124	// PDC Transfer Status Register

// *****************************************************************************
// Parallel I/O Unit
// There are five PIO blocks - A, B, C, D and E.  They all have the
// same register set, but different base addresses
// *****************************************************************************
// Port A
#define PIOA_BASE		0xFFFFF200
#define PIOA_REG(_x_)	*(vulong *)(PIOA_BASE + _x_)

// Port B
#define PIOB_BASE		0xFFFFF400
#define PIOB_REG(_x_)	*(vulong *)(PIOB_BASE + _x_)

// Port C
#define PIOC_BASE		0xFFFFF600
#define PIOC_REG(_x_)	*(vulong *)(PIOC_BASE + _x_)

// Port D
#define PIOD_BASE		0xFFFFF800
#define PIOD_REG(_x_)	*(vulong *)(PIOD_BASE + _x_)

// Port E
#define PIOE_BASE		0xFFFFFA00
#define PIOE_REG(_x_)	*(vulong *)(PIOE_BASE + _x_)

// *****************************************************************************
// Power Management and Clock Control
// *****************************************************************************
#define PMC_BASE		0xFFFFFC00
#define PMC_REG(_x_)	*(vulong *)(PMC_BASE + _x_)

// *****************************************************************************
// MAC Unit
// *****************************************************************************
#define EMAC_BASE		0xFFFBC000
#define EMAC_REG(_x_)	*(vulong *)(EMAC_BASE + _x_)

// *****************************************************************************
// Chip Configuration User Interface - Register and Bit defines in at91sam9263_mem.h
// *****************************************************************************
#define CCFG_BASE	  	0xFFFFED10
#define CCFG_REG(_x_) 	*(vulong *)(CCFG_BASE + _x_)

// *****************************************************************************
// Static Memory Interface Unit - Register and Bit defines in at91sam9263_mem.h
// *****************************************************************************
#define SMC0_BASE	  	0xFFFFE400
#define SMC0_REG(_x_) 	*(vulong *)(SMC0_BASE + _x_)

#define SMC1_BASE	  	0xFFFFEA00
#define SMC1_REG(_x_) 	*(vulong *)(SMC1_BASE + _x_)

// *****************************************************************************
// SDRAM Memory Interface Unit - Register and Bit defines in at91sam9263_mem.h
// *****************************************************************************
#define SDRC0_BASE	  	0xFFFFE200
#define SDRC0_REG(_x_)	*(vulong *)(SDRC0_BASE + _x_)

#define SDRC1_BASE	  	0xFFFFE800
#define SDRC1_REG(_x_)	*(vulong *)(SDRC1_BASE + _x_)

// *****************************************************************************
// Watchdog Timer User Interface
// *****************************************************************************
#define WDT_BASE		0xFFFFFD40
#define WDT_REG(_x_)	*(vulong *)(WDT_BASE + _x_)

// Register Offsets
#define WDT_CR	  	0x00	// Control Register
#define WDT_MR 		0x04	// Mode Register
#define WDT_SR	 	0x08	// Interrupt Enable Register

#endif

