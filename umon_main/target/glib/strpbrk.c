#include "config.h"
#include "ctype.h"
#include "genlib.h"
#include "stddefs.h"

/* strpbrk():
 * Return ptr to first occurance of any character from `brkset'
 * in the character string `string'; NULL if none exists.
 */

char *
strpbrk(register char *string,register char *brkset)
{
	register char *p;

	do {
		for(p=brkset; *p != '\0' && *p != *string; ++p)
			;
		if(*p != '\0')
			return(string);
	}
	while(*string++);
	return((char *)0);
}
