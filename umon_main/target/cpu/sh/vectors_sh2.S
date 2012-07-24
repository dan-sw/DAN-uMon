	.file	"vectors.s"

/*	General notice:
 *	This code is part of a boot-monitor package developed as a generic base
 *	platform for embedded system designs.  As such, it is likely to be
 *	distributed to various projects beyond the control of the original
 *	author.  Please notify the author of any enhancements made or bugs found
 *	so that all may benefit from the changes.  In addition, notification back
 *	to the author will allow the new user to pick up changes that may have
 *	been made by other users after this version of the code was distributed.
 *
 *	Author:	Ed Sutter
 *	email:	esutter@lucent.com		(home: lesutter@worldnet.att.net)
 *	phone:	908-582-2351			(home: 908-889-5161)
 */


	.import	_regtbl
	.import	_exception
	.import	_ExceptionType
	.import	_MonStackEnd

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
	mov.l	r0,@-r15			/* Push R0 onto stack so that it can be used */
	mov.l	p_regtbl,r0			/* to point to the register store table. */
	mov.l	r1,@(4,r0)			/* Save R1 */
	mov.l	r2,@(8,r0)			/* Save R2 */
	mov.l	r3,@(12,r0)			/* Save R3 */
	mov.l	r4,@(16,r0)			/* Save R4 */
	mov.l	r5,@(20,r0)			/* Save R5 */
	mov.l	r6,@(24,r0)			/* Save R6 */
	mov.l	r7,@(28,r0)			/* Save R7 */
	mov.l	r8,@(32,r0)			/* Save R8 */
	mov.l	r9,@(36,r0)			/* Save R9 */
	mov.l	r10,@(40,r0)		/* Save R10 */
	mov.l	r11,@(44,r0)		/* Save R11 */
	mov.l	r12,@(48,r0)		/* Save R12 */
	mov.l	r13,@(52,r0)		/* Save R13 */
	mov.l	r14,@(56,r0)		/* Save R14 */

	mov.l	@r15+,r0			/* Pull vector number off stack and put it */
	mov.l	p_exceptionType,r1	/* into the ExceptionCause variable. */
	mov.l	r0,@r1			

	mov.l	@r15+,r0			/* Pull R0 off stack and now use R1 to point */
	mov.l	p_regtbl,r1			/* to register store table. */
	mov.l	r0,@r1				/* Save R0 */
	mov.l	r15,@(60,r1)		/* Save R15 */
	mov.l	@r15,r0				/* Pull PC off stack and put it in register */
	add		#64,r1				/* store table. */
	mov.l	r0,@r1
	mov.l	@(4,r15),r0			/* Pull SR off stack and put it in register */
	add		#4,r1				/* store table. */
	mov.l	r0,@r1
	add		#4,r1				/* Save PR */
	sts		pr,r8
	mov.l	r8,@r1
	mov.l	p_MonStackEnd,r8	/* Load the monitor's stack poniter. */
	mov.l	@r8,r15
	mov.l	p_exception,r8		/* Jump to C-level exception handler. */
	jmp		@r8
	nop

/*****************************************************************************
 *
 * resume:
 * This function basically does just the opposite of what gotoexception does.
 * It loads each of the registers with what has been loaded into the regtbl[]
 * array and does an rte to load the PC and SR.
 * NOT WORKING YET!!!
 */
