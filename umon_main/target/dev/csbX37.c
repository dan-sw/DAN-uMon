//=============================================================================
//
//      csbX37_init2.c
//		This file used to be the hw_init2.c file used with the CSB337
//		and CSB637 ports.
//
// Author(s):    Michael Kelly, Cogent Computer Systems, Inc.
// Date:         05-26-2002
// Description:  This file contains the 2nd stage initialization called
//				 at boot time.  It assumes the stack has been setup to
//			     point to the internal SRAM.
//
//=============================================================================

#include "stddefs.h"
#include "at91rm9200.h"
#include "at91rm9200_mem.h"
#include "at91rm9200_pmc.h"
#include "at91rm9200_gpio.h"
#include "cpuio.h"

// Debug macro using internal SRAM space...
#define INIT_DBG(a)
//#define INIT_DBG(a)	(*(vulong *)0x00200000) = a

#define SDRAM(_x_)	*(vulong *)(0x20000000 + _x_)	// SDRAM base address

void
csbX37_init2(void)
{
	volatile int i;

	//*******************************************************************
	// Clock and CS0 Initialization
	//*******************************************************************
	INIT_DBG(1);

	// switch to the slow clock unless we already are
	if (PMC_REG(PMC_MCKR)) {
		// clear PRES first
		PMC_REG(PMC_MCKR) = PMC_REG(PMC_MCKR) & ~PMC_MCKR_PRES_MASK;
		// then CSS
		PMC_REG(PMC_MCKR) = PMC_REG(PMC_MCKR) & ~PMC_MCKR_CSS_MASK;
	}
	for (i = 0; i < 100; i++);

	// turn on the main oscillator and wait 50ms (~400 
	// slow clocks).
	PMC_REG(PMC_MOR) = PMC_MOR_MOSCEN;
	for (i = 0; i < 100; i++);

	INIT_DBG(2);

	// enable PLLA for 184.32Mhz
	// 3.6864 Main/2 x 100
	PMC_REG(PMC_PLLAR) = 0x20633E02;

	// wait for PLLA lock bit
	for (i = 0; i < 1000; i++) {
		if (PMC_REG(PMC_SR) & PMC_INT_LCKA) break;
	}

	INIT_DBG(3);

	// set CS0 to 8 wait states, 16-bits, 1 clock
	// address and strobe delay
	SMC_REG(SMC_CSR0) = 0x1100318a;

	INIT_DBG(4);

	// first switch MCK to slow clock
	PMC_REG(PMC_MCKR) = PMC_MCKR_CSS_SLOW;

	// wait for MCK ready bit
	for (i = 0; i < 100; i++){
//		if (PMC_REG(PMC_SR) & 0x00000008) break;
//		if (PMC_REG(PMC_SR) & PMC_INT_MCK_RDY) break;
	}

	INIT_DBG(5);

	// set MCK = core/3
	PMC_REG(PMC_MCKR) = 0x0302;

	// wait for MCK ready bit
	for (i = 0; i < 1000; i++){
//		if (PMC_REG(PMC_SR) & 0x00000008) break;
//		if (PMC_REG(PMC_SR) & PMC_INT_MCK_RDY) break;
	}

	INIT_DBG(6);

	// Enable system clocks, PCK0, MCK and core clock
	PMC_REG(PMC_SCER) = (PMC_SCR_PCK0	// Peripheral Clock 0
			     | PMC_SCR_UHP	// USB Host Port		
			     | PMC_SCR_UDP);	// USB Data Port

	INIT_DBG(7);

	// Enable the clocks to all the on-chip peripherals
	PMC_REG(PMC_PCER) = 0x01fffffc;
	for (i = 0; i < 1000; i++){}

	INIT_DBG(8);

	// set PCK0 to PLLA/4 - 45Mhz
	// It will be enabled in cpuio.c
	PMC_REG(PMC_PCKR0) = (PMC_PCKR_PRES_4	// divide by 4
			      | PMC_PCKR_CSS_PLLA);

	INIT_DBG(9);

	//***********************************************************************
	// GPIO Initialization
	//***********************************************************************

	// Initialize the GPIO - just the basics for now

	// enable d16-31 on portc to be alternate function a (databus)
	// also enable *wait (pc6a) a23 (pc7a) a24 (pc8a) and a25 (pc9a)
	PIOC_REG(PIO_ASR) = 0xffff01c0;
	PIOC_REG(PIO_PDR) = 0xffff01c0;
	PIOC_REG(PIO_PER) = ~(0xffff01c0);

	// pio_odr: all gpio = input
	PIOC_REG(PIO_ODR) = 0xffffffff;

	// enable ethernet and DTXD/DRXD on Port A
	// ethernet = PA16-7, DTXD = PA31
	PIOA_REG(PIO_ASR) = 0xC001FF80;		// PORTA Function A
	PIOA_REG(PIO_PDR) = 0xC001FF80;		// remove PA31, and PA16-7 as GPIO

	// PIO_PDR: PB27 = peripheral, PB19-12 = Ethernet, all else = GPIO
	PIOB_REG(PIO_PDR) = 0x080ff000;
	PIOB_REG(PIO_PER) = ~(0x080ff000);

	// PortB Function A, PB27 = PCK0
	PIOB_REG(PIO_ASR) = 0x08000000;		// PORTB Function A
	// PortB Function B, PB19-12 = Ethernet
	PIOB_REG(PIO_BSR) = 0x000ff000;		// PORTB Function B

	PIOB_REG(PIO_OER) = 0x00000007; 	// PIO_OER: all GPIO=in, except PB0-2
	PIOB_REG(PIO_SODR) = 0x00000007;	// PIO_SODR: first drive PB0-2 high
	PIOB_REG(PIO_CODR) = 0x00000004;	// PIO_CODR: then drive PB0 low

	INIT_DBG(10);
	
	//***********************************************************************
	// SDRAM Initialization
	//***********************************************************************

	// do not intialize the SDRAM if it is already running
	// (such as when we are loaded into ram via JTAG)
	if ((EBI_REG(EBI_CSA) & EBI_CSA_CS1_SDRAM) == 0)	
	{
		// assign sdram_cs to cs1, all others to  sram
		EBI_REG(EBI_CSA) = EBI_CSA_CS1_SDRAM;

		// disable databus D0-D15 pullups and bus sharing
		EBI_REG(EBI_CFGR) = 0;

		INIT_DBG(11);

		// write sdram configuration register - values assume
		// a minimum SDRAM rating of 100Mhz, CL2.
		SDRC_REG(SDRC_CR) = SDRC_CR_TXSR(4)		// CKE to ACT 
		  | SDRC_CR_TRAS(3)		// ACT to PRE Time
		  | SDRC_CR_TRCD(1)		// RAS to CAS Time
		  | SDRC_CR_TRP(1)		// PRE to ACT Time
		  | SDRC_CR_TRC(6)		// REF to ACT Time
		  | SDRC_CR_TWR(1)		// Write Recovery Time
		  | SDRC_CR_CAS_2		// Cas Delay = 2
		  | SDRC_CR_NB_4		// 4 Banks per device
#if (PLATFORM_CSB437 | PLATFORM_CSB637)
		  | SDRC_CR_NR_13		// Number of rows = 13
#else
		  | SDRC_CR_NR_12		// Number of rows = 12
#endif
#ifdef USE_DRAM_128
		  | SDRC_CR_NC_10;		// Number of columns = 9
#else
		  | SDRC_CR_NC_9;		// Number of columns = 9
#endif

		INIT_DBG(12);

		// issue 2 nop's
		SDRC_REG(SDRC_MR) = SDRC_MR_NOP;
		SDRAM(0) = 0;
		SDRAM(0) = 0;
		for (i = 0; i < 100; i++){}		// delay for a bit

		// issue precharge all
		SDRC_REG(SDRC_MR) = SDRC_MR_PRE;
		SDRAM(0) = 0;
		for (i = 0; i < 100; i++){}		// delay for a bit

		// issue 8 refresh cycles
		SDRC_REG(SDRC_MR) = SDRC_MR_REF;
		for (i = 0; i < 8; i++){
			SDRAM(0) = 0;
		}
		for (i = 0; i < 100; i++){}		// delay for a bit

		// issue mode register set
		SDRC_REG(SDRC_MR) = SDRC_MR_MRS;
		SDRAM(0x80) = 0;

		INIT_DBG(13);

		// set normal mode
		SDRC_REG(SDRC_MR) = SDRC_MR_NORM;
		SDRAM(0) = 0;

		INIT_DBG(14);

		// set refresh to 14usec 
		SDRC_REG(SDRC_TR) = 0x200;
		SDRAM(0) = 0;

		INIT_DBG(15);

	} // if EBI_CSA_CS1_SDRAM == 0

	return;
}


