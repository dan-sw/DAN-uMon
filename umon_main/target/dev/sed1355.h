//------------------------------------------------------------------------
// sed1355.h: SED1355 LCD/CRT Controllers
//

//------------------------------------------------------------------------
// cpu specific code must define the following board specific macros.
// Also the SED1355 must be placed in the correct endian mode via
// hardware.  Otherwise, the Macros must handle any address munging.
// #define SED_MEM_BASE 	0xf0600000 <-- just example addresses, 
// #define SED_REG_BASE		0xf0400000 <-- define for each board
// #define SED_STEP			1 <-- 1 = device is on 16-bit boundry, 2 = 32-bit boundry, 4 = 64-bit boundry
// #define SED_REG(_x_)  	*(vuchar *)(SED_REG_BASE + (_x_ * SED_STEP))	// Control/Status Registers
// #define RD_FB16(_reg_,_val_) ((_val_) = *((vushort *)((SED_MEM_BASE + (_reg_ * SED_STEP)))))
// #define WR_FB16(_reg_,_val_) (*((vushort *)((SED_MEM_BASE + (_reg_ * 2)))) = (_val_))

// SED1355 registers
#define SED1355_REG_REV              		SED_REG(0x00)
#define SED1355_REG_MEM_CFG              	SED_REG(0x01)
#define SED1355_REG_PANEL_TYPE              SED_REG(0x02)
#define SED1355_REG_MOD_RATE                SED_REG(0x03)
#define SED1355_REG_HOR_DISP            	SED_REG(0x04)
#define SED1355_REG_HOR_NONDISP        		SED_REG(0x05)
#define SED1355_REG_HOR_START        		SED_REG(0x06)
#define SED1355_REG_HOR_PULSE           	SED_REG(0x07)
#define SED1355_REG_VER_DISP_HT_LO          SED_REG(0x08)
#define SED1355_REG_VER_DISP_HT_HI          SED_REG(0x09)
#define SED1355_REG_VER_NONDISP	        	SED_REG(0x0A)
#define SED1355_REG_VER_START        		SED_REG(0x0B)
#define SED1355_REG_VER_PULSE           	SED_REG(0x0C)
#define SED1355_REG_DISP_MODE               SED_REG(0x0D)
#define SED1355_REG_LINE_COMP0        		SED_REG(0x0E)
#define SED1355_REG_LINE_COMP1        		SED_REG(0x0F)
#define SED1355_REG_DISP1_START_LO     		SED_REG(0x10)
#define SED1355_REG_DISP1_START_MID     	SED_REG(0x11)
#define SED1355_REG_DISP1_START_HI     		SED_REG(0x12)
#define SED1355_REG_DISP2_START_LO     		SED_REG(0x13)
#define SED1355_REG_DISP2_START_MID     	SED_REG(0x14)
#define SED1355_REG_DISP2_START_HI     		SED_REG(0x15)
#define SED1355_REG_MADD_OFF_LO           	SED_REG(0x16)
#define SED1355_REG_MADD_OFF_HI           	SED_REG(0x17)
#define SED1355_REG_PIXEL_PAN              	SED_REG(0x18)
#define SED1355_REG_CLK_CFG               	SED_REG(0x19)
#define SED1355_REG_PWR_CFG          		SED_REG(0x1A)
#define SED1355_REG_MISC                    SED_REG(0x1B)
#define SED1355_REG_MD_CFG_RD_LO        	SED_REG(0x1C)
#define SED1355_REG_MD_CFG_RD_HI        	SED_REG(0x1D)
#define SED1355_REG_GPIO_CFG0               SED_REG(0x1E)
#define SED1355_REG_GPIO_CFG1               SED_REG(0x1F)
#define SED1355_REG_GPIO_CTL0              	SED_REG(0x20)
#define SED1355_REG_GPIO_CTL1              	SED_REG(0x21)
#define SED1355_REG_PERF_LO          		SED_REG(0x22)
#define SED1355_REG_PERF_HI          		SED_REG(0x23)
#define SED1355_REG_LUT_ADD                 SED_REG(0x24)
#define SED1355_REG_RES_1                 	SED_REG(0x25)
#define SED1355_REG_LUT_DAT                 SED_REG(0x26)
#define SED1355_REG_CRSR_CTL         		SED_REG(0x27)
#define SED1355_REG_CRSR_X_LO   	      	SED_REG(0x28)
#define SED1355_REG_CRSR_X_HI       	  	SED_REG(0x29)
#define SED1355_REG_CRSR_Y_LO         		SED_REG(0x2A)
#define SED1355_REG_CRSR_Y_HI         		SED_REG(0x2B)
#define SED1355_REG_CRSR_CLR0_LO	        SED_REG(0x2C)
#define SED1355_REG_CRSR_CLR0_HI    	    SED_REG(0x2D)
#define SED1355_REG_CRSR_CLR1_LO        	SED_REG(0x2E)
#define SED1355_REG_CRSR_CLR1_HI        	SED_REG(0x2F)
#define SED1355_REG_CRSR_START      		SED_REG(0x30)
#define SED1355_REG_ALT_FRM              	SED_REG(0x31)
														   	
