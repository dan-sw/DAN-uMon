/* vectors_sh3.c
 *	Here is the ASM code needed for the SH3 exception handling. 
 *	Including the vector table, a "gotoexception" function, used for saving the
 *	CPU context before jumping to the c-code exception handling and a "resume"
 *	function, currently unused.
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
 *	Author:	Torsten Tetz, Bosch Security Systems
 *	        Mark Jonas, Bosch Security Systems, mark.jonas@de.bosch.com
 *	based on vectors_sh2.S file by Ed Sutter
 */

#include "config.h"

	.file	"vectors_sh3.s"

	.import	_regtbl
	.import	_exception
	.import	_ExceptionType

	.globl	_resume
	.globl	_gotoexception

	.text

/*****************************************************************************
 * 
 * gotoexception:
 * When this code is executed as part of the exception processing,
 * it is assumed that R0 contains the vector number and the actual
 * value that was in R0 prior to the exception has been pushed onto
 * the stack.  
 * The general exception handling process within the monitor is that any
 * exception taken will cause some kind of entry into the monitor code
 * with the array regtbl[] filled with the CPU context at the time of the
 * exception.  When the monitor determines (either internally or through a
 * command issued by the user) that it is to return control to the application,
 * it calls resume() to load all of the registers with the values stored in
 * regtbl[].
 */
gotoexception:
	mov.l	p_exceptionType,r1	/* Put vector number */
	mov.l	r0,@r1			/* into the ExceptionCause variable. */

	mov.l	p_regtbl,r1		/* Set R1 to point to the register store table. */
	
	stc	r0_BANK,r2		/* Save R0 */
	mov.l	r2,@(0,r1)	
	stc	r1_BANK,r2		/* Save R1 */
	mov.l	r2,@(4,r1)
	stc	r2_BANK,r2		/* Save R2 */
	mov.l	r2,@(8,r1)
	stc	r3_BANK,r2		/* Save R3 */
	mov.l	r2,@(12,r1)
	stc	r4_BANK,r2		/* Save R4 */
	mov.l	r2,@(16,r1)
	stc	r5_BANK,r2		/* Save R5 */
	mov.l	r2,@(20,r1)
	stc	r6_BANK,r2		/* Save R6 */
	mov.l	r2,@(24,r1)
	stc	r7_BANK,r2		/* Save R7 */
	mov.l	r2,@(28,r1)

	mov.l	r8,@(32,r1)		/* Save R8 */
	mov.l	r9,@(36,r1)		/* Save R9 */
	mov.l	r10,@(40,r1)		/* Save R10 */
	mov.l	r11,@(44,r1)		/* Save R11 */
	mov.l	r12,@(48,r1)		/* Save R12 */
	mov.l	r13,@(52,r1)		/* Save R13 */
	mov.l	r14,@(56,r1)		/* Save R14 */
	mov.l	r15,@(60,r1)		/* Save R15 */

	add	#64,r1

	stc	spc,r2			/* Save PC */
	mov.l	r2,@(0,r1)

	stc	ssr,r2			/* Save SR */
	mov.l	r2,@(4,r1)

	sts	pr,r2			/* Save PR */
	mov.l	r2,@(8,r1)

	mov.l	p_exception,r1		/* Jump to C-level exception handler. */
	jmp	@r1
	nop
	
/*****************************************************************************
 *
 * resume:
 * This function basically does just the opposite of what gotoexception does.
 * It loads the registers R8 to R15 with what has been loaded into the regtbl[]
 * array and does an rte to load the PC and SR from SPC and SSR.
 * The registers R0 to R7 don’t need to be loaded. The SH3 has 2 banks of 
 * R0-R7 registers and from the moment of an exception the other bank is 
 * used till a resume with the "rte" ASM command occurs. The command "stc Rx_BANK,Rx"
 * need to be used to read the original values of R0 to R7 during exception handling.
 */
_resume:
	mov.l	p_regtbl,r0		/* Load R0 with regtbl[PR]. */
	add	#72,r0		
		
	mov.l	@r0,r0			/* Restore PR */
	lds	r0,pr

	mov.l	p_regtbl,r0		/* Load R0 with pointer regtbl[]. */

	mov.l	@(32,r0),r8		/* Restore R8 */
	mov.l	@(36,r0),r9		/* Restore R9 */
	mov.l	@(40,r0),r10		/* Restore R10 */
	mov.l	@(44,r0),r11		/* Restore R11 */
	mov.l	@(48,r0),r12		/* Restore R12 */
	mov.l	@(52,r0),r13		/* Restore R13 */
	mov.l	@(56,r0),r14		/* Restore R14 */
	mov.l	@(60,r0),r15		/* Restore R15 */
	rte
	nop

	.align 4

p_regtbl:
	.long	_regtbl

p_exception:
	.long	_exception

p_exceptionType:
	.long	_ExceptionType

/***************************************************************************
 *
 * Vector[0x000-0x600]:
 * At reset, the VBR points to address 0.  
 * The function vinit() (C code) sets the VBR to the adrress of the VectorTable, here.
 * Thus, when an exeption occurs, the exception handlers can be jump to trough their
 * offsets.
 * They are all identical, except that they load R0 with the vector number.
 */
	.align 2

	.globl _VectorTable		/* Position H00000000 */
_VectorTable:

	/* Maintain the 0x100 offset for Vector1: */
	.space 0x100

	.globl _Vector1
_Vector100:				/* Position H00000100 */
	bra	gotoexception		/* Branch to the main exception handler with the */
	mov	#1,r0			/* delay slot loading R0 with the vector number. */
_Vector100End:

	/* Maintain the 0x400 offset for Vector2: */
	.space (0x400 - (_Vector100End - _VectorTable))

	.globl _Vector2			/* Position H00000400 */
_Vector400:
	bra	gotoexception
	mov	#2,r0
_Vector400End:

	/* Maintain the 0x600 offset for Vector2: */
	.space (0x600 - (_Vector400End - _VectorTable))

	.globl _Vector3			/* Position H00000600 */
_Vector600:
	bra	gotoexception
	mov	#3,r0

