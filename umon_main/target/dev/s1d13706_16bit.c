//==========================================================================
//
//      s1d13706_16bit.c
//
// Author(s):   Michael Kelly - Cogent Computer Systems, Inc.
// Date:        09/14/03
// Description:	Init Code for S1D13706 Display Controller
//
//==========================================================================

#include "config.h"
#include "cpuio.h"
#include "genlib.h"
#include "stddefs.h"
#include "s1d13706_16bit.h"
#include "sed_lut.h"

//--------------------------------------------------------------------------
// function prototypes
//
void s1d_init(void);
void lcd_bkl(uchar bright);
void lcd_pwr(uchar bright);

extern int udelay(int delay);

// Global array to hold LCD parameters
typedef struct
{
	ushort type;		// 0 = passive, 1 = TFT, 2 = HR-TFT
	ushort pclk;		// pixel clock divider
	ushort hor_total	// horizontal total time PCLK's
	ushort hor_fpw		// horizontal front porch width in PCLK's
	ushort hor_bpw		// horizontal back porch width in PCLK's
	ushort hor_spw		// horizontal sync pulse width in PCLK's
	ushort VERT_TOTAL
	ushort VERT_PERIOD
	ushort VERT_START
	ushort LP_WIDTH
	ushort LP_START
	ushort FP_WIDTH
	ushort FP_START
	ushort DISP_MODE_EFFECTS = s1d_reg[12];	//	Display Mode and Effects
} LCD_INFO *lcd_info;

ushort s1d_reg[14];	// 0-12 hold registers values, 13 = default pwm duty cycle

// S1D13706 GPIO's used to control power and backlighting:
// CVOUT = Backlight, 0 = off, 1 = on, duty cycle for intensity control
// PWMOUT = Vee (50% to 100% duty cycle for SW contrast control, 0 or 1 for analog control)
// GPIO4 = LCD Enable - High True
// DRDY = ACBIAS for Passive Displays, Display Enable for Active Displays

//--------------------------------------------------------------------------
// s1d_init
//
// This function enables the lcd controller and the lcd
//
void s1d_init()
{

	// setup the lcd from the global structure (it must be initialzed by the 
	// calling function before we are called)
	S1D_REG_MCLK_PCLK = s1d_reg[0];				//	MCLK Config
	S1D_REG_PANEL_AND_MOD_RATE = s1d_reg[1];	//	Panel Type
	S1D_REG_HOR_TOTAL = s1d_reg[2];				//	Horizontal Total
	S1D_REG_HOR_PERIOD = s1d_reg[3];			//	Horizontal Period
	S1D_REG_HOR_START = s1d_reg[4];				//	Horizontal Start
	S1D_REG_VERT_TOTAL = s1d_reg[5];			//	Vertical Total
	S1D_REG_VERT_PERIOD = s1d_reg[6];			//	Vertical Period
	S1D_REG_VERT_START = s1d_reg[7];			//	Vertical Start
	S1D_REG_LP_WIDTH = s1d_reg[8];				//	Line Pulse Width
	S1D_REG_LP_START = s1d_reg[9];				//	Line Pulse Start
	S1D_REG_FP_WIDTH = s1d_reg[10];				//	Frame Pulse Width
	S1D_REG_FP_START = s1d_reg[11];				//	Frame Pulse Start
	S1D_REG_DISP_MODE_EFFECTS = s1d_reg[12];	//	Display Mode and Effects


	// turn the LCD backlight off
	lcd_bkl(0);

	

	// Turn on the LCD and then the backlight
	SED1356_REG_LCD_DISP_MODE_and_MISC &= ~H2S1D(SED1356_LCD_DISP_BLANK);
	lcd_bkl(0xff);		// turn the LCD backlight on/full brightness


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
	if (bright) SED1356_BKL_ON;
	else SED1356_BKL_OFF;

}

