/* Nios-II, Altera Soft-core Processor Disassembler:
 *  ELS...
 *
 *  General notice:
 *  This code is part of a boot-monitor package developed as a generic base
 *  platform for embedded system designs.  As such, it is likely to be
 *  distributed to various projects beyond the control of the original
 *  author.  Please notify the author of any enhancements made or bugs found
 *  so that all may benefit from the changes.  In addition, notification back
 *  to the author will allow the new user to pick up changes that may have
 *  been made by other users after this version of the code was distributed.
 *
 *  Note1: the majority of this code was edited with 4-space tabs.
 *  Note2: as more and more contributions are accepted, the term "author"
 *       is becoming a mis-representation of credit.
 *
 *  Original author:  Ed Sutter
 *  Email:        esutter@lucent.com
 *  Phone:        908-582-2351
 */

#include "config.h"
#if INCLUDE_DISASSEMBLER


#include "genlib.h"

extern  char  *abiregnames[];
// Instructions with specal significance
#define BREAK_INSTR     0x005b603a      // instruction code for break (actually trap)
#define NOP_INSTR       0x0001883a      // instruction code for no-op

typedef struct
{
    int        Opcode;     /* the opcode */
    char     * Assembler;  /* how to disassemble this instruction */
    char       ImmType;    /* the data type of the imm field */
    int        ArgIndex[4];/* printf argument index */

} NIOS2_OPCODE, * PNIOS2_OPCODE;

/* IMM Field Types

   u  - unsigned 16- bit
   U  - unsigned 32- bit
   s  -   signed 16- bit
   S  -   signed 32- bit
   b  - a branch address ( PC + 4 + signed(IMM16))
   I  - ignore
*/

// field index definitions
#define   REG_A   0       // register A
#define   REG_B   1       // register B
#define   REG_C   2       // register C
#define   OPCODE  3       // opcode or opxcode if r-type
#define   CUST_N  4       // custom instruction number for c-type
#define   IMM     5       // immediate, 16-bits for i-type, 5-bits for r-type, 26 bits for j type

const NIOS2_OPCODE JTypeOpcodes[] =
{
  { 0x00, "call     0x%08x",                        'I', {  IMM,     0,     0,     0}},
  {   -1, "undef",                                  'I', {    0,     0,     0,     0}}
};

const NIOS2_OPCODE CTypeOpcodes[] =
{
  { 0x32, "user     %03d, r%02d, r%02d, r%02d", 'I',  {CUST_N, REG_C, REG_A, REG_B}},
  {   -1, "undef",                              'I',  {     0,     0,     0,     0}}
};

