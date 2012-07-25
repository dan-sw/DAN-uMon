/*
All files except if stated otherwise in the begining of the file are under the ISC license:
-----------------------------------------------------------------------------------

Copyright (c) 2010-2012 Design Art Networks Ltd.

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

-----------------------------------------------------------------------------------
*/
/**********************************************************************\
 Library    :  uMON (Micro Monitor)
 Filename   :  CGEN.h
 Purpose    :  ClockGen unit (clock and reset generator) h-file
 Owner		:  Igor Lebedev
 Note		:  Based on the original CGEN_api.h & cgen_reg.h
\**********************************************************************/

#ifndef _CGEN_H
#define _CGEN_H

#define CGEN_REASON_REG_DEFAULT_VAL       0x0
#define CGEN_REASON_REG_SUCCESSFUL_BOOT   0x8000
/*
 * -----------------------------------------------------------
 * Read/Write MACRO (define) section
 * -----------------------------------------------------------
 */

#define    outb(x,y)     (*((char *)(x)) = y)
#define    outw(x,y)     (*((short*)(x)) = y)
#define    outl(x,y)     (*((long *)(x)) = y)

#define    inb(x)        (*((char *)(x)))
#define    inw(x)        (*((short*)(x)))
#define    inl(x)        (*((long *)(x)))

#define    outvb(x,y)    (*((volatile char *)(x)) = y)
#define    outvw(x,y)    (*((volatile short*)(x)) = y)
#define    outvl(x,y)    (*((volatile long *)(x)) = y)

#define    invb(x)       (*((volatile char *)(x)))
#define    invw(x)       (*((volatile short*)(x)))
#define    invl(x)       (*((volatile long *)(x)))


/*
 * -----------------------------------------------------------
 * Definitions from "cgen_reg.h"
 * -----------------------------------------------------------
 */

#define clkgen_if_MODULE_ADDR 						0xe573E000

