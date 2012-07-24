/* vmprintf.c:
 *
 *	Same as mprintf, but this uses stdarg.h.
 *	Cleaner and simpler.  I have some targets using this and will probably
 *	convert all targets to use this eventually.
 *
 *	Note that this requires that the CFLAGS used in the building of this
 *	file should omit the "-nostdinc" option, so that the standard header
 *	(stdarg.h) can be pulled in.
 *
 *	Public functions:
 *	vsnprintf(), snprintf(), sprintf(), printf()
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
#include <stdarg.h>
#include "stddefs.h"
#include "cli.h"

extern long abs(long);
extern void puts(char *);
extern int atoi(char *), putchar(char);

#define MAX_NUMBER_SIZE 	18
#define HDRSIZE				4
#define NEGATIVE			1
#define POSITIVE			2
#define SCREENWIDTH			80

/* PROCCHARP(), PROCCHAR(), BUFDEC() & BUFINC():
 * Macros used to conveniently distinguish between buffer-bound
 * and console-bound characters during the formatting process.
 */
#define PROCCHARP(bp,eob,cp) 					\
	if (bp) {									\
		if (eob && (bp >= eob)) {				\
			puts("s_printf buffer overflow\n");	\
			return(-1);							\
		}										\
		*bp++ = *cp++;							\
	}											\
	else {										\
		putchar(*cp++);							\
	}											

#define PROCCHAR(bp,eob,c) 						\
	if (bp) {									\
		if (eob && (bp >= eob)) {				\
			puts("s_printf buffer overflow\n");	\
			return(-1);							\
		}										\
		*bp++ = c;								\
	}											\
	else {										\
		if (c)									\
			putchar(c);						\
	}

#define BUFDEC(bp,val)		\
	if (bp) {				\
		bp -= val;			\
	}		

#define BUFINC(bp,val)		\
	if (bp) {				\
		bp += val;			\
	}		


/* isdigit():
 * Copied here to avoid inclusion of ctype.h
 */
static int
isdigit(char ch)
{
  return ((ch >= '0') && (ch <= '9'));
}

/* long_to_dec():
 * Convert an incoming long value to ASCII decimal notation.
 */
static int
long_to_dec(long lval,char *buf,char *bufend,char *hdr)
{
	short	size, i, minsize;
	long	value;
	char	fill, lbuf[MAX_NUMBER_SIZE], *lbp;

	lbp = lbuf;
	minsize = 0;
	/* hdr would be '8' in the format %8d, for example */
	if (hdr[0]) {
		if (hdr[0] == '0')
			fill = '0';
		else
			fill = ' ';
		minsize = (short)atoi(hdr);
		for(i=0;i<minsize;i++)
			*lbp++ = fill;
	}
	lbp = lbuf;

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
	lbp += size;

	/* Now build the string. */
	value = abs(lval);
	if (value == 0) {
		if (minsize==0) {
			lbuf[0] = '0';
			size = 1;
		}
		else 
			*--lbp = '0';
	}
	else {
		while (value > 0) {
			*--lbp = (char)((value % 10) + '0');
			value /= 10;
		}
	}
	if (lval < 0)
		*--lbp = '-';
	lbuf[size] = 0;

	/* At this point, lbuf[] contains the ascii decimal string
	 * so we can now pass it through PROCCHAR...
	 */
	for(i=0;i<size;i++) {
		PROCCHAR(buf,bufend,lbuf[i]);
	}
	return((int)size);
}

/* long_to_hex():
 * Convert an incoming long value to ASCII hexadecimal notation.
 */
static int
long_to_hex(ulong lval,char *buf,char *bufend,char *hdr)
{
	static char *hexdigits = "0123456789abcdef";
	ulong	value;
	short	size, i, minsize;
	char	lbuf[MAX_NUMBER_SIZE], *lbp;

	lbp = lbuf;
	/* hdr would be '02' in the format %02x, for example */
	minsize = 0;
	if (hdr[0]) {
		if (hdr[1]) {
			minsize = (short)(hdr[1]&0xf);
			for(i=0;i<minsize;i++)
				*lbp++ = hdr[0];
		}
		else {
			minsize = (short)(hdr[0]&0xf);
			for(i=0;i<minsize;i++)
				*lbp++ = ' ';
		}
	}
	lbp = lbuf;

	/* First determine how many ascii digits are needed. */
	value = lval;
	size = 0;
	while (value > 0) {
		size++;
		value /= 16;
	}
	if (minsize > size)
		size = minsize;
	lbp += size;

	/* Now build the string. */
	if (lval == 0) {
		if (size == 0)
			size = 1;
		else
			lbp--;
		*lbp = '0';
	}
	else {
		while (lval > 0) {
			*--lbp = hexdigits[(int)(lval % 16)];
			lval /= 16;
		}
	}
	lbp[size] = 0;

	/* At this point, lbuf[] contains the ascii hex string
	 * so we can now pass it through PROCCHAR...
	 */
	for(i=0;i<size;i++) {
		PROCCHAR(buf,bufend,lbuf[i]);
	}

	return((int)size);
}