const NIOS2_OPCODE ITypeOpcodes[] =
{

  { 0x04, "addi     %3s, %3s, #%06d  : 0x%08x",  'S',{ REG_B, REG_A, IMM,  IMM}},
  { 0x2c, "andhi    %3s, %3s, %04x:0000",        'u',{ REG_B, REG_A, IMM,    0}},
  { 0x0c, "andi     %3s, %3s, 0000:%04x",        'u',{ REG_B, REG_A, IMM,    0}},
  { 0x26, "beq      %3s, %3s, %08x",             'b',{ REG_A, REG_B, IMM,    0}},
  { 0x0e, "bge      %3s, %3s, %08x",             'b',{ REG_A, REG_B, IMM,    0}},
  { 0x2e, "bgeu     %3s, %3s, %08x",             'b',{ REG_A, REG_B, IMM,    0}},
  { 0x16, "blt      %3s, %3s, %08x",             'b',{ REG_A, REG_B, IMM,    0}},
  { 0x36, "bltu     %3s, %3s, %08x",             'b',{ REG_A, REG_B, IMM,    0}},
  { 0x1e, "bne      %3s, %3s, %08x",             'b',{ REG_A, REG_B, IMM,    0}},
  { 0x06, "br       0x%08x",                         'b',{  IMM,      0,   0,    0}},
  { 0x20, "cmpeqi   %3s, %3s, #%06d   : 0x%08x", 'S',{ REG_B, REG_A, IMM,  IMM}},
  { 0x08, "cmpgei   %3s, %3s, #%06d   : 0x%08x", 'S',{ REG_B, REG_A, IMM,  IMM}},
  { 0x28, "cmpgeui  %3s, %3s, #%06d   : 0x%08x", 'S',{ REG_B, REG_A, IMM,  IMM}},
  { 0x10, "cmplti   %3s, %3s, #%06d   : 0x%08x", 'S',{ REG_B, REG_A, IMM,  IMM}},
  { 0x30, "cmpltui  %3s, %3s, #%06d   : 0x%08x", 'S',{ REG_B, REG_A, IMM,  IMM}},
  { 0x18, "cmpnei   %3s, %3s, #%06d   : 0x%08x", 'S',{ REG_B, REG_A, IMM,  IMM}},
  { 0x3b, "flushd   %06d(%3s)",                    's',{   IMM, REG_A,   0,    0}},
  { 0x1b, "flushda  %06d(%3s)",                    's',{   IMM, REG_A,   0,    0}},
  { 0x33, "initd    %06d(%3s)",                    's',{   IMM, REG_A,   0,    0}},
  { 0x07, "ldb      %06d(%3s)",                    's',{   IMM, REG_A,   0,    0}},
  { 0x27, "ldbio    %06d(%3s)",                    's',{   IMM, REG_A,   0,    0}},
  { 0x03, "ldbu     %06d(%3s)",                    's',{   IMM, REG_A,   0,    0}},
  { 0x23, "ldbuio   %06d(%3s)",                    's',{   IMM, REG_A,   0,    0}},
  { 0x0f, "ldh      %06d(%3s)",                    's',{   IMM, REG_A,   0,    0}},
  { 0x2f, "ldhio    %06d(%3s)",                    's',{   IMM, REG_A,   0,    0}},
  { 0x0b, "ldhu     %06d(%3s)",                    's',{   IMM, REG_A,   0,    0}},
  { 0x2b, "ldhuio   %06d(%3s)",                    's',{   IMM, REG_A,   0,    0}},
  { 0x17, "ldw      %06d(%3s)",                    's',{   IMM, REG_A,   0,    0}},
  { 0x37, "ldwio    %06d(%3s)",                    's',{   IMM, REG_A,   0,    0}},
  { 0x24, "muli     %3s, %3s, #%06d   : 0x%08x", 'S',{ REG_B, REG_A, IMM,  IMM}},
  { 0x34, "orhi     %3s, %3s, 0x%04x:0000",       'u',{ REG_B, REG_A, IMM,    0}},
  { 0x14, "ori      %3s, %3s, 0x%04x:0000",       'u',{ REG_B, REG_A, IMM,    0}},
  { 0x05, "stb      %3s, %06d(%3s)",            's',{ REG_B, IMM, REG_A,    0}},
  { 0x25, "stbio    %06d(%3s)",                    's',{   IMM, REG_A,   0,    0}},
  { 0x0d, "sth      %06d(%3s)",                    's',{   IMM, REG_A,   0,    0}},
  { 0x2d, "sthio    %06d(%3s)",                    's',{   IMM, REG_A,   0,    0}},
  { 0x15, "stw      %06d(%3s)",                    's',{   IMM, REG_A,   0,    0}},
  { 0x35, "stwio    %06d(%3s)",                    's',{   IMM, REG_A,   0,    0}},
  { 0x3c, "xorhi    %3s, %3s, 0x%04x:0000",       'u',{ REG_B, REG_A, IMM,    0}},
  { 0x1c, "xori     %3s, %3s, 0x%04x:0000",       'u',{ REG_B, REG_A, IMM,    0}},
  { -1  , "undef",                                     'I',{     0,     0,   0,    0}}
};

