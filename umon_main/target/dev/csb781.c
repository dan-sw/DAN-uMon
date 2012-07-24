//=============================================================================
//
//      csb781.c
//		This file used to be the hw_init2.c file used with the CSB781 port.
//
// Author(s):    Michael Kelly, Cogent Computer Systems, Inc.
// 				 Luis Torrico, Cogent Computer Systems, Inc.	
// Date:         03-18-2009
// Description:  This file contains the 2nd stage initialization called
//				 at boot time.  It assumes the stack has been setup to
//			     point to the internal SRAM.
//
//=============================================================================

#include "stddefs.h"
#include "mpc5121e.h"
#include "mpc5121e_psc.h"
#include "cpuio.h"

// Debug macro using CS2 space...

//#define INIT_DBG(a)	(*(vulong *)0x00310000) = a
#define INIT_DBG(a)


void
csb781_init2(void)
{
	//*******************************************************************
	// IMMRBAR Initialization
	//*******************************************************************
	INIT_DBG(1);

	// Update the IMMRBAR to the desired address.  The default address
	// for IMMRBAR is 0xFF400000.

	//DEFAULT_IMMR_REG(IMMRBAR) = 0xE0000000;

	//*******************************************************************
	// Clock Initialization
	//*******************************************************************
	INIT_DBG(2);

	CLOCK_REG(CLOCK_SCFR1) = 0x01005336;

	// System and Bus clocks are set up by the reset configuration words.
	// Here we are just enabling the peripheral clocks.
	CLOCK_REG(CLOCK_SCCR1) = 0xFFFFF400;
	CLOCK_REG(CLOCK_SCCR2) = 0xFFFC0000; 
	CLOCK_REG(CLOCK_CCCR) = 0x00000000;

	//*******************************************************************
	// Memory Controller Initialization
	//*******************************************************************
	INIT_DBG(3);

	// Set up the boot base addresses.
	//IMMR_REG(LPBAW) = 0xFC00FFFF;
	//IMMR_REG(LPBAW) = 0xFFF0FFFF;

	// Set up cs0 base addresses.
	//IMMR_REG(LPCS0AW) = 0xFC00FFFF;

	// Set up DDR SDRAM base address.
	IMMR_REG(SDRAMBAR) = 0x00000000;

	// Set up DDR SDRAM size to 256MB.  We only have 128MB on board but we need
	// to allow room for uMon memory test to work.
	IMMR_REG(SDRAMATT) = 0x0000001b;

	// Set up SRAM base address.
	IMMR_REG(SRAMBAR) = 0x30000000;

	// Set up NFC base address.
	//IMMR_REG(NFCBAR) = 0x40000000;


	//*******************************************************************
	// CS0 Initialization
	//*******************************************************************
	INIT_DBG(4);

	// Set up CS0 for 5 wait states for read/write, Muxed,
	// External CS enabled, 2 Bytes(16 bit) 
	//LPC_REG(CS0_CONFIG) = 0x05059110;

	// Set the ME bit in the control register.  This enables
	// master mode.
	//LPC_REG(CS_CTRL) = 0x01000000;

	//*******************************************************************
	// DRAM Module Initialization
	//*******************************************************************
	INIT_DBG(5);

	// Enable DRAM module
	DRAM_REG(DDR_SYS_CONFIG) = 0xF0000000;

	// Setup DDR pads for mobile DDR mode, class 2
	IO_CTRL_REG(IOCTL_MEM) = 0x01000000;

	INIT_DBG(6);

	// Set DDR_SYS_CONFIG to CMDmode=1, row=4, bank=3, SelfRefEn=0,
	// (rdly=4, 1/2=1, 1/4=1) -> CAS=3, wdly=1
	DRAM_REG(DDR_SYS_CONFIG) = 0xF0E00100;

	// Set DDR_TIME_CONFIG0 to refresh=0 clks, cmd=40, bank_pre=30
	DRAM_REG(DDR_TIME_CONFIG0) = 0x0000281E;

	// Set DDR_TIME_CONFIG1 to rfc=13, wr1=5, wrt1=4 ,rrd=2, rc=10, ras=6
	DRAM_REG(DDR_TIME_CONFIG1) = 0x34A81146;

	// Set DDR_TIME_CONFIG2 to rcd=3, faw=5, rtw1=7, ccd=2, rtp=2, rp=3, rpa=4
	DRAM_REG(DDR_TIME_CONFIG2) = 0x32B90864;

	INIT_DBG(7);

	// Issue 10x NOP
	DRAM_REG(DDR_COMMAND) = 0x01380000;
	DRAM_REG(DDR_COMMAND) = 0x01380000;
	DRAM_REG(DDR_COMMAND) = 0x01380000;
	DRAM_REG(DDR_COMMAND) = 0x01380000;
	DRAM_REG(DDR_COMMAND) = 0x01380000;
	DRAM_REG(DDR_COMMAND) = 0x01380000;
	DRAM_REG(DDR_COMMAND) = 0x01380000;
	DRAM_REG(DDR_COMMAND) = 0x01380000;
	DRAM_REG(DDR_COMMAND) = 0x01380000;
	DRAM_REG(DDR_COMMAND) = 0x01380000;

	INIT_DBG(8);

	// PRECHARGE_ALL
	DRAM_REG(DDR_COMMAND) = 0x01100400;

	// Issue NOP operation for tRP time
	DRAM_REG(DDR_COMMAND) = 0x01380000;

	// Issue AUTO REFRESH
	DRAM_REG(DDR_COMMAND) = 0x01080000;

	// Issue NOP operation for tRP time
	DRAM_REG(DDR_COMMAND) = 0x01380000;

	// Issue AUTO REFRESH
	DRAM_REG(DDR_COMMAND) = 0x01080000;

	// Issue NOP operation for tRP time
	DRAM_REG(DDR_COMMAND) = 0x01380000;

	// Use LOAD MODE REGISTER to load the standard mode
	// register, CAS=3, BT=Seq, Burst=4
	DRAM_REG(DDR_COMMAND) = 0x01000032;

	// Issue NOP operation for tRP time
	DRAM_REG(DDR_COMMAND) = 0x01380000;

	// Use LOAD MODE REGISTER to load the standard mode
	// register, CAS=3, BT=Seq, Burst=4
	DRAM_REG(DDR_COMMAND) = 0x01020020;

	// Issue NOP operation for tRP time
	DRAM_REG(DDR_COMMAND) = 0x01380000;

	//*******************************************************************
	// Start MDDRC
	//*******************************************************************
	INIT_DBG(9);

	// Set DDR_TIME_CONFIG0 to refresh=1029 clks, cmd=40, bank_pre=30
	DRAM_REG(DDR_TIME_CONFIG0) = 0x0405281e;

	// Remove commmand mode
	DRAM_REG(DDR_SYS_CONFIG) = 0xe8609100;

//	// delay for a bit
//	for (i = 0; i < 10; i++){}		

	return;
}


