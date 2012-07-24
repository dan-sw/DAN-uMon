#include "config.h"
#include "ctype.h"
#include "genlib.h"
#include "stddefs.h"

/* strrchr():
 * Return the ptr in sp at which the character c last
 * appears; NULL if not found
 */
char *
strrchr(register char *sp, register char c)
{
	register char *r;

	r = (char *)0;
	do {
		if(*sp == c)
			r = sp;
	} while(*sp++);
	return(r);
}
