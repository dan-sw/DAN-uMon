/* mprintf.c:
 *	A small, but useful printf.
 *
 *	Public functions:
 *	snprintf(), sprintf(), printf(), cprintf()
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

#define MAXARGS 			12
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
		if (bp >= eob) {						\
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
		if (bp >= eob) {						\
			puts("s_printf buffer overflow\n");	\
			return(-1);							\
		}										\
		*bp++ = c;								\
	}											\
	else {										\
		if (c)									\
			putchar(c);							\
	}

#define BUFDEC(bp,val)		\
	if (bp) {				\
		bp -= val;			\
	}		

#define BUFINC(bp,val)		\
	if (bp) {				\
		bp += val;			\
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
			*--lbp = (char)(value % 10) + '0';
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

/* long_to_ip():
 * Convert an incoming long value to an ascii IP formatted
 * string (i.e.  0x01020304 is converted to "1.2.3.4")
 */
static int
long_to_ip(long lval,char *buf,char *bufend,char *hdr)
{
	int	i, j, len;
	unsigned char *lp;

	len = 0;
	lp = (char *)&lval;
	for(j=0;j<4;j++) {
		i = long_to_dec(*lp++,buf,bufend,hdr);
		BUFINC(buf,i);
		PROCCHAR(buf,bufend,'.');
		len += (i + 1);
	}
	BUFDEC(buf,1);
	len--;
	return(len);
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

/* bin_to_mac():
 * Convert an incoming buffer to an ascii MAC formatted
 * string (i.e.  buf[] = 010203040506 is converted to "01:02:03:04:05:06")
 */
static int
bin_to_mac(uchar *ibin,char *buf,char *bufend)
{
	int	i, j, len;

	len = 0;
	for(j=0;j<6;j++) {
		i = long_to_hex(*ibin++,buf,bufend,"02");
		BUFINC(buf,i);
		PROCCHAR(buf,bufend,':');
		len += (i + 1);
	}
	BUFDEC(buf,1);
	len--;
	return(len);
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

/* snprintf():
 *	Backend to all the others below it.
 *	Formats incoming argument list based on format string.
 *	Terminates population of buffer if it is to exceed the
 *	specified buffer size.
 *
 * Currently these functions are all limited to 12 arguments.
 * This was done to avoid using varargs and/or stdargs.
 */
int
snprintf(char *buf,int bsize, char *fmt,
	long arg1,long arg2,long arg3,long arg4, long arg5, long arg6,
	long arg7,long arg8,long arg9,long arg10,long arg11,long arg12)
{
	int		i, sign, tot;
	char	*cp, hdr[HDRSIZE], *base, *bufend;
	long	args[MAXARGS], nextarg;

	tot = 0;
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
		if (nextarg >= MAXARGS) {
			puts("sprintf arg overflow\n");
			return(0);
		}
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
				PROCCHAR(buf,bufend,(char)args[nextarg]);
				nextarg++;
				tot++;
				break;
			case 's':			/* String conversion */
				i = build_string((char *)args[nextarg++],buf,bufend,hdr,sign);
				BUFINC(buf,i);
				tot += i;
				break;
			case 'M':			/* MAC address conversion */
				i = bin_to_mac((char *)args[nextarg++],buf,bufend);
				BUFINC(buf,i);
				tot += i;
				break;
			case 'I':			/* IP address conversion */
				i = long_to_ip(args[nextarg++],buf,bufend,hdr);
				BUFINC(buf,i);
				tot += i;
				break;
			case 'd':			/* Decimal conversion */
			case 'u':
				i = long_to_dec(args[nextarg++],buf,bufend,hdr);
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
				i = long_to_hex(args[nextarg++],buf,bufend,hdr);
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

/* sprintf(), printf() & cprintf():
 * All three functions use snprintf() to format a string.
 * The string is either transferred to a buffer or transferred
 * directly to this target's console through putchar (refer to
 * the macros at the top of this file).
 *
 * - sprintf() formats to a buffer.
 * - printf() formats to stdio.
 * - cprintf() formats to a buffer, then centers the content of
 *		the buffer based on its size and a console screen with of
 *		SCREENWIDTH characters.
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

int
printf(char *fmt, long arg1, long arg2, long arg3, long arg4,
	long arg5, long arg6, long arg7,long arg8,long arg9,
	long arg10, long arg11, long arg12)
{
	int	tot;

	tot = snprintf(0,0,fmt,
		arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12);

	return(tot);
}

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
