/* mprintf.c:
 *	A small, but useful printf.
 *
 *	General notice:
 *	This code is part of a boot-monitor package developed as a generic base
 *	platform for embedded system designs.  As such, it is likely to be
 *	distributed to various projects beyond the control of the original
 *	author.  Please notify the author of any enhancements made or bugs found
 *	so that all may benefit from the changes.  In addition, notification back
 *	to the author will allow the new user to pick up changes that may have
 *	been made by other users after this version of the code was distributed.
 *
 *	Note1: the majority of this code was edited with 4-space tabs.
 *	Note2: as more and more contributions are accepted, the term "author"
 *		   is becoming a mis-representation of credit.
 *
 *	Original author:	Ed Sutter
 *	Email:				esutter@lucent.com
 *	Phone:				908-582-2351
 */
#include "config.h"
#include "cpuio.h"
#include "genlib.h"
#include "stddefs.h"
#include "cli.h"
#include "ctype.h"

/* printf.c...
 *	This is my homegrown printf.  It is VERY limited, but useful
 *	for small applications that should avoid wasting memory.
 *	The %s, %c, %d, %x formats are supported to some degree.
 *	I also added %I and %M...
 *	%I assumes that the argument is a long and it converts it to
 *		a string in IP format (a.b.c.d).
 *	%M assumes that the argument is a pointer to an array of 6 bytes
 *		and it converts it to a string in MAC format (x:y:z:a:b:c).
 *
 *	So why don't I use varargs?...
 *	Yep, it sure would look nicer, and would eliminate the clumsy limit
 *	of 12 arguments, but so far I haven't had a problem with any of the
 *	CPUs I've ported to and it eliminates the headache of dealing
 *	with varargs vs stdargs depending on the compiler.
 *  
 */

#define MAXARGS 	12
#define HDRSIZE		4
#define NEGATIVE	1
#define POSITIVE	2
#define SCREENWIDTH	80

int
long_to_dec(long lval,char *buf,char *hdr)
{
	short	size, i, minsize;
	long	value;
	char	*base, fill;

	base = buf;
	minsize = 0;
	/* hdr would be '8' in the format %8d, for example */
	if (hdr[0]) {
		if (hdr[0] == '0')
			fill = '0';
		else
			fill = ' ';
		minsize = (short)atoi(hdr);
		for(i=0;i<minsize;i++)
			*buf++ = fill;
	}
	buf = base;

	/* First determine how many ascii digits are needed. */
	value = abs(lval);
	size = 0;
	while (value > 0) {
		size++;
		value /= 10;
	}
	if (lval < 0)
		size++;
	if (minsize > size)
		size = minsize;
	buf += size;

	/* Now build the string. */
	value = abs(lval);
	if (value == 0) {
		if (minsize==0) {
			*base = '0';
			size = 1;
		}
		else 
			*--buf = '0';
	}
	else {
		while (value > 0) {
			*--buf = (char)(value % 10) + '0';
			value /= 10;
		}
	}
	if (lval < 0)
		*--buf = '-';
	base[size] = 0;
	return((int)size);
}

int
long_to_ip(long lval,char *buf,char *hdr)
{
	int	i, j, len;
	unsigned char *lp;

	len = 0;
	lp = (char *)&lval;
	for(j=0;j<4;j++) {
		i = long_to_dec(*lp++,buf,hdr);
		buf += i;
		*buf++ = '.';
		len += (i + 1);
	}
	buf--;
	len--;
	return(len);
}


int
long_to_hex(ulong lval,char *buf,char *hdr)
{
	static char *hexdigits = "0123456789abcdef";
	ulong	value;
	short	size, i, minsize;
	char	*base;

	base = buf;
	/* hdr would be '02' in the format %02x, for example */
	minsize = 0;
	if (hdr[0]) {
		if (hdr[1]) {
			minsize = (short)(hdr[1]&0xf);
			for(i=0;i<minsize;i++)
				*buf++ = hdr[0];
		}
		else {
			minsize = (short)(hdr[0]&0xf);
			for(i=0;i<minsize;i++)
				*buf++ = ' ';
		}
	}
	buf = base;

	/* First determine how many ascii digits are needed. */
	value = lval;
	size = 0;
	while (value > 0) {
		size++;
		value /= 16;
	}
	if (minsize > size)
		size = minsize;
	buf += size;

	/* Now build the string. */
	if (lval == 0) {
		if (size == 0)
			size = 1;
		else
			buf--;
		*buf = '0';
	}
	else {
		while (lval > 0) {
			*--buf = hexdigits[(int)(lval % 16)];
			lval /= 16;
		}
	}
	buf[size] = 0;
	return((int)size);
}

int
bin_to_mac(uchar *ibin,char *buf)
{
	int	i, j, len;

	len = 0;
	for(j=0;j<6;j++) {
		i = long_to_hex(*ibin++,buf,"02");
		buf += i;
		*buf++ = ':';
		len += (i + 1);
	}
	buf--;
	len--;
	return(len);
}

/* build_string():
 * 	Build a string from 'src' to 'dest' based on the hdr and sign
 *	values.  Return the size of the string (may include left or right
 *	justified padding).
 */
