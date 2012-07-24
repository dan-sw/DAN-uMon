//==========================================================================
//
// at91sam9263_emac.h
//
// Author(s):    Luis Torrico, Cogent Computer Systems, Inc.
// Contributors: 
// Date:         12/19/2007
// Description:  This file contains the register offsets and bit
//				 defines for the AT91SAM9263 LCD Controller
//

// *****************************************************************************
// LCD Controller User Interface
// *****************************************************************************

#ifndef AT91SAM9263_LCD_H
#define AT91SAM9263_LCD_H

#include "bits.h"

#define LCDC_BASE			0x00700000
#define LCDC_REG(_x_)		*(vulong *)(LCDC_BASE + _x_)

//Register offsets
#define LCDC_DMABASE1 				0x00 		// DMA Base Address Register 1
#define LCDC_DMABASE2 				0x04 		// DMA Base Address Register 2
#define LCDC_DMAFRMP1 				0x08 		// DMA Frame Pointer Register 1
#define LCDC_DMAFRMP2 				0x0C 		// DMA Frame Pointer Register 2
#define LCDC_DMAFRMA1 				0x10 		// DMA Frame Address Register 1
#define LCDC_DMAFRMA2 				0x14 		// DMA Frame Address Register 2
#define LCDC_DMAFRMC 				0x18 		// DMA Frame Configuration Register
#define LCDC_DMACTL 				0x1C 		// DMA Control Register
#define LCDC_DMA2DA  				0x20 		// DMA 2D Addressing Register
#define LCDC_LCDCTL1  				0x800 		// LCD Control Register 1
#define LCDC_LCDCTL2  				0x804 		// LCD Control Register 2
#define LCDC_LCDTM1  				0x808 		// LCD Timing Register 1
#define LCDC_LCDTM2  				0x80C 		// LCD Timing Register 2
#define LCDC_LCDFRMC  				0x810 		// LCD Frame Configuration Register
#define LCDC_LCDFIFO  				0x814 		// LCD FIFO Register
#define LCDC_DITHP1_2  				0x81C 		// Dithering Pattern DP1_2
#define LCDC_DITHP4_7 				0x820 		// Dithering Pattern DP4_7
#define LCDC_DITHP3_5 				0x824 		// Dithering Pattern DP3_5
#define LCDC_DITHP2_3  				0x828 		// Dithering Pattern DP2_3
#define LCDC_DITHP5_7 				0x82C 		// Dithering Pattern DP5_7
#define LCDC_DITHP3_4  				0x830 		// Dithering Pattern DP3_4
#define LCDC_DITHP4_5  				0x834 		// Dithering Pattern DP4_5
#define LCDC_DITHP6_7 				0x838 		// Dithering Pattern DP6_7
#define LCDC_PWRC	 				0x83C 		// Power Control Register
#define LCDC_CONTC  				0x840 		// Contrast Control Register
#define LCDC_CONTV  				0x844 		// Contrast Value Register
#define LCDC_LCDINTE  				0x848 		// LCD Interrupt Enable Register
#define LCDC_LCDINTD  				0x84C 		// LCD Interrupt Disable Register
#define LCDC_LCDINTM  				0x850 		// LCD Interrupt Mask Register
#define LCDC_LCDINTS  				0x854 		// LCD Interrupt Status Register
#define LCDC_LCDINTC  				0x858 		// LCD Interrupt Clear Register
#define LCDC_LCDINTT  				0x860 		// LCD Interrupt Test Register
#define LCDC_LCDINTRS 				0x864 		// LCD Interrupt Raw Status Register

// DMA Frame Configuration Register, LCDC_DMAFRMC, Offset 0x1C
#define LCDC_DMAFRMC_BRSTLN(_x_)   	((_x_ & 0x7F) <<  24) 		// Burst Length, Program with the desired burst length - 1
#define LCDC_DMAFRMC_FRMSIZE(_x_)  	((_x_ & 0x7FFFFF) <<  0)	// Frame Size, In single scan mode, this is the frame size in words

// DMA Control Register, LCDC_DMACTL, Offset 0x1C
#define LCDC_DMACTL_EN	         	BIT0 		// 1 = DMA enabled
#define LCDC_DMACTL_RST	        	BIT1 		// 1 = Reset DMA module (Write-only) 
#define LCDC_DMACTL_BSY	        	BIT2 		// 1 = DMA module is busy 
#define LCDC_DMACTL_UPD	        	BIT3 		// 1 = Update DMA Configuration (refer to datasheet for description) 
#define LCDC_DMACTL_2DEN	       	BIT4 		// 1 = 2D addressing enabled. 

// LCD Control Register 1, LCDC_LCDCTL1, Offset 0x800
#define LCDC_LCDCTL1_BYPASS         BIT0 						// 1 = LCDDOTCK divider is bypassed
#define LCDC_LCDCTL1_CLKVAL(_x_)   	((_x_ & 0x1FF) <<  12) 		// Pixel Clock Divider, Pixel Clock = SYS_CLK/ (CLKVAL +1)*2 

