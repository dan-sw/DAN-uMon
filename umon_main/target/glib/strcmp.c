#include "config.h"
#include "ctype.h"
#include "genlib.h"
#include "stddefs.h"

/* strcmp():
 * Compare strings:  s1>s2: >0  s1==s2: 0  s1<s2: <0
 */
int
strcmp(register char *s1,register char * s2)
{

	if(s1 == s2)
		return(0);
	while(*s1 == *s2++)
		if(*s1++ == '\0')
			return(0);
	return(*s1 - *--s2);
}
