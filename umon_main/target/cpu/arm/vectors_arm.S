/*
 * General notice:
 * This code is part of a boot-monitor package developed as a generic base
 * platform for embedded system designs.  As such, it is likely to be
 * distributed to various projects beyond the control of the original
 * author.  Please notify the author of any enhancements made or bugs found
 * so that all may benefit from the changes.  In addition, notification back
 * to the author will allow the new user to pick up changes that may have
 * been made by other users after this version of the code was distributed.
 *
 * Author:  Ed Sutter
 * email:   esutter@lucent.com
 * phone:   908-582-2351 
 * 
 * The original version of this code was contributed by Nick Patavalis
 * (npat@inaccessnetworks.com). 
 *
 * All exception handlers are dealt with the same way...
 * By default the monitor will just catch the exception and return to
 * the uMON> prompt with the register cache (dumped via the 'reg' command)
 * containing all pertinent registers.
 * The monitor api provides the option to override the default handler,
 * so if that is the case, then the user-provided 'C-level' function is
 * branched to after the registers are stored away to the stack.  When
 * the function returns, the register context is restored from the stack
 * the the exception returns to the point that caused the exception.
 *
 * NOTE1: This code is coordinated with the code in except_arm.c.
 * NOTE2: This is not the norm for MicroMonitor exception handlers.
 * Generally, there is no need for the monitor API to support the ability
 * to install an exception handler because the exception table is writeable;
 * however, in the case of the ARM architecture, the only way to make the
 * exception table writeable is to relocate it to RAM using the MMU.  This
 * whole exception handling process allows the user to avoid hacking with
 * the MMU.
 *
 * WRITEBACK NOTE:
 * This note refers to all the assembler lines  below that say
 * "see writeback note above"...
 *
 * In the initial version of this code there were several uses of the '!'
 * in the assembler to denote the base register writeback.  For example...
 *
 *				stmia r14!, {r0-r14}^
 *
 * Apparently this is bad because the writeback is unpredictable and as of
 * version 3.4 of the GCC tools, a warning was generated.  So, to clean this
 * up I replace all of the above line types with the following type...
 *
 *				stmia r14, {r0-r14}^
 *				add r14, r14, #(15*4)
 *
 * Notice that the writeback (i.e. '!') is removed and replaced with an
 * add instruction that increments the base register the same way the
 * writeback would have done it.  Note also that I'm no ARM guru, so I
 * -hope- this is a valid fix!
 */

#include "arm.h"

	.global not_used
	.global software_interrupt, not_assigned
	.global fast_interrupt_request, interrupt_request
	.global undefined_instruction, abort_prefetch, abort_data

abort_prefetch_hdlr:
	.long usr_handle_abt_prefetch

abort_data_hdlr:
	.long usr_handle_abt_data

undef_instr_hdlr:
	.long usr_handle_und_instr

swi_hdlr:
	.long usr_handle_swi
	
irq_hdlr:
	.long usr_handle_irq
	
firq_hdlr:
	.long usr_handle_firq
	
	.text

/**************************************************************************
 *
 * abort_data:
 * Exception handler for MMU data access violation.
 * This description applies to all exception handlers in this file.
 * Note: LR=r14, SP=r13
 *
 * Each exception is prepared to branch in one of two different directions:
 * 1. To the generic handler that comes with MicroMonitor.
 * 2. To a user-installed handler that is application specific.
 *
 * Here's the general procedure...
 * See if the function pointer applicable to this exception is non-zero.
 * If it is, then the user has installed an exception handler, so
 * save all registers on the stack and branch to the handler.
 * If it isn't, then store all registers to the regtbl[] array and
 * allow MicroMonitor to deal with the exception.
 */