#define clkgen_if_cg_main_rst_OFFSET  				(0x0)
#define clkgen_if_cg_gp_OFFSET  					(0x4)
#define clkgen_if_cg_rst_reason_OFFSET  			(0x8)
#define clkgen_if_cg_dsp_runstall_OFFSET  			(0xc)
#define clkgen_if_cg_cpu_runstall_OFFSET  			(0x10)
#define clkgen_if_cg_pll0_cfg_OFFSET  				(0x14)
#define clkgen_if_cg_pll1_cfg_OFFSET  				(0x18)
#define clkgen_if_cg_pll2_cfg_OFFSET  				(0x1c)
#define clkgen_if_cg_pll3_cfg_OFFSET  				(0x20)
#define clkgen_if_cg_clocks_cfg_OFFSET  			(0x24)
#define clkgen_if_cg_ddr_bypass_OFFSET  			(0x28)
#define clkgen_if_cg_clkout_div_OFFSET  			(0x2c)
#define clkgen_if_cg_spare0_OFFSET  				(0x30)
#define clkgen_if_cg_spare1_OFFSET  				(0x34)
#define clkgen_if_cg_spare2_OFFSET  				(0x38)
#define clkgen_if_cg_spare3_OFFSET  				(0x3c)
#define clkgen_if_cg_bootid_OFFSET  				(0x40)
#define clkgen_if_cg_arm_target_OFFSET  			(0x44)
#define clkgen_if_cg_fe_tx_clocks_OFFSET  			(0x48)
#define clkgen_if_cg_fe_rx03_clocks_OFFSET  		(0x4c)
#define clkgen_if_cg_fe_rx4_clocks_OFFSET  			(0x50)
#define clkgen_if_cg_arm_rst_OFFSET  				(0x54)
#define clkgen_if_cg_cpu_rst_OFFSET  				(0x58)
#define clkgen_if_cg_dsp_rst_OFFSET  				(0x5c)
#define clkgen_if_cg_serdes0_div_OFFSET  			(0x60)
#define clkgen_if_cg_serdes1_div_OFFSET  			(0x64)
#define clkgen_if_cg_serdes2_div_OFFSET  			(0x68)
#define clkgen_if_cg_serdes3_div_OFFSET  			(0x6c)
#define clkgen_if_cg_haltonrst_OFFSET  				(0x70)
#define clkgen_if_cg_acc_div_OFFSET  				(0x74)
#define clkgen_if_cg_enc_div_OFFSET  				(0x78)
#define clkgen_if_cg_strap_OFFSET  					(0x7c)
#define clkgen_if_cg_fuse0_OFFSET  					(0x80)
#define clkgen_if_cg_fuse1_OFFSET  					(0x84)
#define clkgen_if_cg_fuse2_OFFSET  					(0x88)
#define clkgen_if_cg_fuse3_OFFSET  					(0x8c)
#define clkgen_if_cg_fuse4_OFFSET  					(0x90)
#define clkgen_if_cg_fuse5_OFFSET  					(0x94)
#define clkgen_if_cg_fuse6_OFFSET  					(0x98)
#define clkgen_if_cg_fuse7_OFFSET  					(0x9c)
#define clkgen_if_cg_gmac0_div_OFFSET  				(0xa0)
#define clkgen_if_cg_gmac1_div_OFFSET  				(0xa4)
#define clkgen_if_tx_enc0_cen_OFFSET  				(0x100)
#define clkgen_if_tx_enc0_ren_OFFSET  				(0x104)
#define clkgen_if_tx_enc1_cen_OFFSET  				(0x108)
#define clkgen_if_tx_enc1_ren_OFFSET  				(0x10C)
#define clkgen_if_tx_enc2_cen_OFFSET  				(0x110)
#define clkgen_if_tx_enc2_ren_OFFSET  				(0x114)
#define clkgen_if_tx_enc3_cen_OFFSET  				(0x118)
#define clkgen_if_tx_enc3_ren_OFFSET  				(0x11C)
#define clkgen_if_tx_enc4_cen_OFFSET  				(0x120)
#define clkgen_if_tx_enc4_ren_OFFSET  				(0x124)
#define clkgen_if_tx_enc5_cen_OFFSET  				(0x128)
#define clkgen_if_tx_enc5_ren_OFFSET  				(0x12C)
#define clkgen_if_rx_dec0_cen_OFFSET  				(0x130)
#define clkgen_if_rx_dec0_ren_OFFSET  				(0x134)
#define clkgen_if_rx_dec1_cen_OFFSET  				(0x138)
#define clkgen_if_rx_dec1_ren_OFFSET  				(0x13C)
#define clkgen_if_rx_dec2_cen_OFFSET  				(0x140)
#define clkgen_if_rx_dec2_ren_OFFSET  				(0x144)
#define clkgen_if_rx_dec3_cen_OFFSET  				(0x148)
#define clkgen_if_rx_dec3_ren_OFFSET  				(0x14C)
#define clkgen_if_rx_dec4_cen_OFFSET  				(0x150) 
#define clkgen_if_rx_dec4_ren_OFFSET  				(0x154)
#define clkgen_if_rx_dec5_cen_OFFSET  				(0x158)
#define clkgen_if_rx_dec5_ren_OFFSET  				(0x15C)
#define clkgen_if_tx_fft0_cen_OFFSET  				(0x160)
#define clkgen_if_tx_fft0_ren_OFFSET  				(0x164)
#define clkgen_if_tx_fft1_cen_OFFSET  				(0x168)
#define clkgen_if_tx_fft1_ren_OFFSET  				(0x16C)
#define clkgen_if_rx_fft0_cen_OFFSET  				(0x170)
#define clkgen_if_rx_fft0_ren_OFFSET  				(0x174)
#define clkgen_if_rx_fft1_cen_OFFSET  				(0x178)
#define clkgen_if_rx_fft1_ren_OFFSET  				(0x17C)
#define clkgen_if_rx_fft2_cen_OFFSET  				(0x180)
#define clkgen_if_rx_fft2_ren_OFFSET  				(0x184)
#define clkgen_if_rx_fft3_cen_OFFSET  				(0x188)
#define clkgen_if_rx_fft3_ren_OFFSET  				(0x18C)
#define clkgen_if_rx_fft4_cen_OFFSET  				(0x190)
#define clkgen_if_rx_fft4_ren_OFFSET  				(0x194)
#define clkgen_if_rx_fft5_cen_OFFSET  				(0x198)
#define clkgen_if_rx_fft5_ren_OFFSET  				(0x19C)
#define clkgen_if_tx_cb_cen_OFFSET  				(0x1A0)
#define clkgen_if_tx_cb_ren_OFFSET  				(0x1A4)
#define clkgen_if_rx_cb_cen_OFFSET  				(0x1A8)
#define clkgen_if_rx_cb_ren_OFFSET  				(0x1AC)
#define clkgen_if_npu_cb_cen_OFFSET  				(0x1B0)
#define clkgen_if_npu_cb_ren_OFFSET  				(0x1B4)
#define clkgen_if_tx_gpdma_cen_OFFSET  				(0x1B8)
#define clkgen_if_tx_gpdma_ren_OFFSET  				(0x1BC)
#define clkgen_if_rx_gpdma_cen_OFFSET  				(0x1C0)
#define clkgen_if_rx_gpdma_ren_OFFSET  				(0x1C4)
#define clkgen_if_npu_gpdma0_cen_OFFSET  			(0x1C8)
#define clkgen_if_npu_gpdma0_ren_OFFSET  			(0x1CC)
#define clkgen_if_npu_gpdma1_cen_OFFSET  			(0x1D0)
#define clkgen_if_npu_gpdma1_ren_OFFSET  			(0x1D4)
#define clkgen_if_tx_top_pm_cen_OFFSET  			(0x1D8)
#define clkgen_if_tx_top_pm_ren_OFFSET  			(0x1DC)
#define clkgen_if_rx_top_pm_cen_OFFSET  			(0x1E0)
#define clkgen_if_rx_top_pm_ren_OFFSET  			(0x1E4)
#define clkgen_if_npu_top_pm_cen_OFFSET  			(0x1E8)
#define clkgen_if_npu_top_pm_ren_OFFSET  			(0x1EC)
#define clkgen_if_fe_top_pm_cen_OFFSET  			(0x1F0)
#define clkgen_if_fe_top_pm_ren_OFFSET  			(0x1F4)
#define clkgen_if_tx_dsp0_cen_OFFSET  				(0x1F8)
#define clkgen_if_tx_dsp0_ren_OFFSET  				(0x1FC)
#define clkgen_if_tx_dsp1_cen_OFFSET  				(0x200)
#define clkgen_if_tx_dsp1_ren_OFFSET  				(0x204)
#define clkgen_if_rx_dsp0_cen_OFFSET  				(0x208)
#define clkgen_if_rx_dsp0_ren_OFFSET  				(0x20C)
#define clkgen_if_rx_dsp1_cen_OFFSET  				(0x210)
#define clkgen_if_rx_dsp1_ren_OFFSET  				(0x214)
#define clkgen_if_rx_dsp2_cen_OFFSET  				(0x218)
#define clkgen_if_rx_dsp2_ren_OFFSET  				(0x21C)
#define clkgen_if_rx_dsp3_cen_OFFSET  				(0x220)
#define clkgen_if_rx_dsp3_ren_OFFSET  				(0x224)
#define clkgen_if_tx_cpu0_cen_OFFSET  				(0x228)
#define clkgen_if_tx_cpu0_ren_OFFSET  				(0x22C)
#define clkgen_if_tx_cpu1_cen_OFFSET  				(0x230)
#define clkgen_if_tx_cpu1_ren_OFFSET  				(0x234)
#define clkgen_if_tx_cpu2_cen_OFFSET  				(0x238)
#define clkgen_if_tx_cpu2_ren_OFFSET  				(0x23C)
#define clkgen_if_tx_cpu3_cen_OFFSET  				(0x240)
#define clkgen_if_tx_cpu3_ren_OFFSET  				(0x244)
#define clkgen_if_rx_cpu0_cen_OFFSET  				(0x248)
#define clkgen_if_rx_cpu0_ren_OFFSET  				(0x24C)
#define clkgen_if_rx_cpu1_cen_OFFSET  				(0x250)
#define clkgen_if_rx_cpu1_ren_OFFSET  				(0x254)
#define clkgen_if_npu_cpu0_cen_OFFSET  				(0x258)
#define clkgen_if_npu_cpu0_ren_OFFSET  				(0x25C)
#define clkgen_if_npu_cpu1_cen_OFFSET  				(0x260)
#define clkgen_if_npu_cpu1_ren_OFFSET  				(0x264)
#define clkgen_if_npu_cpu2_cen_OFFSET  				(0x268)
#define clkgen_if_npu_cpu2_ren_OFFSET  				(0x26C)
#define clkgen_if_npu_cpu3_cen_OFFSET  				(0x270)
#define clkgen_if_npu_cpu3_ren_OFFSET  				(0x274)
#define clkgen_if_npu_cpu4_cen_OFFSET  				(0x278)
#define clkgen_if_npu_cpu4_ren_OFFSET  				(0x27C)
#define clkgen_if_npu_cpu5_cen_OFFSET  				(0x280)
#define clkgen_if_npu_cpu5_ren_OFFSET  				(0x284)
#define clkgen_if_npu_arm0_cen_OFFSET  				(0x288)
#define clkgen_if_npu_arm0_ren_OFFSET  				(0x28C)
#define clkgen_if_npu_arm1_cen_OFFSET  				(0x290)
#define clkgen_if_npu_arm1_ren_OFFSET  				(0x294)
#define clkgen_if_npu_arm2_cen_OFFSET  				(0x298)
#define clkgen_if_npu_arm2_ren_OFFSET  				(0x29C)
#define clkgen_if_npu_arm3_cen_OFFSET  				(0x2A0)
#define clkgen_if_npu_arm3_ren_OFFSET  				(0x2A4)
#define clkgen_if_npu_uart_cen_OFFSET  				(0x2A8)
#define clkgen_if_npu_uart_ren_OFFSET  				(0x2AC)
#define clkgen_if_npu_spi_cen_OFFSET  				(0x2B0)
#define clkgen_if_npu_spi_ren_OFFSET  				(0x2B4)
#define clkgen_if_gpio0_cen_OFFSET  				(0x2B8)
#define clkgen_if_gpio0_ren_OFFSET  				(0x2BC)
#define clkgen_if_gpio1_cen_OFFSET  				(0x2C0)
#define clkgen_if_gpio1_ren_OFFSET  				(0x2C4)
#define clkgen_if_gpio2_cen_OFFSET  				(0x2C8)
#define clkgen_if_gpio2_ren_OFFSET  				(0x2CC)
#define clkgen_if_npu_timer0_cen_OFFSET  			(0x2D0)
#define clkgen_if_npu_timer0_ren_OFFSET  			(0x2D4)
#define clkgen_if_npu_timer1_cen_OFFSET  			(0x2D8)
#define clkgen_if_npu_timer1_ren_OFFSET  			(0x2DC)
#define clkgen_if_npu_timer2_cen_OFFSET  			(0x2E0)
#define clkgen_if_npu_timer2_ren_OFFSET  			(0x2E4)
#define clkgen_if_npu_timer3_cen_OFFSET  			(0x2E8)
#define clkgen_if_npu_timer3_ren_OFFSET  			(0x2EC)
#define clkgen_if_npu_wdt_cen_OFFSET  				(0x2F0)
#define clkgen_if_npu_wdt_ren_OFFSET  				(0x2F4)
#define clkgen_if_tx_semaphore_cen_OFFSET  			(0x2F8)
#define clkgen_if_tx_semaphore_ren_OFFSET  			(0x2FC)
#define clkgen_if_rx_semaphore_cen_OFFSET  			(0x300)
#define clkgen_if_rx_semaphore_ren_OFFSET  			(0x304)
#define clkgen_if_npu_semaphore0_cen_OFFSET  		(0x308)
#define clkgen_if_npu_semaphore0_ren_OFFSET  		(0x30C)
#define clkgen_if_npu_semaphore1_cen_OFFSET  		(0x310)
#define clkgen_if_npu_semaphore1_ren_OFFSET  		(0x314)
#define clkgen_if_npu_sw_interrupts_cen_OFFSET  	(0x318)
#define clkgen_if_npu_sw_interrupts_ren_OFFSET  	(0x31C)
#define clkgen_if_tx_ipc0_cen_OFFSET  				(0x320)
#define clkgen_if_tx_ipc0_ren_OFFSET  				(0x324)
#define clkgen_if_tx_ipc1_cen_OFFSET  				(0x328)
#define clkgen_if_tx_ipc1_ren_OFFSET  				(0x32C)
#define clkgen_if_tx_ipc2_cen_OFFSET  				(0x330)
#define clkgen_if_tx_ipc2_ren_OFFSET  				(0x334)
#define clkgen_if_rx_ipc0_cen_OFFSET  				(0x338)
#define clkgen_if_rx_ipc0_ren_OFFSET  				(0x33C)
#define clkgen_if_rx_ipc1_cen_OFFSET  				(0x340)
#define clkgen_if_rx_ipc1_ren_OFFSET  				(0x344)
#define clkgen_if_rx_ipc2_cen_OFFSET  				(0x348)
#define clkgen_if_rx_ipc2_ren_OFFSET  				(0x34C)
#define clkgen_if_npu_ipc0_cen_OFFSET  				(0x350)
#define clkgen_if_npu_ipc0_ren_OFFSET  				(0x354)
#define clkgen_if_npu_ipc1_cen_OFFSET  				(0x358)
#define clkgen_if_npu_ipc1_ren_OFFSET  				(0x35C)
#define clkgen_if_npu_ipc2_cen_OFFSET  				(0x360)
#define clkgen_if_npu_ipc2_ren_OFFSET  				(0x364)
#define clkgen_if_npu_ipc3_cen_OFFSET  				(0x368)
#define clkgen_if_npu_ipc3_ren_OFFSET  				(0x36C)
#define clkgen_if_npu_ipc4_cen_OFFSET  				(0x370)
#define clkgen_if_npu_ipc4_ren_OFFSET  				(0x374)
#define clkgen_if_npu_txf_cen_OFFSET  				(0x378)
#define clkgen_if_npu_txf_ren_OFFSET  				(0x37C)
#define clkgen_if_npu_gmac0_cen_OFFSET  			(0x380)
#define clkgen_if_npu_gmac0_ren_OFFSET  			(0x384)
#define clkgen_if_npu_gmac1_cen_OFFSET  			(0x388)
#define clkgen_if_npu_gmac1_ren_OFFSET  			(0x38C)
#define clkgen_if_tx_dmsh_dma_rd0_cen_OFFSET  		(0x390)
#define clkgen_if_tx_dmsh_dma_rd0_ren_OFFSET  		(0x394)
#define clkgen_if_tx_dmsh_dma_rd1_cen_OFFSET  		(0x398)
#define clkgen_if_tx_dmsh_dma_rd1_ren_OFFSET  		(0x39C)
#define clkgen_if_tx_dmsh_dma_wr0_cen_OFFSET  		(0x3A0)
#define clkgen_if_tx_dmsh_dma_wr0_ren_OFFSET  		(0x3A4)
#define clkgen_if_tx_dmsh_dma_wr1_cen_OFFSET  		(0x3A8)
#define clkgen_if_tx_dmsh_dma_wr1_ren_OFFSET  		(0x3AC)
#define clkgen_if_tx_fmsh_dma_rd0_cen_OFFSET  		(0x3B0)
#define clkgen_if_tx_fmsh_dma_rd0_ren_OFFSET  		(0x3B4)
#define clkgen_if_tx_fmsh_dma_rd1_cen_OFFSET  		(0x3B8)
#define clkgen_if_tx_fmsh_dma_rd1_ren_OFFSET  		(0x3BC)
#define clkgen_if_tx_fmsh_dma_wr0_cen_OFFSET  		(0x3C0)
#define clkgen_if_tx_fmsh_dma_wr0_ren_OFFSET  		(0x3C4)
#define clkgen_if_tx_fmsh_dma_wr1_cen_OFFSET  		(0x3C8)
#define clkgen_if_tx_fmsh_dma_wr1_ren_OFFSET  		(0x3CC)
#define clkgen_if_rx_dmsh_dma_rd0_cen_OFFSET  		(0x3D0)
#define clkgen_if_rx_dmsh_dma_rd0_ren_OFFSET  		(0x3D4)
#define clkgen_if_rx_dmsh_dma_rd1_cen_OFFSET  		(0x3D8)
#define clkgen_if_rx_dmsh_dma_rd1_ren_OFFSET  		(0x3DC)
#define clkgen_if_rx_dmsh_dma_rd2_cen_OFFSET  		(0x3E0)
#define clkgen_if_rx_dmsh_dma_rd2_ren_OFFSET  		(0x3E4)
#define clkgen_if_rx_dmsh_dma_rd3_cen_OFFSET  		(0x3E8)
#define clkgen_if_rx_dmsh_dma_rd3_ren_OFFSET  		(0x3EC)
#define clkgen_if_rx_dmsh_dma_wr0_cen_OFFSET  		(0x3F0)
#define clkgen_if_rx_dmsh_dma_wr0_ren_OFFSET  		(0x3F4)
#define clkgen_if_rx_dmsh_dma_wr1_cen_OFFSET  		(0x3F8)
#define clkgen_if_rx_dmsh_dma_wr1_ren_OFFSET  		(0x3FC)
#define clkgen_if_rx_dmsh_dma_wr2_cen_OFFSET  		(0x400)
#define clkgen_if_rx_dmsh_dma_wr2_ren_OFFSET  		(0x404)
#define clkgen_if_rx_dmsh_dma_wr3_cen_OFFSET  		(0x408)
#define clkgen_if_rx_dmsh_dma_wr3_ren_OFFSET  		(0x40C)
#define clkgen_if_rx_fmsh_dma_rd0_cen_OFFSET  		(0x410)
#define clkgen_if_rx_fmsh_dma_rd0_ren_OFFSET  		(0x414)
#define clkgen_if_rx_fmsh_dma_rd1_cen_OFFSET  		(0x418)
#define clkgen_if_rx_fmsh_dma_rd1_ren_OFFSET  		(0x41C)
#define clkgen_if_rx_fmsh_dma_wr0_cen_OFFSET  		(0x420)
#define clkgen_if_rx_fmsh_dma_wr0_ren_OFFSET  		(0x424)
#define clkgen_if_rx_fmsh_dma_wr1_cen_OFFSET  		(0x428)
#define clkgen_if_rx_fmsh_dma_wr1_ren_OFFSET  		(0x42C)
#define clkgen_if_npu_fmsh_dma_rd0_cen_OFFSET  		(0x430)
#define clkgen_if_npu_fmsh_dma_rd0_ren_OFFSET  		(0x434)
#define clkgen_if_npu_fmsh_dma_rd1_cen_OFFSET  		(0x438)
#define clkgen_if_npu_fmsh_dma_rd1_ren_OFFSET  		(0x43C)
#define clkgen_if_npu_fmsh_dma_wr0_cen_OFFSET  		(0x440)
#define clkgen_if_npu_fmsh_dma_wr0_ren_OFFSET  		(0x444)
#define clkgen_if_npu_fmsh_dma_wr1_cen_OFFSET  		(0x448)
#define clkgen_if_npu_fmsh_dma_wr1_ren_OFFSET  		(0x44C)
#define clkgen_if_fe_tx_path_a_cen_OFFSET  			(0x450)
#define clkgen_if_fe_tx_path_a_ren_OFFSET  			(0x454)
#define clkgen_if_fe_tx_path_b_cen_OFFSET  			(0x458)
#define clkgen_if_fe_tx_path_b_ren_OFFSET  			(0x45C)
#define clkgen_if_fe_rx_path_a_cen_OFFSET  			(0x460)
#define clkgen_if_fe_rx_path_a_ren_OFFSET  			(0x464)
#define clkgen_if_fe_rx_path_b_cen_OFFSET  			(0x468)
#define clkgen_if_fe_rx_path_b_ren_OFFSET  			(0x46C)
#define clkgen_if_fe_fifo_cen_OFFSET  				(0x470)
#define clkgen_if_fe_fifo_ren_OFFSET  				(0x474)
#define clkgen_if_fe_tx_pri0_cen_OFFSET  			(0x478)
#define clkgen_if_fe_tx_pri0_ren_OFFSET  			(0x47C)
#define clkgen_if_fe_tx_pri1_cen_OFFSET  			(0x480)
#define clkgen_if_fe_tx_pri1_ren_OFFSET  			(0x484)
#define clkgen_if_fe_tx_pri2_cen_OFFSET  			(0x488)
#define clkgen_if_fe_tx_pri2_ren_OFFSET  			(0x48C)
#define clkgen_if_fe_tx_pri3_cen_OFFSET  			(0x490)
#define clkgen_if_fe_tx_pri3_ren_OFFSET  			(0x494)
#define clkgen_if_fe_rx_pri0_cen_OFFSET  			(0x498)
#define clkgen_if_fe_rx_pri0_ren_OFFSET  			(0x49C)
#define clkgen_if_fe_rx_pri1_cen_OFFSET  			(0x4A0)
#define clkgen_if_fe_rx_pri1_ren_OFFSET  			(0x4A4)
#define clkgen_if_fe_rx_pri2_cen_OFFSET  			(0x4A8)
#define clkgen_if_fe_rx_pri2_ren_OFFSET  			(0x4AC)
#define clkgen_if_fe_rx_pri3_cen_OFFSET  			(0x4B0)
#define clkgen_if_fe_rx_pri3_ren_OFFSET  			(0x4B4)
#define clkgen_if_fe_rx_pri4_cen_OFFSET  			(0x4B8)
#define clkgen_if_fe_rx_pri4_ren_OFFSET  			(0x4BC)
#define clkgen_if_ddr0_cen_OFFSET  					(0x4C0)
#define clkgen_if_ddr0_ren_OFFSET  					(0x4C4)
#define clkgen_if_ddr1_cen_OFFSET  					(0x4C8)
#define clkgen_if_ddr1_ren_OFFSET  					(0x4CC)
#define clkgen_if_fe_tbus_cen_OFFSET  				(0x4D0)
#define clkgen_if_fe_tbus_ren_OFFSET  				(0x4D4)
#define clkgen_if_srds0_cen_OFFSET  				(0x4D8)
#define clkgen_if_srds0_ren_OFFSET  				(0x4DC)
#define clkgen_if_srds1_cen_OFFSET  				(0x4E0)
#define clkgen_if_srds1_ren_OFFSET  				(0x4E4)
#define clkgen_if_srds2_cen_OFFSET  				(0x4E8)
#define clkgen_if_srds2_ren_OFFSET  				(0x4EC)
#define clkgen_if_srds3_cen_OFFSET  				(0x4F0)
#define clkgen_if_srds3_ren_OFFSET  				(0x4F4)
#define clkgen_if_core_pm_cen_OFFSET  				(0x4F8)
#define clkgen_if_core_pm_ren_OFFSET  				(0x4FC)
#define clkgen_if_io_ctl_cen_OFFSET  				(0x500)
#define clkgen_if_io_ctl_ren_OFFSET  				(0x504)
#define clkgen_if_timing_unit_cen_OFFSET  			(0x508)
#define clkgen_if_timing_unit_ren_OFFSET  			(0x50C)
#define clkgen_if_tx_unused_cen_OFFSET  			(0x510)
#define clkgen_if_tx_unused_ren_OFFSET  			(0x514)
#define clkgen_if_rx_unused_cen_OFFSET  			(0x518)
#define clkgen_if_rx_unused_ren_OFFSET  			(0x51C)
#define clkgen_if_npu_unused_cen_OFFSET  			(0x520)
#define clkgen_if_npu_unused_ren_OFFSET  			(0x524)
#define clkgen_if_npu_security0_cen_OFFSET  		(0x528)
#define clkgen_if_npu_security0_ren_OFFSET  		(0x52C)
#define clkgen_if_npu_security1_cen_OFFSET  		(0x530)
#define clkgen_if_npu_security1_ren_OFFSET  		(0x534)
#define clkgen_if_npu_wimax_sec_cen_OFFSET  		(0x538)
#define clkgen_if_npu_wimax_sec_ren_OFFSET  		(0x53C)

