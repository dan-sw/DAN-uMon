
/*------------------------------------------------------------------------*
 *  FILE:  MASKS860.H                                                     *
 *          														               	  *
 *  DESCRIPTION:  Bit masks for MPC860 registers. Sequence of registers   *
 *                follows device memory map exactly. 				      	  *
 *																		                    * 
 *  REGISTERS NOT														                 *
 *  INCLUDED:  SWSR, SCCRK, PLPRCRK, RSRK, SDAR, SDMR, IDMR1, IDMR2,      *
 *				   TRR1-4, TCR1-4, TCN1-4, PSMR1, SCCE1, SCCM1, SCCS1,        *
 *             PSMR2, PSMR3, SCCE3, SCCM3, SCCS3, PSMR4, SCCE4,	        *
 *             SCCM4, SCCS4, SIRP				         			           *
 *																		                    *
 *  Note: Look in the Protocol Specific section below first before        *
 *        determining that the above registers are not covered.           *
 *										    							                    * 
 *																		                    *
 *  HISTORY:  9/30/97  ecg  Created								                 *
 *            11/17/97 jay  Modified                                      *
 *            1/9/98   ecg  Modified     								           *
 *            3/11/98  jay  changed smc2/pip to smc2_pip                  *
 *------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*
 *                           GENERAL PURPOSE MASKS			    		        *
 *------------------------------------------------------------------------*/
 
#define  ALL_ONES    0xFFFFFFFF
#define  ALL_ZEROS   0x00000000


/*------------------------------------------------------------------------*
 *                              GENERAL SIU							           *
 *------------------------------------------------------------------------*/
                                                                            

/*----------------------------------------*
 * Module Configuration Register (SIUMCR) *
 *----------------------------------------*/

#define SIUMCR_EARB  0x80000000   /* External Arbitration */
#define SIUMCR_EARP  0x70000000   /* External Arbitration Req. Priority */
#define RESERVED1    0x0F000000   
#define SIUMCR_DSHW  0x00800000   /* Data Show Cycles */
#define SIUMCR_DBGC  0x00600000   /* Debug Pins Configuration */
#define SIUMCR_DBPC  0x00180000   /* Debug Port Pins Configuration */
#define RESERVED2    0x00040000       
#define SIUMCR_FRC   0x00020000   /* FRZ Pin configuration */
#define SIUMCR_DLK   0x00010000   /* Debug Register Lock */
#define SIUMCR_PNCS  0x00008000   /* Parity Enable Nonmemory Controller
                                      Regions */  
#define SIUMCR_OPAR  0x00004000   /* Odd Parity */
#define SIUMCR_DPC   0x00002000   /* Data Parity Pins Configuration */
#define SIUMCR_MPRE  0x00001000   /* Multiprocessors Reservation Enable */
#define SIUMCR_MLRC  0x00000C00   /* Multi-Level Reservation Control */
#define SIUMCR_AEME  0x00000200   /* Asynchronous External Master Enable */
#define SIUMCR_SEME  0x00000100   /* Synchronous External Master Enable */
#define SIUMCR_BSC   0x00000080   /* Byte Select Configuration */
#define SIUMCR_GB5E  0x00000040   /* GPL_B(5) Enable */
#define SIUMCR_B2DD  0x00000020   /* Bank 2 Double Drive */
#define SIUMCR_B3DD  0x00000010   /* Bank 3 Double Drive */
#define RESERVED3    0x0000000F        



/*------------------------------------------------------*
 * System Protection Control Register (SYPCR)           *
 *------------------------------------------------------*/

#define SYPCR_SWTC  0xFFFF0000     /* SoftWare Watchdog Timer Count */
#define SYPCR_BMT   0x0000FF00     /* Bus Monitor Timing */
#define SYPCR_BME   0x00000080     /* Bus Monitor Enable */
#define RESERVED4   0x00000070
#define SYPCR_SWF   0x00000008     /* Software Watchdog Freeze */
#define SYPCR_SWE   0x00000004     /* Software Watchdog Enable */
#define SYPCR_SWRI  0x00000002     /* Software Watchdog Reset/Interrupt 
                                       Select */
#define SYPCR_SWP   0x00000001     /* Software Watchdog Prescale */



/*-------------------------------------------------------------*
 * Software Service Register (SWSR)                            *
 *-------------------------------------------------------------*
 * NOTE: Bit masks are not provided for the Software Service   *
 *       Register (SWSR). The SWSR is the location to which    *
 *       the SWT servicing sequence is written. To prevent SWT *
 *       timeout, the user should write $556C followed by 	   *
 *       $AA93 to this register. The SWSR can be written at    *
 *       any time, but returns all zeros when read.            *
 *-------------------------------------------------------------*/ 



/*-------------------------------------*
 * Interrupt Pending Register (SIPEND) *
 *-------------------------------------*/

#define SIPEND_IRQ0  0x80000000
#define SIPEND_LVL0  0x40000000
#define SIPEND_IRQ1  0x20000000
#define SIPEND_LVL1  0x10000000
#define SIPEND_IRQ2  0x08000000
#define SIPEND_LVL2  0x04000000
#define SIPEND_IRQ3  0x02000000
#define SIPEND_LVL3  0x01000000
#define SIPEND_IRQ4  0x00800000
#define SIPEND_LVL4  0x00400000
#define SIPEND_IRQ5  0x00200000
#define SIPEND_LVL5  0x00100000
#define SIPEND_IRQ6  0x00080000
#define SIPEND_LVL6  0x00040000
#define SIPEND_IRQ7  0x00020000
#define SIPEND_LV7   0x00010000
#define RESERVED5    0x0000FFFF


/*-----------------------------------------*
 *   SIU Interrupt Mask Register (SIMASK)  *
 *-----------------------------------------*/

#define SIMASK_IRM0  0x80000000
#define SIMASK_LVM0  0x40000000
#define SIMASK_IRM1  0x20000000
#define SIMASK_LVM1  0x10000000
#define SIMASK_IRM2  0x08000000
#define SIMASK_LVM2  0x04000000
#define SIMASK_IRM3  0x02000000
#define SIMASK_LVM3  0x01000000
#define SIMASK_IRM4  0x00800000
#define SIMASK_LVM4  0x00400000
#define SIMASK_IRM5  0x00200000
#define SIMASK_LVM5  0x00100000
#define SIMASK_IRM6  0x00080000
#define SIMASK_LVM6  0x00040000
#define SIMASK_IRM7  0x00020000
#define SIMASK_LVM7  0x00010000
#define RESERVED6    0x0000FFFF



/*---------------*
 * SIEL Register *
 *---------------*/

#define SIEL_ED0   0x80000000      
#define SIEL_WM0   0x40000000
#define SIEL_ED1   0x20000000
#define SIEL_WM1   0x10000000
#define SIEL_ED2   0x08000000
#define SIEL_WM2   0x04000000
#define SIEL_ED3   0x02000000
#define SIEL_WM3   0x01000000
#define SIEL_ED4   0x00800000
#define SIEL_WM4   0x00400000
#define SIEL_ED5   0x00200000
#define SIEL_WM5   0x00100000
#define SIEL_ED6   0x00080000
#define SIEL_WM6   0x00040000
#define SIEL_ED7   0x00020000
#define SIEL_WM7   0x00010000
#define RESERVED7  0x0000FFFF



/*---------------------------------------*
 * SIU Interrupt Vector Register (SIVEC) *
 *---------------------------------------*/

#define SIVEC_INTCODE   0xFF000000  /* Interrupt Code */



/*---------------------------------------*
 * Transfer Error Status Register (TESR) *
 *---------------------------------------*/

#define RESERVED8  0xC000
#define TESR_IEXT  0x2000    /* Instruction External Transfer Error 
                                 Acknowledge */
#define TESR_IBM   0x1000    /* Instruction transfer Monitor Timeout */ 
#define TESR_IPB0  0x0800    /* Instruction Parity Error on Byte */
#define TESR_IPB1  0x0400    /* Instruction Parity Error on Byte */
#define TESR_IPB2  0x0200    /* Instruction Parity Error on Byte */
#define TESR_IPB3  0x0100    /* Instruction Parity Error on Byte */
#define RESERVED9  0x00C0   
#define TESR_DEXT  0x0020    /* Data External Transfer Error Acknowledge */
#define TESR_DBM   0x0010    /* Data Transfer Monitor Timeout */
#define TESR_DPB0  0x0008    /* Data Parity Error on Byte */
#define TESR_DPB1  0x0004    /* Data Parity Error on Byte */
#define TESR_DPB2  0x0002    /* Data Parity Error on Byte */
#define TESR_DPB3  0x0001    /* Data Parity Error on Byte */ 



/*------------------------------------*
 * SDMA Configuration Register (SDCR) *
 *------------------------------------*/

#define RESERVED10   0xFFFF8000    
#define SDCR_FRZ     0x00006000   /* Freeze */
#define RESERVED11   0x00001FF0
#define RESERVED12   0x000000C0
#define SDCR_RAID    0x00000003   /* RISC Controller Arbitration ID */




/*-------------------------------------------------------------------------*
 *                                  PCMCIA								   *
 *-------------------------------------------------------------------------*/
                                                                            

/*------------------------------------*
 * Interface Option Register 0 (POR0) *
 *------------------------------------*/

#define POR0_BSIZE  0xF8000000    /* PCMCIA Bank Size */
#define RESERVED13  0x07F00000	  
#define POR0_PSHT   0x000F0000    /* PCMCIA Strobe Hold Time */
#define POR0_PSST   0x0000F000    /* PCMCIA Strobe Set Up Time */
#define POR0_PSL    0x00000F80    /* PCMCIA Strobe Length */
#define POR0_PPS    0x00000040    /* PCMCIA Port Size */
#define POR0_PRS    0x00000038    /* PCMCIA Region Select */
#define POR0_PSLOT  0x00000004    /* PCMCIA Slot Identifier */
#define RESERVED14  0x00000002    /* Any write to this window will result
                                      in a bus error */
#define POR0_PV     0x00000001    /* PCMCIA Valid Bit */



/*------------------------------------*
 * Interface Option Register 1 (POR1) *
 *------------------------------------*/

#define POR1_BSIZE  0xF8000000    /* PCMCIA Bank Size */
#define RESERVED15  0x07F00000	  
#define POR1_PSHT   0x000F0000    /* PCMCIA Strobe Hold Time */
#define POR1_PSST   0x0000F000    /* PCMCIA Strobe Set Up Time */
#define POR1_PSL    0x00000F80    /* PCMCIA Strobe Length */
#define POR1_PPS    0x00000040    /* PCMCIA Port Size */
#define POR1_PRS    0x00000038    /* PCMCIA Region Select */
#define POR1_PSLOT  0x00000004    /* PCMCIA Slot Identifier */
#define RESERVED16  0x00000002    /* Any write to this window will result
                                      in a bus error */
#define POR1_PV     0x00000001    /* PCMCIA Valid Bit */



/*------------------------------------*
 * Interface Option Register 2 (POR2) *
 *------------------------------------*/

#define POR2_BSIZE  0xF8000000    /* PCMCIA Bank Size */
#define RESERVED17  0x07F00000	  
#define POR2_PSHT   0x000F0000    /* PCMCIA Strobe Hold Time */
#define POR2_PSST   0x0000F000    /* PCMCIA Strobe Set Up Time */
#define POR2_PSL    0x00000F80    /* PCMCIA Strobe Length */
#define POR2_PPS    0x00000040    /* PCMCIA Port Size */
#define POR2_PRS    0x00000038    /* PCMCIA Region Select */
#define POR2_PSLOT  0x00000004    /* PCMCIA Slot Identifier */
#define RESERVED18  0x00000002    /* Any write to this window will result
                                      in a bus error */
#define POR2_PV     0x00000001    /* PCMCIA Valid Bit */



/*------------------------------------*
 * Interface Option Register 3 (POR3) *
 *------------------------------------*/

#define POR3_BSIZE  0xF8000000    /* PCMCIA Bank Size */
#define RESERVED19  0x07F00000	  
#define POR3_PSHT   0x000F0000    /* PCMCIA Strobe Hold Time */
#define POR3_PSST   0x0000F000    /* PCMCIA Strobe Set Up Time */
#define POR3_PSL    0x00000F80    /* PCMCIA Strobe Length */
#define POR3_PPS    0x00000040    /* PCMCIA Port Size */
#define POR3_PRS    0x00000038    /* PCMCIA Region Select */
#define POR3_PSLOT  0x00000004    /* PCMCIA Slot Identifier */
#define RESERVED20  0x00000002    /* Any write to this window will result
                                      in a bus error */
#define POR3_PV     0x00000001    /* PCMCIA Valid Bit */



/*------------------------------------*
 * Interface Option Register 4 (POR4) *
 *------------------------------------*/

#define POR4_BSIZE  0xF8000000    /* PCMCIA Bank Size */
#define RESERVED21  0x07F00000	  
#define POR4_PSHT   0x000F0000    /* PCMCIA Strobe Hold Time */
#define POR4_PSST   0x0000F000    /* PCMCIA Strobe Set Up Time */
#define POR4_PSL    0x00000F80    /* PCMCIA Strobe Length */
#define POR4_PPS    0x00000040    /* PCMCIA Port Size */
#define POR4_PRS    0x00000038    /* PCMCIA Region Select */
#define POR4_PSLOT  0x00000004    /* PCMCIA Slot Identifier */
#define RESERVED22  0x00000002    /* Any write to this window will result
                                      in a bus error */
#define POR4_PV     0x00000001    /* PCMCIA Valid Bit */



/*------------------------------------*
 * Interface Option Register 5 (POR5) *
 *------------------------------------*/

#define POR5_BSIZE  0xF8000000    /* PCMCIA Bank Size */
#define RESERVED23  0x07F00000	  
#define POR5_PSHT   0x000F0000    /* PCMCIA Strobe Hold Time */
#define POR5_PSST   0x0000F000    /* PCMCIA Strobe Set Up Time */
#define POR5_PSL    0x00000F80    /* PCMCIA Strobe Length */
#define POR5_PPS    0x00000040    /* PCMCIA Port Size */
#define POR5_PRS    0x00000038    /* PCMCIA Region Select */
#define POR5_PSLOT  0x00000004    /* PCMCIA Slot Identifier */
#define RESERVED24  0x00000002    /* Any write to this window will result
                                      in a bus error */
#define POR5_PV     0x00000001    /* PCMCIA Valid Bit */



/*------------------------------------*
 * Interface Option Register 6 (POR6) *
 *------------------------------------*/

#define POR6_BSIZE  0xF8000000    /* PCMCIA Bank Size */
#define RESERVED25  0x07F00000	  
#define POR6_PSHT   0x000F0000    /* PCMCIA Strobe Hold Time */
#define POR6_PSST   0x0000F000    /* PCMCIA Strobe Set Up Time */
#define POR6_PSL    0x00000F80    /* PCMCIA Strobe Length */
#define POR6_PPS    0x00000040    /* PCMCIA Port Size */
#define POR6_PRS    0x00000038    /* PCMCIA Region Select */
#define POR6_PSLOT  0x00000004    /* PCMCIA Slot Identifier */
#define RESERVED26  0x00000002    /* Any write to this window will result
                                      in a bus error */
#define POR6_PV     0x00000001    /* PCMCIA Valid Bit */



/*------------------------------------*
 * Interface Option Register 7 (POR7) *
 *------------------------------------*/

#define POR7_BSIZE  0xF8000000    /* PCMCIA Bank Size */
#define RESERVED27  0x07F00000	  
#define POR7_PSHT   0x000F0000    /* PCMCIA Strobe Hold Time */
#define POR7_PSST   0x0000F000    /* PCMCIA Strobe Set Up Time */
#define POR7_PSL    0x00000F80    /* PCMCIA Strobe Length */
#define POR7_PPS    0x00000040    /* PCMCIA Port Size */
#define POR7_PRS    0x00000038    /* PCMCIA Region Select */
#define POR7_PSLOT  0x00000004    /* PCMCIA Slot Identifier */
#define RESERVED28  0x00000002    /* Any write to this window will result
                                      in a bus error */
