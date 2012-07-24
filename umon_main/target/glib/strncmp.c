#include "config.h"
#include "ctype.h"
#include "genlib.h"
#include "stddefs.h"

/* strncmp():
 * Compare strings (at most n bytes)
 *	returns: s1>s2; >0  s1==s2; 0  s1<s2; <0
 */
int
strncmp(register char *s1,register char *s2,register int n)
{
	if(s1 == s2)
		return(0);
	while(--n >= 0 && *s1 == *s2++)
		if(*s1++ == '\0')
			return(0);
	return((n < 0)? 0: (*s1 - *--s2));
}