const NIOS2_OPCODE RTypeOpcodes[] =
{
  { 0x31, "add      %3s, %3s, %3s",        'I',{ REG_C, REG_A, REG_B,   0}},
  { 0x01, "eret   ",                                'I',{     0,     0,     0,   0}},
  { 0x02, "roli     %3s, %3s, %3s",        'I',{ REG_C, REG_A, REG_B,   0}},
  { 0x03, "rol      %3s, %3s, %3s",        'I',{ REG_C, REG_A, REG_B,   0}},
  { 0x04, "flushp ",                                'I',{     0,     0,     0,   0}},
  { 0x05, "ret",                                    'I',{ REG_C, REG_A, REG_B,   0}},
  { 0x06, "nor      %3s, %3s, %3s",        'I',{ REG_C, REG_A, REG_B,   0}},
  { 0x07, "mulxuu   %3s, %3s, %3s",        'I',{ REG_C, REG_A, REG_B,   0}},
  { 0x08, "cmpge    %3s, %3s, %3s",        'I',{ REG_C, REG_A, REG_B,   0}},
  { 0x09, "bret     %3s, %3s, %3s",        'I',{ REG_C, REG_A, REG_B,   0}},
  { 0x0b, "ror      %3s, %3s, %3s",        'I',{ REG_C, REG_A, REG_B,   0}},
  { 0x0c, "flushi   %3s",                        'I',{     0,     0,     0,   0}},
  { 0x0d, "jmp      %3s",                        'I',{     0,     0,     0,   0}},
  { 0x0e, "and      %3s, %3s, %3s",        'I',{ REG_C, REG_A, REG_B,   0}},
  { 0x10, "cmplt    %3s, %3s, %3s",        'I',{ REG_C, REG_A, REG_B,   0}},
  { 0x12, "slli     %3s, %3s, %2d",           'u',{ REG_C, REG_A,   IMM,   0}},
  { 0x13, "sll      %3s, %3s, %3s",        'I',{ REG_C, REG_A, REG_B,   0}},
  { 0x16, "or       %3s, %3s, %3s",        'I',{ REG_C, REG_A, REG_B,   0}},
  { 0x17, "mulxsu   %3s, %3s, %3s",        'I',{ REG_C, REG_A, REG_B,   0}},
  { 0x18, "cmpne    %3s, %3s",                 'I',{ REG_A, REG_B,     0,   0}},
  { 0x1a, "srli     %3s, %3s, %3s",        'I',{ REG_C, REG_A, REG_B,   0}},
  { 0x1b, "srl      %3s, %3s, %3s",        'I',{ REG_C, REG_A, REG_B,   0}},
  { 0x1c, "nextpc   %3s",                        'I',{ REG_C,     0,     0,   0}},
  { 0x1d, "callr    %3s",                        'I',{ REG_A,     0,     0,   0}},
  { 0x1e, "xor      %3s, %3s, %3s",        'I',{ REG_C, REG_A, REG_B,   0}},
  { 0x1f, "mulxss   %3s, %3s, %3s",        'I',{ REG_C, REG_A, REG_B,   0}},
  { 0x20, "cmpeq    %3s, %3s, %3s",        'I',{ REG_C, REG_A, REG_B,   0}},
  { 0x24, "divu     %3s, %3s, %3s",        'I',{ REG_C, REG_A, REG_B,   0}},
  { 0x25, "div      %3s, %3s, %3s",        'I',{ REG_C, REG_A, REG_B,   0}},
  { 0x26, "rdctl    %3s, ctl%02d",              'u',{ REG_C,   IMM,     0,   0}},
  { 0x27, "mul      %3s, %3s, %3s",        'I',{ REG_C, REG_A, REG_B,   0}},
  { 0x28, "cmpgeu   %3s, %3s, %3s",        'I',{ REG_C, REG_A, REG_B,   0}},
  { 0x29, "initi    %3s",                        'I',{ REG_A,     0,     0,   0}},
  { 0x2d, "trap",                                   'I',{     0,     0,     0,   0}},
  { 0x2e, "wrctl  ctl%02d, %3s",               'u',{   IMM, REG_A,     0,   0}},
  { 0x30, "cmpltu   %3s, %3s, %3s",        'I',{ REG_C, REG_A, REG_B,   0}},
  { 0x34, "break    %02d",                         'I',{   IMM,     0,     0,   0}},
  { 0x36, "sync ",                                  'I',{     0,     0,     0,   0}},
  { 0x39, "sub      %3s, %3s, %3s",        'I',{ REG_C, REG_A, REG_B,   0}},
  { 0x3a, "srai     %3s, %3s, %3s",          'u',{ REG_C, REG_A, REG_B,   0}},
  { 0x3b, "sra      %3s, %3s, %3s",        'I',{ REG_C, REG_A, REG_B,   0}},
  { 0x100,"nop",                                    'I',{     0,     0,     0,   0}},
  { -1  , "undef",                                  'I',{     0,     0,     0,   0}}
};

