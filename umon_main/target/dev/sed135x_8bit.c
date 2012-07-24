//==========================================================================
//
//      sed135x.c
//
// Author(s):   Michael Kelly - Cogent Computer Systems, Inc.
// Date:        05-24-2002
// Description:	Init Code for SED135x Display Controller
//
//==========================================================================

#include "config.h"
#include "cpuio.h"
#include "genlib.h"
#include "stddefs.h"
#include "sed1355.h"
#include "sed1356.h"
#include "font8x16.h"
#include "sed_lut.h"

//--------------------------------------------------------------------------
// function prototypes
//
ulong sed135x_init(void);
void sed135x_on(void);
void sed135x_off(void);
void sed_lcd_bkl(uchar bright);
void sed_putchar(char c);
void sed_writechar(uchar c);
void sed_clr_row(int char_row);
int sed_tst(int argc,char *argv[]);
int sed(int argc,char *argv[]);

extern int udelay(int delay);
extern lcd_bkl(uchar bright);
extern lcd_bkl(uchar bright);
extern void fs6377_init(int);
extern ulong i2c_rd_device(uchar dev, uchar reg, uchar *data);

// global flags to determine what, if anything, was found
int sed135x_ok;
int sed135x_tst;	// flag to indicate we are testing to disable sed_puchar
int sed1355;
int sed_disp_mode_crt;

// globals to keep track of foreground, background colors and x,y position
int sed_color_depth;		// 4, 8 or 16
int sed_fg_color;			// 0 to 15, used as lookup into VGA color table
int sed_bg_color;			// 0 to 15, used as lookup into VGA color table
int sed_col;				// current column, 0 to COLS_PER_SCREEN - 1
int sed_row;				// current row, 0 to (ROWS_PER_SCREEN * 2) - 1
ulong sed_fb_offset;	   	// current offset into frame buffer for sed_putchar

// GPIO1 is used to control the LCD backlight on many CSB's
#define SED1355_BKL_ON		SED1355_REG_GPIO_CTL0 |= SED1355_GPIO_GPIO1		// GPIO1 = 1 
#define SED1355_BKL_OFF		SED1355_REG_GPIO_CTL0 &= ~SED1355_GPIO_GPIO1	// GPIO1 = 0

#define SED1356_BKL_ON		SED1356_REG_GPIO_CTL |= SED1356_GPIO_GPIO1		// GPIO1 = 1 
#define SED1356_BKL_OFF		SED1356_REG_GPIO_CTL &= ~SED1356_GPIO_GPIO1	// GPIO1 = 0

// GPIO2 is used to sense the presence of a monitor.  0 = monitor connected, 1 = no monitor
// we invert the sense to make it easier to test and more logical.
#define SED1355_CRT			SED1355_REG_GPIO_CTL0 & SED1355_GPIO_GPIO2
#define SED1356_CRT			SED1356_REG_GPIO_CTL & SED1356_GPIO_GPIO2

#define SED_FB_SIZE(_depth_)				(((PIXELS_PER_COL * PIXELS_PER_ROW) * _depth_) / 8)

#define SED_ROW_SIZE(_depth_)				((PIXELS_PER_ROW * _depth_) / 8)

#define SED_GET_ADD(_row_, _col_, _depth_)	(((((_row_ * PIXELS_PER_ROW) * FONT_HEIGHT) \
											  + (_col_ * FONT_WIDTH)) \
											  * _depth_) / 8)


