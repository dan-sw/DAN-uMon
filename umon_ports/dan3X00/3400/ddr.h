
//DEBUG FLAGS
//#define USE_DBG_GPIO
//#define UART_PUTS_EN
#define WDT_USE_EN

// USED DDR'S
#define NUM_OF_DDRS					2
#define USE_INIT_DDR0
#define USE_INIT_DDR1

#define ddr0_if_MODULE_ADDR     	0xe5044000
#define ddr1_if_MODULE_ADDR     	0xe5046000

#define DDR_SIZE  					0x20000000
#define DDR0_ADDR 					0x80000000
#define DDR0_SIZE 					0x20000000
#define DDR1_ADDR 					0xa0000000
#define DDR1_SIZE 					0x20000000

#define DDR_REPETITION_NUM			16

//HW WDT define offsets
#define NPU_TOP_BASE       		    (0xE5732000)
#define WDT_BASE       		        (0xE576E000)
#define npu_top_if_NPU_TOP_WDT_OFFSET  (0xf0)

#define dw_wdt_if_WDT_CR_OFFSET     (0x0)
#define dw_wdt_if_WDT_TORR_OFFSET 	(0x4)
#define dw_wdt_if_WDT_CCVR_OFFSET   (0x8)
#define dw_wdt_if_WDT_CRR_OFFSET   	(0xc)
#define dw_wdt_if_WDT_STAT_OFFSET  	(0x10)

#define  WDT_CR_WDT_DIS           	0x00000000  /* wdt disabled */
#ifdef WDT_USE_EN
#define  WDT_CR_WDT_EN            	0x00000001  /* wdt enabled */
#else
#define  WDT_CR_WDT_EN            	WDT_CR_WDT_DIS
#endif
#define  WDT_TIME_OUT_DDR_40        0x00000009  /* TODO: change to real value [7:4] - LOW [3:0]- HIGH*/
#define  WDT_TIME_OUT_DDR_600       0x0000000c  /* TODO: change to real value */
#define  WDT_TIME_OUT_UMON_600      0x0000000B  /* TODO: change to real value */
#define  WDT_CR_RST_PULSE_LEN_256 	0x000001C0  /* 256 pclk to reset */

#define WDT_PAUSE(_p)        		RegWrite32((NPU_TOP_BASE + npu_top_if_NPU_TOP_WDT_OFFSET), (0x1<<16 | (_p & 0x1)))
#define WDT_FEED             		RegWrite32((WDT_BASE + dw_wdt_if_WDT_CRR_OFFSET), 0x76)
#define WDT_CCVR			 		RegRead32(WDT_BASE + dw_wdt_if_WDT_CCVR_OFFSET)

#define DDR_NUM_OF_BANK			 	8 
#define DDR_NUM_OF_COL_ADDR			10 
#define DDR_NUM_OF_ROW_ADDR			14
#define DDRDRV_VERIF_TEST_SIZE  	0x200				//max. size0x400 

#define DDR_SM_TIMEOUT				100000

#define DW_GPIO_1_BASE          	0xc6126000
#define GPIO_SWPORTA_DR_OFFSET    	0x00
#define GPIO_SWPORTA_DDR_OFFSET   	0x04
#define GPIO_SWPORTA_CTL_OFFSET   	0x08
#define GPIO_SWPORTA_DR 			(DW_GPIO_1_BASE + GPIO_SWPORTA_DR_OFFSET)
#define GPIO_SWPORTA_DDR 			(DW_GPIO_1_BASE + GPIO_SWPORTA_DDR_OFFSET)
#define GPIO_SWPORTA_CTL 			(DW_GPIO_1_BASE + GPIO_SWPORTA_CTL_OFFSET)    

#ifdef UART_PUTS_EN
extern char print_buf[]; 
#define UART_puts(_s) DAN_puts(_s)
#else
#define UART_puts(_s) {;}
#endif // UART_PUTS_EN

typedef enum 
{
	DDR3_700MHZ,	 
	DDR3_800MHZ,
	DDR3_1066MHZ,
	DDR3_1333MHZ,
	DDR_NUM_TYPES 
} ddr_config_t;

typedef enum 
{
	DDR3_TEMP_NORMAL = 0,
	DDR3_TEMP_HIGH,
	DDR3_TEMP_MAX,
} ddr_temperature_t;

typedef enum 
{
	DDR_STAT_OK = 0,
	DDR_STAT_WARNING,
	DDR_STAT_ERROR,
} ddr_status_t;

typedef enum 
{
	DDR_MTCL_NOP 			 = 0x0,	// NOP
	DDR_MTCL_SELF_REFRESH  	 = 0x2,	// Self Refresh
	DDR_MTCL_REFRESH 		 = 0x3,	// Refresh
	DDR_MTCL_RST 			 = 0x4,	// DDR3 Reset
	DDR_MTCL_PREA 			 = 0x5,	// Precharge All
	DDR_MTCL_SDRAM_MODE_EXIT = 0x7,	// SDRAM Mode (Self Refresh/Power Down) Exit
	DDR_MTCL_ZQCS 			 = 0xb,	// SDRAM ZQ Calibration Short	
	DDR_MTCL_ZQCL 			 = 0xc,	// SDRAM ZQ Calibration Long	
	DDR_MTCL_PD   			 = 0xe,	// SDRAM Power Down	
	DDR_MTCL_SNOP   		 = 0xf,	// SDRAM NOP	
} mtcl_scmd_t;   //DDR MTCL SDRAM commands

extern uint8 ddr_mode_get_cfg_ix(void);
extern void DAN_puts(char *str);
extern void DDR_init_all(void);
extern void WDT_init(uint8 timeout);
extern void SoC_sw_reset(void);
extern int SOC_init(void);
