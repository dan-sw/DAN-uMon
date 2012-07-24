/*
 * %W% %E%
 *
 * Copyright (c) 1998-1999 Lucent Technologies, Inc.  All Rights Reserved.
 *
 * This software is the confidential and proprietary information of Lucent
 * Technologies, Inc. ("Confidential Information").  You shall not disclose
 * such Confidential Information and shall use it only in accordance with
 * the terms of the license agreement you entered into with Lucent.
 *
 * LUCENT TECHNOLOGIES MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE
 * SUITABILITY OF THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT
 * NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 * A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.  LUCENT TECHNOLOGIES SHALL
 * NOT BE LIABLE FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING,
 * MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.
 */

/*******************************************************************************
*
*	cpu403.h
*
*	CPU definitions for the Power PC PPC403GCX.
*
*	May 1998	S. McCaskill
*	Mar 1999	R. Snyder		Add cpu.s functions as asm macros
*	May 1999	R. Snyder		Add more serial port definitions
*
*	This file includes rewrites of some of the functions from Scott McCaskill's
*	cpu.s file.  See cpu403.c for the others.  It implements functions to
*	access the PowerPC's special purpose and device control registers.  The
*	Diab/Data assembler does not support a "repeat" directive, so Scott's
*	cpu.s file could not be assembled.
*
*	The following functions are provided:
*
*		value = CPU_Read_SPR(index)
*		value = CPU_Read_DCR(index)
*
*	return the value of the register whose index is specified.
*
*		CPU_Write_SPR(index,value)
*		CPU_Write_DCR(index,value)
*
*	write a value into the register whose index is specified.
*
*	WARNING, unlike Scott's routines, reading from a non-existent register or
*	writing to a non-existent register have undefined/undesired effects.
*
*	For  additional  information,  consult chapter 12 of the "IBM
*	PPC403GCX Embedded Controller User's Manual".
*
*******************************************************************************/
#ifndef CPU403_H
#define CPU403_H

/*
*	Declare access functions in cpu.s
*/

#if 0
asm unsigned long   CPU_Read_SPR(unsigned int reg)
{
% con reg
    mfspr   r3, reg
}

asm void	    CPU_Write_SPR(unsigned int reg, unsigned long value)
{
% con reg; reg value
    mtspr   reg, value
}

asm unsigned long   CPU_Read_DCR(unsigned int reg)
{
% con reg
    mfdcr   r3, reg
}

asm void	    CPU_Write_DCR(unsigned int reg, unsigned long value)
{
% con reg; reg value
    mtdcr   reg, value
}

asm unsigned long   CPU_Read_MSR()
{
    mfmsr   r3
}

asm void	    CPU_Write_MSR(unsigned long value)
{
% reg value
    mtmsr   r3
}

extern void		CPU_Read_TB(unsigned long *pTimeBase);
extern void		CPU_uwait(unsigned long uSecs);
extern unsigned long	CPU_usec_Ticks();
#endif

/*
*	Special Purpose Registers.
*/

#define CPU_SPR_XER			0x001	/* Fixed point exception register */

#define CPU_SPR_LR			0x008	/* Link register		*/
#define CPU_SPR_CTR			0x009	/* Count register		*/

#define CPU_SPR_SRR0		0x01a	/* Save/restore register 0	*/
#define CPU_SPR_SRR1		0x01b	/* Save/restore register 1	*/

#define CPU_SPR_SPRG0		0x110	/* Special general register 0	*/
#define CPU_SPR_SPRG1		0x111	/* Special general register 1	*/
#define CPU_SPR_SPRG2		0x112	/* Special general register 2	*/
#define CPU_SPR_SPRG3		0x113	/* Special general register 3	*/

#define CPU_SPR_PVR			0x11f	/* Processor version register	*/

#define CPU_SPR_ZPR			0x3b0	/* Zone protection register	*/
#define CPU_SPR_PID			0x3b1	/* Process ID			*/
#define CPU_SPR_SGR			0x3b9	/* Storage guarded register	*/
#define CPU_SPR_DCWR		0x3ba	/* Data cache write-through register */

#define CPU_SPR_TBHU		0x3cc	/* Time base high user-mode	*/
#define CPU_SPR_TBLU		0x3cd	/* Time base low user-mode	*/

#define CPU_SPR_ICDBDR		0x3d3	/* Inst cache debug data register */
#define CPU_SPR_ESR			0x3d4	/* Exception syndrome register	*/
#define CPU_SPR_DEAR		0x3d5	/* Data error address register	*/
#define CPU_SPR_EVPR		0x3d6	/* Exception vector prefix register */
#define CPU_SPR_CDBCR		0x3d7	/* Cache debug control register	*/
#define CPU_SPR_TSR			0x3d8	/* Timer status register	*/

#define CPU_SPR_TCR			0x3da	/* Timer control register	*/
#define CPU_SPR_PIT			0x3db	/* Programmable interval timer	*/
#define CPU_SPR_TBHI		0x3dc	/* Time base high		*/
#define CPU_SPR_TBLO		0x3dd	/* Time base low		*/
#define CPU_SPR_SRR2		0x3de	/* Save/restore register 2	*/
#define CPU_SPR_SRR3		0x3df	/* Save/restore register 3	*/