#define POR7_PV     0x00000001    /* PCMCIA Valid Bit */



/*----------------------------------------------*
 * Interface General Control Register A (PGCRA) *
 *----------------------------------------------*/

#define PGCRA_CAIREQLVL  0xFF000000   /* Define Interrupt Level for 
                                          IREQ for Card A */
#define PGCRA_CASCHLVL   0x00FF0000   /* Define Interupt Level for 
                                          STSCHG for Card A */
#define PGCRA_CADREQ     0x0000C000   /* Define pin to be used as 
                                          internal DMA request */
#define RESERVED29       0x00003F00   
#define PGCRA_CAOE       0x00000080   /* Card A Output Enable */
#define PGCRA_CARESET    0x00000040   /* Card A Reset */

#define RESERVED30       0x0000003F  



/*----------------------------------------------*
 * Interface General Control Register B (PGCRB) *
 *----------------------------------------------*/

#define PGCRB_CBIRQLVL   0xFF000000   /* Define Interrupt Level for 
                                          IREQ for Card B */
#define PGCRB_CBSCHLVL   0x00FF0000   /* Define Interupt Level for 
                                          STSCHG for Card B */
#define PGCRB_CBDREQ     0x0000C000   /* Define pin to be used as 
                                          internal DMA request */
#define RESERVED31       0x00003F00   
#define PGCRB_CBOE       0x00000080   /* Card B Output Enable */
#define PGCRB_CBRESET    0x00000040   /* Card B Reset */

#define RESERVED32       0x0000003F  



/*------------------------------------------*
 * Interface Status Changed Register (PSCR) *
 *------------------------------------------*/

#define PSCR_CAVS1_C     0x80000000   /* Volt. Sense 1 for Card A changed */
#define PSCR_CAVS2_C     0x40000000   /* Volt. Sense 2 for Card A changed */
#define PSCR_CAWP_C      0x20000000   /* Write Protect for Card A changed */
#define PSCR_CACD2_C     0x10000000   /* Card Detect 2 for Card A changed */
#define PSCR_CACD1_C     0x08000000   /* Card Detect 1 for Card A changed */
#define PSCR_CABVD2_C    0x04000000   /* Batt Volt/SPKR in Card A changed */
#define PSCR_CABVD1_C    0x02000000   /* Batt Volt/STSCHG Card A changed */
#define RESERVED33       0x01000000
#define PSCR_CARDY_L     0x00800000   /* RDY/IRQ of Card A Pin is Low */
#define PSCR_CARDY_H     0x00400000   /* RDY/IRQ of CARD A Pin is High */
#define PSCR_CARDY_R     0x00200000   /* RDY/IRQ of Card A Pin Rising Edge 
                                          detected */
#define PSCR_CARDY_F     0x00100000   /* RDY/IRQ of Card A Pin Falling Edge 
                                          detected */
#define RESERVED34       0x000F0000
#define PSCR_CBVS1_C     0x00008000   /* Volt Sense 1 for Card B changed */
#define PSCR_CBVS2_C     0x00004000   /* Volt Sense 2 for Card B changed */
#define PSCR_CBWP_C      0x00002000   /* Write Protect for Card B changed */
#define PSCR_CBCD2_C     0x00001000   /* Card detect 2 for Card B changed */
#define PSCR_CBCD1_C     0x00000800   /* Card detect 1 for Card B changed */
#define PSCR_CBBVD2_C    0x00000400   /* Batt Volt/SPKR in for Card B 
                                          change */
#define PSCR_CBBVD1_C    0x00000200   /* Batt Volt/STSCHG in for Card B 
                                          change */
#define RESERVED35       0x00000100   
#define PSCR_CBRDY_L     0x00000080   /* RDY/IRQ of Card B Pin is Low */
#define PSCR_CBRDY_H     0x00000040   /* RDY/IRQ of Card B Pin is High */
#define PSCR_CBRDY_R     0x00000020   /* RDY/IRQ of Card B Pin Rising 
                                          Edge detect */
#define PSCR_CBRDY_F     0x00000010   /* RDY/IRQ of Card B Pin Falling
                                          Edge detect */
#define RESERVED36       0x0000000F  



/*--------------------------------------*
 * Interface Input Pins Register (PIPR) *
 *--------------------------------------*/

#define PIPR_CAVS1	 0x80000000   /* Volt Sense 1 for Card A */
#define PIPR_CAVS2   0x40000000   /* Volt Sense 2 for Card A */
#define PIPR_CAWP    0x20000000   /* Write Protect for Card A */
#define PIPR_CACD2   0x10000000   /* Card Detect 2 for Card A */
#define PIPR_CACD1   0x08000000   /* Card Detect 1 for Card A */
#define PIPR_CABVD2  0x04000000   /* Batt Volt/SPKR in for Card A */
#define PIPR_CABVD1  0x02000000   /* Batt Volt/STSCHG in for Card A */
#define PIPR_CARDY   0x01000000   /* RDY/IRQ of Card A Pin */
#define RESERVED37   0x00FF0000
#define PIPR_CBVS1   0x00008000   /* Voltage Sense 1 for Card B */
#define PIPR_CBVS2   0x00004000   /* Voltage Sense 2 for Card B */
#define PIPR_CBWP    0x00002000   /* Write Protect for Card B */
#define PIPR_CBCD2   0x00001000   /* Card Detect 2 for Card B */
#define PIPR_CBCD1   0x00000800   /* Card Detect 1 for Card B */
#define PIPR_CBBVD2  0x00000400   /* Batt Volt/SPKR in for Card B */
#define PIPR_CBBVD1  0x00000200   /* Batt Volt/STSCHG in for Card B */
#define PIPR_CBRDY   0x00000100   /* RDY/IRQ of Card B Pin */
#define RESERVED38   0x000000FF   
                              


/*---------------------------------*
 * Interface Enable Register (PER) *
 *---------------------------------*/

#define PER_CA_EVS1   0x80000000   /* Enable Volt Sense 1 Card A changed */
#define PER_CA_EVS2   0x40000000   /* Enable Volt Sense 2 Card A changed */
#define PER_CA_EWP    0x20000000   /* Enable Write Prot. Card A Changed */
#define PER_CA_ECD2   0x10000000	/* Enable Card Detect 2 Card A Changed */
#define PER_CA_ECD1   0x08000000   /* Enable Card Detect 2 Card A Changed */
#define PER_CA_EBVD2  0x04000000   /* Enable Batt Volt/SPKR in for Card A
                                       changed */
#define PER_CA_EBVD1  0x02000000   /* Enable for Batt Volt/STSCHG in for 
                                       Card A changed */
#define RESERVED39    0x01000000
#define PER_CA_ERDY_L 0x00800000   /* Enable for RDY/IRQ Card A Pin Low */
#define PER_CA_ERDY_H 0x00400000   /* Enable for RDY/IRQ Card A Pin High */
#define PER_CA_ERDY_R 0x00200000   /* Enable for RDY/IRQ Card A Pin R.E. 
                                       Detected */ 
#define PER_CA_ERDY_F 0x00100000   /* Enable for RDY/IRQ Card A Pin F.E. 
                                       Detected */ 
#define RESERVED40    0x000F0000
#define PER_CB_EVS1   0x00008000   /* Enable Volt Sense 1 Card B Changed */
#define PER_CB_EVS2   0x00004000   /* Enable Volt Sense 2 Card B Changed */
#define PER_CB_EWP    0x00002000   /* Enable for Write Protect for Card B 
                                       Changed */
#define PER_CB_ECD2   0x00001000   /* Enable for Card Detect 2 for Card B 
                                       Changed */
#define PER_CB_ECD1   0x00000800   /* Enable for Card Detect 1 for Card B 
                                       Changed */
#define PER_CB_EBVD2  0x00000400   /* Enable Batt Volt/SPKR in for Card B
                                       Changed */
#define PER_CB_EBVD1  0x00000200   /* Enable for Batt Volt/STSCHG in for 
                                       Card B Changed */
#define RESERVED41    0x00000100   
#define CB_ERDY_L     0x00000080   /* Enable for RDY/IRQ of Card B Pin is 
                                       Low */
#define CB_ERDY_H     0x00000040   /* Enable for RDY/IRQ of Card B Pin is 
                                       High */
#define CB_ERDY_R     0x00000020   /* Enable for RDY/IRQ Card B Pin R.E.
                                       Detected */
#define CB_ERDY_F     0x00000010   /* Enable for RDY/IRQ Card B Pin F.E.
                                       Detected */
#define RESERVED42    0x0000000F  



/*-------------------------------------------------------------------------*
 *                                   MEMC								   *
 *-------------------------------------------------------------------------*/


/*----------------------------*
 * Base Register Bank 0 (BR0) *
 *----------------------------*/

#define BR0_BA      0xFFFF8000   /* Base Address */
#define BR0_AT      0x00007000   /* Address Type */
#define BR0_PS      0x00000C00   /* Port Size */
#define BR0_PARE    0x00000200   /* Parity Enable */
#define BR0_WP      0x00000100   /* Write protect */
#define BR0_MS      0x000000C0   /* Machine Select */
#define RESERVED43  0x0000003E  
#define BR0_V       0x00000001   /* Valid Bit */ 



/*------------------------------*
 * Option Register Bank 0 (OR0) *
 *------------------------------*/

#define OR0_AM             0xFFFF8000   /* Address Mask */
#define OR0_ATM            0x00007000   /* Address Type Mask */
#define OR0_CSNT_SAM       0x00000800   /* Chip Select Negation Time
                                            Start Address Multiplex */
#define OR0_ACS_G5LA_G5LS  0x00000600   /* Address to Chip-Select Setup 
                                            General-Purpose Line 5A
                                            General-Purpose Line 5 Start */
#define OR0_BI             0x00000100   /* Burst Inhibit */
#define OR0_SCY            0x000000F0   /* cycle Length in Clocks */
#define OR0_SETA           0x00000008   /* External Transfer Acknowledge */
#define OR0_TRLX           0x00000004   /* Timing Relaxed */
#define OR0_EHTR           0x00000002   /* Extended Hold Time Rd. Access */
#define RESERVED44         0x00000001



/*----------------------------*
 * Base Register Bank 1 (BR1) *
 *----------------------------*/

#define BR1_BA      0xFFFF8000   /* Base Address */
#define BR1_AT      0x00007000   /* Address Type */
#define BR1_PS      0x00000C00   /* Port Size */
#define BR1_PARE    0x00000200   /* Parity Enable */
#define BR1_WP      0x00000100   /* Write protect */
#define BR1_MS      0x000000C0   /* Machine Select */
#define RESERVED45  0x0000003E  
#define BR1_V       0x00000001   /* Valid Bit */ 
  


/*------------------------------*
 * Option Register Bank 1 (OR1) *
 *------------------------------*/

#define OR1_AM             0xFFFF8000   /* Address Mask */
#define OR1_ATM            0x00007000   /* Address Type Mask */
#define OR1_CSNT_SAM       0x00000800   /* Chip Select Negation Time
                                           Start Address Multiplex */
#define OR1_ACS_G5LA_G5LS  0x00000600   /* Address to Chip-Select Setup 
                                           General-Purpose Line 5A
                                           General-Purpose Line 5 Start */
#define OR1_BI             0x00000100   /* Burst Inhibit */
#define OR1_SCY            0x000000F0   /* cycle Length in Clocks */
#define OR1_SETA           0x00000008   /* External Transfer Acknowledge */
#define OR1_TRLX           0x00000004   /* Timing Relaxed */
#define OR1_EHTR           0x00000002   /* Extended Hold Time on Read
                                           Access */
#define RESERVED46         0x00000001



/*----------------------------*
 * Base Register Bank 2 (BR2) *
 *----------------------------*/

#define BR2_BA      0xFFFF8000   /* Base Address */
#define BR2_AT      0x00007000   /* Address Type */
#define BR2_PS      0x00000C00   /* Port Size */
#define BR2_PARE    0x00000200   /* Parity Enable */
#define BR2_WP      0x00000100   /* Write protect */
#define BR2_MS      0x000000C0   /* Machine Select */
#define RESERVED47  0x0000003E  
#define BR2_V       0x00000001   /* Valid Bit */ 
  


/*------------------------------*
 * Option Register Bank 2 (OR2) *
 *------------------------------*/

#define OR2_AM             0xFFFF8000   /* Address Mask */
#define OR2_ATM            0x00007000   /* Address Type Mask */
#define OR2_CSNT_SAM       0x00000800   /* Chip Select Negation Time
                                           Start Address Multiplex */
#define OR2_ACS_G5LA_G5LS  0x00000600   /* Address to Chip-Select Setup 
                                           General-Purpose Line 5A
                                           General-Purpose Line 5 Start */
#define OR2_BI             0x00000100   /* Burst Inhibit */
#define OR2_SCY            0x000000F0   /* cycle Length in Clocks */
#define OR2_SETA           0x00000008   /* External Transfer Acknowledge */
#define OR2_TRLX           0x00000004   /* Timing Relaxed */
#define OR2_EHTR           0x00000002   /* Extended Hold Time Rd. Access */
#define RESERVED48         0x00000001



/*----------------------------*
 * Base Register Bank 3 (BR3) *
 *----------------------------*/

#define BR3_BA      0xFFFF8000   /* Base Address */
#define BR3_AT      0x00007000   /* Address Type */
#define BR3_PS      0x00000C00   /* Port Size */
#define BR3_PARE    0x00000200   /* Parity Enable */
#define BR3_WP      0x00000100   /* Write protect */
#define BR3_MS      0x000000C0   /* Machine Select */
#define RESERVED49  0x0000003E  
#define BR3_V       0x00000001   /* Valid Bit */ 
  


/*------------------------------*
 * Option Register Bank 3 (OR3) *
 *------------------------------*/

#define OR3_AM             0xFFFF8000   /* Address Mask */
#define OR3_ATM            0x00007000   /* Address Type Mask */
#define OR3_CSNT_SAM       0x00000800   /* Chip Select Negation Time
                                           Start Address Multiplex */
#define OR3_ACS_G5LA_G5LS  0x00000600   /* Address to Chip-Select Setup 
                                           General-Purpose Line 5A
                                           General-Purpose Line 5 Start */
#define OR3_BI             0x00000100   /* Burst Inhibit */
#define OR3_SCY            0x000000F0   /* cycle Length in Clocks */
#define OR3_SETA           0x00000008   /* External Transfer Acknowledge */
#define OR3_TRLX           0x00000004   /* Timing Relaxed */
#define OR3_EHTR           0x00000002   /* Extended Hold Time Rd. Access */
#define RESERVED50         0x00000001



/*----------------------------*
 * Base Register Bank 4 (BR4) *
 *----------------------------*/

#define BR4_BA      0xFFFF8000   /* Base Address */
#define BR4_AT      0x00007000   /* Address Type */
#define BR4_PS      0x00000C00   /* Port Size */
#define BR4_PARE    0x00000200   /* Parity Enable */
#define BR4_WP      0x00000100   /* Write protect */
#define BR4_MS      0x000000C0   /* Machine Select */
#define RESERVED51  0x0000003E  
#define BR4_V       0x00000001   /* Valid Bit */ 
  


/*------------------------------*
 * Option Register Bank 4 (OR4) *
 *------------------------------*/

#define OR4_AM             0xFFFF8000   /* Address Mask */
#define OR4_ATM            0x00007000   /* Address Type Mask */
#define OR4_CSNT_SAM       0x00000800   /* Chip Select Negation Time
                                           Start Address Multiplex */
#define OR4_ACS_G5LA_G5LS  0x00000600   /* Address to Chip-Select Setup 
                                           General-Purpose Line 5A
                                           General-Purpose Line 5 Start */
#define OR4_BI             0x00000100   /* Burst Inhibit */
#define OR4_SCY            0x000000F0   /* cycle Length in Clocks */
#define OR4_SETA           0x00000008   /* External Transfer Acknowledge */
#define OR4_TRLX           0x00000004   /* Timing Relaxed */
#define OR4_EHTR           0x00000002   /* Extended Hold Time Rd. Access */
#define RESERVED52         0x00000001



