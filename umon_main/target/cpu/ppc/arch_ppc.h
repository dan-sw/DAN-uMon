/*
 * arch_ppc.h:
 *
 * Some of this is from IBM BSP source code...
 *
 * This source code has been made available to you by IBM on an AS-IS
 * basis.  Anyone receiving this source is licensed under IBM
 * copyrights to use it in any way he or she deems fit, including
 * copying it, modifying it, compiling it, and redistributing it either
 * with or without modifications.  No license under IBM patents or
 * patent applications is to be implied by the copyright license.
 *
 * Any user of this software should understand that IBM cannot provide
 * technical support for this software and will not be responsible for
 * any consequences resulting from the use of this software.
 *
 * Any person who transfers this source code or any derivative work
 * must include the IBM copyright notice, this paragraph, and the
 * preceding two paragraphs in the transferred software.
 *
 * COPYRIGHT   I B M   CORPORATION 1995
 * LICENSED MATERIAL  -  PROGRAM PROPERTY OF I B M
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
 * 
 */

#ifndef MW
 #define	sp   1
 #define	r0   0
 #define	r1   1
 #define	r2   2
 #define	r3   3
 #define	r4   4
 #define	r5   5
 #define	r6   6
 #define	r7   7
 #define	r8   8
 #define	r9   9
 #define	r10   10
 #define	r11   11
 #define	r12   12
 #define	r13   13
 #define	r14   14
 #define	r15   15
 #define	r16   16
 #define	r17   17
 #define	r18   18
 #define	r19   19
 #define	r20   20
 #define	r21   21
 #define	r22   22
 #define	r23   23
 #define	r24   24
 #define	r25   25
 #define	r26   26
 #define	r27   27
 #define	r28   28
 #define	r29   29
 #define	r30   30
 #define	r31   31
#endif

/*
 * Floating Point Registers
 */
 #define	fr0   0
 #define	fr1   1
 #define	fr2   2
 #define	fr3   3
 #define	fr4   4
 #define	fr5   5
 #define	fr6   6
 #define	fr7   7
 #define	fr8   8
 #define	fr9   9
 #define	fr10   10
 #define	fr11   11
 #define	fr12   12
 #define	fr13   13
 #define	fr14   14
 #define	fr15   15
 #define	fr16   16
 #define	fr17   17
 #define	fr18   18
 #define	fr19   19
 #define	fr20   20
 #define	fr21   21
 #define	fr22   22
 #define	fr23   23
 #define	fr24   24
 #define	fr25   25
 #define	fr26   26
 #define	fr27   27
 #define	fr28   28
 #define	fr29   29
 #define	fr30   30
 #define	fr31   31

/* Or, some code uses 'f' instead of 'fr'...
 */
 #define	f0   0
 #define	f1   1
 #define	f2   2
 #define	f3   3
 #define	f4   4
 #define	f5   5
 #define	f6   6
 #define	f7   7
 #define	f8   8
 #define	f9   9
 #define	f10   10
 #define	f11   11
 #define	f12   12
 #define	f13   13
 #define	f14   14
 #define	f15   15
 #define	f16   16
 #define	f17   17
 #define	f18   18
 #define	f19   19
 #define	f20   20
 #define	f21   21
 #define	f22   22
 #define	f23   23
 #define	f24   24
 #define	f25   25
 #define	f26   26
 #define	f27   27
 #define	f28   28
 #define	f29   29
 #define	f30   30
 #define	f31   31

/*
 * Special Purpose Registers
 */
 #define	xer		0x01		/*  fixed point exception register */
 #define	lr		0x08		/*  link register */
 #define	ctr		0x09		/*  count register */
 #define	srr0	0x1a		/*  save/restore register 0 */
 #define	srr1	0x1b		/*  save/restore register 1 */
 #define	dec		22			/*  decrementer register */
 #define	sprg0	272			/*  special general reg 0 */
 #define	sprg1	273			/*  special general reg 1 */
 #define	sprg2	274			/*  special general reg 2 */
 #define	sprg3	275			/*  special general reg 3 */
 #define	pvr		287			/*  processor version */

