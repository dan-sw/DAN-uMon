struct font {
	char  *bitmap;		/* Pointer to font bitmap array. */
	int width;			/* Width of font in array. */
	int height;			/* Height of font in array. */
	int above;			/* Number of pixels of empty space above. */
	int below;			/* Number of pixels of empty space blow. */
	int between;		/* Number of pixels of separation between each char. */
};

extern char font8x12[];
extern struct font font_styles[];
extern int font_style_total(void);

#define MAX_FONT_WIDTH		8
#define MIN_FONT_WIDTH		8

#define FONT_WHITE			0x00ffffff
#define FONT_BLACK			0x00000000
#define FONT_TRANSPARENT	0x01000000
#define FONT_INVERT			0x02000000

#define OPAQUE_BACKGROUND(x)	((x & FONT_TRANSPARENT) == 0)