#define CPU_SPR_DBSR		0x3f0	/* Debug status register	*/
#define CPU_SPR_DBCR		0x3f2	/* Debug control register	*/
#define CPU_SPR_IAC1		0x3f4	/* Instruction address compare 1 */
#define CPU_SPR_IAC2		0x3f5	/* Instruction address compare 2 */
#define CPU_SPR_DAC1		0x3f6	/* Data address compare 1	*/
#define CPU_SPR_DAC2		0x3f7	/* Data address compare 2	*/

#define CPU_SPR_DCCR		0x3fa	/* Data cache cacheability register */
#define CPU_SPR_ICCR		0x3fb	/* Inst cache cacheability register */
#define CPU_SPR_PBL1		0x3fc	/* Protection bound lower 1	*/
#define CPU_SPR_PBU1		0x3fd	/* Protection bound upper 1	*/
#define CPU_SPR_PBL2		0x3fe	/* Protection bound lower 2	*/
#define CPU_SPR_PBU2		0x3ff	/* Protection bound upper 2	*/

/*
*	Device Control Registers.
*/

#define CPU_DCR_EXISR		0x040	/* External interrupt status register */
#define CPU_DCR_EXIER		0x042	/* External interrupt enable register */

#define CPU_DCR_BRH0		0x070	/* Bank register high 0		*/
#define CPU_DCR_BRH1		0x071	/* Bank register high 1		*/
#define CPU_DCR_BRH2		0x072	/* Bank register high 2		*/
#define CPU_DCR_BRH3		0x073	/* Bank register high 3		*/
#define CPU_DCR_BRH4		0x074	/* Bank register high 4		*/
#define CPU_DCR_BRH5		0x075	/* Bank register high 5		*/
#define CPU_DCR_BRH6		0x076	/* Bank register high 6		*/
#define CPU_DCR_BRH7		0x077	/* Bank register high 7		*/

#define CPU_DCR_BR0			0x080	/* Bank register 0		*/
#define CPU_DCR_BR1			0x081	/* Bank register 1		*/
#define CPU_DCR_BR2			0x082	/* Bank register 2		*/
#define CPU_DCR_BR3			0x083	/* Bank register 3		*/
#define CPU_DCR_BR4			0x084	/* Bank register 4		*/
#define CPU_DCR_BR5			0x085	/* Bank register 5		*/
#define CPU_DCR_BR6			0x086	/* Bank register 6		*/
#define CPU_DCR_BR7			0x087	/* Bank register 7		*/

#define CPU_DCR_BEAR		0x090	/* Bus error address register	*/
#define CPU_DCR_BESR		0x091	/* Bus error syndrome register	*/

#define CPU_DCR_IOCR		0x0a0	/* I/O configuration register	*/

#define CPU_DCR_DMACR0		0x0c0	/* DMA channel control register 0 */
#define CPU_DCR_DMACT0		0x0c1	/* DMA count register 0		*/
#define CPU_DCR_DMADA0		0x0c2	/* DMA destination address register 0 */
#define CPU_DCR_DMASA0		0x0c3	/* DMA source address register 0 */
#define CPU_DCR_DMACC0		0x0c4	/* DMA chained count 0		*/

#define CPU_DCR_DMACR1		0x0c8	/* DMA channel control register 1 */
#define CPU_DCR_DMACT1		0x0c9	/* DMA count register 1		*/
#define CPU_DCR_DMADA1		0x0ca	/* DMA destination address register 1 */
#define CPU_DCR_DMASA1		0x0cb	/* DMA source address register 1 */
#define CPU_DCR_DMACC1		0x0cc	/* DMA chained count 1		*/

#define CPU_DCR_DMACR2		0x0d0	/* DMA channel control register 2 */
#define CPU_DCR_DMACT2		0x0d1	/* DMA count register 2		*/
#define CPU_DCR_DMADA2		0x0d2	/* DMA destination address register 2 */
#define CPU_DCR_DMASA2		0x0d3	/* DMA source address register 2 */
#define CPU_DCR_DMACC2		0x0d4	/* DMA chained count 2		*/

#define CPU_DCR_DMACR3		0x0d8	/* DMA channel control register 3 */
#define CPU_DCR_DMACT3		0x0d9	/* DMA count register 3		*/
#define CPU_DCR_DMADA3		0x0da	/* DMA destination address register 3 */
#define CPU_DCR_DMASA3		0x0db	/* DMA source address register 3 */
#define CPU_DCR_DMACC3		0x0dc	/* DMA chained count 3		*/

#define CPU_DCR_DMASR		0x0e0	/* DMA status register		*/

/*
*	BESR definitions.
*/

#define CPU_BESR_DSES			0x80000000	/* Data-side error	*/
#define CPU_BESR_DMES			0x40000000	/* DMA operation error	*/
#define CPU_BESR_RWS			0x20000000	/* Read/write status	*/
#define CPU_BESR_ET				0x1c000000	/* Error type mask	*/

#define CPU_BESR_ET_PROT		0x00000000	/* Protection violation	*/
#define CPU_BESR_ET_PARITY		0x04000000	/* Parity error		*/
#define CPU_BESR_ET_NOTCONFIG	0x08000000	/* Access not configured */
#define CPU_BESR_ET_BUS			0x10000000	/* Bus error		*/
#define CPU_BESR_ET_TIMEOUT		0x18000000	/* Bus timeout		*/

