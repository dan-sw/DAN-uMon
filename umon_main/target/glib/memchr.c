#include "config.h"
#include "ctype.h"
#include "genlib.h"
#include "stddefs.h"

/* memchr():
 * Return the ptr in sp at which the character c appears;
 *   NULL if not found in n chars; don't stop at \0.
 */
char *
memchr(register char *sp, register char c,register int n)
{
	while (--n >= 0)
		if (*sp++ == c)
			return (--sp);
	return (0);
}