/*
 * Machine State Register     msr     Bit Masks
 */
 #define	pow   0x40000        /* Activates power management */
 #define	ile   0x10000        /* Interrupt little Endian */
 #define	ee   0x8000          /* external interrupt*/
 #define	pr   0x4000          /* problem state  */
 #define	fp   0x2000          /* floating point available */
 #define	me   0x1000          /* machine check */
 #define	fe0   0x0800         /* floating  point exception enable */
 #define	se   0x0400          /* single step trace enable*/
 #define	be   0x0200          /* branch trace enable*/
 #define	fe1   0x0100         /* floating  point exception enable*/
 #define	ip   0x0040          /* prefix */
 #define	ir   0x0020          /* instruction relocate*/
 #define	dr   0x0010          /* data relocate*/
 #define	ri   0x0002          /* recoverable exception */
 #define	le   0x0001          /* Little Endian mode */
 #define	MSR_EE   0x8000      /* external interrupt*/
 #define	MSR_PR   0x4000      /* problem state*/
 #define	MSR_FP   0x2000      /* floating point available*/
 #define	MSR_ME   0x1000      /* machine check*/
 #define	MSR_FE0   0x0800     /* floating point exception enable*/
 #define	MSR_SE   0x0400      /* single step trace enable*/
 #define	MSR_FE1   0x0100     /* floating point exception enable*/
 #define	MSR_EP   0x0040      /* prefix*/
 #define	MSR_IR   0x0020      /* instruction relocate*/
 #define	MSR_DR   0x0010      /* data relocate*/
 #define	MSR_RI   0x0002      /* Recoverable interrupt */
 #define	MSR_LE   0x0001      /* Little Endian */


/*
 * Branch Conditions
 */
 #define	false   0x04       /*branch false bo */
 #define	true   0x0c        /*branch true bo*/
 #define	falsectr   0x00    /*dec ctr branch false and ctr != 0 bo*/
 #define	falzezct   0x02    /*dec ctr branch false and ctr  = 0 bo*/
 #define	truectr   0x08     /*dec ctr branch true  and ctr != 0 bo*/
 #define	truezctr   0x0a    /*dec ctr branch true  and ctr  = 0 bo*/
 #define	always   0x14      /*branch unconditional bo*/
 #define	brctr   0x10       /*dec ctr    branch ctr != 0 bo */
 #define	brzctr   0x12      /*dec ctr    branch ctr  = 0 bo */
 #define	lt   0x00          /*less than condition     bit 0   */
 #define	gt   0x01          /*greater than condition     bit 1  */
 #define	eq   0x02          /*equal condition     bit 2 from 0 */
 #define	so   0x03          /*so bit in cr     bit 3  */
 #define	nolk   0x00        /*no link*/
 #define	lk   0x01          /*link*/

/*
 * Branch Conditions Alternate definitions
 */
 #define	BO_dCTR_NZERO_AND_NOT   0
 #define	BO_dCTR_NZERO_AND_NOT_1   1
 #define	BO_dCTR_ZERO_AND_NOT   2
 #define	BO_dCTR_ZERO_AND_NOT_1   3
 #define	BO_IF_NOT   4
 #define	BO_IF_NOT_1   5
 #define	BO_IF_NOT_2   6
 #define	BO_IF_NOT_3   7
 #define	BO_dCTR_NZERO_AND   8
 #define	BO_dCTR_NZERO_AND_1   9
 #define	BO_dCTR_ZERO_AND   10
 #define	BO_dCTR_ZERO_AND_1   11
 #define	BO_IF   12
 #define	BO_IF_1   13
 #define	BO_IF_2   14
 #define	BO_IF_3   15
 #define	BO_dCTR_NZERO   16
 #define	BO_dCTR_NZERO_1   17
 #define	BO_dCTR_ZERO   18
 #define	BO_dCTR_ZERO_1   19
 #define	BO_ALWAYS   20
 #define	BO_ALWAYS_1   21
 #define	BO_ALWAYS_2   22
 #define	BO_ALWAYS_3   23
 #define	BO_dCTR_NZERO_8   24
 #define	BO_dCTR_NZERO_9   25
 #define	BO_dCTR_ZERO_8   26
 #define	BO_dCTR_ZERO_9   27
 #define	BO_ALWAYS_8   28
 #define	BO_ALWAYS_9   29
 #define	BO_ALWAYS_10   30
 #define	BO_ALWAYS_11   31
/*
 * Condition Register Bit Fields
 */
#ifndef MW
 #define	cr0   0
 #define	cr1   1
 #define	cr2   2
 #define	cr3   3
 #define	cr4   4
 #define	cr5   5
 #define	cr6   6
 #define	cr7   7
#endif

/*
 * Condition Register Bit Sub-fields
*/

#define	cr0_0   0
#define	cr0_1   1
#define	cr0_2   2
#define	cr0_3   3

#define	cr1_0   4
#define	cr1_1   5
#define	cr1_2   6
#define	cr1_3   7

#define	cr2_0   8
#define	cr2_1   9
#define	cr2_2   10
#define	cr2_3   11

#define	cr3_0   12
#define	cr3_1   13
#define	cr3_2   14
#define	cr3_3   15

#define	cr4_0   16
#define	cr4_1   17
#define	cr4_2   18
#define	cr4_3   19

#define	cr5_0   20
#define	cr5_1   21
#define	cr5_2   22
#define	cr5_3   23

#define	cr6_0   24
#define	cr6_1   25
#define	cr6_2   26
#define	cr6_3   27

