/*
 * This source code has been made available to you by IBM on an AS-IS
 * basis.  Anyone receiving this source is licensed under IBM
 * copyrights to use it in any way he or she deems fit, including
 * copying it, modifying it, compiling it, and redistributing it either
 * with or without modifications.  No license under IBM patents or
 * patent applications is to be implied by the copyright license.
 *
 * Any user of this software should understand that IBM cannot provide
 * technical support for this software and will not be responsible for
 * any consequences resulting from the use of this software.
 *
 * Any person who transfers this source code or any derivative work
 * must include the IBM copyright notice, this paragraph, and the
 * preceding two paragraphs in the transferred software.
 *
 * COPYRIGHT   I B M   CORPORATION 1992, 1999
 * LICENSED MATERIAL  -  PROGRAM PROPERTY OF I B M
 *
 *	General notice:
 *	This code is part of a boot-monitor package developed as a generic base
 *	platform for embedded system designs.  As such, it is likely to be
 *	distributed to various projects beyond the control of the original
 *	author.  Please notify the author of any enhancements made or bugs found
 *	so that all may benefit from the changes.  In addition, notification back
 *	to the author will allow the new user to pick up changes that may have
 *	been made by other users after this version of the code was distributed.
 *
 *	Note1: the majority of this code was edited with 4-space tabs.
 *	Note2: as more and more contributions are accepted, the term "author"
 *		   is becoming a mis-representation of credit.
 *
 *	Original author:	Ed Sutter
 *	Email:				esutter@lucent.com
 *	Phone:				908-582-2351
 */
#include "arch_ppc.h"

/*
 * Function:     ppcAbend
 * Description:  Execute an invalid op code, causing a Program Check Interrupt
 * Input:        none
 * Output:       none
 */
	.text
	.align  2
	.globl  ppcAbend
ppcAbend:
	.long   0

/*
 * Function:     ppcAndMsr
 * Description:  AND With Machine State Register (MSR)
 * Input:        r3 = value to AND with MSR
 * Output:       r3 = old MSR contents
 */
	.text
	.align  2
	.globl  ppcAndMsr
ppcAndMsr:
	mfmsr   r6
	and     r7,r6,r3
	mtmsr   r7
	ori     r3,r6,0x000
	blr

/*
 * Function:     ppcCntlzw
 * Description:  Count Leading Zeros
 * Input:        r3 = input value
 * Output:       r3 = number of leading zeroes in the input value
 */
	.text
	.align  2
	.globl  ppcCntlzw
ppcCntlzw:
	cntlzw  r3,r3
	blr

/*
 * Function:     ppcDcbi
 * Description:  Data Cache block Invalidate
 * Input:        r3 = effective address
 * Output:       none.
 */
	.text
	.align  2
	.globl  ppcDcbi
ppcDcbi:
	dcbi    r0,r3
	blr

/*
 * Function:     ppcDcbf
 * Description:  Data Cache block flush
 * Input:        r3 = effective address
 * Output:       none.
 */
	.text
	.align  2
	.globl  ppcDcbf
ppcDcbf:
	dcbf    r0,r3
	blr

/*
 * Function:     ppcDcbst
 * Description:  Data Cache block Store
 * Input:        r3 = effective address
 * Output:       none.
 */
	.text
	.align  2
	.globl  ppcDcbst
ppcDcbst:
	dcbst   r0,r3
	blr

/*
 * Function:     ppcDcbz
 * Description:  Data Cache Block set to Zero
 * Input:        r3 = effective address
 * Output:       none.
 */
#ifndef PASS1_405GP
	.text
	.align  2
	.globl	ppcDcbz
ppcDcbz:
	dcbz    r0,r3
	blr
#endif

/*
 * Function:     ppcHalt
 * Description:  Halt Pseudo-Op
 * Input:        none.
 * Output:       none.
 */
	.text
	.align  2
	.globl  ppcHalt
ppcHalt:
	b       ppcHalt

/*
 * Function:     ppcIsync
 * Description:  Instruction Cache Synchronize
 * Input:        none.
 * Output:       none.
 */
	.text
	.align  2
	.globl  ppcIsync 
ppcIsync:
	isync
	blr

/*
 * Function:     ppcIcbi 
 * Description:  Instruction Cache Block Invalidate
 * Input:        r3 = effective address
 * Output:       none.
 */
	.text
	.align  2
	.globl  ppcIcbi
ppcIcbi:
	icbi    r0,r3
	blr

/*
 * Function:     ppcMfgpr1
 * Description:  Move From gpr1
 * Input:        none
 * Output:       r3 = gpr1
 */
	.text
	.align  2
	.globl  ppcMfgpr1