/*
*	Bank Register common definitions.
*
*	Note  that  only  BR4..BR7 may be used for DRAM. BR0..BR3 are
*	hard-wired  for  SRAM;  their CPU_BR_SD bits are ignored when
*	written, and return zero when read.
*/

#define CPU_BR_BAS			0xff000000	/* Base address of bank	*/
#define CPU_BR_BS			0x00e00000	/* Bank size		*/
#define CPU_BR_BU_WRITE		0x00100000	/* Bank is writeable	*/
#define CPU_BR_BU_READ		0x00080000	/* Bank is readable	*/
#define CPU_BR_SLF			0x00040000	/* Sequential line fill	*/
#define CPU_BR_BW			0x00018000	/* Bus width		*/
#define CPU_BR_SD			0x00000001	/* Set for SRAM		*/

#define CPU_BR_BASE_ADDR(n) (((n)&0x0ff00000)<<4) /* Macro to set base addr */

/*
*	Bank Register bank size definitions.
*/

#define CPU_BR_BS_1MB		0x00000000
#define CPU_BR_BS_2MB		0x00200000
#define CPU_BR_BS_4MB		0x00400000
#define CPU_BR_BS_8MB		0x00600000
#define CPU_BR_BS_16MB		0x00800000
#define CPU_BR_BS_32MB		0x00a00000
#define CPU_BR_BS_64MB		0x00c00000

/*
*	Bank Register bus width definitions.
*/

#define CPU_BR_BW_8			0x00000000	/* 8-bit bus		*/
#define CPU_BR_BW_16		0x00008000	/* 16-bit bus		*/
#define CPU_BR_BW_32		0x00010000	/* 32-bit bus		*/

/*
*	Bank Register SRAM definitions.
*/

#define CPU_BR_SRAM_SLF		0x00040000	/* Sequential line fill */
#define CPU_BR_SRAM_BME		0x00020000	/* Burst mode enable	*/
#define CPU_BR_SRAM_RE		0x00004000	/* Ready enable		*/
#define CPU_BR_SRAM_TWT		0x00003f00	/* Transfer wait	*/
#define CPU_BR_SRAM_CSN		0x00000080	/* Chip select timing	*/
#define CPU_BR_SRAM_OEN		0x00000040	/* Output enable timing	*/
#define CPU_BR_SRAM_WBN		0x00000020	/* Write byte enable timing */
#define CPU_BR_SRAM_WBF		0x00000010	/* Write byte disable timing */
#define CPU_BR_SRAM_TH		0x0000000e	/* Transfer hold	*/

/*
*	Bank Register SRAM transfer value macros.
*
*	For CPU_BR_SRAM_BURST, the maximum values are:
*
*		f <= 15		Wait states on first transfer.
*		n <= 3		Wait states on subsequent transfers.
*		h <= 7		Idle bus cycles at end of transfer.
*
*	For CPU_BR_SRAM_NOBURST, the maximum values are:
*
*		n <= 63		Wait states on single transfer.
*		h <= 7		Idle bus cycles at end of transfer.
*
*	Note that the CPU_BR_SRAM_BURST macro sets the CPU_BR_BME bit.
*/

#define CPU_BR_SRAM_BURST(f,n,h) (0x00020000|((f)<<10)|((n)<<8)|((h)<<1))
#define CPU_BR_SRAM_NOBURST(n,h) (((n)<<8)|((h)<<1))

/*
*	Bank Register DRAM bus definitions.
*/

#define CPU_BR_DRAM_ERM		0x00020000	/* Early RAS mode	*/
#define CPU_BR_DRAM_IEM		0x00004000	/* External multiplex	*/
#define CPU_BR_DRAM_RCT		0x00002000	/* RAS-to-CAS timing	*/
#define CPU_BR_DRAM_ARM		0x00001000	/* Alternate refresh	*/
#define CPU_BR_DRAM_PM		0x00000800	/* Page mode		*/
#define CPU_BR_DRAM_FAC		0x00000600	/* First access timing	*/
#define CPU_BR_DRAM_BAC		0x00000180	/* Burst access timing	*/
#define CPU_BR_DRAM_PCC		0x00000040	/* Precharge cycles	*/
#define CPU_BR_DRAM_RAR		0x00000020	/* RAS active on refresh */
#define CPU_BR_DRAM_RR		0x0000001e	/* Refresh interval	*/

/*
*	Bank Register DRAM access value macros.
*
*	Maximum values are:
*
*		f <= 3		Wait states on first access of burst.
*		n <= 3		Wait states on subsequent accesses of burst.
*
*	Actual  timings  are  one,  two, or three SysClk cycles more;
*	consult the manual.
*/

#define CPU_BR_DRAM_ACCESS(f,n) (((f)<<9)|((n)<<7))

/*
*	Bank Register DRAM refresh value macro.
*
*	Maximum value is <= 15.
*/

#define CPU_BR_DRAM_REFRESH(n)  ((n)<<1)

/*
*	Bank Register High definitions.
*/

#define CPU_BRH_PCE		0x80000000	/* Parity check enable	*/