#define	cr7_0   28
#define	cr7_1   29
#define	cr7_2   30
#define	cr7_3   31
/*
 * Special Purpose Registers
 */
#define	zpr    0x3b0     /* zone protection register         (403GC) */
#define	pid    0x3b1     /* process id register              (403GC) */
#define	smr    0x3b8     /* storage mem-coherent (not implemented)   */
#define	sgr    0x3b9     /* storage guarded register         (403GC) */
#define	dcwr   0x3ba     /* data cache write-thru register   (403GC) */
#define	tbhu   0x3cc     /* user-mode time base high         (403GC) */
#define	tblu   0x3cd     /* user-mode time base low          (403GC) */
#define	icdbdr 0x3d3     /* instruction cache debug data reg (403GC) */
#define	esr    0x3d4     /* execption syndrome register              */
#define	dear   0x3d5     /* data exeption address register           */
#define	evpr   0x3d6     /* exeption vector prefix register          */
#define	cdbcr  0x3d7     /* cache debug control register     (403GC) */
#define	tsr    0x3d8     /* timer status register                    */
#define	tcr    0x3da     /* timer control register                   */
#define	pit    0x3db     /* programmable interval timer              */
#define	tbhi   0x3dc     /* time base high                           */
#define	tblo   0x3dd     /* time base low                            */
#define	srr2   0x3de     /* save/restore register 2                  */
#define	srr3   0x3df     /* save/restore register 3                  */
#define	dbsr   0x3f0     /* debug status register                    */
#define	dbcr   0x3f2     /* debug control register                   */
#define	dbcr0  0x3f2     /* debug control register 0 (405GP)         */
#define	dbcr1  0x3bd     /* debug control register 1 (405GP)         */
#define	iac1   0x3f4     /* instruction address comparator 1         */
#define	iac2   0x3f5     /* instruction address comparator 2         */
#define	iac3   0x3b4     /* instruction address comparator 3 (405GP) */
#define	iac4   0x3b5     /* instruction address comparator 4 (405GP) */
#define	dac1   0x3f6     /* data address comparator 1                */
#define	dac2   0x3f7     /* data address comparator 2                */
#define	dvc1   0x3b6     /* data value comparator 1 (405GP)          */
#define	dvc2   0x3b7     /* data value comparator 2 (405GP)          */
#define	dccr   0x3fa     /* data cache control register              */
#define	iccr   0x3fb     /* instruction cache control register       */
#define	pbl1   0x3fc     /* protection bound lower 1                 */
#define	pbu1   0x3fd     /* protection bound upper 1                 */
#define	pbl2   0x3fe     /* protection bound lower 2                 */
#define	pbu2   0x3ff     /* protection bound upper 2                 */


/*
 * Device Control Registers
 */
#define	exisr		0x40	/* external interrupt status register */
#define	exier		0x42	/* external interrupt enable register */
#define	br0			0x80	/* bank register 0 */
#define	br1			0x81	/* bank register 1 */
#define	br2			0x82	/* bank register 2 */
#define	br3			0x83	/* bank register 3 */
#define	br4			0x84	/* bank register 4 */
#define	br5			0x85	/* bank register 5 */
#define	br6			0x86	/* bank register 6 */
#define	br7			0x87	/* bank register 7 */
#define	bear		0x90	/* bus error address register */
#define	besr		0x91	/* bus error syndrome register */
#define	iocr		0xa0	/* input/output configuration register */
#define	dmacr0		0xc0	/* DMA channel control register 0 */
#define	dmact0		0xc1	/* DMA count register 0 */
#define	dmada0		0xc2	/* DMA destination address register 0 */
#define	dmasa0		0xc3	/* DMA source address register 0 */
#define	dmacc0		0xc4	/* DMA chained count 0 */
#define	dmacr1		0xc8	/* DMA channel control register 1 */
#define	dmact1		0xc9	/* DMA count register 1 */
#define	dmada1		0xca	/* DMA destination address register 1 */
#define	dmasa1		0xcb	/* DMA source address register 1  */
#define	dmacc1		0xcc	/* DMA chained count 1 */
#define	dmacr2		0xd0	/* DMA channel control register 2 */
#define	dmact2		0xd1	/* DMA count register 2 */
#define	dmada2		0xd2	/* DMA destination address register 2 */
#define	dmasa2		0xd3	/* DMA source address register 2  */
#define	dmacc2		0xd4	/* DMA chained count 2 */
#define	dmacr3		0xd8	/* DMA channel control register 3  */
#define	dmact3		0xd9	/* DMA count register 3 */
#define	dmada3		0xda	/* DMA destination address register 3 */
#define	dmasa3		0xdb	/* DMA source address register 3  */
#define	dmacc3		0xdc	/* DMA chained count 0 */
#define	dmasr		0xe0	/* DMA status register */