ppcMfgpr1:
	ori     r3,r1,0x0000
	blr

/*
 * Function:     ppcMfgpr2
 * Description:  Move From gpr2
 * Input:        none
 * Output:       r3 = gpr2
 */
	.text
	.align  2
	.globl  ppcMfgpr2
ppcMfgpr2:
	ori     r3,r2,0x0000
	blr

/*
 * Function:     ppcMfmsr
 * Description:  Move From Machine State Register
 * Input:        none
 * Output:       r3 = msr
 */
	.text
	.align  2
	.globl  ppcMfmsr
ppcMfmsr:
	mfmsr   r3
	blr

/*
 * Function:     ppcMfsprg0
 * Description:  Move From SPRG0
 * Input:        none
 * Output:       r3 = sprg0
 */
	.text
	.align  2
	.globl  ppcMfsprg0
ppcMfsprg0:
	mfspr	r3,sprg0
	blr

/*
 * Function:     ppcMfsprg1
 * Description:  Move From SPRG1
 * Input:        none
 * Output:       r3 = sprg1
 */
	.text
	.align  2
	.globl  ppcMfsprg1
ppcMfsprg1:
	mfspr	r3,sprg1
	blr

/*
 * Function:     ppcMfsprg2
 * Description:  Move From SPRG2
 * Input:        none
 * Output:       r3 = sprg2
 */
	.text
	.align  2
	.globl  ppcMfsprg2
ppcMfsprg2:
	mfspr	r3,sprg2
	blr

/*
 * Function:     ppcMfsprg3
 * Description:  Move From SPRG3
 * Input:        none
 * Output:       r3 = sprg3
 */
	.text
	.align  2
	.globl  ppcMfsprg3
ppcMfsprg3:
	mfspr	r3,sprg3
	blr

/*
 * Function:     ppcMfsrr0
 * Description:  Move From SRR0
 * Input:        none
 * Output:       r3 = srr0
 */
	.text
	.align  2
	.globl  ppcMfsrr0
ppcMfsrr0:
	mfsrr0  r3
	blr

/*
 * Function:     ppcMfsrr1
 * Description:  Move From SRR1
 * Input:        none
 * Output:       r3 = srr1
 */
	.text
	.align  2
	.globl  ppcMfsrr1
ppcMfsrr1:
	mfsrr1  r3
	blr

/*
 * Function:     ppcMfpvr
 * Description:  Move From PVR
 * Input:        none
 * Output:       r3 = tid
 */
	.text
	.align  2
	.globl  ppcMfpvr
ppcMfpvr:
	mfpvr   r3
	blr

/*
 * Function:     ppcMtmsr
 * Description:  Move To Machine State Register
 * Input:        none
 * Output:       r3 = msr
 */
	.text
	.align  2
	.globl  ppcMtmsr
ppcMtmsr:
	mtmsr   r3
	blr

/*
 * Function:	ppcMtsprg0
 * Description:	Move To SPRG0
 * Input:	r3 = value to be moved to sprg0
 * Output:	none
 */
	.text
	.align  2
	.globl  ppcMtsprg0
ppcMtsprg0:
	mtspr	sprg0,r3
	blr

/*
 * Function:	ppcMtsprg1
 * Description:	Move To SPRG1
 * Input:	r3 = value to be moved to sprg1
 * Output:	none
 */
	.text
	.align  2
	.globl  ppcMtsprg1
ppcMtsprg1:
	mtspr	sprg1,r3
	blr

/*
 * Function:	ppcMtsprg2
 * Description:	Move To SPRG2
 * Input:	r3 = value to be moved to sprg2
 * Output:	none
 */
	.text
	.align  2
	.globl  ppcMtsprg2
ppcMtsprg2:
	mtspr	sprg2,r3
	blr

/*
 * Function:	ppcMtsprg3
 * Description:	Move To SPRG3
 * Input:	r3 = value to be moved to sprg3
 * Output:	none
 */
	.text
	.align  2
	.globl  ppcMtsprg3
ppcMtsprg3:
	mtspr	sprg3,r3
	blr

/*
 * Function:     ppcMtsrr0
 * Description:  Move To SRR0
 * Input:        r3 = value to be moved to SRR0
 * Output:       none
 */
	.text
	.align  2
	.globl  ppcMtsrr0
ppcMtsrr0:
	mtsrr0  r3
	blr

