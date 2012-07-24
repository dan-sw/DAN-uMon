#include "config.h"
#include "ctype.h"
#include "genlib.h"
#include "stddefs.h"

/* strtok():
 * uses strpbrk and strspn to break string into tokens on
 * sequentially subsequent calls.  returns NULL when no
 * non-separator characters remain.
 * `subsequent' calls are calls with first argument NULL.
 */

char *
strtok(char *string,char *sepset)
{
	register char	*p, *q, *r;
	static char	*savept;

	/*first or subsequent call*/
	p = (string == (char *)0)? savept: string;

	if(p == 0)		/* return if no tokens remaining */
		return((char *)0);

	q = p + strspn(p, sepset);	/* skip leading separators */

	if(*q == '\0')		/* return if no tokens remaining */
		return((char *)0);

	if((r = strpbrk(q, sepset)) == (char *)0)	/* move past token */
		savept = 0;	/* indicate this is last token */
	else {
		*r = '\0';
		savept = ++r;
	}
	return(q);
}

