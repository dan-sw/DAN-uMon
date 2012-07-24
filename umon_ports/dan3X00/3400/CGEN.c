/**********************************************************************\
 Library    :  uMON (Micro Monitor)
 Filename   :  CGEN.c
 Purpose    :  ClockGen unit (clock and reset generator) implementation
 Owner		:  Igor Lebedev
 Note		:  Based on the original CGEN.c from application
\**********************************************************************/

#include "config.h"
#include "cpu.h"
#include "tmr.h"
#include "CGEN.h"

/*
 * -----------------------------------------------------------
 * MACRO (define) section
 * -----------------------------------------------------------
 */

#define PLL_CFG_ADDR(pll)			(clkgen_if_MODULE_ADDR+clkgen_if_cg_##pll##_cfg_OFFSET)
#define CGEN_CLOCKS_CFG_ADDR		(clkgen_if_MODULE_ADDR+clkgen_if_cg_clocks_cfg_OFFSET)
#define CGEN_SRDS_DIV_ADDR(_n)		(clkgen_if_MODULE_ADDR+clkgen_if_cg_serdes##_n##_div_OFFSET)

/* see cgen_reg.h with CLOCKGEN register definitions */
#define CGEN_UNIT_CEN_OFFSET(loc,name)	clkgen_if_##loc##_##name##_cen_OFFSET
#define CGEN_UNIT_REN_OFFSET(loc,name)	clkgen_if_##loc##_##name##_ren_OFFSET 
#define CGEN_TOP_CEN_OFFSET(name)		clkgen_if_##name##_cen_OFFSET
#define CGEN_TOP_REN_OFFSET(name)		clkgen_if_##name##_ren_OFFSET 
#define END_OF_RST_ARRAY       			0xffffffff
#define END_OF_CLK_ARRAY       			0xffffffff

#define PLL0_CFG_PARAM_DEFAULT		0
#define PLL1_CFG_PARAM_DEFAULT		0
#define PLL23_CFG_PARAM_DEFAULT		0

#define CGEN_PLL_POWER_DOWN			1
#define CGEN_PLL_ENABLE				0

#define CGEN_MIN_DELAY_COUNT		4*100		// Based on measurements in DAN LAB

#define CGEN_COUNT_DELAY(_c)						\
{													\
	uint32 _count;									\
	for (_count = 0; _count<_c; _count++)			\
	{												\
		invl(CGEN_CLOCKS_CFG_ADDR);					\
	}												\
}

#define CGEN_CLK_CFG_SET_FIELD(_n,_f,_v)	_n._f=_v


#define SLOW_CLK_EQUALS_FAST	0
#define SLOW_CLK_HALF_FAST		1
#define PLL_IS_USED				0
#define PLL_IS_BYPASSED			1
#define PLL2_IN_CLKIN			0
#define PLL2_IN_PLL3_OUT		1
#define WRITE_ENABLE			1
#define DIVIDER_IS_USED			1

#define DELAY_VAL_MIN			75 /* 40 MHz clock */

#define SRDS_DIV_FACTOR_DEFAULT	20

/* define real CLK GEN address  */
#define CGEN_BASE_ADDR	0xe573e000


/*
 * -----------------------------------------------------------
 * Type definition section
 * -----------------------------------------------------------
 */

/* PLL parameters for Oscillator 40 */
typedef struct PLL_cfg_param_s
{
	uint16	frequency;
	uint8	PRE;
	uint8	MUL;
	uint8	POST;
	uint8	BAND;
} PLL_cfg_param_t;


typedef enum
{
	CGEN_e_pll0_freq_600 = 0,
	CGEN_e_pll0_freq_150,
	CGEN_e_pll0_freq_100,
	CGEN_e_pll0_freq_200,
	CGEN_e_pll0_freq_300,
	CGEN_e_pll0_freq_240,
	CGEN_e_pll0_freq_270,
	PLL0_CFG_PARAM_MAX
} CGEN_e_pll0_cfg;


typedef enum
{
	CGEN_e_pll1_freq_1320 = 0,
	CGEN_e_pll1_freq_1360,
	CGEN_e_pll1_freq_1400,
	PLL1_CFG_PARAM_MAX
} CGEN_e_pll1_cfg;


typedef enum
{
	CGEN_e_pll23_freq_175 = 0,
	CGEN_e_pll23_freq_200,
	CGEN_e_pll23_freq_250,
	CGEN_e_pll23_freq_333,
	CGEN_e_pll23_freq_225,
	CGEN_e_pll23_freq_275,
	CGEN_e_pll23_freq_300,
	CGEN_e_pll23_freq_325,
	PLL23_CFG_PARAM_MAX
} CGEN_e_pll23_cfg;


PLL_cfg_param_t	pll0_cfg_param_array[PLL0_CFG_PARAM_MAX] =
{
// 	frequency	PRE	MUL	POST	BAND
	{600,	1,	29,	1,	0},
	{150,	1,	29, 3,	0},
	{100,	3,	39, 3,	0},
	{200,	3,	39, 2,	0},
	{300,	1,	29, 2,	0},
	{240,	1,	23, 2,	0},
	{270,	1,	26, 2,	0},
};

