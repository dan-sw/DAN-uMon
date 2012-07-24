#include "config.h"
#include "ctype.h"
#include "genlib.h"
#include "stddefs.h"

/* strtoupper():
 *	In-place modification of a string to be all upper case.
 */
char *
strtoupper(char *string)
{
	char *cp;

	cp = string;
	while(*cp) {
		*cp = toupper(*cp);
		cp++;
	}
	return(string);
}