_resume:
	mov.l	p_regtbl,r0			/* Load R0 with regtbl[PC]. */
	add		#64,r0			
	mov.l	@r0,r0
	mov.l	p_regtbl,r1			/* Load R1 with regtbl[SP]. */
	mov.l	@(60,r1),r1	
	mov.l	r0,@r1				/* Place the regtbl[PC] value at the location */
								/* pointed to be regtbl[SP]. */

	mov.l	p_regtbl,r0			/* Load R0 with regtbl[SR]. */
	add		#68,r0			
	mov.l	@r0,r0
	mov.l	r0,@(4,r1)			/* Place the regtbl[SR] value at the location */
								/* pointed to be regtbl[SP] + 4. */

	mov.l	p_regtbl,r0			/* Load R0 with regtbl[PR]. */
	add		#72,r0				/* Restore PR */
	mov.l	@r0,r0
	lds		r0,pr

	/* At this point, the new stack is prepared for the rte.
	 * Now just transfer all registers from regtbl[] to the actual registers...
	 */

	mov.l	p_regtbl,r0			/* Load R0 with pointer regtbl[]. */
	mov.l	@(4,r0),r1			/* Restore R1 */
	mov.l	@(8,r0),r2			/* Restore R2 */
	mov.l	@(12,r0),r3			/* Restore R3 */
	mov.l	@(16,r0),r4			/* Restore R4 */
	mov.l	@(20,r0),r5			/* Restore R5 */
	mov.l	@(24,r0),r6			/* Restore R6 */
	mov.l	@(28,r0),r7			/* Restore R7 */
	mov.l	@(32,r0),r8			/* Restore R8 */
	mov.l	@(36,r0),r9			/* Restore R9 */
	mov.l	@(40,r0),r10		/* Restore R10 */
	mov.l	@(44,r0),r11		/* Restore R11 */
	mov.l	@(48,r0),r12		/* Restore R12 */
	mov.l	@(52,r0),r13		/* Restore R13 */
	mov.l	@(56,r0),r14		/* Restore R14 */
	mov.l	@(60,r0),r15		/* Restore R15 */
	mov.l	@r0,r0				/* Restore R0 */
	rte
	nop

	.align 2

p_regtbl:
	.long	_regtbl

p_exception:
	.long	_exception

p_exceptionType:
	.long	_ExceptionType

p_MonStackEnd:
	.long	_MonStackEnd


/***************************************************************************
 *
 * Vector[4-255]:
 * At reset, the VBR points to address 0.  At this location there is a minimum
 * vector table containing the PC/SP for a manual and power-on reset.
 * The function vinit() (C code) re-establishes the VBR and loads one of these
 * exception handlers for each vector table entry.  They are all identical
 * except that after pushing R0 onto the stack, they load R0 with the 
 * vector number.
 */

	.globl _Vector4
_Vector4:
	mov.l	r0,@-r15		/* Push R0 onto the stack. */
	bra		gotoexception	/* Branch to the main exception handler with the */
	mov		#4,r0			/* delay slot loading R0 with the vector number. */

	.globl _Vector5
_Vector5:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#5,r0

	.globl _Vector6
_Vector6:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#6,r0

	.globl _Vector7
_Vector7:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#7,r0

	.globl _Vector8
_Vector8:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#8,r0

	.globl _Vector9
_Vector9:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#9,r0

	.globl _Vector10
_Vector10:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#10,r0

	.globl _Vector11
_Vector11:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#11,r0

	.globl _Vector12
_Vector12:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#12,r0

	.globl _Vector13
_Vector13:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#13,r0

	.globl _Vector14
_Vector14:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#14,r0

	.globl _Vector15
_Vector15:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#15,r0

	.globl _Vector16
_Vector16:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#16,r0

	.globl _Vector17
_Vector17:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#17,r0

	.globl _Vector18
_Vector18:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#18,r0

	.globl _Vector19
_Vector19:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#19,r0

	.globl _Vector20
_Vector20:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#20,r0

	.globl _Vector21
_Vector21:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#21,r0

	.globl _Vector22
_Vector22:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#22,r0

	.globl _Vector23
_Vector23:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#23,r0

	.globl _Vector24
_Vector24:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#24,r0

	.globl _Vector25
_Vector25:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#25,r0

	.globl _Vector26
_Vector26:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#26,r0

	.globl _Vector27
_Vector27:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#27,r0

	.globl _Vector28
_Vector28:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#28,r0

	.globl _Vector29
_Vector29:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#29,r0

	.globl _Vector30
_Vector30:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#30,r0

	.globl _Vector31
_Vector31:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#31,r0

	.globl _Vector32
_Vector32:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#32,r0

	.globl _Vector33
_Vector33:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#33,r0

	.globl _Vector34
_Vector34:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#34,r0

	.globl _Vector35
_Vector35:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#35,r0

	.globl _Vector36
_Vector36:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#36,r0

	.globl _Vector37
_Vector37:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#37,r0

	.globl _Vector38
_Vector38:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#38,r0

	.globl _Vector39