//--------------------------------------------------------------------------
// sed135x_on
//
// This function turns on the SED1355 or SED1356 LCD and/or CRT
//
void sed135x_on()
{
	uchar temp8;
	int i;

	sed135x_off();

	// Turn on the LCD and/or CRT
	if (sed1355){
		// The SED1355 has only one set of timing registers so we need
		// re-write them with the correct mode each time we get called.
		if (sed_disp_mode_crt) {	// 1 = CRT Mode
			// turn off the display fifo first
			SED1355_REG_PERF_HI |= SED1355_PERF_HI_DFH;

			// turn the LCD backlight off
			lcd_bkl(0);

			// Change the timing to CRT Mode
			SED1355_REG_HOR_PULSE = SED1355_PULSE_WID(SED_HOR_PULSE_WIDTH_CRT) | SED1355_PULSE_LCD_POL_LO;
			SED1355_REG_HOR_START	= SED_HOR_PULSE_START_CRT;
			SED1355_REG_HOR_NONDISP = SED_HOR_NONDISP_CRT;
			SED1355_REG_VER_PULSE = SED1355_PULSE_WID(SED_VER_PULSE_WIDTH_CRT) | SED1355_PULSE_LCD_POL_LO;
			SED1355_REG_VER_START = SED_VER_PULSE_START_CRT;
			SED1355_REG_VER_NONDISP = SED_VER_NONDISP_CRT;
			SED1355_REG_DISP_MODE = SED1355_DISP_MODE_CRT;

			// now we can turn the display fifo on
			SED1355_REG_PERF_HI &= ~SED1355_PERF_HI_DFH;
		} // if CRT Mode
		else {	// 0 = LCD Mode
			// turn off the display fifo first
			SED1355_REG_PERF_HI |= SED1355_PERF_HI_DFH;

			// Change the timing to LCD Mode
			SED1355_REG_HOR_PULSE = SED1355_PULSE_WID(SED_HOR_PULSE_WIDTH_LCD) | SED1355_PULSE_LCD_POL_LO;
			SED1355_REG_HOR_START	= SED_HOR_PULSE_START_LCD;
			SED1355_REG_HOR_NONDISP = SED_HOR_NONDISP_LCD;
			SED1355_REG_VER_PULSE = SED1355_PULSE_WID(SED_VER_PULSE_WIDTH_LCD) | SED1355_PULSE_LCD_POL_LO;
			SED1355_REG_VER_START = SED_VER_PULSE_START_LCD;
			SED1355_REG_VER_NONDISP = SED_VER_NONDISP_LCD;
			SED1355_REG_DISP_MODE = SED1355_DISP_MODE_LCD;
			// now we can turn the display fifo on
			lcd_bkl(0xff);
		} // else LCD Mode
	} // if SED1355
	else {
		// The SED1356 supports seperate LCD and CRT timing registers
		// that have already been setup.  We just blank the side we 
		// aren't using and enable the other.
		if (sed_disp_mode_crt) {	// 1 = CRT Mode
			// Blank the LCD and CRT
			SED1356_REG_LCD_DISP_MODE |= SED1356_LCD_DISP_BLANK;
			SED1356_REG_CRT_DISP_MODE |= SED1356_CRT_DISP_BLANK;

			// turn the LCD backlight off
			lcd_bkl(0);

			// Set the SED1356 to CRT Mode
			SED1356_REG_DISP_MODE = SED1356_DISP_SWIV_NORM | SED1356_DISP_MODE_CRT;

			// Turn on the CRT
			SED1356_REG_CRT_DISP_MODE &= ~SED1356_CRT_DISP_BLANK;
		} // if CRT mode
		else {	// 0 = LCD Mode
			// Blank the LCD and CRT
			SED1356_REG_LCD_DISP_MODE |= SED1356_LCD_DISP_BLANK;
			SED1356_REG_CRT_DISP_MODE |= SED1356_CRT_DISP_BLANK;

			// Set the SED1356 to LCD Mode
			SED1356_REG_DISP_MODE = SED1356_DISP_SWIV_NORM | SED1356_DISP_MODE_LCD;

			// Turn on the LCD
			SED1356_REG_LCD_DISP_MODE &= ~SED1356_LCD_DISP_BLANK;
			lcd_bkl(0xff);		// turn the LCD backlight on/full brightness
		} // else LCD Mode
	} // else SED1356

	udelay(10000); 		// wait for Power-up sequence to complete
}

//--------------------------------------------------------------------------
// sed_lcd_bkl()
//
// This function turns on the LCD backlight connected to GPIO1.  This is
// not used if the board has a different method of controlling the
// backlight.  Since the Sed has only a single GPIO bit and no way
// to modulate it, we use any non-zero value of bright to turn it on.
//
void sed_lcd_bkl(uchar bright)
{
	// Any non-zero value for bright means on
	if (sed1355){
		if (bright) SED1355_BKL_ON;
		else SED1355_BKL_OFF;
	}
	else { // SED1356
		if (bright) SED1356_BKL_ON;
		else SED1356_BKL_OFF;
	}	
}

//--------------------------------------------------------------------------
// sed135x_off
//
// This function turns off the SED1355 LCD and/or CRT and the display
// fifo.  It can also turn off the clocks if mode is true, thus allowing
// the programmable clock generator to be changed.
//
void sed135x_off()
{
	if (sed1355) SED1355_REG_DISP_MODE = 0x00;
	else SED1356_REG_DISP_MODE = SED1356_DISP_SWIV_NORM | SED1356_DISP_MODE_OFF;
	lcd_bkl(0);	// turn the LCD backlight off
}

