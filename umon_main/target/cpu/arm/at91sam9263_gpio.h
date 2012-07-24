//==========================================================================
//
// at91sam9263_gpio.h
//
// Author(s):    Michael Kelly, Cogent Computer Systems, Inc.
// Contributors: 
// Date:         07/16/2003
// Description:  This file contains register offsets and bit defines
//				 for the AT91SAM9263 GPIO blocks
//

#ifndef AT91SAM9263_GPIO_H
#define AT91SAM9263_GPIO_H

#include "bits.h"

// Register Offsets
#define PIO_PER 	0x00	// PIO Enable Register
#define PIO_PDR 	0x04	// PIO Disable Register
#define PIO_PSR 	0x08	// PIO Status Register
#define PIO_OER 	0x10	// Output Enable Register
#define PIO_ODR 	0x14	// Output Disable Register
#define PIO_OSR 	0x18	// Output Status Register
#define PIO_IFER 	0x20	// Input Filter Enable Register
#define PIO_IFDR 	0x24	// Input Filter Disable Register
#define PIO_IFSR 	0x28	// Input Filter Status Register
#define PIO_SODR 	0x30	// Set Output Data Register
#define PIO_CODR 	0x34	// Clear Output Data Register
#define PIO_ODSR 	0x38	// Output Data Status Register
#define PIO_PDSR 	0x3c	// Pin Data Status Register
#define PIO_IER 	0x40	// Interrupt Enable Register
#define PIO_IDR 	0x44	// Interrupt Disable Register
#define PIO_IMR 	0x48	// Interrupt Mask Register
#define PIO_ISR 	0x4c	// Interrupt Status Register
#define PIO_MDER 	0x50	// Multi-driver Enable Register
#define PIO_MDDR 	0x54	// Multi-driver Disable Register
#define PIO_MDSR 	0x58	// Multi-driver Status Register
#define PIO_PUDR 	0x60	// Pull-up Disable Register
#define PIO_PUER 	0x64	// Pull-up Enable Register
#define PIO_PUSR 	0x68	// Pad Pull-up Status Register
#define PIO_ASR 	0x70	// Select A Register
#define PIO_BSR 	0x74	// Select B Register
#define PIO_ABSR 	0x78	// AB Select Status Register
#define PIO_OWER 	0xA0	// Output Write Enable Register
#define PIO_OWDR 	0xA4	// Output Write Disable Register
#define PIO_OWSR 	0xA8	// Output Write Status Register