/*
 * Machine State Register   msr Bit Masks unique to 403GA
 */
#define	we  0x40000      /* wait state enable */
#define	ce  0x20000      /* critical interrupt enable */
#define	MSR_WE  0x40000  /* wait state enable */
#define	MSR_CE  0x20000  /* critical interrupt enable */
#define	MSR_DE  0x0200   /* debug enable */
#define	MSR_PE  0x0008   /* protection enable */
#define	MSR_PX  0x0004   /* protection exclusive mode */


#define	mtesr	mtspr esr,
#define	mtsrr2	mtspr srr2,
#define	mtsrr3	mtspr srr3,
#define	mtdcwr	mtspr dcwr,
#define	mttcr	mtspr tcr,
#define	mtevpr  mtspr evpr,
#define	mtsgr	mtspr sgr,
#define	mtdbsr	mtspr dbsr,
#define	mtdccr	mtspr dccr,
#define	mticcr	mtspr iccr,

#define	mtbr0	mtdcr br0,
#define	mtbr1	mtdcr br1,
#define	mtbr2	mtdcr br2,
#define	mtbr3	mtdcr br3,
#define	mtbr4	mtdcr br4,
#define	mtbr5	mtdcr br5,
#define	mtbr6	mtdcr br6,
#define	mtbr7	mtdcr br7,
#define	mtbesr	mtdcr besr,
#define	mtexier	mtdcr exier,
#define	mtexisr	mtdcr exisr,
#define	mtiocr	mtdcr iocr,

/* Note: if building with DIAB compiler, the "\reg", needs to be 
 * changed to "reg".
 */
#ifdef ASSEMBLY_ONLY
	.macro mfexisr	reg
		mfdcr	\reg,exisr
	.endm
	.macro mfexier	reg
		mfdcr	\reg,exier
	.endm
	.macro mfiocr	reg
		mfdcr	\reg,iocr
	.endm
	.macro mfesr	reg
		mfspr	\reg,esr
	.endm
	.macro mfsrr2	reg
		mfspr	\reg,srr2
	.endm
	.macro mfsrr3	reg
		mfspr	\reg,srr3
	.endm
	.macro mftbhu	reg
		mfspr	\reg,tbhu
	.endm
	.macro mftblu	reg
		mfspr	\reg,tblu
	.endm
	.macro mficcr	reg
		mfspr	\reg,iccr
	.endm
	.macro mfdccr	reg
		mfspr	\reg,dccr
	.endm
#endif


/*
 * PPC405GCX Stuff...
 */
#define CNTRL_DCR_BASE	0x0b0
#define SDRAM_DCR_BASE	0x10

#define	pb0ap			0x10	/* Peripheral Bank 0 Access Parameters */
#define	pb1ap			0x11	/* Peripheral Bank 1 Access Parameters */
#define	pb2ap			0x12	/* Peripheral Bank 2 Access Parameters */
#define	pb3ap			0x13	/* Peripheral Bank 3 Access Parameters */
#define	pb4ap			0x14	/* Peripheral Bank 4 Access Parameters */
#define	pb5ap			0x15	/* Peripheral Bank 5 Access Parameters */
#define	pb6ap			0x16	/* Peripheral Bank 6 Access Parameters */
#define	pb7ap			0x17	/* Peripheral Bank 7 Access Parameters */
#define	pb0cr			0x00	/* Peripheral Bank 0 Configuration Register */
#define	pb1cr			0x01	/* Peripheral Bank 1 Configuration Register */
#define	pb2cr			0x02	/* Peripheral Bank 2 Configuration Register */
#define	pb3cr			0x03	/* Peripheral Bank 3 Configuration Register */
#define	pb4cr			0x04	/* Peripheral Bank 4 Configuration Register */
#define	pb5cr			0x05	/* Peripheral Bank 5 Configuration Register */
#define	pb6cr			0x06	/* Peripheral Bank 6 Configuration Register */
#define	pb7cr			0x07	/* Peripheral Bank 7 Configuration Register */

#define mem_mcopt1		0x20	/* Memory Controller options 1 */
#define mem_mb0cf		0x40	/* Memory bacnk 0 configuration */
#define mem_mb1cf		0x44	/* Memory bacnk 1 configuration */
#define mem_mb2cf		0x48	/* Memory bacnk 2 configuration */
#define mem_mb3cf		0x4c	/* Memory bacnk 3 configuration */
#define mem_mb4cf		0x50	/* Memory bacnk 4 configuration */
#define mem_mb5cf		0x54	/* Memory bacnk 5 configuration */
#define mem_mb6cf		0x58	/* Memory bacnk 6 configuration */
#define mem_mb7cf		0x5c	/* Memory bacnk 7 configuration */

