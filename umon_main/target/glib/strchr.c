#include "config.h"
#include "ctype.h"
#include "genlib.h"
#include "stddefs.h"

/* strchr():
 * Return the ptr in sp at which the character c appears;
 * NULL if not found
 */
char *
strchr(register char *sp, register char c)
{
	do {
		if(*sp == c)
			return(sp);
	} while(*sp++);
	return((char *)0);
}
