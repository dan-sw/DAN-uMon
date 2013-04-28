/* Copyright 2013, Qualcomm Atheros, Inc. */
/*
All files except if stated otherwise in the begining of the file are under the GPLv2 license:
-----------------------------------------------------------------------------------

Copyright (c) 2010-2012 Design Art Networks Ltd.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

-----------------------------------------------------------------------------------
*/
#include "config.h"
#include "stddefs.h"
#include "genlib.h"
#include "cpu.h"
#include "uart16550.h"
#include "CGEN.h"
#include "ral_dw_ddr_ctl.h"
#include "tmr.h"
#include "ddr.h" 
 
char print_buf[128];

void DAN_puts(char *str)
{
	int ix;
	for (ix=0; str[ix]; ix++)
	{
		target_putchar(str[ix]);
	}
}

/*
 * -----------------------------------------------------------
 * SoC_sw_reset - generate SW reset to SoC
 * 				 following HW reset
 * -----------------------------------------------------------
 */
void SoC_sw_reset(void) 
{
	uint32 cgen_reason_reg;
	
	// initialization of wallclock
	TMR_init();
	
	cgen_reason_reg = (RegRead32(clkgen_if_MODULE_ADDR+clkgen_if_cg_rst_reason_OFFSET) & 0xffff); 	
	
	if(cgen_reason_reg == CGEN_REASON_REG_SUCCESSFUL_BOOT) { 
		// Set reason reg bit0 to HIGH
		RegWrite32(clkgen_if_MODULE_ADDR+clkgen_if_cg_rst_reason_OFFSET, (cgen_reason_reg | 0x1));
		
		// Generate SW master reset
		TMR_usec_delay(100000);  // 100mSec  
		CGEN_master_reset();   
	}
}

int SOC_init(void)
{
	uint8 ddr_mode = ddr_mode_get_cfg_ix();
	
    SoC_sw_reset();
	
	// Setup UART pins - IO control patch
	RegWrite32(0xc6124034, 0);
	// Setup GMAC pins - IO control patch
	RegWrite32(0xc612402c, 0);
	RegWrite32(0xc6124030, 0);
	// MDC/MDIO group - IO control patch
	RegWrite32(0xc6124028, 0); // GMAC0/GMAC1/GPIO
	
	// Configure main PLL0 to 600MHz and DDR for 200 MHz
	CGEN_PLL0_init(0, FALSE); 
	CGEN_PLL1_init(2);
	CGEN_PLL3_init(ddr_mode);
	CGEN_PLL2_init(ddr_mode);

	// initialization of wallclock
	TMR_init();

	PLATFORM_write_buffer_drain();
	// Switch Posted write on
	PLATFORM_posted_write_on();
	
	return (0);
}

#define FLUSH_CFG_ELEM(ddr_module_addr, cfg_elem) 							\
	(void)RegRead32((uint32)(ddr_module_addr)+(cfg_elem).offset)

#define WRITE_CFG_ELEM(ddr_module_addr, cfg_elem) 							\
	RegWrite32((uint32)(ddr_module_addr)+(cfg_elem).offset, (cfg_elem).data)

#define	WRITE_FLUSH_CFG_ELEM(ddr_module_addr, cfg_elem) 					\
	{WRITE_CFG_ELEM(ddr_module_addr, cfg_elem);								\
	FLUSH_CFG_ELEM(ddr_module_addr, cfg_elem);} 

#define WRITE_CFG_ELEM_OR(ddr_module_addr, cfg_elem, mask) 					\
	RegWrite32((uint32)(ddr_module_addr)+(cfg_elem).offset, 				\
						(cfg_elem).data | (mask))

#define WRITE_FLUSH_CFG_ELEM_OR(ddr_module_addr, cfg_elem, mask) 			\
	{WRITE_CFG_ELEM_OR(ddr_module_addr, cfg_elem, mask);					\
	FLUSH_CFG_ELEM(ddr_module_addr, cfg_elem);}

#define WRITE_CFG_ELEM_AND(ddr_module_addr, cfg_elem, mask) 				\
	RegWrite32((uint32)(ddr_module_addr)+(cfg_elem).offset, 				\
						(cfg_elem).data & (mask))

#define WRITE_FLUSH_CFG_ELEM_AND(ddr_module_addr, cfg_elem, mask) 			\
	{WRITE_CFG_ELEM_AND(ddr_module_addr, cfg_elem, mask);					\
	FLUSH_CFG_ELEM(ddr_module_addr, cfg_elem);}

char *DDR_clock_str[] = 
{
	"700 MHz",
	"800 MHz",
	"1066 MHz",
	"1333 MHz",
	NULL
};

char *DDR_init_status[] = 
{
	"OK",
	"WARNING!",
	"ERROR!",
 	NULL
};

static const uint32 ddr_base_access_addr[] = 
{
	DDR0_ADDR,
	DDR1_ADDR, 
};

typedef struct ddr_cfg_elem_s
{
	uint32	offset;
	uint32	data;
} ddr_cfg_elem_t;

static const uint32 ddr_base_addr[] =
{
	ddr0_if_MODULE_ADDR,
	ddr1_if_MODULE_ADDR,
};

static const uint32 ddr_reset_addr[] = 
{
	0xe573E4C4,		//DDR0_RST_ADDR
	0xe573E4CC, 	//DDR1_RST_ADDR
};
 

uint8 ddr_mode_get_cfg_ix(void) 
{
	switch (DDR_CLOCK)
	{
	case 700:	return DDR3_700MHZ; 
	case 800:	return DDR3_800MHZ;
	case 1000:	return DDR3_1066MHZ;
	case 1333:	return DDR3_1333MHZ;
	default :	return DDR3_800MHZ;
	}
}

void WDT_init(uint8 timeout)
{
     uint32 data = (WDT_CR_RST_PULSE_LEN_256 | WDT_CR_WDT_EN);
                 
     // set WDT timeout 
     RegWrite32((WDT_BASE + dw_wdt_if_WDT_TORR_OFFSET), timeout);
     // Enable WDT 
     RegWrite32((WDT_BASE + dw_wdt_if_WDT_CR_OFFSET), data);
     WDT_FEED;
}