PLL_cfg_param_t	pll1_cfg_param_array[PLL1_CFG_PARAM_MAX] =
{
// 	frequency	PRE	MUL	POST	BAND
	{1320,	0,	32,	1,	1},
	{1360,	0,	33,	1,	1},
	{1400,	0,	34,	1,	1},
};

PLL_cfg_param_t	pll2_cfg_param_array[PLL23_CFG_PARAM_MAX] =
{
// 	frequency	PRE	MUL	POST	BAND
	{1250,	6,	49,	1,	1},
	{1250,	7,	49,	1,	1},
	{1250,	9,	49,	1,	1},
	{1250,	7,	29,	1,	1},
	{1250,	8,	49,	1,	1},
	{1250,	10,	49,	1,	1},
	{1250,	11,	49,	1,	1},
	{1250,	12,	49,	1,	1},
};

PLL_cfg_param_t	pll3_cfg_param_array[PLL23_CFG_PARAM_MAX] =
{
// 	frequency	PRE	MUL	POST	BAND
// out PLL3
	{175,	3,	69,	3,	0},
	{200,	3,	39,	2,	0},
	{250,	3,	49,	2,	0},
	{333,	2,	49,	2,	0},
	{225,	3,	44,	2,	0},
	{275,	3,	54,	2,	0},
	{300,	3,	59,	2,	0},
	{325,	3,	64,	2,	0},
};

uint32 rx_clk_domain_array[] = {
//									CGEN_UNIT_CEN_OFFSET(rx, dsp0),				// BOOT 
//									CGEN_UNIT_CEN_OFFSET(rx, top_pm),
//									CGEN_UNIT_CEN_OFFSET(rx, cb),
									CGEN_UNIT_CEN_OFFSET(rx, dsp1),
									CGEN_UNIT_CEN_OFFSET(rx, dsp2),
									CGEN_UNIT_CEN_OFFSET(rx, dsp3),
									CGEN_UNIT_CEN_OFFSET(rx, cpu0),
									CGEN_UNIT_CEN_OFFSET(rx, cpu1),
									CGEN_UNIT_CEN_OFFSET(rx, dec0),
									CGEN_UNIT_CEN_OFFSET(rx, dec1),
									CGEN_UNIT_CEN_OFFSET(rx, dec2),
									CGEN_UNIT_CEN_OFFSET(rx, dec3),
									CGEN_UNIT_CEN_OFFSET(rx, dec4),
									CGEN_UNIT_CEN_OFFSET(rx, dec5),
									CGEN_UNIT_CEN_OFFSET(rx, fft0),
									CGEN_UNIT_CEN_OFFSET(rx, fft1),
									CGEN_UNIT_CEN_OFFSET(rx, fft2),
									CGEN_UNIT_CEN_OFFSET(rx, fft3),
									CGEN_UNIT_CEN_OFFSET(rx, fft4),
									CGEN_UNIT_CEN_OFFSET(rx, fft5),
									CGEN_UNIT_CEN_OFFSET(rx, gpdma),
									CGEN_UNIT_CEN_OFFSET(rx, semaphore),
									CGEN_UNIT_CEN_OFFSET(rx, ipc0),
									CGEN_UNIT_CEN_OFFSET(rx, ipc1),
									CGEN_UNIT_CEN_OFFSET(rx, ipc2),
									CGEN_UNIT_CEN_OFFSET(rx, dmsh_dma_rd0),
									CGEN_UNIT_CEN_OFFSET(rx, dmsh_dma_rd1),
									CGEN_UNIT_CEN_OFFSET(rx, dmsh_dma_rd2),
									CGEN_UNIT_CEN_OFFSET(rx, dmsh_dma_rd3),
									CGEN_UNIT_CEN_OFFSET(rx, dmsh_dma_wr0),
									CGEN_UNIT_CEN_OFFSET(rx, dmsh_dma_wr1),
									CGEN_UNIT_CEN_OFFSET(rx, dmsh_dma_wr2),
									CGEN_UNIT_CEN_OFFSET(rx, dmsh_dma_wr3),
									CGEN_UNIT_CEN_OFFSET(rx, fmsh_dma_rd0),
									CGEN_UNIT_CEN_OFFSET(rx, fmsh_dma_rd1),
									CGEN_UNIT_CEN_OFFSET(rx, fmsh_dma_wr0),
									CGEN_UNIT_CEN_OFFSET(rx, fmsh_dma_wr1),
//									CGEN_UNIT_CEN_OFFSET(rx, unused)
									END_OF_CLK_ARRAY
								};