/*
*	Cache Debug Control Register definitions.
*/

#define CPU_CDBCR_CIS		0x00000010	/* Cache information select */
#define CPU_CDBCR_CSS		0x00000001	/* Cache side select	*/

/*
*	Debug Control Register definitions.
*/

#define CPU_DBCR_EDM		0x80000000	/* External debug mode	*/
#define CPU_DBCR_IDM		0x40000000	/* Internal debug mode	*/
#define CPU_DBCR_RST		0x30000000	/* Reset		*/
#define CPU_DBCR_IC			0x08000000	/* Instruction completion */
#define CPU_DBCR_BT			0x04000000	/* Branch taken		*/
#define CPU_DBCR_EDE		0x02000000	/* Exception		*/
#define CPU_DBCR_TDE		0x01000000	/* Trap			*/
#define CPU_DBCR_FER		0x00f80000	/* First events remaining */
#define CPU_DBCR_FT			0x00040000	/* Freeze timers	*/
#define CPU_DBCR_IA1		0x00020000	/* Inst address 1 enable */
#define CPU_DBCR_IA2		0x00010000	/* Inst address 2 enable */
#define CPU_DBCR_D1R		0x00008000	/* Data addr 1 read enable */
#define CPU_DBCR_D1W		0x00004000	/* Data addr 1 write enable */
#define CPU_DBCR_D1S		0x00003000	/* Data addr 1 size	*/
#define CPU_DBCR_D2R		0x00000800	/* Data addr 2 read enable */
#define CPU_DBCR_D2W		0x00000400	/* Data addr 2 write enable */
#define CPU_DBCR_D2S		0x00000300	/* Data addr 2 size	*/
#define CPU_DBCR_SBT		0x00000040	/* Second branch taken	*/
#define CPU_DBCR_SED		0x00000020	/* Second exception	*/
#define CPU_DBCR_STD		0x00000010	/* Second trap		*/
#define CPU_DBCR_SIA		0x00000008	/* Second IAC enable	*/
#define CPU_DBCR_SDA		0x00000004	/* Second DAC enable	*/
#define CPU_DBCR_JOI		0x00000002	/* JTAG outbound int enb */
#define CPU_DBCR_JII		0x00000001	/* JTAG inbound int enb	*/

/*
*	Debug Control Register reset field definitions.
*
*	Writing  one  of  these  patterns  to  the  DBCR will cause a
*	processor reset.
*/

#define CPU_DBCR_RST_CORE	0x10000000	/* Core reset		*/
#define CPU_DBCR_RST_CHIP	0x20000000	/* Chip reset		*/
#define CPU_DBCR_RST_SYSTEM	0x30000000	/* System reset		*/

/*
*	Debug Status Register definitions.
*
*	Bits  in  this  register are set to 1 by hardware, and may be
*	cleared by being written to 1 by software.
*/

#define CPU_DBSR_IC			0x80000000	/* Instruction completion */
#define CPU_DBSR_BT			0x40000000	/* Branch taken		*/
#define CPU_DBSR_EXC		0x20000000	/* Exception		*/
#define CPU_DBSR_TIE		0x10000000	/* Trap			*/
#define CPU_DBSR_UDE		0x08000000	/* Unconditional event	*/
#define CPU_DBSR_IA1		0x04000000	/* Inst address 1	*/
#define CPU_DBSR_IA2		0x02000000	/* Inst address 2	*/
#define CPU_DBSR_DR1		0x01000000	/* Data addr 1 read	*/
#define CPU_DBSR_DW1		0x00800000	/* Data addr 1 write	*/
#define CPU_DBSR_DR2		0x00400000	/* Data addr 2 read	*/
#define CPU_DBSR_DW2		0x00200000	/* Data addr 2 write	*/
#define CPU_DBSR_IDE		0x00100000	/* Imprecise event	*/

#define CPU_DBSR_MRR		0x00000300	/* Most recent reset	*/

#define CPU_DBSR_JIF		0x00000004	/* JTAG in buffer full	*/
#define CPU_DBSR_JIO		0x00000002	/* JTAG in buffer overrun */
#define CPU_DBSR_JOE		0x00000001	/* JTAG out buffer empty */

/*
*	DMA Channel Control Register definitions.
*/

#define CPU_DMACR_CE		0x80000000	/* Channel enable	*/
#define CPU_DMACR_CIE		0x40000000	/* Channel interrupt enable */
#define CPU_DMACR_TD		0x20000000	/* Transfer direction	*/
#define CPU_DMACR_PL		0x10000000	/* Peripheral location	*/
#define CPU_DMACR_PW		0x0c000000	/* Peripheral width	*/
#define CPU_DMACR_DAI		0x02000000	/* Dest addr increment	*/
#define CPU_DMACR_SAI		0x01000000	/* Source addr increment */
#define CPU_DMACR_CP		0x00800000	/* Channel priority	*/
#define CPU_DMACR_TM		0x00600000	/* Transfer mode	*/
#define CPU_DMACR_PSC		0x00180000	/* Peripheral setup cycles */
#define CPU_DMACR_PWC		0x0007e000	/* Peripheral wait cycles */
#define CPU_DMACR_PHC		0x00001c00	/* Peripheral hold cycles */
#define CPU_DMACR_ETD		0x00000200	/* EOT/TC pin direction	*/
#define CPU_DMACR_TCE		0x00000100	/* Terminal count enable */
#define CPU_DMACR_CH		0x00000080	/* Chaining enable	*/
#define CPU_DMACR_BME		0x00000040	/* Burst mode enable	*/
#define CPU_DMACR_ECE		0x00000020	/* EOT chain mode enable */
#define CPU_DMACR_TCD		0x00000010	/* TC chain mode disable */
#define CPU_DMACR_PCE		0x00000008	/* Parity check enable	*/

