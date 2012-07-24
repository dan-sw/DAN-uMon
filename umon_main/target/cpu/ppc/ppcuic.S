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
 *
 *  File Name:   ppcuic.s
 *
 *  Function:    40x PowerPC Universal Interrupt Controller Functions.
 *
 *  Author:      Neil Leeder
 *
 *  Change Activity-
 *
 *  Date        Description of Change                                       BY
 *  ---------   ---------------------                                       ---
 *  13-Apr-99   Created                                                     NL
 *
 */
#include "config.h"
#include "arch_ppc.h"
#include "ppcuic.h"


/*
 * Function:     ppcMfuiccr
 * Description:  Move from UICCR register
 * Input:        none
 * Output:       r3 - value of UICCR.
 */
	.text
	.align	2
	.globl  ppcMfuiccr
ppcMfuiccr:
	mfdcr	r3,uiccr
	blr

/*
 * Function:     ppcMtuiccr
 * Description:  Move to UICCR register
 * Input:        r3 - new value of UICCR.
 * Output:       none
 */
	.text
	.align	2
	.globl  ppcMtuiccr
ppcMtuiccr:
	mtdcr	uiccr,r3
	blr

/*
 * Function:     ppcMfuicer
 * Description:  Move from UICER register
 * Input:        none
 * Output:       r3 - value of UICER.
 */
	.text
	.align	2
	.globl  ppcMfuicer
ppcMfuicer:
	mfdcr	r3,uicer
	blr

/*
 * Function:     ppcMtuicer
 * Description:  Move to UICER register
 * Input:        r3 - new value of UICER.
 * Output:       none
 */
	.text
	.align	2
	.globl  ppcMtuicer
ppcMtuicer:
	mtdcr	uicer,r3
	blr

/*
 * Function:     ppcMfuicmsr
 * Description:  Move from UICMSR register
 * Input:        none
 * Output:       r3 - value of UICMSR.
 */
	.text
        .align  2
	.globl  ppcMfuicmsr
ppcMfuicmsr:
	mfdcr   r3,uicmsr
	blr

/*
 * Function:     ppcMfuicpr
 * Description:  Move from UICPR register
 * Input:        none
 * Output:       r3 - value of UICPR.
 */
	.text
	.align	2
	.globl  ppcMfuicpr
ppcMfuicpr:
	mfdcr	r3,uicpr
	blr

/*
 * Function:     ppcMtuicpr
 * Description:  Move to UICPR register
 * Input:        r3 - new value of UICPR.
 * Output:       none
 */
	.text
	.align	2
	.globl  ppcMtuicpr
ppcMtuicpr:
	mtdcr	uicpr,r3
	blr

/*
 * Function:     ppcMfuicsr
 * Description:  Move from UICSR register
 * Input:        none
 * Output:       r3 - value of UICSR.
 */
	.text
	.align	2
	.globl  ppcMfuicsr
ppcMfuicsr:
	mfdcr	r3,uicsr
	blr

/*
 * Function:     ppcMtuicsr
 * Description:  Move to UICSR register
 * Input:        r3 - new value of UICSR.
 * Output:       none
 */
	.text
	.align	2
	.globl  ppcMtuicsr
ppcMtuicsr:
	mtdcr	uicsr,r3
	blr

/*
 * Function:     ppcMfuicsrs
 * Description:  Move from UICSRS register
 * Input:        none
 * Output:       r3 - value of UICSRS.
 */
	.text
	.align	2
	.globl  ppcMfuicsrs
ppcMfuicsrs:
	mfdcr	r3,uicsrs
	blr

/*
 * Function:     ppcMtuicsrs
 * Description:  Move to UICSRS register
 * Input:        r3 - new value of UICSRS.
 * Output:       none
 */
	.text
	.align	2
	.globl  ppcMtuicsrs
ppcMtuicsrs:
	mtdcr	uicsrs,r3
	blr

/*
 * Function:     ppcMfuictr
 * Description:  Move from UICTR register
 * Input:        none
 * Output:       r3 - value of UICTR.
 */
	.text
	.align	2
	.globl  ppcMfuictr
ppcMfuictr:
	mfdcr	r3,uictr
	blr

/*
 * Function:     ppcMtuictr
 * Description:  Move to UICTR register
 * Input:        r3 - new value of UICTR.
 * Output:       none
 */
	.text
	.align	2
	.globl  ppcMtuictr
ppcMtuictr:
	mtdcr	uictr,r3
	blr

/*
 * Function:     ppcMfuicvr
 * Description:  Move from UICVR register
 * Input:        none
 * Output:       r3 - value of UICVR.
 */
	.text
	.align	2
	.globl  ppcMfuicvr
ppcMfuicvr:
	mfdcr	r3,uicvr
	blr

/*
 * Function:     ppcMtuicvcr
 * Description:  Move to UICVCR register
 * Input:        r3 - new value of UICVCR.
 * Output:       none
 */
	.text
	.align	2
	.globl  ppcMtuicvcr
ppcMtuicvcr:
	mtdcr	uicvcr,r3
	blr