/*----------------------------*
 * Base Register Bank 5 (BR5) *
 *----------------------------*/

#define BR5_BA      0xFFFF8000   /* Base Address */
#define BR5_AT      0x00007000   /* Address Type */
#define BR5_PS      0x00000C00   /* Port Size */
#define BR5_PARE    0x00000200   /* Parity Enable */
#define BR5_WP      0x00000100   /* Write protect */
#define BR5_MS      0x000000C0   /* Machine Select */
#define RESERVED53  0x0000003E  
#define BR5_V       0x00000001   /* Valid Bit */ 
  


/*------------------------------*
 * Option Register Bank 5 (OR5) *
 *------------------------------*/

#define OR5_AM             0xFFFF8000   /* Address Mask */
#define OR5_ATM            0x00007000   /* Address Type Mask */
#define OR5_CSNT_SAM       0x00000800   /* Chip Select Negation Time
                                           Start Address Multiplex */
#define OR5_ACS_G5LA_G5LS  0x00000600   /* Address to Chip-Select Setup 
                                           General-Purpose Line 5A
                                           General-Purpose Line 5 Start */
#define OR5_BI             0x00000100   /* Burst Inhibit */
#define OR5_SCY            0x000000F0   /* cycle Length in Clocks */
#define OR5_SETA           0x00000008   /* External Transfer Acknowledge */
#define OR5_TRLX           0x00000004   /* Timing Relaxed */
#define OR5_EHTR           0x00000002   /* Extended Hold Time Rd. Access */
#define RESERVED54         0x00000001



/*----------------------------*
 * Base Register Bank 6 (BR6) *
 *----------------------------*/

#define BR6_BA      0xFFFF8000   /* Base Address */
#define BR6_AT      0x00007000   /* Address Type */
#define BR6_PS      0x00000C00   /* Port Size */
#define BR6_PARE    0x00000200   /* Parity Enable */
#define BR6_WP      0x00000100   /* Write protect */
#define BR6_MS      0x000000C0   /* Machine Select */
#define RESERVED55  0x0000003E  
#define BR6_V       0x00000001   /* Valid Bit */ 
  


/*------------------------------*
 * Option Register Bank 6 (OR6) *
 *------------------------------*/

#define OR6_AM             0xFFFF8000   /* Address Mask */
#define OR6_ATM            0x00007000   /* Address Type Mask */
#define OR6_CSNT_SAM       0x00000800   /* Chip Select Negation Time
                                           Start Address Multiplex */
#define OR6_ACS_G5LA_G5LS  0x00000600   /* Address to Chip-Select Setup 
                                           General-Purpose Line 5A
                                           General-Purpose Line 5 Start */
#define OR6_BI             0x00000100   /* Burst Inhibit */
#define OR6_SCY            0x000000F0   /* cycle Length in Clocks */
#define OR6_SETA           0x00000008   /* External Transfer Acknowledge */
#define OR6_TRLX           0x00000004   /* Timing Relaxed */
#define OR6_EHTR           0x00000002   /* Extended Hold Time Rd. Access */
#define RESERVED56         0x00000001



/*----------------------------*
 * Base Register Bank 7 (BR7) *
 *----------------------------*/

#define BR7_BA      0xFFFF8000   /* Base Address */
#define BR7_AT      0x00007000   /* Address Type */
#define BR7_PS      0x00000C00   /* Port Size */
#define BR7_PARE    0x00000200   /* Parity Enable */
#define BR7_WP      0x00000100   /* Write protect */
#define BR7_MS      0x000000C0   /* Machine Select */
#define RESERVED57  0x0000003E  
#define BR7_V       0x00000001   /* Valid Bit */ 
  


/*------------------------------*
 * Option Register Bank 7 (OR7) *
 *------------------------------*/

#define OR7_AM             0xFFFF8000   /* Address Mask */
#define OR7_ATM            0x00007000   /* Address Type Mask */
#define OR7_CSNT_SAM       0x00000800   /* Chip Select Negation Time
                                           Start Address Multiplex */
#define OR7_ACS_G5LA_G5LS  0x00000600   /* Address to Chip-Select Setup 
                                           General-Purpose Line 5A
                                           General-Purpose Line 5 Start */
#define OR7_BI             0x00000100   /* Burst Inhibit */
#define OR7_SCY            0x000000F0   /* cycle Length in Clocks */
#define OR7_SETA           0x00000008   /* External Transfer Acknowledge */
#define OR7_TRLX           0x00000004   /* Timing Relaxed */
#define OR7_EHTR           0x00000002   /* Extended Hold Time Rd. Access */
#define RESERVED58         0x00000001



/*-------------------------------*
 * Memory Command Register (MCR) *
 *-------------------------------*/

#define MCR_OP        0xC0000000   /* Command Opcode */
#define RESERVED59    0x3F000000   
#define MCR_UM        0x00800000   /* User Machine */
#define RESERVED60    0x007F0000   
#define MCR_MB        0x0000E000   /* Memory Bank */
#define RESERVED61    0x00001000   
#define MCR_MCLF      0x00000F00   /* Memory Command Loop Field */
#define RESERVED62    0x000000C0   
#define MCR_MAD       0x0000003F   /* Machine Address */



/*--------------------------------*
 * Machine A Mode Register (MAMR) *
 *--------------------------------*/

#define MAMR_PTA         0xFF000000   /* Periodic Timer A Period */
#define MAMR_PTAE        0x00800000   /* Periodic Timer A Enable */
#define MAMR_AMA         0x00700000   /* Address Multiplex Size A */
#define RESERVED63       0x00080000   
#define MAMR_DSA         0x00060000   /* Disable Timer Period */
#define RESERVED64       0x00010000   
#define MAMR_G0CLA       0x0000E000   /* General Line 0 Control A */
#define MAMR_GPLA_A4DIS	 0x00001000   /* GPL_A4 Output Disable Line */
#define MAMR_RLFA        0x00000F00   /* Read Loop Field A */
#define MAMR_WLFA        0x000000F0   /* Write Loop Field A */
#define MAMR_TLFA        0x0000000F   /* Timer Loop Field A */ 



/*--------------------------------*
 * Machine B Mode Register (MBMR) *
 *--------------------------------*/

#define MBMR_PTB        0xFF000000   /* Periodic Timer B Period */
#define MBMR_PTBE       0x00800000   /* Periodic Timer B Enable */
#define MBMR_AMB        0x00700000   /* Address Multiplex Size B */
#define RESERVED65      0x00080000
#define MBMR_DSB        0x00060000   /* Disable Timer Period */
#define RESERVED66      0x00010000  
#define MBMR_G0CLB      0x0000E000   /* General Line 0 Control B */
#define MBMR_GPL_B4DIS	0x00001000   /* GPL_B4 Output Line Disable */
#define MBMR_RLFB       0x00000F00   /* Read Loop Field B */
#define MBMR_WLFB       0x000000F0   /* Write Loop Field B */
#define MBMR_TLFB       0x0000000F   /* Timer Loop Field B */



/*--------------------------------*
 * Memory Status Register (MSTAT) *
 *--------------------------------*/

#define MSTAT_PER0   0x8000   /* Parity Error Bank 0 */
#define MSTAT_PER1   0x4000   /* Parity Error Bank 1 */
#define MSTAT_PER2   0x2000   /* Parity Error Bank 2 */
#define MSTAT_PER3   0x1000   /* Parity Error Bank 3 */
#define MSTAT_PER4   0x0800   /* Parity Error Bank 4 */
#define MSTAT_PER5   0x0400   /* Parity Error Bank 5 */
#define MSTAT_PER6   0x0200   /* Parity Error Bank 6 */
#define MSTAT_PER7   0x0100   /* Parity Error Bank 7 */
#define MSTAT_WPER   0x0080   /* Write Protection Error */
#define RESERVED67   0x007F   




/*--------------------------------------------------*
 * Memory Periodic Timer Prescaler Register (MPTPR) *
 *--------------------------------------------------*/

#define MPTPR_PTP    0xFF00   /* Periodic Timers Prescaler */
#define RESERVED68   0x00FF	  



/*-------------------------------------------------------------------------*
 *                        SYSTEM INTEGRATION TIMERS 					   *
 *-------------------------------------------------------------------------*/


/*-----------------------------------------------------*
 * Timers Timebase Status and Control Register (TBSCR) *
 *-----------------------------------------------------*/

#define TBSCR_TBIRQ   0xFF00   /* Timebase Interrupt Request */
#define TBSCR_REFA    0x0080   /* Reference Interrupt Status */
#define TBSCR_REFB    0x0040   /* Reference Interrupt Status */
#define RESERVED69    0x0030  
#define TBSCR_REFAE   0x0008   /* Refernce Interrupt Enable */
#define TBSCR_REFBE   0x0004   /* Refernce Interrupt Enable */
#define TBSCR_TBF     0x0002   /* Timebase Freeze */
#define TBSCR_TBE     0x0001   /* Timebase Enable */



/*--------------------------------------------------------------------*
 * Timebase Reference Register 0 (TBREFF0)                            *
 *--------------------------------------------------------------------*
 * NOTE: There are two timebase reference registers (TBREFF0/TBREFF1) *
 *       associated with the lower part of the timebase. Each 		  *
 *		   register is 32-bits R/W. When there is a match between the *
 *		   contents of timebase and the reference register, a 		  *
 *		   maskable interrupt is generated. 					      *
 *--------------------------------------------------------------------*/

#define TBREFF0_TBREF   0xFFFFFFFF



/*-----------------------------------------*
 * Timebase Reference Register 1 (TBREFF1) *
 *-----------------------------------------*/

#define TBREFF1_TBREF   0xFFFFFFFF



/*-----------------------------------------------------*
 * Real-Time Clock Status and Control Register (RTCSC) *
 *-----------------------------------------------------*/

#define RTCSC_RTCIRQ    0xFF00   /* RTC Interrupt Request */
#define RTCSC_SEC       0x0080   /* Once Per Second Interrupt */
#define RTCSC_ALR       0x0040   /* Alarm Interrupt */
#define RESERVED70      0x0020  
#define RTCSC_38K       0x0010   /* Real_time Clock Source Select */
#define RTCSC_SIE       0x0008   /* Second interrupt Enable */
#define RTCSC_ALE       0x0004   /* Alarm Interrupt Enable */
#define RTCSC_RTF       0x0002   /* Real-Time Clock Freeze */
#define RTCSC_RTE       0x0001   /* Real-Time Clock Enable */



/*----------------------------------------------------------*
 * Real-Time Clock Register (RTC)                           *
 *----------------------------------------------------------*
 * NOTE: The real-time clock (RTC) register is a 32-bit R/W	*
 *       register. It contains the current value of the     *
 *		   real-time clock. 								*
 *----------------------------------------------------------*/

#define RTC_RTC   0xFFFFFFFF   



/*-----------------------------------------------------------*
 * Real-Time Clock Alarm Register (RTCAL)                    *
 *-----------------------------------------------------------*
 * NOTE: The real-time clock alarm (RTCAL) is a 32-bit R/W 	 *
 *       register. When the value of the RTC is equal to the *
 *		 value programmed in the alarm register, a maskable  *
 *		 interrupt is generated 							 *
 *-----------------------------------------------------------*/

#define RTCAL_ALARM  0xFFFFFFFF   



/*--------------------------------------------------------*
 * Periodic Interrupt Status and Control Register (PISCR) *
 *--------------------------------------------------------*/

#define PISCR_PIRQ   0xFF00   /* Periodic Interrupt Request Level */
#define PISCR_PS     0xFF80   /* Periodic Interrupt Status */
#define RESERVED71   0x0078  
#define PISCR_PIE    0x0004   /* Periodic Interrupt Enable */
#define PISCR_PITF   0x0002   /* Periodic Interrupt Timer Freeze */
#define PISCR_PTE    0x0001   /* Periodic Timer Enable */



/*---------------------------------------------------------------*
 * Periodic Interrupt Timer Count (PITC)                         *
 *---------------------------------------------------------------*
 * NOTE: Contains the 16 bits to be loaded in a modulus counter. *
 *       The register is always R/W.          					 *
 *---------------------------------------------------------------*/

#define PITC_PITC   0xFFFF0000
#define RESERVED72  0x0000FFFF



/*----------------------------------------------------------------*
 * Periodic Interrupt Timer Register (PITR)                       *
 *----------------------------------------------------------------*
 * NOTE: A read-only register that shows the current value in the *
 *       periodic interrupt down counter. Reads or writes to 	  *
 *		 this register have no effect on the register or counter  *
 *----------------------------------------------------------------*/

#define PITR_PIT    0xFFFF0000
#define RESERVED73  0x0000FFFF




/*-------------------------------------------------------------------------*
 *                             CLOCKS AND RESET  						   *
 *-------------------------------------------------------------------------*/



/*--------------------------------------*
 * System Clock Control register (SCCR) *
 *--------------------------------------*/

#define RESERVED74   0x80000000
#define SCCR_COM     0x60000000	/* Clock Output Mode */
#define RESERVED75   0x1C000000
#define SCCR_TBS     0x02000000    /* Timebase Source */
#define SCCR_RTDIV   0x01000000    /* RTC Clock Divide */
#define SCCR_RTSEL   0x00800000    /* RTC circuit input source select */
#define SCCR_CRQEN   0x00400000    /* CPM requect enable */
#define SCCR_PRQEN   0x00200000    /* Power management request enable */
#define RESERVED76   0x00180000  
#define SCCR_EBDF    0x00060000    /* CLKOUT frequency */
#define RESERVED77   0x00018000    
#define SCCR_DFSYNC  0x00006000    /* Division factor of SyncCLK */
#define SCCR_DFBRG   0x00001800    /* Division factor of BRGCLK */
#define SCCR_DFNL    0x00000700    /* Division factor low frequency */
#define SCCR_DFNH    0x000000D0    /* Division factor high frequency */
#define RESERVED78   0x0000001F 



/*-----------------------------------------------------*
 * PLL, Low Power, and Reset Control Register (PLPRCR) *
 *-----------------------------------------------------*/  

#define PLPRCR_MF      0xFFF00000   /* Multiplication factor bits */
#define PLPRCR_MF6     0x00500000   /* Multiplication factor of 6 (5+1) */
#define PLPRCR_MF8     0x00700000   /* Multiplication factor of 8 (7+1) */
#define PLPRCR_MF10    0x00900000   /* Multiplication factor of 10 (9+1) */
#define RESERVED79     0x000F0000
#define PLPRCR_SPLSS   0x00008000   /* SPLL lock status sticky bits */
#define PLPRCR_TEXPS   0x00004000   /* TEXP status bit */
#define RESERVED80     0x00002000   
#define PLPRCR_TMIST   0x00001000   /* Timers interrupt status */
#define RESERVED81     0x00000800   
#define PLPRCR_CSRC    0x00000400   /* Clock source bit */
#define PLPRCR_LPM     0x00000300   /* Low power mode select bits */
#define PLPRCR_CSR     0x00000080   /* Checkstop reset enable */
#define PLPRCR_LOLRE   0x00000040   /* Loss of lock reset enable */
#define PLPRCR_FIOPD   0x00000020   /* Force I/O pull-down */
#define RESERVED82     0x0000001F



/*-----------------------------*
 * Reset status register (RSR) *
 *-----------------------------*/

#define RSR_EHRS    0x80	  /* External hard reset status */
#define RSR_ESRS    0x40	  /* External soft reset status */
#define RSR_LLRS    0x20     /* Loss of lock status */
#define RSR_SWRS    0x10     /* Software watchdog reset status */
#define RSR_CSRS    0x08     /* Check stop reset status */
#define RSR_DBHRS   0x04     /* Debug port hard reset status */
#define RSR_DBSRS   0x02     /* Debug port soft reset status */
#define RSR_JTRS    0x01     /* JTAG reset status */




