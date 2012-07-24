/* vectors.s:
	The exception table of the PowerPC starts at some point based on the
	content of the EVPR (Exception Vector Prefix Register).  In most cases,
	there is one exception at each 0x100 byte interval.  There are typically
	10-15 different exceptions.  If flash is used to overlay this vector area,
	then there is potentially 10-15 times 0x100 bytes of flash space needed.

	To save on flash, the vector table is placed in RAM at runtime by copying
	the "general_vector" code to each of the vectors.  This requires 
	at least 28 bytes of memory space for the vector handler if we use
	a register to conveniently identify the exception that occurred before
	we jump to the common exception code.  
	For the cases of the timer exceptions there is only 16 bytes of space
	between each handler, so we don't use the general_exception handler here.

	The general philosophy here is that the exception handler pushes the
	content of r29 onto the stack, then uses r29 to deal with the exception
	handling code.  At the point where the actual content of r29 is to be
	stored away to the register table (regtbl), it is pulled off the stack.
*/

#include "arch_ppc.h"
#include "cpu_403.h"

	.file	"vectors_403.s"
	.globl	saveregs
	.globl	pit_vector
	.globl	fit_vector
	.globl	wdt_vector
	.globl	end_wdt_vector
	.globl	general_vector
	.globl	end_general_vector
	.extern	exception
	.extern	regtbl

	.text
	.align	4

/* saveregs:
	Used by exception handlers.
*/
saveregs:
	lis		r29,(regtbl)@ha
	addi	r29,r29,(regtbl)@l
	stw	r0,0(r29)
	stw	r1,4(r29)
	stw	r2,8(r29)
	stw	r3,12(r29)
	stw	r4,16(r29)
	stw	r5,20(r29)
	stw	r6,24(r29)
	stw	r7,28(r29)
	stw	r8,32(r29)
	stw	r9,36(r29)
	stw	r10,40(r29)
	stw	r11,44(r29)
	stw	r12,48(r29)
	stw	r13,52(r29)
	stw	r14,56(r29)
	stw	r15,60(r29)
	stw	r16,64(r29)
	stw	r17,68(r29)
	stw	r18,72(r29)
	stw	r19,76(r29)
	stw	r20,80(r29)
	stw	r21,84(r29)
	stw	r22,88(r29)
	stw	r23,92(r29)
	stw	r24,96(r29)
	stw	r25,100(r29)
	stw	r26,104(r29)
	stw	r27,108(r29)
	stw	r28,112(r29)
	lwz r3,-4(sp)			/* R29 was pushed onto the stack */
	stw	r3,116(r29)
	stw	r30,120(r29)
	stw	r31,124(r29)
	li	r0,0				/* To clear syndrom regs. */
	mfmsr	r3
	stw		r3,128(r29)
	mfspr	r3, 26			/* SRR0 */
	stw		r3,132(r29)
	mfspr	r3, 27			/* SRR1 */
	stw		r3,136(r29)
	mfcr	r3				/* CR */
	stw		r3,140(r29)
	mfspr	r3, 990			/* SRR2 */
	stw		r3,144(r29)
	mfspr	r3, 991			/* SRR3 */
	stw		r3,148(r29)
	mfdcr	r3, bear
	stw		r3,152(r29)
	mfspr	r3, dear
	stw		r3,156(r29)
	mfdcr	r3, besr
	stw		r3,160(r29)
	mtdcr	besr, r0		/* clear besr */
	mfspr	r3, esr
	stw		r3,164(r29)
	mtspr	esr, r0			/* clear esr */
	mfdcr	r3, exisr
	stw		r3,168(r29)
	mtdcr	exisr, r3		/* clear bits that were set */
	mfspr	r3, tsr
	stw		r3,172(r29)
	mtspr	tsr, r3			/* clear bits that were set */
	mfspr	r3, sprg0
	stw		r3,176(r29)
	mfspr	r3, sprg1
	stw		r3,180(r29)
	mfspr	r3, sprg2
	stw		r3,184(r29)
	mfspr	r3, sprg3
	stw		r3,188(r29)
	mfspr	r3, lr
	stw		r3,192(r29)

	lwz		r3,-8(sp)		/* parameter to exception (type) */
	lis		r27,(exception)@ha
	addi	r27,r27,(exception)@l
	mtctr	r27
	bctr

pit_exception:
	li		r29,0x1000
	stw		r29,-8(sp)
	lis		r29,(saveregs)@ha
	addi	r29,r29,(saveregs)@l
	mtctr	r29
	bctr

fit_exception:
	li		r29,0x1010
	stw		r29,-8(sp)
	lis		r29,(saveregs)@ha
	addi	r29,r29,(saveregs)@l
	mtctr	r29
	bctr

wdt_exception:
	li		r29,0x1020
	stw		r29,-8(sp)
	lis		r29,(saveregs)@ha
	addi	r29,r29,(saveregs)@l
	mtctr	r29
	bctr

/* These functions are copied into the vector table DRAM by vinit():
 * Multiple copies of general_vector are made, with the value loaded into
 * r29 being modified for each vector. Refer to except_403.c, function
 * copyGeneralVector() for details.
 * NOTE:
 * If this general_vector handler is changed such that the position of
 * the 0x1234 value is moved relative to the start of the handler, then
 * the definition of GV_ID_OFFSET used by copyGeneralVector() must be
 * adjusted as well.
 */
general_vector:
	stw		r29,-4(sp)				/* Save R29 on stack */
	li		r29,0x1234		/* '0x1234' is modified by copyGeneralVector(). */
	stw		r29,-8(sp)				/* Save exception ID on stack */
	lis		r29,(saveregs)@ha		/* Jump to saveregs portion of handler */
	addi	r29,r29,(saveregs)@l
	mtctr	r29
	bctr
end_general_vector:

/* Note that PIT and FIT exception locations only have room for 4
 * instructions. 
 */
pit_vector:
	stw		r29,-4(sp)				/* Save R29 on stack */
	li		r29,0x1000
	stw		r29,-8(sp)				/* Store exception id on stack. */
	b		wdt_done

fit_vector:
	stw		r29,-4(sp)	
	li		r29,0x1010	
	stw		r29,-8(sp)	
	b		wdt_done

wdt_vector:
	stw		r29,-4(sp)	
	li		r29,0x1020	
	stw		r29,-8(sp)	
wdt_done:
	lis		r29,(saveregs)@ha		/* Jump to saveregs portion of handler */
	addi	r29,r29,(saveregs)@l
	mtctr	r29
	bctr
end_wdt_vector:
