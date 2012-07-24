/* regs_sh3.c:
 *	This file is used in conjunction with common/monitor/reg_cache.c and
 *	target/monitor/regnames.c. It is included in a file in the
 *	target-specific directory called "regnames.c".  That file is then
 *	included in the common file called reg_cache.c to build the
 *	target-specific register cache code.
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
 *	Author:	Torsten Tetz, Bosch Security Systems
 *	based on regs_sh2.c file by Ed Sutter
 */

static char  *regnames[] = {
		"R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7",
		"R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15",
		"PC", "SR", "PR",
};

