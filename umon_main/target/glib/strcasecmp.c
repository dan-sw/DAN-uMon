#include "config.h"
#include "ctype.h"
#include "genlib.h"
#include "stddefs.h"

/* strcasecmp():
 * Compare strings (ignore case):  s1>s2: >0  s1==s2: 0  s1<s2: <0
 */
int
strcasecmp(register char *s1,register char * s2)
{

	if(s1 == s2)
		return(0);

	while(tolower(*s1) == tolower(*s2)) {
		s2++;
		if(*s1++ == '\0')
			return(0);
	}
	return(tolower(*s1) - tolower(*(s2-1)));
}
