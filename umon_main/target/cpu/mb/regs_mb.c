/* The MicroBlaze EABI convention is as follows:
 *
 * R0:		Always reads zero (HW)
 * R1:		Stack pointer (SW)
 * R2:		Read-only small-data area anchor (SW)
 * R3-R4:	Return values/temporaries (SW)
 * R5-R10:	Passing parameters/temporaries (SW)
 * R11-R12:	Temporaries (SW)
 * R13:		Read/write small-data area anchor (SW)
 * R14:		Return address for interrupt (HW)
 * R15:		Return address for subroutine (SW)
 * R16:		Return address for trap (HW)
 * R17:		Return address for exceptions
 * R18:		Reserved for assembler (SW)
 * R19-R31:	Must be saved across function calls (callee-save)
 *
 * MSR:		Machine Status Register
 * EAR:		Exception Address Register
 * ESR:		Exception Status Register
 * BTR:		Branch Target Register
 */

static char  *regnames[] = {
	"R0",		"R1",		"R2",		"R3",
	"R4",		"R5",		"R6",		"R7",
	"R8",		"R9",		"R10",		"R11",
	"R12",		"R13",		"R14",		"R15",
	"R16",		"R17",		"R18",		"R19",
	"R20",		"R21",		"R22",		"R23",
	"R24",		"R25",		"R26",		"R27",
	"R28",		"R29",		"R30",		"R31",
	"PC",		"MSR",		"EAR",		"ESR",
	"BTR",		"FSR"
};