/*-------------------------------------------------------------------------*
 *                     SYSTEM INTEGRATION TIMERS KEYS					   *
 *-------------------------------------------------------------------------*
 * NOTE: Bit masks are not provided for System Integration Timers Keys or  *
 *       Clocks and Reset Keys. A write of 0x55CCAA33 to a key's memory    *
 *       address will change it to the open state. A write of any other    *
 *       data to a key's location will change it to the locked state. At   *
 *       power-on reset, all keys (except for real-time clock related 	   *
 *       registers) are	in the open state.                      		   *
 *-------------------------------------------------------------------------*/															  



/*-------------------------------------------------------------------------*
 *                                  I2C 								   *
 *-------------------------------------------------------------------------*/


/*---------------------------*
 * I2C Mode Register (I2MOD) *
 *---------------------------*/

#define RESERVED83   0xC0
#define I2MOD_REVD   0x20   /* Reverse Data */
#define I2MOD_GCD    0x10   /* General Call Disable */
#define I2MOD_FLT    0x08   /* Clock filter */
#define I2MOD_PDIV   0x06   /* Pre Divider */
#define I2MOD_EN     0x01   /* Enable I2C */



/*------------------------------*
 * I2C Address Register (I2ADD) *
 *------------------------------*/

#define I2ADD_SAD   0xFE    /* Slave Address */
#define RESERVED84  0x01



/*--------------------------*
 * I2C BRG Register (I2BRG) *
 *--------------------------*/

#define I2BRG_DIV   0xFF    /* Division Ratio */



/*------------------------------*
 * I2C Command Register (I2COM) *
 *------------------------------*/

#define I2COM_STR   0x80    /* Start Transmit */
#define RESERVED85  0x7E  
#define I2COM_M_S   0x01    /* Master Slave */		  



/*----------------------------*
 * I2C Event Register (I2CER) *
 *----------------------------*/

#define RESERVED86   0xE0   
#define I2CER_TXE    0x10   /* Tx Error */
#define RESERVED87   0x80
#define I2CER_BSY    0x04   /* Busy Condition */
#define I2CER_TXB    0x02   /* Tx Buffer */
#define I2CER_RXB    0x01   /* Rx Buffer */



/*------------------------------------------------------------------*
 * I2C Mask Register (I2CMR)                                        *
 *------------------------------------------------------------------*
 * NOTE: The I2C mask register is an 8-bit read/write register that * 
 *       has the same bit formats as the I2CER. If a bit in the     *
 *		 I2CMR is 1, the corresponding interupt in the I2CER is     *
 *		 enabled. If the bit is zero, the corresponding interrupt   *
 *		 in the I2CER is marked. This register is cleared at reset. *
 *              												    *
 *------------------------------------------------------------------*/

#define RESERVED88   0xE0   
#define I2CMR_TXE    0x10   /* Tx Error */
#define RESERVED89   0x80
#define I2CMR_BSY    0x04   /* Busy Condition */
#define I2CMR_TXB    0x02   /* Tx Buffer */
#define I2CMR_RXB    0x01   /* Rx Buffer */





/*-------------------------------------------------------------------------*
 *                                    DMA 								   *
 *-------------------------------------------------------------------------*/


/*--------------------------------------------------------------------*
 * SDMA Address Register (SDAR) 								   	  *
 *--------------------------------------------------------------------*
 * NOTE: Bit masks are not provived for SDMA Address Register (SDAR). *
 *       The 32-bit read-only SDMA Address Register shows the system  *
 *       address that is accessed during an SDMA bus error. It is	  *
 *       undefined at reset.	                                 	  *
 *--------------------------------------------------------------------*/


 /*-----------------------------*
  * SDMA Status Register (SDSR) *    
  *-----------------------------*/
 
 #define SDSR_SBER    0x80   /* SDMA Channel Bus Error */
 #define SDSR_RINT    0x40   /* Reserved Interrupt */
 #define RESERVED90   0x3C   
 #define SDSR_DSP2    0x02   /* DSP Chain 2 Interrupt */
 #define SDSR_DSP1    0x01   /* DSP Chain 1 Interrupt */
       


/*---------------------------------------------------------------------*
 * SDMA Mask Register (SDMR) 								   	       *
 *---------------------------------------------------------------------*
 * NOTE: Bit masks are not provided for the SDMA Mask Register (SDMR). *
 *       The SDMA Mask Register is an 8-bit read/write register with   *
 *       the same bit format as the SDMA status register. If a bit in  *
 *		 the SDMA mask register is a 1, the corresponding interrupt in *
 *		 the event register is enabled. If the bit is zero, the		   *
 *		 corresponding interrupt in the event register is masked. This *
 *		   register is cleared at reset.                           	   *       
 *---------------------------------------------------------------------*/

            

/*-------------------------------*
 * IDMA1 Status Register (IDSR1) *    
 *-------------------------------*/

#define RESERVED91   0xF8
#define IDSR1_OB     0x04   /* Out of buffers */
#define IDSR1_DONE   0x02   /* IDMA transfer done */		
#define IDSR1_AD     0x01   /* Auxiliary done */



/*---------------------------------------------------------------------*
 * IDMA1 Mask Register (IDMR1) 								   	       *
 *---------------------------------------------------------------------*
 * NOTE: Bit masks are not provided for the IDMA1 Mask Register. The   *
 *       IDMA mask register is an 8-bit read/write register with       *
 *       the same bit format as the IDSR. If a bit in the IDMR is a 1, *
 *       the corresponding interrupt in the status register is 		   *
 *       enabled. If the bit is zero, the corresponding interrupt in   *
 *       the status  register is masked. This register is cleared at   *
 *       reset.                                                        *       
 *---------------------------------------------------------------------*/



/*-------------------------------*
 * IDMA2 Status Register (IDSR2) *    
 *-------------------------------*/

#define RESERVED92   0xF8
#define IDSR2_OB     0x04   /* Out of buffers */
#define IDSR2_DONE   0x02   /* IDMA transfer done */		
#define IDSR3_AD     0x01   /* Auxiliary done */



/*----------------------------------------------------------------------*
 * IDMA2 Mask Register (IDMR2) 								   	        *
 *----------------------------------------------------------------------*
 * NOTE: Bit masks are not provided for the IDMA2 Mask Register. The 	*
 *       IDMA mask register is an 8-bit read/write register with        *
 *       the same bit format as the IDSR. If a bit in                   *
 *		   the IDMR is a 1, the corresponding interrupt in              *
 *		   the status register is enabled. If the bit is zero, the	    *
 *		   corresponding interrupt in the status  register is masked. 	*
 *       This register is cleared at reset.                             *       
 *----------------------------------------------------------------------*/




/*-------------------------------------------------------------------------*
 *                          CPM INTERRUPT CONTROL						   *
 *-------------------------------------------------------------------------*/


/*-------------------------------------*
 * CP Interrupt Vector Register (CIVR) *    
 *-------------------------------------*/

#define CIVR_VN      0xF800   /* Vector number */
#define RESERVED93	0x07FE
#define CIVR_IACK    0x0001   /* Interrupt acknowledge */



/*--------------------------------------------------------------*
 * CP Interrupt Configuration Register (CICR)                   *    
 *--------------------------------------------------------------*
 * NOTE: The 24-bit R/W CICR defines the request level for the  *
 *       CPM interrupts, the priority between the SCCs, and the *
 *	     highest priority interrupt. The CICR, which can be     *
 *		 dynamically changed the the user, is cleared at reset. *
 *--------------------------------------------------------------*/

#define RESERVED94   0xFF000000
#define CICR_SDdP    0x00C00000   /* SCCd priority order */
#define CICR_SCcP    0x00300000   /* SCCc priority order */
#define CICR_SCbP    0x000C0000   /* SCCb priority order */
#define CICR_SCaP    0x00030000   /* SCCa priority order */
#define CICR_IRL0    0x00008000   /* Interrupt request level */
#define CICR_IRL1    0x00004000   /* Interrupt request level */
#define CICR_IRL2    0x00002000   /* Interrupt request level */
#define CICR_HP0     0x00001000   /* Highest priority */
#define CICR_HP1     0x00000800   /* Highest priority */
#define CICR_HP2     0x00000400   /* Highest priority */
#define CICR_HP3     0x00000200   /* Highest priority */
#define CICR_HP4     0x00000100   /* Highest priority */
#define RESERVED95   0x0000007E 
#define CICR_SPS     0x00000001   /* Spread priority scheme */



/*-----------------------------------------------------------------*
 * CP Interrupt Pending Register (CIPR)                            *    
 *-----------------------------------------------------------------*
 * NOTE: Each bit in the 32-bit R/W CPM Interrupt Pending Register *
 *       (CIPR) corresponds to a CPM interrupt source. When a CPM  *
 *       interrupt is received, the CPIC sets the corresponding    *
 *       bit in the CIPR. This register is cleared at reset.	   * 
 *       See MPC860 User's Manual.			                	   *
 *-----------------------------------------------------------------*/

#define CIPR_PC15      0x80000000   
#define CIPR_SCC1      0x40000000
#define CIPR_SCC2      0x20000000
#define CIPR_SCC3      0x10000000
#define CIPR_SCC4      0x08000000
#define CIPR_PC14      0x04000000
#define CIPR_TIMER1    0x02000000
#define CIPR_PC13      0x01000000
#define CIPR_PC12      0x00800000
#define CIPR_SDMA      0x00400000
#define CIPR_IDMA1     0x00200000
#define CIPR_IDMA2     0x00100000
#define RESERVED96     0x00080000
#define CIPR_TIMER2    0x00040000
#define CIPR_R_TT      0x00020000
#define CIPR_I2C       0x00010000
#define CIPR_PC11      0x00008000
#define CIPR_PC10      0x00004000
#define RESERVED97     0x00002000
#define CIPR_TIMER3    0x00001000
#define CIPR_PC9       0x00000800
#define CIPR_PC8       0x00000400
#define CIPR_PC7       0x00000200
#define RESERVED98     0x00000100
#define CIPR_TIMER4    0x00000080
#define CIPR_PC6       0x00000040
#define CIPR_SPI       0x00000020
#define CIPR_SMC1      0x00000010
#define CIPR_SMC2_PIP  0x00000008
#define CIPR_PC5       0x00000004
#define CIPR_PC4       0x00000002
#define RESERVED99     0x00000001



/*------------------------------------------------------------------*
 * CP Interrupt Mask Register (CIMR)                                *    
 *------------------------------------------------------------------*
 * NOTE: Each bit in the CIMR coresponds to a CPM interrupt source. *
 *       The user masks an interrupt by clearing the coresponding   *
 *		 bit in the CIMR and enables an interrupt by setting the	*
 *		 corresponding bit in the CIMR. When a masked CPM interrupt *
 *		 occurs, the corresponding bit in the CIPR is still set, 	*
 *		 regardless of the CIMR bit, but no interupt request is     *
 *		 passed to the CPU core. This register is cleared at reset. *
 *       See MPC860 User's Manual.		                         	*
 *------------------------------------------------------------------*/

#define CIMR_PC15      0x80000000   
#define CIMR_SCC1      0x40000000
#define CIMR_SCC2      0x20000000
#define CIMR_SCC3      0x10000000
#define CIMR_SCC4      0x08000000
#define CIMR_PC14      0x04000000
#define CIMR_TIMER1    0x02000000
#define CIMR_PC13      0x01000000
#define CIMR_PC12      0x00800000
#define CIMR_SDMA      0x00400000
#define CIMR_IDMA1     0x00200000
#define CIMR_IDMA2     0x00100000
#define RESERVED100    0x00080000
#define CIMR_TIMER2    0x00040000
#define CIMR_R_TT      0x00020000
#define CIMR_I2C       0x00010000
#define CIMR_PC11      0x00008000
#define CIMR_PC10      0x00004000
#define RESERVED101    0x00002000
#define CIMR_TIMER3    0x00001000
#define CIMR_PC9       0x00000800
#define CIMR_PC8       0x00000400
#define CIMR_PC7       0x00000200
#define RESERVED102    0x00000100
#define CIMR_TIMER4    0x00000080
#define CIMR_PC6       0x00000040
#define CIMR_SPI       0x00000020
#define CIMR_SMC1      0x00000010
#define CIMR_SMC2_PIP  0x00000008
#define CIMR_PC5       0x00000004
#define CIMR_PC4       0x00000002
#define RESERVED103    0x00000001




/*----------------------------------------------*
 * CP Interrupt In-Service Register (CISR)      *    
 *----------------------------------------------*
 * NOTE: This register is cleared at reset. See	*
 *       MPC860 User's Manual.     				*
 *----------------------------------------------*/
 
#define CISR_PC15      0x80000000   
#define CISR_SCC1      0x40000000
#define CISR_SCC2      0x20000000
#define CISR_SCC3      0x10000000
#define CISR_SCC4      0x08000000
#define CISR_PC14      0x04000000
#define CISR_TIMER1    0x02000000
#define CISR_PC13      0x01000000
#define CISR_PC12      0x00800000
#define CISR_SDMA      0x00400000
#define CISR_IDMA1     0x00200000
#define CISR_IDMA2     0x00100000
#define RESERVED104    0x00080000
#define CISR_TIMER2    0x00040000
#define CISR_R_TT      0x00020000
#define CISR_I2C       0x00010000
#define CISR_PC11      0x00008000
#define CISR_PC10      0x00004000
#define RESERVED105    0x00002000
#define CISR_TIMER3    0x00001000
#define CISR_PC9       0x00000800
#define CISR_PC8       0x00000400
#define CISR_PC7       0x00000200
#define RESERVED106    0x00000100
#define CISR_TIMER4    0x00000080
#define CISR_PC6       0x00000040
#define CISR_SPI       0x00000020
#define CISR_SMC1      0x00000010
#define CISR_SMC2_PIP  0x00000008
#define CISR_PC5       0x00000004
#define CISR_PC4       0x00000002
#define RESERVED107    0x00000001



/*-------------------------------------------------------------------------*
 *                            INPUT/OUTPUT PORT						       *
 *-------------------------------------------------------------------------*/



/*-------------------------------------------------------*
 * Port A Data Direction Register (PADIR)                *    
 *-------------------------------------------------------*
 * NOTE: For each DRx bit, the definition is as follows: *
 *														 *
 *        0 = The corrsponding bit is an input.		     *
 *                                                  	 *
 *        1 = The corresponding bit is an output.		 *
 *														 *
 *  This register is cleared at system reset.			 *
 *-------------------------------------------------------*/

#define PADIR_DR0   0x8000
#define PADIR_DR1   0x4000
#define PADIR_DR2   0x2000
#define PADIR_DR3   0x1000
#define PADIR_DR4   0x0800
#define PADIR_DR5   0x0400
#define PADIR_DR6   0x0200
#define PADIR_DR7   0x0100
#define PADIR_DR8   0x0080
#define PADIR_DR9   0x0040
#define PADIR_DR10  0x0020
#define PADIR_DR11  0x0010
#define PADIR_DR12  0x0008
#define PADIR_DR13  0x0004
#define PADIR_DR14  0x0002
#define PADIR_DD15  0x0001



/*----------------------------------------------------------------*
 * Port A Pin Assignment Register (PAPAR)                         *    
 *----------------------------------------------------------------*
 * NOTE: For each DDx bit, the definition is as follows:          *
 *													              *
 *	    0 = General-purpose I/O. The peripheral functions		  *
 *		    of the pin are not used.							  *       
 *                                                                *
 *      1 = Dedicated peripheral function. The pin is used	      *
 *		    by the internal module. The on-chip peripheral 	      *
 *		    function to which it is dedicated can be determined   *
 *		    by other bits such as those in the PADIR.			  *
 * 																  *
 *  This register is cleared at system reset.					  * 
 *----------------------------------------------------------------*/

#define PAPAR_DD0   0x8000
#define PAPAR_DD1   0x4000
#define PAPAR_DD2   0x2000
#define PAPAR_DD3   0x1000
#define PAPAR_DD4   0x0800
#define PAPAR_DD5   0x0400
#define PAPAR_DD6   0x0200
#define PAPAR_DD7   0x0100
#define PAPAR_DD8   0x0080
#define PAPAR_DD9   0x0040
#define PAPAR_DD10  0x0020
#define PAPAR_DD11  0x0010
#define PAPAR_DD12  0x0008
#define PAPAR_DD13  0x0004
#define PAPAR_DD14  0x0002
#define PAPAR_DD15  0x0001



