/***********************************************************************
 *
 * Purpose:
 *    BAT register setup example with fictional memory map
 *
 *  Assumptions:        
 *    This code is meant to show how to setup BAT registers.
 *    In order to achieve this purpose, we will
 *    assume that the machine we're writing code for has 3 memory 
 *    regions: RAM, ROM, and IO.
 *        
 *    This is just an arbitrary example and is not meant to represent any
 *    real hardware configuration.  These values should be replaced with the
 *    values for your board.
 *
 *    The procedure for setting up BAT registers is as follows:
 *      1) make sure address translation is disabled in the MMU
 *      2) isync
 *      3) invalidate the BAT by setting the upper BAT to all 0's
 *         (certain scenarios that are considered error states
 *          can be avoided by performing this operation)
 *      4) load the lower half of the BAT 
 *      5) load the upper half of the BAT 
 *      6) isync
 *      
 *    Once this process is complete, the MMU may be reenabled.  
 *
 *	I (Ed Sutter) downloaded this code from the Moto website to verify
 *	my understanding of the useage of the BAT registers.
 *	
 *	This configuration has been tested on the Galileo 64260 Eval board
 *	just to verify my understanding of the BAT registers.  With the
 *	configuration below, DRAM, TFS Flash and the UART are mapped 
 *	to the same space by BAT0-2 and BAT3 is used to map 0xe0200000 to
 *	DRAM space 0x200000.
 *	To test this, this I called the "setup_bats" function and then used
 *	the "pm" command to modify data at 0xe0200000.  Then I reset the board
 *	to disable MMU and BATS and what I wrote to 0xe0200000 was as location
 * 	0x200000; hence the remapping of physical 0x200000 to virtual 0xe0200000
 *	worked.
 */
#include "arch_ppc.h"

	.file "batsetup.s"
        
/*
 * RAM Values:  
 * start address 0x00000000 physical
 *               0x00000000 virtual
 * size 8M
 * instruction - read/write
 * data - read/write
 */
#define BAT0_BEPI		0x00000000
#define BAT0_BRPN		0x00000000
#define IBAT0L_VAL		(BAT0_BRPN | BAT_READ_WRITE)
#define IBAT0U_VAL		(BAT0_BEPI | BAT_BL_8M | BAT_VALID_SUPERVISOR | \
						 BAT_VALID_USER) 
#define DBAT0L_VAL		IBAT0L_VAL
#define DBAT0U_VAL		IBAT0U_VAL
        
/*
 * TFS Flash Values:  
 * start address 0x1a000000 physical
 *               0x1a000000 virtual
 * size 16M
 * instruction - read-only, cache inhibited
 * data - read-only, cache inhibited
 */      
#define BAT1_BEPI		0x1a000000
#define BAT1_BRPN		0x1a000000
#define IBAT1L_VAL		(BAT1_BRPN | BAT_CACHE_INHIBITED | BAT_READ_WRITE)
#define IBAT1U_VAL		(BAT1_BEPI | BAT_BL_16M | BAT_VALID_SUPERVISOR | \
					     BAT_VALID_USER)
#define DBAT1L_VAL		IBAT1L_VAL
#define DBAT1U_VAL		IBAT1U_VAL
        

/*
 * IO (UART) Values:
 * start address 0x1d000000 physical
 *               0x1d000000 virtual
 *
 * size 128K (small as possible)
 * instruction - not mapped (no instructions in IO space)
 * data - read/write, cache-inhibited, guarded
 */      

#define BAT2_BEPI		0x1d000000
#define BAT2_BRPN		0x1d000000                      
#define IBAT2L_VAL		(BAT_NO_ACCESS)
#define IBAT2U_VAL		(BAT_INVALID)
#define DBAT2L_VAL		(BAT2_BRPN | BAT_CACHE_INHIBITED | BAT_READ_WRITE)
#define DBAT2U_VAL		(BAT2_BEPI | BAT_VALID_SUPERVISOR | \
						 BAT_VALID_USER | BAT_BL_128K)                
/*
 * IO (TEST) Values:
 * start address 0x00200000 physical
 *               0xe0200000 virtual
 *
 * size 128K
 * instruction - not mapped 
 * data - read/write, cache-inhibited, guarded
 */      