// The AT91RM9200 GPIO's are spread across four 32-bit ports A-D.
// To make it easier to interface with them and to eliminate the need
// to track which GPIO is in which port, we	convert the Port x, Bit y
// into a single GPIO number 0 - 127.
//
// Board specific defines will assign the board level signal to a
// virutal GPIO.
//
// PORT A
#define GPIO_0		BIT0	
#define GPIO_1		BIT1	
#define GPIO_2		BIT2	
#define GPIO_3		BIT3	
#define GPIO_4		BIT4	
#define GPIO_5		BIT5	
#define GPIO_6		BIT6	
#define GPIO_7		BIT7	
#define GPIO_8		BIT8	
#define GPIO_9		BIT9	
#define GPIO_10		BIT10
#define GPIO_11		BIT11
#define GPIO_12		BIT12
#define GPIO_13		BIT13
#define GPIO_14		BIT14
#define GPIO_15		BIT15
#define GPIO_16		BIT16
#define GPIO_17		BIT17
#define GPIO_18		BIT18
#define GPIO_19		BIT19
#define GPIO_20		BIT20
#define GPIO_21		BIT21
#define GPIO_22		BIT22
#define GPIO_23		BIT23
#define GPIO_24		BIT24
#define GPIO_25		BIT25
#define GPIO_26		BIT26
#define GPIO_27		BIT27
#define GPIO_28		BIT28
#define GPIO_29		BIT29
#define GPIO_30		BIT30
#define GPIO_31		BIT31
// PORT B
#define GPIO_32		BIT0	
#define GPIO_33		BIT1	
#define GPIO_34		BIT2	
#define GPIO_35		BIT3	
#define GPIO_36		BIT4	
#define GPIO_37		BIT5	
#define GPIO_38		BIT6	
#define GPIO_39		BIT7	
#define GPIO_40		BIT8	
#define GPIO_41		BIT9	
#define GPIO_42		BIT10
#define GPIO_43		BIT11
#define GPIO_44		BIT12
#define GPIO_45		BIT13
#define GPIO_46		BIT14
#define GPIO_47		BIT15
#define GPIO_48		BIT16
#define GPIO_49		BIT17
#define GPIO_50		BIT18
#define GPIO_51		BIT19
#define GPIO_52		BIT20
#define GPIO_53		BIT21
#define GPIO_54		BIT22
#define GPIO_55		BIT23
#define GPIO_56		BIT24
#define GPIO_57		BIT25
#define GPIO_58		BIT26
#define GPIO_59		BIT27
#define GPIO_60		BIT28
#define GPIO_61		BIT29
#define GPIO_62		BIT30
#define GPIO_63		BIT31
// PORT C
#define GPIO_64		BIT0	
#define GPIO_65		BIT1	
#define GPIO_66		BIT2	
#define GPIO_67		BIT3	
#define GPIO_68		BIT4	
#define GPIO_69		BIT5	
#define GPIO_70		BIT6	
#define GPIO_71		BIT7	
#define GPIO_72		BIT8	
#define GPIO_73		BIT9	
#define GPIO_74		BIT10
#define GPIO_75		BIT11
#define GPIO_76		BIT12
#define GPIO_77		BIT13
#define GPIO_78		BIT14
#define GPIO_79		BIT15
#define GPIO_80		BIT16
#define GPIO_81		BIT17
#define GPIO_82		BIT18
#define GPIO_83		BIT19
#define GPIO_84		BIT20
#define GPIO_85		BIT21
#define GPIO_86		BIT22
#define GPIO_87		BIT23
#define GPIO_88		BIT24
#define GPIO_89		BIT25
#define GPIO_90		BIT26
#define GPIO_91		BIT27
#define GPIO_92		BIT28
#define GPIO_93		BIT29
#define GPIO_94		BIT30
#define GPIO_95		BIT31
// PORT D
#define GPIO_96		BIT0	
#define GPIO_97		BIT1	
#define GPIO_98		BIT2	
#define GPIO_99		BIT3	
#define GPIO_100	BIT4	
#define GPIO_101	BIT5	
#define GPIO_102	BIT6	
#define GPIO_103	BIT7	
#define GPIO_104	BIT8	
#define GPIO_105	BIT9	
#define GPIO_106	BIT10
#define GPIO_107	BIT11
#define GPIO_108	BIT12
#define GPIO_109	BIT13
#define GPIO_110	BIT14
#define GPIO_111	BIT15
#define GPIO_112	BIT16
#define GPIO_113	BIT17
#define GPIO_114	BIT18
#define GPIO_115	BIT19
#define GPIO_116	BIT20
#define GPIO_117	BIT21
#define GPIO_118	BIT22
#define GPIO_119	BIT23
#define GPIO_120	BIT24
#define GPIO_121	BIT25
#define GPIO_122	BIT26
#define GPIO_123	BIT27
#define GPIO_124	BIT28
#define GPIO_125	BIT29
#define GPIO_126	BIT30
#define GPIO_127	BIT31
// PORT E
#define GPIO_128	BIT0	
#define GPIO_129	BIT1	
#define GPIO_130	BIT2	
#define GPIO_131	BIT3	
#define GPIO_132	BIT4	
#define GPIO_133	BIT5	
#define GPIO_134	BIT6	
#define GPIO_135	BIT7	
#define GPIO_136	BIT8	
#define GPIO_137	BIT9	
#define GPIO_138	BIT10
#define GPIO_139	BIT11
#define GPIO_140	BIT12
#define GPIO_141	BIT13
#define GPIO_142	BIT14
#define GPIO_143	BIT15
#define GPIO_144	BIT16
#define GPIO_145	BIT17
#define GPIO_146	BIT18
#define GPIO_147	BIT19
#define GPIO_148	BIT20
#define GPIO_149	BIT21
#define GPIO_150	BIT22
#define GPIO_151	BIT23
#define GPIO_152	BIT24
#define GPIO_153	BIT25
#define GPIO_154	BIT26
#define GPIO_155	BIT27
#define GPIO_156	BIT28
#define GPIO_157	BIT29
#define GPIO_158	BIT30
#define GPIO_159	BIT31

// Most of the GPIO pins can have one of two alternate functions
// in addition to being GPIO