_Vector39:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#39,r0

	.globl _Vector40
_Vector40:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#40,r0

	.globl _Vector41
_Vector41:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#41,r0

	.globl _Vector42
_Vector42:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#42,r0

	.globl _Vector43
_Vector43:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#43,r0

	.globl _Vector44
_Vector44:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#44,r0

	.globl _Vector45
_Vector45:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#45,r0

	.globl _Vector46
_Vector46:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#46,r0

	.globl _Vector47
_Vector47:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#47,r0

	.globl _Vector48
_Vector48:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#48,r0

	.globl _Vector49
_Vector49:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#49,r0

	.globl _Vector50
_Vector50:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#50,r0

	.globl _Vector51
_Vector51:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#51,r0

	.globl _Vector52
_Vector52:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#52,r0

	.globl _Vector53
_Vector53:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#53,r0

	.globl _Vector54
_Vector54:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#54,r0

	.globl _Vector55
_Vector55:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#55,r0

	.globl _Vector56
_Vector56:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#56,r0

	.globl _Vector57
_Vector57:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#57,r0

	.globl _Vector58
_Vector58:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#58,r0

	.globl _Vector59
_Vector59:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#59,r0

	.globl _Vector60
_Vector60:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#60,r0

	.globl _Vector61
_Vector61:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#61,r0

	.globl _Vector62
_Vector62:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#62,r0

	.globl _Vector63
_Vector63:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#63,r0

	.globl _Vector64
_Vector64:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#64,r0

	.globl _Vector65
_Vector65:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#65,r0

	.globl _Vector66
_Vector66:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#66,r0

	.globl _Vector67
_Vector67:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#67,r0

	.globl _Vector68
_Vector68:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#68,r0

	.globl _Vector69
_Vector69:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#69,r0

	.globl _Vector70
_Vector70:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#70,r0

	.globl _Vector71
_Vector71:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#71,r0

	.globl _Vector72
_Vector72:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#72,r0

	.globl _Vector73
_Vector73:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#73,r0

	.globl _Vector74
_Vector74:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#74,r0

	.globl _Vector75
_Vector75:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#75,r0

	.globl _Vector76
_Vector76:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#76,r0

	.globl _Vector77
_Vector77:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#77,r0

	.globl _Vector78
_Vector78:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#78,r0

	.globl _Vector79
_Vector79:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#79,r0

	.globl _Vector80
_Vector80:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#80,r0

	.globl _Vector81
_Vector81:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#81,r0

	.globl _Vector82
_Vector82:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#82,r0

	.globl _Vector83
_Vector83:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#83,r0

	.globl _Vector84
_Vector84:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#84,r0

	.globl _Vector85
_Vector85:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#85,r0

	.globl _Vector86
_Vector86:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#86,r0

	.globl _Vector87
_Vector87:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#87,r0

	.globl _Vector88
_Vector88:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#88,r0

	.globl _Vector89
_Vector89:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#89,r0

	.globl _Vector90
_Vector90:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#90,r0

	.globl _Vector91
_Vector91:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#91,r0

	.globl _Vector92
_Vector92:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#92,r0

	.globl _Vector93
_Vector93:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#93,r0

	.globl _Vector94
_Vector94:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#94,r0

	.globl _Vector95
_Vector95:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#95,r0

	.globl _Vector96
_Vector96:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#96,r0

	.globl _Vector97
_Vector97:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#97,r0

	.globl _Vector98
_Vector98:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#98,r0

	.globl _Vector99
_Vector99:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#99,r0

	.globl _Vector100
_Vector100:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#100,r0

	.globl _Vector101
_Vector101:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#101,r0

	.globl _Vector102
_Vector102:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#102,r0

	.globl _Vector103
_Vector103:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#103,r0

	.globl _Vector104
_Vector104:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#104,r0

	.globl _Vector105
_Vector105:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#105,r0

	.globl _Vector106
_Vector106:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#106,r0

	.globl _Vector107
_Vector107:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#107,r0

	.globl _Vector108
_Vector108:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#108,r0

	.globl _Vector109
_Vector109:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#109,r0

	.globl _Vector110
_Vector110:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#110,r0

	.globl _Vector111
_Vector111:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#111,r0

	.globl _Vector112