//--------------------------------------------------------------------------
// sed135x_off
//
// This function turns off the SED1356 LCD and/or CRT and the display
// fifo.  It can also turn off the clocks if mode is true, thus allowing
// the programmable clock generator to be changed.
//
void sed135x_off()
{
	SED1356_REG_DISP_MODE = H2S1D(SED1356_DISP_SWIV_NORM | SED1356_DISP_MODE_OFF);
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
	sed_disp_mode_crt = 0;	// assume LCD

	// enable host access
	SED1356_REG_REV_and_MISC = 0x0000;

	// Check the ID to make sure we even have a SED1356 installed
	temp16 = SED1356_REG_REV_and_MISC & H2S1D(SED1356_REV_ID_MASK);

	if (temp16 != H2S1D(SED1356_REV_ID_1356)){
		printf("SED1356 Not Found! SED_REG_REV = %04x.\n", temp16);
		return -1;
	}

	// Disable the display
	SED1356_REG_DISP_MODE = H2S1D(SED1356_DISP_SWIV_NORM | SED1356_DISP_MODE_OFF);

	// Test for the presence of a CRT
	SED1356_REG_GPIO_CTL = 0x0000;						// Disable Backlight 
	SED1356_REG_GPIO_CFG = H2S1D(SED1356_GPIO_GPIO1);	// GPIO1 Out, GPIO2 In
	if (SED1356_CRT) sed_disp_mode_crt = 0; 
	else sed_disp_mode_crt = 1;

	// Enable Power Save Mode before we mess with the clocks
	SED1356_REG_MEM_CFG_and_REF_RATE = H2S1D(SED1356_REF_TYPE_SELF);	// set dram to self refresh first
	// shut off MCLK
	SED1356_REG_PWR_CFG_and_STAT = H2S1D(SED1356_PWR_MCLK);

	// Wait until power is down - when MCLK bit goes true
	while ((SED1356_REG_PWR_CFG_and_STAT & H2S1D(SED1356_PWR_MCLK)) == 0){}

	// Change the programmable clock generator to the desired timing
	if (sed_disp_mode_crt) fs6377_init(SED_DISP_MODE_CRT);
	else fs6377_init(SED_DISP_MODE_LCD);

	// Re-enable Power
	SED1356_REG_PWR_CFG_and_STAT = 0x0000;

	// Common Control Registers
	SED1356_REG_MCLK_CFG = H2S1D(SED1356_MCLK_SRC_BCLK);
	SED1356_REG_LCD_PCLK_CFG = H2S1D(SED1356_PCLK_SRC_CLKI);
	SED1356_REG_CRT_PCLK_CFG = H2S1D(SED1356_PCLK_SRC_CLKI);
	SED1356_REG_MEDIA_PCLK_CFG = 0x0000;
	SED1356_REG_WAIT_STATE = H2S1D(0x0001);
	SED1356_REG_MEM_CFG_and_REF_RATE = H2S1D(SED1356_MEM_CFG_2CAS_EDO | SED1356_REF_RATE_2048);
	SED1356_REG_MEM_TMG0_and_1 = H2S1D(SED1356_MEM_TMG0_EDO50_MCLK33 | SED1356_MEM_TMG1_EDO50_MCLK33);
	SED1356_REG_PANEL_TYPE_and_MOD_RATE = H2S1D(SED1356_PANEL_TYPE_16 | SED1356_PANEL_TYPE_CLR | SED1356_PANEL_TYPE_TFT);

	// LCD Specific Registers
	SED1356_REG_LCD_HOR_DISP = H2S1D((PIXELS_PER_ROW/8) - 1);
	SED1356_REG_LCD_HOR_NONDISP_and_START = H2S1D(SED_HOR_NONDISP_LCD | (SED_HOR_PULSE_START_LCD << 8));
	SED1356_REG_LCD_HOR_PULSE = H2S1D(SED1356_PULSE_WID(SED_HOR_PULSE_WIDTH_LCD) | SED1356_PULSE_POL_LOW);
	SED1356_REG_LCD_VER_DISP_HT_LO_and_HI = H2S1D((PIXELS_PER_COL - 1) & 0x3ff);
	SED1356_REG_LCD_VER_NONDISP_and_START = H2S1D(SED_VER_NONDISP_LCD | (SED_VER_PULSE_START_LCD << 8));
	SED1356_REG_LCD_VER_PULSE = H2S1D(SED1356_PULSE_WID(SED_VER_PULSE_WIDTH_LCD) | SED1356_PULSE_POL_LOW);
	switch (sed_color_depth) {
		case 4:  SED1356_REG_LCD_DISP_MODE_and_MISC = H2S1D(SED1356_LCD_DISP_BLANK | SED1356_LCD_DISP_SWIV_NORM | SED1356_LCD_DISP_4BPP);  break;
		case 8:  SED1356_REG_LCD_DISP_MODE_and_MISC = H2S1D(SED1356_LCD_DISP_BLANK | SED1356_LCD_DISP_SWIV_NORM | SED1356_LCD_DISP_8BPP);  break;
		default: SED1356_REG_LCD_DISP_MODE_and_MISC = H2S1D(SED1356_LCD_DISP_BLANK | SED1356_LCD_DISP_SWIV_NORM | SED1356_LCD_DISP_16BPP); break;
	}

	SED1356_REG_LCD_DISP_START_LO_and_MID = 0x0000;
	SED1356_REG_LCD_DISP_START_HI = 0x0000;
	SED1356_REG_LCD_ADD_OFFSET_LO_and_HI = H2S1D((SED_ROW_SIZE(sed_color_depth) / 2) & 0x7ff);
	SED1356_REG_LCD_PIXEL_PAN = 0x0000;
	SED1356_REG_LCD_FIFO_THRESH_LO_and_HI = 0x0000;	// auto mode

	// LCD Specific Registers
	SED1356_REG_CRT_HOR_DISP = H2S1D((PIXELS_PER_ROW/8) - 1);
	SED1356_REG_CRT_HOR_NONDISP_and_START = H2S1D(SED_HOR_NONDISP_CRT | (SED_HOR_PULSE_START_CRT << 8));
	SED1356_REG_CRT_HOR_PULSE = H2S1D(SED1356_PULSE_WID(SED_HOR_PULSE_WIDTH_CRT) | SED1356_PULSE_POL_LOW);
	SED1356_REG_CRT_VER_DISP_HT_LO_and_HI = H2S1D((PIXELS_PER_COL - 1) & 0x3ff);
	SED1356_REG_CRT_VER_NONDISP_and_START = H2S1D(SED_VER_NONDISP_CRT | (SED_VER_PULSE_START_CRT << 8));
	SED1356_REG_CRT_VER_PULSE_and_OUT_CTL = H2S1D(SED1356_PULSE_WID(SED_VER_PULSE_WIDTH_CRT) | SED1356_PULSE_POL_LOW | SED1356_CRT_OUT_DAC_LVL);
	switch (sed_color_depth) {
		case 4:  SED1356_REG_CRT_DISP_MODE = H2S1D(SED1356_CRT_DISP_BLANK | SED1356_CRT_DISP_4BPP);  break;
		case 8:  SED1356_REG_CRT_DISP_MODE = H2S1D(SED1356_CRT_DISP_BLANK | SED1356_CRT_DISP_8BPP);  break;
		default: SED1356_REG_CRT_DISP_MODE = H2S1D(SED1356_CRT_DISP_BLANK | SED1356_CRT_DISP_16BPP); break;
	}


	SED1356_REG_CRT_DISP_START_LO_and_MID = 0x0000;
	SED1356_REG_CRT_DISP_START_HI = 0x0000;
	SED1356_REG_CRT_ADD_OFFSET_LO_and_HI = H2S1D((SED_ROW_SIZE(sed_color_depth) / 2) & 0x7ff);
	SED1356_REG_CRT_PIXEL_PAN = 0x0000;
	SED1356_REG_CRT_FIFO_THRESH_LO_and_HI = 0x0000;	// auto mode

	// Disable Cursor
	SED1356_REG_LCD_CURSOR_CTL_and_START_ADD = 0x0000;
	SED1356_REG_CRT_CURSOR_CTL_and_START_ADD = 0x0000;

	// Disable BitBlt
	SED1356_REG_BLT_CTL_0_and_1 = 0x0000;
	SED1356_REG_BLT_ROP_CODE_and_BLT_OP = 0x0000;
	SED1356_REG_BLT_SRC_START_LO_and_MID = 0x0000;
	SED1356_REG_BLT_SRC_START_HI = 0x0000;
	SED1356_REG_BLT_DEST_START_LO_and_MID = 0x0000;
	SED1356_REG_BLT_DEST_START_HI = 0x0000;
	SED1356_REG_BLT_ADD_OFFSET_LO_and_HI = 0x0000;
	SED1356_REG_BLT_WID_LO_and_HI = 0x0000;
	SED1356_REG_BLT_HGT_LO_and_HI = 0x0000;
	SED1356_REG_BLT_BG_CLR_LO_and_HI = 0x0000;
	SED1356_REG_BLT_FG_CLR_LO_and_HI = 0x0000;

	// Fill the LUT, write to both LCD and CRT luts simultaneously
	SED1356_REG_LUT_MODE = 0x0000;
	for (i = 0; i < 256; i++){

		SED1356_REG_LUT_ADD = H2S1D(i);
		SED1356_REG_LUT_DATA = H2S1D(sed_lut_16bit[i][0]);	// red
		SED1356_REG_LUT_DATA = H2S1D(sed_lut_16bit[i][1]);	// green
		SED1356_REG_LUT_DATA = H2S1D(sed_lut_16bit[i][2]);	// blue
	}

	// Disable Power Save Mode
	SED1356_REG_PWR_CFG_and_STAT = 0x0000;

	// Set Watchdog						   
	SED1356_REG_WATCHDOG_CTL = 0x0000;

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
		SED1356_REG_REV_and_MISC = 0x0000;

		// Disable the display
		SED1356_REG_DISP_MODE = H2S1D(SED1356_DISP_MODE_OFF);

		// Enable Power Save Mode
		// set dram to self refresh first
		SED1356_REG_MEM_CFG_and_REF_RATE = H2S1D(SED1356_REF_TYPE_SELF);

		// shut off MCLK
		SED1356_REG_PWR_CFG_and_STAT = H2S1D(SED1356_PWR_MCLK);

		// Wait until power is down - when MCLK bit goes true
		while ((SED1356_REG_PWR_CFG_and_STAT & H2S1D(SED1356_PWR_MCLK)) == 0){}
}

