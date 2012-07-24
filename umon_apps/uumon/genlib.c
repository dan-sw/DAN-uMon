/* genlib.c:
 *	This file has some of the stuff that is typically pulled in by the c
 *	library.  It is provided as local source so that the monitor can be
 *	built without the need for a library.  Some of this code was taken from
 *	the Berkley source code (see notice below).
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
/*
 * Copyright (c) 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include "config.h"
#include "ctype.h"
#include "genlib.h"
#include "stddefs.h"

char *BSDCRN = "Copyright (c) 1988, 1993 The Regents of the University of California.  All rights reserved.";

unsigned char	ctypetbl[] = {
	 0,		_C,		_C,		_C,		_C,		_C,		_C,		_C,
	_C,		_S|_C,	_S|_C,	_S|_C,	_S|_C,	_S|_C,	_C,		_C,
	_C,		_C,		_C,		_C,		_C,		_C,		_C,		_C,
	_C,		_C,		_C,		_C,		_C,		_C,		_C,		_C,
	_S|_B,	_P,		_P,		_P,		_P,		_P,		_P,		_P,
	_P,		_P,		_P,		_P,		_P,		_P,		_P,		_P,
	_N|_X,	_N|_X,	_N|_X,	_N|_X,	_N|_X,	_N|_X,	_N|_X,	_N|_X,
	_N|_X,	_N|_X,	_P,		_P,		_P,		_P,		_P,		_P,
	_P,		_U|_X,	_U|_X,	_U|_X,	_U|_X,	_U|_X,	_U|_X,	_U,
	_U,		_U,		_U,		_U,		_U,		_U,		_U,		_U,
	_U,		_U,		_U,		_U,		_U,		_U,		_U,		_U,
	_U,		_U,		_U,		_P,		_P,		_P,		_P,		_P,
	_P,		_L|_X,	_L|_X,	_L|_X,	_L|_X,	_L|_X,	_L|_X,	_L,
	_L,		_L,		_L,		_L,		_L,		_L,		_L,		_L,
	_L,		_L,		_L,		_L,		_L,		_L,		_L,		_L,
	_L,		_L,		_L,		_P,		_P,		_P,		_P,		_C,
	 0,		 0,		 0,		 0,		 0,		 0,		 0,		 0,
	 0,		 0,		 0,		 0,		 0,		 0,		 0,		 0,
	 0,		 0,		 0,		 0,		 0,		 0,		 0,		 0,
	 0,		 0,		 0,		 0,		 0,		 0,		 0,		 0,
	 0,		 0,		 0,		 0,		 0,		 0,		 0,		 0,
	 0,		 0,		 0,		 0,		 0,		 0,		 0,		 0,
	 0,		 0,		 0,		 0,		 0,		 0,		 0,		 0,
	 0,		 0,		 0,		 0,		 0,		 0,		 0,		 0,
	 0,		 0,		 0,		 0,		 0,		 0,		 0,		 0,
	 0,		 0,		 0,		 0,		 0,		 0,		 0,		 0,
	 0,		 0,		 0,		 0,		 0,		 0,		 0,		 0,
	 0,		 0,		 0,		 0,		 0,		 0,		 0,		 0,
	 0,		 0,		 0,		 0,		 0,		 0,		 0,		 0,
	 0,		 0,		 0,		 0,		 0,		 0,		 0,		 0,
	 0,		 0,		 0,		 0,		 0,		 0,		 0,		 0,
	 0,		 0,		 0,		 0,		 0,		 0,		 0,		 0
};

#define DIGIT(x)	(isdigit(x) ? (x) - '0' : \
			islower(x) ? (x) + 10 - 'a' : (x) + 10 - 'A')
#define MBASE	('z' - 'a' + 1 + 10)

long
strtol(register char *str,char **ptr,register int base)
{
	register long val;
	register int c;
	int xx, neg = 0;

	if (ptr != (char **)0)
		*ptr = str; /* in case no number is formed */
	if (base < 0 || base > MBASE)
		return (0); /* base is invalid -- should be a fatal error */
	if (!isalnum((c = *str))) {
		while (isspace(c))
			c = *++str;
		switch (c) {
		case '-':
			neg++;
		case '+': /* fall-through */
			c = *++str;
		}
	}
	if (base == 0) {
		if (c != '0')
			base = 10;
		else if (str[1] == 'x' || str[1] == 'X')
			base = 16;
		else
			base = 8;
	}
	/*
	 * for any base > 10, the digits incrementally following
	 *	9 are assumed to be "abc...z" or "ABC...Z"
	 */
	if (!isalnum(c) || (xx = DIGIT(c)) >= base)
		return (0); /* no number formed */
	if (base == 16 && c == '0' && isxdigit(str[2]) &&
	    (str[1] == 'x' || str[1] == 'X'))
		c = *(str += 2); /* skip over leading "0x" or "0X" */
	for (val = -DIGIT(c); isalnum((c = *++str)) && (xx = DIGIT(c)) < base; )
		/* accumulate neg avoids surprises near MAXLONG */
		val = base * val - xx;
	if (ptr != (char **)0)
		*ptr = str;
	return (neg ? val : -val);
}

