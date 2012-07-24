#include "config.h"
#include "ctype.h"
#include "genlib.h"
#include "stddefs.h"

/* strncpy():
 * Copy s2 to s1, truncating or null-padding to always copy n bytes
 * return s1
 */
char *
strncpy(register char *s1,register char *s2,register int n)
{
	register char *os1 = s1;

	while (--n >= 0)
		if ((*s1++ = *s2++) == '\0')
			while (--n >= 0)
				*s1++ = '\0';
	return (os1);
}

