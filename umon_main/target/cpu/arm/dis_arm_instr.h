/* 
 * dis_arm_instr.h
 *
 * Instruction disassebly code for ARM processors.
 * Supports ARM and THUMB instruction sets.
 *
 * by Nick Patavalis (npat@inaccessnetworks.com)
 * 
 * Summary:
 *
 * F disarm_print_arm_instr - disasseble arm instruction
 * F disarm_print_thumb_instr - disasseble thumb instruction
 * F disarm_get_regname_num_options - num of reg-naming options
 * F disarm_set_regname_option - select reg-naming option
 * F disarm_get_regnames - info about reg-naming options
 *
 * This code was part of libopcodes which was part of GNU binutils.
 * Distributed under the terms of the GNU GPL v2 or latter version.
 * Copyright (C) Free Software Foundation, Inc. 
 * original code:
 *   contributed by Richard Earnshaw (rwe@pegasus.esprit.ec.org)
 *   modifications by James G. Smith (jsmith@cygnus.co.uk)
 *
 */

#ifndef DIS_ARM_INSTR_H
#define DIS_ARM_INSTR_H

/*************************************************************************/

/*
 * F disarm_print_arm_instr
 *
 * Disasemble a signle ARM instruction and print the
 * resulting assembly statement.
 *
 * Arguments:
 *   - pc - the address of the instruction (i.e. the program counter
 *   value when the instruction is executed.
 *   - given - the instruction word
 *
 * Returns:
 *   The size of the instruction disassebled (i.e. always 4).
 */
int disarm_print_arm_instr (unsigned long pc, long given);

/*************************************************************************/

/*
 * F disarm_print_thumb_instr
 *
 * Disasemble a signle THUMB instruction and print the
 * resulting assembly statement.
 *
 * Arguments:
 *   - pc - the address of the instruction (i.e. the program counter
 *   value when the instruction is executed.
 *   - given - the instruction half-word, in the least significant 
 *   half of 'given'.
 *
 * Returns:
 *   The size of the instruction disassebled (i.e. always 4).
 */
int disarm_print_thumb_instr (unsigned long pc, long given);

/*************************************************************************/

/*
 * F disarm_get_regname_num_options
 *
 * Returns the number of register-naming options
 *
 */
int disarm_get_regname_num_options (void);

/*
 * F disarm_set_regname_option
 *
 * Selects a register-naming option.
 *
 * Arguments:
 *   - option - the register-naming option to be selected
 *
 * Returns:
 *   The old register-naming option is returned.
 */
int disarm_set_regname_option (int option);

/*
 * F disarm_get_regnames
 *
 * Returns pointer to structures describing (giving information about)
 * the available register-naming options.
 *
 * Arguments:
 *   - option - the option you are interested in.
 *   - setname - Output. The name if the coresponding option.
 *   - setdescription - Output. A short descr of the coresponding 
 *   option.
 *   - register_names - Output. An array of register names
 *
 * Returns:
 *   The number of registers in the array
 */
int disarm_get_regnames (int option, const char **setname,
						 const char **setdescription,
						 const char ***register_names);

/*************************************************************************/

#endif /* of DIS_ARM_INSTR_H */
