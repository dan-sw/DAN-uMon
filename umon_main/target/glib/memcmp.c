#include "config.h"
#include "ctype.h"
#include "genlib.h"
#include "stddefs.h"

/* memcmp():
 * Compare n bytes:  s1>s2: >0  s1==s2: 0  s1<s2: <0
 */
#ifdef WATCHDOG_ENABLED
#ifndef MEMCMP_CHUNK
#define MEMCMP_CHUNK (64*1024)
#endif

int
memcmp(register char *s1,register char *s2,register int n)
{
	int diff, tn;

	if (s1 == s2) return (0);
	do {
		tn = (n < MEMCMP_CHUNK) ? n : MEMCMP_CHUNK;
		n -= tn;
		while (--tn >= 0)
			if ((diff = (*s1++ - *s2++)))
				return (diff);
			WATCHDOG_MACRO;
	} while (n);
	return (0);
}

#else
int
memcmp(register char *s1,register char *s2,register int n)
{
	int diff;

	if (s1 != s2)
		while (--n >= 0)
			if ((diff = (*s1++ - *s2++)))
				return (diff);
	return (0);
}
#endif