// Port A, Alternate Function A
#define PIOA_ASR_MC0DA0		BIT0	// MMC/SD Card 0 Slot A Data 0
#define PIOA_ASR_MC0CDA		BIT1	// MMC/SD Card 0 Slot A Command
//#define PIOA_ASR_			BIT2	// No alternate function A
#define PIOA_ASR_MC0DA1		BIT3	// MMC/SD Card 0 Slot A Data 1
#define PIOA_ASR_MC0DA2		BIT4	// MMC/SD Card 0 Slot A Data 2
#define PIOA_ASR_MC0DA3		BIT5	// MMC/SD Card 0 Slot A Data 3
#define PIOA_ASR_MC1CK		BIT6	// MMC/SD Card 1 Clock
#define PIOA_ASR_MC1CDA		BIT7	// MMC/SD Card 1 Slot A Command
#define PIOA_ASR_MC1DA0		BIT8	// MMC/SD Card 1 Slot A Data 0
#define PIOA_ASR_MC1DA1		BIT9	// MMC/SD Card 1 Slot A Data 1
#define PIOA_ASR_MC1DA2		BIT10	// MMC/SD Card 1 Slot A Data 2 
#define PIOA_ASR_MC1DA3		BIT11	// MMC/SD Card 1 Slot A Data 3
#define PIOA_ASR_MC0CK		BIT12	// MMC/SD Card 0 Clock
#define PIOA_ASR_CANTX		BIT13	// CAN Input
#define PIOA_ASR_CANRX		BIT14	// CAN Output
#define PIOA_ASR_TCLK2		BIT15	// Timer 2 Clock
#define PIOA_ASR_MC0CDB		BIT16	// MMC/SD Card 0 Slot B Command
#define PIOA_ASR_MC0DB0		BIT17	// MMC/SD Card 0 Slot B Data 0
#define PIOA_ASR_MC0DB1		BIT18	// MMC/SD Card 0 Slot B Data 1
#define PIOA_ASR_MC0DB2		BIT19	// MMC/SD Card 0 Slot B Data 2
#define PIOA_ASR_MC0DB3		BIT20	// MMC/SD Card 0 Slot B Data 3
#define PIOA_ASR_MC1CDB		BIT21	// MMC/SD Card 1 Slot B Command
#define PIOA_ASR_MC1DB0		BIT22	// MMC/SD Card 1 Slot B Data 0
#define PIOA_ASR_MC1DB1		BIT23	// MMC/SD Card 1 Slot B Data 1
#define PIOA_ASR_MC1DB2		BIT24	// MMC/SD Card 1 Slot B Data 2
#define PIOA_ASR_MC1DB3		BIT25	// MMC/SD Card 1 Slot B Data 3
#define PIOA_ASR_TXD0		BIT26	// USART 0 Transmit
#define PIOA_ASR_RXD0		BIT27	// USART 0 Receive
#define PIOA_ASR_RTS0		BIT28	// USART 0 RTS
#define PIOA_ASR_CTS0		BIT29	// USART 0 CTS
#define PIOA_ASR_SCK0		BIT30	// USART 0 Clock
#define PIOA_ASR_DMARQ0		BIT31	// Direct Memory Access (DMA) Request 0


// Port A, Alternate Function B
#define PIOA_BSR_MISO0		BIT0	// SPI0 Master In (RX), Slave out
#define PIOA_BSR_MOSI0		BIT1	// SPI0 Master Out (TX), Slave In
#define PIOA_BSR_SP0CK		BIT2	// SPI0 Clock
#define PIOA_BSR_NP0CS1		BIT3	// SPI0 Chip Select 1
#define PIOA_BSR_NP0CS2		BIT4	// SPI0 Chip Select 2
#define PIOA_BSR_NP0CS0		BIT5	// SPI0 Chip Select 0
#define PIOA_BSR_PCK2		BIT6	// Peripheral Clock 2
//#define PIOA_BSR_			BIT7	// No alternate function B
//#define PIOA_BSR_			BIT8	// No alternate function B
//#define PIOA_BSR_			BIT9	// No alternate function B
//#define PIOA_BSR_			BIT10	// No alternate function B
//#define PIOA_BSR_			BIT11	// No alternate function B
//#define PIOA_BSR_			BIT12	// No alternate function B
#define PIOA_BSR_PCK0		BIT13	// Peripheral Clock 0
#define PIOA_BSR_IRQ0		BIT14	// IRQ0
#define PIOA_BSR_IRQ1		BIT15	// IRQ1
#define PIOA_BSR_EB1D16		BIT16	// EBI1 - Databus 1 Bit 16
#define PIOA_BSR_EB1D17		BIT17	// EBI1 - Databus 1 Bit 17
#define PIOA_BSR_EB1D18		BIT18	// EBI1 - Databus 1 Bit 18
#define PIOA_BSR_EB1D19		BIT19	// EBI1 - Databus 1 Bit 19
#define PIOA_BSR_EB1D20		BIT20	// EBI1 - Databus 1 Bit 20
#define PIOA_BSR_EB1D21		BIT21	// EBI1 - Databus 1 Bit 21
#define PIOA_BSR_EB1D22		BIT22	// EBI1 - Databus 1 Bit 22
#define PIOA_BSR_EB1D23		BIT23	// EBI1 - Databus 1 Bit 23
#define PIOA_BSR_EB1D24		BIT24	// EBI1 - Databus 1 Bit 24
#define PIOA_BSR_EB1D25		BIT25	// EBI1 - Databus 1 Bit 25
#define PIOA_BSR_EB1D26		BIT26	// EBI1 - Databus 1 Bit 26
#define PIOA_BSR_EB1D27		BIT27	// EBI1 - Databus 1 Bit 27
#define PIOA_BSR_EB1D28		BIT28	// EBI1 - Databus 1 Bit 28
#define PIOA_BSR_EB1D29		BIT29	// EBI1 - Databus 1 Bit 29
#define PIOA_BSR_EB1D30		BIT30	// EBI1 - Databus 1 Bit 30
#define PIOA_BSR_EB1D31		BIT31	// EBI1 - Databus 1 Bit 31