_Vector112:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#112,r0

	.globl _Vector113
_Vector113:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#113,r0

	.globl _Vector114
_Vector114:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#114,r0

	.globl _Vector115
_Vector115:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#115,r0

	.globl _Vector116
_Vector116:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#116,r0

	.globl _Vector117
_Vector117:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#117,r0

	.globl _Vector118
_Vector118:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#118,r0

	.globl _Vector119
_Vector119:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#119,r0

	.globl _Vector120
_Vector120:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#120,r0

	.globl _Vector121
_Vector121:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#121,r0

	.globl _Vector122
_Vector122:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#122,r0

	.globl _Vector123
_Vector123:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#123,r0

	.globl _Vector124
_Vector124:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#124,r0

	.globl _Vector125
_Vector125:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#125,r0

	.globl _Vector126
_Vector126:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#126,r0

	.globl _Vector127
_Vector127:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#127,r0

	.globl _Vector128
_Vector128:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#128,r0

	.globl _Vector129
_Vector129:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#129,r0

	.globl _Vector130
_Vector130:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#130,r0

	.globl _Vector131
_Vector131:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#131,r0

	.globl _Vector132
_Vector132:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#132,r0

	.globl _Vector133
_Vector133:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#133,r0

	.globl _Vector134
_Vector134:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#134,r0

	.globl _Vector135
_Vector135:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#135,r0

	.globl _Vector136
_Vector136:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#136,r0

	.globl _Vector137
_Vector137:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#137,r0

	.globl _Vector138
_Vector138:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#138,r0

	.globl _Vector139
_Vector139:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#139,r0

	.globl _Vector140
_Vector140:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#140,r0

	.globl _Vector141
_Vector141:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#141,r0

	.globl _Vector142
_Vector142:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#142,r0

	.globl _Vector143
_Vector143:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#143,r0

	.globl _Vector144
_Vector144:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#144,r0

	.globl _Vector145
_Vector145:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#145,r0

	.globl _Vector146
_Vector146:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#146,r0

	.globl _Vector147
_Vector147:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#147,r0

	.globl _Vector148
_Vector148:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#148,r0

	.globl _Vector149
_Vector149:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#149,r0

	.globl _Vector150
_Vector150:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#150,r0

	.globl _Vector151
_Vector151:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#151,r0

	.globl _Vector152
_Vector152:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#152,r0

	.globl _Vector153
_Vector153:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#153,r0

	.globl _Vector154
_Vector154:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#154,r0

	.globl _Vector155
_Vector155:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#155,r0

	.globl _Vector156
_Vector156:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#156,r0

	.globl _Vector157
_Vector157:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#157,r0

	.globl _Vector158
_Vector158:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#158,r0

	.globl _Vector159
_Vector159:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#159,r0

	.globl _Vector160
_Vector160:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#160,r0

	.globl _Vector161
_Vector161:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#161,r0

	.globl _Vector162
_Vector162:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#162,r0

	.globl _Vector163
_Vector163:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#163,r0

	.globl _Vector164
_Vector164:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#164,r0

	.globl _Vector165
_Vector165:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#165,r0

	.globl _Vector166
_Vector166:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#166,r0

	.globl _Vector167
_Vector167:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#167,r0

	.globl _Vector168
_Vector168:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#168,r0

	.globl _Vector169
_Vector169:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#169,r0

	.globl _Vector170
_Vector170:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#170,r0

	.globl _Vector171
_Vector171:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#171,r0

	.globl _Vector172
_Vector172:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#172,r0

	.globl _Vector173
_Vector173:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#173,r0

	.globl _Vector174
_Vector174:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#174,r0

	.globl _Vector175
_Vector175:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#175,r0

	.globl _Vector176
_Vector176:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#176,r0

	.globl _Vector177
_Vector177:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#177,r0

	.globl _Vector178
_Vector178:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#178,r0

	.globl _Vector179
_Vector179:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#179,r0

	.globl _Vector180
_Vector180:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#180,r0

	.globl _Vector181
_Vector181:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#181,r0

	.globl _Vector182
_Vector182:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#182,r0

	.globl _Vector183
_Vector183:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#183,r0

	.globl _Vector184
