#include "config.h"
#include "ctype.h"
#include "genlib.h"
#include "stddefs.h"

/* memccpy():
 * Copy s2 to s1, stopping if character c is copied. Copy no more than n bytes.
 * Return a pointer to the byte after character c in the copy,
 * or NULL if c is not found in the first n bytes.
 */
char *
memccpy(register char *s1,register char *s2,register int c,register int n)
{
	while (--n >= 0)
		if ((*s1++ = *s2++) == c)
			return (s1);
	return (0);
}