// Port B, Function A
#define PIOB_ASR_AC97FS 	BIT0	// AC'97 Frame Synchronization 
#define PIOB_ASR_AC97CK 	BIT1	// AC'97 Clock 
#define PIOB_ASR_AC97TX 	BIT2	// AC'97 Transmit Data
#define PIOB_ASR_AC97RX 	BIT3	// AC'97 Receive Data
#define PIOB_ASR_TWD 		BIT4	// Two-Wire (I2C) Data
#define PIOB_ASR_TWCK 		BIT5	// Two-Wire (I2C) Clock
#define PIOB_ASR_TF1 		BIT6	// I2S 1 Transmit Frame 
#define PIOB_ASR_TK1 		BIT7	// I2S 1 Transmit Clock 
#define PIOB_ASR_TD1 		BIT8	// I2S 1 Transmit Data 
#define PIOB_ASR_RD1 		BIT9	// I2S 1 Receive Data 
#define PIOB_ASR_RK1 		BIT10	// I2S 1 Receive Clock 
#define PIOB_ASR_RF1 		BIT11	// I2S 1 Receive Frame 
#define PIOB_ASR_MISO1 		BIT12	// SPI1 Master In (RX), Slave out 
#define PIOB_ASR_MOSI1 		BIT13	// SPI1 Master Out (TX), Slave In 
#define PIOB_ASR_SP1CK 		BIT14	// SPI1 Clock 
#define PIOB_ASR_NP1CS0		BIT15	// SPI1 Chip Select 0
#define PIOB_ASR_NP1CS1		BIT16	// SPI1 Chip Select 1 
#define PIOB_ASR_NP1CS2		BIT17	// SPI1 Chip Select 2 
#define PIOB_ASR_NP1CS3		BIT18	// SPI1 Chip Select 3
//#define PIOB_ASR_	 		BIT19	// No alternate function A
//#define PIOB_ASR_			BIT20	// No alternate function A
//#define PIOB_ASR_			BIT21	// No alternate function A
//#define PIOB_ASR_			BIT22	// No alternate function A
//#define PIOB_ASR_			BIT23	// No alternate function A
//#define PIOB_ASR_			BIT24	// No alternate function A
//#define PIOB_ASR_	 		BIT25	// No alternate function A
//#define PIOB_ASR_			BIT26	// No alternate function A
//#define PIOB_ASR_			BIT27	// No alternate function A 
//#define PIOB_ASR_			BIT28	// No alternate function A
//#define PIOB_ASR_			BIT29	// No alternate function A
//#define PIOB_ASR_			BIT30	// No alternate function A
//#define PIOB_ASR_			BIT31	// No alternate function A