// LCD Control Register 2, LCDC_LCDCTL2, Offset 0x804
#define LCDC_LCDCTL2_TYPE_MONO		(0 << 0) 	// Display Type is STN Monochrome
#define LCDC_LCDCTL2_TYPE_CLR		(1 << 0) 	// Display Type is STN Color
#define LCDC_LCDCTL2_TYPE_TFT		(2 << 0) 	// Display Type is TFT
#define LCDC_LCDCTL2_DUALSCAN		BIT2	 	// 1 = Dual Scan, 0 = Single Scan
#define LCDC_LCDCTL2_WIDTH4			(0 << 3) 	// Interface Width is 4-bit
#define LCDC_LCDCTL2_WIDTH8			(1 << 3) 	// Interface Width is 8-bit
#define LCDC_LCDCTL2_WIDTH16		(2 << 3) 	// Interface Width is 16-bit
#define LCDC_LCDCTL2_PIXSZ1			(0 << 5) 	// Bits per Pixel = 1
#define LCDC_LCDCTL2_PIXSZ2			(1 << 5) 	// Bits per Pixel = 2
#define LCDC_LCDCTL2_PIXSZ4			(2 << 5) 	// Bits per Pixel = 4
#define LCDC_LCDCTL2_PIXSZ8			(3 << 5) 	// Bits per Pixel = 8
#define LCDC_LCDCTL2_PIXSZ16		(4 << 5) 	// Bits per Pixel = 16
#define LCDC_LCDCTL2_PIXSZ24P		(5 << 5) 	// Bits per Pixel = 24 packed
#define LCDC_LCDCTL2_PIXSZ24		(6 << 5) 	// Bits per Pixel = 24 unpacked
#define LCDC_LCDCTL2_INVPOL			BIT8	 	// 1 = LCDD polarity inverted
#define LCDC_LCDCTL2_INVFRM			BIT9	 	// 1 = LCDVSYNC polarity inverted
#define LCDC_LCDCTL2_INVLINE		BIT10	 	// 1 = LCDHSYNC polarity inverted
#define LCDC_LCDCTL2_INVCLK			BIT11	 	// 1 = LCDDOTCLK polarity inverted
#define LCDC_LCDCTL2_INVDVAL		BIT12	 	// 1 = LCDEN polarity inverted
#define LCDC_LCDCTL2_CLK_ON			BIT15	 	// 1 = LCDDOTCLK always active
#define LCDC_LCDCTL2_MEMORBE		(0 << 30) 	// Memory Ordering Format = Big Endian
#define LCDC_LCDCTL2_MEMORLE		(2 << 30) 	// Memory Ordering Format = Little Endian
#define LCDC_LCDCTL2_MEMORCE		(3 << 30) 	// Memory Ordering Format = WinCE Format

// LCD Timing Configuration Register 1, LCDC_LCDTM1, Offset 0x808
#define LCDC_LCDTM1_VFP(_x_)  		((_x_ & 0xFF) <<  0) 	// Vertical Front Porch
#define LCDC_LCDTM1_VBP(_x_)  		((_x_ & 0xFF) <<  8) 	// Vertical Back Porch
#define LCDC_LCDTM1_VPW(_x_)  		((_x_ & 0x3F) <<  16) 	// Vertical Synchronization Pulse Width (VPW +1)
#define LCDC_LCDTM1_VHDLY(_x_)		((_x_ & 0xF) <<  24) 	// Vertical to Horizontal Delay (VHDLY + 1)

// LCD Timing Configuration Register 2, LCDC_LCDTM2, Offset 0x80C
#define LCDC_LCDTM2_HBP(_x_)	 	((_x_ & 0xFF) <<  0) 	// Horizontal Back Porch
#define LCDC_LCDTM2_HPW(_x_)	 	((_x_ & 0x3F) <<  8) 	// Horizontal Synchronization Pulse Width
#define LCDC_LCDTM2_HFP(_x_)	 	((_x_ & 0x7FF) <<  21) 	// Horizontal Front Porch

// LCD Frame Configuration Register, LCDC_LCDFRMC, Offset 0x810
#define LCDC_LCDFRMC_LINEVAL(_x_)	((_x_ & 0x7FF) <<  0) 	// Vertical Size of LCD (in pixels minus 1)
#define LCDC_LCDFRMC_LINESIZ(_x_)	((_x_ & 0x7FF) << 21) 	// Horizontal Size of LCD (in pixels minus 1)

// Power Control Register, LCDC_PWRC, Offset 0x83C
#define LCDC_PWRC_LCDPWRON			BIT0 				// 1 = LCD_PWR signal is high, other LCD signals are active
#define LCDC_PWRC_GUARDTM(_x_)	((_x_ & 0x7F) << 1) 	// Guard Time - Delay in frame periods


#endif // #ifndef AT91SAM9263_LCD_H