//--------------------------------------------------------------------------
// sed135x_init
//
// This function sets up the sed1355 or sed1356 whichever is found
//
vulong sed135x_init()
{

	vushort temp16;
	int i;
	
	sed135x_ok = 0;
	sed1355 = 0;
	sed_disp_mode_crt = 0;	// assume LCD

	// Check the ID to make sure we even have a SED1355 or 1356 installed
	temp16 = SED_REG_REV & SED_REV_ID_MASK;

	if ((temp16 != SED_REV_ID_1355) && (temp16 != SED_REV_ID_1356)){
		printf("SED135x Not Found! SED_REG_REV = %04x.\n", temp16);
		return -1;
	}

	if (temp16 == SED_REV_ID_1355)
	{
		sed1355 = 1;

		// Enable Host Access
		SED1355_REG_MISC = 0x00;

		// Disable the display FIFO to stop all display accesses
		SED1355_REG_PERF_HI |= SED1355_PERF_HI_DFH;

		// Test for the presence of a CRT
		SED1355_REG_GPIO_CTL0 = 0x00;   // clear all GPIO's
		SED1355_REG_GPIO_CFG0 = SED1355_GPIO_GPIO1;	// set GPIO1 as output, GPIO2 as input
		if (SED1355_CRT == 0) sed_disp_mode_crt = 1; 

		// Enable Power Save Mode before we mess with the clocks
		SED1355_REG_PWR_CFG |= SED1355_PWR_CFG_SUSP_SR | SED1355_PWR_CFG_LCD_PWR | SED1355_PWR_CFG_SW_SUSP;

		// Wait until power is down
		while ((SED1355_REG_PWR_CFG & SED1355_PWR_CFG_STAT) == 0){}

		// Change the programmable clock generator to the desired timing
		if (sed_disp_mode_crt) fs6377_init(SED_DISP_MODE_CRT);
		else fs6377_init(SED_DISP_MODE_LCD);

		// Re-enable Power
		SED1355_REG_PWR_CFG = 0x00;

		// Write the registers that are common to both modes
		// LCD and CRT specific timing will be written when the
		// display is turned on.
		SED1355_REG_MEM_CFG	= SED1355_MEM_CFG_REF_2048 | SED1355_MEM_CFG_TYPE_EDO | SED1355_MEM_CFG_CAS_CTL;
		SED1355_REG_PANEL_TYPE = SED1355_PANEL_TYPE_16 | SED1355_PANEL_TYPE_CLR | SED1355_PANEL_TYPE_TFT;
		SED1355_REG_HOR_DISP = (PIXELS_PER_ROW/8) - 1;
		SED1355_REG_VER_DISP_HT_LO = (PIXELS_PER_COL - 1) & 0x0ff;
		SED1355_REG_VER_DISP_HT_HI = ((PIXELS_PER_COL - 1) & 0x300) >> 8;
		SED1355_REG_LINE_COMP0 = 0xFF; // 1 screen so set the split point to the max - 1024
		SED1355_REG_LINE_COMP1 = 0x03;
		SED1355_REG_MADD_OFF_LO = PIXELS_PER_ROW & 0x0ff;
		SED1355_REG_MADD_OFF_HI = (PIXELS_PER_ROW & 0x300) >> 8;
		SED1355_REG_PERF_LO = SED1355_PERF_LO_NRC_3 | SED1355_PERF_LO_RCD_1 | SED1355_PERF_LO_NRP_1;

		// The following registers are all intialized to 0
		SED1355_REG_DISP1_START_LO = 0x00; 
		SED1355_REG_DISP1_START_MID = 0x00; 
		SED1355_REG_DISP1_START_HI = 0x00;  
		SED1355_REG_DISP2_START_LO = 0x00;  
		SED1355_REG_DISP2_START_MID = 0x00; 
		SED1355_REG_DISP2_START_HI = 0x00;  
		SED1355_REG_PIXEL_PAN = 0x00;       
		SED1355_REG_CLK_CFG = 0x00; // PCLK = MCLK         
		SED1355_REG_PWR_CFG = 0x00;         
		SED1355_REG_GPIO_CFG1 = 0x00;       
		SED1355_REG_GPIO_CTL1 = 0x00;       
		SED1355_REG_LUT_ADD = 0x00;         
		SED1355_REG_LUT_DAT = 0x00;         
		SED1355_REG_CRSR_CTL = 0x00;        
		SED1355_REG_CRSR_X_LO = 0x00;   	
		SED1355_REG_CRSR_X_HI = 0x00;       
		SED1355_REG_CRSR_Y_LO = 0x00;       
		SED1355_REG_CRSR_Y_HI = 0x00;       
		SED1355_REG_CRSR_CLR0_LO = 0x00;	
		SED1355_REG_CRSR_CLR0_HI = 0x00;    
		SED1355_REG_CRSR_CLR1_LO = 0x00;    
		SED1355_REG_CRSR_CLR1_HI = 0x00;    
		SED1355_REG_CRSR_START = 0x00;      
		SED1355_REG_ALT_FRM = 0x00;         

		// Test for the presence of a CRT
		if (SED1355_CRT == 0) sed_disp_mode_crt = 1; 

	} // if sed1355

	else {

		// Enable Host Access
		SED1356_REG_MISC = 0x00;

		// Disable the display
		SED1356_REG_DISP_MODE = SED1356_DISP_SWIV_NORM | SED1356_DISP_MODE_OFF;

		// Test for the presence of a CRT
		SED1356_REG_GPIO_CTL = 0x00;				// Disable Backlight 
		SED1356_REG_GPIO_CFG = SED1356_GPIO_GPIO1;	// GPIO1 Out, GPIO2 In
		if (SED1356_CRT) sed_disp_mode_crt = 0; 
		else sed_disp_mode_crt = 1;

		// Enable Power Save Mode before we mess with the clocks
		SED1356_REG_REF_RATE |= SED1356_REF_TYPE_SELF;	// set dram to self refresh first
		SED1356_REG_PWR_CFG |= SED1356_PWR_MCLK;

		// Wait until power is down
		while ((SED1356_REG_PWR_STAT & SED1356_PWR_MCLK) == 0){}

		// Change the programmable clock generator to the desired timing
		if (sed_disp_mode_crt) fs6377_init(SED_DISP_MODE_CRT);
		else fs6377_init(SED_DISP_MODE_LCD);

		// Re-enable Power
		SED1356_REG_PWR_CFG = 0x00;

		// Common Control Registers
		SED1356_REG_MCLK_CFG = SED1356_MCLK_SRC_BCLK;
		SED1356_REG_LCD_PCLK_CFG = SED1356_PCLK_SRC_CLKI;
		SED1356_REG_CRT_PCLK_CFG = SED1356_PCLK_SRC_CLKI;
		SED1356_REG_MEDIA_PCLK_CFG = 0x00;
		SED1356_REG_WAIT_STATE = 0x01;
		SED1356_REG_MEM_CFG = SED1356_MEM_CFG_2CAS_EDO;
		SED1356_REG_REF_RATE = SED1356_REF_RATE_2048;
		SED1356_REG_MEM_TMG0 = SED1356_MEM_TMG0_EDO50_MCLK33;
		SED1356_REG_MEM_TMG1 = SED1356_MEM_TMG1_EDO50_MCLK33;
		SED1356_REG_PANEL_TYPE = SED1356_PANEL_TYPE_16 | SED1356_PANEL_TYPE_CLR | SED1356_PANEL_TYPE_TFT;
		SED1356_REG_MOD_RATE = 0x00;

		// LCD Specific Registers
		SED1356_REG_LCD_HOR_DISP = (PIXELS_PER_ROW/8) - 1;
		SED1356_REG_LCD_HOR_NONDISP = SED_HOR_NONDISP_LCD;
		SED1356_REG_LCD_HOR_START = SED_HOR_PULSE_START_LCD;
		SED1356_REG_LCD_HOR_PULSE = SED1356_PULSE_WID(SED_HOR_PULSE_WIDTH_LCD) | SED1356_PULSE_POL_LOW;
		SED1356_REG_LCD_VER_DISP_HT_LO = (PIXELS_PER_COL - 1) & 0x0ff;
		SED1356_REG_LCD_VER_DISP_HT_HI = ((PIXELS_PER_COL - 1) & 0x300) >> 8;
		SED1356_REG_LCD_VER_NONDISP = SED_VER_NONDISP_LCD;
		SED1356_REG_LCD_VER_START = SED_VER_PULSE_START_LCD;
		SED1356_REG_LCD_VER_PULSE = SED1356_PULSE_WID(SED_VER_PULSE_WIDTH_LCD) | SED1356_PULSE_POL_LOW;
		switch (sed_color_depth) {
			case 4:  SED1356_REG_LCD_DISP_MODE |= SED1356_LCD_DISP_BLANK | SED1356_LCD_DISP_SWIV_NORM | SED1356_LCD_DISP_4BPP;  break;
			case 8:  SED1356_REG_LCD_DISP_MODE |= SED1356_LCD_DISP_BLANK | SED1356_LCD_DISP_SWIV_NORM | SED1356_LCD_DISP_8BPP;  break;
			default: SED1356_REG_LCD_DISP_MODE |= SED1356_LCD_DISP_BLANK | SED1356_LCD_DISP_SWIV_NORM | SED1356_LCD_DISP_16BPP; break;
		}
		SED1356_REG_LCD_MISC = 0x00;
		SED1356_REG_LCD_DISP_START_LO = 0x00;
		SED1356_REG_LCD_DISP_START_MID = 0x00;
		SED1356_REG_LCD_DISP_START_HI = 0x00;
		SED1356_REG_LCD_ADD_OFFSET_LO = (SED_ROW_SIZE(sed_color_depth) / 2) & 0x0ff;
		SED1356_REG_LCD_ADD_OFFSET_HI = ((SED_ROW_SIZE(sed_color_depth) / 2) & 0x700) >> 8;
		SED1356_REG_LCD_PIXEL_PAN = 0x00;
		SED1356_REG_LCD_FIFO_HI_THRES = 0x00;	// auto mode
		SED1356_REG_LCD_FIFO_LO_THRES = 0x00;

		// CRT Specific Registers
		SED1356_REG_CRT_HOR_DISP = (PIXELS_PER_ROW/8) - 1;
		SED1356_REG_CRT_HOR_NONDISP = SED_HOR_NONDISP_CRT;
		SED1356_REG_CRT_HOR_START = SED_HOR_PULSE_START_CRT;
		SED1356_REG_CRT_HOR_PULSE = SED1356_PULSE_WID(SED_HOR_PULSE_WIDTH_CRT) | SED1356_PULSE_POL_LOW;
		SED1356_REG_CRT_VER_DISP_HT_LO = (PIXELS_PER_COL - 1) & 0x0ff;
		SED1356_REG_CRT_VER_DISP_HT_HI = ((PIXELS_PER_COL - 1) & 0x300) >> 8;
		SED1356_REG_CRT_VER_NONDISP = SED_VER_NONDISP_CRT;
		SED1356_REG_CRT_VER_START = SED_VER_PULSE_START_CRT;
		SED1356_REG_CRT_VER_PULSE = SED1356_PULSE_WID(SED_VER_PULSE_WIDTH_CRT) | SED1356_PULSE_POL_LOW;
		switch (sed_color_depth) {
			case 4:  SED1356_REG_CRT_DISP_MODE |= SED1356_CRT_DISP_BLANK | SED1356_CRT_DISP_4BPP;  break;
			case 8:  SED1356_REG_CRT_DISP_MODE |= SED1356_CRT_DISP_BLANK | SED1356_CRT_DISP_8BPP;  break;
			default: SED1356_REG_CRT_DISP_MODE |= SED1356_CRT_DISP_BLANK | SED1356_CRT_DISP_16BPP; break;
		}
		SED1356_REG_CRT_OUT_CTL = SED1356_CRT_OUT_DAC_LVL;
		SED1356_REG_CRT_DISP_START_LO = 0x00;
		SED1356_REG_CRT_DISP_START_MID = 0x00;
		SED1356_REG_CRT_DISP_START_HI = 0x00;
		SED1356_REG_CRT_ADD_OFFSET_LO = (SED_ROW_SIZE(sed_color_depth) / 2) & 0x0ff;
		SED1356_REG_CRT_ADD_OFFSET_HI = ((SED_ROW_SIZE(sed_color_depth) / 2) & 0x700) >> 8;
		SED1356_REG_CRT_PIXEL_PAN = 0x00;
		SED1356_REG_CRT_FIFO_HI_THRES = 0x00;	// auto mode
		SED1356_REG_CRT_FIFO_LO_THRES = 0x00;

		// Disable Cursor
		SED1356_REG_LCD_CURSOR_CTL = 0x00;

		// Disable BitBlt
		SED1356_REG_BLT_CTL_0 = 0x00;
		SED1356_REG_BLT_CTL_1 = 0x00;
		SED1356_REG_BLT_ROP_CODE = 0x00;
		SED1356_REG_BLT_OP = 0x00;
		SED1356_REG_BLT_SRC_START_LO = 0x00;
		SED1356_REG_BLT_SRC_START_MID = 0x00;
		SED1356_REG_BLT_SRC_START_HI = 0x00;
		SED1356_REG_BLT_DEST_START_LO = 0x00;
		SED1356_REG_BLT_DEST_START_MID = 0x00;
		SED1356_REG_BLT_DEST_START_HI = 0x00;
		SED1356_REG_BLT_ADD_OFFSET_LO = 0x00;
		SED1356_REG_BLT_ADD_OFFSET_HI = 0x00;
		SED1356_REG_BLT_WID_LO = 0x00;
		SED1356_REG_BLT_WID_HI = 0x00;
		SED1356_REG_BLT_HGT_LO = 0x00;
		SED1356_REG_BLT_HGT_HI = 0x00;
		SED1356_REG_BLT_BG_CLR_LO = 0x00;
		SED1356_REG_BLT_BG_CLR_HI = 0x00;
		SED1356_REG_BLT_FG_CLR_LO = 0x00;
		SED1356_REG_BLT_FG_CLR_HI = 0x00;

		// Fill the LUT, write to both LCD and CRT luts simultaneously
		SED1356_REG_LUT_MODE = 0x00;
		for (i = 0; i < 256; i++){

			SED1356_REG_LUT_ADD = i;
			SED1356_REG_LUT_DATA = sed_lut[i][0];	// red
			SED1356_REG_LUT_DATA = sed_lut[i][1];	// green
			SED1356_REG_LUT_DATA = sed_lut[i][2];	// blue
		}

		// Disable Power Save Mode
		SED1356_REG_PWR_CFG = 0x00;

		// Set Watchdog						   
		SED1356_REG_WATCHDOG_CTL = 0x00;

	} // else sed1356

	// initialize the globals	
	sed135x_ok = 1;
	sed_fg_color = SED_FG_DEF;
	sed_bg_color = SED_BG_DEF;
	sed_row = 0;
	sed_col = 0;
	sed_fb_offset = 0x00;

	// fill the screen with the background color, remember for the console driver
	// we do double buffering, so fill twice as mush as the active screen size
	switch (sed_color_depth){
		case 4:  temp16 = sed_bg_color | sed_bg_color << 4 | sed_bg_color << 8 | sed_bg_color << 12; break;
		case 8:  temp16 = sed_bg_color | sed_bg_color << 8; break;
		default: temp16 = sed_bg_color; break;
	}
	for (i = 0; i < (PIXELS_PER_COL * PIXELS_PER_ROW) * 4; i += 2){	
		WR_FB16(i, temp16);
	}

	// turn on the display
	sed135x_on();

	return 0;
}