// Port B, Function B
#define PIOB_BSR_TF0		BIT0	// I2S 0 Transmit Frame
#define PIOB_BSR_TK0		BIT1	// I2S 0 Transmit Clock
#define PIOB_BSR_TD0		BIT2	// I2S 0 Transmit Data
#define PIOB_BSR_RD0		BIT3	// I2S 0 Receive Data
#define PIOB_BSR_RK0		BIT4	// I2S 0 Receive Clock
#define PIOB_BSR_RF0		BIT5	// I2S 0 Receive Frame
#define PIOB_BSR_DMARQ1		BIT6	// Direct Memory Access (DMA) Request 1
#define PIOB_BSR_PWM0		BIT7	// Pulse Width Modulation 0
#define PIOB_BSR_PWM1		BIT8	// Pulse Width Modulation 1
#define PIOB_BSR_LCDCC		BIT9	// LCD Contrast Control
//#define PIOB_BSR_PCK1		BIT10	// Peripheral Clock 1 ***DUPLICATED at BIT16 ???
#define PIOB_BSR_NP0CS3		BIT11	// SPI0 Chip Select 3
//#define PIOB_BSR_			BIT12	// No alternate function B
//#define PIOB_BSR_			BIT13	// No alternate function B
//#define PIOB_BSR_			BIT14	// No alternate function B
//#define PIOB_BSR_			BIT15	// No alternate function B
#define PIOB_BSR_PCK1		BIT16	// Peripheral Clock 1
#define PIOB_BSR_TIOA2		BIT17	// Timer 2 IO A
#define PIOB_BSR_TIOB2		BIT18	// Timer 2 IO B
//#define PIOB_BSR_			BIT19	// No alternate function B
//#define PIOB_BSR_			BIT20	// No alternate function B
//#define PIOB_BSR_			BIT21	// No alternate function B
//#define PIOB_BSR_			BIT22	// No alternate function B
//#define PIOB_BSR_			BIT23	// No alternate function B
#define PIOB_BSR_DMARQ3		BIT24	// Direct Memory Access (DMA) Request 3
//#define PIOB_BSR_			BIT25	// No alternate function B
//#define PIOB_BSR_			BIT26	// No alternate function B
#define PIOB_BSR_PWM2		BIT27	// Pulse Width Modulation 2
#define PIOB_BSR_TCLK0		BIT28	// Timer 0 Clock 
#define PIOB_BSR_PWM3		BIT29	// Pulse Width Modulation 3
//#define PIOB_BSR_			BIT30	// No alternate function B
//#define PIOB_BSR_			BIT31	// No alternate function B


// Port C, Alternate Function A
#define PIOC_ASR_VSYNC		BIT0	// LCD Vertical Sync
#define PIOC_ASR_HSYNC		BIT1	// LCD Horizontal Sync
#define PIOC_ASR_LCDCK		BIT2	// LCD Dot Clock
#define PIOC_ASR_LCDEN		BIT3	// LCD Data Enable
#define PIOC_ASR_LCDD0		BIT4	// LCD Data Bit 0
#define PIOC_ASR_LCDD1		BIT5	// LCD Data Bit 1
#define PIOC_ASR_LCDD2		BIT6	// LCD Data Bit 2
#define PIOC_ASR_LCDD3		BIT7	// LCD Data Bit 3
#define PIOC_ASR_LCDD4		BIT8	// LCD Data Bit 4
#define PIOC_ASR_LCDD5		BIT9	// LCD Data Bit 5
#define PIOC_ASR_LCDD6		BIT10	// LCD Data Bit 6
#define PIOC_ASR_LCDD7		BIT11	// LCD Data Bit 7
#define PIOC_ASR_LCDD8		BIT12	// LCD Data Bit 8
#define PIOC_ASR_LCDD9		BIT13	// LCD Data Bit 9
#define PIOC_ASR_LCDD10		BIT14	// LCD Data Bit 10
#define PIOC_ASR_LCDD11		BIT15	// LCD Data Bit 11
#define PIOC_ASR_LCDD12		BIT16	// LCD Data Bit 12
#define PIOC_ASR_LCDD13		BIT17	// LCD Data Bit 13
#define PIOC_ASR_LCDD14		BIT18	// LCD Data Bit 14
#define PIOC_ASR_LCDD15		BIT19	// LCD Data Bit 15
#define PIOC_ASR_LCDD16		BIT20	// LCD Data Bit 16
#define PIOC_ASR_LCDD17		BIT21	// LCD Data Bit 17
#define PIOC_ASR_LCDD18		BIT22	// LCD Data Bit 18
#define PIOC_ASR_LCDD19		BIT23	// LCD Data Bit 19
#define PIOC_ASR_LCDD20		BIT24	// LCD Data Bit 20
#define PIOC_ASR_LCDD21		BIT25	// LCD Data Bit 21
#define PIOC_ASR_LCDD22		BIT26	// LCD Data Bit 22
#define PIOC_ASR_LCDD23		BIT27	// LCD Data Bit 23
#define PIOC_ASR_PWM0		BIT28	// Pulse Width Modulation 0
#define PIOC_ASR_PCK0		BIT29	// Peripheral Clock 0
#define PIOC_ASR_DRXD		BIT30	// Debug UART Receive
#define PIOC_ASR_DTXD		BIT31	// Debug UART Transmit


