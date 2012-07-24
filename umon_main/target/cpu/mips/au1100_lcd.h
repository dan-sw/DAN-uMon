//
// Au1100 LCD Controller Register and Bit Definitions
//
extern int lcd_init(void);
extern void lcd_on(void);
extern void lcd_off(void);

extern void lcd_putchar(char c);
extern void lcd_writechar(uchar c);
extern void lcd_clr_row(int char_row);

#define AU1100_LCD_BASE	0xb5000000	
#define LCD_REG(_x_)	*(vulong *)(AU1100_LCD_BASE + _x_)

//---------------------------------------
//     LCD Controller Register Offsets
#define LCD_CTL      			0x000          				// LCD Control
#define LCD_ISTAT      			0x004          				// LCD Interrupt Status
#define LCD_INTEN      			0x008          				// LCD Interrupt Enable
#define LCD_HOR      			0x00C          				// LCD Horizontal Timing
#define LCD_VER      			0x010          				// LCD Vertical Timing
#define LCD_CLK        			0x014          				// LCD Clock Control
#define LCD_DMA0     			0x018          				// DMA Start Address 0
#define LCD_DMA1     			0x01c          				// DMA Start Address 1
#define LCD_WORDS      			0x020          				// Frame Buffer Word Count
#define LCD_PWMDIV	 			0x024          				// PWM Frequency Divider
#define LCD_PWMHI     			0x028          				// PWM High Time
#define LCD_PALLETE    			0x400          				// Pallette Interface

// Bit Definitions
//	LCD_CTL - LCD Control
#define LCD_CTL_GO				0x00000001		// 1 = Start LCD DMA Engine
#define LCD_CTL_BPP1			(0 << 1)		// 1 Bit Per Pixel
#define LCD_CTL_BPP2			(1 << 1)		// 2 Bits Per Pixel
#define LCD_CTL_BPP4			(2 << 1)		// 4 Bits Per Pixel
#define LCD_CTL_BPP8			(3 << 1)		// 8 Bits Per Pixel
#define LCD_CTL_BPP12			(4 << 1)		// 12 Bits Per Pixel
#define LCD_CTL_BPP16			(5 << 1)		// 16 Bits Per Pixel
#define LCD_CTL_MONO			0x00000000		// Monochrome Panel
#define LCD_CTL_COLOR			0x00000020		// Color Panel
#define LCD_CTL_PAS				0x00000000		// Passive Panel
#define LCD_CTL_TFT				0x00000040		// TFT Panel
#define LCD_CTL_MONO_D4			0x00000000		// 4-Bit Data Interface - Mono Only
#define LCD_CTL_MONO_D8			0x00000080		// 8-Bit Data Interface - Mono Only
#define LCD_CTL_PO0				(0 << 8)		// Pixel Ordering in Frame Buffer
#define LCD_CTL_PO1				(1 << 8)		// See Au1100 Users Manual Table 69
#define LCD_CTL_PO2				(2 << 8)
#define LCD_CTL_PO3				(3 << 8)
#define LCD_CTL_STN_DP			0x00000400		// 1 = STN Dual Panel, 0 = STN Single Panel, N/A in TFT Mode
#define LCD_CTL_RGB				0x00000000		// Color Channel Ordering RGB
#define LCD_CTL_BGR				0x00000800		// Color Channel Ordering BGR
#define LCD_CTL_TFT_D16			0x00000000		// 16-Bit Data Interface - TFT
#define LCD_CTL_TFT_D12			0x00001000		// 12-Bit Data Interface - TFT
#define LCD_CTL_SWIV_0			(0 << 13)		// Swivel Mode 0 degrees
#define LCD_CTL_SWIV_90			(1 << 13)		// Swivel Mode 90 degrees - QVGA only
#define LCD_CTL_SWIV_180		(2 << 13)		// Swivel Mode 180 degrees
#define LCD_CTL_SWIV_270		(3 << 13)		// Swivel Mode 270 degrees - QVGA only
#define LCD_CTL_COH				0x00008000		// LCD Transfers are marked as coherent on the system bus
#define LCD_CTL_WD_EN			0x00010000		// 1 = White Data Enable
#define LCD_CTL_WD_POL			0x00020000		// Polarity of data written when WD_EN = 1
#define LCD_CTL_PPF_655			(0 << 18)		// 16-Bit BPP Format = 6 Red, 5 Green, 5 Blue
#define LCD_CTL_PPF_565			(1 << 18)		// 5 Red, 6 Green, 5 Blue
#define LCD_CTL_PPF_556			(2 << 18)		// 5 Red, 5 Green, 6 Blue	
#define LCD_CTL_PPF_555			(3 << 18)		// 5 Red, 5 Green, 5 Blue
#define LCD_CTL_PPF_555I		(4 << 18)		// 5 Red, 5 Green, 5 Blue, 1 Intensity