int
build_string(char *src,char *dest,char *hdr,int sign)
{
	char	*cp1;
	short	minsize, i, j;

	if (!src) {
		strcpy(dest,"NULL_POINTER");
		return(12);
	}
	if (!*src)
		return(0);
	if (!hdr[0]) {
		j = 0;
		while(*src) {
			*dest++ = *src++;
			j++;
		}
		return(j);
	}
	minsize = atoi(hdr);
	i = 0;
	cp1 = (char *)src;
	while(*cp1) {
		i++;
		cp1++;
	}
	cp1 = (char *)src;
	j = 0;
	if (minsize > i) {
		if (sign == POSITIVE) {
			while(minsize > i) {
				j++;
				*dest++ = ' ';
				minsize--;
			}
			while(*cp1) {
				j++;
				*dest++ = *cp1++;
			}
		}
		else {
			while(*cp1) {
				j++;
				*dest++ = *cp1++;
			}
			while(minsize > i) {
				j++;
				*dest++ = ' ';
				minsize--;
			}
		}
	}
	else {
		while(*cp1) {
			j++;
			*dest++ = *cp1++;
		}
	}
	return(j);
}

/* snprintf():
 *	Backend to all the others below it.
 *	Formats incoming argument list based on format string.
 *	Terminates population of buffer if it is to exceed the
 *	specified buffer size.
 *	NOTE: the buffer size check is not complete... It's barely started!
 */
int
snprintf(char *buf,int bsize,char *fmt,
	long arg1,long arg2,long arg3,long arg4, long arg5, long arg6,
	long arg7,long arg8,long arg9,long arg10,long arg11,long arg12)
{
	int		i, sign;
	char	*cp, hdr[HDRSIZE], *base, *bufend;
	long	args[MAXARGS], nextarg;

	bsize--;		/* Decrement by one to save space for NULL */
	args[0] = arg1; 
	args[1] = arg2; 
	args[2] = arg3;
	args[3] = arg4; 
	args[4] = arg5; 
	args[5] = arg6;
	args[6] = arg7; 
	args[7] = arg8;
	args[8] = arg9;
	args[9] = arg10; 
	args[10] = arg11; 
	args[11] = arg12;

	base = buf;
	bufend = base+bsize;
	cp = fmt;
	nextarg = 0;
	for(i=0;i<HDRSIZE;i++)
		hdr[i] = 0;
	while(*cp) {
		if (buf >= bufend) {
			*bufend = 0;
			puts("sprintf buffer overflow\n");
			return(bsize);
		}
		if (nextarg >= MAXARGS) {
			puts("sprintf arg overflow\n");
			return(0);
		}
		if (*cp != '%') {
			*buf++ = *cp++;
			continue;
		}
		cp++;
		if (*cp == '%') {
			*buf++ = *cp++;
			continue;
		}
		sign = POSITIVE;
		if (*cp == '-') {
			sign = NEGATIVE;
			cp++;
		}
		if (isdigit(*cp)) {
			for(i=0;i<(HDRSIZE-1);i++) {
				if (isdigit(*cp))
					hdr[i] = *cp++;
				else
					break;
			}
		}

		if (*cp == 'l')			/* Ignore the 'long' designator */
			cp++;

		switch(*cp) {
			case 'c':			/* Character conversion */
				*buf++ = (char)args[nextarg++];
				break;
			case 's':			/* String conversion */
				i = build_string((char *)args[nextarg++],buf,hdr,sign);
				buf += i;
				break;
			case 'M':			/* MAC address conversion */
				i = bin_to_mac((char *)args[nextarg++],buf);
				buf += i;
				break;
			case 'I':			/* IP address conversion */
				i = long_to_ip(args[nextarg++],buf,hdr);
				buf += i;
				break;
			case 'd':			/* Decimal conversion */
			case 'u':
				i = long_to_dec(args[nextarg++],buf,hdr);
				buf += i;
				break;
			case 'p':			/* Hex conversion */
			case 'x':
			case 'X':
				if (*cp == 'p') {
					*buf++ = '0';
					*buf++ = 'x';
				}
				i = long_to_hex(args[nextarg++],buf,hdr);
				buf += i;
				break;
			default:
				*buf++ = *cp;
				break;
		}
		cp++;

		if (hdr[0]) {
			for(i=0;i<HDRSIZE;i++)
				hdr[i] = 0;
		}
	}
	*buf = 0;
	return(buf-base);
}

/* sprintf():
 * 	puts formatted string into buffer.
 *	Yep, if you use more than 12 args this breaks...
 *	So use something else!
 */
int
sprintf(char *buf, char *fmt,
	long arg1,long arg2,long arg3, long arg4,  long arg5,  long arg6,
	long arg7,long arg8,long arg9, long arg10, long arg11, long arg12)
{
	int	tot;

	tot = snprintf(buf,999999,fmt,
		arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12);
	return(tot);
}

/* printf():
 *	Yep, it breaks if there are more than CMDLINESIZE characters in the string
 *	built by sprintf.
 */

int
printf(char *fmt, long arg1, long arg2, long arg3, long arg4,
	long arg5, long arg6, long arg7,long arg8,long arg9,
	long arg10, long arg11, long arg12)
{
	int	i, tot;
	char	pbuf[CMDLINESIZE];

	tot = snprintf(pbuf,CMDLINESIZE,fmt,
		arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12);
	for(i=0;i<tot;i++)
		putchar(pbuf[i]);
	return(tot);
}

/* cprintf():
 *	Same as printf, but center text based on SCREENWIDTH by
 *	inserting whitespace.
 */
int
cprintf(char *fmt, long arg1, long arg2, long arg3, long arg4,
	long arg5, long arg6, long arg7,long arg8,long arg9,
	long arg10, long arg11, long arg12)
{
	int	i, tot, spaces;
	char	pbuf[CMDLINESIZE];

	tot = snprintf(pbuf,CMDLINESIZE,fmt,
		arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12);
	if (tot < SCREENWIDTH) {
		spaces = (SCREENWIDTH-tot)/2;
		for(i=0;i<spaces;i++)
			putchar(' ');
	}
	else
		spaces = 0;
	for(i=0;i<tot;i++)
		putchar(pbuf[i]);
	return(tot+spaces);
}