//--------------------------------------------------------------------------
// sed_pwr_dn()
void sed_pwr_dn(){

		// Enable Host Access
		SED1356_REG_MISC = 0x00;

		// Disable the display
		SED1356_REG_DISP_MODE = SED1356_DISP_SWIV_NORM | SED1356_DISP_MODE_OFF;

		// Enable Power Save Mode
		SED1356_REG_REF_RATE |= SED1356_REF_TYPE_SELF;	// set dram to self refresh first
		SED1356_REG_PWR_CFG |= SED1356_PWR_MCLK;

		// Wait until power is down
		while ((SED1356_REG_PWR_STAT & SED1356_PWR_MCLK) == 0){}

}

//--------------------------------------------------------------------------
// sed_tst()
//
// This test will initialize the SED135x, do a frame buffer
// test and then display 16 VGA colors full screen until stopped.
//
#define USAGE_STRING "-[cd:ln]"
#include "sed_tstHelp.c"

int sed_tst(int argc,char *argv[])
{
	ushort wr16, rd16, temp16;
	int i, x, opt;
	ulong sed_fb_end;
	int no_wait = 0;
	char c;

	if (sed135x_ok == 0) return -1;

	sed135x_tst = 1;
	sed135x_off();

    while ((opt=getopt(argc,argv,"clnd:")) != -1) {
        switch(opt) {
	        case 'l':   // Override sed_disp_mode_crt, Run Test in LCD Mode
				sed_disp_mode_crt = 0;
				printf("Forcing LCD Mode!\n");
			break;
	        case 'c':   // Override sed_disp_mode_crt, Run Test in CRT Mode
				sed_disp_mode_crt = 1;
				printf("Forcing CRT Mode!\n");
			break;
			case 'd':	// set the color depth
                switch(*optarg) {
            	    case '4':
	        	        sed_color_depth = 4;
						printf("Forcing 4bpp Mode!\n");
        	            break;
    	            case '8':
            	        sed_color_depth = 8;
						printf("Forcing 8bpp Mode!\n");
                    	break;
    	            case '1':
	                    sed_color_depth = 16;
						printf("Forcing 16bpp Mode!\n");
        	            break;
	                default:	// test with current depth
						break;
                }
	            break;
	        case 'n':   // no waiting fo keypress - fastest operation
				no_wait = 1;
				printf("No Keypress Mode, Must Reset System to Stop!\n");
				break;
			default:	// test with current mode
				break;
		}
	}
	sed135x_init();
	sed135x_on();

	printf("Begin Frame Buffer R/W Test...");
	// do an address=data read/write test on the frame buffer
	// PIXELS_PER_COL * PIXELS_PER_ROW is the highest pixel.  
	// Multiply by bits_per_pixel (sed_color_depth), then
	// divide by 8 to get the actual byte count.
	for (i = 0; i < SED_FB_SIZE(sed_color_depth); i += 2){	
		WR_FB16(i, (i & 0xffff));
		RD_FB16(i, rd16);
		if(rd16 != (i & 0xffff)){
			printf("Failed at 0x%08x, Wrote 0x%04x, Read 0x%04x!\n",SED_MEM_BASE + i, i, rd16);
			return -1;
		}
	} 

	printf("Passed!\n");
	printf("Frame Buffer Start: 0x%08lx, End 0x%08lx\n",SED_MEM_BASE, SED_MEM_BASE + SED_FB_SIZE(sed_color_depth));
	if (no_wait)
	{
		printf("Begin Full Screen Color Test.\n");
		while(1){
			// fill the frame buffer with incrementing color values
			for (x = 0; x < 16; x++){
				switch (sed_color_depth){
					case 4:  wr16 = x | x << 4 | x << 8 | x << 12; break;
					case 8:  wr16 = x | x << 8; break;
					default: wr16 = vga_lookup[x]; break;	// 16-bits bypasses the lookup table
				}
				for (i = 0; i < SED_FB_SIZE(sed_color_depth); i += 2){	
					WR_FB16(i, wr16);
				}
			} // for x
		} // while
	} // no_wait
	else
	{
		printf("Begin Full Screen Color Test, Press any key to end at current color.\n");
		while(1){
			// fill the frame buffer with incrementing color values
			for (x = 0; x < 16; x++){
				switch (sed_color_depth){
					case 4:  wr16 = x | x << 4 | x << 8 | x << 12; break;
					case 8:  wr16 = x | x << 8; break;
					default: wr16 = vga_lookup[x]; break;	// 16-bits bypasses the lookup table
				}
				for (i = 0; i < SED_FB_SIZE(sed_color_depth); i += 2){	
					WR_FB16(i, wr16);
				}
				if (no_wait == 0){
					if (gotachar()) goto sed_tst_next;
				}
			} // for x
		} // while
	} // else no keycheck test

	sed_tst_next:

	sed135x_off();
	sed135x_init();
	sed135x_on();
	return 0;
}