/*---------------------------------------------------------------*
 * Port A Open Drain Register (PAODR)                            *    
 *---------------------------------------------------------------*
 * NOTE: For each ODx bit, the definition is as follows:    	 *
 *															     *
 *        0 = The I/O pin is actively driven as an output.	     *
 *															     *
 *        1 = The I/O pin is an open-drain driver. As an output, *
 *		      the pin is actively driven low, otherwise it is    *
 *		  	  three-stated.										 *
 *																 *
 *  This register is cleared at system reset.					 *
 *---------------------------------------------------------------*/

#define PAODR_OD9    0x0040
#define PAODR_OD10   0x0020
#define PAODR_OD11   0x0010
#define PAODR_OD12   0x0008
#define PAODR_OD14   0x0002  



/*--------------------------------------------*
 * Port A Data Register (PADAT)               *    
 *--------------------------------------------*
 * NOTE: This register is undefined at reset. *
 *       See MPC860 User's Manual.			  *
 *--------------------------------------------*/

#define PADAT_D0   0x8000
#define PADAT_D1   0x4000
#define PADAT_D2   0x2000
#define PADAT_D3   0x1000
#define PADAT_D4   0x0800
#define PADAT_D5   0x0400
#define PADAT_D6   0x0200
#define PADAT_D7   0x0100
#define PADAT_D8   0x0080
#define PADAT_D9   0x0040
#define PADAT_D10  0x0020
#define PADAT_D11  0x0010
#define PADAT_D12  0x0008
#define PADAT_D13  0x0004
#define PADAT_D14  0x0002
#define PADAT_D15  0x0001



/*--------------------------------------------------------*
 * Port C Data Direction Register (PCDIR)                 *    
 *--------------------------------------------------------*
 * NOTE: When read, the Port C data (PCDAT) register      *
 *       always reflects the current status of each line. *
 *--------------------------------------------------------*/

#define RESERVED108   0xF000
#define PCDIR_DR4     0x0800
#define PCDIR_DR5     0x0400
#define PCDIR_DR6     0x0200
#define PCDIR_DR7     0x0100
#define PCDIR_DR8     0x0080
#define PCDIR_DR9     0x0040
#define PCDIR_DR10    0x0020
#define PCDIR_DR11    0x0010
#define PCDIR_DR12    0x0008
#define PCDIR_DR13    0x0004
#define PCDIR_DR14    0x0002
#define PCDIR_DR15    0x0001



/*------------------------------------------*
 * Port C Pin Assignment Register (PCPAR)   *    
 *------------------------------------------*
 * NOTE: This register is cleared at system *
 *       reset. 							*
 *------------------------------------------*/

#define RESERVED109   0xF000
#define PCPAR_DD4     0x0800
#define PCPAR_DD5     0x0400
#define PCPAR_DD6     0x0200
#define PCPAR_DD7     0x0100
#define PCPAR_DD8     0x0080
#define PCPAR_DD9     0x0040
#define PCPAR_DD10    0x0020
#define PCPAR_DD11    0x0010
#define PCPAR_DD12    0x0008
#define PCPAR_DD13    0x0004
#define PCPAR_DD14    0x0002
#define PCPAR_DD15    0x0001



/*--------------------------------------------------*
 * Port C Special Options Register (PCSO)           *    
 *--------------------------------------------------*
 * NOTE: PCSO is a 16-bit R/W register. Each bit    *
 *       defined in the PCSO coresponds to a 		*
 *       Port C line. The PCSO is cleared at reset. *
 *--------------------------------------------------*/ 

#define RESERVED110   0xF000
#define PCSO_CD4      0x0800
#define PCSO_CTS4     0x0400
#define PCSO_CD3      0x0200
#define PCSO_CTS3     0x0100
#define PCSO_CD2      0x0080
#define PCSO_CTS2     0x0040
#define PCSO_CD1      0x0020
#define PCSO_CTS1     0x0010
#define RESERVED111   0x000C
#define PCSO_DREQ1    0x0002
#define PCSO_DREQ2    0x0001



/*----------------------------------------*
 * Port C Data Register (PCDAT)           *    
 *----------------------------------------*
 * NOTE: When read, PCDAT always reflects *
 *       the current status of each line. *
 *----------------------------------------*/

#define RESERVED112  0xF000
#define PCPAT_D4     0x0800
#define PCDAT_D5     0x0400
#define PCDAT_D6     0x0200
#define PCDAT_D7     0x0100
#define PCDAT_D8     0x0080
#define PCDAT_D9     0x0040
#define PCDAT_D10    0x0020
#define PCDAT_D11    0x0010
#define PCDAT_D12    0x0008
#define PCDAT_D13    0x0004
#define PCDAT_D14    0x0002
#define PCDAT_D15    0x0001



/*-------------------------------------------------------------*
 * Port C Interrupt Control Register (PCINT)                   *    
 *-------------------------------------------------------------*
 * NOTE: The corresponding port C line (PCx) asserts an		   *
 *       interrupt request according to the following:		   *
 *															   *
 *       0 = Any change on PCx generates an interrupt request. *
 *															   *
 *       1 = High-to-low change on PCx generates an interrupt  *
 *            request. 										   *
 *															   *
 *  This register is cleared at reset.						   *
 *-------------------------------------------------------------*/

#define RESERVED113      0xF000
#define PCPAT_EDM4       0x0800
#define PCEDMAT_EDM5     0x0400
#define PCEDMAT_EDM6     0x0200
#define PCEDMAT_EDM7     0x0100
#define PCEDMAT_EDM8     0x0080
#define PCEDMAT_EDM9     0x0040
#define PCEDMAT_EDM10    0x0020
#define PCEDMAT_EDM11    0x0010
#define PCEDMAT_EDM12    0x0008
#define PCEDMAT_EDM13    0x0004
#define PCEDMAT_EDM14    0x0002
#define PCEDMAT_EDM15    0x0001



/*-------------------------------------------------*
 * Port D Data Direction Register (PDDIR)          *    
 *-------------------------------------------------*
 * NOTE: This register is cleared at system reset. *
 *-------------------------------------------------*/

#define PDDIR_OD8     0x8000
#define PDDIR_OD10    0x4000
#define RESERVED114   0x2000
#define PDDIR_DR3     0x1000
#define PDDIR_DR4     0x0800
#define PDDIR_DR5     0x0400
#define PDDIR_DR6     0x0200
#define PDDIR_DR7     0x0100
#define PDDIR_DR8     0x0080
#define PDDIR_DR9     0x0040
#define PDDIR_DR10    0x0020
#define PDDIR_DR11    0x0010
#define PDDIR_DR12    0x0008
#define PDDIR_DR13    0x0004
#define PDDIR_DR14    0x0002
#define PDDIR_DR15    0x0001



/*--------------------------------------------------------*
 * Port D Pin Assignment Register (PDPAR)                 *    
 *--------------------------------------------------------*
 * NOTE: For each DDx bit, the definition is as follows:  *
 *														  *
 *		0 = General-purpose I/O. The peripheral functions *
 *	        of the pin are not used. 					  *
 *														  *
 *      1 = Dedicated peripheral function. The pin is	  *
 *		    used by the internal module. 				  *
 *		                                                  *
 *  This register is cleared at system reset.             *
 *--------------------------------------------------------*/

#define RESERVED115   0xE000
#define PDPAR_DD3     0x1000
#define PDPAR_DD4     0x0800
#define PDPAR_DD5     0x0400
#define PDPAR_DD6     0x0200
#define PDPAR_DD7     0x0100
#define PDPAR_DD8     0x0080
#define PDPAR_DD9     0x0040
#define PDPAR_DD10    0x0020
#define PDPAR_DD11    0x0010
#define PDPAR_DD12    0x0008
#define PDPAR_DD13    0x0004
#define PDPAR_DD14    0x0002
#define PDPAR_DD15    0x0001



/*--------------------------------------------*
 * Port D Data Register (PDDAT)               *    
 *--------------------------------------------*
 * NOTE: This register is undefined at reset. *
 *       See MPC860 User's Manual.		      *
 *--------------------------------------------*/

#define RESERVED116  0xE000
#define PDDAT_D3     0x1000
#define PDDAT_D4     0x0800
#define PDDAT_D5     0x0400
#define PDDAT_D6     0x0200
#define PDDAT_D7     0x0100
#define PDDAT_D8     0x0080
#define PDDAT_D9     0x0040
#define PDDAT_D10    0x0020
#define PDDAT_D11    0x0010
#define PDDAT_D12    0x0008
#define PDDAT_D13    0x0004
#define PDDAT_D14    0x0002
#define PDDAT_D15    0x0001



/*-------------------------------------------------------------------------*
 *                               CPM TIMERS					               *
 *-------------------------------------------------------------------------*/


/*--------------------------------------------*
 * Timer Global Configuration Register (TGCR) *
 *--------------------------------------------*
 * NOTE: This register is cleared by reset    *
 *--------------------------------------------*/

#define TGCR_CAS4   0x8000	  /* Cascade Timers */
#define TGCR_FRZ4   0x4000	  /* Freeze */
#define TGCR_STP4   0x2000	  /* Stop Timer */
#define TGCR_RST4   0x1000	  /* Reset Timer */
#define TGCR_GM2    0x0800	  /* Gate Mode for Pin */
#define TGCR_FRZ3   0x0400	  /* Freeze */
#define TGCR_STP3   0x0200	  /* Stop Timer */
#define TGCR_RST3   0x0100	  /* Reset Timer */
#define TGCR_CAS2   0x0080
#define TGCR_FRZ2   0x0040
#define TGCR_STP2   0x0020
#define TGCR_RST2   0x0010
#define TGCR_GM1    0x0008	  /* Gate Mode for Pin 1 */
#define TGCR_FRZ1   0x0004
#define TGCR_STP1   0x0002
#define TGCR_RST1   0x0001



/*-----------------------------------------------*
 * Timer 1 Mode Register (TMR1)                  *
 *-----------------------------------------------*
 * NOTE: TMR1 through TMR4 are identical 16-bit, *
 *       memory-mapped R/W registers. These 	 *
 *		 registers are cleared by reset.   		 *
 *-----------------------------------------------*/

#define TMR1_PS    0xFF00
#define TMR1_CE    0x00C0
#define TMR1_OM    0x0020
#define TMR1_ORI   0x0010
#define TMR1_FRR   0x0008
#define TMR1_ICLK  0x0006
#define TMR1_GE    0x0001



/*---------------------------------*
 * Timer 2 Mode Register (TMR2)	   *
 *---------------------------------*/

#define TMR2_PS    0xFF00
#define TMR2_CE    0x00C0
#define TMR2_OM    0x0020
#define TMR2_ORI   0x0010
#define TMR2_FRR   0x0008
#define TMR2_ICLK  0x0006
#define TMR2_GE    0x0001



/*---------------------------------*
 * Timer 3 Mode Register (TMR3)	   *
 *---------------------------------*/

#define TMR3_PS    0xFF00
#define TMR3_CE    0x00C0
#define TMR3_OM    0x0020
#define TMR3_ORI   0x0010
#define TMR3_FRR   0x0008
#define TMR3_ICLK  0x0006
#define TMR3_GE    0x0001



/*---------------------------------*
 * Timer 4 Mode Register (TMR4)	   *
 *---------------------------------*/

#define TMR4_PS    0xFF00
#define TMR4_CE    0x00C0
#define TMR4_OM    0x0020
#define TMR4_ORI   0x0010
#define TMR4_FRR   0x0008
#define TMR4_ICLK  0x0006
#define TMR4_GE    0x0001



/*---------------------------------------*
 * NOTE: Bit masks are not provided for: *
 *                                       *
 *       Timer Reference Registers 1-4   *
 *       Timer Capture Registers 1-4     *
 *		   Timer Counters 1-4			 *
 *---------------------------------------*/


 
/*----------------------------------------------*
 * Timer 1 Event Register (TER1)                *
 *----------------------------------------------*
 * NOTE: TER1 through TER4 are identical 16-bit	*
 *       memory mapped registers. These 		*
 *		   registers are cleared at reset.		*
 *----------------------------------------------*/

 #define RESERVED117   0xFFFC
 #define TER1_REF      0x0002	 /* Output Reference Event */
 #define TER1_CAP      0x0001	 /* Capture Event */
 
 

/*------------------------------------------*
 * Timer 2 Event Register (TER2)            *
 *------------------------------------------*/

 #define RESERVED118   0xFFFC
 #define TER2_REF      0x0002	 /* Output Reference Event */
 #define TER2_CAP      0x0001	 /* Capture Event */



 /*------------------------------------------*
  * Timer 3 Event Register (TER3)            *
  *------------------------------------------*/
 
 #define RESERVED119   0xFFFC
 #define TER3_REF      0x0002	 /* Output Reference Event */
 #define TER3_CAP      0x0001	 /* Capture Event */



 /*------------------------------------------*
  * Timer 4 Event Register (TER4)            *
  *------------------------------------------*/
 
 #define RESERVED120   0xFFFC
 #define TER4_REF      0x0002	 /* Output Reference Event */
 #define TER4_CAP      0x0001	 /* Capture Event */
  
 
 
 
/*-------------------------------------------------------------------------*
 *                        COMMUNICATION PROCESSOR					       *
 *-------------------------------------------------------------------------*/



/*------------------------------------------*
 * CPM Command Register (CPCR)              *
 *------------------------------------------*
 * NOTE: This register is cleared by reset. *
 *       See MPC860 User's Manual. 		    *
 *------------------------------------------*/

#define CPCR_RST                 0x8000	/* Software Reset Command */
#define RESERVED121	            0x7000

#define CPCR_INIT_TX_RX_PARAMS   0x0000   /* Opcode 0 */
#define CPCR_INIT_RX_PARAMS      0x0100   /* Opcode 1 */
#define CPCR_INIT_TX_PARAMS      0x0200   /* Opcode 2 */
#define CPCR_ENTER_HUNT_MODE     0x0300   /* Opcode 3 */
#define CPCR_STOP_TX             0x0400   /* Opcode 4 */
#define CPCR_GRACEFUL_STOP_TX    0x0500   /* Opcode 5 */
#define CPCR_RESTART_TX          0x0600   /* Opcode 6 */
#define CPCR_CLOSE_RX_BD         0x0700   /* Opcode 7 */
#define CPCR_SET_GRP_ADDR        0x0800   /* Opcode 8 */
#define CPCR_RESET_BCS           0x0A00   /* Opcode 10 */

#define CPCR_SCC1_CH             0x0000   /* CPCR ch Num Field:  SCC1 */
#define CPCR_I2C_IDMA1_CH        0x0010   /* CPCR ch Num Field:  I2C/IDMA1 */
#define CPCR_SCC2_CH             0x0040   /* CPCR Ch Num Field:  SCC2 */
#define CPCR_SPI_IDMA2_TIMER_CH  0x0050   /* CPCR Ch Num Field:  SPI/IDMA/
                                                                 RISC Timers */

#define CPCR_SCC3_CH             0x0080   /* CPCR Ch Num Field:  SCC3 */
#define CPCR_SMC1_DSP1_CH        0x0090   /* CPCR Ch Num Field:  SMC1/DSP1 */
#define CPCR_SCC4_CH             0x00C0   /* CPCR Ch Num Field:  SCC4 */
#define CPCR_SMC2_DSP2_CH        0x00D0   /* CPCR Ch Num Field:  SMC2/DSP2 */

#define RESERVED122              0x000E
#define CPCR_FLG                 0x0001	/* Command Semaphore Flag */






/*------------------------------------------------------*
 * RISC Controller Configuration Register (RCCR)        *
 *------------------------------------------------------*
 * NOTE: This register is initialized to zero at reset. *
 *------------------------------------------------------*/

