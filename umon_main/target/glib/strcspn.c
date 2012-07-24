#include "config.h"
#include "ctype.h"
#include "genlib.h"
#include "stddefs.h"

/* strcspn():
 * Return the number of characters in the maximum leading segment
 * of string which consists solely of characters not from charset.
 */
int
strcspn(char *string,register char *charset)
{
	register char *p, *q;

	for(q=string; *q != '\0'; ++q) {
		for(p=charset; *p != '\0' && *p != *q; ++p)
			;
		if(*p != '\0')
			break;
	}
	return(q-string);
}
