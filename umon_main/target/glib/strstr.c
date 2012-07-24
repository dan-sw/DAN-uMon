#include "config.h"
#include "ctype.h"
#include "genlib.h"
#include "stddefs.h"

/* strstr():
 * Find the first occurrence of find in s.
 */
char *
strstr(register char *s, register char *find)
{
	register char c, sc;
	register int len;

	if ((c = *find++) != 0) {
		len = strlen(find);
		do {
			do {
				if ((sc = *s++) == 0)
					return ((char *)0);
			} while (sc != c);
		} while (strncmp(s, find, len) != 0);
		s--;
	}
	return ((char *)s);
}