unsigned long
strtoul(char *str, char **ptr,int base)
{
	long	val;

	val = strtol(str, ptr, base);
	return((unsigned long)val);
}

/* Variables used by getopt: */
int	optind;			/* argv index to first cmd line arg that is not part
					 * of the option list.
					 */
char	*optarg;	/* pointer to argument associated with an option */
int	sp;

int
getopt(int argc,char *argv[],char *opts)
{
	register int c;
	register char *cp;

	if(sp == 1) {
		if (optind >= argc)
			return(-1);
		else if (argv[optind][0] != '-')
			return(-1);
		else if (argv[optind][1] == '\0')
			return(-1);
		else if(strcmp(argv[optind], "--") == 0) {
			optind++;
			return(-1);
		}
	}
	c = argv[optind][sp];
	if(c == ':' || (cp=strchr(opts, c)) == 0) {
		printf("Bad opt: %c\n",c);
		if(argv[optind][++sp] == '\0') {
			optind++;
			sp = 1;
		}
		return('?');
	}
	if(*++cp == ':') {
		if(argv[optind][sp+1] != '\0')
			optarg = &argv[optind++][sp+1];
		else if(++optind >= argc) {
			printf("Needs arg\n");
			sp = 1;
			return('?');
		} else
			optarg = argv[optind++];
		sp = 1;
	} else {
		if(argv[optind][++sp] == '\0') {
			sp = 1;
			optind++;
		}
		optarg = 0;
	}
	return(c);
}

/* getoptinit():
 * Since getopt() can be used by every command in the monitor
 * it's variables must be reinitialized prior to each command
 * executed through docommand()...
 */
void
getoptinit(void)
{
	sp = 1;
	optind = 1;
}

/* strcpy():
 * Copy string s2 to s1.  s1 must be large enough.
 * return s1
 */

char *
strcpy(register char *s1,register char *s2)
{
	register char *os1;

	os1 = s1;
	while((*s1++ = *s2++))
		;
	return(os1);
}

char *
strncpy(register char *s1,register char *s2,register int n)
{
	register char *os1 = s1;

	while (--n >= 0)
		if ((*s1++ = *s2++) == '\0')
			while (--n >= 0)
				*s1++ = '\0';
	return (os1);
}

/* strcmp():
 * Compare strings:  s1>s2: >0  s1==s2: 0  s1<s2: <0
 */
int
strcmp(register char *s1,register char * s2)
{

	if(s1 == s2)
		return(0);
	while(*s1 == *s2++)
		if(*s1++ == '\0')
			return(0);
	return(*s1 - *--s2);
}

/* strncmp():
 * Compare strings (at most n bytes)
 *  returns: s1>s2; >0  s1==s2; 0  s1<s2; <0
 */
