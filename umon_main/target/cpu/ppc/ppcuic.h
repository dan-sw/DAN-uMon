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
 * This file is a combination of the original uic.h and ppcuic.h.
 */
#ifndef _ppcuic_h_
#define _ppcuic_h_

/* Universal interrupt controller */

/* For some reason the base address of the UIC_DCR is different on
 * the PPCSTB04300 (which is supposed to be PPC405-compliant!!!).
 * So, to support something other than 0xc0, we allow it to be defined
 * in config.h or default to 0xc0 here...
 * By the way, on the PPCSTB04300 its 0x40.
 */
#ifndef UIC_DCR_BASE
#define UIC_DCR_BASE	0xc0
#endif
#define uicsr        (UIC_DCR_BASE+0x0)  /* UIC status                       */
#define uicsrs       (UIC_DCR_BASE+0x1)  /* UIC status set                   */
#define uicer        (UIC_DCR_BASE+0x2)  /* UIC enable                       */
#define uiccr        (UIC_DCR_BASE+0x3)  /* UIC critical                     */
#define uicpr        (UIC_DCR_BASE+0x4)  /* UIC polarity                     */
#define uictr        (UIC_DCR_BASE+0x5)  /* UIC triggering                   */
#define uicmsr       (UIC_DCR_BASE+0x6)  /* UIC masked status                */
#define uicvr        (UIC_DCR_BASE+0x7)  /* UIC vector                       */
#define uicvcr       (UIC_DCR_BASE+0x8)  /* UIC vector configuration         */

#ifndef ASSEMBLY_ONLY
/* --------------------------- UIC DCRs ------------------------------------*/
unsigned long   ppcMfuiccr(void);
void            ppcMtuiccr(unsigned long);
unsigned long   ppcMfuicer(void);
void            ppcMtuicer(unsigned long);
unsigned long   ppcMfuicmsr(void);          /* R/O */
unsigned long   ppcMfuicpr(void);
void            ppcMtuicpr(unsigned long);
unsigned long   ppcMfuicsr(void);
void            ppcMtuicsr(unsigned long);
unsigned long   ppcMfuicsrs(void);
void            ppcMtuicsrs(unsigned long);
unsigned long   ppcMfuictr(void);
void            ppcMtuictr(unsigned long);
unsigned long   ppcMfuicvr(void);           /* R/O */
void            ppcMtuicvcr(unsigned long); /* W/O */
#endif

/*-----------------------------------------------------------------------------+
|  Universal interrupt controller interrupts
+-----------------------------------------------------------------------------*/
#define UIC_UART0     0x80000000      /* UART 0                             */
#define UIC_UART1     0x40000000      /* UART 1                             */
#define UIC_IIC       0x20000000      /* IIC                                */
#define UIC_EXT_MAST  0x10000000      /* External Master                    */
#define UIC_PCI       0x08000000      /* PCI                                */
#define UIC_DMA0      0x04000000      /* DMA chan. 0                        */
#define UIC_DMA1      0x02000000      /* DMA chan. 1                        */
#define UIC_DMA2      0x01000000      /* DMA chan. 2                        */
#define UIC_DMA3      0x00800000      /* DMA chan. 3                        */
#define UIC_EMAC_WAKE 0x00400000      /* EMAC wake up                       */
#define UIC_MAL_SERR  0x00200000      /* MAL SERR                           */
#define UIC_MAL_TXEOB 0x00100000      /* MAL TXEOB                          */
#define UIC_MAL_RXEOB 0x00080000      /* MAL RXEOB                          */
#define UIC_MAL_TXDE  0x00040000      /* MAL TXDE                           */
#define UIC_MAL_RXDE  0x00020000      /* MAL RXDE                           */
#define UIC_ENET      0x00010000      /* Ethernet                           */
#define UIC_EXT_PCI_SERR 0x00008000   /* External PCI SERR#                 */
#define UIC_CPU_TIMER 0x00004000      /* CPU timer                          */
#define UIC_EXT0      0x00000040      /* External  interrupt 0              */
#define UIC_EXT1      0x00000020      /* External  interrupt 1              */
#define UIC_EXT2      0x00000010      /* External  interrupt 2              */
#define UIC_EXT3      0x00000008      /* External  interrupt 3              */
#define UIC_EXT4      0x00000004      /* External  interrupt 4              */
#define UIC_EXT5      0x00000002      /* External  interrupt 5              */
#define UIC_EXT6      0x00000001      /* External  interrupt 6              */

#endif /* _ppcuic_h_ */