// Bit definitions										   	
//
// SED1355_MEM_CFG											   	
#define SED1355_MEM_CFG_REF_8192			0x70		// refresh rate divisor (CLKI/x)
#define SED1355_MEM_CFG_REF_4096			0x60
#define SED1355_MEM_CFG_REF_2048			0x50
#define SED1355_MEM_CFG_REF_1024			0x40
#define SED1355_MEM_CFG_REF_512				0x30
#define SED1355_MEM_CFG_REF_256				0x20
#define SED1355_MEM_CFG_REF_128				0x10
#define SED1355_MEM_CFG_REF_64				0x00
#define SED1355_MEM_CFG_WE_CTL				0x04		// 0 = CAS controlled, 1 = WE controlled				
#define SED1355_MEM_CFG_CAS_CTL				0x00		// 0 = CAS controlled, 1 = WE controlled				
#define SED1355_MEM_CFG_TYPE_FPM			0x01		// 0 = EDO, 1 = Fast Page Mode
#define SED1355_MEM_CFG_TYPE_EDO			0x00		// 0 = EDO, 1 = Fast Page Mode
             
// SED1355_PANEL_TYPE          
#define SED1355_PANEL_TYPE_EL				0x80
#define SED1355_PANEL_TYPE_16				0x20		// Panel Width, Passive or TFT 16-Bit
#define SED1355_PANEL_TYPE_8_12				0x10		// Passive 8-Bit, TFT 12-Bit
#define SED1355_PANEL_TYPE_4_9				0x00		// Passive 4-Bit, TFT 9-Bit
#define SED1355_PANEL_TYPE_FMT				0x08		// 0 = Passive Format 1, 1 = Passive Format 2
#define SED1355_PANEL_TYPE_CLR				0x04		// 0 = Passive Mono, 1 = Passive Color
#define SED1355_PANEL_TYPE_DUAL				0x02		// 0 = Passive Single, 1 = Passive Dual
#define SED1355_PANEL_TYPE_TFT				0x01		// 0 = Passive, 1 = TFT (DUAL, FMT & CLR are don't cares)

// SED1355_HRTC_PULSE and SED1355_VRTC_PULSE
#define SED1355_PULSE_CRT_POL_HI			0x80		// 1 = HRTC Active High
#define SED1355_PULSE_CRT_POL_LO			0x00		// 0 = HRTC Active Low
#define SED1355_PULSE_LCD_POL_HI			0x40		// 1 = FPLINE Active High
#define SED1355_PULSE_LCD_POL_LO			0x00		// 0 = HRTC Active Low
#define SED1355_PULSE_WID(_x_)				(_x_ & 	0x0f)