/* build_string():
 * 	Build a string from 'src' to 'dest' based on the hdr and sign
 *	values.  Return the size of the string (may include left or right
 *	justified padding).
 */
static int
build_string(char *src,char *dest,char *bufend,char *hdr,int sign)
{
	char	*cp1;
	short	minsize, i, j;

	if (!src) {
		cp1 = "NULL_POINTER";
		while(*cp1) {
			PROCCHARP(dest,bufend,cp1);
		}
		return(12);
	}
	if (!*src)
		return(0);
	if (!hdr[0]) {
		j = 0;
		while(*src) {
			PROCCHARP(dest,bufend,src);
			j++;
		}
		return(j);
	}
	minsize = (short)atoi(hdr);
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
				PROCCHAR(dest,bufend,' ');
				minsize--;
			}
			while(*cp1) {
				j++;
				PROCCHARP(dest,bufend,cp1);
			}
		}
		else {
			while(*cp1) {
				j++;
				PROCCHARP(dest,bufend,cp1);
			}
			while(minsize > i) {
				j++;
				PROCCHAR(dest,bufend,' ');
				minsize--;
			}
		}
	}
	else {
		while(*cp1) {
			j++;
			PROCCHARP(dest,bufend,cp1);
		}
	}
	return(j);
}

/* vsnprintf():
 *	Backend to all the others below it.
 *	Formats incoming argument list based on format string.
 *	Terminates population of buffer if it is to exceed the
 *	specified buffer size.
 */
int
vsnprintf(char *buf,int bsize, char *fmt, va_list argp)
{
	long	arg_l;
	int		i, sign, tot;
	char	*cp, hdr[HDRSIZE], *base, *bufend, arg_c, *arg_cp;

	tot = 0;
	base = buf;

	if (bsize == 0)
		bufend = 0;
	else
		bufend = base+(bsize-1);

	cp = fmt;
	for(i=0;i<HDRSIZE;i++)
		hdr[i] = 0;
	while(*cp) {
		if (*cp != '%') {
			PROCCHARP(buf,bufend,cp);
			tot++;
			continue;
		}
		cp++;
		if (*cp == '%') {
			PROCCHARP(buf,bufend,cp);
			tot++;
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
				arg_c = (char)va_arg(argp,int);
				PROCCHAR(buf,bufend,arg_c);
				tot++;
				break;
			case 's':			/* String conversion */
				arg_cp = (char *)va_arg(argp,int);
				i = build_string(arg_cp,buf,bufend,hdr,sign);
				BUFINC(buf,i);
				tot += i;
				break;
			case 'd':			/* Decimal conversion */
			case 'u':
				arg_l = (long)va_arg(argp,int);
				i = long_to_dec(arg_l,buf,bufend,hdr);
				BUFINC(buf,i);
				tot += i;
				break;
			case 'p':			/* Hex conversion */
			case 'x':
			case 'X':
				if (*cp == 'p') {
					PROCCHAR(buf,bufend,'0');
					PROCCHAR(buf,bufend,'x');
				}
				arg_l = (long)va_arg(argp,int);
				i = long_to_hex((ulong)arg_l,buf,bufend,hdr);
				BUFINC(buf,i);
				tot += i;
				break;
			default:
				PROCCHARP(buf,bufend,cp);
				tot++;
				break;
		}
		cp++;

		if (hdr[0]) {
			for(i=0;i<HDRSIZE;i++)
				hdr[i] = 0;
		}
	}
	PROCCHAR(buf,bufend,0);
	return(tot);
}

/* snprintf(), sprintf(), printf():
 * All functions use vsnprintf() to format a string.
 * The string is either transferred to a buffer or transferred
 * directly to this target's console through putchar (refer to
 * the macros at the top of this file).
 *
 * - sprintf() formats to a buffer.
 * - printf() formats to stdio.
 */
int
snprintf(char *buf, int bsize, char *fmt, ...)
{
	int	tot;
	va_list	argp;

	va_start(argp,fmt);
	tot = vsnprintf(buf,bsize,fmt,argp);
	va_end(argp);
	return(tot);
}

int
sprintf(char *buf, char *fmt, ...)
{
	int	tot;
	va_list argp;

	va_start(argp,fmt);
	tot = vsnprintf(buf,0,fmt,argp);
	va_end(argp);
	return(tot);
}

int
printf(char *fmt, ...)
{
	int	tot;
	va_list argp;

	va_start(argp,fmt);
	tot = vsnprintf(0,0,fmt,argp);
	va_end(argp);
	return(tot);
}