#define CEN_DIS					0x0000
#define CEN_EN					~CEN_DIS					

#define REN_EN					0x0000
#define REN_DIS					~CEN_EN

#define CGEN_CHIP_REVISION_A	0x00
#define	CGEN_CHIP_REVISION_B	0x0B


typedef union clkgen_cg_pll0_cfg_s
{
	uint32 word;
	struct {
		uint32 pre:5;
		uint32 rsv0:3;
		uint32 mul:7;
		uint32 rsv1:1;
		uint32 post:2;
		uint32 rsv2:6;
		uint32 bs:1;
		uint32 rsv3:6;
		uint32 pd:1;
	} bits;
} clkgen_cg_pll0_cfg_t;


typedef union clkgen_cg_clocks_cfg_s
{
	uint32 word;
	struct {
		uint32 div2_slow:1;
		uint32 pll0_byp:1;
		uint32 pll1_byp:1;
		uint32 pll2_byp:1;
		uint32 pll3_byp:1;
		uint32 rsv0:1;
		uint32 tx_enc_sel:1;
		uint32 pll2_in_sel:1;
		uint32 clkout_sel:2;
		uint32 rsv1:2;
		uint32 fe_tx0_pol:1;
		uint32 fe_tx1_pol:1;
		uint32 fe_tx2_pol:1;
		uint32 fe_tx3_pol:1;
		uint32 div2_slow_we:1;
		uint32 pll0_byp_we:1;
		uint32 pll1_byp_we:1;
		uint32 pll2_byp_we:1;
		uint32 pll3_byp_we:1;
		uint32 rsv2:1;
		uint32 tx_enc_sel_we:1;
		uint32 pll2_in_sel_we:1;
		uint32 clkout_sel_we:2;
		uint32 rsv3:2;
		uint32 fe_tx0_pol_we:1;
		uint32 fe_tx1_pol_we:1;
		uint32 fe_tx2_pol_we:1;
		uint32 fe_tx3_pol_we:1;
	} bits;
} clkgen_cg_clocks_cfg_t;