// Port C, Alternate Function B
//#define PIOC_BSR_			BIT0	// No alternate function B
//#define PIOC_BSR_			BIT1	// No alternate function B
//#define PIOC_BSR_			BIT2	// No alternate function B
#define PIOC_BSR_PWM1		BIT3	// Pulse Width Modulation 1
#define PIOC_BSR_LCDD3		BIT4	// LCD Data Bit 3
#define PIOC_BSR_LCDD4		BIT5	// LCD Data Bit 4
#define PIOC_BSR_LCDD5		BIT6	// LCD Data Bit 5
#define PIOC_BSR_LCDD6		BIT7	// LCD Data Bit 6
#define PIOC_BSR_LCDD7		BIT8	// LCD Data Bit 7
#define PIOC_BSR_LCDD10		BIT9	// LCD Data Bit 10
#define PIOC_BSR_LCDD11		BIT10	// LCD Data Bit 11
#define PIOC_BSR_LCDD12		BIT11	// LCD Data Bit 12
#define PIOC_BSR_LCDD13		BIT12	// LCD Data Bit 13
#define PIOC_BSR_LCDD14		BIT13	// LCD Data Bit 14
#define PIOC_BSR_LCDD15		BIT14	// LCD Data Bit 15
#define PIOC_BSR_LCDD19		BIT15	// LCD Data Bit 19
#define PIOC_BSR_LCDD20		BIT16	// LCD Data Bit 20
#define PIOC_BSR_LCDD21		BIT17	// LCD Data Bit 21
#define PIOC_BSR_LCDD22		BIT18	// LCD Data Bit 22
#define PIOC_BSR_LCDD23		BIT19	// LCD Data Bit 23
#define PIOC_BSR_ETX2		BIT20	// EMAC TXD2
#define PIOC_BSR_ETX3		BIT21	// EMAC TXD3
#define PIOC_BSR_ERX2		BIT22	// EMAC RXD2
#define PIOC_BSR_ERX3		BIT23	// EMAC RXD3
#define PIOC_BSR_ETXER		BIT24	// EMAC TXER
#define PIOC_BSR_ERXDV		BIT25	// EMAC RXDV
#define PIOC_BSR_ECOL		BIT26	// EMAC COL
#define PIOC_BSR_ERXCK		BIT27	// EMAC RX Clock
#define PIOC_BSR_TCLK1		BIT28	// Timer 1 Clock
#define PIOC_BSR_PWM2		BIT29	// Pulse Width Modulation 2
//#define PIOC_BSR_		BIT30	// No alternate function B
//#define PIOC_BSR_		BIT31	// No alternate function B


// Port D, Alternate Function A
#define PIOD_ASR_TXD1		BIT0	// USART 1 Transmit
#define PIOD_ASR_RXD1		BIT1	// USART 1 Receive
#define PIOD_ASR_TXD2		BIT2	// USART 2 Transmit
#define PIOD_ASR_RXD2		BIT3	// USART 2 Receive
#define PIOD_ASR_FIQ 		BIT4	// AIC - Fast Interrupt Input
#define PIOD_ASR_N0WAIT		BIT5	// EBI0 - WAIT Input
#define PIOD_ASR_N0CS4		BIT6	// EBI0 - CS4 or Compact Flash CS0
#define PIOD_ASR_N0CS5 		BIT7	// EBI0 - CS5 or Compact Flash CS1
#define PIOD_ASR_CFCE1 		BIT8	// EBI0 - Compact Flash CE1
#define PIOD_ASR_CFCE2 		BIT9	// EBI0 - Compact Flash CE2
//#define PIOD_ASR_	 		BIT10	// No alternate function A
#define PIOD_ASR_N0CS2 		BIT11	// EBI0 - CS2
#define PIOD_ASR_A23 		BIT12	// EBI0 - A23
#define PIOD_ASR_A24 		BIT13	// EBI0 - A24
#define PIOD_ASR_A25 		BIT14	// EBI0 - A25 or Compact Flash R/W
#define PIOD_ASR_N0CS3		BIT15	// EBI0 - CS3 or NAND Flash Chip Select
#define PIOD_ASR_EB0D16		BIT16	// EBI0 - Databus 0 Bit 16 
#define PIOD_ASR_EB0D17		BIT17	// EBI0 - Databus 0 Bit 17
#define PIOD_ASR_EB0D18		BIT18	// EBI0 - Databus 0 Bit 18
#define PIOD_ASR_EB0D19		BIT19	// EBI0 - Databus 0 Bit 19
#define PIOD_ASR_EB0D20		BIT20	// EBI0 - Databus 0 Bit 20
#define PIOD_ASR_EB0D21		BIT21	// EBI0 - Databus 0 Bit 21
#define PIOD_ASR_EB0D22		BIT22	// EBI0 - Databus 0 Bit 22
#define PIOD_ASR_EB0D23		BIT23	// EBI0 - Databus 0 Bit 23
#define PIOD_ASR_EB0D24		BIT24	// EBI0 - Databus 0 Bit 24
#define PIOD_ASR_EB0D25		BIT25	// EBI0 - Databus 0 Bit 25
#define PIOD_ASR_EB0D26		BIT26	// EBI0 - Databus 0 Bit 26
#define PIOD_ASR_EB0D27		BIT27	// EBI0 - Databus 0 Bit 27
#define PIOD_ASR_EB0D28		BIT28	// EBI0 - Databus 0 Bit 28
#define PIOD_ASR_EB0D29		BIT29	// EBI0 - Databus 0 Bit 29
#define PIOD_ASR_EB0D30		BIT30	// EBI0 - Databus 0 Bit 30
#define PIOD_ASR_EB0D31		BIT31	// EBI0 - Databus 0 Bit 31


