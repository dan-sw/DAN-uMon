//==========================================================================
//
//      fb_console.c
//
// Author(s):   Michael Kelly - Cogent Computer Systems, Inc.
// Date:        08/02/2003
// Description:	This code will write characters to a frame buffer
//				at programmable pixel depths and screen sizes.
//
//==========================================================================

#include "config.h"
#include "cpuio.h"
#include "genlib.h"
#include "stddefs.h"
#include "font8x16.h"
#include "font5x7.h"

//--------------------------------------------------------------------------
// target requirements
//
// The following macros, defines and functions are target specific
// These are just examples.
//
// Note that this code is optimized to read/write the frame buffer 
// 16-bits at a time (unsigned short).  This provides the widest
// compatibility with both internal LCD controllers and external
// controllers such as the Epson S1D13706.  This does constrain the
// pixel depth to a minimum of 2 for 8x8 fonts and the pixels per
// row must be divisable by 8.  This insures that the minimum write 
// size is 16-bits.  To simplify, we do not support 1 or 2 bits/pixel.
// For mono displays, you will have to enable the palette and use a
// pixel depth of 4 or greater.
//
// #define FB_BASE_ADD				0xf0600000
// #define FB_STEP					1	// FB is 16-bit port on 16-bit boundaries (1) or 32-bit boundaries (2)
// #define RD_FB16(_reg_,_val_) 	((_val_) = *((vushort *)((FB_BASE_ADD + ((ulong)_reg_ * FB_STEP)))))
// #define WR_FB16(_reg_,_val_) 	(*((vushort *)((SED_MEM_BASE + ((ulong)_reg_ * FB_STEP)))) = (_val_))
//
// void fb_scroll()
// This function is very critical to the performance of the FB driver.
// If possible, this target specific routine should manage a pointer
// to the frame buffer that can be adjusted.  This allows the display
// to scroll one row by moving this pointer instead of simply copying
// every byte.  A DMA method could also be used if the target hardware
// allows it.  See sed_scroll() in SED_1356_16bit.c for a good example
// of the frame buffer pointer method.
//

//--------------------------------------------------------------------------
// common macros - FONT_WIDTH and FONT_HEIGHT are set by the chosen font.
//
#define ROWS_PER_SCREEN						(fb_pixels_per_row / FONT_HEIGHT)

#define COLS_PER_SCREEN						(fb_pixels_per_col / FONT_WIDTH)

#define FB_SIZE(_depth_)					(((fb_pixels_per_col * fb_pixels_per_row) * _depth_) / 8)

#define FB_ROW_SIZE(_depth_)				((fb_pixels_per_row * _depth_) / 8)

#define FB_GET_ADD(_row_, _col_, _depth_)	(((((_row_ * fb_pixels_per_row) * FONT_HEIGHT) \
											  + (_col_ * FONT_WIDTH)) \
											  * _depth_) / 8)

//--------------------------------------------------------------------------
// function prototypes
//
void fb_init(void);
void fb_putchar(char c);
void fb_writechar(uchar c);
void fb_clr_row(int char_row);

// globals to keep track of foreground, background colors and x,y position
int fb_color_depth = 4;			// 4, 8 or 16
int fb_pixels_per_col = 320;	// typcial values are 320 and 640
int fb_pixels_per_row = 240;	// typcial values are 240 and 480
int fb_fg_color = 15;			// 0 to 15, used as lookup into VGA color table or LCD palette
int fb_bg_color = 0;			// 0 to 15, used as lookup into VGA color table or LCD palette
int fb_col = 0;					// current column, 0 to COLS_PER_SCREEN - 1
int fb_row = 0;					// current row, 0 to ROWS_PER_SCREEN - 1
ulong fb_offset = 0;   			// current offset into frame buffer for fb_putchar

//--------------------------------------------------------------------------
// fb_clr_row()
//
// This routine writes the background color to the font row specified.  This
// routine assumes that the pixel rows are contiguous.
//
void fb_clr_row(int char_row)
{

	ulong fb_mem_add;
	int i, x;
	ushort wr16;

	// get the starting offset into the frame buffer for this row
	fb_mem_add = FB_GET_ADD(char_row, 0, fb_color_depth);

	// get the background color
	wr16 = ((fb_bg_color << 12) | (fb_bg_color << 8) | (fb_bg_color << 4) | (fb_bg_color << 0));

	// fill the font row with the background color, 16-bits at a time
	for (i = 0; i < FB_ROW_SIZE(fb_color_depth); i += 2){
		WR_FB16(fb_mem_add, wr16);
		fb_mem_add += 2;
	} // for font_row
} // fb_clr_row


