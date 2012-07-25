/*
All files except if stated otherwise in the begining of the file are under the ISC license:
-----------------------------------------------------------------------------------

Copyright (c) 2010-2012 Design Art Networks Ltd.

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

-----------------------------------------------------------------------------------
*/
/*
 *  armboot - Startup Code for ARM926EJS CPU-core
 *
 */

/*
 *************************************************************************
 *
 * Jump vector table
 *
 *************************************************************************
 */

.global _start
.type _start,function
.section .text.vector
_start:
    b	reset
    ldr	pc, _undefined_instruction
    ldr	pc, _software_interrupt
    ldr	pc, _prefetch_abort
    ldr	pc, _data_abort
    ldr	pc, _not_used
    ldr	pc, _irq
    ldr	pc, _fiq

_undefined_instruction:
    .word undefined_instruction
_software_interrupt:
    .word software_interrupt
_prefetch_abort:
    .word prefetch_abort
_data_abort:
    .word data_abort
_not_used:
    .word not_used
_irq:
    .word irq
_fiq:
    .word fiq

    .balignl 16,0xdeadbeef

/*
 *************************************************************************
 *
 * Startup Code (reset vector)
 *
 * do important init only if we don't start from memory!
 * setup Memory and board specific bits prior to relocation.
 * relocate armboot to ram
 * setup stack
 *
 *************************************************************************
 */

.global _end_of_image
_end_of_image:
    .word __image_end

.global _start_of_free_ram
_start_of_free_ram:
    .word _end

.global _bootload_start
_bootload_start:
    .word _start

/*
 * These are defined in the board-specific linker script.
 */

.global _bss_start
_bss_start:
    .word __bss_start

.global _bss_end
_bss_end:
    .word _end

/*
 * the actual reset code
 */

reset:
    /*
     * set the cpu to SVC32 mode
     */
    mrs	r0,cpsr
    bic	r0,r0,#0x1f
    orr	r0,r0,#0x13
    msr	cpsr,r0

    /* Set up the stack						    */
stack_setup:
    ldr	r0, =__bss_start	/* First 4K of SDRAM used as stack  */
    sub r0, r0, #4
    mov sp, r0

clear_bss:
    ldr	r0, _bss_start		/* find start of bss segment        */
    ldr	r1, _bss_end		/* stop here                        */
    mov r2, #0x00000000		/* clear                            */
    mov r3, #0x00000000		/* clear                            */
    mov r4, #0x00000000		/* clear                            */
    mov r5, #0x00000000		/* clear                            */
    mov r6, #0x00000000		/* clear                            */
    mov r7, #0x00000000		/* clear                            */
    mov r8, #0x00000000		/* clear                            */
    mov r9, #0x00000000		/* clear                            */

clbss_l:
	stmia	r0, {r2-r9}		/* clear loop...                    */
    add		r0, r0, #32
    cmp		r0, r1
    ble		clbss_l

    ldr		pc, _main

_main:
    .word umon_main

    .global _abort
    .global abort
abort:
_abort:
	ldr	r0, =0x6573E000			/* Reset register address                   */
	mov	r1, #0x00010000			/* Reset register value						*/
	str	r1, [r0]				/* HW Reboot...                             */
	ldr	r0, =0xFFFF0000			/* ROM boot entry point                     */
	blx	r0						/* SW Reboot...                             */

    .global _reboot
    .global reboot
reboot:
_reboot:
	ldr	r0, =0x2001D030			/* Reset register address                   */
	mov	r1, #0x00800000			/* Reset register value						*/
	str	r1, [r0]				/* HW Reboot...                             */
	ldr	r0, =0xFFFF0000			/* ROM boot entry point                     */
	blx	r0						/* SW Reboot...                             */

	.global cache_off_asm
cache_off_asm:
    /*
     * flush v4 I/D caches
     */
	mov	r0, #0