/*--------------------------------------------------------------------------
  There are four types of Nios-II instrictions:

  I-Type - Instruction contains an immediate value within the word
  R-Type - All arguments and results are specified as registers
  J-Type - An opcode and an immediate data field - call only
  C-Type - Custom instruction

  --------------------------------------------------------------------------*/
 typedef union
 {
    unsigned word;

    struct
    {
        unsigned op   : 6;  //  6-bit opcode field
        unsigned imm  : 16; // 16-bit immediate data field
        unsigned b    : 5;  //  5-bit register field b
        unsigned a    : 5;  //  5-bit register field a
    } IType;

    struct
    {
        unsigned op   : 6;  //  6-bit opcode field
        unsigned imm5 : 5;  //  5-bit field ignored by proc, used by debugger
        unsigned opx  : 6;  //  6-bit opcode extended field
        unsigned c    : 5;  //  5-bit register field c
        unsigned b    : 5;  //  5-bit register field b
        unsigned a    : 5;  //  5-bit register field a
    } RType;

    struct
    {
        unsigned op   : 6;  //  6-bit opcode field
        unsigned imm  :26;  // 26-bit immediate data field
    } JType;

    struct
    {
        unsigned op   : 6;  //  6-bit opcode field
        unsigned n    : 8;  //  8-bit field ignored by proc, used by debugger
        unsigned rc   : 1;  //  retruns result in rc
        unsigned rb   : 1;  //  uses rb
        unsigned ra   : 1;  //  uses ra
        unsigned c    : 5;  //  5-bit register field c
        unsigned b    : 5;  //  5-bit register field b
        unsigned a    : 5;  //  5-bit register field a
    } CType;

} NIOS2_INST, * PNIOS2_INST;

void  DisassembleInstruction( PNIOS2_INST pInst);

char *DisHelp[] = {
  "Disassemble memory",
  "-[m] {address | .} [lines]",
  " -m  interactive (-more-)",
  0,
};

static  int *loc;

int
Dis(argc,argv)
int argc;
char  *argv[];
{
  void  disass( int *,int,int);
  int lines, opt, more;
  int  *at = 0;

  more = 0;
  while ((opt=getopt(argc,argv,"m")) != -1) {
    switch(opt) {
    case 'm':
      more = 1;
      break;
    default:
      return(0);
    }
  }

  if (*argv[optind] == '.')
  //  getreg("PC",&at);
  more = more;
  else
    at = ( int *)strtoul(argv[optind],0,0);

  if (argc == (optind+2))
    lines = strtoul(argv[optind+1],0,0);
  else
    lines = 8;

  while(1) {
    disass((int  *)at,lines,0);
    if (more && More())
      at = loc;
    else
      break;
  }
  return(0);
}



