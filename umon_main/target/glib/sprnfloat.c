#include <ctype.h>
#include <float.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

extern int long_to_dec(long lval,char *buf,char *bufend,char *hdr);

typedef union {
	long L;
	float F;
} LF_t;

char *
sprnfloat(char *buf, int bsize, float fpval, int trunc)
{
	LF_t	lf;
	char	*bp;
	short	exp2;
	long	mantissa, int_part, frac_part;

	if (fpval == 0.0) {
		strcpy(buf, "0.0");
		return buf;
	}
	lf.F = fpval;

	exp2 = (0xFF & (lf.L >> 23)) - 127;
	mantissa = (lf.L & 0xFFFFFF) | 0x800000;
	frac_part = 0;
	int_part = 0;

	if (exp2 >= 31) {
		strcpy(buf, "FPTOOBIG");
		return(buf);
	}else if (exp2 < -23) {
		strcpy(buf, "FPTOOSMALL");
		return(buf);
	}else if (exp2 >= 23)
		int_part = mantissa << (exp2 - 23);
	else if (exp2 >= 0) {
		int_part = mantissa >> (23 - exp2);
		frac_part = (mantissa << (exp2 + 1)) & 0xFFFFFF;
	}else /* if (exp2 < 0) */
		frac_part = (mantissa & 0xFFFFFF) >> -(exp2 + 1);

	bp = buf;

	if (lf.L < 0)
		*bp++ = '-';

	if (int_part == 0)
		*bp++ = '0';
	else
		bp += long_to_dec(int_part,bp,buf+bsize-1,"");
	*bp++ = '.';

	if (frac_part == 0)
		*bp++ = '0';
	else {
		char m, max;

		max = bsize - (bp - buf) - 1;
		if (max > trunc)
			max = trunc;
		/* print BCD */
		for (m = 0; m < max; m++) {
			/* frac_part *= 10; */
			frac_part = (frac_part << 3) + (frac_part << 1);

			*bp++ = (frac_part >> 24) + '0';
			frac_part &= 0xFFFFFF;
		}
		/* delete ending zeroes */
		for (--bp; bp[0] == '0' && bp[-1] != '.'; --bp)
			;
		++bp;
	}
	*bp = 0;

	return(buf);
}