tci_loop:
	mrc	p15, 0, r15, c7, c14, 3	/* flush and invalidate D-cache             */
	bne tci_loop
	mcr p15, 0, r0, c7, c10, 4	/* drain the write buffer (memory barrier)  */
	mcr p15, 0, r0, c8, c7,  0	/* invalidate TLB                           */
    /*
     * Disable D-cache and I-cache
     */
	mcr	p15, 0, r0, c7, c5, 0	/* invalidate I-cache                       */
	mrc	p15, 0, r0, c1, c0, 0	/* read CP15 register 1 into r0             */
	bic	r0, r0, #0x00000004		/* Disable D cache                          */
	mcr	p15, 0, r0, c1, c0, 0	/* write CP15 register 1 back               */
	bic	r0, r0, #0x00001000		/* Disable I cache                          */
	mcr	p15, 0, r0, c1, c0, 0	/* write CP15 register 1 back               */
    /*
     * Disable MMU
     */
	mrc	p15, 0, r0, c1, c0, 0	/* read CP15 register 1 into r0             */
	bic	r0, r0, #0x00000001		/* Disable MMU                              */
	mcr	p15, 0, r0, c1, c0, 0	/* write CP15 register 1 back               */
	mcr p15, 0, r0, c8, c7,  0	/* invalidate TLB                           */
	mov	pc, lr					/* back to my caller                        */

	.global	core_mmu_init
core_mmu_init:
	mov	r3,	r0					/* The parameter is the Table address       */
    /*
     * Setup Domain AP
     */
	ldr	r1, =0xFFFFFFFF			/* Setup domain AP                          */
	mcr	p15, 0, r1, c3, c0, 0	/* write CP15 register 3                    */

	/* Do the MMU init stuff */
	ldr	r2, =0xFFF00000			/* r2 has the mask for address              */
	ldr	r1, =0x00000C16			/* r1 has the value to put in               */
	mov	r0, #0					/* prepare table index                      */
mmu_init_loop:
	bic	r1, r1, r2				/* Clean address bits                       */
	orr	r1, r1, r0, lsl #18		/* Set the current address                  */
	str	r1, [r3, r0]			/* Set the table entry                      */
	add	r0, r0, #4				/* increment the r0                         */
	cmp	r0,	#0x4000				/* check the end of the table               */
	blt	mmu_init_loop			/* Loop if needed                           */
	mcr	p15,0,r3,c2,c0,0		/* Set the physical address of the table    */

    /*
     * Enable MMU
     */
	mrc	p15, 0, r0, c1, c0, 0	/* read CP15 register 1 into r0             */
	bic	r0, r0, #0x00001000		/* reset bit 12 (I) ensure disable I-Cache  */
	bic	r0, r0, #0x00000004		/* reset bit 2 (C) ensure disable D-Cache   */
	orr	r0, r0, #0x00000001		/* set bit 0 (M) to enable MMU              */
	mcr	p15, 0, r0, c1, c0, 0	/* write CP15 register 1 back               */

	mov	r0, r3					/* prepare translation table as return value*/
    mov	pc, lr					/* back to my caller                        */

	.global	cache_on_asm
cache_on_asm:
	mrc	p15, 0, r0, c1, c0, 0	/* read CP15 register 1 into r0             */
    /*
     * Enable I-cache
     */
	orr	r0, r0, #0x00001000		/* set bit 12 (I) I-Cache                   */
	mcr	p15, 0, r0, c1, c0, 0	/* write CP15 register 1                    */
    /*
     * Enable D-cache
     */
	orr	r0, r0, #0x00000004		/* set bit 2 (C) to enable D-Cache          */
	mcr	p15, 0, r0, c1, c0, 0	/* write CP15 register 1                    */

    mov	pc, lr					/* back to my caller                        */


/*
 *************************************************************************
 *
 * Interrupt handling
 *
 *************************************************************************
 */

/*
 * exception handlers
 */
    .align  5
undefined_instruction:
    movs	pc, lr					/* back to my caller                   */
    .align	5
software_interrupt:
    movs	pc, lr					/* back to my caller                   */
    .align	5
prefetch_abort:
    movs	pc, lr					/* back to my caller                   */
    .align	5
data_abort:
    movs	pc, lr					/* back to my caller                   */
    .align	5
not_used:
    movs	pc, lr					/* back to my caller                   */
    .align	5
irq:
    movs	pc, lr					/* back to my caller                   */
    .align	5
fiq:
    movs	pc, lr					/* back to my caller                   */