/*
*	DMA Channel Control Register peripheral width definitions.
*/

#define CPU_DMACR_PW_8		0x00000000	/* 8 bit peripheral	*/
#define CPU_DMACR_PW_16		0x04000000	/* 16 bit peripheral	*/
#define CPU_DMACR_PW_32		0x08000000	/* 32 bit peripheral	*/
#define CPU_DMACR_PW_MEM	0x0c000000	/* Memory-to-memory	*/

/*
*	DMA Channel Control Register transfer mode definitions.
*/

#define CPU_DMACR_TM_BUFFERED	0x00000000	/* Buffered mode	*/
#define CPU_DMACR_TM_FLYBY		0x00200000	/* Fly-by mode		*/
#define CPU_DMACR_TM_MEM_SW		0x00400000	/* SW initiated mem-to-mem */
#define CPU_DMACR_TM_MEM_HW		0x00600000	/* HW initiated mem-to-mem */

/*
*	DMA Channel Control Register peripheral timing definitions.
*
*	Maximum values are:
*
*		s <= 3		Peripheral setup cycles.
*		w <= 63		Peripheral wait cycles.
*		h <= 7		Peripheral hold cycles.
*/

#define CPU_DMACR_TIMING(s,w,h) (((s)<<19)|((w)<<13)|((h)<<10))

/*
*	DMA Status Register definitions.
*
*	Bits  in  this  register are set to 1 by hardware, and may be
*	cleared by being written to 1 by software.
*/

#define CPU_DMASR_CS0		0x80000000	/* Channel 0 TC reached	*/
#define CPU_DMASR_CS1		0x40000000	/* Channel 1 TC reached	*/
#define CPU_DMASR_CS2		0x20000000	/* Channel 2 TC reached	*/
#define CPU_DMASR_CS3		0x10000000	/* Channel 3 TC reached	*/

#define CPU_DMASR_TS0		0x08000000	/* Channel 0 EOT requested */
#define CPU_DMASR_TS1		0x04000000	/* Channel 1 EOT requested */
#define CPU_DMASR_TS2		0x02000000	/* Channel 2 EOT requested */
#define CPU_DMASR_TS3		0x01000000	/* Channel 3 EOT requested */

#define CPU_DMASR_RI0		0x00800000	/* Channel 0 error status */
#define CPU_DMASR_RI1		0x00400000	/* Channel 1 error status */
#define CPU_DMASR_RI2		0x00200000	/* Channel 2 error status */
#define CPU_DMASR_RI3		0x00100000	/* Channel 3 error status */

#define CPU_DMASR_CT0		0x00080000	/* Channel 0 chained	*/

#define CPU_DMASR_IR0		0x00040000	/* Chan 0 int DMA pending */
#define CPU_DMASR_IR1		0x00020000	/* Chan 1 int DMA pending */
#define CPU_DMASR_IR2		0x00010000	/* Chan 2 int DMA pending */
#define CPU_DMASR_IR3		0x00008000	/* Chan 3 int DMA pending */

#define CPU_DMASR_ER0		0x00004000	/* Chan 0 ext DMA pending */
#define CPU_DMASR_ER1		0x00002000	/* Chan 1 ext DMA pending */
#define CPU_DMASR_ER2		0x00001000	/* Chan 2 ext DMA pending */
#define CPU_DMASR_ER3		0x00000800	/* Chan 3 ext DMA pending */

#define CPU_DMASR_CB0		0x00000400	/* Channel 0 busy	*/
#define CPU_DMASR_CB1		0x00000200	/* Channel 1 busy	*/
#define CPU_DMASR_CB2		0x00000100	/* Channel 2 busy	*/
#define CPU_DMASR_CB3		0x00000080	/* Channel 3 busy	*/

#define CPU_DMASR_CT1		0x00000040	/* Channel 1 chained	*/
#define CPU_DMASR_CT2		0x00000020	/* Channel 2 chained	*/
#define CPU_DMASR_CT3		0x00000010	/* Channel 3 chained	*/

/*
*	Exception Syndrome Register definitions.
*/

#define CPU_ESR_IMCP		0x80000000	/* Protection violation	*/
#define CPU_ESR_IMCN		0x40000000	/* Non-configured access */
#define CPU_ESR_IMCB		0x20000000	/* Bus error		*/
#define CPU_ESR_IMCT		0x10000000	/* Bus timeout		*/
#define CPU_ESR_PIL			0x08000000	/* Illegal instruction	*/
#define CPU_ESR_PPR			0x04000000	/* Privilege violation	*/
#define CPU_ESR_PTR			0x02000000	/* Program trap		*/
#define CPU_ESR_DST			0x00800000	/* Data store operation	*/
#define CPU_ESR_DIZ			0x00400000	/* Zone fault		*/