uint32 rx_rst_domain_array[] = {
//									CGEN_UNIT_REN_OFFSET(rx, dsp0),				// BOOT 
//									CGEN_UNIT_REN_OFFSET(rx, top_pm),
//									CGEN_UNIT_REN_OFFSET(rx, cb),
//									CGEN_UNIT_REN_OFFSET(rx, dsp1),
//									CGEN_UNIT_REN_OFFSET(rx, dsp2),
//									CGEN_UNIT_REN_OFFSET(rx, dsp3),
//									CGEN_UNIT_REN_OFFSET(rx, cpu0),
//									CGEN_UNIT_REN_OFFSET(rx, cpu1),
									CGEN_UNIT_REN_OFFSET(rx, dec0),
									CGEN_UNIT_REN_OFFSET(rx, dec1),
									CGEN_UNIT_REN_OFFSET(rx, dec2),
									CGEN_UNIT_REN_OFFSET(rx, dec3),
									CGEN_UNIT_REN_OFFSET(rx, dec4),
									CGEN_UNIT_REN_OFFSET(rx, dec5),
									CGEN_UNIT_REN_OFFSET(rx, fft0),
									CGEN_UNIT_REN_OFFSET(rx, fft1),
									CGEN_UNIT_REN_OFFSET(rx, fft2),
									CGEN_UNIT_REN_OFFSET(rx, fft3),
									CGEN_UNIT_REN_OFFSET(rx, fft4),
									CGEN_UNIT_REN_OFFSET(rx, fft5),
									CGEN_UNIT_REN_OFFSET(rx, gpdma),
									CGEN_UNIT_REN_OFFSET(rx, semaphore),
									CGEN_UNIT_REN_OFFSET(rx, ipc0),
									CGEN_UNIT_REN_OFFSET(rx, ipc1),
									CGEN_UNIT_REN_OFFSET(rx, ipc2),
									CGEN_UNIT_REN_OFFSET(rx, dmsh_dma_rd0),
									CGEN_UNIT_REN_OFFSET(rx, dmsh_dma_rd1),
									CGEN_UNIT_REN_OFFSET(rx, dmsh_dma_rd2),
									CGEN_UNIT_REN_OFFSET(rx, dmsh_dma_rd3),
									CGEN_UNIT_REN_OFFSET(rx, dmsh_dma_wr0),
									CGEN_UNIT_REN_OFFSET(rx, dmsh_dma_wr1),
									CGEN_UNIT_REN_OFFSET(rx, dmsh_dma_wr2),
									CGEN_UNIT_REN_OFFSET(rx, dmsh_dma_wr3),
									CGEN_UNIT_REN_OFFSET(rx, fmsh_dma_rd0),
									CGEN_UNIT_REN_OFFSET(rx, fmsh_dma_rd1),
									CGEN_UNIT_REN_OFFSET(rx, fmsh_dma_wr0),
									CGEN_UNIT_REN_OFFSET(rx, fmsh_dma_wr1),
//									CGEN_UNIT_REN_OFFSET(rx, unused)
									END_OF_RST_ARRAY
								};
uint32 tx_clk_domain_array[] = {
//									CGEN_UNIT_CEN_OFFSET(tx, cpu0),				// BOOT	
//									CGEN_UNIT_CEN_OFFSET(tx, top_pm),
//									CGEN_UNIT_CEN_OFFSET(tx, cb),
									CGEN_UNIT_CEN_OFFSET(tx, dsp0),
									CGEN_UNIT_CEN_OFFSET(tx, dsp1),
									CGEN_UNIT_CEN_OFFSET(tx, cpu1),
									CGEN_UNIT_CEN_OFFSET(tx, cpu2),
									CGEN_UNIT_CEN_OFFSET(tx, cpu3),
									CGEN_UNIT_CEN_OFFSET(tx, enc0),
									CGEN_UNIT_CEN_OFFSET(tx, enc1),
									CGEN_UNIT_CEN_OFFSET(tx, enc2),
									CGEN_UNIT_CEN_OFFSET(tx, enc3),
									CGEN_UNIT_CEN_OFFSET(tx, enc4),
									CGEN_UNIT_CEN_OFFSET(tx, enc5),
									CGEN_UNIT_CEN_OFFSET(tx, fft0),
									CGEN_UNIT_CEN_OFFSET(tx, fft1),
									CGEN_UNIT_CEN_OFFSET(tx, gpdma),
									CGEN_UNIT_CEN_OFFSET(tx, semaphore),
									CGEN_UNIT_CEN_OFFSET(tx, ipc0),
									CGEN_UNIT_CEN_OFFSET(tx, ipc1),
									CGEN_UNIT_CEN_OFFSET(tx, ipc2),
									CGEN_UNIT_CEN_OFFSET(tx, dmsh_dma_rd0),
									CGEN_UNIT_CEN_OFFSET(tx, dmsh_dma_rd1),
									CGEN_UNIT_CEN_OFFSET(tx, dmsh_dma_wr0),
									CGEN_UNIT_CEN_OFFSET(tx, dmsh_dma_wr1),
									CGEN_UNIT_CEN_OFFSET(tx, fmsh_dma_rd0),
									CGEN_UNIT_CEN_OFFSET(tx, fmsh_dma_rd1),
									CGEN_UNIT_CEN_OFFSET(tx, fmsh_dma_wr0),
									CGEN_UNIT_CEN_OFFSET(tx, fmsh_dma_wr1),
//									CGEN_UNIT_CEN_OFFSET(tx, unused)
									END_OF_CLK_ARRAY
								};