#define	ebccfga			0x12	/* Peripheral Controller Address Register */
#define	ebccfgd			0x13	/* Peripheral Controller Data Register */
#define	ebc0cfg			0x23	/* EBC Configuration Register */

#define pllmd   (CNTRL_DCR_BASE+0x0)  /* PLL mode  register */
#define cntrl0  (CNTRL_DCR_BASE+0x1)  /* Control 0 register */
#define cntrl1  (CNTRL_DCR_BASE+0x2)  /* Control 1 register */
#define resetr  (CNTRL_DCR_BASE+0x3)  /* reset register */
#define strapr  (CNTRL_DCR_BASE+0x4)  /* strap register */
/* OR... */
#define cpc0_pllmr		(CNTRL_DCR_BASE+0x00)
#define cpc0_cr0		(CNTRL_DCR_BASE+0x01)
#define cpc0_cr1		(CNTRL_DCR_BASE+0x02)
#define cpc0_psr		(CNTRL_DCR_BASE+0x04)
#define cpc0_jtagid		(CNTRL_DCR_BASE+0x05)
#define cpc0_sr			(CNTRL_DCR_BASE+0x08)
#define cpc0_er			(CNTRL_DCR_BASE+0x09)
#define cpc0_fr			(CNTRL_DCR_BASE+0x0a)

#define memcfga  (SDRAM_DCR_BASE+0x0)   /* Memory configuration address reg  */
#define memcfgd  (SDRAM_DCR_BASE+0x1)   /* Memory configuration data    reg  */

/* values for memcfga register - indirect addressing of these regs */
#define mem_besra		0x00	/* bus error syndrome reg a */
#define mem_besrsa		0x04	/* bus error syndrome reg set a */
#define mem_besrb		0x08	/* bus error syndrome reg b */
#define mem_besrsb		0x0c	/* bus error syndrome reg set b */
#define mem_bear		0x10	/* bus error address reg */
#define mem_mcopt1		0x20	/* memory controller options 1 */
#define mem_rtr			0x30	/* refresh timer reg */
#define mem_pmit		0x34	/* power management idle timer */
#define mem_mb0cf		0x40	/* memory bank 0 configuration */
#define mem_mb1cf		0x44	/* memory bank 1 configuration */
#define mem_mb2cf		0x48	/* memory bank 2 configuration */
#define mem_mb3cf		0x4c	/* memory bank 3 configuration */
#define mem_sdtr1		0x80	/* timing reg 1 */
#define mem_ecccf		0x94	/* ECC configuration */
#define mem_eccerr		0x98	/* ECC error status */

#define ppcMsrPOW		0x00040000	/* Power Management                */
#define ppcMsrILE		0x00010000	/* Interrupt Little Endian         */
#define ppcMsrEE		0x00008000	/* External Interrupt Enable       */
#define ppcMsrPR		0x00004000	/* Problem State                   */
#define ppcMsrFP		0x00002000	/* Floating-Point Available        */
#define ppcMsrME		0x00001000	/* Machine Check Enable            */
#define ppcMsrFE0		0x00000800	/* Floating-Point Mode 0           */
#define ppcMsrSE		0x00000400	/* Single-Step Trace Enable        */
#define ppcMsrBE		0x00000200	/* Branch Trace Enable             */
#define ppcMsrFE1		0x00000100	/* Floating-Point Mode 1           */
#define ppcMsrIP		0x00000040	/* Interrupt Prefix                */
#define ppcMsrIR		0x00000020	/* Instruction relocate            */
#define ppcMsrDR		0x00000010	/* Data relocate                   */
#define ppcMsrRI		0x00000002	/* Recoverable interrupt           */
#define ppcMsrLE		0x00000001	/* Little Endian                   */

/* PPC750-specific 
 */
#define hid0	1008
#define hid1	1009
#define hid2	1016
#define HID0	1008
#define ibat0u	528
#define ibat0l	529
#define ibat1u	530
#define ibat1l	531
#define ibat2u	532
#define ibat2l	533
#define ibat3u	534
#define ibat3l	535
#define ibat4u	560
#define ibat4l	561
#define ibat5u	562
#define ibat5l	563
#define ibat6u	564
#define ibat6l	565
#define ibat7u	566
#define ibat7l	567
#define dbat0u	536
#define dbat0l	537
#define dbat1u	538
#define dbat1l	539
#define dbat2u	540
#define dbat2l	541
#define dbat3u	542
#define dbat3l	543
#define dbat4u	568
#define dbat4l	569
#define dbat5u	570
#define dbat5l	571
#define dbat6u	572
#define dbat6l	573
#define dbat7u	574
#define dbat7l	575
#define pvr		287
#define l2cr	1017

