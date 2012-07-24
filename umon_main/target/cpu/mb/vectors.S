/* vectors.s:
 */
	.global usr_exception_handler
	.global interrupt_handler
	.global break_handler
	.global hw_exception_handler
	.extern regtbl

#include "config.h"
#include "warmstart.h"
#include "cpu.h"

/* Microblaze has 4 main vectors all of which end up here.
 * The idea is to simply store all registers into the regtbl[] 
 * array prior to jumping to the C-level exception() function
 * in uMon.
 */
	.align 2
usr_exception_handler:
	swi		r6, r1, 0		/* Save R6 and exception type on stack */
	addik	r1, r1, -4		/* then branch to common handler */
	addik	r6, r0, EXCEPTION_USER
	swi		r6, r1, 0
	brai	common_handler

	.align 2
interrupt_handler:
	swi		r6, r1, 0
	addik	r1, r1, -4
	addik	r6, r0, EXCEPTION_INTERRUPT
	swi		r6, r1, 0
	brai	common_handler

	.align 2
break_handler:
	swi		r6, r1, 0
	addik	r1, r1, -4
	addik	r6, r0, EXCEPTION_BREAK
	swi		r6, r1, 0
	brai	common_handler

	.align 2
hw_exception_handler:
	swi		r6, r1, 0
	addik	r1, r1, -4
	addik	r6, r0, EXCEPTION_HARDWARE
	swi		r6, r1, 0
	brai	common_handler

common_handler:
	addik	r6, r0, regtbl	/* Use R6 as base pointer to regtbl[], and copy */
	swi		r0, r6, 0		/* all registers to their appropriate positions */ 
	swi		r2, r6, 8		/* in the regtlb[] array. */
	swi		r3, r6, 12
	swi		r4, r6, 16
	swi		r5, r6, 20		/* note: R1 & R6 is stored in regtbl below */
	swi		r7, r6, 28
	swi		r8, r6, 32
	swi		r9, r6, 36
	swi		r10, r6, 40
	swi		r11, r6, 44
	swi		r12, r6, 48
	swi		r13, r6, 52
	swi		r14, r6, 56
	swi		r15, r6, 60
	swi		r16, r6, 64
	swi		r17, r6, 68
	swi		r18, r6, 72
	swi		r19, r6, 76
	swi		r20, r6, 80
	swi		r21, r6, 84
	swi		r22, r6, 88
	swi		r23, r6, 92
	swi		r24, r6, 96
	swi		r25, r6, 100
	swi		r26, r6, 104
	swi		r27, r6, 108
	swi		r28, r6, 112
	swi		r29, r6, 116
	swi		r30, r6, 120
	swi		r31, r6, 124
							/* <-- Leave space in regtbl for the PC value. */
	mfs		r4, rmsr		/* It is loaded later (in C) from r14, r15, r16 */
	swi		r4, r6, 132		/* or r17 (depending on the execption taken) */
	mfs		r4, rear		/* (refer to except_mb.c). */
	swi		r4, r6, 136
	mfs		r4, resr
	swi		r4, r6, 140
	mfs		r4, rbtr
	swi		r4, r6, 144
	mfs		r4, rfsr
	swi		r4, r6, 148
	lwi		r5, r1, 0		/* Pull exception type off stack and put it in */
	addik	r1, r1, 4		/* R5 (to be passed to exception() below). */
	swi		r1, r6, 4		/* Also, with r1 (i.e. sp) incremented back to */ 
							/* what it was prior to the exception, save it */
							/* to regtbl[] as well. */
	lwi		r4, r1, 0		/* Pull the original value of R6 off the stack */
	swi		r4, r6, 24		/* and put it in regtbl[]. */
	brai	exception		/* Finally, branch to 'exception(type)' with  */
							/* R5 already loaded with the exception type. */
