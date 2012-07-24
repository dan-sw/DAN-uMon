/*
 *	Huffman encoding program 
 *	Usage:	pack [[ - ] filename ... ] filename ...
 *		- option: enable/disable listing of statistics
 *  ELS:
 *  This is a hack of the original pack.c to support packing of raw data
 *  within a program.  No files, just memory space.  The entry point is
 *  the function packdata() below.
 */


#include  <stdio.h>
#include  <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "packdata.h"

//extern	char *malloc();

typedef unsigned char uchar;

#define	END	256
#define NAMELEN 80
#define	SUF0	'.'
#define	SUF1	'z'

/* union for overlaying a long int with a set of four characters */
union FOUR {
	struct { long int lng; } lint;
	struct { char c0, c1, c2, c3; } chars;
};

/* character counters */
static long	count [END+1];
static union	FOUR insize;
static long	outsize;
static long	dictsize;
static int	diffbytes;

/* i/o stuff */
static char	inbuff [BUFSIZ];
static char	outbuff [BUFSIZ+4];

/* variables associated with the tree */
static int	maxlev;
static int	levcount [25];
static int	lastnode;
static int	parent [2*END+1];

/* variables associated with the encoding process */
static char	length [END+1];
static long	bits [END+1];
static union	FOUR mask;
static long	inc;
#if defined(vax) || defined(i386)
char	*maskshuff[4]  = {&(mask.chars.c3), &(mask.chars.c2), &(mask.chars.c1), &(mask.chars.c0)};
#elif defined(pdp11)
char	*maskshuff[4]  = {&(mask.chars.c1), &(mask.chars.c0), &(mask.chars.c3), &(mask.chars.c2)};
#else	/* just about everything else */
char	*maskshuff[4]  = {&(mask.chars.c0), &(mask.chars.c1), &(mask.chars.c2), &(mask.chars.c3)};
#endif

/* the heap */
static int	n;
static struct	heap {
	long int count;
	int node;
} heap [END+2];
#define hmove(a,b) {(b).count = (a).count; (b).node = (a).node;}

/* encode the current file */
/* return 1 if successful, 0 otherwise */
static int
output (void)
{
	extern long lseek();
	int c, i, inleft;
	char *inp;
	register char **q, *outp;
	register int bitsleft;
	long temp;

	/* output ``PACKED'' header */
	outbuff[0] = 037; 	/* ascii US */
	outbuff[1] = 036; 	/* ascii RS */
	/* output the length and the dictionary */
	temp = insize.lint.lng;
	for (i=5; i>=2; i--) {
		outbuff[i] =  (char) (temp & 0377);
		temp >>= 8;
	}
	outp = &outbuff[6];
	*outp++ = maxlev;
	for (i=1; i<maxlev; i++)
		*outp++ = levcount[i];
	*outp++ = levcount[maxlev]-2;
	for (i=1; i<=maxlev; i++)
		for (c=0; c<END; c++)
			if (length[c] == i)
				*outp++ = c;
	dictsize = outp-&outbuff[0];

	/* output the text */
	outsize = 0;
	bitsleft = 8;
	inleft = 0;
	do {
		if (inleft <= 0) {
			inleft = getdata(inp = &inbuff[0], BUFSIZ);
			if (inleft < 0) {
				fprintf(stderr,": getdata() error");
				return (0);
			}
		}
		c = (--inleft < 0) ? END : (*inp++ & 0377);
		mask.lint.lng = bits[c]<<bitsleft;
		q = &maskshuff[0];
		if (bitsleft == 8)
			*outp = **q++;
		else
			*outp |= **q++;
		bitsleft -= length[c];
		while (bitsleft < 0) {
			*++outp = **q++;
			bitsleft += 8;
		}
		if (outp >= &outbuff[BUFSIZ]) {
			putdata(outbuff, BUFSIZ);
			((union FOUR *) outbuff)->lint.lng = ((union FOUR *) &outbuff[BUFSIZ])->lint.lng;
			outp -= BUFSIZ;
			outsize += BUFSIZ;
		}
	} while (c != END);
	if (bitsleft < 8)
		outp++;
	c = outp-outbuff;
	putdata(outbuff,c);
	outsize += c;
	return (1);
}