/*
 * -----------------------------------------------------------
 * Global prototypes section
 * -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------
 * Function:	CGEN_PLL0_get_clk
 * Description:	Get output clock of DAN PLL0
 * Input:		Input clock
 * Output:		Output clock
 * -----------------------------------------------------------
 */
uint32 CGEN_PLL0_get_clk(uint32 inp_clk);

/*
 * -----------------------------------------------------------
 * Function:	CGEN_PLL1_get_clk
 * Description:	Get output clock of DAN PLL1
 * Input:		Input clock
 * Output:		Output clock
 * -----------------------------------------------------------
 */
uint32 CGEN_PLL1_get_clk(uint32 inp_clk);

/*
 * -----------------------------------------------------------
 * Function:	CGEN_PLL2_get_clk
 * Description:	Get output clock of DAN PLL2
 * Input:		Input clock
 * Output:		Output clock
 * -----------------------------------------------------------
 */
uint32 CGEN_PLL2_get_clk(uint32 inp_clk);

/*
 * -----------------------------------------------------------
 * Function:	CGEN_PLL3_get_clk
 * Description:	Get output clock of DAN PLL3
 * Input:		Input clock
 * Output:		Output clock
 * -----------------------------------------------------------
 */
uint32 CGEN_PLL3_get_clk(uint32 inp_clk);