uint32 tx_rst_domain_array[] = {
//									CGEN_UNIT_REN_OFFSET(tx, cpu0),				// BOOT	
//									CGEN_UNIT_REN_OFFSET(tx, top_pm),
//									CGEN_UNIT_REN_OFFSET(tx, cb),
//									CGEN_UNIT_REN_OFFSET(tx, dsp0),
//									CGEN_UNIT_REN_OFFSET(tx, dsp1),
//									CGEN_UNIT_REN_OFFSET(tx, cpu1),
//									CGEN_UNIT_REN_OFFSET(tx, cpu2),
//									CGEN_UNIT_REN_OFFSET(tx, cpu3),
									CGEN_UNIT_REN_OFFSET(tx, enc0),
									CGEN_UNIT_REN_OFFSET(tx, enc1),
									CGEN_UNIT_REN_OFFSET(tx, enc2),
									CGEN_UNIT_REN_OFFSET(tx, enc3),
									CGEN_UNIT_REN_OFFSET(tx, enc4),
									CGEN_UNIT_REN_OFFSET(tx, enc5),
									CGEN_UNIT_REN_OFFSET(tx, fft0),
									CGEN_UNIT_REN_OFFSET(tx, fft1),
									CGEN_UNIT_REN_OFFSET(tx, gpdma),
									CGEN_UNIT_REN_OFFSET(tx, semaphore),
									CGEN_UNIT_REN_OFFSET(tx, ipc0),
									CGEN_UNIT_REN_OFFSET(tx, ipc1),
									CGEN_UNIT_REN_OFFSET(tx, ipc2),
									CGEN_UNIT_REN_OFFSET(tx, dmsh_dma_rd0),
									CGEN_UNIT_REN_OFFSET(tx, dmsh_dma_rd1),
									CGEN_UNIT_REN_OFFSET(tx, dmsh_dma_wr0),
									CGEN_UNIT_REN_OFFSET(tx, dmsh_dma_wr1),
									CGEN_UNIT_REN_OFFSET(tx, fmsh_dma_rd0),
									CGEN_UNIT_REN_OFFSET(tx, fmsh_dma_rd1),
									CGEN_UNIT_REN_OFFSET(tx, fmsh_dma_wr0),
									CGEN_UNIT_REN_OFFSET(tx, fmsh_dma_wr1),
//									CGEN_UNIT_REN_OFFSET(tx, unused)
									END_OF_RST_ARRAY
								};
uint32 npu_clk_domain_array[] = {
//									CGEN_UNIT_CEN_OFFSET(npu, arm3),			// BOOT		
//									CGEN_UNIT_CEN_OFFSET(npu, top_pm),
//									CGEN_UNIT_CEN_OFFSET(npu, cb),
									CGEN_UNIT_CEN_OFFSET(npu, cpu0),
									CGEN_UNIT_CEN_OFFSET(npu, cpu1),
									CGEN_UNIT_CEN_OFFSET(npu, cpu2),
									CGEN_UNIT_CEN_OFFSET(npu, cpu3),
									CGEN_UNIT_CEN_OFFSET(npu, cpu4),
									CGEN_UNIT_CEN_OFFSET(npu, cpu5),
									CGEN_UNIT_CEN_OFFSET(npu, arm0),
									CGEN_UNIT_CEN_OFFSET(npu, arm1),
									CGEN_UNIT_CEN_OFFSET(npu, arm2),
//									CGEN_UNIT_CEN_OFFSET(npu, uart),
//									CGEN_UNIT_CEN_OFFSET(npu, spi),
//									CGEN_UNIT_CEN_OFFSET(npu, timer0),
									CGEN_UNIT_CEN_OFFSET(npu, timer1),
									CGEN_UNIT_CEN_OFFSET(npu, timer2),
									CGEN_UNIT_CEN_OFFSET(npu, timer3),
//									CGEN_UNIT_CEN_OFFSET(npu, wdt),
									CGEN_UNIT_CEN_OFFSET(npu, gpdma0),
									CGEN_UNIT_CEN_OFFSET(npu, gpdma1),
									CGEN_UNIT_CEN_OFFSET(npu, semaphore0),
									CGEN_UNIT_CEN_OFFSET(npu, semaphore1),
									CGEN_UNIT_CEN_OFFSET(npu, sw_interrupts),
									CGEN_UNIT_CEN_OFFSET(npu, ipc0),
									CGEN_UNIT_CEN_OFFSET(npu, ipc1),
									CGEN_UNIT_CEN_OFFSET(npu, ipc2),
									CGEN_UNIT_CEN_OFFSET(npu, ipc3),
									CGEN_UNIT_CEN_OFFSET(npu, ipc4),
									CGEN_UNIT_CEN_OFFSET(npu, txf),
//									CGEN_UNIT_CEN_OFFSET(npu, gmac0),
//									CGEN_UNIT_CEN_OFFSET(npu, gmac1),
									CGEN_UNIT_CEN_OFFSET(npu, fmsh_dma_rd0),
									CGEN_UNIT_CEN_OFFSET(npu, fmsh_dma_rd1),
									CGEN_UNIT_CEN_OFFSET(npu, fmsh_dma_wr0),
									CGEN_UNIT_CEN_OFFSET(npu, fmsh_dma_wr1),
									CGEN_UNIT_CEN_OFFSET(npu, security0),
									CGEN_UNIT_CEN_OFFSET(npu, security1),
									CGEN_UNIT_CEN_OFFSET(npu, wimax_sec),
//									CGEN_UNIT_CEN_OFFSET(npu, unused)
									END_OF_CLK_ARRAY
								};