/* makes a heap out of heap[i],...,heap[n] */
static void
heapify (int i)
{
	register int k;
	int lastparent;
	struct heap heapsubi;
	hmove (heap[i], heapsubi);
	lastparent = n/2;
	while (i <= lastparent) {
		k = 2*i;
		if (heap[k].count > heap[k+1].count && k < n)
			k++;
		if (heapsubi.count < heap[k].count)
			break;
		hmove (heap[k], heap[i]);
		i = k;
	}
	hmove (heapsubi, heap[i]);
}

/* return 1 after successful packing, 0 otherwise */
static int
packbuffer(void)
{
	register int c, i, p;
	long bitsout;

	/* put occurring chars in heap with their counts */
	diffbytes = -1;
	count[END] = 1;
	insize.lint.lng = n = 0;
	for (i=END; i>=0; i--) {
		parent[i] = 0;
		if (count[i] > 0) {
			diffbytes++;
			insize.lint.lng += count[i];
			heap[++n].count = count[i];
			heap[n].node = i;
		}
	}
	if (diffbytes == 1) {
		fprintf(stderr,": trivial file");
		return (0);
	}
	insize.lint.lng >>= 1;
	for (i=n/2; i>=1; i--)
		heapify(i);

	/* build Huffman tree */
	lastnode = END;
	while (n > 1) {
		parent[heap[1].node] = ++lastnode;
		inc = heap[1].count;
		hmove (heap[n], heap[1]);
		n--;
		heapify(1);
		parent[heap[1].node] = lastnode;
		heap[1].node = lastnode;
		heap[1].count += inc;
		heapify(1);
	}
	parent[lastnode] = 0;

	/* assign lengths to encoding for each character */
	bitsout = maxlev = 0;
	for (i=1; i<=24; i++)
		levcount[i] = 0;
	for (i=0; i<=END; i++) {
		c = 0;
		for (p=parent[i]; p!=0; p=parent[p])
			c++;
		levcount[c]++;
		length[i] = c;
		if (c > maxlev)
			maxlev = c;
		bitsout += c*(count[i]>>1);
	}
	if (maxlev > 24) {
		/* can't occur unless insize.lint.lng >= 2**24 */
		fprintf(stderr,": Huffman tree has too many levels");
		return(0);
	}

	/* don't bother if no compression results */
	outsize = ((bitsout+7)>>3)+6+maxlev+diffbytes;
	if ((insize.lint.lng+BUFSIZ-1)/BUFSIZ <= (outsize+BUFSIZ-1)/BUFSIZ) {
		fprintf(stderr,"no saving");
		return(0);
	}

	/* compute bit patterns for each character */
	inc = 1L << 24;
	inc >>= maxlev;
	mask.lint.lng = 0;
	for (i=maxlev; i>0; i--) {
		for (c=0; c<=END; c++)
			if (length[c] == i) {
				bits[c] = mask.lint.lng;
				mask.lint.lng += inc;
			}
		mask.lint.lng &= ~inc;
		inc <<= 1;
	}
	return (output());
}

int sourceSize;
char *sourceBase;
char *destBase;

int
getdata(char *buffer,int size)
{
	int	i, tot;

	if (size < sourceSize)
		tot = size;
	else
		tot = sourceSize;

	sourceSize -= tot;
	for(i=0;i<tot;i++)
		*buffer++ = *sourceBase++;
	return(tot);
}

int
putdata(char *buffer,int size)
{
	int	i;

	for(i=0;i<size;i++)
		*destBase++ = *buffer++;
	return(size);
}

int
packdata(char *src,char *dest,int srcsize,int verbose)
{
	int	i;

	sourceSize = srcsize;
	sourceBase = src;
	destBase = dest;

	/* Gather character frequency statistics */
	for (i=0; i<END; i++)
		count[i] = 0;
	for(i=0;i<srcsize;i++)
		count[(src[i]&0xff)] += 2;

	if (!packbuffer()) {
		fprintf(stderr," - file unchanged\n");
		return(0);
	}

	if (verbose) {
		printf("%d%% compression (from %d to %d bytes)\n",
			(int)(((double)(-outsize+(insize.lint.lng))/
			 (double)insize.lint.lng)*100),insize.lint.lng, outsize);
	}

	return(outsize);
}
