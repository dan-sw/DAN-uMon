
#include "arch_ppc.h"
#include "masks860.h"

    .file "vectors_ppc.S"

/* vectors.s:
    Each exception handler assumes that R27, R28 & R29 are scratch
    (change the .equ lines below if this is a bad assumption!).
    The excpetion handler loads a value into R28, then branches to 
    saveregs.  The saveregs code stores all registers to regtbl[], then
    jumps to exception() with r3 loaded with the id of the exception handler.
    The function vinit() copies the code from vectortable_start thru
    vectortable_end to the base of the vector table (0x100).
*/

    .equ    RSCR1,r27
    .equ    RSCR2,r28
    .equ    RSCR3,r29

    .globl  vectortable_start
    .globl  vectortable_end
    .extern exception
    .extern regtbl

    .text

vectortable_start:
system_reset_exception:
    li      RSCR2,0x100
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
machine_check_exception:
    li      RSCR2,0x200
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
dsi_exception:
    li      RSCR2,0x300
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
isi_exception:
    li      RSCR2,0x400
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
external_interrupt_exception:
    li      RSCR2,0x500
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
alignment_exception:
    li      RSCR2,0x600
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
program_exception:
    li      RSCR2,0x700
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
floating_point_unavailable_exception:
    li      RSCR2,0x800
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
decrementer_exception:
    li      RSCR2,0x900
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
reserved1_exception:
    li      RSCR2,0xa00
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
reserved2_exception:
    li      RSCR2,0xb00
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr

    .balign 0x100
system_call_exception:
    li      RSCR2,0xc00
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
trace_exception:
    li      RSCR2,0xd00
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
floating_point_assist_exception:
    li      RSCR2,0xe00
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
reserved3_exception:
    li      RSCR2,0xf00
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
software_emulation_exception:
    li      RSCR2,0x1000
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
instruction_tlbmiss_exception:
    li      RSCR2,0x1100
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
data_tlbmiss_exception:
    li      RSCR2,0x1200
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
instruction_tlberror_exception:
    li      RSCR2,0x1300
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
data_tlberror_exception:
    li      RSCR2,0x1400
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
reserved4_exception:
    li      RSCR2,0x1500
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
reserved5_exception:
    li      RSCR2,0x1600
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
reserved6_exception:
    li      RSCR2,0x1700
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
reserved7_exception:
    li      RSCR2,0x1800
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
reserved8_exception:
    li      RSCR2,0x1900
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
reserved9_exception:
    li      RSCR2,0x1a00
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
reservedA_exception:
    li      RSCR2,0x1b00
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
data_breakpoint_exception:
    li      RSCR2,0x1c00
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
instruction_breakpoint_exception:
    li      RSCR2,0x1d00
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
peripheral_breakpoint_exception:
    li      RSCR2,0x1e00
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

    .balign 0x100
nonmaskable_development_port_exception:
    li      RSCR2,0x1f00
    lis     RSCR1,(saveregs)@ha
    addi    RSCR1,RSCR1,(saveregs)@l
    mtctr   RSCR1
    bctr    

vectortable_end:

/* savregs:
 * This function simply copies all pertinent registers from the
 * context of the CPU to a local cache accessible by the monitor.
 * Note that the storage of these registers must be coordinated with
 * regs_750.c
 */
saveregs:
    lis     RSCR3,(regtbl)@ha
    addi    RSCR3,RSCR3,(regtbl)@l
    stw     r0,0(RSCR3)
    stw     r1,4(RSCR3)
    stw     r2,8(RSCR3)
    stw     r3,12(RSCR3)
    stw     r4,16(RSCR3)
    stw     r5,20(RSCR3)
    stw     r6,24(RSCR3)
    stw     r7,28(RSCR3)
    stw     r8,32(RSCR3)
    stw     r9,36(RSCR3)
    stw     r10,40(RSCR3)
    stw     r11,44(RSCR3)
    stw     r12,48(RSCR3)
    stw     r13,52(RSCR3)
    stw     r14,56(RSCR3)
    stw     r15,60(RSCR3)
    stw     r16,64(RSCR3)
    stw     r17,68(RSCR3)
    stw     r18,72(RSCR3)
    stw     r19,76(RSCR3)
    stw     r20,80(RSCR3)
    stw     r21,84(RSCR3)
    stw     r22,88(RSCR3)
    stw     r23,92(RSCR3)
    stw     r24,96(RSCR3)
    stw     r25,100(RSCR3)
    stw     r26,104(RSCR3)
    stw     r27,108(RSCR3)
    stw     r28,112(RSCR3)
    stw     r29,116(RSCR3)
    stw     r30,120(RSCR3)
    stw     r31,124(RSCR3)

    mfmsr   r3              /* MSR */
    stw     r3, 128(RSCR3)
    mfcr    r3              /* CR */
    stw     r3, 132(RSCR3)
    mfspr   r3, 8           /* LR */
    stw     r3, 136(RSCR3)
    mfspr   r3, 1           /* XER */
    stw     r3, 140(RSCR3)

    mfspr   r3, 19          /* DAR */
    stw     r3, 144(RSCR3)
    mfspr   r3, 18          /* DSISR */
    stw     r3, 148(RSCR3)
    mfspr   r3, 26          /* SRR0 */
    stw     r3, 152(RSCR3)
    mfspr   r3, 27          /* SRR1 */
    stw     r3, 156(RSCR3)

    mfspr   r3, 9           /* CTR */
    stw     r3, 160(RSCR3)
    mfspr   r3, 22          /* DEC */
    stw     r3, 164(RSCR3)
    mftb    r3              /* TBL */
    stw     r3, 168(RSCR3)
    mftbu   r3              /* TBU */
    stw     r3, 172(RSCR3)

    mfspr   r3, 272         /* SPRG0 */
    stw     r3, 176(RSCR3)
    mfspr   r3, 273         /* SPRG1 */
    stw     r3, 180(RSCR3)
    mfspr   r3, 274         /* SPRG2 */
    stw     r3, 184(RSCR3)
    mfspr   r3, 275         /* SPRG3 */
    stw     r3, 188(RSCR3)

    mr      r3,RSCR2
    lis     RSCR1,(exception)@ha
    addi    RSCR1,RSCR1,(exception)@l
    mtctr   RSCR1
    bctr    
