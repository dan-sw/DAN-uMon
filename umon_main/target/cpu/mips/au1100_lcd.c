//==========================================================================
//
//      au1100_lcd.c
//
// Author(s):   Michael Kelly - Cogent Computer Systems, Inc.
// Date:        04/11/2003
// Description:	Init Code for Au1100 LCD Controller
//
//==========================================================================

#include "config.h"
#include "cpuio.h"
#include "stddefs.h"
#include "genlib.h"
#include "au1100.h"
#include "au1100_lcd.h"
#include "font8x16.h"
#include "lcd_lut.h"

#define LCD_DBG

//--------------------------------------------------------------------------
// The LCD frame buffer is fixed at 0xa1000000, which is 1Mbyte from the
// beginning of SDRAM space.  Note that we access it 16-bits at a time.
//
#define LCD_BUF_ADD		0xa1000000
#define LCD_BUF(_x_)	*(vushort *)(LCD_BUF_ADD + _x_)	// Frame Buffer

void lcd_putchar(char c);
void lcd_writechar(uchar c);
void lcd_clr_row(int char_row);

extern void udelay(int);

const ushort vga_lookup[] = {
LU_BLACK,		
LU_BLUE,			
LU_GREEN,		
LU_CYAN,			
LU_RED,			
LU_VIOLET,		
LU_YELLOW,		
LU_GREY,			
LU_WHITE,		
LU_BRT_BLUE,		
LU_BRT_GREEN,	
LU_BRT_CYAN,		
LU_BRT_RED,		
LU_BRT_VIOLET,
LU_BRT_YELLOW,
LU_BRT_WHITE	
};

// globals to keep track of foreground, background colors and x,y position
int lcd_color_depth;		// 4, 8 or 16
int lcd_fg_color;			// 0 to 15, used as lookup into VGA color table
int lcd_bg_color;			// 0 to 15, used as lookup into VGA color table
int lcd_col;				// current column, 0 to COLS_PER_SCREEN - 1
int lcd_row;				// current row, 0 to (ROWS_PER_SCREEN * 2) - 1
int lcd_tst_mode = 0;
ulong lcd_fb_offset;	   	// current offset into frame buffer for lcd_putchar

#define PIXELS_PER_ROW			640
#define PIXELS_PER_COL			480

#define ROWS_PER_SCREEN			30
#define COLS_PER_SCREEN			80

#define LCD_BG_DEF				1
#define LCD_FG_DEF				14

#define LCD_FB_SIZE(_depth_)				(((PIXELS_PER_COL * PIXELS_PER_ROW) * _depth_) / 8)

#define LCD_ROW_SIZE(_depth_)				((PIXELS_PER_ROW * _depth_) / 8)

#define LCD_GET_ADD(_row_, _col_, _depth_)	(((((_row_ * PIXELS_PER_ROW) * FONT_HEIGHT) \
											  + (_col_ * FONT_WIDTH)) \
											  * _depth_) / 8)

