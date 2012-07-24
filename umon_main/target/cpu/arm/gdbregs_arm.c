#define CPU_PC_REG	"PC"

/* List of the register names for the ARM in the order they
 * are expected by the GDB 'g' command (read all registers).
 * R13=SP, R14=LR
 */
static char  *gdb_regtbl[] = {
	"R0",	"R1",	"R2",	"R3", "R4", "R5", "R6", "R7",
	"R8",	"R9",	"R10",	"R11", "R12", "R13", "R14", "PC",
	"R0",	"CPSR",	
};