uint32 npu_rst_domain_array[] = {
//									CGEN_UNIT_REN_OFFSET(npu, arm3),			// BOOT		
//									CGEN_UNIT_REN_OFFSET(npu, top_pm),
//									CGEN_UNIT_REN_OFFSET(npu, cb),
//									CGEN_UNIT_REN_OFFSET(npu, cpu0),
//									CGEN_UNIT_REN_OFFSET(npu, cpu1),
//									CGEN_UNIT_REN_OFFSET(npu, cpu2),
//									CGEN_UNIT_REN_OFFSET(npu, cpu3),
//									CGEN_UNIT_REN_OFFSET(npu, cpu4),
//									CGEN_UNIT_REN_OFFSET(npu, cpu5),
//									CGEN_UNIT_REN_OFFSET(npu, arm0),
//									CGEN_UNIT_REN_OFFSET(npu, arm1),
//									CGEN_UNIT_REN_OFFSET(npu, arm2),
//									CGEN_UNIT_REN_OFFSET(npu, uart),
//									CGEN_UNIT_REN_OFFSET(npu, spi),
//									CGEN_UNIT_REN_OFFSET(npu, timer0),
									CGEN_UNIT_REN_OFFSET(npu, timer1),
									CGEN_UNIT_REN_OFFSET(npu, timer2),
									CGEN_UNIT_REN_OFFSET(npu, timer3),
//									CGEN_UNIT_REN_OFFSET(npu, wdt),
									CGEN_UNIT_REN_OFFSET(npu, gpdma0),
									CGEN_UNIT_REN_OFFSET(npu, gpdma1),
									CGEN_UNIT_REN_OFFSET(npu, semaphore0),
									CGEN_UNIT_REN_OFFSET(npu, semaphore1),
									CGEN_UNIT_REN_OFFSET(npu, sw_interrupts),
									CGEN_UNIT_REN_OFFSET(npu, ipc0),
									CGEN_UNIT_REN_OFFSET(npu, ipc1),
									CGEN_UNIT_REN_OFFSET(npu, ipc2),
									CGEN_UNIT_REN_OFFSET(npu, ipc3),
									CGEN_UNIT_REN_OFFSET(npu, ipc4),
									CGEN_UNIT_REN_OFFSET(npu, txf),
//									CGEN_UNIT_REN_OFFSET(npu, gmac0),
//									CGEN_UNIT_REN_OFFSET(npu, gmac1),
									CGEN_UNIT_REN_OFFSET(npu, fmsh_dma_rd0),
									CGEN_UNIT_REN_OFFSET(npu, fmsh_dma_rd1),
									CGEN_UNIT_REN_OFFSET(npu, fmsh_dma_wr0),
									CGEN_UNIT_REN_OFFSET(npu, fmsh_dma_wr1),
									CGEN_UNIT_REN_OFFSET(npu, security0),
									CGEN_UNIT_REN_OFFSET(npu, security1),
									CGEN_UNIT_REN_OFFSET(npu, wimax_sec),
//									CGEN_UNIT_REN_OFFSET(npu, unused)
									END_OF_RST_ARRAY
								};

uint32 fe_clk_domain_array[] = {
//									CGEN_UNIT_CEN_OFFSET(fe, top_pm),			
									CGEN_UNIT_CEN_OFFSET(fe, tx_path_a),
									CGEN_UNIT_CEN_OFFSET(fe, tx_path_b),
									CGEN_UNIT_CEN_OFFSET(fe, rx_path_a),
									CGEN_UNIT_CEN_OFFSET(fe, rx_path_b),
									CGEN_UNIT_CEN_OFFSET(fe, fifo),
									CGEN_UNIT_CEN_OFFSET(fe, tx_pri0),
									CGEN_UNIT_CEN_OFFSET(fe, tx_pri1),
									CGEN_UNIT_CEN_OFFSET(fe, tx_pri2),
									CGEN_UNIT_CEN_OFFSET(fe, tx_pri3),
									CGEN_UNIT_CEN_OFFSET(fe, rx_pri0),
									CGEN_UNIT_CEN_OFFSET(fe, rx_pri1),
									CGEN_UNIT_CEN_OFFSET(fe, rx_pri2),
									CGEN_UNIT_CEN_OFFSET(fe, rx_pri3),
									CGEN_UNIT_CEN_OFFSET(fe, rx_pri4),
									CGEN_UNIT_CEN_OFFSET(fe, tbus),
									END_OF_CLK_ARRAY
								};	