//--------------------------------------------------------------------------
// lcd_init
//
// This function sets up the Au1100 LCD controller
//
int lcd_init()
{
	ulong index;
	int i;
	ushort temp16;

#ifdef LCD_DBG
	lcd_tst_mode = 1;
	printf("Au1100 LCD Initialization.\n");
	lcd_tst_mode = 0;
#endif

	// Setup LCD Clock Generator for 96Mhz from Auxillary PLL
	// This allows us to use a PCD = 1 which divides this
	// by 4 for 24Mhz.  We use the Auxillary PLL because it
	// will always be set it to 96Mhz regardless of the CPU
	// frequency
	AU1100_SYS_REG(sys_clksrc) |= SYS_CLKSRC_ML(1);	// direct from MUX

	LCD_REG(LCD_CLK) = (LCD_CLK_PCD(1)				// divide by 4
  				      | LCD_CLK_FCLK_NEG			// clock polarity
  				      | LCD_CLK_LCLK_NEG
  				      | LCD_CLK_PCLK_NEG
  				      | LCD_CLK_BIAS_POS);

#ifdef LCD_DBG
	lcd_tst_mode = 1;
	printf("Au1100 LCD_CLK = 0x%08lx.\n", LCD_REG(LCD_CLK));
	lcd_tst_mode = 0;
#endif

	// set the PWM frequency to lowest possible
	LCD_REG(LCD_PWMHI) = 0;		// clear high time so always off
	LCD_REG(LCD_PWMDIV) = LCD_PWMDIV_DIV(0xfff);
	LCD_REG(LCD_PWMDIV) = LCD_PWMDIV_EN;


	// Setup Timing Registers
	// leave the go bit off for now.  It will be enabled
	// in the lcd enable function
	LCD_REG(LCD_CTL) = (LCD_CTL_TFT								// TFT Panel
					   | LCD_CTL_PO0							// Little Endian Pixel Ordering - All BPP settings
					   | LCD_CTL_TFT_D16						// 16-Bit Data Interface
					   | LCD_CTL_COH							// mark LCD transfers as coherent
//					   | LCD_CTL_COLOR							// Color display
				       | LCD_CTL_PPF_565);						// RGB 565 ordering on pins

	// set the bpp based on lcd_color_depth
	switch (lcd_color_depth)
	{
		case 4:
			LCD_REG(LCD_CTL) |= LCD_CTL_BPP4;					// 4-bits/pixel
			break;
		case 8:
			LCD_REG(LCD_CTL) |= LCD_CTL_BPP8;					// 8-bits/pixel
			break;
		default:
			LCD_REG(LCD_CTL) |= LCD_CTL_BPP16;					// 16-bits/pixel
			break;
	} // switch color depth
#ifdef LCD_DBG
	lcd_tst_mode = 1;
	printf("Au1100 LCD_CTL = 0x%08lx.\n", LCD_REG(LCD_CTL));
	lcd_tst_mode = 0;
#endif

	LCD_REG(LCD_VER) = (LCD_VER_VN1(32)							// Beginning-of-frame line clock wait count = VN1 + 1
					   | LCD_VER_VN2(32)						// End-of-frame line clock wait count = VN2 + 1
					   | LCD_VER_VPW(1)						// Vertical sync pulse width = VPW + 1
					   | LCD_VER_LPP((PIXELS_PER_COL - 1)));	// Lines per panel = LPP + 1
#ifdef LCD_DBG
	lcd_tst_mode = 1;
	printf("Au1100 LCD_VER = 0x%08lx.\n", LCD_REG(LCD_VER));
	lcd_tst_mode = 0;
#endif

	LCD_REG(LCD_HOR) = (LCD_HOR_HN1(80)  				 		// Beginning-of-Line pixel clock Wait count = HN1 + 1
	 		    	   | LCD_HOR_HN2(80)  						// End-of-line pixel clock wait count = HN2 + 1
	 		    	   | LCD_HOR_HPW(2)  						// Horizontal sync pulse width = HPW + 1
			 		   | LCD_HOR_PPL((PIXELS_PER_ROW - 1)));	// Pixels per line = PPL + 1

#ifdef LCD_DBG
	lcd_tst_mode = 1;
	printf("Au1100 LCD_HOR = 0x%08lx.\n", LCD_REG(LCD_HOR));
	lcd_tst_mode = 0;
#endif

	LCD_REG(LCD_WORDS) = (LCD_FB_SIZE(lcd_color_depth) / 2) - 1;		// frame buffer size in 16-bit words
#ifdef LCD_DBG
	lcd_tst_mode = 1;
	printf("Au1100 LCD_WORDS = 0x%08lx.\n", LCD_REG(LCD_WORDS));
	lcd_tst_mode = 0;
#endif

	index = 0;
	// Fill the pallette buffer with the lookup table data
	// note this buffer is unused in 16-bit/pixel mode
	for (i = 0; i < 256; i ++)
	{
		LCD_REG(LCD_PALLETE + index) = LCD_PALLETE_TFT((((lcd_lut[i][0] & 0xf8) << 8)	// Red
													 | ((lcd_lut[i][1] & 0xfc) << 3) 	// Green 
													 | (lcd_lut[i][2] >> 3)));			// Blue

		index += 4;
	}

	// initialize the globals	
	lcd_fg_color = LCD_FG_DEF;
	lcd_bg_color = LCD_BG_DEF;
	lcd_row = 0;
	lcd_col = 0;
	lcd_fb_offset = 0x00;

	// fill the screen with the background color, remember for the console driver
	// we do double buffering, so fill twice as mush as the active screen size
	switch (lcd_color_depth){
		case 4:  temp16 = lcd_bg_color | lcd_bg_color << 4 | lcd_bg_color << 8 | lcd_bg_color << 12; break;
		case 8:  temp16 = lcd_bg_color | lcd_bg_color << 8; break;
		default: temp16 = lcd_bg_color; break;
	}
	for (i = 0; i < (PIXELS_PER_COL * PIXELS_PER_ROW) * 2; i += 2){	
		LCD_BUF(i) = temp16;
	}

	return 0;
}