// sed_scroll()
//
// Because we are most likely running out of FLASH and probably also with
// cache disabled, a brute force memcpy of the whole screen would be very
// slow, even with reduced color depths.  Instead, we define a frame buffer
// that is twice the size of our actual display.  This does limit us to a
// 1Mbyte active display size, but 640 x 480 @ 16-bits/pixel = 614K so it
// works just fine.  800 x 600 can be had by reducing the color depth to
// 8-bits/pixel and using the look up tables.
//
// With the double buffering, we always write to the first buffer, even
// when the second buffer is active.  This allows us to scroll by adjusting
// the starting and ending addresses in the SED135x by one row.  When we
// reach the end of our virtual buffer, we reset the starting and ending
// addresses to the first buffer.  Note that we can not adjust the SED135x
// registers until it is in vertical retrace.  That means we have to wait
// until it is in active display, then goes to non-display, unless the
// screen is blanked, in which case we can update immediately.
//
ulong sed_scroll(){

	ushort temp16;

	sed_row++;

	// clear the new row(s)
	sed_clr_row(sed_row);
	if (sed_row >= (ROWS_PER_SCREEN - 1)){
		sed_clr_row(sed_row - ROWS_PER_SCREEN);
	}
	// when sed_y_pos is greater than ROWS_PER_SCREEN we just adjust the
	// start and end addresses in the SED135x.  If it is equal to 2 *
	// ROWS_PER_SCREEN, we reset the start and end addresses to SED_MEM_BASE.
	if (sed_row > (ROWS_PER_SCREEN - 1))
	{
		if (sed_row > ((ROWS_PER_SCREEN * 2) - 1))
		{
			sed_fb_offset = 0x00;
			sed_row = ROWS_PER_SCREEN - 1;
		}
		else
		{
			// calculate the new offset address of the frame buffer in words
			sed_fb_offset += (SED_GET_ADD(1, 0, sed_color_depth) / 2);
		}

		// write the new sed_fb_offset value
		if (sed1355)
		{
			SED1355_REG_DISP1_START_LO  = ((sed_fb_offset & 0x0000ff) >> 0);
			SED1355_REG_DISP1_START_MID = ((sed_fb_offset & 0x00ff00) >> 8);
			SED1355_REG_DISP1_START_HI  = ((sed_fb_offset & 0x030000) >> 16);
		}
		else // sed1356
		{
#if 1
			if (sed_disp_mode_crt){
			// before we change the address offset, wait for the display to 
			// go from active to non-active, unless the display is not enabled
				if (SED1356_REG_DISP_MODE & SED1356_DISP_MODE_CRT){	// CRT is on
					while ((SED1356_REG_CRT_VER_NONDISP & SED1356_VER_NONDISP) == 0){}
					while ((SED1356_REG_CRT_VER_NONDISP & SED1356_VER_NONDISP) == 1){}
				}
				SED1356_REG_CRT_DISP_START_LO  = ((sed_fb_offset & 0x0000ff) >> 0);
				SED1356_REG_CRT_DISP_START_MID = ((sed_fb_offset & 0x00ff00) >> 8);
				SED1356_REG_CRT_DISP_START_HI  = ((sed_fb_offset & 0x030000) >> 16);
			}
			else // LCD
			{
				if (SED1356_REG_DISP_MODE & SED1356_DISP_MODE_CRT){	// LCD is on
					while ((SED1356_REG_LCD_VER_NONDISP & SED1356_VER_NONDISP) == 0){}
					while ((SED1356_REG_LCD_VER_NONDISP & SED1356_VER_NONDISP) == 1){}
				}
				SED1356_REG_LCD_DISP_START_LO  = ((sed_fb_offset & 0x0000ff) >> 0);
				SED1356_REG_LCD_DISP_START_MID = ((sed_fb_offset & 0x00ff00) >> 8);
				SED1356_REG_LCD_DISP_START_HI  = ((sed_fb_offset & 0x030000) >> 16);
			}
#endif
		}
	} // if (sed_row > (ROWS_PER_SCREEN - 1))
	return;
		

} // 