//--------------------------------------------------------------------------
// sed_tst()
//
// This test will initialize the SED135x, do a frame buffer
// test and then display 16 VGA colors full screen until stopped.
//
#define SED_I_OPTION 
#define SED_S_OPTION 
#define USAGE_STRING "-[cd:ilns]"
#include "sed_tstHelp.c"

int sed_tst(int argc,char *argv[])
{
	volatile ushort wr16, rd16, temp16;
	int i, x, opt;
	int no_wait = 0;
	int init_only = 0;
	char c;

	sed135x_tst = 1;
	sed135x_off();

    while ((opt=getopt(argc,argv,"clnsid:")) != -1) {
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
	        case 'n':   // no waiting for keypress - fastest operation
				no_wait = 1;
				printf("No Keypress Mode, Must Reset System to Stop!\n");
				break;
	        case 'i':   // init only 
				no_wait = 1;
				printf("Initializing SED, Skipping tests!\n");
				init_only = 1;
				break;
	        case 's':   // Scope loop
				no_wait = 1;
				printf("Scope Loop, press any key to Stop!\n");
				while(1){
					SED1356_REG_REV_and_MISC = 0;
					temp16 = SED1356_REG_REV_and_MISC;
					if (gotachar()) break;
				}
				break;
			default:	// test with current mode
				break;
		}
	}
	sed135x_init();
	if (sed135x_ok == 0) return -1;

	sed135x_on();

	if (init_only) return 0;

	printf("Frame Buffer R/W...");
	// do an address=data read/write test on the frame buffer
	// PIXELS_PER_COL * PIXELS_PER_ROW is the highest pixel.  
	// Multiply by bits_per_pixel (sed_color_depth), then
	// divide by 8 to get the actual byte count.
	for (i = 0; i < SED_FB_SIZE(sed_color_depth); i += 4){	
		WR_FB16(i, (i & 0xffff));
		RD_FB16(i, rd16);
		if(rd16 != (i & 0xffff)){
			printf("Fail at 0x%08lx, WR 0x%08x, RD 0x%04x!\n",
				(long)(SED_MEM_BASE + i), i, rd16);
//			return -1;
		}
	} 

	printf("OK!\n");

	printf("Frame Buffer Start: 0x%08lx, End 0x%08lx\n",(long)SED_MEM_BASE,
		(long)(SED_MEM_BASE + SED_FB_SIZE(sed_color_depth)));
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
		printf("Begin Full Screen Color Test, Press any key to go to next color, \'x\' to end.\n");
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
					c = getchar();
					if (c == 'x') goto sed_tst_next;
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
void
sed_scroll()
{
	sed_row++;

	// clear the new row(s)
	sed_clr_row(sed_row);
	if (sed_row > (ROWS_PER_SCREEN - 1)){
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
		if (sed_disp_mode_crt){
		// before we change the address offset, wait for the display to 
		// go from active to non-active, unless the display is not enabled
			if (SED1356_REG_DISP_MODE & H2S1D(SED1356_DISP_MODE_CRT)){	// CRT is on
				while ((SED1356_REG_CRT_VER_NONDISP_and_START & H2S1D(SED1356_VER_NONDISP)) == 0){}
				while ((SED1356_REG_CRT_VER_NONDISP_and_START & H2S1D(SED1356_VER_NONDISP)) == 1){}
			}
			SED1356_REG_CRT_DISP_START_LO_and_MID  = H2S1D(((sed_fb_offset & 0x00ffff) >> 0));
			SED1356_REG_CRT_DISP_START_HI  = H2S1D(((sed_fb_offset & 0x030000) >> 16));
		}
		else // LCD
		{
			if (SED1356_REG_DISP_MODE & H2S1D(SED1356_DISP_MODE_LCD)){	// LCD is on
				while ((SED1356_REG_LCD_VER_NONDISP_and_START & H2S1D(SED1356_VER_NONDISP)) == 0){}
				while ((SED1356_REG_LCD_VER_NONDISP_and_START & H2S1D(SED1356_VER_NONDISP)) == 1){}
			}
			SED1356_REG_LCD_DISP_START_LO_and_MID  = H2S1D(((sed_fb_offset & 0x00ffff) >> 0));
			SED1356_REG_LCD_DISP_START_HI  = H2S1D(((sed_fb_offset & 0x030000) >> 16));
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

#ifdef SED_DBG
	sed135x_tst = 1;
	printf("SED Clear Row %d, FB Add 0x%08lx, CPU Add 0x%08lx.\n ", char_row, sed_mem_add, SED_GET_PHYS_ADD(sed_mem_add));
	sed135x_tst = 0;
#endif

	switch (sed_color_depth)
	{
		case 4:
			wr16 = ((sed_bg_color << 12) | (sed_bg_color << 8) | (sed_bg_color << 4) | (sed_bg_color << 0));
#ifdef SED_DBG
	sed135x_tst = 1;
	printf("SED Clear Row %d, FB Add 0x%08lx to 0x%08lx.\n ", char_row, sed_mem_add, sed_mem_add + ((PIXELS_PER_ROW * FONT_HEIGHT) / 2));
	sed135x_tst = 0;
#endif
			for (i = 0; i < ((PIXELS_PER_ROW * FONT_HEIGHT) / 2); i += 2){
				WR_FB16(sed_mem_add, wr16);
				sed_mem_add += 2;
			} // for font_row
			break;
		case 8:
			wr16 = ((sed_bg_color << 12) | (sed_bg_color << 8) | (sed_bg_color << 4) | (sed_bg_color << 0));
			for (i = 0; i < (PIXELS_PER_ROW * FONT_HEIGHT); i += 2){
				WR_FB16(sed_mem_add, wr16);
				sed_mem_add += 2;
			} // for font_row
			break;
		case 16:
			wr16 = ((sed_bg_color << 12) | (sed_bg_color << 8) | (sed_bg_color << 4) | (sed_bg_color << 0));
			for (i = 0; i < ((PIXELS_PER_ROW * FONT_HEIGHT) * 2); i += 2){
				WR_FB16(sed_mem_add, wr16);
				sed_mem_add += 2;
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

	if ((sed135x_ok == 0) || (sed135x_tst == 1)) return;

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
	ushort wr16;

	// Convert the current row,col and color depth values 
	// into an address
	sed_mem_add = SED_GET_ADD(sed_row, sed_col, sed_color_depth);

#ifdef SED_DBG
	sed135x_tst = 1;
	printf("SED writechar at row %d, col %d, FB Add 0x%08lx, CPU Add 0x%08lx.\n ", sed_row, sed_col, sed_mem_add, SED_GET_PHYS_ADD(sed_mem_add));
	sed135x_tst = 0;
#endif

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
	" options for the S1D13506 Video Controller",
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

int
sed(int argc,char *argv[])
{
	int	opt, reinit;

	if (sed135x_ok == 0){
		printf("S1D13506 Not Found!\n");
		 return -1;
	}

	reinit = 0;
    if (argc == 1) {
		printf("S1D13506 @ 0x%08lx, ", (ulong)SED_REG_BASE);
		if(sed_disp_mode_crt) printf("CRT Mode, ");
		else  printf("LCD Mode, ");
		printf("Color Depth = %d,\n", sed_color_depth);
		printf("Foreground Color = %d, ", sed_fg_color);
		printf("Background Color = %d, \n", sed_bg_color);
		return(0);
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
		printf("Re-initializing S1D13506.\n");
		sed135x_off();
		sed135x_init();
		sed135x_on();
	}
	return 0;
} // sed()