// SED1355_DISP_MODE        
#define SED1355_DISP_MODE_SWIV				0x80		// 1 = enable swivel view        
#define SED1355_DISP_MODE_EVEN				0x50		// mode for simultaneous display        
#define SED1355_DISP_MODE_INTER     		0x40   
#define SED1355_DISP_MODE_DOUBLE			0x20
#define SED1355_DISP_MODE_NORM				0x00
#define SED1355_DISP_MODE_16BPP				0x14		// Bit Per Pixel Selection
#define SED1355_DISP_MODE_15BPP				0x10
#define SED1355_DISP_MODE_8BPP				0x0c
#define SED1355_DISP_MODE_4BPP				0x08
#define SED1355_DISP_MODE_2BPP				0x04
#define SED1355_DISP_MODE_1BPP				0x00
#define SED1355_DISP_MODE_CRT				0x02		// 1 = Enable CRT Display
#define SED1355_DISP_MODE_LCD				0x01		// 1 = Enable LCD DIsplay

// SED_CLK_CFG             
#define SED1355_CLK_CFG_MCLK				0x04		// 0: MCLK = CLKI, 1: MCLK = CLKI/2
#define SED1355_CLK_CFG_PCLK4				0x03		// PCLK = MCLK/4
#define SED1355_CLK_CFG_PCLK3				0x02		// PCLK = MCLK/3
#define SED1355_CLK_CFG_PCLK2				0x01		// PCLK = MCLK/2
#define SED1355_CLK_CFG_PCLK1				0x00		// PCLK = MCLK/1

// SED1355_PWR_CFG          	
#define SED1355_PWR_CFG_STAT				0x80		// Power Save Status (Read Only)
#define SED1355_PWR_CFG_LCD_PWR				0x08		// 0 = LCD Powered from SED_DISP_MODE_LCDEN, 1 = LCD OFF
#define SED1355_PWR_CFG_SUSP_NR				0x04		// No DRAM Refresh in Suspend Mode
#define SED1355_PWR_CFG_SUSP_SR				0x02		// DRAM Self Refresh in Suspended Mode
#define SED1355_PWR_CFG_SUSP_CBR			0x00		// DRAM Cas Before RAS Refresh in Suspended Mode
#define SED1355_PWR_CFG_SW_SUSP				0x01		// 1 = Software Suspend Mode Enabled

// SED1355_MISC                
#define SED1355_MISC_HST_DIS				0x80		// 1 = disable host interface (default after reset)
#define SED1355_MISC_HFB_DIS				0x01		// 1 = disable half frame buffer - SED1355 only

// SED1355_GPIO_CTL0           
// SED1355_GPIO_CFG0           
#define SED1355_GPIO_GPIO3					0x08
#define SED1355_GPIO_GPIO2					0x04
#define SED1355_GPIO_GPIO1					0x02

// SED_PERF_LO
#define SED1355_PERF_LO_NRC_3				0x40		// Random Cycle Width in MCLK's
#define SED1355_PERF_LO_NRC_4				0x20
#define SED1355_PERF_LO_NRC_5				0x00
#define SED1355_PERF_LO_RCD_1				0x10		// RAS to CAS delay, 0 = 2 MCLK's, 1 = 1 MCLK
#define SED1355_PERF_LO_NRP_1				0x08		// 1 - RAS Precharge in MCLK's
#define SED1355_PERF_LO_NRP_1_5				0x04		// 1.5
#define SED1355_PERF_LO_NRP_2				0x00		// 2
          	
// SED1355_PERF_HI          	
#define SED1355_PERF_HI_DFH					0x80		// 1 = Display Fifo Halt
#define SED1355_PERF_HI_CPU_WAIT			0x00		// 0 as long as BCLK is at least 1/2 MCLK or greater
#define SED1355_PERF_HI_FTH_MASK			0x0F		// FIfo Threshold Bits

// SED1355_CRSR_CTL      
#define SED1355_CRSR_CTL_INK				0x80		// Cursor is in INK Mode
#define SED1355_CRSR_CTL_CRSR				0x40		// Cursor is in CURSOR Mode
#define SED1355_CRSR_CTL_OFF				0x00		// Cursor is Off
#define SED1355_CRSR_CTL_CTH_MASK			0x0F		// Cursor FIFO Threshold Bits

// SED1355_REG_VER_NONDISP
#define SED1355_VER_NONDISP					0x80		// vertical retrace status 1 = in retrace