//--------------------------------------------------------------------------
// sed_clr_row()
//
// This routine writes the background color to the font row specified
// printable character
//
void sed_clr_row(int char_row){

	ulong sed_mem_add;
	int i;
	ushort wr16;

	// clear the desired row
	sed_mem_add = SED_GET_ADD(char_row, 0, sed_color_depth);

	switch (sed_color_depth)
	{
		case 4:
			wr16 = ((sed_bg_color << 12) | (sed_bg_color << 8) | (sed_bg_color << 4) | (sed_bg_color << 0));
			for (i = 0; i < ((PIXELS_PER_ROW * FONT_HEIGHT) / 2);i += 2){
				WR_FB16(sed_mem_add + i, wr16);
			} // for font_row
			break;
		case 8:
			wr16 = ((sed_bg_color << 12) | (sed_bg_color << 8) | (sed_bg_color << 4) | (sed_bg_color << 0));
			for (i = 0; i < (PIXELS_PER_ROW * FONT_HEIGHT);i += 2){
				WR_FB16(sed_mem_add + i, wr16);
			} // for font_row
			break;
		case 16:
			wr16 = ((sed_bg_color << 12) | (sed_bg_color << 8) | (sed_bg_color << 4) | (sed_bg_color << 0));
			for (i = 0; i < ((PIXELS_PER_ROW * FONT_HEIGHT) * 2);i += 2){
				WR_FB16(sed_mem_add + i, wr16);
			} // for font_row
			break;
	} // switch sed_color_depth
} // sed_clr_row