void ddr_pll0_bypass(int use_bypass) 
{  	 
	CGEN_PLL0_bypass(use_bypass); 
	TMR_usec_delay(50);

	CPU_Init();
	InitUART(DEFAULT_BAUD_RATE);
};   

int ddr_init_verify_short_mem_test(int ddr_module_id, uint32 offset)
{
	uint32 j, b, r, c;
	uint32 ddr_write_addr_val = 0x00000000, ddr_read_addr_val = 0x00000000; 
	uint32 base_addr = ddr_base_access_addr[ddr_module_id];
	uint32 fail_flag = 0;
	
#if defined(SAVE_RESTORE_DDR_CONTEXT)	
	static UINT32 store_buf[DDRDRV_VERIF_TEST_SIZE*DDR_NUM_OF_BANK];
	UINT32 ix=0;

	/* Save DDR context to NPU, stack should be big enough */
	for(b=0; b<DDR_NUM_OF_BANK; b=b+1) {				    									// DDR banks (DDR_NUM_OF_BANK)
		for(j=0; j<DDRDRV_VERIF_TEST_SIZE; j=j+1) {												// DDR test size
			
				c = ( ((j*((0x1<<DDR_NUM_OF_COL_ADDR)/DDRDRV_VERIF_TEST_SIZE)+offset) % 0x3ff));			// 10bits (DDR_NUM_OF_COL_ADDR)
				r = ( ((j*((0x1<<DDR_NUM_OF_ROW_ADDR)/DDRDRV_VERIF_TEST_SIZE)+offset) % 0x3fff));		// 14bits (DDR_NUM_OF_ROW_ADDR)						
				ddr_write_addr_val = base_addr + ((r<<15) | (b<<12) | (c<<2));			
			
				store_buf[ix++] = RegRead32(ddr_write_addr_val);								// Read DDR
		} 
	}

#endif /* SAVE_RESTORE_DDR_CONTEXT */	
	

	//WRITE DDR
	//--------- 
	for(b=0; b<DDR_NUM_OF_BANK; b=b+1) {	 			    									// DDR banks (DDR_NUM_OF_BANK)
		for(j=0; j<DDRDRV_VERIF_TEST_SIZE; j=j+1) {												// DDR test size	
		
				c = ( ((j*((0x1<<DDR_NUM_OF_COL_ADDR)/DDRDRV_VERIF_TEST_SIZE)+offset) % 0x3ff));			// 10bits (DDR_NUM_OF_COL_ADDR)
				r = ( ((j*((0x1<<DDR_NUM_OF_ROW_ADDR)/DDRDRV_VERIF_TEST_SIZE)+offset) % 0x3fff));		// 14bits (DDR_NUM_OF_ROW_ADDR)						
				ddr_write_addr_val = base_addr + ((r<<15) | (b<<12) | (c<<2));			
			
				RegWrite32(ddr_write_addr_val, ddr_write_addr_val);								//Write DDR
		}
	}

	//READ DDR
	//--------
	ddr_write_addr_val = 0x00000000;
		
	for(b=0; b<DDR_NUM_OF_BANK; b=b+1) {				    									// DDR banks (DDR_NUM_OF_BANK)
		for(j=0; j<DDRDRV_VERIF_TEST_SIZE; j=j+1) {												// DDR test size
			
				c = ( ((j*((0x1<<DDR_NUM_OF_COL_ADDR)/DDRDRV_VERIF_TEST_SIZE)+offset) % 0x3ff));			// 10bits (DDR_NUM_OF_COL_ADDR)
				r = ( ((j*((0x1<<DDR_NUM_OF_ROW_ADDR)/DDRDRV_VERIF_TEST_SIZE)+offset) % 0x3fff));		// 14bits (DDR_NUM_OF_ROW_ADDR)						
				ddr_write_addr_val = base_addr + ((r<<15) | (b<<12) | (c<<2));			
			
				ddr_read_addr_val = RegRead32(ddr_write_addr_val);								// Read DDR
									
				if(ddr_read_addr_val != ddr_write_addr_val) {
					fail_flag = (-1);
					break;
				}	
		} 
		if (fail_flag)
			break;
	}

#if defined(SAVE_RESTORE_DDR_CONTEXT)	
	
	// restore DDR context
	//--------- 
	ix = 0;
	for(b=0; b<DDR_NUM_OF_BANK; b=b+1) {	 			    	// DDR banks (DDR_NUM_OF_BANK)
		for(j=0; j<DDRDRV_VERIF_TEST_SIZE; j=j+1) {				// DDR test size	
		
				c = ( ((j*((0x1<<DDR_NUM_OF_COL_ADDR)/DDRDRV_VERIF_TEST_SIZE)+offset) % 0x3ff));			// 10bits (DDR_NUM_OF_COL_ADDR)
				r = ( ((j*((0x1<<DDR_NUM_OF_ROW_ADDR)/DDRDRV_VERIF_TEST_SIZE)+offset) % 0x3fff));		// 14bits (DDR_NUM_OF_ROW_ADDR)						
				ddr_write_addr_val = base_addr + ((r<<15) | (b<<12) | (c<<2));			
			
				RegWrite32(ddr_write_addr_val, store_buf[ix++]);	//Write DDR
		}
	}
#endif /* SAVE_RESTORE_DDR_CONTEXT */
	return (fail_flag);
		
}	//ddr_init_verify_short_mem_test	

