/*
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
 * This file is a combination of the original files ppcmal.h and mal.h.
 *
 */
#ifndef _ppcmal_h_
#define _ppcmal_h_

#define MAL_DCR_BASE 0x180
#define malmcr		(MAL_DCR_BASE+0x00)	/* MAL Config reg */
#define malesr		(MAL_DCR_BASE+0x01)	/* Error Status reg (Read/Clear) */
#define malier		(MAL_DCR_BASE+0x02)	/* Interrupt enable reg */
#define maldbr		(MAL_DCR_BASE+0x03)	/* Mal Debug reg (Read only) */
#define maltxcasr	(MAL_DCR_BASE+0x04)	/* TX Channel active reg (set) */
#define maltxcarr	(MAL_DCR_BASE+0x05)	/* TX Channel active reg (Reset) */
#define maltxeobisr	(MAL_DCR_BASE+0x06)	/* TX End of buffer int status reg */
#define maltxdeir	(MAL_DCR_BASE+0x07)	/* TX Descr. Error Int reg */
#define malrxcasr	(MAL_DCR_BASE+0x10)	/* RX Channel active reg (set) */
#define malrxcarr	(MAL_DCR_BASE+0x11)	/* RX Channel active reg (Reset) */
#define malrxeobisr	(MAL_DCR_BASE+0x12)	/* RX End of buffer int status reg */
#define malrxdeir	(MAL_DCR_BASE+0x13)	/* RX Descr. Error Int reg */
#define maltxctp0r	(MAL_DCR_BASE+0x20)	/* TX 0 Channel table pointer reg */
#define maltxctp1r	(MAL_DCR_BASE+0x21)	/* TX 1 Channel table pointer reg */
#define malrxctp0r	(MAL_DCR_BASE+0x40)	/* RX 0 Channel table pointer reg */
#define malrcbs0	(MAL_DCR_BASE+0x60)	/* RX 0 Channel buffer size reg */

/* MADMAL transmit and receive status/control bits  */
/* for COMMAC bits, refer to the COMMAC header file */

#define MAL_TX_CTRL_READY		 0x8000
#define MAL_TX_CTRL_WRAP 		 0x4000
#define MAL_TX_CTRL_CM   		 0x2000
#define MAL_TX_CTRL_LAST 		 0x1000
#define MAL_TX_CTRL_INTR 		 0x0400

#define MAL_RX_CTRL_EMPTY		 0x8000
#define MAL_RX_CTRL_WRAP 		 0x4000
#define MAL_RX_CTRL_CM	 		 0x2000
#define MAL_RX_CTRL_LAST 		 0x1000
#define MAL_RX_CTRL_FIRST		 0x0800
#define MAL_RX_CTRL_INTR 		 0x0400

      /* Configuration Reg  */
#define MAL_CR_MMSR				0x80000000
#define MAL_CR_PLBP_1			0x00400000   /* lowsest is 00 */
#define MAL_CR_PLBP_2			0x00800000
#define MAL_CR_PLBP_3			0x00C00000   /* highest       */
#define MAL_CR_GA				0x00200000
#define MAL_CR_OA				0x00100000
#define MAL_CR_PLBLE			0x00080000
#define MAL_CR_PLBLT_1			0x00040000
#define MAL_CR_PLBLT_2			0x00020000
#define MAL_CR_PLBLT_3			0x00010000
#define MAL_CR_PLBLT_4			0x00008000
#define MAL_CR_PLBLT_DEFAULT	0x00078000 /* ????? */	  
#define MAL_CR_PLBB				0x00004000
#define MAL_CR_OPBBL			0x00000080
#define MAL_CR_EOPIE			0x00000004
#define MAL_CR_LEA				0x00000002
#define MAL_CR_MSD				0x00000001

    /* Error Status Reg    */
#define MAL_ESR_EVB				0x80000000
#define MAL_ESR_CID				0x40000000
#define MAL_ESR_DE				0x00100000
#define MAL_ESR_ONE				0x00080000
#define MAL_ESR_OTE				0x00040000
#define MAL_ESR_OSE				0x00020000
#define MAL_ESR_PEIN			0x00010000
#define MAL_ESR_ANY				0x801F0000
      /* same bit position as the IER */
      /* VV                      VV   */
#define MAL_ESR_DEI				0x00000010
#define MAL_ESR_ONEI			0x00000008
#define MAL_ESR_OTEI			0x00000004
#define MAL_ESR_OSEI			0x00000002
#define MAL_ESR_PBEI			0x00000001
      /* ^^                      ^^   */
      /* Mal IER                      */
#define MAL_IER_DE				0x00000010
#define MAL_IER_NE				0x00000008
#define MAL_IER_TE				0x00000004
#define MAL_IER_OPBE			0x00000002
#define MAL_IER_PLBE			0x00000001


#ifndef ASSEMBLY_ONLY
/* MAL Buffer Descriptor structure */
typedef struct {
  short  ctrl;              /* MAL / Commac status control bits */
  short  data_len;          /* Max length is 4K-1 (12 bits)     */
  char  *data_ptr;          /* pointer to actual data buffer    */
} mal_desc_t;
    
unsigned long   ppcMfmalcr(void);
void            ppcMtmalcr(unsigned long);
unsigned long   ppcMfmalesr(void);
void            ppcMtmalesr(unsigned long);
unsigned long   ppcMfmalier(void);
void            ppcMtmalier(unsigned long);
unsigned long   ppcMfmaldbr(void);
unsigned long   ppcMfmaltxcasr(void);
void            ppcMtmaltxcasr(unsigned long);
unsigned long   ppcMfmaltxcarr(void);
void            ppcMtmaltxcarr(unsigned long);
unsigned long   ppcMfmaltxeobisr(void);
void            ppcMtmaltxeobisr(unsigned long);
unsigned long   ppcMfmaltxdeir(void);
void            ppcMtmaltxdeir(unsigned long);
unsigned long   ppcMfmalrxcasr(void);
void            ppcMtmalrxcasr(unsigned long);
unsigned long   ppcMfmalrxcarr(void);
void            ppcMtmalrxcarr(unsigned long);
unsigned long   ppcMfmalrxeobisr(void);
void            ppcMtmalrxeobisr(unsigned long);
unsigned long   ppcMfmalrxdeir(void);
void            ppcMtmalrxdeir(unsigned long);
unsigned long   ppcMfmaltxctp0r(void);
void            ppcMtmaltxctp0r(unsigned long);
unsigned long   ppcMfmaltxctp1r(void);
void            ppcMtmaltxctp1r(unsigned long);
unsigned long   ppcMfmalrxctp0r(void);
void            ppcMtmalrxctp0r(unsigned long);
unsigned long   ppcMfmalrcbs0(void);
void            ppcMtmalrcbs0(unsigned long);
#endif

#endif
