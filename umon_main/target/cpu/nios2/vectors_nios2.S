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
 */

	.extern hal_vsr_table

    .text

    .set noat

/*********************************************************************
 *
 * _interrupt_handler
 *
 * This is the default handler for hardware interrupts.
 */

    .section .text

    .globl _hwi_int_handler
    .type  _hwi_int_handler, @function
    
_hwi_int_handler:

    /*
     * Process an external hardware interrupt.
     *
     * First, save all registers in the rgetbl
     *
     */
     
    movhi r24,     %hiadj(regtbl)
    addi  r24, r24,   %lo(regtbl)

    stw     r0,  0x00(r24)
    stw     r1,  0x04(r24)
    stw     r2,  0x08(r24)
    stw     r3,  0x0C(r24)
    stw     r4,  0x10(r24)
    stw     r5,  0x14(r24)
    stw     r6,  0x18(r24)
    stw     r7,  0x1C(r24)
    stw     r8,  0x20(r24)
    stw     r9,  0x24(r24)
    stw     r10, 0x28(r24)
    stw     r11, 0x2C(r24)
    stw     r12, 0x30(r24)
    stw     r13, 0x34(r24)
    stw     r14, 0x38(r24)
    stw     r15, 0x3C(r24)
    stw     r16, 0x40(r24)
    stw     r17, 0x44(r24)
    stw     r18, 0x48(r24)
    stw     r19, 0x4C(r24)
    stw     r20, 0x50(r24)
    stw     r21, 0x54(r24)
    stw     r22, 0x58(r24)
    stw     r23, 0x5C(r24)
    stw     r24, 0x60(r24)
//  stw     r25, 0x64(r24) not available in user or super modes
    stw     r26, 0x68(r24)
    stw     r27, 0x6C(r24)
    stw     r28, 0x70(r24)
    stw     r29, 0x74(r24)
//  stw     r30, 0x78(r24) not available in user or super modes
    stw     r31, 0x7C(r24)

    // use r4 as a temp
    rdctl   r4,  ctl0
    stw     r4,  0x80(r24)
    rdctl   r4,  ctl1
    stw     r4,  0x84(r24)
    rdctl   r4,  ctl2
    stw     r4,  0x88(r24)
    rdctl   r4,  ctl3
    stw     r4,  0x8C(r24)
    rdctl   r4,  ctl4
    stw     r4,  0x90(r24)


    // all register are stored in the register table

    // determine thesource of the interrupt

    /*
     * Obtain a bitlist of the pending interrupts.
     */

    rdctl r24, ipending
    mov    r4, zero


    /*
     * Search through the bit list stored in r24 to find the first enabled
     * bit. The offset of this bit is the index of the interrupt that is
     * to be handled.
     */

6:
    andi r3, r24, 1
    bne r3, zero, 7f
    addi r4, r4, 1
    srli r24, r24, 1
    br 6b

7:
    /*
     * Having located the interrupt source, r4 contains the index of the
     * interrupt to be handled.
     *
     * Load the address at the time of the exception in to R5
     */

    mov  r5, r4
    mov  r4, ea

    call umon_hwi_handler

    // don't return


    /*
     * ========================================================================
     * _software_exception_handler
     *
     * This is the default handler for software exceptions.
     */

    .globl _swi_int_handler
	.type  _swi_int_handler, @function

_swi_int_handler:

	/*
     * First, save all registers in the rgetbl
     *
     */

    movhi r24,     %hiadj(regtbl)
    addi  r24, r24,   %lo(regtbl)

    stw     r0,  0x00(r24)
    stw     r1,  0x04(r24)
    stw     r2,  0x08(r24)
    stw     r3,  0x0C(r24)
    stw     r4,  0x10(r24)
    stw     r5,  0x14(r24)
    stw     r6,  0x18(r24)
    stw     r7,  0x1C(r24)
    stw     r8,  0x20(r24)
    stw     r9,  0x24(r24)
    stw     r10, 0x28(r24)
    stw     r11, 0x2C(r24)
    stw     r12, 0x30(r24)
    stw     r13, 0x34(r24)
    stw     r14, 0x38(r24)
    stw     r15, 0x3C(r24)
    stw     r16, 0x40(r24)
    stw     r17, 0x44(r24)
    stw     r18, 0x48(r24)
    stw     r19, 0x4C(r24)
    stw     r20, 0x50(r24)
    stw     r21, 0x54(r24)
    stw     r22, 0x58(r24)
    stw     r23, 0x5C(r24)
    stw     r24, 0x60(r24)
//  stw     r25, 0x64(r24) not available in user or super modes
    stw     r26, 0x68(r24)
    stw     r27, 0x6C(r24)
    stw     r28, 0x70(r24)
    stw     r29, 0x74(r24)
//  stw     r30, 0x78(r24) not available in user or super modes
    stw     r31, 0x7C(r24)

    // use r4 as a temp
    rdctl   r4,  ctl0
    stw     r4,  0x80(r24)
    rdctl   r4,  ctl1
    stw     r4,  0x84(r24)
    rdctl   r4,  ctl2
    stw     r4,  0x88(r24)
    rdctl   r4,  ctl3
    stw     r4,  0x8C(r24)
    rdctl   r4,  ctl4
    stw     r4,  0x90(r24)


    mov  r4, ea

    call umon_swi_handler