/*
*	External Interrupt Enable Register definitions.
*/

#define CPU_EXIER_CIE		0x80000000	/* Critical interrupt	*/
#define CPU_EXIER_SRIE		0x08000000	/* Serial port receive	*/
#define CPU_EXIER_STIE		0x04000000	/* Serial port transmit	*/
#define CPU_EXIER_JRIE		0x02000000	/* JTAG port receive	*/
#define CPU_EXIER_JTIE		0x01000000	/* JTAG port transmit	*/
#define CPU_EXIER_D0IE		0x00800000	/* DMA chan 0 interrupt	*/
#define CPU_EXIER_D1IE		0x00400000	/* DMA chan 1 interrupt	*/
#define CPU_EXIER_D2IE		0x00200000	/* DMA chan 2 interrupt	*/
#define CPU_EXIER_D3IE		0x00100000	/* DMA chan 3 interrupt	*/

#define CPU_EXIER_E0IE		0x00000010	/* External interrupt 0	*/
#define CPU_EXIER_E1IE		0x00000008	/* External interrupt 1	*/
#define CPU_EXIER_E2IE		0x00000004	/* External interrupt 2	*/
#define CPU_EXIER_E3IE		0x00000002	/* External interrupt 3	*/
#define CPU_EXIER_E4IE		0x00000001	/* External interrupt 4	*/

/*
*	External Interrupt Status Register definitions.
*
*	Bits  in  this  register are set to 1 by hardware, and may be
*	cleared by being written to 1 by software.
*/

#define CPU_EXISR_CIS		0x80000000	/* Critical interrupt	*/
#define CPU_EXISR_SRIS		0x08000000	/* Serial port receive	*/
#define CPU_EXISR_STIS		0x04000000	/* Serial port transmit	*/
#define CPU_EXISR_JRIS		0x02000000	/* JTAG port receive	*/
#define CPU_EXISR_JTIS		0x01000000	/* JTAG port transmit	*/
#define CPU_EXISR_D0IS		0x00800000	/* DMA chan 0 interrupt	*/
#define CPU_EXISR_D1IS		0x00400000	/* DMA chan 1 interrupt	*/
#define CPU_EXISR_D2IS		0x00200000	/* DMA chan 2 interrupt	*/
#define CPU_EXISR_D3IS		0x00100000	/* DMA chan 3 interrupt	*/

#define CPU_EXISR_E0IS		0x00000010	/* External interrupt 0	*/
#define CPU_EXISR_E1IS		0x00000008	/* External interrupt 1	*/
#define CPU_EXISR_E2IS		0x00000004	/* External interrupt 2	*/
#define CPU_EXISR_E3IS		0x00000002	/* External interrupt 3	*/
#define CPU_EXISR_E4IS		0x00000001	/* External interrupt 4	*/

/*
*	I/O Configuration Register definitions.
*/

#define CPU_IOCR_E0T		0x80000000	/* Ext int 0 trigger	*/
#define CPU_IOCR_E0L		0x40000000	/* Ext int 0 level	*/
#define CPU_IOCR_E1T		0x20000000	/* Ext int 1 trigger	*/
#define CPU_IOCR_E1L		0x10000000	/* Ext int 1 level	*/
#define CPU_IOCR_E2T		0x08000000	/* Ext int 2 trigger	*/
#define CPU_IOCR_E2L		0x04000000	/* Ext int 2 level	*/
#define CPU_IOCR_E3T		0x02000000	/* Ext int 3 trigger	*/
#define CPU_IOCR_E3L		0x01000000	/* Ext int 3 level	*/
#define CPU_IOCR_E4T		0x00800000	/* Ext int 4 trigger	*/
#define CPU_IOCR_E4L		0x00400000	/* Ext int 4 level	*/
#define CPU_IOCR_EDT		0x00080000	/* Enable DRAM tri-state */
#define CPU_IOCR_SOR		0x00040000	/* Sample on READY	*/
#define CPU_IOCR_EDO		0x00008000	/* EDO DRAM enable	*/
#define CPU_IOCR_2XC		0x00004000	/* Clock-double core	*/
#define CPU_IOCR_ATC		0x00002000	/* Addr bus tri-state	*/
#define CPU_IOCR_SPD		0x00001000	/* Static power disable	*/
#define CPU_IOCR_BEM		0x00000800	/* Byte enable mode	*/
#define CPU_IOCR_PTD		0x00000400	/* Bus timeout disable	*/
#define CPU_IOCR_ARE		0x00000080	/* Asynchronous ready enable */
#define CPU_IOCR_DRC		0x00000020	/* DRAM read on CAS	*/
#define CPU_IOCR_RDM		0x00000018	/* Real-time debug mode	*/
#define CPU_IOCR_TCS		0x00000004	/* Timer clock source	*/
#define CPU_IOCR_SCS		0x00000002	/* Serial port clock source */
#define CPU_IOCR_SPC		0x00000001	/* Serial port configuration */

/*
*	I/O Configuration Register Real-Time Debug Mode definitions.
*/

