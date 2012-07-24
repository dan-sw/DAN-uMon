//------------------------------------------------------------------------
// s1d13706_16bit.h: S1D13706 LCD Controller - 16-Bit access mode
//

#include "bits.h"
//------------------------------------------------------------------------
// cpu specific code must define the following board specific macros.
// in cpuio.h.  These examples assume the S1D13706 has been placed in 
// the correct endian mode via hardware.
// #define S1D_MEM_BASE 	0xf0600000 <-- just example addresses, 
// #define S1D_REG_BASE		0xf0400000 <-- define for each board
// #define S1D_STEP			1 <-- 1 = device is on 16-bit boundry, 2 = 32-bit boundry, 4 = 64-bit boundry
// #define S1D_REG16(_x_)  	*(vushortr *)(S1D_REG_BASE + (_x_ * S1D_STEP))	// Control/Status Registers
// #define RD_FB16(_reg_,_val_) ((_val_) = *((vushort *)((S1D_MEM_BASE + (_reg_ * S1D_STEP)))))
// #define WR_FB16(_reg_,_val_) (*((vushort *)((S1D_MEM_BASE + (_reg_ * 2)))) = (_val_))
// Big endian processors
// #define H2S1D(_x_)	((((x) & 0xff00) >> 8) | (((x) & 0x00ff) << 8))
// Little endian
// #define H2S1D(_x_)	(_x_)
		

// ----------------------------------------------------------
// S1D registers - 16-Bit Access Mode.  The assumption is that
// the even register is bits 0-7 while the odd register is 8-15.
// This makes accessing the registers endian neutral.  Addresses
// shown are for 16-bit offsets. The S1D macros take care of 16 
// and 32-bit address offsets
//															High Byte				Low Byte				
#define S1D_REG_REV_BUF					S1D_REG16(0x00) // 	Display Buffer Size		Revision 				
#define S1D_REG_CNF        				S1D_REG16(0x02) // 	-						CNF0-7 Value 			
#define S1D_REG_MCLK_PCLK				S1D_REG16(0x04)	//	PCLK Config				MCLK Config				
#define S1D_REG_BLUT_WR_AND_GLUT_WR		S1D_REG16(0x08)	//	Green LUT Write Data	Blue LUT Write Data		
#define S1D_REG_RLUT_WR_AND_LUT_WR_ADD	S1D_REG16(0x0A) //	LUT Write Address		Red LUT Write Data		
#define S1D_REG_BLUT_RD_AND_GLUT_RD		S1D_REG16(0x0C)	//	Green LUT Read Data		Blue LUT Read Data		
#define S1D_REG_RLUT_RD_AND_LUT_RD_ADD	S1D_REG16(0x0E) //	LUT Read Address		Red LUT Read Data		
#define S1D_REG_PANEL_AND_MOD_RATE		S1D_REG16(0x10) //	MOD Rate				Panel Type				
#define S1D_REG_HOR_TOTAL				S1D_REG16(0x12) //	-						Horizontal Total		
#define S1D_REG_HOR_PERIOD				S1D_REG16(0x14) //	-						Horizontal Period		
#define S1D_REG_HOR_START				S1D_REG16(0x16) //	Horizontal Start 8-9	Horizontal Start 0-7 	
#define S1D_REG_VERT_TOTAL				S1D_REG16(0x18) //	Vertical Total 8-9		Vertical Total 0-7		
#define S1D_REG_VERT_PERIOD				S1D_REG16(0x1C) //	Vertical Period 8-9		Vertical Period 0-7		
#define S1D_REG_VERT_START				S1D_REG16(0x1E) //	Vertical Start 8-9		Vertical Start 0-7 		
#define S1D_REG_LP_WIDTH				S1D_REG16(0x20) //							Line Pulse Width
#define S1D_REG_LP_START				S1D_REG16(0x22) //	Line Pulse Start 8-9	Line Pulse Start 0-7 	
#define S1D_REG_FP_WIDTH				S1D_REG16(0x24) //							Frame Pulse Width
#define S1D_REG_FP_START				S1D_REG16(0x26) //	Frame Pulse Start 8-9	Frame Pulse Start 0-7 	
#define S1D_REG_DTFD_GCP_IDX			S1D_REG16(0x28) //	-						D-TFD GCP Index			
#define S1D_REG_DTFD_GCP_DATA			S1D_REG16(0x2C) //	-						D-TFD GCP Data			
#define S1D_REG_DISP_MODE				S1D_REG16(0x70) //	Special Effects			Display Mode			
#define S1D_REG_MAIN_START_LO			S1D_REG16(0x74) //	Main Window Start 8-15	Main Window Start 0-7	
#define S1D_REG_MAIN_START_HI			S1D_REG16(0x76) //	-						Main Window Start 16	
#define S1D_REG_MAIN_OFFSET				S1D_REG16(0x78) //	Main Window Offset 8-9	Main Window Offset 0-7	
#define S1D_REG_PIP_START_LO			S1D_REG16(0x7C) //	Main Window Start 8-15	PIP Window Start 0-7	
#define S1D_REG_PIP_START_HI			S1D_REG16(0x7E) //	-						PIP Window Start 16		
#define S1D_REG_PIP_OFFSET				S1D_REG16(0x80) //	PIP Window Offset 8-9	PIP Window Offset 0-7	
#define S1D_REG_PIP_X_START				S1D_REG16(0x84) //	PIP Window X Start 8-9	PIP Window X Start 0-7	
#define S1D_REG_PIP_Y_START				S1D_REG16(0x88) //	PIP Window Y Start 8-9	PIP Window Y Start 0-7	
#define S1D_REG_PIP_X_END				S1D_REG16(0x8C) //	PIP Window X End 8-9	PIP Window X End 0-7	
#define S1D_REG_PIP_Y_END				S1D_REG16(0x90) //	PIP Window Y End 8-9	PIP Window Y End 0-7	
#define S1D_REG_PWR_SAVE				S1D_REG16(0xA0) //	-						Power Save Config		
#define S1D_REG_RSRVD					S1D_REG16(0xA2) //	Reserved - must write 0 to it
#define S1D_REG_SCRATCHPAD				S1D_REG16(0xA4) //	Scratch Pad 9-15		Scratch Pad 0-7			
#define S1D_REG_GPIO_CFG				S1D_REG16(0xA8) //	GPIO In Enable			GPIO 0-6 Config			
#define S1D_REG_GPIO_STAT				S1D_REG16(0xAC) //	GPIO Output Control		GPIO0-6 Status			
#define S1D_REG_PWM_CTL					S1D_REG16(0xB0) //	PWM Control				PWM Control				
#define S1D_REG_PWM_DUTY				S1D_REG16(0xB2) //	PWM Duty Cycle			CV Burst Length			