uint32 fe_rst_domain_array[] = {
//									CGEN_UNIT_REN_OFFSET(fe, top_pm),			
									CGEN_UNIT_REN_OFFSET(fe, tx_path_a),
									CGEN_UNIT_REN_OFFSET(fe, tx_path_b),
									CGEN_UNIT_REN_OFFSET(fe, rx_path_a),
									CGEN_UNIT_REN_OFFSET(fe, rx_path_b),
									CGEN_UNIT_REN_OFFSET(fe, fifo),
									CGEN_UNIT_REN_OFFSET(fe, tx_pri0),
									CGEN_UNIT_REN_OFFSET(fe, tx_pri1),
									CGEN_UNIT_REN_OFFSET(fe, tx_pri2),
									CGEN_UNIT_REN_OFFSET(fe, tx_pri3),
									CGEN_UNIT_REN_OFFSET(fe, rx_pri0),
									CGEN_UNIT_REN_OFFSET(fe, rx_pri1),
									CGEN_UNIT_REN_OFFSET(fe, rx_pri2),
									CGEN_UNIT_REN_OFFSET(fe, rx_pri3),
									CGEN_UNIT_REN_OFFSET(fe, rx_pri4),
									CGEN_UNIT_REN_OFFSET(fe, tbus),
									END_OF_RST_ARRAY
								};	
uint32 top_clk_domain_array[] = {
//									CGEN_TOP_CEN_OFFSET(core_pm),
//									CGEN_TOP_CEN_OFFSET(io_ctl),
									CGEN_TOP_CEN_OFFSET(gpio0),
//									CGEN_TOP_CEN_OFFSET(gpio1),
									CGEN_TOP_CEN_OFFSET(gpio2),
//									CGEN_TOP_CEN_OFFSET(ddr0),
//									CGEN_TOP_CEN_OFFSET(ddr1),
									CGEN_TOP_CEN_OFFSET(srds0),
									CGEN_TOP_CEN_OFFSET(srds1),
									CGEN_TOP_CEN_OFFSET(srds2),
									CGEN_TOP_CEN_OFFSET(srds3),
									CGEN_TOP_CEN_OFFSET(timing_unit),
									END_OF_CLK_ARRAY
								};
uint32 top_rst_domain_array[] = {
//									CGEN_TOP_REN_OFFSET(core_pm),
//									CGEN_TOP_REN_OFFSET(io_ctl),
									CGEN_TOP_REN_OFFSET(gpio0),
//									CGEN_TOP_REN_OFFSET(gpio1),
									CGEN_TOP_REN_OFFSET(gpio2),
//									CGEN_TOP_REN_OFFSET(ddr0),
//									CGEN_TOP_REN_OFFSET(ddr1),
									CGEN_TOP_REN_OFFSET(srds0),
									CGEN_TOP_REN_OFFSET(srds1),
									CGEN_TOP_REN_OFFSET(srds2),
									CGEN_TOP_REN_OFFSET(srds3),
									CGEN_TOP_REN_OFFSET(timing_unit),
									END_OF_RST_ARRAY
								};
/*
 * -----------------------------------------------------------
 * Global prototypes section
 * -----------------------------------------------------------
 */

uint8 CGEN_get_chip_revision(void)
{
	return ((uint8) invl(clkgen_if_MODULE_ADDR+clkgen_if_cg_fuse3_OFFSET));
}

uint8 CGEN_is_chip_revision(uint8 rev)
{
	return (((uint8) invl(clkgen_if_MODULE_ADDR+clkgen_if_cg_fuse3_OFFSET)) == rev);
}


void CGEN_master_reset(void) 
{
	// Generate SW master reset
	outvl(clkgen_if_MODULE_ADDR + clkgen_if_cg_main_rst_OFFSET, 0xffff0000);	
	PLATFORM_write_buffer_drain();
}

void CGEN_ctrl_reset(uint32 offset, uint32 set_val)
{
	outvl((clkgen_if_MODULE_ADDR+offset), set_val);
	PLATFORM_write_buffer_drain();	
}

void CGEN_ctrl_clk(uint32 offset, uint32 set_val)
{
	outvl((clkgen_if_MODULE_ADDR + offset), set_val);
	PLATFORM_write_buffer_drain();
} 

void CGEN_ctrl_rst_domain_switch(uint32 *array_ptr, uint32 set_val)
{
	int i;

	for(i=0;(array_ptr[i] != END_OF_RST_ARRAY);i++) {
		CGEN_ctrl_reset(array_ptr[i], (0xffff0000 | (set_val & 0xffff)));
		TMR_usec_delay(10);		
	}
}
void CGEN_ctrl_clk_domain_switch(uint32 *array_ptr, uint32 set_val)
{
	int i;

	for(i=0;(array_ptr[i] != END_OF_CLK_ARRAY);i++) {
		CGEN_ctrl_clk(array_ptr[i], (0xffff0000 | (set_val & 0xffff)));
		TMR_usec_delay(50);		
	}
}

