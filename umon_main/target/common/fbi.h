/* fbi.h:
 * Frame Buffer Interface...
 */
#include "config.h"

#if	INCLUDE_FBI

#define fbi_putchar(a)	fbi_putchar(a)
extern void fbi_putchar(char c);

#ifdef FBI_NO_CURSOR
#define fbi_cursor()
#else
#define fbi_cursor(a)	fbi_cursor(a)
extern void fbi_cursor(void);
#endif

#else

#define fbi_putchar(a)
#define fbi_cursor()

#endif

extern void fbi_setpixel(int x, int y, long rgbcolor);
extern void fbi_splash(void);
extern void fbdev_init(void);
