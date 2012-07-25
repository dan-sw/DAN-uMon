/*
All files except if stated otherwise in the begining of the file are under the GPLv2 license:
-----------------------------------------------------------------------------------

Copyright (c) 2010-2012 Design Art Networks Ltd.

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
 Copyright	:  Design Art Networks (c) 2010
 Library    :  uMON (Micro Monitor)
 Filename   :  spi.h
 Purpose    :  Simple SPI driver for uMON only.
 Owner		:  Albert Yosher
 Created    :  18.08.2010
\**********************************************************************/

#ifndef _SPI_H
#define _SPI_H


typedef unsigned (*SPI_chksum_cb) (unsigned state, uint8 data);

/*
 * -----------------------------------------------------------
 * MACRO (define) section
 * -----------------------------------------------------------
 */
 
#ifdef DAN2400
#define DAN2400_SPI 
#endif
 
#ifdef DAN2400_SPI
#define SSI_BASE			   0x20001C00
#else
#define SSI_BASE			   0xE5770000
#endif

#define Ssi_Ctrlr0_Offset      0x00
#define Ssi_Ctrlr1_Offset      0x04
#define Ssi_Ssienr_Offset      0x08
#define Ssi_Mwcr_Offset        0x0C
#define Ssi_Ser_Offset         0x10
#define Ssi_Baudr_Offset       0x14
#define Ssi_Txftlr_Offset      0x18
#define Ssi_Rxftlr_Offset      0x1C
#define Ssi_Txflr_Offset       0x20
#define Ssi_Rxflr_Offset       0x24
#define Ssi_Sr_Offset          0x28
#define Ssi_Imr_Offset         0x2C
#define Ssi_Isr_Offset         0x30
#define Ssi_Risr_Offset        0x34
#define Ssi_Txoicr_Offset      0x38
#define Ssi_Rxoicr_Offset      0x3C
#define Ssi_Rxuicr_Offset      0x40
#define Ssi_Msticr_Offset      0x44
#define Ssi_Icr_Offset         0x48
#define Ssi_Dmacr_Offset       0x4C
#define Ssi_Dmatdlr_Offset     0x50
#define Ssi_Dmardlr_Offset     0x54
#define Ssi_Idr_Offset         0x58
#define Ssi_Version_Id_Offset  0x5C
#define Ssi_Dr_Low_Offset      0x60
#define Ssi_Dr_High_Offset     0x9C

/* CTRL Register #0*/
#define    SPI_SPI             0x0000
#define    SPI_CFS             0x0000 /* valid only for microwire*/
#define    SPI_SRL             0x0000 /* debug purposes - shift register loop*/
#define    SPI_TMODE           0x0000 /* transmit & receive mode*/
#define    SPI_SCPH            0x0040 /* begin sampling on first clock edge*/
#define    SPI_DFS             0x0007
#define    SPI_CLK_POL         0x0080 /* high polatiry*/

/*  TXFLTR Register*/
#define SPI_TX_FIFO_THRESHOLD  0x0002 /* 4 entries on tx FIFI will trigger an interrupt*/

/*  RXFLTR Register*/
#define SPI_RX_FIFO_THRESHOLD  0x0002 /* 4 entries on rx FIFI will trigger an interrupt*/

#define SPI_WRITE_REG(offset, val)											\
    (*(volatile unsigned *)(SSI_BASE + (offset))) = (val)

#define SPI_READ_REG(offset, val)											\
    (val) = (*(volatile unsigned *)(SSI_BASE + (offset)))

#define SPI_EN_CFG															\
								SPI_CFS | SPI_SRL | SPI_TMODE | SPI_SPI |	\
								SPI_DFS | SPI_SCPH | SPI_CLK_POL

#define SPI_SPEED				12000000	//14000000

#if UMON_TARGET == UMON_TARGET_ARM && !defined(USE_NPU_CB_ONLY)
// KS: Fix added by Albert to overcome ARM's slow-running problem
// I moved it to this define from a global var
#define SPI_SPEED_WITHFIX 	(SPI_SPEED/5)
#else
#define SPI_SPEED_WITHFIX 	SPI_SPEED
#endif

#define SPI_BOOT_DEVICE			0


/*
 * -----------------------------------------------------------
 * Global prototypes section
 * -----------------------------------------------------------
 */

extern unsigned SPI_divider;


// Changes SPI divider (change the SPI baud rate)
static inline void SPI_set_speed(unsigned speed)
{
	// Calculate SPI divider as even number of:
	// BUS clock / desired speed (BUS clock = CPU clock / 2)
	// The minimal possible divider is different for ARM and CPU
	SPI_divider = MAX((CGEN_PLL0_get_clk(INPUT_CLOCK_HZ) / speed / 2) & (~1), SPI_MIN_DIVIDER);
}

static inline void SPI_enable(uint8 device)
{
	/* configure the SPI master to the specific slave expected settings*/
	SPI_WRITE_REG (Ssi_Ctrlr0_Offset, SPI_EN_CFG);
	SPI_WRITE_REG (Ssi_Baudr_Offset, SPI_divider);
	SPI_WRITE_REG (Ssi_Ser_Offset, 1 << device);
	SPI_WRITE_REG (Ssi_Ssienr_Offset, 0x01); /* enable spi */
}

static inline void SPI_disable()
{
     SPI_WRITE_REG (Ssi_Ser_Offset, 0x00); /* de-select slave*/
     SPI_WRITE_REG (Ssi_Ssienr_Offset, 0x00); /* disable spi */
}

static inline void SPI_init()
{
	SPI_disable();

	SPI_WRITE_REG (Ssi_Imr_Offset, 0x3F);
	SPI_WRITE_REG (Ssi_Ctrlr0_Offset, SPI_DFS);
	SPI_WRITE_REG (Ssi_Txftlr_Offset, SPI_TX_FIFO_THRESHOLD);
	SPI_WRITE_REG (Ssi_Rxftlr_Offset, SPI_RX_FIFO_THRESHOLD);

	SPI_set_speed(SPI_SPEED);
}


// Perform SPI transaction, maximum transaction length is SPI_FIFO_DEPTH (133)
void SPI_process 
	(
		uint8		bank,
		unsigned	len_in, 
		unsigned	len_out1,
		unsigned	len_out2, 
		uint8		*inbuf, 
		uint8		*outbuf1,
		uint8		*outbuf2,
		unsigned	in_skip_start, 
		unsigned	in_skip_end
	);


#endif // _SPI_H
