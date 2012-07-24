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
	.text
	.align	2
	.global	getPBA1
getPBA1:
	addi	r4,0,pb1ap
	mtdcr	ebccfga,r4
	mfdcr	r3,ebccfgd
	blr

	.global	getPBC0
getPBC0:
	addi	r4,0,pb0cr
	mtdcr	ebccfga,r4
	mfdcr	r3,ebccfgd
	blr
	
	.global	getPBC1
getPBC1:
	addi	r4,0,pb1cr
	mtdcr	ebccfga,r4
	mfdcr	r3,ebccfgd
	blr
	
	.global	getPBC2
getPBC2:
	addi	r4,0,pb2cr
	mtdcr	ebccfga,r4
	mfdcr	r3,ebccfgd
	blr
	
	.global	getPBC3
getPBC3:
	addi	r4,0,pb3cr
	mtdcr	ebccfga,r4
	mfdcr	r3,ebccfgd
	blr
	
	.global	getPBC4
getPBC4:
	addi	r4,0,pb4cr
	mtdcr	ebccfga,r4
	mfdcr	r3,ebccfgd
	blr
	
	.global	getPBC5
getPBC5:
	addi	r4,0,pb5cr
	mtdcr	ebccfga,r4
	mfdcr	r3,ebccfgd
	blr
	
	.global ppcMfcpc0psr
ppcMfcpc0psr:
	mfdcr	r3,cpc0_psr
	blr
	
/*
 * On-chip-memory (ocm) register access functions
 * for the PPC405:
 */

	.text
	.align  2
	.globl  ppcMtocm0isarc
ppcMtocm0isarc:
	mtdcr	ocm0_isarc,r3
	blr

	.text
	.align  2
	.globl  ppcMtocm0isctl
ppcMtocm0isctl:
	mtdcr	ocm0_isctl,r3
	blr

	.text
	.align  2
	.globl  ppcMtocm0dsarc
ppcMtocm0dsarc:
	mtdcr	ocm0_dsarc,r3
	blr

	.text
	.align  2
	.globl  ppcMtocm0dsctl
ppcMtocm0dsctl:
	mtdcr	ocm0_dsctl,r3
	blr

	.text
	.align  2
	.globl  ppcMfocm0isarc
ppcMfocm0isarc:
	mfdcr	r3,ocm0_isarc
	blr

	.text
	.align  2
	.globl  ppcMfocm0isctl
ppcMfocm0isctl:
	mfdcr	r3,ocm0_isctl
	blr

	.text
	.align  2
	.globl  ppcMfocm0dsarc
ppcMfocm0dsarc:
	mfdcr	r3,ocm0_dsarc
	blr

	.text
	.align  2
	.globl  ppcMfocm0dsctl
ppcMfocm0dsctl:
	mfdcr	r3,ocm0_dsctl
	blr

	.text
	.align  2
	.globl  ppcMfcntrl0
ppcMfcntrl0:
	mfdcr	r3,cntrl0
	blr

	.text
	.align  2
	.globl  ppcMtcntrl0
ppcMtcntrl0:
	mtdcr	cntrl0,r3
	blr

	.text
	.align  2
	.globl  ppcMfcpc0er
ppcMfcpc0er:
	mfdcr	r3,cpc0_er
	blr

	.text
	.align  2
	.globl  ppcMtcpc0er
ppcMtcpc0er:
	mtdcr	cpc0_er,r3
	blr

	.text
	.align  2
	.globl  ppcMtexier
ppcMtexier:
	mtdcr	exier,r3
	blr

	.text
	.align  2
	.globl  ppcMfexier
ppcMfexier:
	mfdcr	r3,exier
	blr

	.text
	.align  2
	.globl  ppcMtexisr
ppcMtexisr:
	mtdcr	exisr,r3
	blr

	.text
	.align  2
	.globl  ppcMfexisr
ppcMfexisr:
	mfdcr	r3,exisr
	blr

	.text
	.align	2
	.global	getTLB
getTLB:
	mfspr	r3,0x10c
	blr