//------------------------------------------------------------------------------------------------------------
// Bit Assignments - Little Endian, Use H2S1D() macro for endian neutral access

// 	Revision and Display Buffer Size
// Bits 0 -7
#define S1D_REG_REV_REV_MASK				0x03
#define S1D_REG_REV_REV_SHIFT				0
#define S1D_REG_REV_PROD_MASK				0x3F
#define S1D_REG_REV_PROD_SHIFT				3
// Bits 8-15
#define S1D_REG_REV_BUF_MASK				0xFF
#define S1D_REG_REV_BUF_SHIFT				8

// 	CNF0-7 Value
// Bits 0 -7
#define S1D_REG_CNF_CNF7				BIT7
#define S1D_REG_CNF_CNF6				BIT6
#define S1D_REG_CNF_CNF5				BIT5
#define S1D_REG_CNF_CNF4				BIT4
#define S1D_REG_CNF_CNF3				BIT3
#define S1D_REG_CNF_CNF2				BIT2
#define S1D_REG_CNF_CNF1				BIT1
#define S1D_REG_CNF_CNF0				BIT0

//	MCLK Config and PCLK Config
// Bits 0 -7
#define S1D_REG_MCLK_DIV1				(0x00 << 4)
#define S1D_REG_MCLK_DIV2				(0x01 << 4)
#define S1D_REG_MCLK_DIV3				(0x02 << 4)
#define S1D_REG_MCLK_DIV4				(0x03 << 4)

// Bits 8-15
#define S1D_REG_PCLK_SRC_MCLK			(0x00 << 8)
#define S1D_REG_PCLK_SRC_BCLK			(0x01 << 8)
#define S1D_REG_PCLK_SRC_CLKI			(0x02 << 8)
#define S1D_REG_PCLK_SRC_CLKI2			(0x03 << 8)
#define S1D_REG_PCLK_DIV1				(0x00 << 12)
#define S1D_REG_PCLK_DIV2				(0x01 << 12)
#define S1D_REG_PCLK_DIV3				(0x02 << 12)
#define S1D_REG_PCLK_DIV4				(0x03 << 12)
#define S1D_REG_PCLK_DIV8				(0x04 << 12)

//	Panel Type and MOD Rate
// Bits 0 -7
#define S1D_REG_PANEL_TYPE_STN			(0x00 << 0)	// Panel Type
#define S1D_REG_PANEL_TYPE_TFT			(0x01 << 0)
#define S1D_REG_PANEL_TYPE_HRTFT		(0x02 << 0)
#define S1D_REG_PANEL_TYPE_DTFD			(0x03 << 0)
#define S1D_REG_PANEL_ACT_320X240		BIT3		// for HRTFT and DTFD 1 = 320 x 240
#define S1D_REG_PANEL_WIDTH_PAS_4		(0x00 << 4)	// For STN Types, selects the data bus width
#define S1D_REG_PANEL_WIDTH_PAS_8		(0x01 << 4)
#define S1D_REG_PANEL_WIDTH_PAS_16		(0x02 << 4)
#define S1D_REG_PANEL_WIDTH_ACT_9		(0x00 << 4)	// for active types, selects the data bus width
#define S1D_REG_PANEL_WIDTH_ACT_12		(0x01 << 4)
#define S1D_REG_PANEL_WIDTH_ACT_18		(0x02 << 4)
#define S1D_REG_PANEL_COLOR				BIT6		// 1 = Color, 0 = mono
#define S1D_REG_PANEL_FORMAT2			BIT7		// 1 = Format 2, 0 = Format 1 (Color 8-bit only)
// Bits 8-15
#define S1D_REG_MOD_RATE(_x_)			((_x_ & 0x3F) << 8)