//--------------------------------------------------------------------------
// lcd_on
//
// This function turns on the PXA250 LCD Controller and the Backlight Inverter
//
void lcd_on()
{
	// Make sure it's off first
	lcd_off();

	// Turn on the LCD Controller
	LCD_REG(LCD_DMA0) = LCD_DMA0_ADD(LCD_BUF_ADD);			// frame buffer start address
#ifdef LCD_DBG
	lcd_tst_mode = 1;
	printf("Au1100 LCD_DMA0 = 0x%08lx.\n", LCD_REG(LCD_DMA0));
	lcd_tst_mode = 0;
#endif

	LCD_REG(LCD_CTL) |= LCD_CTL_GO;							// Go!
#ifdef LCD_DBG
	lcd_tst_mode = 1;
	printf("Au1100 LCD_CTL = 0x%08lx.\n", LCD_REG(LCD_CTL));
	lcd_tst_mode = 0;
#endif

	udelay(1000);

	// Enable the Backlight
	LCD_REG(LCD_PWMHI) |= LCD_PWMHI_PWM0(0xfff);			// 100% duty cycle = always on

	// wait for power-up sequence to complete
	udelay(10000);
}

//--------------------------------------------------------------------------
// lcd_off
//
// This function turns off the LCD Controller and the LCD backlight
//
void lcd_off()
{

	return;

	// Turn off the LCD Controller by clearing the go bit
	// first see if it was on
	if (LCD_REG(LCD_CTL) & LCD_CTL_GO)
	{
		LCD_REG(LCD_CTL) &= ~LCD_CTL_GO;
		
		// Wait for the shutdown bit to go true
//		while(!(LCD_REG(LCD_ISTAT) & LCD_INT_SD)) {}
		udelay(10000);
	}

	// Turn off the backlight
	LCD_REG(LCD_PWMHI) &= ~(LCD_PWMHI_PWM0(0xfff));
}

char *lcd_tstHelp[] = {
	" This command allows the user to test ",
	" the Au1100 LCD Controller.  The user ",
	" may set the color depth to run the ",
	" test at.  Note that the frame buffer ",
	" R/W test will test all of the frame ",
	" buffer regardless of depth.\n",
    " Usage:",
    " sed_tst -[n,x,d:[4,8,16]]",
    " Options...",
    " -n run test without keycheck - CAUTION: RESET SYSTEM TO STOP!",
	" -d4 run test, force a depth of 4-bits/pixel",
	" -d8 run test, force a depth of 8-bits/pixel",
	" -d16 run test, force a depth of 16-bits/pixel",
	" -x init only, do not run frame buffer tests",
	" no options, default to current mode and depth",
	0
};