abort_data:
	str r14, [sp, #-4]!			/* Push link-addr to this exception stack */
	str r3, [sp, #-4]!			/* Push R3 to stack */
	ldr r3, abort_data_hdlr		/* Load function pointer into R3 */
	ldr	r3, [r3]				/* and test for non-zero. */
	cmp	r3, #0		
	ldr r3, [sp], #4			/* Pop R3 */
	bne	abort_data_user			/* If non-zero, jump to installed handler */
								/* else, store registers to regtbl and jump */
								/* to uMon's default handler... */
	ldr r14, =regtbl			/* Load r14 with addr of regtbl */
	stmia r14, {r0-r14}^		/* Store usr regs to regtbl */
	add r14, r14, #(15*4)		/* See writeback note above */
	mov r0, r14					/* Move regtbl[pc] addr to r0 */
	sub r0, r0, #4				/* Decrement to regtbl[lr] */
	ldr r14, [sp], #4			/* Load link-addr from stack to r14 */
	str r14, [r0], #4			/* Save link-addr to regtbl[lr] */
	sub r14, r14, #LRADJ_ABORTD	/* R14 = address that caused exception */
	str r14, [r0], #4			/* Save link-addr to regtbl[pc] */
	mrs r1, spsr				/* Move SPSR value to r1 */
	str r1, [r0], #4			/* Save SPSR value to regtbl[cpsr] */
	mov r0, r14					/* Pass PC & exception type to uMon's */
	mov r1, #EXCTYPE_ABORTD		/* default exception handler. */				
	b umon_exception			/* This branch does not return */
						
abort_data_user:
	stmfd sp, {r0-r14}^			/* Push all usr regs to stack */
	add sp, sp, #(15*4)			/* See writeback note above */
	sub r0, r14, #LRADJ_ABORTD	/* Subtract offset adjustment */
	ldr r3, abort_data_hdlr		/* Load function pointer into R3 */
	ldr	r3, [r3]				/* and jump to it. */
	mov	lr, pc
	mov	pc, r3
	ldmfd sp, {r0-r14}^			/* Pop all usr regs from stack */
	add sp, sp, #(15*4)			/* See writeback note above */
	ldr r14, [sp], #4			/* Pop R14 & return from exception */
	subs pc, r14, #4			/* At the time of the exception, LR - 8 */	
								/* points to the instruction that caused */
								/* the exception, so LR - 4 is loaded */
								/* into the PC to return. */

/**************************************************************************
 *
 * abort_prefetch:
 * Exception handler for MMU instruction access violation.
 * See comments in abort_data above.
 */
abort_prefetch:
	str r14, [sp, #-4]!			/* Push link-addr to this exception stack */
	str r3, [sp, #-4]!			/* Push R3 to stack */
	ldr r3, abort_prefetch_hdlr	/* Load function pointer into R3 */
	ldr	r3, [r3]				/* and test for non-zero. */
	cmp	r3, #0		
	ldr r3, [sp], #4			/* Pop R3 */
	bne	abort_prefetch_user		/* If non-zero, jump to installed handler */
								/* else, store registers to regtbl and jump */
								/* to uMon's default handler... */
	ldr r14, =regtbl			/* Load r14 with addr of regtbl */
	stmia r14, {r0-r14}^		/* Store usr regs to regtbl */
	add r14, r14, #(15*4)			/* See writeback note above */
	mov r0, r14					/* Move regtbl[pc] addr to r0 */
	sub r0, r0, #4				/* Decrement to regtbl[lr] */
	ldr r14, [sp], #4			/* Load link-addr from stack to r14 */
	str r14, [r0], #4			/* Save link-addr to regtbl[lr] */
	sub r14, r14, #LRADJ_ABORTP	/* R14 = address that caused exception */
	str r14, [r0], #4			/* Save link-addr to regtbl[pc] */
	mrs r1, spsr				/* Move SPSR value to r1 */
	str r1, [r0], #4			/* Save SPSR value to regtbl[cpsr] */
	mov r0, r14					/* Pass PC & exception type to uMon's */
	mov r1, #EXCTYPE_ABORTP		/* default exception handler. */				
	b umon_exception			/* This branch does not return */
						
abort_prefetch_user:
	stmfd sp, {r0-r14}^			/* Push all usr regs to stack */
	add sp, sp, #(15*4)			/* See writeback note above */
	sub r0, r14, #LRADJ_ABORTP	/* Load R0 with addr that caused exception */
	ldr r3, abort_prefetch_hdlr	/* Load function pointer into R3 */
	ldr	r3, [r3]				/* and jump to it. */
	mov	lr, pc
	mov	pc, r3
	ldmfd sp, {r0-r14}^			/* Pop all usr regs from stack */
	add sp, sp, #(15*4)			/* See writeback note above */
	ldr r14, [sp], #4			/* Pop R14 & return from exception */
	movs pc, r14				/* At the time of the exception, LR - 4 */	
								/* points to the instruction that caused */
								/* the exception, so LR is loaded */
								/* into the PC to return. */

/**************************************************************************
 *
 * undefined_instruction:
 * Exception handler for fetching an invalid instruction.
 * See comments in abort_data above.
 */
undefined_instruction:
	str r14, [sp, #-4]!			/* Push link-addr to this exception stack */
	str r3, [sp, #-4]!			/* Push R3 to stack */
	ldr r3, undef_instr_hdlr	/* Load function pointer into R3 */
	ldr	r3, [r3]				/* and test for non-zero. */
	cmp	r3, #0		
	ldr r3, [sp], #4			/* Pop R3 */
	bne	undef_instr_user		/* If non-zero, jump to installed handler */
								/* else, store registers to regtbl and jump */
								/* to uMon's default handler... */
	ldr r14, =regtbl			/* Load r14 with addr of regtbl */
	stmia r14, {r0-r14}^		/* Store usr regs to regtbl */
	add r14, r14, #(15*4)		/* See writeback note above */
	mov r0, r14					/* Move regtbl[pc] addr to r0 */
	sub r0, r0, #4				/* Decrement to regtbl[lr] */
	ldr r14, [sp], #4			/* Load link-addr from stack to r14 */
	str r14, [r0], #4			/* Save link-addr to regtbl[lr] */
	sub r14, r14, #LRADJ_UNDEF	/* R14 = address that caused exception */
	str r14, [r0], #4			/* Save link-addr to regtbl[pc] */
	mrs r1, spsr				/* Move SPSR value to r1 */
	str r1, [r0], #4			/* Save SPSR value to regtbl[cpsr] */
	mov r0, r14					/* Pass PC & exception type to uMon's */
	mov r1, #EXCTYPE_UNDEF		/* default exception handler. */				
	b umon_exception			/* This branch does not return */
						
undef_instr_user:
	stmfd sp, {r0-r14}^			/* Push all usr regs to stack */
	add sp, sp, #(15*4)			/* See writeback note above */
	sub r0, r14, #LRADJ_UNDEF	/* Load R0 with addr that caused exception */
	ldr r3, undef_instr_hdlr	/* Load function pointer into R3 */
	ldr	r3, [r3]				/* and jump to it. */
	mov	lr, pc
	mov	pc, r3
	ldmfd sp, {r0-r14}^			/* Pop all usr regs from stack */
	add sp, sp, #(15*4)			/* See writeback note above */
	ldr r14, [sp], #4			/* Pop R14 & return from exception */
	movs pc, r14				/* At the time of the exception, LR */	
								/* points to the next instruction after */
								/* the undefined instruction, so LR is */
								/* loaded into the PC to return. */

/**************************************************************************
 *
 * software_interrupt:
 * Exception handler for software interrupt.
 * See comments in abort_data above.
 */
software_interrupt:
	str r14, [sp, #-4]!			/* Push link-addr to this exception stack */
	str r3, [sp, #-4]!			/* Push R3 to stack */
	ldr r3, swi_hdlr			/* Load function pointer into R3 */
	ldr	r3, [r3]				/* and test for non-zero. */
	cmp	r3, #0		
	ldr r3, [sp], #4			/* Pop R3 */
	bne	software_int_user		/* If non-zero, jump to installed handler */
								/* else, store registers to regtbl and jump */
								/* to uMon's default handler... */
	ldr r14, =regtbl			/* Load r14 with addr of regtbl */
	stmia r14, {r0-r14}			/* Store usr regs to regtbl */
	add r14, r14, #(15*4)		/* See writeback note above */
	mov r0, r14					/* Move regtbl[pc] addr to r0 */
	sub r0, r0, #4				/* Decrement to regtbl[lr] */
	ldr r14, [sp], #4			/* Load link-addr from stack to r14 */
	str r14, [r0], #4			/* Save link-addr to regtbl[lr] */
	mov r13, r14
	sub r14, r14, #LRADJ_SWI	/* R14 = address that caused exception */
	str r14, [r0], #4			/* Save link-addr to regtbl[pc] */
	mrs r1, spsr				/* Move SPSR value to r1 */
	str r1, [r0], #4			/* Save SPSR value to regtbl[cpsr] */
	mov r0, r14					/* Pass PC & SWI number to uMon's */
	ldr r1, [r13, #-4]			/* swi handler. */
	bic r1, r1, #0xff000000
	bl umon_swi_handler			/* This branch does not return */
						
software_int_user:
	stmfd sp, {r0-r14}			/* Push all usr regs to stack */
	add sp, sp, #(15*4)			/* See writeback note above */
	sub r0, r14, #LRADJ_SWI		/* Load R0 with address of SWI and */
	ldr r1, [r14, #-4]			/* R1 with the SWI number. */
	bic r1, r1, #0xff000000
	ldr r3, swi_hdlr			/* Load function pointer into R3 */
	ldr	r3, [r3]				/* and jump to it. */
	mov	lr, pc
	mov	pc, r3
	ldmfd sp, {r0-r14}			/* Pop all usr regs from stack */
	add sp, sp, #(15*4)			/* See writeback note above */
	ldr r14, [sp], #4			/* Pop R14 & return from exception */
	movs pc, r14				/* At the time of the exception, LR */	
								/* points to the next instruction after */
								/* the SWI instruction, so LR is */
								/* loaded into the PC to return. */

/**************************************************************************
 *
 * interrupt_request:
 * Exception handler for interrupt requests.
 * See comments in abort_data above.
 */
interrupt_request:
	str r14, [sp, #-4]!			/* Push link-addr to this exception stack */
	str r3, [sp, #-4]!			/* Push R3 to stack */
	ldr r3, irq_hdlr			/* Load function pointer into R3 */
	ldr	r3, [r3]				/* and test for non-zero. */
	cmp	r3, #0		
	ldr r3, [sp], #4			/* Pop R3 */
	bne	int_request_user		/* If non-zero, jump to installed handler */
								/* else, store registers to regtbl and jump */
								/* to uMon's default handler... */
	ldr r14, =regtbl			/* Load r14 with addr of regtbl */
	stmia r14, {r0-r14}^		/* Store usr regs to regtbl */
	add r14, r14, #(15*4)		/* See writeback note above */
	mov r0, r14					/* Move regtbl[pc] addr to r0 */
	sub r0, r0, #4				/* Decrement to regtbl[lr] */
	ldr r14, [sp], #4			/* Load link-addr from stack to r14 */
	str r14, [r0], #4			/* Save link-addr to regtbl[lr] */
	sub r14, r14, #LRADJ_IRQ	/* R14 = address that caused exception */
	str r14, [r0], #4			/* Save link-addr to regtbl[pc] */
	mrs r1, spsr				/* Move SPSR value to r1 */
	str r1, [r0], #4			/* Save SPSR value to regtbl[cpsr] */
	mov r0, r14					/* Pass PC & exception type to uMon's */
	mov r1, #EXCTYPE_IRQ		/* default exception handler. */				
	b umon_exception			/* This branch does not return */
						
int_request_user:
	stmfd sp, {r0-r14}^			/* Push all usr regs to stack */
	add sp, sp, #(15*4)			/* See writeback note above */
	sub r0, r14, #8				/* Set R0 to addr that caused exception */
	ldr r3, irq_hdlr			/* Load function pointer into R3 */
	ldr	r3, [r3]				/* and jump to it. */
	mov	lr, pc
	mov	pc, r3
	ldmfd sp, {r0-r14}^			/* Pop all usr regs from stack */
	add sp, sp, #(15*4)			/* See writeback note above */
	ldr r14, [sp], #4			/* Pop R14 & return from exception */
	subs pc, r14, #4			/* At the time of the exception, LR - 4 */	
								/* points to the return address from the */
								/* IRQ handler, so LR - 4 is loaded */
								/* into the PC to return. */

/**************************************************************************
 *
 * fast_interrupt_request:
 * Exception handler for fast interrupt requests.
 * See comments in abort_data above.
 */
fast_interrupt_request:
	str r14, [sp, #-4]!			/* Push link-addr to this exception stack */
	str r3, [sp, #-4]!			/* Push R3 to stack */
	ldr r3, firq_hdlr			/* Load function pointer into R3 */
	ldr	r3, [r3]				/* and test for non-zero. */
	cmp	r3, #0		
	ldr r3, [sp], #4			/* Pop R3 */
	bne	fint_request_user		/* If non-zero, jump to installed handler */
								/* else, store registers to regtbl and jump */
								/* to uMon's default handler... */
	ldr r14, =regtbl			/* Load r14 with addr of regtbl */
	stmia r14, {r0-r14}^		/* Store usr regs to regtbl */
	add r14, r14, #(15*4)		/* See writeback note above */
	mov r0, r14					/* Move regtbl[pc] addr to r0 */
	sub r0, r0, #4				/* Decrement to regtbl[lr] */
	ldr r14, [sp], #4			/* Load link-addr from stack to r14 */
	str r14, [r0], #4			/* Save link-addr to regtbl[lr] */
	sub r14, r14, #LRADJ_FIRQ	/* R14 = address that caused exception */
	str r14, [r0], #4			/* Save link-addr to regtbl[pc] */
	mrs r1, spsr				/* Move SPSR value to r1 */
	str r1, [r0], #4			/* Save SPSR value to regtbl[cpsr] */
	mov r0, r14					/* Pass PC & exception type to uMon's */
	mov r1, #EXCTYPE_FIRQ		/* default exception handler. */				
	b umon_exception			/* This branch does not return */
						
fint_request_user:
	stmfd sp, {r0-r14}^			/* Push all usr regs to stack */
	add sp, sp, #(15*4)			/* See writeback note above */
	sub r0, r14, #8				/* Set R0 to addr that caused exception */
	ldr r3, firq_hdlr			/* Load function pointer into R3 */
	ldr	r3, [r3]				/* and jump to it. */
	mov	lr, pc
	mov	pc, r3
	ldmfd sp, {r0-r14}^			/* Pop all usr regs from stack */
	add sp, sp, #(15*4)			/* See writeback note above */
	ldr r14, [sp], #4			/* Pop R14 & return from exception */
	subs pc, r14, #4			/* At the time of the exception, LR - 4 */	
								/* points to the return address from the */
								/* FIRQ handler, so LR - 4 is loaded */
								/* into the PC to return. */

/**************************************************************************
 *
 * not_assigned:
 * Should never get here but, this is provided as way out for the
 * exception tablie entry that is not used.
 */
not_assigned:
	str r14, [sp, #-4]!			/* Push link-addr to this exception stack */
	ldr r14, =regtbl			/* Load r14 with addr of regtbl */
	stmia r14, {r0-r14}^		/* Store usr regs to regtbl */
	add r14, r14, #(15*4)		/* See writeback note above */
	mov r0, r14					/* Move regtbl[pc] addr to r0 */
	sub r0, r0, #4				/* Decrement to regtbl[lr] */
	ldr r14, [sp], #4			/* Load link-addr from stack to r14 */
	str r14, [r0], #4			/* Save link-addr to regtbl[lr] */
	str r14, [r0], #4			/* Save link-addr to regtbl[pc] */
	mrs r1, spsr				/* Move SPSR value to r1 */
	str r1, [r0], #4			/* Save SPSR value to regtbl[cpsr] */
	mov r0, r14					/* Pass PC & exception type to uMon's */
	mov r1, #EXCTYPE_NOTASSGN	/* default exception handler. */				
	b umon_exception			/* This branch does not return */