/*
 * -----------------------------------------------------------
 * Function:	CGEN_PLL0_init
 * Description:	Initialization DAN PLL0
 * Input:		Configuration mode per PLL number
 * Output:		none
 * -----------------------------------------------------------
 */
void CGEN_PLL0_init(uint8 pll0_mode, BOOL bypass);

void CGEN_PLL0_bypass(int use_bypass);
/*
 * -----------------------------------------------------------
 * Function:	CGEN_PLL1_init
 * Description:	Initialization DAN PLL1
 * Input:		Configuration mode per PLL number
 * Output:		none
 * -----------------------------------------------------------
 */
void CGEN_PLL1_init(uint8 pll1_mode);

/*
 * -----------------------------------------------------------
 * Function:	CGEN_PLL2_init
 * Description:	Initialization DAN PLL2
 * Input:		Configuration mode per PLL number
 * Output:		none
 * -----------------------------------------------------------
 */
void CGEN_PLL2_init(uint8 pll2_mode);

/*
 * -----------------------------------------------------------
 * Function:	CGEN_PLL3_init
 * Description:	Initialization DAN PLL3
 * Input:		Configuration mode per PLL number
 * Output:		none
 * -----------------------------------------------------------
 */
void CGEN_PLL3_init(uint8 pll3_mode);