/* general BAT defines for bit settings to compose BAT regs */
/* represent all the different block lengths */
/* The BL field is part of the Upper Bat Register */
#define BAT_BL_128K		0x00000000
#define BAT_BL_256K		0x00000004
#define BAT_BL_512K		0x0000000C
#define BAT_BL_1M		0x0000001C
#define BAT_BL_2M		0x0000003C
#define BAT_BL_4M		0x0000007C
#define BAT_BL_8M		0x000000FC
#define BAT_BL_16M		0x000001FC
#define BAT_BL_32M		0x000003FC
#define BAT_BL_64M		0x000007FC
#define BAT_BL_128M		0x00000FFC
#define BAT_BL_256M		0x00001FFC

/* supervisor/user valid mode definitions - Upper BAT*/
#define BAT_VALID_SUPERVISOR	0x00000002
#define BAT_VALID_USER			0x00000001
#define BAT_INVALID				0x00000000

/* WIMG bit settings - Lower BAT */
#define BAT_WRITE_THROUGH		0x00000040
#define BAT_CACHE_INHIBITED		0x00000020
#define BAT_COHERENT			0x00000010
#define BAT_GUARDED				0x00000008

/* Protection bits - Lower BAT */
#define BAT_NO_ACCESS			0x00000000
#define BAT_READ_ONLY			0x00000001
#define BAT_READ_WRITE			0x00000002

#define BAT_BEPI_MASK			0xFFFE0000
#define BAT_BRPN_MASK			0xFFFE0000
#define BAT_BL_MASK				0x00001FFC
#define BAT_WIMG_MASK			0x00000078
#define BAT_WIM_MASK			0x00000070
#define BAT_WI_MASK				0x00000060
#define BAT_VALID_MASK			0x00000003
#define BAT_PROTECT_MASK		0x00000003

/* Bit defines for the L2CR register */
#define L2CR_L2E				0x80000000	/* bit 0 - enable */
#define L2CR_L2PE				0x40000000	/* bit 1 - data parity */
#define L2CR_L2SIZ_2M			0x00000000	/* bits 2-3 2 MB; MPC7400 ONLY! */
#define L2CR_L2SIZ_1M			0x30000000	/* bits 2-3 1MB */
#define L2CR_L2SIZ_HM			0x20000000	/* bits 2-3 512K */
#define L2CR_L2SIZ_QM			0x10000000	/* bits 2-3 256K; MPC750 ONLY */
#define L2CR_L2CLK_1			0x02000000	/* bits 4-6 Clock Ratio div 1 */
#define L2CR_L2CLK_1_5			0x04000000	/* bits 4-6 Clock Ratio div 1.5 */
#define L2CR_L2CLK_2			0x08000000	/* bits 4-6 Clock Ratio div 2 */
#define L2CR_L2CLK_2_5			0x0a000000	/* bits 4-6 Clock Ratio div 2.5 */
#define L2CR_L2CLK_3			0x0c000000	/* bits 4-6 Clock Ratio div 3 */
#define L2CR_L2RAM_BURST		0x01000000	/* bits 7-8 burst SRAM */
#define L2CR_DO					0x00400000	/* bit 9 Enable caching of instr */
											/* in L2 */
#define L2CR_L2I				0x00200000	/* bit 10 Global invalidate bit */
#define L2CR_TS					0x00040000	/* bit 13 Test support on */
#define L2CR_TS_OFF				~L2CR_TS	/* bit 13 Test support off */
#define L2CR_L2OH_5				0x00000000	/* bits 14-15 Output Hold */
											/* time = 0.5ns */
#define L2CR_L2OH_1				0x00010000	/* bits 14-15 Output Hold */
											/* time = 1.0ns */
#define L2CR_L2OH_INV			0x00020000	/* bits 14-15 Output Hold */
											/* time = 1.0ns */

/* PPC405GP-specific DCRs ...
 */
#define ocm0_isarc	0x018	/* OCM instruction side address range compare	*/
#define ocm0_isctl	0x019	/* OCM instruction side control					*/
#define ocm0_dsarc	0x01a	/* OCM data side address range compare			*/
#define ocm0_dsctl	0x01b	/* OCM data side control						*/

#define	uic_sr		0x0c0	/* UIC Status */
#define	uic_er		0x0c2	/* UIC Enable */
#define	uic_cr		0x0c3	/* UIC Critical  */
#define	uic_pr		0x0c4	/* UIC Poloarity */
#define	uic_tr		0x0c5	/* UIC Trigger */
#define	uic_msr		0x0c6	/* UIC Masked Status  */
#define	uic_vr		0x0c7	/* UIC Vector Configuration */
#define	uic_vsr		0x0c8	/* UIC Vector */