#define CPU_IOCR_RDM_DISABLED	0x00000000	/* Trace port disabled	*/
#define CPU_IOCR_RDM_BUS		0x00000008	/* Bus status		*/
#define CPU_IOCR_RDM_TRACE		0x00000010	/* Trace output		*/
#define CPU_IOCR_RDM_PARITY		0x00000018	/* Byte parity		*/

/*
*	Machine State Register Definitions.
*/

#define CPU_MSR_WE		0x00040000	/* Wait state enable	*/
#define CPU_MSR_CE		0x00020000	/* Critical interrupt enable */
#define CPU_MSR_ILE		0x00010000	/* Interrupt low-endian	*/
#define CPU_MSR_EE		0x00008000	/* External interrupt enable */
#define CPU_MSR_PR		0x00004000	/* Problem (user) state	*/
#define CPU_MSR_ME		0x00001000	/* Machine check enable	*/
#define CPU_MSR_DE		0x00000200	/* Debug exception enable */
#define CPU_MSR_IR		0x00000020	/* Instruction relocate	*/
#define CPU_MSR_DR		0x00000010	/* Data relocate	*/
#define CPU_MSR_PE		0x00000008	/* Protection enable	*/
#define CPU_MSR_PX		0x00000004	/* Protection exclusive	*/
#define CPU_MSR_LE		0x00000001	/* Low-endian mode	*/

/*
*	Timer Control Register definitions.
*
*	All  bits  in this register are cleared by any form of reset.
*	After  that,  a  bit  that  has  been written to 1 can not be
*	written to 0.
*/

#define CPU_TCR_WP		0xc0000000	/* Watchdog period	*/
#define CPU_TCR_WRC		0x30000000	/* Watchdog reset control */
#define CPU_TCR_WIE		0x08000000	/* Watchdog interrupt enable */
#define CPU_TCR_PIE		0x04000000	/* PIT interrupt enable	*/
#define CPU_TCR_FP		0x03000000	/* FIT period		*/
#define CPU_TCR_FIE		0x00800000	/* FIT interrupt enable	*/
#define CPU_TCR_ARE		0x00400000	/* Auto reload enable	*/

/*
*	Timer Control Register Watchdog Period definitions.
*
*	The  times  noted, assume a 66 MHz core clock (doubled from a
*	33 MHz input).
*/

#define CPU_TCR_WP_17		0x00000000	/* About 2 msec.	*/
#define CPU_TCR_WP_21		0x40000000	/* About 32 msec.	*/
#define CPU_TCR_WP_25		0x80000000	/* About 1/2 second	*/
#define CPU_TCR_WP_29		0xc0000000	/* About 8 seconds	*/

/*
*	Timer Control Register Watchdog Reset definitions.
*/

#define CPU_TCR_WRC_CORE	0x10000000	/* Core reset		*/
#define CPU_TCR_WRC_CHIP	0x20000000	/* Chip reset		*/
#define CPU_TCR_WRC_SYSTEM	0x30000000	/* System reset		*/

/*
*	Timer Control Register FIT Period definitions.
*
*	The  times  noted, assume a 66 MHz core clock (doubled from a
*	33 MHz input).
*/

#define CPU_TCR_FP_9		0x00000000	/* About 8 usec.	*/
#define CPU_TCR_FP_13		0x01000000	/* About 128 usec.	*/
#define CPU_TCR_FP_17		0x02000000	/* About 2 msec.	*/
#define CPU_TCR_FP_21		0x03000000	/* About 32 msec.	*/

/*
*	Timer Status Register definitions.
*
*	Bits  in  this  register are set to 1 by hardware, and may be
*	cleared by being written to 1 by software.
*/

#define CPU_TSR_ENW		0x80000000	/* Enable next watchdog	*/
#define CPU_TSR_WIS		0x40000000	/* Watchdog interrupt status */
#define CPU_TSR_WRS		0x30000000	/* Watchdog reset status */
#define CPU_TSR_PIS		0x08000000	/* PIT interrupt status	*/
#define CPU_TSR_FIS		0x04000000	/* FIT interrupt status	*/

/*
*	Fixed Point Exception Register definitions.
*/

#define CPU_XER_SO		0x80000000	/* Summary overflow	*/
#define CPU_XER_OV		0x40000000	/* Overflow		*/
#define CPU_XER_CA		0x20000000	/* Carry		*/
#define CPU_XER_TBC		0x0000007f	/* Transfer byte count	*/

/*
*	Serial Port Unit Registers (Memory-Mapped).
*/

#define CPU_SPU_REG(n)	(*((volatile unsigned char *)((n)+0x40000000)))

#define CPU_SPU_SPLS	CPU_SPU_REG(0x0) /* Serial port line status	*/
#define CPU_SPU_SPHS	CPU_SPU_REG(0x2) /* Serial port handshake status */
#define CPU_SPU_BRDH	CPU_SPU_REG(0x4) /* Baud rate divisor high	*/
#define CPU_SPU_BRDL	CPU_SPU_REG(0x5) /* Baud rate divisor low	*/
#define CPU_SPU_SPCTL	CPU_SPU_REG(0x6) /* Serial port control		*/
#define CPU_SPU_SPRC	CPU_SPU_REG(0x7) /* Serial port receiver command */
#define CPU_SPU_SPTC	CPU_SPU_REG(0x8) /* Serial port transmitter command */
#define CPU_SPU_SPRB	CPU_SPU_REG(0x9) /* Serial port receive buffer	*/
#define CPU_SPU_SPTB	CPU_SPU_REG(0x9) /* Serial port transmit buffer	*/

