/* Not all CPUs refer to the register pointing to the next instruction
 * address as "PC", so this definition allows it to be cpu specific...
 */
#define CPU_PC_REG	"PC"

/* List of the register names for the ColdFire in the order they
 * are expected by the GDB 'g' command (read all registers).
 * Not sure why I need to pad the end of the table, but GDB appears
 * to want to see more registers than are actually in the CPU.
 */
static char *gdb_regtbl[] = {
	"r0",			"r1/at",		"r2/v0",		"r3/v1",
	"r4/a0",		"r5/a1",		"r6/a2",		"r7/a3",
	"r8/t0",		"r9/t1",		"r10/t2",		"r11/t3",
	"r12/t4",		"r13/t5",		"r14/t6",		"r15/t7",
	"r16/s0",		"r17/s1",		"r18/s2",		"r19/s3",
	"r20/s4",		"r21/s5",		"r22/s6",		"r23/s7",
	"r24/t8",		"r25/t9",		"r26/k0",		"r27/k1",
	"r28/gp",		"r29/sp",		"r30/s8",		"r31/ra",
#if 0
	"sr","lo","hi","bad","cause","pc",
	"fp00","fp01","fp02","fp03","fp04","fp05","fp06","fp07",
	"fp08","fp09","fp10","fp11","fp12","fp13","fp14","fp15",
	"fp16","fp17","fp18","fp19","fp20","fp21","fp22","fp23",
	"fp24","fp25","fp26","fp27","fp28","fp29","fp30","fp31",
	"fsr","fir","fp"
#else	/* Gotta fix this ... */
	"r28/gp",		"r29/sp",		"r30/s8",		"r31/ra",
	"r28/gp",		"r29/sp",		"r30/s8",		"r31/ra",
	"r28/gp",		"r29/sp",		"r30/s8",		"r31/ra",
	"r28/gp",		"r29/sp",		"r30/s8",		"r31/ra",
	"r28/gp",		"r29/sp",		"r30/s8",		"r31/ra",
	"r28/gp",		"r29/sp",		"r30/s8",		"r31/ra",
	"r28/gp",		"r29/sp",		"r30/s8",		"r31/ra",
	"r28/gp",		"r29/sp",		"r30/s8",		"r31/ra",
	"r28/gp",		"r29/sp",		"r30/s8",		"r31/ra",
	"r28/gp",		"r29/sp",		"r30/s8",		"r31/ra",
	"r28/gp",
#endif
};