int ddr_init_verify_long_mem_test(int ddr_module_id)
{
	uint32 b, r, c; 
	uint32 ddr_write_addr_val = 0x00000000, ddr_read_addr_val = 0x00000000;
	uint32 base_addr = ddr_base_access_addr[ddr_module_id];
	
	//WRITE DDR
	//---------
	for(b=0; b<DDR_NUM_OF_BANK; b=b+1) {				    								// DDR banks (DDR_NUM_OF_BANK)
		for(r=0; r<(0x1<<DDR_NUM_OF_ROW_ADDR); r=r+1) {										// RAWS index	
			for(c=0; c<(0x1<<DDR_NUM_OF_COL_ADDR); c=c+1) {									// COLS index
				
				ddr_write_addr_val = base_addr + (((r & 0x3fff)<<15) | ((b & 0x7)<<12) | ((c & 0x3ff)<<2));			
			
				RegWrite32(ddr_write_addr_val, ddr_write_addr_val);							//Write DDR
			} 
		}
	}
	
	//READ DDR
	//--------
	ddr_write_addr_val = 0x00000000;
	
	for(b=0; b<DDR_NUM_OF_BANK; b=b+1) {				    								// DDR banks (DDR_NUM_OF_BANK)
		for(r=0; r<(0x1<<DDR_NUM_OF_ROW_ADDR); r=r+1) {										// RAWS index	
			for(c=0; c<(0x1<<DDR_NUM_OF_COL_ADDR); c=c+1) {									// COLS index
				
				ddr_write_addr_val = base_addr + (((r & 0x3fff)<<15) | ((b & 0x7)<<12) | ((c & 0x3ff)<<2));			
			
				ddr_read_addr_val = RegRead32(ddr_write_addr_val);							// Read DDR
									
				if(ddr_read_addr_val != ddr_write_addr_val) 
					return (-1);
			} 
		}
	}
	
	return (0);
		
}	//ddr_init_verify_long_mem_test	

void sdram_cmd_mtcl(uint8 ddr_module_id, mtcl_scmd_t command){

	uint32 mtcl_dcr; 
	uint32 ddr_module_addr = ddr_base_addr[ddr_module_id];
	int idx = 0;
	
	mtcl_dcr = RegRead32(ddr_module_addr + dw_ddr_ctl_if_DCR_OFFSET);
	do {
		RegWrite32(ddr_module_addr + dw_ddr_ctl_if_DCR_OFFSET, ( ((0x1<<31) | ((command & 0xf)<<27) | (mtcl_dcr & 0x07ffffff)) & ~(0x1<<16) ) );	// Exec. DDR3 reset (SDRAM)
		TMR_usec_delay(50000);
		sprintf(print_buf, "\n   %x		--> idx = %d		ddr_module_id= %d\n", RegRead32(ddr_module_addr + dw_ddr_ctl_if_DCR_OFFSET), idx, ddr_module_id);
		DAN_puts(print_buf);
		TMR_usec_delay(50000);
		idx++;  
		mtcl_dcr = RegRead32(ddr_module_addr + dw_ddr_ctl_if_DCR_OFFSET);
	} while((mtcl_dcr & 0x80000000) != 0x0);  // wait for the command to be executed
	
}

static inline int check_pub_sm(uint32 ddr_module_addr, uint32 stage)
{
	const uint32 max_wait_count = DDR_SM_TIMEOUT;
	uint32 wait_count; 
    // Poll PUB_SMCTL (Master SM Control Array)
  	// wait for SM Control reach ST2
    for (wait_count = 0; wait_count < max_wait_count; wait_count++)
    {
    	if (RegRead32(ddr_module_addr+dw_ddr_ctl_if_PUB_SMCTL_OFFSET) == stage) {
    		TMR_usec_delay(1);    		
    		break;
    	}	
    }
    if (wait_count >= max_wait_count)  
    	return (-1);
    
    return (0);
}

void set_ddr_pm_reg(uint32 ddr_module_addr, uint32 ddr_pm_offset, uint32 val)
{
	RegWrite32(ddr_module_addr + ddr_pm_offset, val); 
	RegRead32(ddr_module_addr + ddr_pm_offset);
	TMR_usec_delay(1);	
}	//set_ddr_pm_reg 

int ddr_DevTest(uint8 ddr_module_id) 
{
	static const uint32 ddr_mem_base[NUM_OF_DDRS] = {0x00000000, 0x20000000};
	static const uint32 ddr_test_size = CACHE_LINE_SIZE;
	volatile uint32 *ddr_ptr_cached = (void *)(ddr_mem_base[ddr_module_id]);
	volatile uint32 *ddr_ptr_uncached = 
		(void *)(ddr_mem_base[ddr_module_id] | 0x80000000);
	uint32 i;
	
	PLATFORM_cache_control((void *)ddr_ptr_cached, ddr_test_size, 
			CACHE_OP_FLUSH | CACHE_OP_INVALIDATE);
	
	for (i = 0; i < (ddr_test_size / sizeof(uint32)); i++)
	{
		ddr_ptr_cached[i] = i;
	}

	PLATFORM_cache_control((void *)ddr_ptr_cached, ddr_test_size, 
			CACHE_OP_FLUSH | CACHE_OP_INVALIDATE);

	for (i = 0; i < (ddr_test_size / sizeof(uint32)); i++)
	{
		if (ddr_ptr_cached[i] != i)
			return (-1);
	}

	for (i = 0; i < (ddr_test_size / sizeof(uint32)); i++)
	{
		if (ddr_ptr_uncached[i] != i)
			return (-1);
	}

	return (0);
}

int ddr_DevOK(uint8 ddr_module_id) 
{
	return (check_pub_sm(ddr_base_addr[ddr_module_id], 0xffff0000));
} 

void ddr_DevReset(uint8 ddr_module_id)
{
	// Assert Resets
	RegWrite32(ddr_reset_addr[ddr_module_id], 0xFFFF0000);
	RegRead32(ddr_reset_addr[ddr_module_id]);
	TMR_usec_delay(1);	
	// Release Resets
	RegWrite32(ddr_reset_addr[ddr_module_id], 0xFFFFFFFF);
	RegRead32(ddr_reset_addr[ddr_module_id]);
	TMR_usec_delay(1);	
}

void ddr_periodic_updates(uint32 ddr_module_addr, uint32 ddr_pm_offset, uint32 enable, uint32 period)
{
	uint32 reg_value = 0x00000000;
	
	reg_value = (enable & 0x1)<<31 | (period & 0x3fff);
	set_ddr_pm_reg(ddr_module_addr, ddr_pm_offset, reg_value);
	
	return; 
}

