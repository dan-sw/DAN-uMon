/* regs_mips.c:
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
static char  *regnames[] = {
	"R0",			"R1/AT",		"R2/V0",		"R3/V1",
	"R4/A0",		"R5/A1",		"R6/A2",		"R7/A3",
	"R8/T0",		"R9/T1",		"R10/T2",		"R11/T3",
	"R12/T4",		"R13/T5",		"R14/T6",		"R15/T7",
	"R16/S0",		"R17/S1",		"R18/S2",		"R19/S3",
	"R20/S4",		"R21/S5",		"R22/S6",		"R23/S7",
	"R24/T8",		"R25/T9",		"R26/K0",		"R27/K1",
	"R28/GP",		"R29/SP",		"R30/S8",		"R31/RA",
};
