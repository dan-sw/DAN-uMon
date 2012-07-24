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
 *  File Name:   ppcmal.s
 *
 *  Function:    40x PowerPC Ethernet MAL Functions.
 *
 *  Author:      James Burke 
 *
 *  Change Activity-
 *
 *  Date        Description of Change                                       BY
 *  ---------   ---------------------                                       ---
 *  26-May-99   Created                                                     JWB
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
#include "ppcmal.h"

/*
 * Function:     ppcMfmalcr
 * Description:  Move from MALMCR register
 * Input:        none
 * Output:       r3 - value of MALMCR.
 */
	.text
	.align	2
	.globl  ppcMfmalcr
ppcMfmalcr:
	mfdcr	r3,malmcr
	blr

/*
 * Function:     ppcMtmalcr
 * Description:  Move to MALMCR register
 * Input:        r3 - new value of MALMCR.
 * Output:       none
 */
	.text
	.align	2
	.globl  ppcMtmalcr
ppcMtmalcr:
	mtdcr	malmcr,r3
	blr

/*
 * Function:     ppcMfmalesr
 * Description:  Move from MALESR register
 * Input:        none
 * Output:       r3 - value of MALESR.
 */
	.text
	.align  2
	.globl  ppcMfmalesr
ppcMfmalesr:
	mfdcr   r3,malesr
	blr

/*
 * Function:     ppcMtmalesr
 * Description:  Move to MALESR register
 * Input:        r3 - new value of MALESR.
 * Output:       none
 */
	.text
	.align  2
	.globl  ppcMtmalesr
ppcMtmalesr:
	mtdcr   malesr,r3
	blr

/*
 * Function:     ppcMfmalier
 * Description:  Move from MALIER register
 * Input:        none
 * Output:       r3 - value of MALIER.
 */
	.text
	.align  2
	.globl  ppcMfmalier
ppcMfmalier:
	mfdcr   r3,malier
	blr

/*
 * Function:     ppcMtmalier
 * Description:  Move to MALIER register
 * Input:        r3 - new value of MALIER.
 * Output:       none
 */
	.text
	.align  2
	.globl  ppcMtmalier
ppcMtmalier:
	mtdcr   malier,r3
	blr


/*
 * Function:     ppcMfmaldbr
 * Description:  Move from MALDBR register
 * Input:        none
 * Output:       r3 - value of MALDBR.
 */
	.text
	.align  2
	.globl  ppcMfmaldbr
ppcMfmaldbr:
	mfdcr   r3,maldbr
	blr

/*
 * Function:     ppcMfmaltxcasr
 * Description:  Move from MALTXCASR register
 * Input:        none
 * Output:       r3 - value of MALTXCASR.
 */
	.text
	.align  2
	.globl  ppcMfmaltxcasr
ppcMfmaltxcasr:
	mfdcr   r3,maltxcasr
	blr

/*
 * Function:     ppcMtmaltxcasr
 * Description:  Move to MALTXCASR register
 * Input:        r3 - new value of MALTXCASR.
 * Output:       none
 */
	.text
	.align  2
	.globl  ppcMtmaltxcasr
ppcMtmaltxcasr:
	mtdcr   maltxcasr,r3
	blr

/*
 * Function:     ppcMfmaltxcarr
 * Description:  Move from MALTXCARR register
 * Input:        none
 * Output:       r3 - value of MALTXCARR.
 */
	.text
	.align  2
	.globl  ppcMfmaltxcarr
ppcMfmaltxcarr:
	mfdcr   r3,maltxcarr
	blr

/*
 * Function:     ppcMtmaltxcarr
 * Description:  Move to MALTXCARR register
 * Input:        r3 - new value of MALTXCARR.
 * Output:       none
 */
	.text
	.align  2
	.globl  ppcMtmaltxcarr
ppcMtmaltxcarr:
	mtdcr   maltxcarr,r3
	blr


/*
 * Function:     ppcMfmaltxeobisr
 * Description:  Move from MALTXEOBISR register
 * Input:        none
 * Output:       r3 - value of MALTXEOBISR.
 */
	.text
	.align  2
	.globl  ppcMfmaltxeobisr
ppcMfmaltxeobisr:
	mfdcr   r3,maltxeobisr
	blr


/*
 * Function:     ppcMtmaltxeobisr
 * Description:  Move to MALTXEOBISR register
 * Input:        r3 - new value of MALTXEOBISR register
 * Output:       none
 */
	.text
	.align  2
	.globl  ppcMtmaltxeobisr
ppcMtmaltxeobisr:
	mtdcr   maltxeobisr,r3
	blr


/*
 * Function:     ppcMfmaltxdeir  
 * Description:  Move from MALTXDEIR register
 * Input:        none
 * Output:       r3 - value of MALTXDEIR.
 */
	.text
	.align  2
	.globl  ppcMfmaltxdeir  
ppcMfmaltxdeir:
	mfdcr   r3,maltxdeir
	blr

/*
 * Function:     ppcMtmaltxdeir
 * Description:  Move to MALTXDEIR register
 * Input:        r3 - new value of MALTXDEIR.
 * Output:       none
 */
	.text
	.align  2
	.globl  ppcMtmaltxdeir
ppcMtmaltxdeir:
	mtdcr   maltxdeir,r3
	blr

/*
 * Function:     ppcMfmalrxcasr
 * Description:  Move from MALRXCASR register
 * Input:        none
 * Output:       r3 - value of MALRXCASR.
 */
	.text
	.align  2
	.globl  ppcMfmalrxcasr
