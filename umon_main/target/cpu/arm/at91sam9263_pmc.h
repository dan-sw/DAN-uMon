//==========================================================================
//
// at91sam9263_pmc.h
//
// Author(s):    Michael Kelly, Cogent Computer Systems, Inc.
// Contributors: 
// Date:         7/24/2003
// Description:  This file contains the bit defines for the AT91SAM9263
//				 Power Management and Clock Section


#ifndef AT91SAM9263_PMC_H
#define AT91SAM9263_PMC_H

#include "bits.h"

// *****************************************************************************
// Power Management and Clock Control Register Offsets
// *****************************************************************************

#define PMC_SCER 	0x00	// System Clock Enable Register
#define PMC_SCDR 	0x04	// System Clock Disable Register
#define PMC_SCSR 	0x08	// System Clock Status Register
#define PMC_PCER 	0x10	// Peripheral Clock Enable Register
#define PMC_PCDR 	0x14	// Peripheral Clock Disable Register
#define PMC_PCSR 	0x18	// Peripheral Clock Status Register
#define PMC_MOR 	0x20	// Main Oscillator Register
#define PMC_MCFR 	0x24	// Main Clock  Frequency Register
#define PMC_PLLAR 	0x28	// PLL A Register
#define PMC_PLLBR 	0x2C	// PLL B Register
#define PMC_MCKR 	0x30	// Master Clock Register
#define PMC_PCKR0 	0x40	// Programmable Clock Register 0
#define PMC_PCKR1 	0x44	// Programmable Clock Register 1
#define PMC_PCKR2 	0x48	// Programmable Clock Register 2
#define PMC_PCKR3 	0x4C	// Programmable Clock Register 3
#define PMC_PCKR4 	0x50	// Programmable Clock Register 4
#define PMC_PCKR5 	0x54	// Programmable Clock Register 5
#define PMC_PCKR6 	0x58	// Programmable Clock Register 6
#define PMC_PCKR7 	0x5C	// Programmable Clock Register 7
#define PMC_IER 	0x60	// Interrupt Enable Register
#define PMC_IDR 	0x64	// Interrupt Disable Register
#define PMC_SR		0x68	// Status Register
#define PMC_IMR 	0x6C	// Interrupt Mask Register
#define PMC_CPR 	0x80	// Charge Pump Current Register

// Bit Defines

// PMC_SCER - System Clock Enable Register
// PMC_SCDR - System Clock Disable Register
// PMC_SCSR - System Clock Status Register
#define PMC_SCR_PCK7			BIT15
#define PMC_SCR_PCK6			BIT14
#define PMC_SCR_PCK5			BIT13
#define PMC_SCR_PCK4			BIT12
#define PMC_SCR_PCK3			BIT11
#define PMC_SCR_PCK2			BIT10
#define PMC_SCR_PCK1			BIT9
#define PMC_SCR_PCK0			BIT8
#define PMC_SCR_UDP 			BIT7
#define PMC_SCR_UHP				BIT6
#define PMC_SCR_PCK 			BIT0		// This bit is only used in PMC_SCDR & PMC_SCSR

// PMC_PCER - Peripheral Clock Enable Register
// PMC_PCDR - Peripheral Clock Disable Register
// PMC_PCSR - Peripheral Clock Status Register
#define PMC_PCR_PID_UHP			BIT29		// USB Host Port Peripheral Clock
#define PMC_PCR_PID_DMA			BIT27		// DMA Controller Peripheral Clock
#define PMC_PCR_PID_LCD			BIT26		// LCD Controller Peripheral Clock
#define PMC_PCR_PID_ISI			BIT25		// Image Sensor Interface Peripheral Clock
#define PMC_PCR_PID_UDP			BIT24		// USB Device Port Peripheral Clock
#define PMC_PCR_PID_2DGE		BIT23		// 2D Graphic Engine Peripheral Clock
#define PMC_PCR_PID_EMAC		BIT21		// Ethernet Peripheral Clock
#define PMC_PCR_PID_PWM			BIT20		// Pulse Width Modulation Controller Peripheral Clock
#define PMC_PCR_PID_TC			BIT19		// Timer/Counter 0, 1 and 2 Peripheral Clock
#define PMC_PCR_PID_AC97		BIT18		// AC97 Controller Peripheral Clock
#define PMC_PCR_PID_SSC1		BIT17		// Synchronous Serial 1 Peripheral Clock
#define PMC_PCR_PID_SSC0		BIT16		// Synchronous Serial 0 Peripheral Clock
#define PMC_PCR_PID_SPI1		BIT15		// Serial Peripheral Interface 1 Peripheral Clock
#define PMC_PCR_PID_SPI0		BIT14		// Serial Peripheral Interface 0 Peripheral Clock
#define PMC_PCR_PID_TWI			BIT13		// Two-Wire Interface Peripheral Clock
#define PMC_PCR_PID_CAN			BIT12		// CAN Controller Peripheral Clock
#define PMC_PCR_PID_MCI1		BIT11		// MMC/SD Card 1 Peripheral Clock
#define PMC_PCR_PID_MCI0		BIT10		// MMC/SD Card 0 Peripheral Clock
#define PMC_PCR_PID_US2			BIT9		// USART 2 Peripheral Clock
#define PMC_PCR_PID_US1			BIT8		// USART 1 Peripheral Clock
#define PMC_PCR_PID_US0			BIT7		// USART 0 Peripheral Clock
#define PMC_PCR_PID_PIOCDE		BIT4		// Parallel I/O C, D and E Peripheral Clock
#define PMC_PCR_PID_PIOB		BIT3		// Parallel I/O B Peripheral Clock
#define PMC_PCR_PID_PIOA		BIT2		// Parallel I/O A Peripheral Clock