/*
*	Serial Port Control Register definitions.
*/

#define CPU_SPCTL_LM		0xc0	/* Loopback modes		*/
#define CPU_SPCTL_DTR		0x20	/* DTR activate			*/
#define CPU_SPCTL_RTS		0x10	/* RTS activate			*/
#define CPU_SPCTL_DB		0x08	/* Number of data bits (7/8)	*/
#define CPU_SPCTL_PE		0x04	/* Parity enable		*/
#define CPU_SPCTL_PTY		0x02	/* Parity (even/odd)		*/
#define CPU_SPCTL_SB		0x01	/* Stop bits (1/2)		*/

/*
*	Serial Port Control Register loopback mode definitions.
*/

#define CPU_SPCTL_LM_NONE		0x00	/* No loopback			*/
#define CPU_SPCTL_LM_INTERNAL	0x40	/* Internal loopback		*/
#define CPU_SPCTL_LM_AUTOECHO	0x80	/* Automatic echo mode		*/

/*
*	Serial Port Control Register number of data bits definitions.
*/

#define CPU_SPCTL_DB_7		0x00	/* 7 data bits                  */
#define CPU_SPCTL_DB_8		0x08	/* 8 data bits                  */

/*
*	Serial Port Control Register parity settings
*/

#define CPU_SPCTL_PTY_EVEN	0x00	/* Even parity                  */
#define CPU_SPCTL_PTY_ODD	0x02	/* Odd parity                   */

/*
*	Serial Port Control Register number of stop bits
*/

#define CPU_SPCTL_SB_1		0x00	/* 1 stop bit                   */
#define CPU_SPCTL_SB_2		0x01	/* 2 stop bits                  */

/*
*	Serial Port Handshake Register definitions.
*
*	Bits  in  this  register are set to 1 by hardware, and may be
*	cleared by being written to 1 by software.
*/

#define CPU_SPHS_DIS		0x80	/* DSR has gone inactive	*/
#define CPU_SPHS_CS			0x40	/* CTS has gone inactive	*/

/*
*	Serial Port Line Status Register definitions.
*
*	The  CPU_SPLS_FE,  CPU_SPLS_OE,  CPU_SPLS_PE, and CPU_SPLS_LB
*	bits are set when the indicated condition occurs, and must be
*	reset by being written to 1 by software.
*/

#define CPU_SPLS_RBR		0x80	/* Receive buffer ready		*/
#define CPU_SPLS_FE			0x40	/* Framing error		*/
#define CPU_SPLS_OE			0x20	/* Overrun error		*/
#define CPU_SPLS_PE			0x10	/* Parity error			*/
#define CPU_SPLS_LB			0x08	/* Line break			*/
#define CPU_SPLS_TBR		0x04	/* Transmit buffer ready	*/
#define CPU_SPLS_TSR		0x02	/* Transmit shift register ready */

/*
*	Serial Port Receiver Command Register definitions.
*/

#define CPU_SPRC_ER			0x80	/* Enable receiver		*/
#define CPU_SPRC_DME		0x60	/* DMA mode or interrupt enable	*/
#define CPU_SPRC_EIE		0x10	/* Error interrupt enable	*/
#define CPU_SPRC_PME		0x08	/* Pause mode enable (flow control) */

/*
*	Serial Port Receiver Command Register DMA mode definitions.
*/

#define CPU_SPRC_DME_NONE	0x00	/* No DMA, interrupt disabled	*/
#define CPU_SPRC_DME_INT	0x20	/* No DMA, interrupt enabled	*/
#define CPU_SPRC_DME_DMA2	0x40	/* Receive using DMA channel 2	*/
#define CPU_SPRC_DME_DMA3	0x60	/* Receive using DMA channel 3	*/

/*
*	Serial Port Transmitter Command Register definitions.
*/

#define CPU_SPTC_ET			0x80	/* Enable transmitter		*/
#define CPU_SPTC_DME		0x60	/* DMA mode or interrupt enable	*/
#define CPU_SPTC_TIE		0x10	/* Underrun interrupt enable	*/
#define CPU_SPTC_EIE		0x08	/* Error interrupt enable	*/
#define CPU_SPTC_SPE		0x04	/* Stop/pause enable		*/
#define CPU_SPTC_TB			0x02	/* Transmit break		*/
#define CPU_SPTC_PGM		0x01	/* Pattern generation mode	*/

/*
*	Serial Port Transmitter Command Register DMA mode definitions.
*/

#define CPU_SPTC_DME_NONE	0x00	/* No DMA, interrupt disabled	*/
#define CPU_SPTC_DME_INT	0x20	/* No DMA, interrupt enabled	*/
#define CPU_SPTC_DME_DMA2	0x40	/* Transmit using DMA channel 2	*/
#define CPU_SPTC_DME_DMA3	0x60	/* Transmit using DMA channel 3	*/

/*******************************************************************************
*
*	End of cpu403.h
*
*******************************************************************************/
#endif	/* CPU403_H */