void CGEN_ctrl_rst_domain(uint16 set_val) 
{
	CGEN_ctrl_rst_domain_switch(top_rst_domain_array, set_val);
	CGEN_ctrl_rst_domain_switch(npu_rst_domain_array, set_val);
	CGEN_ctrl_rst_domain_switch(rx_rst_domain_array, set_val);
	CGEN_ctrl_rst_domain_switch(tx_rst_domain_array, set_val);
	CGEN_ctrl_rst_domain_switch(fe_rst_domain_array, set_val);
}

void CGEN_ctrl_clk_domain(uint16 set_val) 
{
	CGEN_ctrl_clk_domain_switch(top_clk_domain_array, set_val);
	CGEN_ctrl_clk_domain_switch(npu_clk_domain_array, set_val);
	CGEN_ctrl_clk_domain_switch(rx_clk_domain_array, set_val);
	CGEN_ctrl_clk_domain_switch(tx_clk_domain_array, set_val);
	CGEN_ctrl_clk_domain_switch(fe_clk_domain_array, set_val);
}

// Updates clocks, should be aligned
void __attribute__((noinline)) CGEN_clks_update(uint32 clks) 
{
	outvl(CGEN_CLOCKS_CFG_ADDR, clks);
	PLATFORM_write_buffer_drain();
	CGEN_COUNT_DELAY(CGEN_MIN_DELAY_COUNT);
}


static inline uint32 CGEN_PLL_get_clk(clkgen_cg_pll0_cfg_t pll, uint32 inp_clk)
{
	return (((inp_clk / (pll.bits.pre + 1)) * (pll.bits.mul + 1) * 2) >> pll.bits.post);
}


uint32 CGEN_PLL0_get_clk(uint32 inp_clk)
{
	uint32 out_clk = inp_clk;
	clkgen_cg_pll0_cfg_t	pll0;
	clkgen_cg_clocks_cfg_t	clks;

	clks.word = invl(CGEN_CLOCKS_CFG_ADDR);
	pll0.word = invl(PLL_CFG_ADDR(pll0));

	if (clks.bits.pll0_byp == PLL_IS_USED)
		out_clk = CGEN_PLL_get_clk(pll0, inp_clk);

	return (out_clk);
}


void CGEN_PLL0_init(uint8 pll0_mode, BOOL bypass)
{
	clkgen_cg_pll0_cfg_t	pll0 = {0};
	clkgen_cg_clocks_cfg_t	clks;
	
	clks.word = invl(CGEN_CLOCKS_CFG_ADDR);

	if (clks.bits.pll0_byp == PLL_IS_USED)
	{
		// PLL is in use, first put it in bypass
		clks.bits.pll0_byp		= PLL_IS_BYPASSED;
		clks.bits.pll0_byp_we	= WRITE_ENABLE;
		CGEN_clks_update(clks.word);
	}

	pll0.bits.pre  = pll0_cfg_param_array[pll0_mode].PRE;
	pll0.bits.mul  = pll0_cfg_param_array[pll0_mode].MUL;
	pll0.bits.post = pll0_cfg_param_array[pll0_mode].POST;
	pll0.bits.bs   = pll0_cfg_param_array[pll0_mode].BAND;
	pll0.bits.pd   = CGEN_PLL_POWER_DOWN;

	outvl(PLL_CFG_ADDR(pll0), pll0.word);
	PLATFORM_write_buffer_drain();
	CGEN_COUNT_DELAY(CGEN_MIN_DELAY_COUNT);

	pll0.bits.pd   = CGEN_PLL_ENABLE;
	outvl(PLL_CFG_ADDR(pll0), pll0.word);
	PLATFORM_write_buffer_drain();
	CGEN_COUNT_DELAY(CGEN_MIN_DELAY_COUNT);
		
//	if(bypass)	{	
//		clks.bits.pll0_byp = PLL_IS_BYPASSED;
//	}	
//	else {
		clks.bits.pll0_byp = PLL_IS_USED;
//	}
	clks.bits.pll0_byp_we = WRITE_ENABLE;
	CGEN_clks_update(clks.word);

}

void CGEN_PLL0_bypass(int use_bypass)
{
	clkgen_cg_clocks_cfg_t clks = { 0 };
	clks.word = invl(CGEN_CLOCKS_CFG_ADDR);

	clks.bits.pll0_byp = use_bypass ? PLL_IS_BYPASSED : PLL_IS_USED;
	clks.bits.pll0_byp_we = WRITE_ENABLE;
	CGEN_clks_update(clks.word);
}