int
strncmp(register char *s1,register char *s2,register int n)
{
    if(s1 == s2)
        return(0);
    while(--n >= 0 && *s1 == *s2++)
        if(*s1++ == '\0')
            return(0);
    return((n < 0)? 0: (*s1 - *--s2));
}

/* strlen():
 * Returns the number of
 * non-NULL bytes in string argument.
 */
int
strlen(register char *s)
{
	register char *s0 = s + 1;

	while (*s++ != '\0')
		;
	return (s - s0);
}

/* strchr():
 * Return the ptr in sp at which the character c appears;
 * NULL if not found
 */
char *
strchr(register char *sp, register char c)
{
	do {
		if(*sp == c)
			return(sp);
	} while(*sp++);
	return((char *)0);
}

/* memset():
 * Set an array of n chars starting at sp to the character c.
 * Return sp.
 */
char *
memset(register char *sp, register char c, register int n)
{
	register char *sp0 = sp;

	while (--n >= 0) {
		*sp++ = c;
	}
	return (sp0);
}

/* memcpy():
 * Copy one buffer to another.
 */
char *
memcpy(char *to,char *from,int count)
{
	char *end, *tmp;

	tmp = to;
	end = to+count;
	while(to < end)
		*to++ = *from++;

	return (tmp);
}

int
memcmp(char *s1,char *s2,int n)
{
    int diff;

    if (s1 != s2)
        while (--n >= 0)
            if ((diff = (*s1++ - *s2++)))
                return (diff);
    return (0);
}

int
abs(int arg)
{
	return (arg >= 0 ? arg : -arg);
}

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
			break;
		case '+': /* fall-through */
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

/* ticktock():
 * Put out a ticker...
 */
void
ticktock(void)
{
	static char tick;
	char	tock;

	switch(tick) {
	case 1:
		tock = '|';
		break;
	case 2:
		tock = '/';
		break;
	case 3:
		tock = '-';
		break;
	case 4:
		tock = '\\';
		break;
	default:
		tock = '|';
		tick = 1;
		break;
	}
	tick++;
	printf("%c\b",tock);
}

#if INCLUDE_FLASH
/* inRange():
 * This function is handed a range string and a value.
 * If the value is within the range of the string specified, then
 * return 1; else return 0.
 * The incoming string can be a mix of ranges and values with each
 * range and/or value separated by a comma and a range is specified 
 * by 2 numbers separated by a dash.
 * Also, incoming ranges of "all" or "any" immediately return true
 * and an incoming range of "none" or a null or empty pointer will
 * return false.
 */
int
inRange(char *range, int value)
{
	int		lo, hi;
	char 	rcopy[32], *rsp, *comma, *dash;

	/* If incoming range is NULL, return zero.
	 */
	if ((range == 0) || (*range == 0) || (strcmp(range,"none") == 0))
		return(0);

	/* If the range string is "all" or "any", then immediately return true...
	 */
	if ((strcmp(range,"all") == 0) || (strcmp(range,"any") == 0))
		return(1);

	/* Scan the range string for valid characters:
	 */
	rsp = range;
	while(*rsp) {
		if ((*rsp == ',') || (*rsp == '-') || 
			(*rsp == 'x') || isxdigit(*rsp))
			rsp++;
		else
			break;
	}
	if (*rsp)
		return(0);

	/* If incoming range string exceeds size of copy buffer, return 0.
	 */
	if (strlen(range) > sizeof(rcopy)-1)
		return(0);

	strcpy(rcopy,range);
	rsp = rcopy;
	do {
		comma = strchr(rsp,',');
		if (comma)
			*comma = 0;
		dash = strchr(rsp,'-');
		if (dash) {
			lo = strtol(rsp,0,0);
			hi = strtol(dash+1,0,0);
			if ((value >= lo) && (value <= hi))
				return(1);
		}
		else {
			if (value == strtol(rsp,0,0))
				return(1);
		}
		rsp = comma+1;
	} while (comma);
	return(0);
}
#endif