// PMC_MOR - Main Oscillator Register
#define PMC_MOR_OSCBY			BIT1		// 1 = Main Oscillator Bypassed, PMC_MOR_MOSCEN must be set to 0. 
#define PMC_MOR_MOSCEN			BIT0		// 1 = Main Oscillator Enabled, PMC_MOR_OSCBY must be set to 0. 

// PMC_MCFR - Main Clock  Frequency Register
#define PMC_MCFR_MAINRDY		BIT16

// PMC_PLLAR - PLL A Register
#define PMC_PLLAR_MUST_SET		BIT29					// This bit must be set according to the docs
#define PMC_PLLAR_MUL(_x_)		((_x_ & 0x7ff) << 16)	// Multiplier	
#define PMC_PLLAR_OUT_80_200	(0 << 14)				// select when PLL frequency is 80-200 Mhz
#define PMC_PLLAR_OUT_190_240	(2 << 14)				// select when PLL frequency is 190-240 Mhz
#define PMC_PLLAR_DIV(_x_)		((_x_ & 0xff) << 0)		// Divider

// PMC_PLLBR - PLL B Register
#define PMC_PLLBR_USB_DIV1		(0 << 28)				// USB Clock = PLLB
#define PMC_PLLBR_USB_DIV2		(1 << 28)				// USB Clock = PLLB/2
#define PMC_PLLBR_USB_DIV4		(2 << 28)				// USB Clock = PLLB/4
#define PMC_PLLBR_MUL(_x_)		((_x_ & 0x7ff) << 16)	// Multiplier	
#define PMC_PLLBR_OUT_80_200	(0 << 14)				// select when PLL frequency is 80-200 Mhz
#define PMC_PLLBR_OUT_190_240	(2 << 14)				// select when PLL frequency is 190-240 Mhz
#define PMC_PLLBR_DIV(_x_)		((_x_ & 0xff) << 0)		// Divider

// PMC_MCKR - Master Clock Register
#define PMC_MCKR_MDIV_MASK		(3 << 8)				// for masking out the MDIV field
#define PMC_MCKR_MDIV_1			(0 << 8)				// MCK = Core/1
#define PMC_MCKR_MDIV_2			(1 << 8)				// MCK = Core/2
#define PMC_MCKR_MDIV_3			(2 << 8)				// MCK = Core/4
#define PMC_MCKR_PRES_MASK		(7 << 2)				// for masking out the PRES field
#define PMC_MCKR_PRES_1			(0 << 2)				// Core = CSS/1
#define PMC_MCKR_PRES_2			(1 << 2)				// Core = CSS/2
#define PMC_MCKR_PRES_4			(2 << 2)				// Core = CSS/4
#define PMC_MCKR_PRES_8			(3 << 2)				// Core = CSS/8
#define PMC_MCKR_PRES_16		(4 << 2)				// Core = CSS/16
#define PMC_MCKR_PRES_32		(5 << 2)				// Core = CSS/32
#define PMC_MCKR_PRES_64		(6 << 2)				// Core = CSS/64
#define PMC_MCKR_CSS_MASK		(3 << 0)				// for masking out the CSS field
#define PMC_MCKR_CSS_SLOW		(0 << 0)				// Core Source = Slow Clock
#define PMC_MCKR_CSS_MAIN		(1 << 0)				// Core Source = Main Oscillator
#define PMC_MCKR_CSS_PLLA		(2 << 0)				// Core Source = PLL A
#define PMC_MCKR_CSS_PLLB		(3 << 0)				// Core Source = PLL B

// PMC_PCKR0 - 7 - Programmable Clock Register 0
#define PMC_PCKR_PRES_1			(0 << 2)				// Peripheral Clock = CSS/1
#define PMC_PCKR_PRES_2			(1 << 2)				// Peripheral Clock = CSS/2
#define PMC_PCKR_PRES_4			(2 << 2)				// Peripheral Clock = CSS/4
#define PMC_PCKR_PRES_8			(3 << 2)				// Peripheral Clock = CSS/8
#define PMC_PCKR_PRES_16		(4 << 2)				// Peripheral Clock = CSS/16
#define PMC_PCKR_PRES_32		(5 << 2)				// Peripheral Clock = CSS/32
#define PMC_PCKR_PRES_64		(6 << 2)				// Peripheral Clock = CSS/64
#define PMC_PCKR_CSS_SLOW		(0 << 0)				// Peripheral Clock Source = Slow Clock
#define PMC_PCKR_CSS_MAIN		(1 << 0)				// Peripheral Clock Source = Main Oscillator
#define PMC_PCKR_CSS_PLLA		(2 << 0)				// Peripheral Clock Source = PLL A
#define PMC_PCKR_CSS_PLLB		(3 << 0)				// Peripheral Clock Source = PLL B

// PMC_IER - Interrupt Enable Register
// PMC_IDR - Interrupt Disable Register
// PMC_SR - Status Register
// PMC_IMR - Interrupt Mask Register
#define PMC_INT_PCK7_RDY		BIT15
#define PMC_INT_PCK6_RDY		BIT14
#define PMC_INT_PCK5_RDY		BIT13
#define PMC_INT_PCK4_RDY		BIT12
#define PMC_INT_PCK3_RDY		BIT11
#define PMC_INT_PCK2_RDY		BIT10
#define PMC_INT_PCK1_RDY		BIT9
#define PMC_INT_PCK0_RDY		BIT8
#define PMC_INT_MCK_RDY			BIT3
#define PMC_INT_LOCKB			BIT2
#define PMC_INT_LOCKA			BIT1
#define PMC_INT_MOSCS			BIT0


#endif