int ddr_DevInit(uint8 ddr_module_id, uint32 do_verify_mem_test ,ddr_config_t cfg, ddr_temperature_t temp)
{
	uint32 i, ddr_module_addr = ddr_base_addr[ddr_module_id];
	uint32 pub_smctl_state, pub_reg_state, state_timeout_cnt = 0;
			
    //=====================
	// DDR Driver paramters
	//=====================
	static const uint32 ddr_drlcfg_en[DDR_NUM_TYPES] = 			//PUB_DRLCFG enable
	{
		0,								// DDR3_700MHZ			
		0,								// DDR3_800MHZ
		1,								// DDR3_1066MHZ
		0								// DDR3_1333MHZ
	};
	static const uint32 ddr_drlcfg[DDR_NUM_TYPES] = 			//PUB_DRLCFG value
	{
		0x00000000,		 				// DDR3_700MHZ			
		0x00000000,						// DDR3_800MHZ
		0x000016db,						// DDR3_1066MHZ
		0x00000000						// DDR3_1333MHZ
	};
	static const uint32 ddr_lcdlr_en[DDR_NUM_TYPES] = 			// PHY_DXnLCDLR2 Enable
	{
		0,								// DDR3_700MHZ
		0,								// DDR3_800MHZ
		1,								// DDR3_1066MHZ
		1								// DDR3_1333MHZ
	};
	static const uint32 ddr_lcdlr[DDR_NUM_TYPES][NUM_OF_DDRS] =  // PHY_DXnLCDLR2 value 
	{
		{0x00000000, 0x00000000},		// DDR3_700MHZ			
		{0x00000000, 0x00000000},		// DDR3_800MHZ
		{0x00000067, 0x00000074},		// DDR3_1066MHZ
		{0x00000037, 0x0000001a}		// DDR3_1333MHZ
	};
	
	static const ddr_cfg_elem_t ddr_cfg_refresh_reg[DDR_NUM_TYPES][DDR3_TEMP_MAX] = //MCTL_DRR
	{
		{
			{0x010, 0x20bb4840},			// DDR3_700MHZ DDR3_TEMP_NORMAL
			{0x010, 0x20bb4880},			// DDR3_700MHZ DDR3_TEMP_HIGH 
		},			
		{
			{0x010, 0x20d6b040},			// DDR3_800MHZ DDR3_TEMP_NORMAL
			{0x010, 0x20d6b080},			// DDR3_800MHZ DDR3_TEMP_HIGH 
		},
		{
			{0x010, 0x210d8856},			// DDR3_1066MHZ DDR3_TEMP_NORMAL 
			{0x010, 0x210d8856},			// DDR3_1066MHZ DDR3_TEMP_HIGH 
		},
		{
			{0x010, 0x2168f083},			// DDR3_1333MHZ DDR3_TEMP_NORMAL
			{0x010, 0x2168f083},			// DDR3_1333MHZ DDR3_TEMP_HIGH
		}
	};
	
	static const ddr_cfg_elem_t ddr_cfg_mode_regs[DDR_NUM_TYPES][5] = 	// MODE registers, MCTL, PUB
	{
		{
			{0x1f0, 0x00000420},			// DDR3_700MHZ MR0
			{0x1f4, 0x00000042},			// DDR3_700MHZ MR1
			{0x48c, 0x00000042},			// DDR3_700MHZ PUB_MR1
			{0x044, 0x00010000},			// DDR3_700MHZ ODTCR
			{0x1fc, 0x00000000}				// DDR3_700MHZ MR3	
		},
		{
			{0x1f0, 0x00000420},			// DDR3_800MHZ MR0
			{0x1f4, 0x00000042},			// DDR3_800MHZ MR1
			{0x48c, 0x00000042},			// DDR3_800MHZ PUB_MR1
			{0x044, 0x00010000},			// DDR3_800MHZ ODTCR
			{0x1fc, 0x00000000}				// DDR3_800MHZ MR3				
		},
		{
			{0x1f0, 0x00000830},			// DDR3_1066MHZ MR0
			{0x1f4, 0x00000042},			// DDR3_1066MHZ MR1
			{0x48c, 0x00000042},			// DDR3_1066MHZ PUB_MR1
			{0x044, 0x00010000},			// DDR3_1066MHZ ODTCR
			{0x1fc, 0x00000000}				// DDR3_1066MHZ MR3				
		},
		{
			{0x1f0, 0x00000a50},			// DDR3_1333MHZ MR0
			{0x1f4, 0x00000042},			// DDR3_1333MHZ MR1
			{0x48c, 0x00000042},			// DDR3_1333MHZ PUB_MR1
			{0x044, 0x00010000},			// DDR3_1333MHZ ODTCR
			{0x1fc, 0x00000000}				// DDR3_1333MHZ MR3				
		}
	};
	
	static const ddr_cfg_elem_t ddr_cfg_MR2_reg[DDR_NUM_TYPES][DDR3_TEMP_MAX] =		// MCTL_MR2 
	{
		{
			{0x1f8, 0x00000000},			// DDR3_700MHZ MR2 DDR3_TEMP_NORMAL
			{0x1f8, 0x00000040},			// DDR3_700MHZ MR2 DDR3_TEMP_HIGH
		},			
		{
			{0x1f8, 0x00000000},			// DDR3_800MHZ MR2 DDR3_TEMP_NORMAL
			{0x1f8, 0x00000040},			// DDR3_800MHZ MR2 DDR3_TEMP_HIGH
		},
		{
			{0x1f8, 0x00000008},			// DDR3_1066MHZ MR2 DDR3_TEMP_NORMAL
			{0x1f8, 0x00000008},			// DDR3_1066MHZ MR2 DDR3_TEMP_HIGH
		},
		{
			{0x1f8, 0x00000018},			// DDR3_1333MHZ MR2 DDR3_TEMP_NORMAL
			{0x1f8, 0x00000018},			// DDR3_1333MHZ MR2 DDR3_TEMP_HIGH
		}
	};

	static const ddr_cfg_elem_t ddr_cfg_timing_params[DDR_NUM_TYPES][5] =		// MCTL_TPR0-4
	{
		{
			{0x014, 0xbe80881a},		// DDR3_700MHZ TPR0
			{0x018, 0x003528cc},		// DDR3_700MHZ TPR1
			{0x01c, 0x000a2e00},		// DDR3_700MHZ TPR2
			{0x020, 0x003002b2},		// DDR3_700MHZ TPR3
			{0x058, 0x00001b07}			// DDR3_700MHZ TPR4
		},			
		{
			{0x014, 0xbe80881a},		// DDR3_800MHZ TPR0
			{0x018, 0x003528cc},		// DDR3_800MHZ TPR1
			{0x01c, 0x000a2e00},		// DDR3_800MHZ TPR2
			{0x020, 0x003002b2},		// DDR3_800MHZ TPR3
			{0x058, 0x00001b07}			// DDR3_800MHZ TPR4
		},
		{
			{0x014, 0xc6c0991a},		// DDR3_1066MHZ TPR0
			{0x018, 0x003528e4},		// DDR3_1066MHZ TPR1
			{0x01c, 0x00053200},		// DDR3_1066MHZ TPR2
			{0x020, 0x0040033a},		// DDR3_1066MHZ TPR3
			{0x058, 0x00001d07}			// DDR3_1066MHZ TPR4
		},
		{
			{0x014, 0xd0a0bb1b},		// DDR3_1333MHZ TPR0
			{0x018, 0x004528fc},		// DDR3_1333MHZ TPR1
			{0x01c, 0x0002c200},		// DDR3_1333MHZ TPR2
			{0x020, 0x0050044a},		// DDR3_1333MHZ TPR3
			{0x058, 0x00001906}			// DDR3_1333MHZ TPR4
		}
	};
	static const uint32 mode_elem_num = sizeof(ddr_cfg_mode_regs[cfg]) / 
									sizeof(ddr_cfg_mode_regs[cfg][0]);
	static const uint32 timing_elem_num = sizeof(ddr_cfg_timing_params[cfg]) / 
									sizeof(ddr_cfg_timing_params[cfg][0]);

//================================================================================================		
//=============================DAN SW DDR Driver Start============================================	
//================================================================================================
/*
    if(do_verify_mem_test)	{	
		RegWrite32(ddr_reset_addr[ddr_module_id], 0xfffffffc); 						// DDR MCTL and PHY Reset
		TMR_usec_delay(1);		
		RegWrite32(ddr_reset_addr[ddr_module_id], 0xffffffff);						// DDR PHY  activate
		TMR_usec_delay(5);		
	}
*/
	////////////////////////////
	// DAN SW DRV - DDRDRV_INIT
	////////////////////////////
    pub_smctl_state = RegRead32(ddr_module_addr + dw_ddr_ctl_if_PUB_SMCTL_OFFSET);		
    while (pub_smctl_state != 0x00010000)											// PUB_SMCTL - didn't wait in ST1 (finish ST0 - phy init.)
    {
	   if (pub_smctl_state != 0x00000000) {								
		   ddr_DevReset(ddr_module_id); 
/*
           RegWrite32(ddr_reset_addr[ddr_module_id], 0xfffffffc); 					// PHY MCTL and PHY Reset
   		   TMR_usec_delay(1);		
           RegWrite32(ddr_reset_addr[ddr_module_id], 0xffffffff);
   		   TMR_usec_delay(5);		           
*/
      }
	  TMR_usec_delay(1);			  
      pub_smctl_state = RegRead32(ddr_module_addr + dw_ddr_ctl_if_PUB_SMCTL_OFFSET);
    }
    
	// Verify (ST0) PHY INIT. DONE
	//----------------------------
	// PHY_PGSR0 - PHY Init. DONE --> bits[2:0]   = '111' - INIT, PLLINT, CAL
	// PHY_ZQ0SR0 - ZQ calibration DONE, ERR --> bits[31:30] =  '10' - ZDONE, ZERR
	if (((RegRead32(ddr_module_addr + dw_ddr_ctl_if_PHY_PGSR0_OFFSET)  & 0x00000007) != 0x00000007) & \
	    ((RegRead32(ddr_module_addr + dw_ddr_ctl_if_PHY_ZQ0SR0_OFFSET) & 0xc0000000) != 0x80000000)) {
			return (-1);
    }		
	// Verify (ST0) MTCL update with PHY INIT. DONE
	//---------------------------------------------
	if((RegRead32(ddr_module_addr + dw_ddr_ctl_if_CSR_OFFSET) & 0x0000000f) != 0x0000000f) {	// MTCL_CSR - MCTL CSR.[3:0] --> bits = '1111' -
		return (-1);																// PHY CALDONE / ZCALDONE / PLL_INIT_DONE / INIT_DONE	
    }    
    
	/////////////////////////
	// DDR DRV Configuration
	/////////////////////////
  	WRITE_FLUSH_CFG_ELEM_OR(ddr_module_addr, ddr_cfg_refresh_reg[cfg][temp], (1<<31));			// MCTL_DRR - (DRAM Refresh Register) --> Disable refresh
	TMR_usec_delay(1);			

	for (i = 0; i < mode_elem_num; i++){
  		WRITE_FLUSH_CFG_ELEM(ddr_module_addr, ddr_cfg_mode_regs[cfg][i]);			// MCTL_MR0-3, PUB_MR1 - SDRAM Mode registers				
		TMR_usec_delay(1);			
	}
	
	WRITE_FLUSH_CFG_ELEM(ddr_module_addr, ddr_cfg_MR2_reg[cfg][temp]);				// MCTL_MR2 - SDRAM Mode registers, temperature update				
	TMR_usec_delay(1);			

	for (i = 0; i < timing_elem_num; i++){
  		WRITE_FLUSH_CFG_ELEM(ddr_module_addr, ddr_cfg_timing_params[cfg][i]);		// MCTL_TPR0-3 - SDRAM Timing
		TMR_usec_delay(1);			
	}

	set_ddr_pm_reg(ddr_module_addr, dw_ddr_ctl_if_DCR_OFFSET, 0x000041c5);			// MCTL_DCR - DO_INIT[16] (DRAM Configuration Register)
																					// 			  config: DRAM I/O width: x8; DRAM Chip Density: 1Gb; 
																					// 					  ECC = DISABLED;
																					//					  SDRAM System I/O Width: 32 bits; AMAP = 01)	
	set_ddr_pm_reg(ddr_module_addr, dw_ddr_ctl_if_PHY_PGCR1_OFFSET, 0x0000f082);	// PHY_PGCR1 - Setting WLSTEP Write Leveling Step -
																					//				WLSTEP = 32 step size, IODDRM 01 [8:7]	
																					//				WLSTEP = 1 step size (0x0000f086)
																																										
	set_ddr_pm_reg(ddr_module_addr, dw_ddr_ctl_if_PHY_ZQ0CR1_OFFSET, 0x0000003b);   // PHY_ZQ0CR1 - ZQ Calbiration - ST0 (PHY_ZQ0CR1.ZPROG)         
																					// 				0x0000007b --> ODT= 60ohm, ZO=40ohm 
																					//				0x000000bb --> ODT= 40ohm, ZO=40ohm 
																 					//				0x0000003b --> ODT=120ohm, ZO=40ohm 
																 					//				0x0000003d --> ODT=120ohm, ZO=34ohm 
																 					//				0x0000007d --> ODT= 60ohm, ZO=34ohm 
																 					//				0x000000bd --> ODT= 40ohm, ZO=34ohm 
																 					
	set_ddr_pm_reg(ddr_module_addr, dw_ddr_ctl_if_PHY_PIR_OFFSET, 0x0100fe40); 		// PHY_PIR - Setting LP filter depth to higher value of 16 PIR register
																					// 			 ENABLE Msater delay line MDLEN 		

	set_ddr_pm_reg(ddr_module_addr, dw_ddr_ctl_if_PHY_DX0GCR_OFFSET, 0x00000115);	// Enable PHY_DX0GCR.MDLEN, combined with DXCCR.MDLEN (PHY_DX0GCR0 Register) 
	set_ddr_pm_reg(ddr_module_addr, dw_ddr_ctl_if_PHY_DX1GCR_OFFSET, 0x00000115);	// Enable PHY_DX1GCR.MDLEN, combined with DXCCR.MDLEN (PHY_DX1GCR0 Register) 
	set_ddr_pm_reg(ddr_module_addr, dw_ddr_ctl_if_PHY_DX2GCR_OFFSET, 0x00000115);	// Enable PHY_DX2GCR.MDLEN, combined with DXCCR.MDLEN (PHY_DX2GCR0 Register) 
	set_ddr_pm_reg(ddr_module_addr, dw_ddr_ctl_if_PHY_DX3GCR_OFFSET, 0x00000115);	// Enable PHY_DX3GCR.MDLEN, combined with DXCCR.MDLEN (PHY_DX3GCR0 Register) 

	// PUB_FSM - 'Exe' ST1(SDRAM INIT.), Wait ST2
	//===========================================
	set_ddr_pm_reg(ddr_module_addr, dw_ddr_ctl_if_PUB_SMCTL_OFFSET, 0x00010004);	
	
	// Initiate SDRAM init.
	//=====================	
	set_ddr_pm_reg(ddr_module_addr, dw_ddr_ctl_if_DCR_OFFSET, 0x000141c5);			// MCTL_DCR - DO_INIT[16] (DRAM Configuration Register)
	TMR_usec_delay(10);																// 		config: DRAM I/O width: x8;DRAM Chip Density: 1Gb; 
																					// 				ECC = DISABLED; I/O Width: 32 bits; AMAP = 01
	state_timeout_cnt = 0;
 	pub_smctl_state = RegRead32(ddr_module_addr + dw_ddr_ctl_if_PUB_SMCTL_OFFSET);
	while (pub_smctl_state != 0x00030004)
	{
		if(state_timeout_cnt==DDR_SM_TIMEOUT) { 	
			return (-1);
		}			
		else {
			state_timeout_cnt++;
			TMR_usec_delay(1);			
			pub_smctl_state = RegRead32(ddr_module_addr + dw_ddr_ctl_if_PUB_SMCTL_OFFSET);			
		}	
	} 
	
	// Verify (ST1) SDRAM INIT DONE
	//-----------------------------
	state_timeout_cnt = 0;
	pub_reg_state = RegRead32(ddr_module_addr + dw_ddr_ctl_if_CSR_OFFSET) & 0x00040000;		// MCTL_CSR - MCTL_CSR.INIT bit - If de-asserted to LOW:
	while (pub_reg_state != 0x00000000)														//             PHY initialization and SDRAM initialization DONE
	{
		if(state_timeout_cnt==DDR_SM_TIMEOUT) { 	
			return (-1);
		}			
		else {
			state_timeout_cnt++;
			TMR_usec_delay(1);			
			pub_reg_state = RegRead32(ddr_module_addr + dw_ddr_ctl_if_CSR_OFFSET) & 0x00040000;			
		}	
	}

	set_ddr_pm_reg(ddr_module_addr, dw_ddr_ctl_if_DCR_OFFSET, 0x000041c5);			// MCTL_DCR - DO_INIT[16] (DRAM Configuration Register)
																					// 		config: DRAM I/O width: x8;DRAM Chip Density: 1Gb; 
																					// 				ECC = DISABLED; I/O Width: 32 bits; AMAP = 01
	/////////////////////////
	// DDRDRV_INIT_TRAINING
	////////////////////////
	// PUB_FSM - Exe ST2, Wait ST3	
	//============================
	set_ddr_pm_reg(ddr_module_addr, dw_ddr_ctl_if_PUB_SMCTL_OFFSET, 0x00030008);	
	
  	if (check_pub_sm(ddr_module_addr, 0x00070008) < 0)  {
  		return (-1);
  	}  		
	TMR_usec_delay(1);			

	// Verify ST2 DONE
	//----------------- 
	state_timeout_cnt = 0;
	pub_reg_state = RegRead32(ddr_module_addr+dw_ddr_ctl_if_PHY_PIR_OFFSET) & 0x10000000;	// PHY_PIR - PHY_PIR.INHVT bit - assert HIGH -
	while (pub_reg_state != 0x10000000)														//           Verify PUB SM inhibits VT Compensation
	{
		if(state_timeout_cnt==DDR_SM_TIMEOUT) { 	
			return (-1);
		}			
		else {
			state_timeout_cnt++; 
			TMR_usec_delay(1);			
			pub_reg_state = RegRead32(ddr_module_addr+dw_ddr_ctl_if_PHY_PIR_OFFSET) & 0x10000000;			
		}	
	}	

	//////////////
	// DDRDRV_WL1
	//////////////
	set_ddr_pm_reg(ddr_module_addr, dw_ddr_ctl_if_PUB_SLCR_OFFSET, 0x00030300);		// PUB_SLCR   - qs_alen=1 extending qs gate , one cycle asserted on DQS gating
	
	set_ddr_pm_reg(ddr_module_addr, dw_ddr_ctl_if_PHY_DXCCR_OFFSET, 0x00001ee5);	// PHY_DXCCR  - Overwriting DQSRES to 344 Ohms - DXODT enable during PHY_INIT  							

	set_ddr_pm_reg(ddr_module_addr, dw_ddr_ctl_if_PHY_PIR_OFFSET, 0x1100fe40);		// PHY_PIR - (Redandent) PHY_PIR.INHVT bit - assert HIGH 
	
	set_ddr_pm_reg(ddr_module_addr, dw_ddr_ctl_if_CCR_OFFSET, 0x00004000);			// MCTL_CCR.DQTUNE - one cycle more following the read burst
	
	set_ddr_pm_reg(ddr_module_addr, dw_ddr_ctl_if_PPER3_OFFSET, 0x00000000);		// MCTL_PPER3 - Disabling On-the-Fly VT updates
	
	set_ddr_pm_reg(ddr_module_addr, dw_ddr_ctl_if_PUB_PPMCFG_OFFSET, 0x00001df0);  	// PUB_PPMCFG - Rank 0 - Enabled, Byte Lanes 0,1,2,3 ONLY - Enabled,
																					// 						 ECC lane (4) + Rest Lanes - Disabled
	// PUB_FSM - Exe ST3,ST5, Wait ST6	
	//================================
	set_ddr_pm_reg(ddr_module_addr, dw_ddr_ctl_if_PUB_SMCTL_OFFSET, 0x00170040);
	
  	if (check_pub_sm(ddr_module_addr, 0x003f0040) < 0)	{
  		return (-1);
  	}
	
	// Verify ST3 DONE - WL1 Done
	//---------------------------
	// PHY_DXnGSR0 - WL1 Done when all DATX8 DONE without errors 
	//               {PHY_DXnGSR0.WLERR, PHY_DXnGSR0.WLDONE} = '01'
	if (((RegRead32(ddr_module_addr + dw_ddr_ctl_if_PHY_DX0GSR0_OFFSET) & 0x00000060) != 0x00000020) & 	\
	    ((RegRead32(ddr_module_addr + dw_ddr_ctl_if_PHY_DX1GSR0_OFFSET) & 0xc0000060) != 0x80000020) & 	\
	    ((RegRead32(ddr_module_addr + dw_ddr_ctl_if_PHY_DX2GSR0_OFFSET) & 0xc0000060) != 0x80000020) & 	\
	    ((RegRead32(ddr_module_addr + dw_ddr_ctl_if_PHY_DX3GSR0_OFFSET) & 0xc0000060) != 0x80000020)) {
			return (-1);
	}				
	 
	// PUB_DRLCFG Correcting if needed
	if (ddr_drlcfg_en[cfg]){
		set_ddr_pm_reg(ddr_module_addr, dw_ddr_ctl_if_PUB_DRLCFG_0_OFFSET, ddr_drlcfg[cfg]);				
	}
	// PHY_DXnLCDLR2 - correcting if needed
	if (ddr_lcdlr_en[cfg]){
		set_ddr_pm_reg(ddr_module_addr, dw_ddr_ctl_if_PHY_DX0LCDLR2_OFFSET, ddr_lcdlr[cfg][ddr_module_id]);						
		set_ddr_pm_reg(ddr_module_addr, dw_ddr_ctl_if_PHY_DX1LCDLR2_OFFSET, ddr_lcdlr[cfg][ddr_module_id]);						
		set_ddr_pm_reg(ddr_module_addr, dw_ddr_ctl_if_PHY_DX2LCDLR2_OFFSET, ddr_lcdlr[cfg][ddr_module_id]);						
		set_ddr_pm_reg(ddr_module_addr, dw_ddr_ctl_if_PHY_DX3LCDLR2_OFFSET, ddr_lcdlr[cfg][ddr_module_id]);						
	}
	
	///////////////////
	// DDRDRV_TRAINING
	///////////////////	
	// PUB_FSM - Exe ST6-ST15
	//=======================
	set_ddr_pm_reg(ddr_module_addr, dw_ddr_ctl_if_PUB_SMCTL_OFFSET, 0x647f0000);	
	
  	if (check_pub_sm(ddr_module_addr, 0xffff0000) < 0)	{
  		return (-1);
  	}	
	TMR_usec_delay(1);		
	
	//////////////
	//DDRDRV_DONE
	//////////////	 
	set_ddr_pm_reg(ddr_module_addr, dw_ddr_ctl_if_PHY_PIR_OFFSET, 0x0100fe40); 		// PHY_PIR  - Enable VT Compensation feature

  	WRITE_FLUSH_CFG_ELEM(ddr_module_addr, ddr_cfg_refresh_reg[cfg][temp]);			// MCTL_DRR - (DRAM Refresh Register) --> Enable periodic refreshs
	TMR_usec_delay(1);			


	set_ddr_pm_reg(ddr_module_addr, dw_ddr_ctl_if_CCR_OFFSET, 0x00000004);			// MCTL_CCR.HOSTEN - (Controller Configuration Register), HOST - Enable
	TMR_usec_delay(1);
	
	/////////////  
	//DDRDRV OPR
	/////////////	

	if(do_verify_mem_test) { 
		if(ddr_init_verify_short_mem_test(ddr_module_id, DDRDRV_VERIF_TEST_SIZE/2) < 0) {			// DDR - Short memory test	
			return(-1);
		}  
	}			
	
	return (0);	
}