void CGEN_PLL1_init(uint8 pll1_mode)
{
	clkgen_cg_pll0_cfg_t	pll1 = {0};
	clkgen_cg_clocks_cfg_t	clks;
	
	clks.word = invl(CGEN_CLOCKS_CFG_ADDR);

	if (clks.bits.pll1_byp == PLL_IS_USED)
	{
		// PLL is in use, first put it in bypass
		clks.bits.pll1_byp		= PLL_IS_BYPASSED;
		clks.bits.pll1_byp_we	= WRITE_ENABLE;
		CGEN_clks_update(clks.word);
	}

	pll1.bits.pre  = pll1_cfg_param_array[pll1_mode].PRE;
	pll1.bits.mul  = pll1_cfg_param_array[pll1_mode].MUL;
	pll1.bits.post = pll1_cfg_param_array[pll1_mode].POST;
	pll1.bits.bs   = pll1_cfg_param_array[pll1_mode].BAND;
	pll1.bits.pd   = CGEN_PLL_POWER_DOWN;

	outvl(PLL_CFG_ADDR(pll1), pll1.word);
	PLATFORM_write_buffer_drain();
	CGEN_COUNT_DELAY(CGEN_MIN_DELAY_COUNT);

	pll1.bits.pd   = CGEN_PLL_ENABLE;
	outvl(PLL_CFG_ADDR(pll1), pll1.word);
	PLATFORM_write_buffer_drain();
	CGEN_COUNT_DELAY(CGEN_MIN_DELAY_COUNT);

	clks.bits.pll1_byp		= PLL_IS_USED;
	clks.bits.pll1_byp_we	= WRITE_ENABLE;
	CGEN_clks_update(clks.word);
}


void CGEN_PLL2_init(uint8 pll2_mode)
{
	clkgen_cg_pll0_cfg_t	pll2 = {0};
	clkgen_cg_clocks_cfg_t	clks;

	clks.word = invl(CGEN_CLOCKS_CFG_ADDR);
	
	if (clks.bits.pll2_byp == PLL_IS_USED)
	{
		// PLL is in use, first put it in bypass
		clks.bits.pll2_byp		= PLL_IS_BYPASSED;
		clks.bits.pll2_byp_we	= WRITE_ENABLE;
		CGEN_clks_update(clks.word);
	}

	pll2.bits.pre  = pll2_cfg_param_array[pll2_mode].PRE;
	pll2.bits.mul  = pll2_cfg_param_array[pll2_mode].MUL;
	pll2.bits.post = pll2_cfg_param_array[pll2_mode].POST;
	pll2.bits.bs   = pll2_cfg_param_array[pll2_mode].BAND;
	pll2.bits.pd   = CGEN_PLL_POWER_DOWN;

	outvl(PLL_CFG_ADDR(pll2), pll2.word);
	PLATFORM_write_buffer_drain();
	CGEN_COUNT_DELAY(CGEN_MIN_DELAY_COUNT);

	clks.bits.div2_slow		= SLOW_CLK_HALF_FAST;
	clks.bits.div2_slow_we	= WRITE_ENABLE;
	clks.bits.pll2_in_sel	= PLL2_IN_PLL3_OUT;
	clks.bits.pll2_in_sel_we	= WRITE_ENABLE;
	CGEN_clks_update(clks.word);

	pll2.bits.pd   = CGEN_PLL_ENABLE;
	outvl(PLL_CFG_ADDR(pll2), pll2.word);
	PLATFORM_write_buffer_drain();
	CGEN_COUNT_DELAY(CGEN_MIN_DELAY_COUNT);

	clks.bits.pll2_byp		= PLL_IS_USED;
	clks.bits.pll2_byp_we	= WRITE_ENABLE;
	CGEN_clks_update(clks.word);
}


void CGEN_PLL3_init(uint8 pll3_mode)
{
	clkgen_cg_pll0_cfg_t	pll3 = {0};
	clkgen_cg_clocks_cfg_t	clks;

	clks.word = invl(CGEN_CLOCKS_CFG_ADDR);

	if (clks.bits.pll3_byp == PLL_IS_USED)
	{
		// PLL is in use, first put it in bypass
		clks.bits.pll3_byp		= PLL_IS_BYPASSED;
		clks.bits.pll3_byp_we	= WRITE_ENABLE;
		CGEN_clks_update(clks.word);
	}
	
	pll3.bits.pre  = pll3_cfg_param_array[pll3_mode].PRE;
	pll3.bits.mul  = pll3_cfg_param_array[pll3_mode].MUL;
	pll3.bits.post = pll3_cfg_param_array[pll3_mode].POST;
	pll3.bits.bs   = pll3_cfg_param_array[pll3_mode].BAND;
	pll3.bits.pd   = CGEN_PLL_POWER_DOWN;

	outvl(PLL_CFG_ADDR(pll3), pll3.word);
	PLATFORM_write_buffer_drain();
	CGEN_COUNT_DELAY(CGEN_MIN_DELAY_COUNT);

	pll3.bits.pd   = CGEN_PLL_ENABLE;
	outvl(PLL_CFG_ADDR(pll3), pll3.word);
	PLATFORM_write_buffer_drain();
	CGEN_COUNT_DELAY(CGEN_MIN_DELAY_COUNT);

	clks.bits.pll3_byp		= PLL_IS_USED;
	clks.bits.pll3_byp_we	= WRITE_ENABLE;
	CGEN_clks_update(clks.word);
}
