/* little_print.c:
 * These functions provide a primitive mechanism to support basic
 * printing of strings and hex values.  Quite useful when building
 * applications that can't afford even a "simple" printf...
 */

void
prstring(char *str)
{
	if (str) {
		while(*str) {
			if (*str == '\n')
				target_putchar('\r');
			target_putchar(*str++);
		}
	}
}

void
prlong(long lval)
{
	static char *hexdigits = "0123456789ABCDEF";
	int idx, shift;
	unsigned long mask = 0xf0000000;

	prstring("0x");
	shift = 28;
	while(1) {
		idx = (lval & mask) >> shift;
		target_putchar(hexdigits[idx]);
		if (mask == 0xf)
			break;
		mask >>= 4;
		shift -= 4;
	}
	return;
}