//--------------------------------------------------------------------------
// fb_putchar()
//
// This routine parses the character and calls fb_writechar if it is a
// printable character
//
void fb_putchar(char c)
{

	if (fb_tst) return;	// can't print if we are testing the frame buffer

	// First parse the character to see if it printable or an acceptable control
    switch (c) {
	    case '\r':
	        fb_col = 0;
	        return;
	    case '\n':
	        fb_scroll();
	        return;
	    case '\b':
	        if (fb_col > 0)
	        {
		        fb_col--;
			}
			else
			{
	            if (fb_row > 0)
	            {
	            	fb_row--;
		            fb_col = COLS_PER_SCREEN - 1;
				}
		    }
			c = CURSOR_ON;		// put the cursor where the character was
			fb_writechar(c);
	        break;
	    default:
	        if (((uchar)c < FIRST_CHAR) || ((uchar)c > LAST_CHAR)) return; // drop anything we can't print
			c -= FIRST_CHAR;	// get aligned to the first printable character
			fb_writechar(c);
			// advance to next column
			fb_col++;
		    if (fb_col == COLS_PER_SCREEN)
		    {
				fb_col = 0;
			    fb_scroll();
			}
			c = CURSOR_ON;	// write the cursor character
			fb_writechar(c);
			break;
    } 

} // fb_putchar()

//--------------------------------------------------------------------------
// fb_writechar()
//
// This routine writes the character to the screen at the current cursor
// location.
//
void fb_writechar(uchar c)
{

	ulong fb_mem_add;
	int font_row, font_col;
	uchar font_data8;
	ushort wr16;

	// Convert the current row, col and color depth values 
	// into an address
	fb_mem_add = FB_GET_ADD(fb_row, fb_col, fb_color_depth);

	switch (fb_color_depth)
	{
		case 4:
			// Now render the font by painting one font row at a time
			for (font_row = 0; font_row < FONT_HEIGHT; font_row++){
				// get the font row of data
				font_data8 = font8x16[(c * FONT_HEIGHT) + font_row];
	
	
				for (font_col = 0; font_col < FONT_WIDTH; font_col += 4)
				{
					// get a words worth of pixels
					wr16 = (((font_data8 & 0x80) ? fb_fg_color << 12 : fb_bg_color << 12)
						  | ((font_data8 & 0x40) ? fb_fg_color << 8 : fb_bg_color << 8)
						  | ((font_data8 & 0x20) ? fb_fg_color << 4 : fb_bg_color << 4)
						  | ((font_data8 & 0x10) ? fb_fg_color << 0 : fb_bg_color << 0));
					font_data8 = font_data8 << 4;
					WR_FB16(fb_mem_add, wr16);
					fb_mem_add += 2;
				} // for font_col
				// go to the next pixel row
				fb_mem_add += (FB_ROW_SIZE(fb_color_depth) - ((FONT_WIDTH * 4) / 8));
			} // for font_row
			break;

		case 8:
			// Now render the font by painting one font row at a time
			for (font_row = 0; font_row < FONT_HEIGHT; font_row++){
				// get the font row of data
				font_data8 = font8x16[(c * FONT_HEIGHT) + font_row];
				for (font_col = 0; font_col < FONT_WIDTH; font_col += 2)
				{
					// get a words worth of pixels
					wr16 = (((font_data8 & 0x80) ? fb_fg_color << 8 : fb_bg_color << 8)
						  | ((font_data8 & 0x40) ? fb_fg_color << 0 : fb_bg_color << 0));
					font_data8 = font_data8 << 2;
					WR_FB16(fb_mem_add, wr16);
					fb_mem_add += 2;
				} // for font_col
				// go to the next pixel row
				fb_mem_add += (FB_ROW_SIZE(fb_color_depth) - ((FONT_WIDTH * 8) / 8));
			} // for font_row
			break;

		case 16:
			// Now render the font by painting one font row at a time
			for (font_row = 0; font_row < FONT_HEIGHT; font_row++){
				// get the font row of data
				font_data8 = font8x16[(c * FONT_HEIGHT) + font_row];
				for (font_col = 0; font_col < FONT_WIDTH; font_col++)
				{
					// get a words worth of pixels
					wr16 = ((font_data8 & 0x80) ? fb_fg_color : fb_bg_color);
					font_data8 = font_data8 << 1;
					WR_FB16(fb_mem_add, wr16);
					fb_mem_add += 2;
				} // for font_col
				// go to the next pixel row
				fb_mem_add += (FB_ROW_SIZE(fb_color_depth) - ((FONT_WIDTH * 16) / 8));
			} // for font_row
			break;
	} // switch fb_color depth
} // fb_writechar()	

