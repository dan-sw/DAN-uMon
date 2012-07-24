/* 
 * dis_arm_instr.c
 *
 * Instruction disassebly code for ARM processors.
 * Supports ARM and THUMB instruction sets.
 *
 * by Nick Patavalis (npat@inaccessnetworks.com)
 * 
 * This code was part of libopcodes which was part of GNU binutils.
 * Distributed under the terms of the GNU GPL v2 or latter version.
 * Copyright (C) Free Software Foundation, Inc. 
 * original code:
 *   contributed by Richard Earnshaw (rwe@pegasus.esprit.ec.org)
 *   modifications by James G. Smith (jsmith@cygnus.co.uk)
 *
 */

#include "config.h"
extern int printf(char *fmt, ...);

#if INCLUDE_DISASSEMBLER

#include "dis_arm_opcodes.h"

/*************************************************************************/

char dis_arm_instr_cvsid[] = "$Id";

/*************************************************************************/

#define NOTABS

#ifndef NUM_ELEM
#define NUM_ELEM(a)     (sizeof (a) / sizeof (a)[0])
#endif

/*************************************************************************/

static char * arm_conditional[] = {
	"eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc",
	"hi", "ls", "ge", "lt", "gt", "le", "", "nv"
};

typedef struct {
	const char * name;
	const char * description;
	const char * reg_names[16];
} arm_regname;

static arm_regname regnames[] =
{
  { "raw" , "Select raw register names",
    { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", 
	  "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
	}
  },
  { "std",  "Select register names used in ARM's ISA documentation",
    { "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", 
	  "r8", "r9", "r10", "r11", "r12", "sp",  "lr",  "pc" 
	}
  },
  { "apcs", "Select register names used in the APCS",
    { "a1", "a2", "a3", "a4", "v1", "v2", "v3", "v4", 
	  "v5", "v6", "sl",  "fp",  "ip",  "sp",  "lr",  "pc" 
	}
  },
  { "atpcs", "Select register names used in the ATPCS",
    { "a1", "a2", "a3", "a4", "v1", "v2", "v3", "v4", 
	  "v5", "v6", "v7",  "v8",  "IP",  "SP",  "LR",  "PC" 
	}
  },
  { "special-atpcs", "Select special register names used in the ATPCS",
    { "a1", "a2", "a3", "a4", "v1", "v2", "v3", "WR", 
	  "v5", "SB", "SL",  "FP",  "IP",  "SP",  "LR",  "PC" 
	}
  }
};

/* Default to raw register name set.  */
static unsigned int disarm_regname_selected;

#define NUM_ARM_REGNAMES  NUM_ELEM (regnames)
#define arm_regnames      regnames[disarm_regname_selected].reg_names

static char * arm_fp_const[] = {
	"0.0", "1.0", "2.0", "3.0", "4.0", "5.0", "0.5", "10.0"
};

static char * arm_shift[] = {
	"lsl", "lsr", "asr", "ror"
};

/*************************************************************************/

static void arm_decode_shift (unsigned long given);

int disarm_print_arm_instr (unsigned long pc, long given);

int disarm_print_thumb_instr (unsigned long pc, long given);

int disarm_get_regname_num_options (void);
int disarm_set_regname_option (int option);
int disarm_get_regnames (int option, const char **setname,
						 const char **setdescription,
						 const char ***register_names);

/*************************************************************************/

int
disarm_get_regname_num_options (void)
{
  return NUM_ARM_REGNAMES;
}

/*************************************************************************/

int
disarm_set_regname_option (int option)
{
  int old = disarm_regname_selected;
  disarm_regname_selected = option;
  return old;
}

/*************************************************************************/

int
disarm_get_regnames (int option, const char **setname,
					 const char **setdescription,
					 const char ***register_names)
{
  *setname = regnames[option].name;
  *setdescription = regnames[option].description;
  *register_names = regnames[option].reg_names;
  return 16;
}

/*************************************************************************/