#define RCCR_TIME     0x8000   /* Timer Enable */
#define RESERVED123   0x4000   
#define RCCR_TIMEP    0x3F00   /* Timer Period */
#define RCCR_DR1M     0x0080   /* IDMA Request 1 Mode */
#define RCCR_DR0M     0x0040   /* IDMA Request 0 Mode */
#define RCCR_DRQP     0x0030   /* IDMA Request Priority */
#define RCCR_EIE      0x0008   /* External Interrupt Enable */
#define RCCR_SCD      0x0004   /* Scheduler Configuration */
#define RCCR_ERAM     0x0003   /* Enable RAM Microcode */



/*---------------------------------------------------------------*
 * RISC Timer Event Register (RTER)                              *
 *---------------------------------------------------------------*
 * NOTE: R/W register used to report events recognized by the 16 *
 *       timers and to generate interrupts. An interrupt is only *
 *       generated if the RISC timer table bit is set in the CPM *
 *       interrupt mask register. RTER can be read at any time.  *
 *       Bits are cleared by writing a 1 (zeros have no effect). *
 *       This register is cleared at reset. 					 *
 *---------------------------------------------------------------*/

#define RTER_TMR15   0x8000
#define RTER_TMR14   0x4000
#define RTER_TMR13   0x2000
#define RTER_TMR12   0x1000
#define RTER_TMR11   0x0800
#define RTER_TMR10   0x0400
#define RTER_TMR9    0x0200
#define RTER_TMR8    0x0100
#define RTER_TMR7    0x0080
#define RTER_TMR6    0x0040
#define RTER_TMR5    0x0020
#define RTER_TMR4    0x0010
#define RTER_TMR3    0x0008
#define RTER_TMR2    0x0004
#define RTER_TMR1    0x0002
#define RTER_TMR0    0x0001



/*--------------------------------------------------------------------*
 * RISC Timer Mask Register (RTMR)                                    *
 *--------------------------------------------------------------------*
 * NOTE: R/W register used to enable interrupts that can be generated *
 *       in the RTER. If a bit is set, it enables the corresponding	  *
 *		   interrupt in the RTER. If a bit is cleared, the RISC time  *
 *		   mask register (RTMR) masks the corresponding interrupt in  *
 *		   the RTER. An interrupt is only generated if the RISC timer *
 *		   table bit is set in the CIMR. This register is cleared at  *
 *		   reset. 													  *
 *--------------------------------------------------------------------*/

#define RTMR_TMR15   0x8000
#define RTMR_TMR14   0x4000
#define RTMR_TMR13   0x2000
#define RTMR_TMR12   0x1000
#define RTMR_TMR11   0x0800
#define RTMR_TMR10   0x0400
#define RTMR_TMR9    0x0200
#define RTMR_TMR8    0x0100
#define RTMR_TMR7    0x0080
#define RTMR_TMR6    0x0040
#define RTMR_TMR5    0x0020
#define RTMR_TMR4    0x0010
#define RTMR_TMR3    0x0008
#define RTMR_TMR2    0x0004
#define RTMR_TMR1    0x0002
#define RTMR_TMR0    0x0001



/*-------------------------------------------------------------------------*
 *                                   BRGs   					           *
 *-------------------------------------------------------------------------*/



/*--------------------------------------*
 * BRG1 Configuration Register (BRGC1)  *
 *--------------------------------------*
 * NOTE: BRG's 1-4 are cleared at reset *
 *--------------------------------------*/

#define RESERVED124   0xFFFC0000
#define BRGC1_RST     0x00020000  /* Reset BRG */
#define BRGC1_EN      0x00010000  /* Enable BRG Count */
#define BRGC1_EXTC    0x0000C000  /* External Clock Source */
#define BRGC1_ATB     0x00002000  /* Autobaud */
#define BRGC1_CD      0x00001FFE  /* Clock Divider */
#define BRGC1_DIV16   0x00000001  /* BRG Clock Prescaler Divide by 16 */



/*-------------------------------------*
 * BRG2 Configuration Register (BRGC2) *
 *-------------------------------------*/

#define RESERVED125   0xFFFC0000
#define BRGC2_RST     0x00020000  /* Reset BRG */
#define BRGC2_EN      0x00010000  /* Enable BRG Count */
#define BRGC2_EXTC    0x0000C000  /* External Clock Source */
#define BRGC2_ATB     0x00002000  /* Autobaud */
#define BRGC2_CD      0x00001FFE  /* Clock Divider */
#define BRGC2_DIV16   0x00000001  /* BRG Clock Prescaler Divide by 16 */



/*-------------------------------------*
 * BRG3 Configuration Register (BRGC3) *
 *-------------------------------------*/

#define RESERVED126   0xFFFC0000
#define BRGC3_RST     0x00020000  /* Reset BRG */
#define BRGC3_EN      0x00010000  /* Enable BRG Count */
#define BRGC3_EXTC    0x0000C000  /* External Clock Source */
#define BRGC3_ATB     0x00002000  /* Autobaud */
#define BRGC3_CD      0x00001FFE  /* Clock Divider */
#define BRGC3_DIV16   0x00000001  /* BRG Clock Prescaler Divide by 16 */



/*-------------------------------------*
 * BRG4 Configuration Register (BRGC4) *
 *-------------------------------------*/

#define RESERVED127   0xFFFC0000
#define BRGC4_RST     0x00020000  /* Reset BRG */
#define BRGC4_EN      0x00010000  /* Enable BRG Count */
#define BRGC4_EXTC    0x0000C000  /* External Clock Source */
#define BRGC4_ATB     0x00002000  /* Autobaud */
#define BRGC4_CD      0x00001FFE  /* Clock Divider */
#define BRGC4_DIV16   0x00000001  /* BRG Clock Prescaler Divide by 16 */



/*-------------------------------------------------------------------------*
 *                                   SCC1   					           *
 *-------------------------------------------------------------------------*/


/*--------------------------------------*
 * SCC1 General Mode Register (GSMR_L1) *
 *--------------------------------------*/

#define GSMR_L1_SIR     0x80000000	 /* Serial Infrared Encoding */
#define GSMR_L1_EDGE    0x60000000	 /* Clock Edge */
#define GSMR_L1_TCI     0x10000000	 /* Transmit Clock Invert */
#define GSMR_L1_TSNC    0x0C000000	 /* Transmit Sense */
#define GSMR_L1_RINV    0x02000000	 /* DPLL Receive Input Invert Data */
#define GSMR_L1_TINV    0x01000000	 /* DPLL Transmit Input Invert Data */
#define GSMR_L1_TPL     0x00E00000	 /* Tx Preamble Length */
#define GSMR_L1_TPP     0x00180000	 /* Tx Preamble Pattern */
#define GSMR_L1_TEND    0x00040000	 /* Transmitter Frame Ending */
#define GSMR_L1_TDCR    0x00030000	 /* Transmit Divide Clock Rate */
#define GSMR_L1_RDCR    0x0000C000	 /* Receive DPLL Clock Rate */
#define GSMR_L1_RENC    0x00003800   /* Receiver Decoding Method */
#define GSMR_L1_TENC    0x00000700   /* Transmitter Encoding Method */
#define GSMR_L1_DIAG    0x000000C0   /* Diagnostic Mode */
#define GSMR_L1_INT_LB  0x00000040   /* Internal Loopback mask */
#define GSMR_L1_ENR     0x00000020   /* Enable Receive */
#define GSMR_L1_ENT     0x00000010   /* Enable Transmit */
#define GSMR_L1_MODE    0x0000000F   /* Channel Protocol Mode */



/*--------------------------------------*
 * SCC1 General Mode Register (GSMR_H1) *
 *--------------------------------------*/

#define RESERVED128     0xFFF80000   
#define GSMR_H1_IRP     0x00040000   /* Infrared Rx Polarity */
#define RESERVED129     0x00020000   
#define GSMR_H1_GDE     0x00010000   /* Glitch Detect Enable */
#define GSMR_H1_TCRC    0x0000C000   /* Transparent CRC */
#define GSMR_H1_REVD    0x00002000   /* Reverse Data */
#define GSMR_H1_TRX     0x00001000   /* Transparent Receiver */
#define GSMR_H1_TTX     0x00000800   /* Transparent Transmitter */
#define GSMR_H1_CDP     0x00000400   /* CD Pulse */
#define GSMR_H1_CTSP    0x00000200   /* CTS Pulse */
#define GSMR_H1_CDS     0x00000100   /* CD Sampling */
#define GSMR_H1_CTSS    0x00000080   /* CTS Sampling */
#define GSMR_H1_TFL     0x00000040   /* Transmit FIFO Length */
#define GSMR_H1_RFW     0x00000020   /* Rx FIFO Width */
#define GSMR_H1_TXSY    0x00000010   /* Transmitter Synched to Receiver */
#define GSMR_H1_SYNL    0x0000000C   /* Sync Length */
#define GSMR_H1_RTSM    0x00000002   /* RTS mode */
#define GSMR_H1_RSYN    0x00000001   /* Receive Sync Timing */



/*--------------------------------------*
 * NOTE: Bit masks not provided for     *
 *       SCC Protocol Specific Mode	    *
 *		   Register1  (PSMR1)             *
 *--------------------------------------*/


 
/*------------------------------------------*
 * SCC1 Transmit_On-Demand Register (TODR1) *
 *------------------------------------------*/

#define TODR1_TOD     0x8000   /* Transmit on Demand */
#define RESERVED130   0x0FFF



/*-------------------------------------------*
 * SCC1 Data Synchronization Register (DSR1) *
 *-------------------------------------------*/

#define DSR1_SYN2   0xFF00 
#define DSR1_SYN1   0x00FF
#define DSR_ENET    0xD555
#define DSR_HDLC    0x7E7E    /* SCC Data Synch. Reg. */




/*-------------------------------------*
 * NOTE: Bit masks not provided for    *
 *   							               *
 *      SCC Event Register	1  (SCCE1)  *
 *      SCC Mask Register 1   (SCCM1)  *
 *      SCC Status Register 1 (SCCS1)  *
 *-------------------------------------*/



/*-------------------------------------------------------------------------*
 *                                   SCC2   					                  *
 *-------------------------------------------------------------------------*/


/*--------------------------------------*
 * SCC2 General Mode Register (GSMR_L2) *
 *--------------------------------------*/

#define GSMR_L2_SIR     0x80000000	 /* Serial Infrared Encoding */
#define GSMR_L2_EDGE    0x60000000	 /* Clock Edge */
#define GSMR_L2_TCI     0x10000000	 /* Transmit Clock Invert */
#define GSMR_L2_TSNC    0x0C000000	 /* Transmit Sense */
#define GSMR_L2_RINV    0x02000000	 /* DPLL Receive Input Invert Data */
#define GSMR_L2_TINV    0x01000000	 /* DPLL Transmit Input Invert Data */
#define GSMR_L2_TPL     0x00E00000	 /* Tx Preamble Length */
#define GSMR_L2_TPP     0x00180000	 /* Tx Preamble Pattern */
#define GSMR_L2_TEND    0x00040000	 /* Transmitter Frame Ending */
#define GSMR_L2_TDCR    0x00030000	 /* Transmit Divide Clock Rate */
#define GSMR_L2_RDCR    0x0000C000	 /* Receive DPLL Clock Rate */
#define GSMR_L2_RENC    0x00003800   /* Receiver Decoding Method */
#define GSMR_L2_TENC    0x00000700   /* Transmitter Encoding Method */
#define GSMR_L2_DIAG    0x000000C0   /* Diagnostic Mode */
#define GSMR_L2_ENR     0x00000020   /* Enable Receive */
#define GSMR_L2_ENT     0x00000010   /* Enable Transmit */
#define GSMR_L2_MODE    0x0000000F   /* Channel Protocol Mode */



/*--------------------------------------*
 * SCC2 General Mode Register (GSMR_H2) *
 *--------------------------------------*/

#define RESERVED131     0xFFF80000   
#define GSMR_H2_IRP     0x00040000   /* Infrared Rx Polarity */
#define RESERVED132     0x00020000   
#define GSMR_H2_GDE     0x00010000   /* Glitch Detect Enable */
#define GSMR_H2_TCRC    0x0000C000   /* Transparent CRC */
#define GSMR_H2_REVD    0x00002000   /* Reverse Data */
#define GSMR_H2_TRX     0x00001000   /* Transparent Receiver */
#define GSMR_H2_TTX     0x00000800   /* Transparent Transmitter */
#define GSMR_H2_CDP     0x00000400   /* CD Pulse */
#define GSMR_H2_CTSP    0x00000200   /* CTS Pulse */
#define GSMR_H2_CDS     0x00000100   /* CD Sampling */
#define GSMR_H2_CTSS    0x00000080   /* CTS Sampling */
#define GSMR_H2_TFL     0x00000040   /* Transmit FIFO Length */
#define GSMR_H2_RFW     0x00000020   /* Rx FIFO Width */
#define GSMR_H2_TXSY    0x00000010   /* Transmitter Synched to Receiver */
#define GSMR_H2_SYNL    0x0000000C   /* Sync Length */
#define GSMR_H2_RTSM    0x00000002   /* RTS mode */
#define GSMR_H2_RSYN    0x00000001   /* Receive Sync Timing */



/*--------------------------------------*
 * NOTE: Bit masks not provided for 	*
 *       SCC Protocol Specific Mode	    *
 *		 Register2  (PSMR2)				*
 *--------------------------------------*/


 
/*------------------------------------------*
 * SCC2 Transmit_On-Demand Register (TODR2) *
 *------------------------------------------*/

#define TODR2_TOD     0x8000   /* Transmit on Demand */
#define RESERVED133   0x0FFF



/*-------------------------------------------*
 * SCC2 Data Synchronization Register (DSR2) *
 *-------------------------------------------*/

#define DSR2_SYN2   0xFF00 
#define DSR2_SYN1   0x00FF



/*---------------------------------------*
 * NOTE: Bit masks not provided for      *
 *   							         *
 *       SCC Event Register	2  (SCCE2)   *
 *       SCC Mask Register 2   (SCCM2)   *
 *       SCC Status Register 2 (SCCS2)   *
 *---------------------------------------*/

 

/*-------------------------------------------------------------------------*
 *                                   SCC3   					           *
 *-------------------------------------------------------------------------*/


/*--------------------------------------*
 * SCC3 General Mode Register (GSMR_L3) *
 *--------------------------------------*/

#define GSMR_L3_SIR     0x80000000	 /* Serial Infrared Encoding */
#define GSMR_L3_EDGE    0x60000000	 /* Clock Edge */
#define GSMR_L3_TCI     0x10000000	 /* Transmit Clock Invert */
#define GSMR_L3_TSNC    0x0C000000	 /* Transmit Sense */
#define GSMR_L3_RINV    0x02000000	 /* DPLL Receive Input Invert Data */
#define GSMR_L3_TINV    0x01000000	 /* DPLL Transmit Input Invert Data */
#define GSMR_L3_TPL     0x00E00000	 /* Tx Preamble Length */
#define GSMR_L3_TPP     0x00180000	 /* Tx Preamble Pattern */
#define GSMR_L3_TEND    0x00040000	 /* Transmitter Frame Ending */
#define GSMR_L3_TDCR    0x00030000	 /* Transmit Divide Clock Rate */
#define GSMR_L3_RDCR    0x0000C000	 /* Receive DPLL Clock Rate */
#define GSMR_L3_RENC    0x00003800   /* Receiver Decoding Method */
#define GSMR_L3_TENC    0x00000700   /* Transmitter Encoding Method */
#define GSMR_L3_DIAG    0x000000C0   /* Diagnostic Mode */
#define GSMR_L3_ENR     0x00000020   /* Enable Receive */
#define GSMR_L3_ENT     0x00000010   /* Enable Transmit */
#define GSMR_L3_MODE    0x0000000F   /* Channel Protocol Mode */



/*--------------------------------------*
 * SCC3 General Mode Register (GSMR_H3) *
 *--------------------------------------*/