_Vector184:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#184,r0

	.globl _Vector185
_Vector185:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#185,r0

	.globl _Vector186
_Vector186:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#186,r0

	.globl _Vector187
_Vector187:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#187,r0

	.globl _Vector188
_Vector188:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#188,r0

	.globl _Vector189
_Vector189:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#189,r0

	.globl _Vector190
_Vector190:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#190,r0

	.globl _Vector191
_Vector191:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#191,r0

	.globl _Vector192
_Vector192:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#192,r0

	.globl _Vector193
_Vector193:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#193,r0

	.globl _Vector194
_Vector194:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#194,r0

	.globl _Vector195
_Vector195:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#195,r0

	.globl _Vector196
_Vector196:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#196,r0

	.globl _Vector197
_Vector197:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#197,r0

	.globl _Vector198
_Vector198:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#198,r0

	.globl _Vector199
_Vector199:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#199,r0

	.globl _Vector200
_Vector200:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#200,r0

	.globl _Vector201
_Vector201:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#201,r0

	.globl _Vector202
_Vector202:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#202,r0

	.globl _Vector203
_Vector203:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#203,r0

	.globl _Vector204
_Vector204:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#204,r0

	.globl _Vector205
_Vector205:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#205,r0

	.globl _Vector206
_Vector206:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#206,r0

	.globl _Vector207
_Vector207:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#207,r0

	.globl _Vector208
_Vector208:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#208,r0

	.globl _Vector209
_Vector209:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#209,r0

	.globl _Vector210
_Vector210:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#210,r0

	.globl _Vector211
_Vector211:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#211,r0

	.globl _Vector212
_Vector212:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#212,r0

	.globl _Vector213
_Vector213:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#213,r0

	.globl _Vector214
_Vector214:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#214,r0

	.globl _Vector215
_Vector215:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#215,r0

	.globl _Vector216
_Vector216:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#216,r0

	.globl _Vector217
_Vector217:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#217,r0

	.globl _Vector218
_Vector218:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#218,r0

	.globl _Vector219
_Vector219:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#219,r0

	.globl _Vector220
_Vector220:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#220,r0

	.globl _Vector221
_Vector221:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#221,r0

	.globl _Vector222
_Vector222:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#222,r0

	.globl _Vector223
_Vector223:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#223,r0

	.globl _Vector224
_Vector224:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#224,r0

	.globl _Vector225
_Vector225:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#225,r0

	.globl _Vector226
_Vector226:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#226,r0

	.globl _Vector227
_Vector227:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#227,r0

	.globl _Vector228
_Vector228:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#228,r0

	.globl _Vector229
_Vector229:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#229,r0

	.globl _Vector230
_Vector230:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#230,r0

	.globl _Vector231
_Vector231:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#231,r0

	.globl _Vector232
_Vector232:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#232,r0

	.globl _Vector233
_Vector233:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#233,r0

	.globl _Vector234
_Vector234:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#234,r0

	.globl _Vector235
_Vector235:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#235,r0

	.globl _Vector236
_Vector236:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#236,r0

	.globl _Vector237
_Vector237:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#237,r0

	.globl _Vector238
_Vector238:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#238,r0

	.globl _Vector239
_Vector239:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#239,r0

	.globl _Vector240
_Vector240:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#240,r0

	.globl _Vector241
_Vector241:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#241,r0

	.globl _Vector242
_Vector242:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#242,r0

	.globl _Vector243
_Vector243:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#243,r0

	.globl _Vector244
_Vector244:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#244,r0

	.globl _Vector245
_Vector245:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#245,r0

	.globl _Vector246
_Vector246:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#246,r0

	.globl _Vector247
_Vector247:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#247,r0

	.globl _Vector248
_Vector248:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#248,r0

	.globl _Vector249
_Vector249:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#249,r0

	.globl _Vector250
_Vector250:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#250,r0

	.globl _Vector251
_Vector251:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#251,r0

	.globl _Vector252
_Vector252:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#252,r0

	.globl _Vector253
_Vector253:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#253,r0

	.globl _Vector254
_Vector254:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#254,r0

	.globl _Vector255
_Vector255:
	mov.l	r0,@-r15
	bra		gotoexception
	mov		#255,r0