static void
arm_decode_shift (unsigned long given)
{
  printf( "%s", arm_regnames[given & 0xf]);
  
  if ((given & 0xff0) != 0)
    {
      if ((given & 0x10) == 0)
	{
	  int amount = (given & 0xf80) >> 7;
	  int shift = (given & 0x60) >> 5;
	  
	  if (amount == 0)
	    {
	      if (shift == 3)
		{
		  printf( ", rrx");
		  return;
		}
	      
	      amount = 32;
	    }
	  
	  printf( ", %s #%d", arm_shift[shift], amount);
	}
      else
	printf( ", %s %s", arm_shift[(given & 0x60) >> 5],
	      arm_regnames[(given & 0xf00) >> 8]);
    }
}

/*************************************************************************/

int
disarm_print_arm_instr (unsigned long pc, long given)
{
	struct arm_opcode *  insn;

	for (insn = arm_opcodes; insn->assembler; insn++) {
		if ((given & insn->mask) == insn->value) {
			char * c;
	  
			for (c = insn->assembler; *c; c++) {
				if (*c == '%') {
					switch (*++c) {
					case '%':
						printf( "%%");
						break;

					case 'a':
						if (((given & 0x000f0000) == 0x000f0000)
							&& ((given & 0x02000000) == 0)) {
							int offset = given & 0xfff;
			  
							printf( "[pc");
 
							if (given & 0x01000000) {
								if ((given & 0x00800000) == 0)
									offset = - offset;
							  
								/* pre-indexed */
								printf( ", #%x]", offset);

								offset += pc + 8;

								/* Cope with the possibility of write-back
								   being used.  Probably a dangerous thing
								   for the programmer, but who are we to
								   argue ?  */
								if (given & 0x00200000)
									printf( "!");
							} else {
								/* Post indexed.  */
								printf( "], #%x", offset);

								offset = pc + 8;  /* ie ignore the offset.  */
							}
			  
							printf( " ; ");
#if 0
							info->print_address_func (offset, info);
#else
							printf( "0x%x", offset);
#endif
						} else {
							printf( "[%s", 
								  arm_regnames[(given >> 16) & 0xf]);
							if ((given & 0x01000000) != 0)
							{
								if ((given & 0x02000000) == 0)
								{
									int offset = given & 0xfff;
									if (offset)
										printf( ", %s#%d",
											  (((given & 0x00800000) == 0)
											   ? "-" : ""), offset);
								} else {
									printf( ", %s",
										  (((given & 0x00800000) == 0)
										   ? "-" : ""));
									arm_decode_shift (given);
								}

								printf( "]%s", 
									  ((given & 0x00200000) != 0) ? "!" : "");
							} else {
								if ((given & 0x02000000) == 0) {
									int offset = given & 0xfff;
									if (offset)
										printf( "], %s#%d",
											  (((given & 0x00800000) == 0)
											   ? "-" : ""), offset);
									else 
										printf( "]");
								} else {
									printf( "], %s",
										  (((given & 0x00800000) == 0) 
										   ? "-" : ""));
									arm_decode_shift (given);
								}
							}
						}
						break;

					case 's':
						if ((given & 0x004f0000) == 0x004f0000) {
							/* PC relative with immediate offset.  */
							int offset = ((given & 0xf00) >> 4) 
								| (given & 0xf);
			  
							if ((given & 0x00800000) == 0)
								offset = -offset;
			  
							printf( "[pc, #%x] ; ", offset);
						  
#if 0
							(*info->print_address_func)
								(offset + pc + 8, info);
#else
							printf( "0x%lx", offset + pc + 8);
#endif
						} else {
							printf( "[%s", 
								  arm_regnames[(given >> 16) & 0xf]);
							if ((given & 0x01000000) != 0) {
								/* Pre-indexed.  */
								if ((given & 0x00400000) == 0x00400000) {
									/* Immediate.  */
									int offset = ((given & 0xf00) >> 4) 
										| (given & 0xf);
									if (offset)
										printf( ", %s#%d",
											  (((given & 0x00800000) == 0)
											   ? "-" : ""), offset);
								} else {
									/* Register.  */
									printf( ", %s%s",
										  (((given & 0x00800000) == 0)
										   ? "-" : ""),
										  arm_regnames[given & 0xf]);
								}

								printf( "]%s", 
									  ((given & 0x00200000) != 0) ? "!" : "");
							} else {
								/* Post-indexed.  */
								if ((given & 0x00400000) == 0x00400000) {
									/* Immediate.  */
									int offset = ((given & 0xf00) >> 4) 
										| (given & 0xf);
									if (offset)
										printf( "], %s#%d",
											  (((given & 0x00800000) == 0)
											   ? "-" : ""), offset);
									else 
										printf( "]");
								} else {
									/* Register.  */
									printf( "], %s%s",
										  (((given & 0x00800000) == 0)
										   ? "-" : ""),
										  arm_regnames[given & 0xf]);
								}
							}
						}
						break;
			  
					case 'b':
#if 0
						(*info->print_address_func)
							(BDISP (given) * 4 + pc + 8, info);
#else
						printf( "0x%lx", BDISP (given) * 4 + pc + 8);
#endif
						break;

					case 'c':
						printf( "%s",
							  arm_conditional [(given >> 28) & 0xf]);
						break;

					case 'm':
					{
						int started = 0;
						int reg;

						printf( "{");
						for (reg = 0; reg < 16; reg++)
							if ((given & (1 << reg)) != 0) {
								if (started)
									printf( ", ");
								started = 1;
								printf( "%s", arm_regnames[reg]);
							}
						printf( "}");
					}
					break;

					case 'o':
						if ((given & 0x02000000) != 0) {
							int rotate = (given & 0xf00) >> 7;
							int immed = (given & 0xff);

							immed = (((immed << (32 - rotate))
									  | (immed >> rotate)) & 0xffffffff);
							printf( "#%d ; 0x%x", immed, immed);
						} else
							arm_decode_shift (given);
						break;

					case 'p':
						if ((given & 0x0000f000) == 0x0000f000)
							printf( "p");
						break;

					case 't':
						if ((given & 0x01200000) == 0x00200000)
							printf( "t");
						break;

					case 'h':
						if ((given & 0x00000020) == 0x00000020)
							printf( "h");
						else
							printf( "b");
						break;

					case 'A':
						printf( "[%s", 
							  arm_regnames [(given >> 16) & 0xf]);
						if ((given & 0x01000000) != 0) {
							int offset = given & 0xff;
							if (offset)
								printf( ", %s#%d]%s",
									  ((given & 0x00800000) == 0 ? "-" : ""),
									  offset * 4,
									  ((given & 0x00200000) != 0 ? "!" : ""));
							else
								printf( "]");
						} else {
							int offset = given & 0xff;
							if (offset)
								printf( "], %s#%d",
									  ((given & 0x00800000) == 0 ? "-" : ""),
									  offset * 4);
							else
								printf( "]");
						}
						break;

					case 'C':
						switch (given & 0x000f0000)
						{
						default:
							printf( "_???");
							break;
						case 0x90000:
							printf( "_all");
							break;
						case 0x10000:
							printf( "_c");
							break;
						case 0x20000:
							printf( "_x");
							break;
						case 0x40000:
							printf( "_s");
							break;
						case 0x80000:
							printf( "_f");
							break;
						}
						break;

					case 'F':
						switch (given & 0x00408000) {
						case 0:
							printf( "4");
							break;
						case 0x8000:
							printf( "1");
							break;
						case 0x00400000:
							printf( "2");
							break;
						default:
							printf( "3");
						}
						break;
			
					case 'P':
						switch (given & 0x00080080) {
						case 0:
							printf( "s");
							break;
						case 0x80:
							printf( "d");
							break;
						case 0x00080000:
							printf( "e");
							break;
						default:
							printf( "<illegal precision>");
							break;
						}
						break;
					case 'Q':
						switch (given & 0x00408000) {
						case 0:
							printf( "s");
							break;
						case 0x8000:
							printf( "d");
							break;
						case 0x00400000:
							printf( "e");
							break;
						default:
							printf( "p");
							break;
						}
						break;
					case 'R':
						switch (given & 0x60) {
						case 0:
							break;
						case 0x20:
							printf( "p");
							break;
						case 0x40:
							printf( "m");
							break;
						default:
							printf( "z");
							break;
						}
						break;

					case '0': case '1': case '2': case '3': case '4': 
					case '5': case '6': case '7': case '8': case '9':
					{
						int bitstart = *c++ - '0';
						int bitend = 0;

						while (*c >= '0' && *c <= '9')
							bitstart = (bitstart * 10) + *c++ - '0';

						switch (*c) {
						case '-':
							c++;
						  
							while (*c >= '0' && *c <= '9')
								bitend = (bitend * 10) + *c++ - '0';
						  
							if (!bitend) {
								printf( "***OOPS!***");
								return 4;
							}
						  
							switch (*c) {
							case 'r':
							{
								long reg;
				  
								reg = given >> bitstart;
								reg &= (2 << (bitend - bitstart)) - 1;
				  
								printf( "%s", arm_regnames[reg]);
							}
							break;
							case 'd':
							{
								long reg;
				  
								reg = given >> bitstart;
								reg &= (2 << (bitend - bitstart)) - 1;
				  
								printf( "%ld", reg);
							}
							break;

							case 'x':
							{
								long reg;
							  
								reg = given >> bitstart;
								reg &= (2 << (bitend - bitstart)) - 1;
				  
								printf( "0x%08lx", reg);
				  
								/* Some SWI instructions have special
								   meanings.  */
								if ((given & 0x0fffffff) == 0x0FF00000)
									printf( " ; IMB");
								else if ((given & 0x0fffffff) == 0x0FF00001)
									printf( " ; IMBRange");
							}
							break;

							case 'X':
							{
								long reg;
				  
								reg = given >> bitstart;
								reg &= (2 << (bitend - bitstart)) - 1;
				  
								printf( "%01lx", reg & 0xf);
							}
							break;

							case 'f':
							{
								long reg;
				  
								reg = given >> bitstart;
								reg &= (2 << (bitend - bitstart)) - 1;
							  
								if (reg > 7)
									printf( "#%s",
										  arm_fp_const[reg & 7]);
								else
									printf( "f%ld", reg);
							}
							break;

							default:
								printf( "***OOPS!***");
								return 4;
							}
							break;
						  
						case '`':
							c++;
							if ((given & (1 << bitstart)) == 0)
								printf( "%c", *c);
							break;
						case '\'':
							c++;
							if ((given & (1 << bitstart)) != 0)
								printf( "%c", *c);
							break;
						case '?':
							++c;
							if ((given & (1 << bitstart)) != 0)
								printf( "%c", *c++);
							else
								printf( "%c", *++c);
							break;
						default:
							printf( "***OOPS!***");
							return 4;
						}
						break;
					  
					default:
						printf( "***OOPS!***");
						return 4;
					}
					}
#ifdef NOTABS
				} else if (*c == '\t') {
					printf( "%c", ' ');
#endif
				} else {
					printf( "%c", *c);
				}
			}
			return 4;
		}
	}
	printf( "***OOPS!***");
	return 4;
}