//	Display Mode and Special Effects
// Bits 0 -7
#define S1D_REG_DISP_MODE_1BPP			(0x00 << 0)
#define S1D_REG_DISP_MODE_2BPP			(0x01 << 0)
#define S1D_REG_DISP_MODE_4BPP			(0x02 << 0)
#define S1D_REG_DISP_MODE_8BPP			(0x03 << 0)
#define S1D_REG_DISP_MODE_16BPP			(0x04 << 0)
#define S1D_REG_DISP_MODE_INVERT		BIT4			// 1 = inverse video
#define S1D_REG_DISP_MODE_NODITHER		BIT6			// 1 = disable dithering
#define S1D_REG_DISP_MODE_BLANK			BIT7			// 1 = blank display
// Bits 8-15
#define S1D_REG_DISP_EFFECTS_SWIV0		(0x00 << 8)
#define S1D_REG_DISP_EFFECTS_SWIV90		(0x01 << 8)
#define S1D_REG_DISP_EFFECTS_SWIV180	(0x02 << 8)
#define S1D_REG_DISP_EFFECTS_SWIV270	(0x03 << 8)
#define S1D_REG_DISP_EFFECTS_PIP		BIT12			// 1 = Picture in Picture Enable
#define S1D_REG_DISP_EFFECTS_BYTE_SWAP	BIT14			// 1 = Swap Bytes between CPU and Frame Buffer
#define S1D_REG_DISP_EFFECTS_WORD_SWAP	BIT15			// 1 = Swap Words between CPU and Frame Buffer

//	Power Save Configuration
// Bits 0 -7
#define S1D_REG_PWR_SAVE_ENABLE			BIT0			// 1 = Enter Power Save Mode
#define S1D_REG_PWR_SAVE_MEM_STAT		BIT3			// 1 = Memory is in power save mode
#define S1D_REG_PWR_SAVE_VERT_RETRACE	BIT7			// 1 = Controller is in Vertical Retrace Period

//	GPIO 0-6 Config and GPIO In Enable
// Bits 0 -7
#define S1D_REG_GPIO_CFG0				BIT0			// 1 = output, 0 = input
#define S1D_REG_GPIO_CFG1				BIT1			// 1 = output, 0 = input
#define S1D_REG_GPIO_CFG2				BIT2			// 1 = output, 0 = input
#define S1D_REG_GPIO_CFG3				BIT3			// 1 = output, 0 = input
#define S1D_REG_GPIO_CFG4				BIT4			// 1 = output, 0 = input
#define S1D_REG_GPIO_CFG5				BIT5			// 1 = output, 0 = input
#define S1D_REG_GPIO_CFG6				BIT6			// 1 = output, 0 = input
// Bits 8-15
#define S1D_REG_GPIO_CFG_IN				BIT15			// 1 = allow GPIO's to be inputs (if their CFG bit is cleared)

//	GPIO0-6 Status and GPIO Output Control
// Bits 0 -7
#define S1D_REG_GPIO_CTL0				BIT0			// Write output value, read input status (based on CFG bit)
#define S1D_REG_GPIO_CTL1				BIT1
#define S1D_REG_GPIO_CTL2				BIT2
#define S1D_REG_GPIO_CTL3				BIT3
#define S1D_REG_GPIO_CTL4				BIT4
#define S1D_REG_GPIO_CTL5				BIT5
#define S1D_REG_GPIO_CTL6				BIT6
// Bits 8-15
#define S1D_REG_GPIO_CTL_GPO			BIT15			// Drive GPO Bit

//	PWM and CV Control
// Bits 0 -7
#define S1D_REG_PWM_CTL_CV_ENABLE		BIT0			// 1 = Enable CV Pulse
#define S1D_REG_PWM_CTL_CV_START		BIT1			// 1 = Start CV Pulse
#define S1D_REG_PWM_CTL_CV_STATUS		BIT2			// 1 = CV Pulse is active
#define S1D_REG_PWM_CTL_CV_FORCE1		BIT3			// 1 = Force CV Pulse High
#define S1D_REG_PWM_CTL_PWM_ENABLE		BIT4			// 1 = Enable PWM
#define S1D_REG_PWM_CTL_PWM_FORCE		BIT7			// 1 = Force PWM High
// Bits 8-15
#define S1D_REG_PWM_CTL_SOURCE_CLKI		0x00			// PWM Source Clock = CLKI
#define S1D_REG_PWM_CTL_SOURCE_CLKI2	BIT8			// PWM Source Clock = CLKI2
#define S1D_REG_PWM_CTL_CV_DIVIDE(_x_)	((_x_ & 0x07) << 9)
#define S1D_REG_PWM_CTL_PWM_DIVIDE(_x_)	((_x_ & 0x0f) << 12)