/* MPC860 specific... */
/* Most of the 860 masks are provided by the masks860.h file found on  */
/* Motorola's website.  These additional ones are added here so that the */
/* masks860.h file from moto can be left untouched. */
#define	EIE			80	/* External Interrupt Enable */
#define EID			81	/* External Interrupt Disable */
#define NRI			82	/* Non Recoverable Interrupt */
#define CMPA		144	/* Comparator A value register */
#define CMPB		145	/* Comparator B value register */
#define CMPC		146	/* Comparator C value register */
#define CMPD		147	/* Comparator D value register */
#define ICR			148	/* Interrupt Cause Register */
#define DER			149	/* Debug Enable Register*/
#define COUNTA		150	/* Breakpoint Counter A value & control reg. */
#define COUNTB		151	/* Breakpoint Counter B value & control reg. */
#define CMPE		152	/* Comparator E value register */
#define CMPF		153	/* Comparator F value register */
#define CMPG		154	/* Comparator G value register */
#define CMPH		155	/* Comparator H value register */
#define LCTRL1		156	/* Load store support comparator Control reg. */
#define LCTRL2		157	/* Load store support AND-OR Control register */
#define ICTRL		158	/* Instruction support Control register */
#define BAR			159	/* Breakpoint Address register */

#define IC_CST		560	/* Instruction Cache Control and Status reg. */
#define IC_ADR		561	/* Instruction Cache Address register */
#define IC_DAT		562	/* Instruction Cache Data port */
#define DC_CST		568	/* Data Cache Control and Status register */
#define DC_ADR		569	/* Data Cache Address register */
#define DC_DAT		570	/* Data Cache Data port*/

#define DPDR		630	/* Development Port Data Register */
#define DPIR		631	/* */
#define IMMR		638	/* */
#define immr		638

#define MI_CTR		784	/* Instruction MMU Control Register */
#define MI_AP		786	/* Instruction MMU Access Protection Register */
#define MI_EPN		787	/* Instruction MMU Effective Number Register */
#define MI_TWC		789	/* Instruction MMU Tablewalk control Register */
#define MI_RPN		790	/* Instruction MMU Real Page Number Port */

#define MD_CTR		792	/* Data MMU Control Register */
#define M_CASID		793	/* CASID register */
#define MD_AP		794	/* Data Access Protection Register */
#define MD_EPN		795	/* Data Effective Number Register */
#define M_TWB		796	/* MMU Tablewalk base register */
#define MD_TWC		797	/* Data Tablewalk control Register */
#define MD_RPN		798	/* Data Real Page Number Port */
#define M_TW		799	/* MMU Tablewalk special register */

#define MI_DBCAM	816	/* Instruction MMU CAM entry read register */
#define MI_DBRAM0	817	/* Instruction MMU RAM entry read register 0 */
#define MI_DBRAM1	818	/* Instruction MMU RAM entry read register 1 */
#define MD_DBCAM	824	/* Data MMU CAM entry read register */
#define MD_DBRAM0	825	/* Data MMU RAM entry read register 0 */
#define MD_DBRAM1	826	/* Data MMU RAM entry read register 1 */

#define PLPRCR_MF7		0x00700000	/* Multiplication factor = 7 */
#define SIUMCR_DBGC3	0x00300000
#define ISCT_SER7		0x00000007	/* Core is not serialized (normal mode) */
									/* and no show cycles will be performed */
									/* for fetched instructions. */


#define MPTPR_PTP_DIV2	0x2000		/* BRGCLK divided by 2 */
#define MPTPR_PTP_DIV4	0x1000		/* BRGCLK divided by 4 */
#define MPTPR_PTP_DIV8	0x0800		/* BRGCLK divided by 8 */
#define MPTPR_PTP_DIV16	0x0400		/* BRGCLK divided by 16 */
#define MPTPR_PTP_DIV32	0x0200		/* BRGCLK divided by 32 */
#define MPTPR_PTP_DIV64	0x0100		/* BRGCLK divided by 64 */

#define SIUMCR_BASE		0x0000
#define PLPRCR_BASE		0x0284
#define PLPRCRK_BASE	0x0384		/* MPC866 */
#define SCCR_BASE		0x0280		/* MPC866 */
#define SCCRK_BASE		0x0380		/* MPC866 */
#define PBPAR_BASE		0x0abc
#define PBDIR_BASE		0x0ab8
#define PBDAT_BASE		0x0ac4
#define SYPCR_BASE		0x0004
#define SWSR_BASE		0x000e
#define CPCR_BASE		0x09c0
#define SIMASK_BASE		0x0014
#define CIPR_BASE		0x0944
#define CIMR_BASE		0x0948
#define SICR_BASE		0x0aec
#define SDCR_BASE		0x0030
#define PISCR_BASE		0x0240
#define MAR_BASE		0x0164
#define MCR_BASE		0x0168
#define MDR_BASE		0x017c
#define MAMR_BASE		0x0170
#define MBMR_BASE		0x0174		/* MPC866 */
#define MPTPR_BASE		0x017a
#define BR0_BASE		0x0100
#define OR0_BASE		0x0104
#define BR1_BASE		0x0108
#define OR1_BASE		0x010c
#define BR2_BASE		0x0110
#define OR2_BASE		0x0114
#define BR3_BASE		0x0118
#define OR3_BASE		0x011c
#define BR4_BASE		0x0120
#define OR4_BASE		0x0124
#define BR5_BASE		0x0128
#define OR5_BASE		0x012c

