#include "config.h"
#include "ctype.h"
#include "genlib.h"
#include "stddefs.h"

int
atoi(char *p)
{
	int n, c, neg;

	neg = 0;
	if (!isdigit((c = *p))) {
		while (isspace(c))
			c = *++p;
		switch (c) {
		case '-':
			neg++;
			c = *++p;
			break;
		case '+':
			c = *++p;
			break;
		}
		if (!isdigit(c))
			return (0);
	}
	for (n = '0' - c; isdigit((c = *++p)); ) {
		n *= 10;		/* two steps to avoid unnecessary overflow */
		n += '0' - c;	/* accum neg to avoid surprises at MAX */
	}
	return (neg ? n : -n);
}