/*
 * -----------------------------------------------------------
 * Function:	CGEN_clk_ctrl
 * Description:	Control of clock register for desired unit
 * Input:		CGEN_UNIT_CEN_OFFSET(loc,name)
 *	 			set_val � �0�- disable CLK
 * Output:		None
 * -----------------------------------------------------------
 */
void CGEN_ctrl_clk(uint32 offset, uint32 set_val);

/*
 * -----------------------------------------------------------
 * Function:	CGEN_reset_ctrl
 * Description:	Control of reset register for desired unit
 * Input:		CGEN_UNIT_REN_OFFSET(loc,name)
 *	 			set_val � �0�- Reset
 * Output:		None
 * -----------------------------------------------------------
 */
void CGEN_ctrl_reset(uint32 offset, uint32 set_val);

/*
 * -----------------------------------------------------------
 * Function:	CGEN_master_reset
 * Description:	SW Reset of SOC 
 * Input:		None
 * Output:		None
 * -----------------------------------------------------------
 */

void CGEN_master_reset(void);


/*
 * -----------------------------------------------------------
 * Function:	CGEN_get_chip_revision
 * Description:	Get revivion of SOC 
 * Input:		None
 * Output:		None
 * -----------------------------------------------------------
 */

uint8 CGEN_get_chip_revision(void);

/*
 * -----------------------------------------------------------
 * Function:	CGEN_is_chip_revision
 * Description:	Check revivion of SOC 
 * Input:		Revivion number (see  defines CGEN_CHIP_REVISION_A, B)
 * Output:		None
 * -----------------------------------------------------------
 */

uint8 CGEN_is_chip_revision(uint8 rev);


/*
 * -----------------------------------------------------------
 * Function:	CGEN_ctrl_##_clk_domain
 * Description:	controls the CGEN clock enables (cen) 
 * Input:		set_val - uint16, each bit set HIGH enable
 * Output:		None
 * -----------------------------------------------------------
 */
extern void CGEN_ctrl_rst_domain_switch(uint32 *array_ptr, uint32 set_val);
extern void CGEN_ctrl_rst_domain(uint16 set_val);
extern void CGEN_ctrl_clk_domain_switch(uint32 *array_ptr, uint32 set_val);
extern void CGEN_ctrl_clk_domain(uint16 set_val);

#endif /* _CGEN_H */

