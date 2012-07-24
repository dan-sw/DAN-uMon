//=============================================================================
//
//      csb737_init2.c
//		This file used to be the hw_init2.c file used with the CSB737 port.
//
// Author(s):    Michael Kelly, Cogent Computer Systems, Inc.
// 				 Luis Torrico, Cogent Computer Systems, Inc.	
// Date:         05-26-2002
// Modified:	 12-14-2007
// Description:  This file contains the 2nd stage initialization called
//				 at boot time.  It assumes the stack has been setup to
//			     point to the internal SRAM.
//
//=============================================================================

#include "stddefs.h"
#include "at91sam9263.h"
#include "at91sam9263_mem.h"
#include "at91sam9263_pmc.h"
#include "at91sam9263_gpio.h"
#include "cpuio.h"

// Debug macro using CS2 space...

//#define INIT_DBG(a)	(*(vulong *)0x00310000) = a
#define INIT_DBG(a)

#define SDRAM(_x_)	*(vulong *)(0x20000000 + _x_)	// SDRAM base address

void
csb737_init2(void)
{
	volatile int i;

	//*******************************************************************
	// Clock and CS0 Initialization
	//*******************************************************************
	INIT_DBG(1);

	// turn on the main oscillator and wait 50ms (~400 
	// slow clocks).
	//PMC_REG(PMC_MOR) = PMC_MOR_OSCNT(0x40) | PMC_MOR_MOSCEN;
	//for (i = 0; i < 1000; i++) {
	//	if (PMC_REG(PMC_SR) & PMC_INT_MOSCS) break;
	//}

	// enable PLLA for 198.656Mhz
	// [18.432 Main / DIVA x MULA + 1] = 18.432 Main/9 x 97
	// MULA = 96, OUTA = 0x10 (190-240MHz), PLLACOUNT = 63, DIVA = 9.
	PMC_REG(PMC_PLLAR) = 0x2060BF09;

	// wait for PLLA lock bit
	for (i = 0; i < 10000; i++) {
		if (PMC_REG(PMC_SR) & PMC_INT_LOCKA) break;
	}

	INIT_DBG(2);

	// Enable the clocks to all the on-chip peripherals
	PMC_REG(PMC_PCER) = 0x2d2fff9f;
//	PMC_REG(PMC_PCDR) = 0xf7fffc63;

	INIT_DBG(3);

	// set CS0 to Setup and Pulse lengths of 1,
	// 16-bits, 3 Cycle clocks
	// address and strobe delay
	SMC0_REG(SMC_CYCLE0) = 0x002f002f;
	SMC0_REG(SMC_MODE0)	 = 0x00041003;
	SMC0_REG(SMC_SETUP0) = 0x05050505;
	SMC0_REG(SMC_MODE0)	 = 0x00041003;
	SMC0_REG(SMC_PULSE0) = 0x0f0f0f0f;
	SMC0_REG(SMC_MODE0)	 = 0x00041003;

	INIT_DBG(4);


	INIT_DBG(5);

	// switch MCK to PLLA / 2
	PMC_REG(PMC_MCKR) = PMC_MCKR_CSS_PLLA | PMC_MCKR_MDIV_2;
	// wait for MCK ready bit
//	for (i = 0; i < 100; i++){
//		if (PMC_REG(PMC_SR) & 0x00000008) break;
//		if (PMC_REG(PMC_SR) & PMC_INT_MCK_RDY) break;
//		if (PMC_REG(PMC_SR) & PMC_INT_LOCKA) break;
//	}

	INIT_DBG(6);

	// Enable needed GPIO's in Ports A-E
//	PIOB_REG(PIO_OER) = 0x20000000; 	// PIO_OER: all GPIO=in, except PB29 (LCD_BKL)
	//PIOB_REG(PIO_SODR) = 0x20000000;	// PIO_SODR: first drive PB29 high
//	PIOB_REG(PIO_CODR) = 0x20000000;	// PIO_CODR: then drive PB29 (LCD_BKL) low
	
//	PIOD_REG(PIO_OER) = 0x00000010; 	// PIO_OER: all GPIO=in, except PD4 (USR_LED)
//	PIOD_REG(PIO_SODR) = 0x00000010;	// PIO_SODR: first drive PD4 (USR_LED) high
//	PIOD_REG(PIO_CODR) = 0x00000010;	// PIO_CODR: then drive PD4 (USR_LED) low


	INIT_DBG(7);

	// do not intialize the SDRAM if it is already running
	// (such as when we are loaded into ram via JTAG)
	if ((CCFG_REG(CCFG_EBI0_CSA) & CCFG_EBI0_CSA_CS1_SDRAM) == 0)	
	{
		// assign sdram_cs to cs1, all others to  sram
		// disable databus D0-D15 pullups and bus sharing
		// Set Memories are 3.3 volts
		CCFG_REG(CCFG_EBI0_CSA) = CCFG_EBI0_CSA_CS1_SDRAM
								| CCFG_EBI0_CSA_DBPU
								| CCFG_EBI0_CSA_MVS;
	
	 }

	INIT_DBG(8);

	//***********************************************************************
	// GPIO Initialization
	//***********************************************************************

	// Initialize the GPIO - just the basics for now

	// Enable EBI0 D16-31 on Port D to be alternate function A (databus)
	// Also enable *Wait (PD6A) A23 (PD12A) A24 (PD13A) and A25 (PD14A)
	PIOD_REG(PIO_PDR) = 0xfffffb63;
	PIOD_REG(PIO_PER) = 0x0000043c;
	PIOD_REG(PIO_ASR) = 0xfffffb03;
	PIOD_REG(PIO_ODR) = 0xfffff408;
	PIOD_REG(PIO_PUER) = 0xffffffff;

	// Enable LCD and Debug UART on Port C alternate function A.
	// Enable Ehternet CRSDV and LCD on Port C alternate function B.
//	PIOC_REG(PIO_PDR) = 0xc20fffff;
//	PIOC_REG(PIO_PER) = 0x3df00000;
//	PIOC_REG(PIO_ASR) = 0xc000000f;
//	PIOC_REG(PIO_BSR) = 0x020ffff0;
//	PIOC_REG(PIO_OER) = 0x00000000;
//	PIOC_REG(PIO_SODR) = 0x00000000;
//	PIOC_REG(PIO_CODR) = 0x00000000;

	// Enable Ethernet on Port E alternate function A
//	PIOE_REG(PIO_PDR) = 0x7faf3fff;
//	PIOE_REG(PIO_PER) = 0x8050c000;
//	PIOE_REG(PIO_ASR) = 0x7fa00fff;
//	PIOE_REG(PIO_BSR) = 0x00003000;
//	PIOE_REG(PIO_OER) = 0x00000000;
//	PIOE_REG(PIO_SODR) = 0x00000000;
//	PIOE_REG(PIO_CODR) = 0x00000000;

	// Enable  LED (GPIO 0) in Ports D to make sure we got to this point
//	PIOD_REG(PIO_OER) = 0x00000010; 	// PIO_OER: all GPIO=in, except PD4 (USR_LED)
//	PIOD_REG(PIO_SODR) = 0x00000010;	// PIO_SODR: first drive PD4 (USR_LED) high
//	PIOD_REG(PIO_CODR) = 0x00000010;	// PIO_CODR: then drive PD4 (USR_LED) low

	INIT_DBG(9);
	
	//***********************************************************************
	// SDRAM Initialization
	//***********************************************************************

		// write sdram configuration register - values assume
		// a minimum SDRAM rating of 100Mhz, CL2.
//		SDRC0_REG(SDRC_CR) = SDRC_CR_TXSR(10)		// CKE to ACT 
//		  | SDRC_CR_TRAS(6)		// ACT to PRE Time
//		  | SDRC_CR_TRCD(3)		// RAS to CAS Time
//		  | SDRC_CR_TRP(3)		// PRE to ACT Time
//		  | SDRC_CR_TRC(9)		// REF to ACT Time
//		  | SDRC_CR_TWR(2)		// Write Recovery Time
//		  | SDRC_CR_CAS_3		// Cas Delay = 3
//		  | SDRC_CR_NB_4		// 4 Banks per device
//		  | SDRC_CR_NR_13		// Number of rows = 13
//		  | SDRC_CR_NC_9;		// Number of columns = 9

		SDRC0_REG(SDRC_CR) = 0xa6339279;		// Settings from KCI 

		INIT_DBG(10);

		// issue 2 nop's
		SDRC0_REG(SDRC_MR) = SDRC_MR_NOP;
		SDRAM(0) = 0;
		SDRAM(0) = 0;
		for (i = 0; i < 100; i++){}		// delay for a bit

		// issue precharge all
		SDRC0_REG(SDRC_MR) = SDRC_MR_PRE;
		SDRAM(0) = 0;
		for (i = 0; i < 100; i++){}		// delay for a bit

		// issue 8 refresh cycles
		SDRC0_REG(SDRC_MR) = SDRC_MR_REF;
		for (i = 0; i < 8; i++){
			SDRAM(0) = 0;
		}
		for (i = 0; i < 100; i++){}		// delay for a bit

		// issue mode register set
		SDRC0_REG(SDRC_MR) = SDRC_MR_MRS;
		SDRAM(0x80) = 0;

		INIT_DBG(11);

		// set normal mode
		SDRC0_REG(SDRC_MR) = SDRC_MR_NORM;
		SDRAM(0) = 0;


		// set refresh to 14usec 
		SDRC0_REG(SDRC_TR) = 0x200;
		SDRAM(0) = 0;


//	} // if CCFG_EBI0_CSA_CS1_SDRAM == 0

	return;
}