#ifndef ASSEMBLY_ONLY
extern void				ppcAbend(void);
extern unsigned long	ppcAndMsr(unsigned long value);
extern unsigned long	ppcCntlzw(unsigned long value);

extern void				ppcDcbi(void *addr);
extern void				ppcDcbf(void *addr);
extern void				ppcDcbst(void *addr);
extern void				ppcDcbz(void *addr);
extern void				ppcHalt(void);
extern void				ppcIcbi(void *addr);
extern void				ppcIsync(void);
extern unsigned long	ppcMfgpr1(void);
extern unsigned long 	ppcMfgpr2(void);
extern unsigned long	ppcMfmsr(void);
extern unsigned long	ppcMfsprg0(void);
extern unsigned long	ppcMfsprg1(void);
extern unsigned long	ppcMfsprg2(void);
extern unsigned long	ppcMfsprg3(void);
extern unsigned long 	ppcMfsrr0(void);
extern unsigned long	ppcMfsrr1(void);
extern unsigned long	ppcMfpvr(void);
extern unsigned long	ppcMfdccr(void);
extern unsigned long	ppcMficcr(void);
extern unsigned long	ppcMfevpr(void);
extern void				ppcMtevpr(unsigned long);
extern void				ppcMticcr(unsigned long);
extern void				ppcMtdccr(unsigned long);
extern void				ppcMtmsr(unsigned long msr_value);
extern void				ppcMtsprg0(unsigned long sprg_value);
extern void				ppcMtsprg1(unsigned long sprg_value);
extern void				ppcMtsprg2(unsigned long sprg_value);
extern void				ppcMtsprg3(unsigned long sprg_value);
extern void				ppcMtsrr0(unsigned long srr0_value);
extern void 			ppcMtsrr1(unsigned long srr1_value);
extern unsigned long	ppcOrMsr(unsigned long value);
extern void				ppcSync(void);
extern void				ppcEieio(void);

extern void				ppcMtocm0isctl(unsigned long);
extern void				ppcMtocm0isarc(unsigned long);
extern void				ppcMtocm0dsctl(unsigned long);
extern void				ppcMtocm0dsarc(unsigned long);
extern void				ppcMtcntrl0(unsigned long);
extern unsigned long	ppcMfocm0isarc(void);
extern unsigned long	ppcMfocm0isctl(void);
extern unsigned long	ppcMfocm0dsarc(void);
extern unsigned long	ppcMfocm0dsctl(void);
extern unsigned long	ppcMfcntrl0(void);

extern void 			ppcMtdbcr0(unsigned long);
extern void 			ppcMtdbcr1(unsigned long);
extern void 			ppcMtdbsr(unsigned long);
extern void 			ppcMtiac1(unsigned long);
extern void 			ppcMtiac2(unsigned long);
extern void 			ppcMtiac3(unsigned long);
extern void 			ppcMtiac4(unsigned long);
extern void 			ppcMtdac1(unsigned long);
extern void 			ppcMtdac2(unsigned long);
extern void 			ppcMtdvc1(unsigned long);
extern void 			ppcMtdvc2(unsigned long);
extern void 			ppcMtexier(unsigned long);
extern void 			ppcMtexisr(unsigned long);
extern void				ppcMtcr(unsigned long);
extern void				ppcMtimmr(unsigned long);
extern void				ppcMtsp(unsigned long);
extern void				ppcMtiabr(unsigned long);
extern void				ppcMtdabr(unsigned long);
extern unsigned long	ppcMfdbcr0(void);
extern unsigned long	ppcMfdbcr1(void);
extern unsigned long	ppcMfdbsr(void);
extern unsigned long	ppcMfiac1(void);
extern unsigned long	ppcMfiac2(void);
extern unsigned long	ppcMfiac3(void);
extern unsigned long	ppcMfiac4(void);
extern unsigned long	ppcMfdac1(void);
extern unsigned long	ppcMfdac2(void);
extern unsigned long	ppcMfdvc1(void);
extern unsigned long	ppcMfdvc2(void);
extern unsigned long	ppcMfsp(void);
extern unsigned long	ppcMfcr(void);
extern unsigned long	ppcMfexier(void);
extern unsigned long	ppcMfexisr(void);
extern unsigned long	ppcMfimmr(void);

extern unsigned long baseofExceptionTable(void);
#endif