void
disass( int *inst, int lines, int next)
{
  while (lines)
  {
    if (next)
      printf("Nxt inst: %04x ",*inst);
    else
      DisassembleInstruction( (PNIOS2_INST)inst);

    lines--;
    inst++;
  }

  loc = inst;
}



void  DisassembleInstruction( PNIOS2_INST pInst)
{
   int            InstField[5];
   unsigned short imm     = 0;
   PNIOS2_OPCODE  pOpcode = 0;

   // determine the instruction type
   switch ( pInst->IType.op)
   {
      case 0: // call instruction - the only j-type
      {
         // J-Type
         pOpcode            = (PNIOS2_OPCODE)JTypeOpcodes;

         InstField[ OPCODE] = pInst->JType.op;

         // in the case of a jump i = PC[31-28]:imm * 4
         InstField[ IMM]    = ( pInst->JType.imm << 2) | ((int)pInst & 0xf0000000);

         break;
      }

      case 0x3a: // this opcode indicates rtype, opcode in opx filed
      {
         // R-Type
         pOpcode              = (PNIOS2_OPCODE)RTypeOpcodes;

         InstField[ REG_A]    = (int)abiregnames[ pInst->RType.a];
         InstField[ REG_B]    = (int)abiregnames[ pInst->RType.b];
         InstField[ REG_C]    = (int)abiregnames[ pInst->RType.c];
         InstField[ OPCODE]   = pInst->RType.opx;

         imm      = (unsigned short)pInst->RType.imm5;

         // allow the disassembler to print out nop's
         if ( *(int *)pInst == NOP_INSTR)
         {
            InstField[ OPCODE] = 0x100;
         }

         break;
      }

      case 0x32: // the opcode for custom instruction
      {
         // C-Type
         pOpcode            = (PNIOS2_OPCODE)CTypeOpcodes;

         InstField[ REG_A]  = (int)abiregnames[ pInst->CType.a];
         InstField[ REG_B]  = (int)abiregnames[ pInst->CType.b];
         InstField[ REG_C]  = (int)abiregnames[ pInst->CType.c];
         InstField[ CUST_N] = pInst->CType.n;

         break;
      }

      default:
      {
         // I-Type
         pOpcode            = (PNIOS2_OPCODE)ITypeOpcodes;

         InstField[ REG_A]  = (int)abiregnames[ pInst->IType.a];
         InstField[ REG_B]  = (int)abiregnames[ pInst->IType.b];
         InstField[ OPCODE] = pInst->IType.op;

         imm                = (unsigned short)pInst->IType.imm;

         break;
      }
   }

   // find the instruction in the table based on opcode
   while( pOpcode->Opcode != -1)
   {
      if( pOpcode->Opcode == InstField[ OPCODE]) break;
      pOpcode++;
   }

   // pOpcode now points to the structure that defines the formatting of the inst

   // we may need to operate on the IMM field
   switch( pOpcode->ImmType)
   {
      case 'u':  // unsigned 16-bit
      {
         InstField[IMM] = (unsigned int)imm;
         break;
      }
      case 'U':  // unsigned 32-bit
      {
         InstField[IMM] = (unsigned int)imm;
         break;
      }
      case 's':  //   signed 16-bit
      {
         InstField[IMM] = (int)imm;
         break;
      }
      case 'S':  //   signed 32-bit
      {
         InstField[IMM] = (int)imm;
         break;
      }
      case 'b':  //   branch address PC  4 + (signed)IMM
      {
         InstField[IMM] = (int)imm + 4 + (int)pInst;
         break;
      }
   }

   // nothing left to do but print
   printf("%08x: %08x  ", pInst, *pInst);
   printf( pOpcode->Assembler, InstField[ pOpcode->ArgIndex[0]],InstField[ pOpcode->ArgIndex[1]],InstField[ pOpcode->ArgIndex[2]],InstField[ pOpcode->ArgIndex[3]]);
   printf("\n");
}


#endif