int lcd_tst(int argc,char *argv[])
{
	volatile ushort wr16, rd16;
	int i, x, opt;
	int no_wait = 0;
	int init_only = 0;
	char c;

	lcd_tst_mode = 1;
	lcd_off();

    while ((opt=getopt(argc,argv,"clnsxd:4,8,16")) != -1) {
        switch(opt) {
			case 'd':	// set the color depth
                switch(*optarg) {
            	    case '4':
	        	        lcd_color_depth = 4;
						printf("Forcing 4bpp Mode!\n");
        	            break;
    	            case '8':
            	        lcd_color_depth = 8;
						printf("Forcing 8bpp Mode!\n");
                    	break;
	                default:	// test with 16bpp
	                    lcd_color_depth = 16;
						printf("Forcing 16bpp Mode!\n");
						break;
                }
	            break;
	        case 'n':   // no waiting for keypress - fastest operation
				no_wait = 1;
				printf("No Keypress Mode, Must Reset System to Stop!\n");
				break;
	        case 'x':   // init only 
				no_wait = 1;
				printf("Initializing LCD, Skipping testsp!\n");
				init_only = 1;
				break;
			default:	// test with current mode
				break;
		}
	}

	// get the new parameters into the LCD controller
	lcd_init();
	lcd_on();

	if (init_only) return 0;

	printf("Frame Buffer R/W...");
	// do an address=data read/write test on the frame buffer
	// PIXELS_PER_COL * PIXELS_PER_ROW is the highest pixel.  
	// Multiply by bits_per_pixel (sed_color_depth), then
	// divide by 8 to get the actual byte count.
	for (i = 0; i < LCD_FB_SIZE(lcd_color_depth); i += 2){	
		LCD_BUF(i) = i & 0xffff;
		rd16 = LCD_BUF(i);
		if(rd16 != (i & 0xffff)){
			printf("Fail at 0x%08x, WR 0x%08x, RD 0x%04x!\n",LCD_BUF_ADD + i, i, rd16);
			lcd_off();
			return -1;
		}
	} 

	printf("OK!, Press key to continue.\n");

	c = getchar();

	printf("Frame Buffer Start: 0x%08x, End 0x%08x\n",LCD_BUF_ADD, LCD_BUF_ADD + LCD_FB_SIZE(lcd_color_depth));
	if (no_wait)
	{
		printf("Begin Full Screen Color Test.\n");
		while(1){
			// fill the frame buffer with incrementing color values
			for (x = 0; x < 16; x++){
				switch (lcd_color_depth){
					case 4:  wr16 = x | x << 4 | x << 8 | x << 12; break;
					case 8:  wr16 = x | x << 8; break;
					default: wr16 = vga_lookup[x]; break;	// 16-bits bypasses the lookup table
				}
				for (i = 0; i < LCD_FB_SIZE(lcd_color_depth); i += 2){	
					LCD_BUF(i) = wr16;
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
				switch (lcd_color_depth){
					case 4:  wr16 = x | x << 4 | x << 8 | x << 12; break;
					case 8:  wr16 = x | x << 8; break;
					default: wr16 = vga_lookup[x]; break;	// 16-bits bypasses the lookup table
				}
				for (i = 0; i < LCD_FB_SIZE(lcd_color_depth); i += 2){	
					LCD_BUF(i) = wr16;
				}
				c = getchar();
				if (c == 'x') goto lcd_tst_next;
			} // for x
		} // while
	} // else no keycheck test

	lcd_tst_next:

	lcd_off();
	lcd_tst_mode = 0;
	return 0;
}

// lcd_scroll()
//
// Because we are most likely running out of FLASH and probably also with
// cache disabled, a brute force memcpy of the whole screen would be very
// slow, even with reduced color depths.  Instead, we define a frame buffer
// that is twice the size of our actual display.
//
// With the double buffering, we always write to the first buffer, even
// when the second buffer is active.  This allows us to scroll by adjusting
// the starting and ending addresses in the Au1100 by one row.  When we
// reach the end of our virtual buffer, we reset the starting and ending
// addresses to the first buffer.
//
ulong lcd_scroll(){

	lcd_row++;

	// clear the new row(s)
	lcd_clr_row(lcd_row);
	if (lcd_row > (ROWS_PER_SCREEN - 1)){
		lcd_clr_row(lcd_row - ROWS_PER_SCREEN);
	}
	// when lcd_y_pos is greater than ROWS_PER_SCREEN we just adjust the
	// start and end addresses in the Au1100.  If it is equal to 2 *
	// ROWS_PER_SCREEN, we reset the start and end addresses to LCD_MEM_BASE.
	if (lcd_row > (ROWS_PER_SCREEN - 1))
	{
		if (lcd_row > ((ROWS_PER_SCREEN * 2) - 1))
		{
			lcd_fb_offset = 0x00;
			lcd_row = ROWS_PER_SCREEN - 1;
		}
		else
		{
			// calculate the new offset address of the frame buffer in words
			lcd_fb_offset += (LCD_GET_ADD(1, 0, lcd_color_depth));
		}

#ifdef LCD_DBG
	lcd_tst_mode = 1;
	printf("LCD New FB Offset = 0x%08lx.\n ", lcd_fb_offset);
	lcd_tst_mode = 0;
#endif

		// we have to wait for the LCD_INT_S0 bit to go true
		// indicating the Au1100 LCD controller has read the
		// address before we write the new frame buffer address
		while (!(LCD_REG(LCD_ISTAT) & LCD_INT_S0)){}
		LCD_REG(LCD_DMA0) = LCD_DMA0_ADD((LCD_BUF_ADD + lcd_fb_offset));			//new frame buffer start address

	} // if (lcd_row > (ROWS_PER_SCREEN - 1))
	return(0);
		

} // 

//--------------------------------------------------------------------------
// lcd_clr_row()
//
// This routine writes the background color to the font row specified
//
void lcd_clr_row(int char_row){

	ulong lcd_mem_add;
	int i;
	ushort wr16;

	// clear the desired row
	lcd_mem_add = LCD_GET_ADD(char_row, 0, lcd_color_depth);

#ifdef LCD_DBG
	lcd_tst_mode = 1;
	printf("LCD Clear Row %d, FB Add 0x%08lx.\n ", char_row, lcd_mem_add);
	lcd_tst_mode = 0;
#endif

	switch (lcd_color_depth)
	{
		case 4:
			wr16 = ((lcd_bg_color << 12) | (lcd_bg_color << 8) | (lcd_bg_color << 4) | (lcd_bg_color << 0));
			for (i = 0; i < ((PIXELS_PER_ROW * FONT_HEIGHT) / 2); i += 2){
				LCD_BUF(lcd_mem_add) = wr16;
				lcd_mem_add += 2;
			} // for font_row
			break;
		case 8:
			wr16 = ((lcd_bg_color << 8) | (lcd_bg_color << 0));
			for (i = 0; i < (PIXELS_PER_ROW * FONT_HEIGHT); i += 2){
				LCD_BUF(lcd_mem_add) = wr16;
				lcd_mem_add += 2;
			} // for font_row
			break;
		case 16:
			wr16 = (lcd_bg_color << 0);
			for (i = 0; i < ((PIXELS_PER_ROW * FONT_HEIGHT) * 2); i += 2){
				LCD_BUF(lcd_mem_add) = wr16;
				lcd_mem_add += 2;
			} // for font_row
			break;
	} // switch lcd_color_depth
} // lcd_clr_row


//--------------------------------------------------------------------------
// lcd_putchar()
//
// This routine parses the character and calls lcd_writechar if it is a
// printable character
//
void lcd_putchar(char c){

	if (lcd_tst_mode) return;

	// First parse the character to see if it is printable or an acceptable control
    switch (c) {
	    case '\r':
	        lcd_col = 0;
	        return;
	    case '\n':
	        lcd_scroll();
	        return;
	    case '\b':
	        lcd_col--;
	        if (lcd_col < 0)
	        {
	            lcd_row--;
	            if (lcd_row < 0) lcd_row = 0;
	            lcd_col = COLS_PER_SCREEN - 1;
		    }
			c = 0;		// erase the character
			lcd_writechar(c);
	        break;
	    default:
	        if (((uchar)c < FIRST_CHAR) || ((uchar)c > LAST_CHAR)) return; // drop anything we can't print
			c -= FIRST_CHAR;	// get aligned to the first printable character
			lcd_writechar(c);
			// advance to next column
			lcd_col++;
		    if (lcd_col == COLS_PER_SCREEN)
		    {
				lcd_col = 0;
			    lcd_scroll();
			}
			break;
    } 

} // lcd_putchar()

//--------------------------------------------------------------------------
// lcd_writechar()
//
// This routine writes the character to the screen at the current cursor
// location.
//
void lcd_writechar(uchar c) {

	ulong lcd_mem_add;
	int font_row, font_col;
	uchar font_data8;
	ushort wr16;

	// Convert the current row,col and color depth values 
	// into an address
	lcd_mem_add = LCD_GET_ADD(lcd_row, lcd_col, lcd_color_depth);

#ifdef LCD_DBG
	lcd_tst_mode = 1;
	printf("LCD writechar at row %d, col %d, FB Add 0x%08lx.\n ", lcd_row, lcd_col, lcd_mem_add);
	lcd_tst_mode = 0;
#endif

	if (FONT_WIDTH == 8)
	{
		switch (lcd_color_depth){
			case 4:
				// Now render the font by painting one font row at a time
				for (font_row = 0; font_row < FONT_HEIGHT; font_row++){
					// get the font row of data
					font_data8 = font8x16[(c * FONT_HEIGHT) + font_row];
					for (font_col = 0; font_col < 8; font_col += 4)
					{
						// get a words worth of pixels
						wr16 = (((font_data8 & 0x80) ? lcd_fg_color << 0 : lcd_bg_color << 0)
							  | ((font_data8 & 0x40) ? lcd_fg_color << 4 : lcd_bg_color << 4)
							  | ((font_data8 & 0x20) ? lcd_fg_color << 8 : lcd_bg_color << 8)
							  | ((font_data8 & 0x10) ? lcd_fg_color << 12 : lcd_bg_color << 12));
						font_data8 = font_data8 << 4;
//						wr16 = (((font_data8 & 0x80) ? lcd_fg_color << 12 : lcd_bg_color << 12)
//							  | ((font_data8 & 0x40) ? lcd_fg_color << 8 : lcd_bg_color << 8)
//							  | ((font_data8 & 0x20) ? lcd_fg_color << 4 : lcd_bg_color << 4)
//							  | ((font_data8 & 0x10) ? lcd_fg_color << 0 : lcd_bg_color << 0));
//						font_data8 = font_data8 << 4;
						LCD_BUF(lcd_mem_add) = wr16;
						// if we are in the 2nd frame buffer, write to the 1st frame buffer also
						if (lcd_row > (ROWS_PER_SCREEN - 1)){
							LCD_BUF(lcd_mem_add - LCD_FB_SIZE(lcd_color_depth)) = wr16;
						}
						lcd_mem_add += 2;
					} // for font_col
					// go to the next pixel row
					lcd_mem_add += (LCD_ROW_SIZE(lcd_color_depth) - ((FONT_WIDTH * lcd_color_depth) / 8));
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
						wr16 = (((font_data8 & 0x80) ? lcd_fg_color << 0 : lcd_bg_color << 0)
							  | ((font_data8 & 0x40) ? lcd_fg_color << 8 : lcd_bg_color << 8));
						font_data8 = font_data8 << 2;
//						wr16 = (((font_data8 & 0x80) ? lcd_fg_color << 8 : lcd_bg_color << 8)
//							  | ((font_data8 & 0x40) ? lcd_fg_color << 0 : lcd_bg_color << 0));
//						font_data8 = font_data8 << 2;
						LCD_BUF(lcd_mem_add) = wr16;
						// if we are in the 2nd frame buffer, write to the 1st frame buffer also
						if (lcd_row > (ROWS_PER_SCREEN - 1)){
							LCD_BUF(lcd_mem_add - LCD_FB_SIZE(lcd_color_depth)) = wr16;
						}
						lcd_mem_add += 2;
					} // for font_col
					// go to the next pixel row
					lcd_mem_add += (LCD_ROW_SIZE(lcd_color_depth) - ((FONT_WIDTH * lcd_color_depth) / 8));
				} // for font_row
				break;

			default:	// 16bpp is the default
				// Now render the font by painting one font row at a time
				for (font_row = 0; font_row < FONT_HEIGHT; font_row++){
					// get the font row of data
					font_data8 = font8x16[(c * FONT_HEIGHT) + font_row];
					for (font_col = 0; font_col < 8; font_col++)
					{
						// get a words worth of pixels
						wr16 = ((font_data8 & 0x80) ? lcd_fg_color : lcd_bg_color);
						font_data8 = font_data8 << 1;
						LCD_BUF(lcd_mem_add) = wr16;
						// if we are in the 2nd frame buffer, write to the 1st frame buffer also
						if (lcd_row > (ROWS_PER_SCREEN - 1)){
							LCD_BUF(lcd_mem_add - LCD_FB_SIZE(lcd_color_depth)) = wr16;
						}
						lcd_mem_add += 2;
					} // for font_col
					// go to the next pixel row
					lcd_mem_add += (LCD_ROW_SIZE(lcd_color_depth) - ((FONT_WIDTH * lcd_color_depth) / 8));
				} // for font_row
				break;
		} // switch lcd_color depth
	} // FONT_WIDTH == 8
	else
	{
		return;
	}
} // lcd_writechar()	

