#define CPU_PC_REG	"SRR0"

/* List of the register names for the PowerPC in the order they
 * are expected by the GDB 'g' command (read all registers).
 */
static char  *gdb_regtbl[] = {
	"R0",	"R1",	"R2",	"R3", "R4", "R5", "R6", "R7",
	"R8",	"R9",	"R10",	"R11", "R12", "R13", "R14", "R15",
	"R16",	"R17",	"R18",	"R19", "R20", "R21", "R22", "R23",
	"R24",	"R25",	"R26",	"R27", "R28", "R29", "R30", "R31",
	0,		"MSR",	"CR",	"LR", 
#if 0	/* Need to add these for GDB... */
	"CTR", "XER", "FPCSR", "VCSR", "VRSAVE",
#else	/* Fake it for now... */
	"SPRG0", "SPRG0", "SPRG0", "SPRG0", "SPRG0",
#endif
	0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	"SRR0"
};