#define BAT3_BEPI		0xe0200000
#define BAT3_BRPN		0x00200000                      
#define IBAT3L_VAL		(BAT_NO_ACCESS)
#define IBAT3U_VAL		(BAT_INVALID)
#define DBAT3L_VAL		(BAT3_BRPN | BAT_CACHE_INHIBITED | BAT_READ_WRITE)
#define DBAT3U_VAL		(BAT3_BEPI | BAT_VALID_SUPERVISOR | \
						 BAT_VALID_USER | BAT_BL_128K)                


/* setup_bats:
 * This routine programs the BAT registers.  This particular sample
 * assumes we're coming out of reset.
 * Since we're messing with the BAT registers, we first need to make
 * sure address translation is disabled in the MMU by clearing the IR and
 * DR bits.
 *
 * We will enable the MMU after the BAT setup is complete 
 */       
	.text

	.global setup_bats

setup_bats:             

	/* Disable MMU:
	 */
	mfmsr   r3
	addis   r4,0,0xffff
	ori     r4,r4,0xffcf
	and     r3,r3,r4
	mtmsr   r3

	/* Clear Upper word of each BAT register to disable BAT:
	 */
	isync

	addis   r5,0,0x0000
	mtspr   ibat0u,r5
	mtspr   ibat1u,r5
	mtspr   ibat2u,r5
	mtspr   ibat3u,r5
	mtspr   dbat0u,r5
	mtspr   dbat1u,r5
	mtspr   dbat2u,r5
	mtspr   dbat3u,r5
	isync

	/* Start loading each BAT register with values defined above:
	 */
	addis   r4,0,IBAT0L_VAL@h
	ori     r4,r4,IBAT0L_VAL@l
	addis   r3,0,IBAT0U_VAL@h
	ori     r3,r3,IBAT0U_VAL@l
	mtspr   ibat0l,r4
	mtspr   ibat0u,r3
	isync

	addis   r4,0,DBAT0L_VAL@h
	ori     r4,r4,DBAT0L_VAL@l
	addis   r3,0,DBAT0U_VAL@h
	ori     r3,r3,DBAT0U_VAL@l
	mtspr   dbat0l,r4
	mtspr   dbat0u,r3
	isync

	addis   r4,0,IBAT1L_VAL@h
	ori     r4,r4,IBAT1L_VAL@l
	addis   r3,0,IBAT1U_VAL@h
	ori     r3,r3,IBAT1U_VAL@l
	mtspr   ibat1l,r4
	mtspr   ibat1u,r3
	isync

	addis   r4,0,DBAT1L_VAL@h
	ori     r4,r4,DBAT1L_VAL@l
	addis   r3,0,DBAT1U_VAL@h
	ori     r3,r3,DBAT1U_VAL@l
	mtspr   dbat1l,r4
	mtspr   dbat1u,r3
	isync

	addis   r4,0,IBAT2L_VAL@h
	ori     r4,r4,IBAT2L_VAL@l
	addis   r3,0,IBAT2U_VAL@h
	ori     r3,r3,IBAT2U_VAL@l
	mtspr   ibat2l,r4
	mtspr   ibat2u,r3
	isync

	addis   r4,0,DBAT2L_VAL@h
	ori     r4,r4,DBAT2L_VAL@l
	addis   r3,0,DBAT2U_VAL@h
	ori     r3,r3,DBAT2U_VAL@l
	mtspr   dbat2l,r4
	mtspr   dbat2u,r3
	isync

	addis   r4,0,IBAT3L_VAL@h
	ori     r4,r4,IBAT3L_VAL@l
	addis   r3,0,IBAT3U_VAL@h
	ori     r3,r3,IBAT3U_VAL@l
	mtspr   ibat3l,r4
	mtspr   ibat3u,r3
	isync

	addis   r4,0,DBAT3L_VAL@h
	ori     r4,r4,DBAT3L_VAL@l
	addis   r3,0,DBAT3U_VAL@h
	ori     r3,r3,DBAT3U_VAL@l
	mtspr   dbat3l,r4
	mtspr   dbat3u,r3
	isync
                                
	/* Now that we're done setting up our BATs, re-enable translation:
	 */
	mfmsr   r3
	ori     r3,r3,0x0030
	mtmsr   r3
	isync

	blr