/*
 * Function:     ppcMtsrr1
 * Description:  Move To SRR1
 * Input:        r3 = value to be moved to SRR1
 * Output:       none
 */
	.text
	.align  2
	.globl  ppcMtsrr1
ppcMtsrr1:
	mtsrr1  r3
	blr

/*
 * Function:     ppcMtevpr
 * Description:  Move To EVPR
 * Input:        r3 = value to be moved to EVPR
 * Output:       none
 */
	.text
	.align  2
	.globl  ppcMtevpr
ppcMtevpr:
	mtspr	evpr,r3
	blr

/*
 * Function:     ppcMfevpr
 * Description:  Move From EVPR
 * Input:        r3 = value to be moved from EVPR
 * Output:       none
 */
	.text
	.align  2
	.globl  ppcMfevpr
ppcMfevpr:
	mfspr	r3,evpr
	blr

/*
 * Function:     ppcMtdccr
 * Description:  Move To DCCR
 * Input:        none
 */
	.text
	.align  2
	.globl  ppcMtdccr
ppcMtdccr:
	mtspr	dccr,r3
	blr

/*
 * Function:     ppcMticcr
 * Description:  Move To ICCR
 * Input:        none
 */
	.text
	.align  2
	.globl  ppcMticcr
ppcMticcr:
	mtspr	iccr,r3
	blr

/*
 * Function:     ppcMficcr
 * Description:  Move From ICCR
 * Input:        r3 = value to be moved from ICCR
 * Output:       none
 */
	.text
	.align  2
	.globl  ppcMficcr
ppcMficcr:
	mfspr	r3,iccr
	blr

/*
 * Function:     ppcMfdccr
 * Description:  Move From DCCR
 * Input:        r3 = value to be moved from DCCR
 * Output:       none
 */
	.text
	.align  2
	.globl  ppcMfdccr
ppcMfdccr:
	mfspr	r3,dccr
	blr


/*
 * Function:     ppcOrMsr
 * Description:  OR With Machine State Register (MSR)
 * Input:        r3 = value to OR with MSR
 * Output:       r3 = old MSR contents
 */
	.text
	.align  2
	.globl  ppcOrMsr
ppcOrMsr:
	mfmsr   r6
	or      r7,r6,r3
	mtmsr   r7
	ori     r3,r6,0x0000
	blr

/*
 * Function:     ppcSync
 * Description:  Processor Synchronize
 * Input:        none.
 * Output:       none.
 */
	.text
	.align  2
	.globl  ppcSync
ppcSync:
	sync
	blr

/*
 * Function:     ppcEieio
 * Description:  Enforce in-order execution of I/O
 * Input:        none.
 * Output:       none.
 */
	.text
	.align  2
	.globl  ppcEieio
ppcEieio:
	eieio
	blr

	.text
	.align	2
	.globl	ppcMtsp
ppcMtsp:
	mr		r1,r3
	blr

	.text
	.align	2
	.globl	ppcMfsp
ppcMfsp:
	mr		r3,r1
	blr

	.text
	.align	2
	.globl	ppcMfcr
ppcMfcr:
	mfcr   r3
	blr

	.text
	.align	2
	.globl	ppcMtcr
ppcMtcr:
	mtcr   r3
	blr


/*
 * Debug (PPC403/405) register access:
 */
	.text
	.align  2
	.globl  ppcMtdbcr0
ppcMtdbcr0:
	mtspr	dbcr0,r3
	blr

	.text
	.align  2
	.globl  ppcMfdbcr0
ppcMfdbcr0:
	mfspr	r3,dbcr0
	blr

	.text
	.align  2
	.globl  ppcMtdbcr1
ppcMtdbcr1:
	mtspr	dbcr1,r3
	blr

	.text
	.align  2
	.globl  ppcMfdbcr1
ppcMfdbcr1:
	mfspr	r3,dbcr1
	blr

	.text
	.align  2
	.globl  ppcMtdbsr
ppcMtdbsr:
	mtspr	dbsr,r3
	blr

	.text
	.align  2
	.globl  ppcMfdbsr
ppcMfdbsr:
	mfspr	r3,dbsr
	blr

	.text
	.align  2
	.globl  ppcMtiac1
ppcMtiac1:
	mtspr	iac1,r3
	blr

	.text
	.align  2
	.globl  ppcMfiac1
ppcMfiac1:
	mfspr	r3,iac1
	blr

	.text
	.align  2
	.globl  ppcMtiac2
ppcMtiac2:
	mtspr	iac2,r3
	blr

	.text
	.align  2
	.globl  ppcMfiac2
ppcMfiac2:
	mfspr	r3,iac2
	blr

	.text
	.align  2
	.globl  ppcMtiac3