char *ddr_init_help[] = {
    "Initialize DDR controller",   
    "<0..1>",
    0,
};  

int ddr_init(int dev, uint32 do_verify_mem_test, ddr_config_t cfg, ddr_temperature_t temp)
{
	int max_tries = 0;

#ifndef DAN2400
	
		if ((dev >=0) && (dev <= 1)) 
		{
			do
			{
				ddr_DevReset(dev); 
				if (ddr_DevInit(dev, do_verify_mem_test, cfg, DDR3_TEMP_NORMAL) == 0)
				{
					break;
				}
				max_tries++;				
				TMR_usec_delay(1);			
			}
			while (max_tries < DDR_REPETITION_NUM);  
		}
#endif
	return (max_tries); 
}
 
// DDR initalization sequence can't use via. CLI command.
int ddr_init_cli(int argc, char *argv[]) 
{

	printf("\nddr_init_cli - NOT IMPLEMENTED \n");

	return (0);
}

void DDR_init_all(void)
{
	int ddr0_tries = 0;
	int ddr1_tries = 0;
	ddr_status_t stat0 = DDR_STAT_OK;
	ddr_status_t stat1 = DDR_STAT_OK; 
	ddr_config_t ddr_cfg = ddr_mode_get_cfg_ix();

	// SoC Modules Init.
	//================== 
	SOC_init();  
	CPU_Init(); 
	InitUART(DEFAULT_BAUD_RATE);  
	
	DAN_puts("\r\nBooting...\r\n");

	// DDR DRIVER 
	//===========
	CGEN_ctrl_clk_domain(CEN_DIS);												// Disable Unused PLL0 Clock domain	
	
	// Voltage drop dummy init. 
	//-------------------------
//	WDT_init(WDT_TIME_OUT_DDR_600); 
//	ddr_DevReset(0, TRUE);	
//	ddr_DevReset(1, TRUE);	
//	ddr_DevInit(1, FALSE, ddr_cfg, DDR3_TEMP_NORMAL);							// DUMMY INIT(1) 
	
	// Set PLL0 --> 135MHz   
	//-------------------
//	ddr_pll0_bypass(TRUE); 	
	CGEN_PLL0_init(1, FALSE);	 												
	CPU_Init();
	InitUART(DEFAULT_BAUD_RATE);
	
#ifdef USE_INIT_DDR0
	WDT_init(WDT_TIME_OUT_DDR_40);   
	
	ddr0_tries = ddr_init(0, TRUE, ddr_cfg, DDR3_TEMP_NORMAL);    
	
	if (ddr0_tries >= DDR_REPETITION_NUM)
		stat0 = DDR_STAT_ERROR;
	else if (ddr0_tries == 0)
		stat0 = DDR_STAT_OK;
	else 
		stat0 = DDR_STAT_WARNING; 
	
	if(stat0 == DDR_STAT_ERROR) { 
		CGEN_master_reset();		
	}
	
#endif	
	
#ifdef USE_INIT_DDR1
	WDT_init(WDT_TIME_OUT_DDR_40); 
	
	ddr1_tries = ddr_init(1, TRUE, ddr_cfg, DDR3_TEMP_NORMAL);
	
	if (ddr1_tries >= DDR_REPETITION_NUM)
		stat1 = DDR_STAT_ERROR;
	else if (ddr1_tries == 0)
		stat1 = DDR_STAT_OK;
	else
		stat1 = DDR_STAT_WARNING;
	
	if(stat1 == DDR_STAT_ERROR)  {
		CGEN_master_reset();				
	}
	
#endif	 

	// Set PLL0 --> 600MHz 
	//--------------------- 
//	ddr_pll0_bypass(FALSE);   	
	CGEN_PLL0_init(0, FALSE);	
	CPU_Init();
	InitUART(DEFAULT_BAUD_RATE);	
	
	// Enable VT periodic compensation  
	//--------------------------------
	WDT_init(WDT_TIME_OUT_DDR_600);	
#ifdef USE_INIT_DDR0
//	ddr_periodic_updates(ddr0_if_MODULE_ADDR, dw_ddr_ctl_if_PPER0_OFFSET, TRUE, 0x4);	// MCTL_PPER0 - DISABLE periodic ZQ updates
//	ddr_periodic_updates(ddr0_if_MODULE_ADDR, dw_ddr_ctl_if_PPER1_OFFSET, TRUE, 0x40);	// MCTL_PPER1 - DISABLE periodic ZQCS updates
 	ddr_periodic_updates(ddr0_if_MODULE_ADDR, dw_ddr_ctl_if_PPER2_OFFSET, TRUE, 0x6);	// MCTL_PPER2 - Enabling periodic VT updates
#endif
	
#ifdef USE_INIT_DDR1
//	ddr_periodic_updates(ddr1_if_MODULE_ADDR, dw_ddr_ctl_if_PPER0_OFFSET, FALSE, 0x4);	// MCTL_PPER0 - DISABLE periodic ZQ updates
//	ddr_periodic_updates(ddr1_if_MODULE_ADDR, dw_ddr_ctl_if_PPER1_OFFSET, FALSE, 0x40);	// MCTL_PPER1 - DISABLE eriodic ZQCS updates
	ddr_periodic_updates(ddr1_if_MODULE_ADDR, dw_ddr_ctl_if_PPER2_OFFSET, TRUE,  0x6);	// MCTL_PPER2 - Enabling periodic VT updates
#endif
	
	// MEM TEST
#ifdef USE_INIT_DDR0  
	WDT_init(WDT_TIME_OUT_DDR_600);

	if(ddr_init_verify_short_mem_test(0, 0x0) < 0) {		// DDR0 Memory test
		CGEN_master_reset();													// Generate SW master reset		
	}
	
#endif
#ifdef USE_INIT_DDR1
	WDT_init(WDT_TIME_OUT_DDR_600);
	
	if(ddr_init_verify_short_mem_test(1, 0x0) < 0) {		// DDR1 Memory test
		CGEN_master_reset();													// Generate SW master reset		
	}
	
#endif
	
	CGEN_ctrl_clk_domain(CEN_EN);												// Gradual Enable of PLL0 Clocks domain
	
 }
