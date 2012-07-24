#include "config.h"
#include "ctype.h"
#include "genlib.h"
#include "stddefs.h"

/* memset():
 * Set an array of n chars starting at sp to the character c.
 * Return sp.
 */
char *
memset(register char *sp, register char c, register int n)
{
	register char *sp0 = sp;

	while (--n >= 0) {
#ifdef WATCHDOG_ENABLED
		if (((ulong)sp & 0xff) == 0)
			WATCHDOG_MACRO;
#endif
		*sp++ = c;
	}
	return (sp0);
}