ppcMtiac3:
	mtspr	iac3,r3
	blr

	.text
	.align  2
	.globl  ppcMfiac3
ppcMfiac3:
	mfspr	r3,iac3
	blr

	.text
	.align  2
	.globl  ppcMtiac4
ppcMtiac4:
	mtspr	iac4,r3
	blr

	.text
	.align  2
	.globl  ppcMfiac4
ppcMfiac4:
	mfspr	r3,iac4
	blr

	.text
	.align  2
	.globl  ppcMtdac1
ppcMtdac1:
	mtspr	dac1,r3
	blr

	.text
	.align  2
	.globl  ppcMfdac1
ppcMfdac1:
	mfspr	r3,dac1
	blr

	.text
	.align  2
	.globl  ppcMtdac2
ppcMtdac2:
	mtspr	dac2,r3
	blr

	.text
	.align  2
	.globl  ppcMfdac2
ppcMfdac2:
	mfspr	r3,dac2
	blr

	.text
	.align  2
	.globl  ppcMtdvc1
ppcMtdvc1:
	mtspr	dvc1,r3
	blr

	.text
	.align  2
	.globl  ppcMfdvc1
ppcMfdvc1:
	mfspr	r3,dvc1
	blr

	.text
	.align  2
	.globl  ppcMtdvc2
ppcMtdvc2:
	mtspr	dvc2,r3
	blr

	.text
	.align  2
	.globl  ppcMfdvc2
ppcMfdvc2:
	mfspr	r3,dvc2
	blr

	.text
	.align  2
	.globl  ppcMtimmr
ppcMtimmr:
	mtspr	immr,r3
	blr

	.text
	.align  2
	.globl  ppcMfimmr
ppcMfimmr:
	mfspr	r3,immr
	blr

	.text
	.align  2
	.globl  ppcMtdabr
ppcMtdabr:
	mtspr	1013,r3
	blr

	.text
	.align  2
	.globl  ppcMtiabr
ppcMtiabr:
	mtspr	1010,r3
	blr

/*
 *	void ppc_flushDcache(char *addr, unsigned int size)
 *
 *	Assumes R3=addr, R4=size.
 *	Flush d-cache for size bytes starting at addr.
 */

	.global ppc_flushDcache

ppc_flushDcache:
	add		r4,r3,r4		/* Last address to flush */
	clrrwi	r3,r3,4			/* Clear low 4 bits */

flush_loop:
	dcbf	r0,r3			/* Flush data block pointed to by r3 */
	addi	r3,r3,16		/* Advance to next block */
	cmplw	r3,r4
	ble		flush_loop
	sync
	blr	

/*
 *	void ppc_invalidateIcache(char *addr, unsigned int size)
 *
 *	Assumes r3=addr, r4=size and r5=scratch.
 *	Invalidate i-cache for size bytes starting at addr.
 *
 */
	.global ppc_invalidateIcache

ppc_invalidateIcache:
	add		r4,r3,r4		/* Last address to flush */
	clrrwi	r3,r3,4			/* Clear low 4 bits */

inv_loop:
	icbi	r0,r3			/* Invalidate instr block pointed to by r3 */
	addi	r3,r3,16		/* Advance to next block */
	cmplw	r3,r4
	ble		inv_loop
	isync
	blr	

	.global rlwtry
	.global rlwtry1

	/* This assembler instruction confuses me everytime I have to
	 * understand how it works, so I figured it would be worth it
	 * to have these two examples to refer to...
 	 *
	 * rlwinm RA,RS,SH,MB,ME
	 *
	 * Shift RS left 'SH' bits; then AND that with the mask established 
	 * by MB & ME and put the result into RA.
	 *
	 * Look at the examples below...
	 *
	 * The shift value is 0, with MB,ME set to 3,5
	 * Recall that bit numbers start from the left for PPC...
	 * Literally...
	 * Starting at bit position MB (inclusive) and moving right, all bits
	 * from MB to ME are set to create the mask.  If MB is greater than ME,
	 * then when we get to 31, just wrap around to bit zero and continue.
	 * 
	 */
rlwtry:
	xor		r3,r3,r3
	addis   r3,r0,0xffff
	ori     r3,r3,0xffff
	rlwinm	r3,r3,0,3,5			/* Mask = 0x1c000000 */
	blr

rlwtry1:
	xor		r3,r3,r3
	addis   r3,r0,0xffff
	ori     r3,r3,0xffff
	rlwinm	r3,r3,0,5,3			/* Mask = 0xf7ffffff */
	blr