//--------------------------------------------------------------------------
// sed_putchar()
//
// This routine parses the character and calls sed_writechar if it is a
// printable character
//
void sed_putchar(char c){

	if ((sed135x_ok == 0) || (sed135x_tst)) return;

	// First parse the character to see if it printable or an acceptable control
    switch (c) {
	    case '\r':
	        sed_col = 0;
	        return;
	    case '\n':
	        sed_scroll();
	        return;
	    case '\b':
	        sed_col--;
	        if (sed_col < 0)
	        {
	            sed_row--;
	            if (sed_row < 0) sed_row = 0;
	            sed_col = COLS_PER_SCREEN - 1;
		    }
			c = 0;		// erase the character
			sed_writechar(c);
	        break;
	    default:
	        if (((uchar)c < FIRST_CHAR) || ((uchar)c > LAST_CHAR)) return; // drop anything we can't print
			c -= FIRST_CHAR;	// get aligned to the first printable character
			sed_writechar(c);
			// advance to next column
			sed_col++;
		    if (sed_col == COLS_PER_SCREEN)
		    {
				sed_col = 0;
			    sed_scroll();
			}
			break;
    } 

} // sed_putchar()

//--------------------------------------------------------------------------
// sed_writechar()
//
// This routine writes the character to the screen at the current cursor
// location.
//
void sed_writechar(uchar c) {

	ulong sed_mem_add;
	int font_row, font_col;
	uchar font_data8;
	ushort font_data16, wr16;

	// Convert the current row,col and color depth values 
	// into an address
	sed_mem_add = SED_GET_ADD(sed_row, sed_col, sed_color_depth);

	if (FONT_WIDTH == 8)
	{
		switch (sed_color_depth){
			case 4:
				// Now render the font by painting one font row at a time
				for (font_row = 0; font_row < FONT_HEIGHT; font_row++){
					// get the font row of data
					font_data8 = font8x16[(c * FONT_HEIGHT) + font_row];
					for (font_col = 0; font_col < 8; font_col += 4)
					{
						// get a words worth of pixels
						wr16 = (((font_data8 & 0x80) ? sed_fg_color << 12 : sed_bg_color << 12)
							  | ((font_data8 & 0x40) ? sed_fg_color << 8 : sed_bg_color << 8)
							  | ((font_data8 & 0x20) ? sed_fg_color << 4 : sed_bg_color << 4)
							  | ((font_data8 & 0x10) ? sed_fg_color << 0 : sed_bg_color << 0));
						font_data8 = font_data8 << 4;
						WR_FB16(sed_mem_add, wr16);
						// if we are in the 2nd frame buffer, write to the 1st frame buffer also
						if (sed_row > (ROWS_PER_SCREEN - 1)){
							WR_FB16((sed_mem_add - SED_FB_SIZE(sed_color_depth)), wr16);
						}
						sed_mem_add += 2;
					} // for font_col
					// go to the next pixel row
					sed_mem_add += (SED_ROW_SIZE(sed_color_depth) - ((FONT_WIDTH * sed_color_depth) / 8));
				} // for font_row
				break;

			case 8:
				// Now render the font by painting one font row at a time
				for (font_row = 0; font_row < FONT_HEIGHT; font_row++){
					// get the font row of data
					font_data8 = font8x16[(c * FONT_HEIGHT) + font_row];
					for (font_col = 0; font_col < 8; font_col += 2)
					{
						// get a words worth of pixels
						wr16 = (((font_data8 & 0x80) ? sed_fg_color << 8 : sed_bg_color << 8)
							  | ((font_data8 & 0x40) ? sed_fg_color << 0 : sed_bg_color << 0));
						font_data8 = font_data8 << 2;
						WR_FB16(sed_mem_add, wr16);
						// if we are in the 2nd frame buffer, write to the 1st frame buffer also
						if (sed_row > (ROWS_PER_SCREEN - 1)){
							WR_FB16((sed_mem_add - SED_FB_SIZE(sed_color_depth)), wr16);
						}
						sed_mem_add += 2;
					} // for font_col
					// go to the next pixel row
					sed_mem_add += (SED_ROW_SIZE(sed_color_depth) - ((FONT_WIDTH * sed_color_depth) / 8));
				} // for font_row
				break;

			case 16:
				// Now render the font by painting one font row at a time
				for (font_row = 0; font_row < FONT_HEIGHT; font_row++){
					// get the font row of data
					font_data8 = font8x16[(c * FONT_HEIGHT) + font_row];
					for (font_col = 0; font_col < 8; font_col++)
					{
						// get a words worth of pixels
						wr16 = ((font_data8 & 0x80) ? sed_fg_color : sed_bg_color);
						font_data8 = font_data8 << 1;
						WR_FB16(sed_mem_add, wr16);
						// if we are in the 2nd frame buffer, write to the 1st frame buffer also
						if (sed_row > (ROWS_PER_SCREEN - 1)){
							WR_FB16((sed_mem_add - SED_FB_SIZE(sed_color_depth)), wr16);
						}
						sed_mem_add += 2;
					} // for font_col
					// go to the next pixel row
					sed_mem_add += (SED_ROW_SIZE(sed_color_depth) - ((FONT_WIDTH * sed_color_depth) / 8));
				} // for font_row
				break;

		} // switch sed_color depth
	} // FONT_WIDTH == 8
	else
	{
		return;
	}
} // sed_writechar()	

