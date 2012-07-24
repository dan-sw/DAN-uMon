#include "config.h"
#include "ctype.h"
#include "genlib.h"
#include "stddefs.h"

/* strspn():
 * Return the number of characters in the maximum leading segment
 * of string which consists solely of characters from charset.
 */
int
strspn(char *string,register char *charset)
{
	register char *p, *q;

	for(q=string; *q != '\0'; ++q) {
		for(p=charset; *p != '\0' && *p != *q; ++p)
			;
		if(*p == '\0')
			break;
	}
	return(q-string);
}