// Port D, Alternate Function B
#define PIOD_BSR_NP0CS2		BIT0	// SPI0 Chip Select 2	
#define PIOD_BSR_NP0CS3		BIT1	// SPI0 Chip Select 3	
#define PIOD_BSR_NP1CS2		BIT2	// SPI1 Chip Select 2	
#define PIOD_BSR_NP1CS3		BIT3	// SPI1 Chip Select 3	
#define PIOD_BSR_DMARQ2		BIT4	// Direct Memory Access (DMA) Request 2	
#define PIOD_BSR_RTS2		BIT5	// USART 2 RTS	
#define PIOD_BSR_CTS2		BIT6	// USART 2 CTS	
#define PIOD_BSR_RTS1		BIT7	// USART 1 RTS	
#define PIOD_BSR_CTS1		BIT8	// USART 1 CTS	
#define PIOD_BSR_SCK2		BIT9	// USART 2 Clock	
#define PIOD_BSR_SCK1		BIT10	// USART 1 Clock	
#define PIOD_BSR_TSYNC		BIT11	// ETM Sync	
#define PIOD_BSR_TCLK		BIT12	// ETM Clock
#define PIOD_BSR_TPS0		BIT13	// ETM Processor Status 0
#define PIOD_BSR_TPS1		BIT14	// ETM Processor Status 1
#define PIOD_BSR_TPS2		BIT15	// ETM Processor Status 2
#define PIOD_BSR_TPK0		BIT16	// ETM Packet Data 0
#define PIOD_BSR_TPK1		BIT17	// ETM Packet Data 1
#define PIOD_BSR_TPK2		BIT18	// ETM Packet Data 2
#define PIOD_BSR_TPK3		BIT19	// ETM Packet Data 3
#define PIOD_BSR_TPK4		BIT20	// ETM Packet Data 4
#define PIOD_BSR_TPK5		BIT21	// ETM Packet Data 5
#define PIOD_BSR_TPK6		BIT22	// ETM Packet Data 6
#define PIOD_BSR_TPK7		BIT23	// ETM Packet Data 7
#define PIOD_BSR_TPK8		BIT24	// ETM Packet Data 8
#define PIOD_BSR_TPK9		BIT25	// ETM Packet Data 9
#define PIOD_BSR_TPK10		BIT26	// ETM Packet Data 10
#define PIOD_BSR_TPK11		BIT27	// ETM Packet Data 11
#define PIOD_BSR_TPK12		BIT28	// ETM Packet Data 12
#define PIOD_BSR_TPK13		BIT29	// ETM Packet Data 13
#define PIOD_BSR_TPK14		BIT30	// ETM Packet Data 14
#define PIOD_BSR_TPK15		BIT31	// ETM Packet Data 15