//--------------------------------------------------------------------------
// fb()
//
char *fbHelp[] = {
	" This command allows the user to set",
	" options for the Frame Buffer based display",
	" found on most Cogent Single Boards.",
	" The user may set the row x col pixels, ",
	" set the color depth to run at, and set",
	" the foreground and background colors.",
	" Note that after all but the foreground and",
	" background options, the target LCD controller",
	" will need to be re-initialized.  This",
	" command can be put into a startup script\n",
    " Usage:",
    " fb -[v,c,l,f,b,d:[4,8,16]]",
    " Options...",
	" -v set verbose mode where we print each option being set",
    " -cx sets the pixels per column to x, i.e. 320",
    " -rx sets the pixels per row to x, i.e. 320",
    " -fx set foreground color to x (VGA 16 Color Pallette 0-15)",
    " -bx set background color to x (VGA 16 Color Pallette 0-15)",
	" -d4 set a depth of 4-bits/pixel",
	" -d8 set a depth of 8-bits/pixel",
	" -d16 set a depth of 16-bits/pixel",
	" calling fb with no options will return current status",
	0
};

int fb(int argc,char *argv[])
{
	int	opt, verbose;

	reinit = 0;
    if (argc == 1) {
		printf("0x%08lx - Frame Buffer Base Address\n", (ulong)FB_GET_ADD(0, 0, fb_color_depth));
		printf("%d bits-per-pixel\n", fb_color_depth);
		printf("%d x %d Display Size (Pixels)\n", fb_pixels_per_col, fb_pixels_per_row);
		printf("%d x %d Font Size\n", FONT_WIDTH, FONT_HEIGHT);
		printf("%d - Foreground Color\n", fb_fg_color);
		printf("%d - Background Color\n", fb_bg_color);
		return(0);
    }

    while ((opt=getopt(argc,argv,"v,c,l,f,b,d:")) != -1) {
		if (opt == 'v') verbose = 1;
        switch(opt) {
	        case 'c':
				fb_pixels_per_col = (*optarg);
				if (verbose) printf("Setting Pixels Per Column to %d\n", fb_pixels_per_col);
				break;
	        case 'r':
				fb_pixels_per_row = (*optarg);
				if (verbose) printf("Setting Pixels Per Row to %d\n", fb_pixels_per_row);
				break;
			case 'd':	// set the color depth
                switch(*optarg) {
            	    case '4':
	        	        fb_color_depth = 4;
						if (verbose) printf("Setting 4bpp Mode!\n");
        	            break;
    	            case '8':
            	        fb_color_depth = 8;
						if (verbose) printf("Setting 8bpp Mode!\n");
                    	break;
    	            case '1':
	                    fb_color_depth = 16;
						if (verbose) printf("Setting 16bpp Mode!\n");
        	            break;
	                default:	// test with current depth
						if (verbose) printf("Unsupported Color Depth, Use 4, 8 or 16!\n");
						break;
                }
	            break;
	        case 'f':	// set foreground color
				fb_fg_color = atoi(optarg);
				if (verbose) printf("New Foreground Color = %d.\n", fb_fg_color);
				break;
	        case 'b':	// set foreground color
				fb_bg_color = atoi(optarg);
				if (verbose) printf("New Background Color = %d.\n", fb_bg_color);
				break;
			default:
				break;
		}
	}
	return 0;
} // sed()