//	LCD_ISTAT - LCD Interrupt Status or LCD_INTEN - same bit definitions
#define LCD_INT_S0				0x00000001		// Start Address 0 Latched
#define LCD_INT_S1				0x00000002		// Start Address 1 Latched - Dual Panel STN only
#define LCD_INT_SS				0x00000004		// Start of Vertical Retrace
#define LCD_INT_SA				0x00000008		// Start of Active Video
#define LCD_INT_UF				0x00000020		// Output Fifo Underrun
#define LCD_INT_OF				0x00000040		// Output Fifo Overflow
#define LCD_INT_SD				0x00000080		// Shutdown complete

//	LCD_HOR - LCD Horizontal Timing
#define LCD_HOR_PPL(_x_)		((_x_ & 0x3ff) << 0)	// Pixels per Line = PPL + 1
#define LCD_HOR_HPW(_x_)		((_x_ & 0x03f) << 10)	// Pulse Width in Pixels = HPW + 1
#define LCD_HOR_HN1(_x_)		((_x_ & 0x0ff) << 16)	// Non Display Period 1 (Frontporch) = HN1 + 1
#define LCD_HOR_HN2(_x_)		((_x_ & 0x0ff) << 24)	// Non Display Period 2 (Backporch) = HN2 + 1

//	LCD_VER - LCD Vertical Timing
#define LCD_VER_LPP(_x_)		((_x_ & 0x3ff) << 0)	// Lines per Panel = LLP + 1
#define LCD_VER_VPW(_x_)		((_x_ & 0x03f) << 10)	// Pulse Width in Pixels = VPW + 1
#define LCD_VER_VN1(_x_)		((_x_ & 0x0ff) << 16)	// Non Display Period 1 (Frontporch) = VN1 + 1
#define LCD_VER_VN2(_x_)		((_x_ & 0x0ff) << 24)	// Non Display Period 2 (Backporch) = VN2 + 1

//	LCD_CLK - LCD Clock Control
#define LCD_CLK_PCD(_x_)		((_x_ & 0x3ff) << 0)	// Clock = LCD Clock Gen/(2 * (PCD + 1))
#define LCD_CLK_BF(_x_)			((_x_ & 0x01f) << 10)	// Bias Frequency in Lines = BF + 1
#define LCD_CLK_FCLK_POS		0x00000000				// Polarity of FCLK
#define LCD_CLK_FCLK_NEG		0x00008000
#define LCD_CLK_LCLK_POS		0x00000000				// Polarity of LCLK
#define LCD_CLK_LCLK_NEG		0x00010000
#define LCD_CLK_PCLK_POS		0x00000000				// Output Data on rising egde of Pixel CLK
#define LCD_CLK_PCLK_NEG		0x00020000
#define LCD_CLK_BIAS_POS		0x00040000				// BIAS Polarity
#define LCD_CLK_BIAS_NEG		0x00000000

//	LCD_DMA0 - DMA Start Address 0
#define LCD_DMA0_ADD(_x_)		(_x_ & 0xfffffff0)		// 16 Byte aligned 

//	LCD_DMA1 - DMA Start Address 1 - Used for Lower Panel on Dual STN panels only
#define LCD_DMA1_ADD(_x_)		(_x_ & 0xfffffff0)		// 16 Byte aligned 