// Port E, Alternate Function A
#define PIOE_ASR_ISID0		BIT0	// Image Sensor Data 0
#define PIOE_ASR_ISID1		BIT1	// Image Sensor Data 1
#define PIOE_ASR_ISID2		BIT2	// Image Sensor Data 2
#define PIOE_ASR_ISID3		BIT3	// Image Sensor Data 3
#define PIOE_ASR_ISID4 		BIT4	// Image Sensor Data 4
#define PIOE_ASR_ISID5 		BIT5	// Image Sensor Data 5
#define PIOE_ASR_ISID6		BIT6	// Image Sensor Data 6
#define PIOE_ASR_ISID7 		BIT7	// Image Sensor Data 7
#define PIOE_ASR_ISIPCK		BIT8	// Image Sensor Data Clock
#define PIOE_ASR_ISIHSYNC	BIT9	// Image Sensor	Horizontal Sync
#define PIOE_ASR_ISIVSYNC	BIT10	// Image Sensor	Vertical Sync
#define PIOE_ASR_ISIMCK		BIT11	// Image Sensor	Reference Clock
//#define PIOE_ASR_	 		BIT12	// No alternate function A
//#define PIOE_ASR_	 		BIT13	// No alternate function A
//#define PIOE_ASR_	 		BIT14	// No alternate function A
//#define PIOE_ASR_			BIT15	// No alternate function At
//#define PIOE_ASR_			BIT16	// No alternate function A 
//#define PIOE_ASR_			BIT17	// No alternate function A
//#define PIOE_ASR_			BIT18	// No alternate function A
//#define PIOE_ASR_			BIT19	// No alternate function A
//#define PIOE_ASR_			BIT20	// No alternate function A
#define PIOE_ASR_ETXCK		BIT21	// EMAC TX Clock
#define PIOE_ASR_ECRS		BIT22	// EMAC CRS
#define PIOE_ASR_ETX0		BIT23	// EMAC TXD0
#define PIOE_ASR_ETX1		BIT24	// EMAC TXD1
#define PIOE_ASR_ERX0		BIT25	// EMAC RXD0
#define PIOE_ASR_ERX1		BIT26	// EMAC RXD1
#define PIOE_ASR_ERXER		BIT27	// EMAC RXER
#define PIOE_ASR_ETXEN		BIT28	// EMAC TXEN
#define PIOE_ASR_EMDC		BIT29	// EMAC MDC
#define PIOE_ASR_EMDIO		BIT30	// EMAC MDIO
#define PIOE_ASR_EF100		BIT31	// EMAC Speed 100 (RMII Only)


// Port E, Alternate Function B
//#define PIOE_BSR_			BIT0	// No alternate function B	
//#define PIOE_BSR_			BIT1	// No alternate function B	
//#define PIOE_BSR_			BIT2	// No alternate function B	
//#define PIOE_BSR_			BIT3	// No alternate function B	
//#define PIOE_BSR_			BIT4	// No alternate function B	
//#define PIOE_BSR_			BIT5	// No alternate function B	
//#define PIOE_BSR_			BIT6	// No alternate function B	
//#define PIOE_BSR_			BIT7	// No alternate function B	
#define PIOE_BSR_TIOA1		BIT8	// Timer 1 IO A	
#define PIOE_BSR_TIOB1		BIT9	// Timer 1 IO B	
#define PIOE_BSR_PWM3		BIT10	// Pulse Width Modulation 3	
#define PIOE_BSR_PCK3		BIT11	// Peripheral Clock 3	
#define PIOE_BSR_ISID8		BIT12	// Image Sensor Data 8
#define PIOE_BSR_ISID9		BIT13	// Image Sensor Data 9
#define PIOE_BSR_ISID10		BIT14	// Image Sensor Data 10
#define PIOE_BSR_ISID11		BIT15	// Image Sensor Data 11
//#define PIOE_BSR_			BIT16	// No alternate function B
//#define PIOE_BSR_			BIT17	// No alternate function B
#define PIOE_BSR_TIOA0		BIT18	// Timer 0 IO A
#define PIOE_BSR_TIOB0		BIT19	// Timer 0 IO B
#define PIOE_BSR_N1WAIT		BIT20	// EBI1 - WAIT Input
#define PIOE_BSR_NANDWE		BIT21	// EBI1 - NAND Flash Write Enable
#define PIOE_BSR_N1CS2		BIT22	// EBI1 - CS2 or NAND Flash Chip Select
#define PIOE_BSR_NANDOE		BIT23	// EBI1 - NAND Output Enable
#define PIOE_BSR_N1WR3		BIT24	// EBI1 - Static Memory Write Signal or	Mask Signal
#define PIOE_BSR_N1CS1		BIT25	// EBI1 - CS1 or SDRAM Chip Select
//#define PIOE_BSR_			BIT26	// No alternate function B
#define PIOE_BSR_SDCKE		BIT27	// EBI1 - SDRAM Clock Enable
#define PIOE_BSR_RAS		BIT28	// EBI1 - SDRAM RAS
#define PIOE_BSR_CAS		BIT29	// EBI1 - SDRAM CAS
#define PIOE_BSR_SDWE		BIT30	// EBI1 - SDRAM Write Enable
#define PIOE_BSR_SDA10		BIT31	// EBI1 - SDRAM Address 10 Line

#endif

