/* Au1100 CP0 registers
 */
#define CP0_Index		$0
#define CP0_Random		$1
#define CP0_EntryLo0	$2
#define CP0_EntryLo1	$3
#define CP0_Context		$4
#define CP0_PageMask	$5
#define CP0_Wired		$6
#define CP0_BadVAddr	$8
#define CP0_Count		$9
#define CP0_EntryHi		$10
#define CP0_Compare		$11
#define CP0_Status		$12
#define CP0_Cause		$13
#define CP0_EPC			$14
#define CP0_PRId		$15
#define CP0_Config		$16
#define CP0_Config0		$16
#define CP0_Config1		$16,1
#define CP0_LLAddr		$17
#define CP0_WatchLo		$18
#define CP0_IWatchLo	$18,1
#define CP0_WatchHi		$19
#define CP0_IWatchHi	$19,1
#define CP0_Scratch		$22
#define CP0_Debug		$23
#define CP0_DEPC		$24
#define CP0_PerfCnt		$25
#define CP0_PerfCtrl	$25,1
#define CP0_DTag		$28
#define CP0_DData		$28,1
#define CP0_ITag		$29
#define CP0_IData		$29,1
#define CP0_ErrorEPC	$30
#define CP0_DESave		$31

/* Au1100 base addresses (in KSEG1 region)
 */
#define AU1100_MEM_ADDR		0xB4000000

#define AU1100_AC97_ADDR	0xB0000000
#define AU1100_USBH_ADDR	0xB0100000
#define AU1100_USBD_ADDR	0xB0200000
#define AU1100_MACEN_ADDR	0xB1520000
#define AU1100_UART0_ADDR	0xB1100000
#define AU1100_UART3_ADDR	0xB1400000
#define AU1100_SYS_ADDR		0xB1900000
#define AU1100_GPIO2_ADDR	0xB1700000

#define AU1100_SYS_REG(_x_)	*(vulong *)(AU1100_SYS_ADDR + _x_)

/* Au1100 gpio2 register offsets
 */
#define gpio2_dir		0x0000
#define gpio2_output	0x0008
#define gpio2_pinstate	0x000c
#define gpio2_inten		0x0010
#define gpio2_enable	0x0014

/* Au1100 memory controller register offsets
 */
#define mem_sdmode0		0x0000
#define mem_sdmode1		0x0004
#define mem_sdmode2		0x0008
#define mem_sdaddr0		0x000C
#define mem_sdaddr1		0x0010
#define mem_sdaddr2		0x0014
#define mem_sdrefcfg	0x0018
#define mem_sdprecmd	0x001C
#define mem_sdautoref	0x0020
#define mem_sdwrmd0		0x0024
#define mem_sdwrmd1		0x0028
#define mem_sdwrmd2		0x002C
#define mem_sdsleep		0x0030
#define mem_sdsmcke		0x0034

#define mem_stcfg0		0x1000
#define mem_sttime0		0x1004
#define mem_staddr0		0x1008
#define mem_stcfg1		0x1010
#define mem_sttime1		0x1014
#define mem_staddr1		0x1018
#define mem_stcfg2		0x1020
#define mem_sttime2		0x1024
#define mem_staddr2		0x1028
#define mem_stcfg3		0x1030
#define mem_sttime3		0x1034
#define mem_staddr3		0x1038

/*
 * Au1100 peripheral register offsets
 */
#define ac97_enable		0x0010
#define usbh_enable		0x0007FFFC
#define usbd_enable		0x0058
#define irda_enable		0x0040
#define macen_mac0		0x0000
#define i2s_enable		0x0008
#define uart_enable		0x0100
#define ssi_enable		0x0100

#define sys_scratch0	0x0018
#define sys_scratch1	0x001c
#define sys_cntctrl		0x0014
#define sys_freqctrl0	0x0020
#define sys_freqctrl1	0x0024
#define sys_clksrc		0x0028
#define sys_pinfunc		0x002C
#define sys_powerctrl	0x003C
#define sys_endian		0x0038
#define sys_wakesrc		0x005C
#define sys_cpupll		0x0060
#define sys_auxpll		0x0064
#define sys_pininputen	0x0110

/*
 * Board-specific values
 * NOTE: All values are for operation at 396MHz, SD=2
 */

#define SYS_CPUPLL		33	/* 396MHz */
#define SYS_POWERCTRL	0	/* SD=2   */
#define SYS_AUXPLL		8	/* 96MHz  */

/* RCE0: 32MB StrataFlash */
#define MEM_STCFG0	0x00000003
#define MEM_STTIME0	0x22080b20
#define MEM_STADDR0	0x11c03f00

/* RCE1: */
#define MEM_STCFG1	0x00000080
#define MEM_STTIME1	0x22080a20
#define MEM_STADDR1	0x10c03f00

/* RCE2: */
#define MEM_STCFG2	0x000000C4
#define MEM_STTIME2	0x22080a20
#define MEM_STADDR2	0x11803f00

/* RCE3: */
#define MEM_STCFG3	0x00000002
#define MEM_STTIME3	0x280E3E07
#define MEM_STADDR3	0x10000000

/*
 * SDCS0 - Not used, for SMROM
 * SDCS1 - 32MB Micron 48LC8M16A2
 */
#define MEM_SDMODE0		0x00552229
#define MEM_SDMODE1		0x00552229
#define MEM_SDMODE2		0x00552229

#define MEM_SDADDR0		0x001003F8 
#define MEM_SDADDR1		0x001023F8
#define MEM_SDADDR2		0x001043F8

#define MEM_SDREFCFG_D	0x74000c30	/* disable */
#define MEM_SDREFCFG_E	0x76000c30	/* enable */
#define MEM_SDWRMD0		0x00000023
#define MEM_SDWRMD1		0x00000023
#define MEM_SDWRMD2		0x00000023

#define MEM_1MS			((396000000/1000000) * 1000)

// Clock Source Bits - Only LCD defined for now
#define SYS_CLKSRC_ML(_x_)	((_x_ & 0x7) << 7)	// mux select for LCD
#define SYS_CLKSRC_DL	0x00000040		// 1 = divide by 2, 0 = divide by 4
#define SYS_CLKSRC_CL	0x00000020		// 1 = use DL, 0 use mux directly