#define RESERVED134     0xFFF80000   
#define GSMR_H3_IRP     0x00040000   /* Infrared Rx Polarity */
#define RESERVED135     0x00020000   
#define GSMR_H3_GDE     0x00010000   /* Glitch Detect Enable */
#define GSMR_H3_TCRC    0x0000C000   /* Transparent CRC */
#define GSMR_H3_REVD    0x00002000   /* Reverse Data */
#define GSMR_H3_TRX     0x00001000   /* Transparent Receiver */
#define GSMR_H3_TTX     0x00000800   /* Transparent Transmitter */
#define GSMR_H3_CDP     0x00000400   /* CD Pulse */
#define GSMR_H3_CTSP    0x00000200   /* CTS Pulse */
#define GSMR_H3_CDS     0x00000100   /* CD Sampling */
#define GSMR_H3_CTSS    0x00000080   /* CTS Sampling */
#define GSMR_H3_TFL     0x00000040   /* Transmit FIFO Length */
#define GSMR_H3_RFW     0x00000020   /* Rx FIFO Width */
#define GSMR_H3_TXSY    0x00000010   /* Transmitter Synched to Receiver */
#define GSMR_H3_SYNL    0x0000000C   /* Sync Length */
#define GSMR_H3_RTSM    0x00000002   /* RTS mode */
#define GSMR_H3_RSYN    0x00000001   /* Receive Sync Timing */



/*--------------------------------------*
 * NOTE: Bit masks not provided for     *
 *       SCC Protocol Specific Mode	    *
 *		   Register3  (PSMR3)			*
 *--------------------------------------*/


/*------------------------------------------*
 * SCC3 Transmit_On-Demand Register (TODR3) *
 *------------------------------------------*/

#define TODR3_TOD     0x8000   /* Transmit on Demand */
#define RESERVED136   0x0FFF



/*-------------------------------------------*
 * SCC3 Data Synchronization Register (DSR3) *
 *-------------------------------------------*/

#define DSR3_SYN2   0xFF00 
#define DSR3_SYN1   0x00FF



/*---------------------------------------*
 * NOTE: Bit masks not provided for      *
 *   							         *
 *       SCC Event Register	3  (SCCE3)   *
 *       SCC Mask Register 3   (SCCM3)   *
 *       SCC Status Register 3 (SCCS3)   *
 *---------------------------------------*/




/*-------------------------------------------------------------------------*
 *                                   SCC4   					           *
 *-------------------------------------------------------------------------*/


/*--------------------------------------*
 * SCC4 General Mode Register (GSMR_L4) *
 *--------------------------------------*/

#define GSMR_L4_SIR     0x80000000	 /* Serial Infrared Encoding */
#define GSMR_L4_EDGE    0x60000000	 /* Clock Edge */
#define GSMR_L4_TCI     0x10000000	 /* Transmit Clock Invert */
#define GSMR_L4_TSNC    0x0C000000	 /* Transmit Sense */
#define GSMR_L4_RINV    0x02000000	 /* DPLL Receive Input Invert Data */
#define GSMR_L4_TINV    0x01000000	 /* DPLL Transmit Input Invert Data */
#define GSMR_L4_TPL     0x00E00000	 /* Tx Preamble Length */
#define GSMR_L4_TPP     0x00180000	 /* Tx Preamble Pattern */
#define GSMR_L4_TEND    0x00040000	 /* Transmitter Frame Ending */
#define GSMR_L4_TDCR    0x00030000	 /* Transmit Divide Clock Rate */
#define GSMR_L4_RDCR    0x0000C000	 /* Receive DPLL Clock Rate */
#define GSMR_L4_RENC    0x00003800   /* Receiver Decoding Method */
#define GSMR_L4_TENC    0x00000700   /* Transmitter Encoding Method */
#define GSMR_L4_DIAG    0x000000C0   /* Diagnostic Mode */
#define GSMR_L4_ENR     0x00000020   /* Enable Receive */
#define GSMR_L4_ENT     0x00000010   /* Enable Transmit */
#define GSMR_L4_MODE    0x0000000F   /* Channel Protocol Mode */



/*--------------------------------------*
 * SCC4 General Mode Register (GSMR_H4) *   
 *--------------------------------------*/

#define RESERVED137     0xFFF80000   
#define GSMR_H4_IRP     0x00040000   /* Infrared Rx Polarity */
#define RESERVED138     0x00020000   
#define GSMR_H4_GDE     0x00010000   /* Glitch Detect Enable */
#define GSMR_H4_TCRC    0x0000C000   /* Transparent CRC */
#define GSMR_H4_REVD    0x00002000   /* Reverse Data */
#define GSMR_H4_TRX     0x00001000   /* Transparent Receiver */
#define GSMR_H4_TTX     0x00000800   /* Transparent Transmitter */
#define GSMR_H4_CDP     0x00000400   /* CD Pulse */
#define GSMR_H4_CTSP    0x00000200   /* CTS Pulse */
#define GSMR_H4_CDS     0x00000100   /* CD Sampling */
#define GSMR_H4_CTSS    0x00000080   /* CTS Sampling */
#define GSMR_H4_TFL     0x00000040   /* Transmit FIFO Length */
#define GSMR_H4_RFW     0x00000020   /* Rx FIFO Width */
#define GSMR_H4_TXSY    0x00000010   /* Transmitter Synched to Receiver */
#define GSMR_H4_SYNL    0x0000000C   /* Sync Length */
#define GSMR_H4_RTSM    0x00000002   /* RTS mode */
#define GSMR_H4_RSYN    0x00000001   /* Receive Sync Timing */



/*--------------------------------------*
 * NOTE: Bit masks not provided for 	*
 *       SCC Protocol Specific Mode	    *
 *		   Register4  (PSMR4)			*
 *--------------------------------------*/


 
/*------------------------------------------*
 * SCC4 Transmit_On-Demand Register (TODR4) *
 *------------------------------------------*/

#define TODR4_TOD     0x8000   /* Transmit on Demand */
#define RESERVED139   0x0FFF



/*-------------------------------------------*
 * SCC4 Data Synchronization Register (DSR4) *
 *-------------------------------------------*/

#define DSR4_SYN2   0xFF00 
#define DSR4_SYN1   0x00FF



/*---------------------------------------*
 * NOTE: Bit masks not provided for      *
 *   							         *
 *       SCC Event Register	4  (SCCE4)   *
 *       SCC Mask Register 4   (SCCM4)   *
 *       SCC Status Register 4 (SCCS4)   *
 *---------------------------------------*/



/*-------------------------------------------------------------------------*
 *                                   SMC1   					           *
 *-------------------------------------------------------------------------*/



/*-----------------------------*
 * SMC1 Mode Register (SMCMR1) *
 *-----------------------------*/

#define RESERVED140     0x8000
#define SMCMR1_CLEN	   0x7800   /* Character Length */
#define SMCMR1_SL       0x0400   /* Stop Length */
#define SMCMR1_PEN      0x0200   /* Parity Enable */
#define SMCMR1_PM       0x0100   /* Parity Mode */
#define RESERVED141     0x00C0   /* These bits should be cleared by user */
#define SMCMR1_SM       0x0030   /* SMC Mode */
#define SMCMR1_DM       0x000C   /* Diagnostic Mode */
#define SMCMR1_TEN      0x0002   /* SMC Transmit Enable */
#define SMCMR1_REN      0x0001   /* SMC Receive Enable */



/*-----------------------------*
 * SMC1 Event Register (SMCE1) *
 *-----------------------------*/

#define RESERVED142   0xE0
#define SMCE1_TXE  	 0x10    /* Tx Error */
#define RESERVED143   0x08
#define SMCE1_BSY     0x04    /* Busy Condition */
#define SMCE1_TX      0x02    /* Tx Buffer */
#define SMCE1_RX      0x01    /* Rx Buffer */



/*----------------------------------------*
 * SMC1 Transparent Mask Register (SMCE1) *
 *----------------------------------------*/

#define RESERVED144   0xE0
#define SMCE1_TXE  	 0x10    /* Tx Error */
#define RESERVED145   0x08
#define SMCE1_BSY     0x04    /* Busy Condition */
#define SMCE1_TX      0x02    /* Tx Buffer */
#define SMCE1_RX      0x01    /* Rx Buffer */



/*-------------------------------------------------------------------------*
 *                                   SMC2   					           *
 *-------------------------------------------------------------------------*/



/*-----------------------------*
 * SMC2 Mode Register (SMCMR2) *
 *-----------------------------*/

#define RESERVED146     0x8000
#define SMCMR2_CLEN	   0x7800   /* Character Length */
#define SMCMR2_SL       0x0400   /* Stop Length */
#define SMCMR2_PEN      0x0200   /* Parity Enable */
#define SMCMR2_PM       0x0100   /* Parity Mode */
#define RESERVED147     0x00C0   /* These bits should be cleared by user */
#define SMCMR2_SM       0x0030   /* SMC Mode */
#define SMCMR2_DM       0x000C   /* Diagnostic Mode */
#define SMCMR2_TEN      0x0002   /* SMC Transmit Enable */
#define SMCMR2_REN      0x0001   /* SMC Receive Enable */



/*-----------------------------*
 * SMC2 Event Register (SMCE2) *
 *-----------------------------*/

#define RESERVED148   0xE0
#define SMCE2_TXE  	 0x10    /* Tx Error */
#define RESERVED149   0x08
#define SMCE2_BSY     0x04    /* Busy Condition */
#define SMCE2_TX      0x02    /* Tx Buffer */
#define SMCE2_RX      0x01    /* Rx Buffer */



/*----------------------------------------*
 * SMC2 Transparent Mask Register (SMCE2) *
 *----------------------------------------*/

#define RESERVED150   0xE0
#define SMCE2_TXE  	 0x10    /* Tx Error */
#define RESERVED151   0x08
#define SMCE2_BSY     0x04    /* Busy Condition */
#define SMCE2_TX      0x02    /* Tx Buffer */
#define SMCE2_RX      0x01    /* Rx Buffer */
                                             



/*-------------------------------------------------------------------------*
 *                                   SPI   					               *
 *-------------------------------------------------------------------------*/



/*----------------------------*
 * SPI Mode Register (SPMODE) *
 *----------------------------*/

#define SPMODE_LOOP   0x4000   /* Loop Mode */
#define SPMODE_CI     0x2000   /* Clock Invert */
#define SPMODE_CP     0x1000   /* Clock Phase */
#define SPMODE_DIV16  0x0800   /* Divide by 16 */
#define SPMODE_REV    0x0400   /* Reverse Data */
#define SPMODE_M_S    0x0200   /* Master / Slave */
#define SPMODE_EN     0x0100   /* Enabe SPI */
#define SPMODE_LEN    0x00F0   /* Character Length */
#define SPMODE_PM     0x000F   /* Prescale Modulus Select */



/*---------------------------*
 * SPI Event Register (SPIE) *
 *---------------------------*/

#define RESERVED152   0xC0
#define SPIE_MME      0x20       /* Multi Master Error */
#define SPIE_TXE      0x10       /* Tx Error */
#define RESERVED153   0x08       
#define SPIE_BSY      0x04       /* Busy Condition */
#define SPIE_TXB      0x02       /* Tx Buffer */
#define SPIE_RXB      0x01       /* Rx Buffer */



/*---------------------------*
 * SPI Mask Register (SPIM)  *
 *---------------------------*/

#define RESERVED154   0xC0
#define SPIM_MME      0x20       /* Multi Master Error */
#define SPIM_TXE      0x10       /* Tx Error */
#define RESERVED155   0x08       
#define SPIM_BSY      0x04       /* Busy Condition */
#define SPIM_TXB      0x02       /* Tx Buffer */
#define SPIM_RXB      0x01       /* Rx Buffer */


/*------------------------------*
 * SPI Command Register (SPCOM) *
 *------------------------------*/

#define SPCOM_STR     0x80       /* Start Transmit */ 
#define RESERVED156   0x7F   




/*-------------------------------------------------------------------------*
 *                                   PIP   					               *
 *-------------------------------------------------------------------------*/



/*-----------------------------------*
 * PIP Configuration Register (PIPC) *
 *-----------------------------------*/

#define PIPC_STR      0x8000    /* Start (valid for a transmitter only) */
#define RESERVED157   0x7000    
#define PIPC_SACK     0x0800    /* Set Acknowledge */
#define PIPC_CBSY     0x0400    /* Clear BUSY */
#define PIPC_SBSY     0x0200    /* Set Busy */
#define PIPC_EBSY     0x0100    /* Enable BUSY (receiver) */
#define PIPC_TMOD     0x00C0    /* Timing Mode (Centronics Receiver) */
#define PIPC_MODL     0x0030    /* Mode Low */
#define PIPC_MODH     0x000C    /* Mode High */
#define PIPC_HSC      0x0002    /* Host Control */
#define PIPC_T_R      0x0001    /* Transmit/Receive Select */



/*-----------------------------------*
 * PIP Timing Params Register (PTPR) *
 *-----------------------------------*/

#define PTPR_TPAR2   0xFF00   /* Timing Parameter 2 */
#define PTPR_TPAR1   0x00FF   /* Timing Parameter 1 */




/*------------------------------------------*
 * Port B Data Direction Register (PBDIR)   *
 *------------------------------------------*
 * NOTE: For all DRx bits					*
 *      									*
 *       0 = Corresponding pin is an input  *
 *       1 = Corresponding pin is an output *
 *  										*
 *  This register is cleared at system		*
 *  reset.								    *
 *------------------------------------------*/

#define RESERVED158   0xFFFC0000
#define PBDIR_DR14	  0x00020000  
#define PBDIR_DR15    0x00010000
#define PBDIR_DR16    0x00008000
#define PBDIR_DR17    0x00004000
#define PBDIR_DR18    0x00002000
#define PBDIR_DR19    0x00001000
#define PBDIR_DR20	  0x00000800
#define PBDIR_DR21    0x00000400
#define PBDIR_DR22    0x00000200
#define PBDIR_DR23    0x00000100
#define PBDIR_DR24    0x00000080
#define PBDIR_DR25    0x00000040
#define PBDIR_DR26    0x00000020
#define PBDIR_DR27    0x00000010
#define PBDIR_DR28    0x00000008
#define PBDIR_DR29    0x00000004
#define PBDIR_DR30    0x00000002
#define PBDIR_DR31    0x00000001




/*------------------------------------------------------------*
 * Port B Pin Assignment Register (PBPAR)                     *
 *------------------------------------------------------------*
 * NOTE: For each DDX bit				                      *
 *      									                  *
 *       0 = General purpose I/O. Peripheral functions of the *
 *           pin are not used.								  *
 *                                                            *
 *       1 = Dedictated peripheral function. Pin is used by   *
 *           internal module. The on-chip peripheral function *
 *			    to which it is dedicated can be determined by *
 *			    other bits such as those in the PBDIR.		  *  
 *															  *
 * This register is cleared at system reset.			      *
 *------------------------------------------------------------*/

#define RESERVED159   0xFFFC0000
#define PBPAR_DD14	 0x00020000  
#define PBPAR_DD15    0x00010000
#define PBPAR_DD16    0x00008000
#define PBPAR_DD17    0x00004000
#define PBPAR_DD18    0x00002000
#define PBPAR_DD19    0x00001000
#define PBPAR_DD20	 0x00000800
#define PBPAR_DD21    0x00000400
#define PBPAR_DD22    0x00000200
#define PBPAR_DD23    0x00000100
#define PBPAR_DD24    0x00000080
#define PBPAR_DD25    0x00000040
#define PBPAR_DD26    0x00000020
#define PBPAR_DD27    0x00000010
#define PBPAR_DD28    0x00000008
#define PBPAR_DD29    0x00000004
#define PBPAR_DD30    0x00000002
#define PBPAR_DD31    0x00000001




/*-------------------------------------------------*
 * Port B Open Drain Register (PBODR)              *
 *-------------------------------------------------*
 * NOTE: For all ODx bits			        	   *
 *      								           *
 *       0 = The I/O pin is actively driven        *
 *           as an output                          *
 *												   *
 *       1 = The I/O pin is an open-drain driver.  *
 *           As an output, the pin is actively     *
 *           driven low. Otherwise, it is 		   *
 *           three-stated. 					       *
 *												   *
 * This register is cleared at system reset.	   *
 *-------------------------------------------------*/