ppcMfmalrxcasr:
	mfdcr   r3,malrxcasr
	blr

/*
 * Function:     ppcMtmalrxcasr
 * Description:  Move to MALRXCASR register
 * Input:        r3 - new value of MALRXCASR.
 * Output:       none
 */
	.text
	.align  2
	.globl  ppcMtmalrxcasr
ppcMtmalrxcasr:
	mtdcr   malrxcasr,r3
	blr

/*
 * Function:     ppcMfmalrxcarr
 * Description:  Move from MALRXCARR register
 * Input:        none
 * Output:       r3 - value of MALRXCARR.
 */
	.text
	.align  2
	.globl  ppcMfmalrxcarr
ppcMfmalrxcarr:
	mfdcr   r3,malrxcarr
	blr

/*
 * Function:     ppcMtmalrxcarr
 * Description:  Move to MALRXCARR register
 * Input:        r3 - new value of MALRXCARR.
 * Output:       none
 */
	.text
	.align  2
	.globl  ppcMtmalrxcarr
ppcMtmalrxcarr:
	mtdcr   malrxcarr,r3
	blr

/*
 * Function:     ppcMfmalrxeobisr
 * Description:  Move from MALRXEOBISR register
 * Input:        none
 * Output:       r3 - value of MALRXEOBISR.
 */
	.text
	.align  2
	.globl  ppcMfmalrxeobisr
ppcMfmalrxeobisr:
	mfdcr   r3,malrxeobisr
	blr

/*
 * Function:     ppcMtmalrxeobisr
 * Description:  Move to MALRXEOBISR register
 * Input:        r3 - new value of MALRXEOBISR register
 * Output:       none
 */
	.text
	.align  2
	.globl  ppcMtmalrxeobisr
ppcMtmalrxeobisr:
	mtdcr   malrxeobisr,r3
	blr


/*
 * Function:     ppcMfmalrxdeir
 * Description:  Move from MALRXDEIR register
 * Input:        none
 * Output:       r3 - value of MALRXDEIR.
 */
	.text
	.align  2
	.globl  ppcMfmalrxdeir
ppcMfmalrxdeir:
	mfdcr   r3,malrxdeir
	blr

/*
 * Function:     ppcMtmalrxdeir
 * Description:  Move to MALRXDEIR register
 * Input:        r3 - new value of MALRXDEIR.
 * Output:       none
 */
	.text
	.align  2
	.globl  ppcMtmalrxdeir
ppcMtmalrxdeir:
	mtdcr   malrxdeir,r3
	blr


/*
 * Function:     ppcMfmaltxctp0r
 * Description:  Move from MALTXCTP0R register
 * Input:        none
 * Output:       r3 - value of MALTXCTP0R.
 */
	.text
	.align  2
	.globl  ppcMfmaltxctp0r
ppcMfmaltxctp0r:
	mfdcr   r3,maltxctp0r
	blr

/*
 * Function:     ppcMtmaltxctp0r
 * Description:  Move to MALTXCTP0R register
 * Input:        r3 - new value of MALTXCTP0R.
 * Output:       none
 */
	.text
	.align  2
	.globl  ppcMtmaltxctp0r
ppcMtmaltxctp0r:
	mtdcr   maltxctp0r,r3
	blr

/*
 * Function:     ppcMfmaltxctp1r
 * Description:  Move from MALTXCTP1R register
 * Input:        none
 * Output:       r3 - value of MALTXCTP1R.
 */
	.text
	.align  2
	.globl  ppcMfmaltxctp1r
ppcMfmaltxctp1r:
	mfdcr   r3,maltxctp1r
	blr

/*
 * Function:     ppcMtmaltxctp1r
 * Description:  Move to MALTXCTP1R register
 * Input:        r3 - new value of MALTXCTP1R.
 * Output:       none
 */
	.text
	.align  2
	.globl  ppcMtmaltxctp1r
ppcMtmaltxctp1r:
	mtdcr   maltxctp1r,r3
	blr

/*
 * Function:     ppcMfmalrxctp0r
 * Description:  Move from MALRXCTP0R register
 * Input:        none
 * Output:       r3 - value of MALRXCTP0R.
 */
	.text
	.align  2
	.globl  ppcMfmalrxctp0r
ppcMfmalrxctp0r:
	mfdcr   r3,malrxctp0r
	blr

/*
 * Function:     ppcMtmalrxctp0r
 * Description:  Move to MALRXCTP0R register
 * Input:        r3 - new value of MALRXCTP0R.
 * Output:       none
 */
	.text
	.align  2
	.globl  ppcMtmalrxctp0r
ppcMtmalrxctp0r:
	mtdcr   malrxctp0r,r3
	blr

/*
 * Function:     ppcMfmalrcbs0  
 * Description:  Move from MALRCBS0 register
 * Input:        none
 * Output:       r3 - value of MALRCBS0.
 */
	.text
	.align  2
	.globl  ppcMfmalrcbs0
ppcMfmalrcbs0:
	mfdcr   r3,malrcbs0
	blr

/*
 * Function:     ppcMtmalrcbs0
 * Description:  Move to MALRCBS0 register
 * Input:        r3 - new value of MALRCBS0.
 * Output:       none
 */
	.text
	.align  2
	.globl  ppcMtmalrcbs0
ppcMtmalrcbs0:
	mtdcr   malrcbs0,r3
	blr