/*************************************************************************/

int
disarm_print_thumb_instr (unsigned long pc, long given)
{
	struct thumb_opcode * insn;

	for (insn = thumb_opcodes; insn->assembler; insn++) {
		if ((given & insn->mask) == insn->value) {
			char * c = insn->assembler;

			/* Special processing for Thumb 2 instruction BL sequence:  */
			if (!*c) { /* Check for empty (not NULL) assembler string.  */
                printf( "bl\t");
		
#if 0
				info->print_address_func (BDISP23 (given) * 2 + pc + 4, info);
#else
				printf( "0x%lx", BDISP23 (given) * 2 + pc + 4);
#endif
				return 4;
            } else {
				given &= 0xffff;
	      
				for (; *c; c++) {
					if (*c == '%') {
						int domaskpc = 0;
						int domasklr = 0;
		      
						switch (*++c) {
                        case '%':
							printf( "%%");
							break;
							
                        case 'S':
						{
                            long reg;
			    
                            reg = (given >> 3) & 0x7;
                            if (given & (1 << 6))
								reg += 8;
			    
                            printf( "%s", arm_regnames[reg]);
						}
						break;

                        case 'D':
						{
                            long reg;
							
                            reg = given & 0x7;
                            if (given & (1 << 7))
								reg += 8;
			    
                            printf( "%s", arm_regnames[reg]);
						}
						break;

                        case 'T':
							printf( "%s",
								  arm_conditional [(given >> 8) & 0xf]);
							break;

                        case 'N':
							if (given & (1 << 8))
								domasklr = 1;
							/* Fall through.  */
                        case 'O':
							if (*c == 'O' && (given & (1 << 8)))
								domaskpc = 1;
							/* Fall through.  */
                        case 'M':
						{
                            int started = 0;
                            int reg;
			    
                            printf( "{");
			    
                            /* It would be nice if we could spot
                               ranges, and generate the rS-rE format: */
                            for (reg = 0; (reg < 8); reg++)
								if ((given & (1 << reg)) != 0) {
									if (started)
										printf( ", ");
									started = 1;
									printf( "%s", arm_regnames[reg]);
                                }

                            if (domasklr) {
                                if (started)
									printf( ", ");
                                started = 1;
                                printf( (char *)arm_regnames[14] /* "lr" */);
							}

                            if (domaskpc) {
                                if (started)
									printf( ", ");
                                printf( (char *)arm_regnames[15] /* "pc" */);
							}

                            printf( "}");
						}
						break;


                        case '0': case '1': case '2': case '3': case '4': 
                        case '5': case '6': case '7': case '8': case '9':
						{
                            int bitstart = *c++ - '0';
                            int bitend = 0;
			    
                            while (*c >= '0' && *c <= '9')
								bitstart = (bitstart * 10) + *c++ - '0';

                            switch (*c) {
							case '-':
							{
								long reg;
								
								c++;
								while (*c >= '0' && *c <= '9')
                                    bitend = (bitend * 10) + *c++ - '0';
								if (!bitend) {
									printf( "***OOPS!***");
									return 2;
								}
								reg = given >> bitstart;
								reg &= (2 << (bitend - bitstart)) - 1;
								switch (*c) {
								case 'r':
									printf( "%s", arm_regnames[reg]);
									break;
									
								case 'd':
									printf( "%ld", reg);
									break;

								case 'H':
									printf( "%ld", reg << 1);
									break;
									
								case 'W':
									printf( "%ld", reg << 2);
									break;
									
								case 'a':
									/* PC-relative address -- the bottom two
									   bits of the address are dropped
									   before the calculation.  */
#if 0
									info->print_address_func
										(((pc + 4) & ~3) + (reg << 2), info);
#else
									printf( "0x%lx", 
										  ((pc + 4) & ~3) + (reg << 2));
#endif
									break;

								case 'x':
									printf( "0x%04lx", reg);
									break;

								case 'I':
									reg = ((reg ^ (1 << bitend)) 
										   - (1 << bitend));
									printf( "%ld", reg);
									break;

								case 'B':
									reg = ((reg ^ (1 << bitend)) 
										   - (1 << bitend));
#if 0
									(*info->print_address_func)
                                        (reg * 2 + pc + 4, info);
#else
									printf( "0x%lx", reg * 2 + pc + 4);
#endif
									break;

								default:
									printf( "***OOPS!***");
									return 2;
								}
							}
							break;

							case '\'':
                                c++;
                                if ((given & (1 << bitstart)) != 0)
									printf( "%c", *c);
                                break;

							case '?':
                                ++c;
                                if ((given & (1 << bitstart)) != 0)
									printf( "%c", *c++);
                                else
									printf( "%c", *++c);
                                break;

							default:
								printf( "***OOPS!***");
								return 2;
							}
						}
						break;

                        default:
							printf( "***OOPS!***");
							return 2;
                        }
#ifdef NOTABS
				} else if (*c == '\t') {
					printf( "%c", ' ');
#endif
				} else
					printf( "%c", *c);
                }
			}
			return 2;
		}
    }

	/* No match.  */
	printf( "***OOPS!***");
	return 2;
}

/*************************************************************************/

#endif /* of INCLUDE_DISASSEMBLER */