#define PBODR_OD16   0x8000
#define PBODR_OD17   0x4000
#define PBODR_OD18   0x2000
#define PBODR_OD19   0x1000
#define PBODR_OD20   0x0800
#define PBODR_OD21   0x0400
#define PBODR_OD22   0x0200
#define PBODR_OD23   0x0100
#define PBODR_OD24   0x0080
#define PBODR_OD25   0x0040
#define PBODR_OD26   0x0020
#define PBODR_OD27   0x0010
#define PBODR_OD28   0x0008
#define PBODR_OD29   0x0004
#define PBODR_OD30   0x0002
#define PBODR_OD31   0x0001




/*--------------------------------------------*
 * Port B Data Register (PBDAT)               *
 *--------------------------------------------*
 * NOTE: This register is undefined at reset. *
 *       See MPC860 User's Manual.		      *
 *--------------------------------------------*/

#define RESERVED160   0xFFFC0000
#define PBDAT_D14     0x00020000
#define PBDAT_D15     0x00010000
#define PBDAT_D16     0x00008000
#define PBDAT_D17     0x00004000
#define PBDAT_D18     0x00002000
#define PBDAT_D19     0x00001000
#define PBDAT_D20     0x00000800
#define PBDAT_D21     0x00000400
#define PBDAT_D22     0x00000200
#define PBDAT_D23     0x00000100
#define PBDAT_D24     0x00000080
#define PBDAT_D25     0x00000040
#define PBDAT_D26     0x00000020
#define PBDAT_D27     0x00000010
#define PBDAT_D28     0x00000008
#define PBDAT_D29     0x00000004
#define PBDAT_D30     0x00000002
#define PBDAT_D31     0x00000001




/*-------------------------------------------------------------------------*
 *                                   SI   					               *
 *-------------------------------------------------------------------------*/


/*---------------------------*
 * SI Mode Register (SIMODE) *
 *---------------------------*/

#define SIMODE_SMC2      0x80000000   /* SMC2 Connection */
#define SIMODE_SMC2CS	 0x70000000   /* SMC2 Clock Source (NMSI mode) */
#define SIMODE_SDMB      0x0C000000   /* SI Diagnostic Mode for TDM B */
#define SIMODE_RFSDB     0x03000000   /* Receive Frame Sync delay TDM B */
#define SIMODE_DSCB      0x00800000   /* Double Speed Clock for TDM B */
#define SIMODE_CRTB      0x00400000   /* Common Rec/Trans Pins for TDM B */
#define SIMODE_STZB      0x00200000   /* Set L1TXDx to Zero for TDM B */
#define SIMODE_CEB       0x00100000   /* Clock Edge for TDM B */
#define SIMODE_FEB       0x00080000   /* Frame Sync Edge for TDM B */
#define SIMODE_GMB       0x00040000   /* Grant Mode for TDM B */
#define SIMODE_TFSDB     0x00030000   /* Transmit Frame Sync Delay TDM B */
#define SIMODE_SMC1      0x00008000   /* SMC1 Connection */
#define SIMODE_SMC1CS    0x00007000   /* SMC1 Clock Source (NMSI mode) */
#define SIMODE_SDMA      0x00000C00   /* SI Diagnostic Mode for TDM A */
#define SIMODE_RFSDA     0x00000300   /* Receive Frame Sync delay TDM A */
#define SIMODE_DSCA      0x00000080   /* Double Speed Clock for TDM A */
#define SIMODE_CRTA      0x00000040   /* Common Rec/Trans Pins for TDM A */
#define SIMODE_STZA      0x00000020   /* Set L1TXDx to Zero for TDM A */
#define SIMODE_CEA       0x00000010   /* Clock Edge for TDM A */
#define SIMODE_FEA       0x00000008   /* Frame Sync Edge for TDM A */
#define SIMODE_GMA       0x00000004   /* Grant Mode for TDM A */
#define SIMODE_TFSDA     0x00000003   /* Transmit Frame Sync Delay TDM A */



/*---------------------------------*
 * SI Global Mode Register (SIGMR) *
 *---------------------------------*/

#define RESERVED161   0xF0
#define SIGMR_ENB     0x08   /* Enable Channel B */
#define SIGMR_ENA     0x04   /* Enable Channel A */
#define SIGMR_RDM     0x03   /* RAM Division Mode */ 



/*----------------------------*
 * SI Status Register (SISTR) *
 *----------------------------*/

#define SISTR_CRORA   0x80   /* Current Route of TDMa Receiver */
#define SISTR_CROTA   0x40   /* Current Route of TDMa Transmitter */
#define SISTR_CRORB   0x20   /* Current Route of TDMb Receiver */
#define SISTR_CROTB   0x10   /* Current Route of TDMb Transmitter */
#define RESERVED162   0x0F

 

/*-----------------------------*
 * SI Command Register (SICMR) *
 *-----------------------------*/

#define SICMR_CSRRA   0x80   /* Change Shadow RAM for TDM A Receiver */
#define SICMR_CSRTA   0x40   /* Change Shadow RAM for TDM A Transmitter */
#define SICMR_CSRRB   0x20   /* Change Shadow RAM for TDM B Receiver */
#define SICMR_CSRTB   0x10   /* Change Shadow RAM for TDM B Transmitter */
#define RESERVED163   0x0F



/*--------------------------------*
 * SI Clock Route Register (SICR) *
 *--------------------------------*/

#define SICR_GR4   0x80000000   /* Grant Support of SCC4 */
#define SICR_SC4   0x40000000   /* SCC4 Connection */
#define SICR_R4CS  0x38000000   /* Receive Clock Source for SCC4 */
#define SICR_T4CS  0x07000000   /* Transmit Clock Source for SCC4 */
#define SICR_GR3   0x00800000   /* Grant Support of SCC3 */
#define SICR_SC3   0x00400000   /* SCC3 Connection */
#define SICR_R3CS  0x00380000   /* Receive Clock Source for SCC3 */
#define SICR_T3CS  0x00070000   /* Transmit Clock Source for SCC3 */
#define SICR_GR2   0x00008000   /* Grant Support of SCC2 */
#define SICR_SC2   0x00004000   /* SCC2 Connection */
#define SICR_R2CS  0x00003800   /* Receive Clock Source for SCC2 */
#define SICR_T2CS  0x00000700   /* Transmit Clock Source for SCC2 */
#define SICR_GR1   0x00000080   /* Grant Support of SCC1 */
#define SICR_SC1   0x00000040   /* SCC1 Connection */
#define SICR_R1CS  0x00000038   /* Receive Clock Source for SCC1 */
#define SICR_T1CS  0x00000007   /* Transmit Clock Source for SCC1 */



/*-------------------------------------*
 * NOTE: No bit masks are provided for *
 *       the SI RAM Pointer Register.  *
 *-------------------------------------*/


/*-------------------------------------------------------------------------*
 *                        Protocol Specific Masks                          *
 *-------------------------------------------------------------------------*/
 

/********/
/* HDLC */
/********/

/*----------------------------*/
/* SCCE Register in HDLC Mode */
/*----------------------------*/


#define  HDLC_SCCE_RXB     0x0001  /* Complete buffer not received */
#define  HDLC_SCCE_TXB     0x0002  /* Last byte of the buffer xmited */
#define  HDLC_SCCE_BSY     0x0004  /* frame discarded-no buffer */
#define  HDLC_SCCE_RXF     0x0008  /* frame received */
#define  HDLC_SCCE_TXE     0x0010  /* some error like CTS lost or overrun */
#define  HDLC_SCCE_GRA     0x0080  /* Graceful stop complete */
#define  HDLC_SCCE_IDL     0x0100  /* Idle sequence status changed */
#define  HDLC_SCCE_FLG     0x0200  /* HDLC idle flag status */
#define  HDLC_SCCE_DCC     0x0400  /* DPLL CS Changed */
#define  HDLC_SCCE_GLT     0x0800  /* Glitch on Tx */
#define  HDLC_SCCE_GLR     0x1000  /* Glitch on Rx */


/*----------------------------*/
/* SCCM Register in HDLC Mode */
/*----------------------------*/


#define  HDLC_SCCM_RXB     0x0001  /* Complete buffer not received */
#define  HDLC_SCCM_TXB     0x0002  /* Last byte of the buffer xmited */
#define  HDLC_SCCM_BSY     0x0004  /* frame discarded-no buffer */
#define  HDLC_SCCM_RXF     0x0008  /* frame received */
#define  HDLC_SCCM_TXE     0x0010  /* some error like CTS lost or overrun */
#define  HDLC_SCCM_GRA     0x0080  /* Graceful stop complete */
#define  HDLC_SCCM_IDL     0x0100  /* Idle sequence status changed */
#define  HDLC_SCCM_FLG     0x0200  /* HDLC idle flag status */
#define  HDLC_SCCM_DCC     0x0400  /* DPLL CS Changed */
#define  HDLC_SCCM_GLT     0x0800  /* Glitch on Tx */
#define  HDLC_SCCM_GLR     0x1000  /* Glitch on Rx */


/*----------------------------*/
/* PSMR Register in HDLC Mode */
/*----------------------------*/

#define  HDLC_PSMR_MFF           0x0008   /* Multiple frames in FIFO */
#define  HDLC_PSMR_BRM           0x0010   /* HDLC Bus RTS Mode */
#define  HDLC_PSMR_BUS           0x0020   /* HDLC Bus Mode */
#define  HDLC_PSMR_DRT           0x0040   /* Disable Reciever While Xmitting */
#define  HDLC_PSMR_FSE           0x0080   /* Flag Sharing Enable */
#define  HDLC_PSMR_RTE           0x0200   /* Retransmit Enable */
#define  HDLC_PSMR_16BIT_CRC     0x0000   /* CRC Selection */
#define  HDLC_PSMR_32BIT_CRC     0x0800   /* CRC Selection */
#define  HDLC_PSMR_NOF_0         0x0000   /* No flags between frames */
#define  HDLC_PSMR_NOF_1         0x1000   /* 1 flag between frames */
#define  HDLC_PSMR_NOF_2         0x2000   /* 2 flags between frames */
#define  HDLC_PSMR_NOF_3         0x3000   /* 3 flags between frames */
#define  HDLC_PSMR_NOF_4         0x4000   /* 4 flags between frames */
#define  HDLC_PSMR_NOF_5         0x5000   /* 5 flags between frames */
#define  HDLC_PSMR_NOF_6         0x6000   /* 6 flags between frames */
#define  HDLC_PSMR_NOF_7         0x7000   /* 7 flags between frames */
#define  HDLC_PSMR_NOF_8         0x8000   /* 8 flags between frames */
#define  HDLC_PSMR_NOF_9         0x9000   /* 9 flags between frames */
#define  HDLC_PSMR_NOF_10        0xA000   /* 10 flags between frames */
#define  HDLC_PSMR_NOF_11        0xB000   /* 11 flags between frames */
#define  HDLC_PSMR_NOF_12        0xC000   /* 12 flags between frames */
#define  HDLC_PSMR_NOF_13        0xD000   /* 13 flags between frames */
#define  HDLC_PSMR_NOF_14        0xE000   /* 14 flags between frames */
#define  HDLC_PSMR_NOF_15        0xF000   /* 15 flags between frames */



/********************/
/* Transparent Mode */
/********************/

/*-----------------------------------*/
/* SCCE Register in Transparent Mode */
/*-----------------------------------*/

#define  TRAN_SCCE_RXB     0x0001  /* Complete frame not completed */
#define  TRAN_SCCE_TXB     0x0002  /* Last byte of the buffer xmited */
#define  TRAN_SCCE_BSY     0x0004  /* byte/word discarded-no buffer */
#define  TRAN_SCCE_RXH     0x0008  /* byte/word received */
#define  TRAN_SCCE_TXE     0x0010  /* some error like CTS lost or overrun */
#define  TRAN_SCCE_GRA     0x0080  /* Graceful stop complete */
#define  TRAN_SCCE_DCC     0x0400  /* DPLL CS Changed */
#define  TRAN_SCCE_GLT     0x0800  /* Glitch on Tx */
#define  TRAN_SCCE_GLR     0x1000  /* Glitch on Rx */


/*-----------------------------------*/
/* SCCM Register in Transparent Mode */
/*-----------------------------------*/

#define  TRAN_SCCM_RXB     0x0001  /* Complete frame not completed */
#define  TRAN_SCCM_TXB     0x0002  /* Last byte of the buffer xmited */
#define  TRAN_SCCM_BSY     0x0004  /* byte/word discarded-no buffer */
#define  TRAN_SCCM_RXH     0x0008  /* byte/word received */
#define  TRAN_SCCM_TXE     0x0010  /* some error like CTS lost or overrun */
#define  TRAN_SCCM_GRA     0x0080  /* Graceful stop complete */
#define  TRAN_SCCM_DCC     0x0400  /* DPLL CS Changed */
#define  TRAN_SCCM_GLT     0x0800  /* Glitch on Tx */
#define  TRAN_SCCM_GLR     0x1000  /* Glitch on Rx */



/************/
/* Ethernet */
/************/

/*--------------------------------*/
/* SCCE Register in Ethernet Mode */
/*--------------------------------*/

#define ENET_SCCE_GRA	0x0080  /* Graceful stop Complete */
#define ENET_SCCE_TXE	0x0010  /* Tx Error */
#define ENET_SCCE_RXF	0x0008  /* Rx Frame */
#define ENET_SCCE_BSY   0x0004	/* Busy Condition */
#define ENET_SCCE_TXB	0x0002	/* Tx Buffer */
#define ENET_SCCE_RXB   0x0001  /* Rx Buffer */


/*--------------------------------*/
/* SCCM Register in Ethernet Mode */
/*--------------------------------*/

#define ENET_SCCM_GRA	0x0080  /* Graceful stop Complete */
#define ENET_SCCM_TXE	0x0010  /* Tx Error */
#define ENET_SCCM_RXF	0x0008  /* Rx Frame */
#define ENET_SCCM_BSY   0x0004	/* Busy Condition */
#define ENET_SCCM_TXB	0x0002	/* Tx Buffer */
#define ENET_SCCM_RXB   0x0001  /* Rx Buffer */


/*--------------------------------*/
/* PSMR Register in Ethernet Mode */
/*--------------------------------*/

#define ENET_PSMR_HBC		0x8000  /* Heartbeat Checking */
#define ENET_PSMR_FC			0x4000  /* Force Collision */
#define ENET_PSMR_RSH		0x2000  /* Receive Short Frames */
#define ENET_PSMR_IAM		0x1000  /* Individual Address Mode */
#define ENET_PSMR_CRC      0x0C00  /* CRC Selection */
#define ENET_PSMR_PRO		0x0200  /* Promiscuous */
#define ENET_PSMR_BRO      0x0100  /* Broadcast Address */
#define ENET_PSMR_SBT      0x0080  /* Stop Backoff Timer */
#define ENET_PSMR_LPB		0x0040  /* Loopback Operation */
#define ENET_PSMR_SIP		0x0020  /* Sample Input Pins */ 
#define ENET_PSMR_LCW      0x0010  /* Late Collision Window */
#define ENET_PSMR_NIB      0x000E  /* Number of Ignored Bits */
#define ENET_PSMR_FDE      0x0001  /* Full Duplex Ethernet */

/*---------------------------*/
/* Ethernet Buffer Descriptor */
/*---------------------------*/

#define BD_PAD			0x4000
#define BD_TC			0x0400
#define BD_DEF			0x0200
#define BD_HB			0x0100
#define BD_LC			0x0080
#define BD_RL			0x0040
#define BD_RMASK		0x003c
#define BD_UN			0x0002
#define BD_CSL			0x0001

/*---------------------------*/
/* Generic Buffer Descriptor */
/*---------------------------*/

#define BD_READY		0x8000
#define BD_WRAP			0x2000
#define BD_INTERRUPT	0x1000
#define BD_LAST			0x0800