//	LCD_PWMDIV - PWM Frequency Divider - for both PWM Clocks
#define LCD_PWMDIV_DIV(_x_)		((_x_ & 0xfff) << 0)	// Divisor
#define LCD_PWMDIV_EN			0x00001000

//	LCD_PWMHI - PWM High Time
#define LCD_PWMHI_PWM0(_x_)		((_x_ & 0xfff) << 0)	// High Time for PWM0
#define LCD_PWMHI_PWM1(_x_)		((_x_ & 0xfff) << 12)	// High Time for PWM1

//	LCD_PALLETE - Pallette Interface
#define LCD_PALLETE_MONO(_x_)	((_x_ & 0xf) << 0)
#define LCD_PALLETE_STN(_r_, _g_, _b_)	(((_r_ & 0xf) << 0) | ((_g_ & 0xf) << 4) | ((_b_ & 0xf) << 8)) 
#define LCD_PALLETE_TFT(_x_)	((_x_ & 0xffff) << 0)


// 16-bit pixels are RGB 565 - LSB of RED and BLUE are tied low at the 
// LCD Interface, while the LSB of GREEN is loaded as 0
#define RED_SUBPIXEL(n)		(n & 0x1f) << 11
#define GREEN_SUBPIXEL(n)	(n & 0x1f) << 6
#define BLUE_SUBPIXEL(n)	(n & 0x1f) << 0

// define a simple VGA style 16-color pallette
#define	LU_BLACK		RED_SUBPIXEL(0x00) | GREEN_SUBPIXEL(0x00) | BLUE_SUBPIXEL(0x00)
#define	LU_BLUE			RED_SUBPIXEL(0x0f) | GREEN_SUBPIXEL(0x00) | BLUE_SUBPIXEL(0x00)
#define	LU_GREEN		RED_SUBPIXEL(0x00) | GREEN_SUBPIXEL(0x0f) | BLUE_SUBPIXEL(0x00)
#define	LU_CYAN			RED_SUBPIXEL(0x00) | GREEN_SUBPIXEL(0x0f) | BLUE_SUBPIXEL(0x0f)
#define	LU_RED			RED_SUBPIXEL(0x0f) | GREEN_SUBPIXEL(0x00) | BLUE_SUBPIXEL(0x00)
#define	LU_VIOLET		RED_SUBPIXEL(0x0f) | GREEN_SUBPIXEL(0x00) | BLUE_SUBPIXEL(0x0f)
#define	LU_YELLOW		RED_SUBPIXEL(0x0f) | GREEN_SUBPIXEL(0x0f) | BLUE_SUBPIXEL(0x00)
#define	LU_GREY			RED_SUBPIXEL(0x0f) | GREEN_SUBPIXEL(0x0f) | BLUE_SUBPIXEL(0x0f)
#define	LU_WHITE		RED_SUBPIXEL(0x17) | GREEN_SUBPIXEL(0x17) | BLUE_SUBPIXEL(0x17)
#define	LU_BRT_BLUE		RED_SUBPIXEL(0x00) | GREEN_SUBPIXEL(0x00) | BLUE_SUBPIXEL(0x1f)
#define	LU_BRT_GREEN	RED_SUBPIXEL(0x00) | GREEN_SUBPIXEL(0x1f) | BLUE_SUBPIXEL(0x00)
#define	LU_BRT_CYAN		RED_SUBPIXEL(0x00) | GREEN_SUBPIXEL(0x1f) | BLUE_SUBPIXEL(0x1f)
#define	LU_BRT_RED		RED_SUBPIXEL(0x1f) | GREEN_SUBPIXEL(0x00) | BLUE_SUBPIXEL(0x00)
#define	LU_BRT_VIOLET	RED_SUBPIXEL(0x1f) | GREEN_SUBPIXEL(0x00) | BLUE_SUBPIXEL(0x1f)
#define	LU_BRT_YELLOW	RED_SUBPIXEL(0x00) | GREEN_SUBPIXEL(0x1f) | BLUE_SUBPIXEL(0x1f)
#define	LU_BRT_WHITE	RED_SUBPIXEL(0x1f) | GREEN_SUBPIXEL(0x1f) | BLUE_SUBPIXEL(0x1f)