//--------------------------------------------------------------------------
// sed_tst()
//
// This test will initialize the SED135x, do a frame buffer
// test and then display 16 VGA colors full screen until stopped.
//
char *sedHelp[] = {
	" This command allows the user to set",
	" options for the SED135x Video Controller",
	" found on some Cogent Single Boards.",
	" The user may set the mode to CRT or LCD, ",
	" set the color depth to run at, and set",
	" the foreground and background colors.",
	" Note that all but the foreground and",
	" background options will cause the",
	" S1D1350x to be re-initialized.\n",
    " Usage:",
    " sed -[c,l,f,b,d:[4,8,16]]",
    " Options...",
    " -c force display to CRT mode, overrides auto detect",
    " -l force display to LCD mode, overrides auto detect",
    " -fx set foreground color to x (VGA 16 Color Pallette 0-15)",
    " -bx set background color to x (VGA 16 Color Pallette 0-15)",
	" -d4 set a depth of 4-bits/pixel: Power-On Default",
	" -d8 set a depth of 8-bits/pixel",
	" -d16 set a depth of 16-bits/pixel",
	" No options will return current status",
	0
};

int sed(int argc,char *argv[])
{
	ushort wr16, rd16, temp16;
	int i, x, opt, reinit;
	ulong sed_fb_end;
	int no_wait = 0;
	char c;

	if (sed135x_ok == 0){
		printf("S1D13505 Not Found!\n");
		 return -1;
	}

	reinit = 0;
    if (argc == 1) {
		if(sed1355) printf("S1D13505 @ 0x%08lx, ", SED_REG_BASE);
		else printf("S1D13506 @ 0x%08lx, ", SED_REG_BASE);
		if(sed_disp_mode_crt) printf("CRT Mode, ");
		else  printf("CRT Mode, ");
		printf("Color Depth = %d,\n", sed_color_depth);
		printf("Foreground Color = %d, ", sed_fg_color);
		printf("Background Color = %d, \n", sed_bg_color);
		return;
    }

    while ((opt=getopt(argc,argv,"clf:b:d:")) != -1) {
        switch(opt) {
	        case 'l':   // Override sed_disp_mode_crt, Run Test in LCD Mode
				sed_disp_mode_crt = 0;
				reinit = 1;
				printf("Setting LCD Mode!\n");
				break;
	        case 'c':   // Override sed_disp_mode_crt, Run Test in CRT Mode
				sed_disp_mode_crt = 1;
				reinit = 1;
				printf("Setting CRT Mode!\n");
				break;
			case 'd':	// set the color depth
                switch(*optarg) {
            	    case '4':
	        	        sed_color_depth = 4;
						printf("Setting 4bpp Mode!\n");
						reinit = 1;
        	            break;
    	            case '8':
            	        sed_color_depth = 8;
						printf("Setting 8bpp Mode!\n");
						reinit = 1;
                    	break;
    	            case '1':
	                    sed_color_depth = 16;
						printf("Setting 16bpp Mode!\n");
						reinit = 1;
        	            break;
	                default:	// test with current depth
						printf("Unsupported Color Depth, Use 4, 8 or 16!\n");
						break;
                }
	            break;
	        case 'f':	// set foreground color
				sed_fg_color = atoi(optarg);
				printf("New Foreground Color = %d.\n", sed_fg_color);
				break;
	        case 'b':	// set foreground color
				sed_bg_color = atoi(optarg);
				printf("New Background Color = %d.\n", sed_bg_color);
				break;
			default:
				break;
		}
	}
	if (reinit){
		printf("Re-initializing S1D13505.\n");
		sed135x_off();
		sed135x_init();
		sed135x_on();
	}
	return 0;
} // sed()

