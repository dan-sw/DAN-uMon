/* 
 * pio_arm.c
 *
 * Support the basic ability to configure and set/clear 
 * the GPIO pins of the SA-1110 / SA-1100.
 *
 * by Nick Patavalis (npat@inaccessnetworks.com)
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
 *
 */

#include "config.h"

#if INCLUDE_PIO
#include "genlib.h"


/*************************************************************************/

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

/*************************************************************************/

#ifndef PIO_DUMMY
#include "SA-1100.h"
#else
volatile ulong GPLR;
volatile ulong GPDR;
volatile ulong GPSR;
volatile ulong GPCR;
volatile ulong GRER;
volatile ulong GFER;
volatile ulong GEDR;
volatile ulong GAFR;
#endif

/*************************************************************************/

static char *
prbin(char *bufA, ulong lval, int nbits,
	  int split, int hex, ulong mrk)
{
	static char lbuf[128], *buf;
	int i;
	ulong msk;

	if (nbits > 32) nbits = 32;
	if (!bufA) bufA = lbuf;

	buf = bufA;
	msk = 0x00000001UL << (nbits - 1);
	for (i=0; i< nbits; i++) {
		if ( split ) {
			if ( i && ! (i % 8) )
				*buf++ = ' ';
			else if ( i && ! (i % 4) )
				*buf++ = '.';
		}
		if ( mrk & msk ) {
			*buf++ = '<';
		}
		*buf++ = lval & msk?'1':'0';
		if ( mrk & msk ) {
			*buf++ = '>';
		}
		msk >>= 1;
	}
	if (hex)
		sprintf(buf, " = 0x%x", lval);
	else
		*buf = '\0';

	return bufA;
}

/*************************************************************************/

#ifdef PIO_DEBUG
static void 
piodump (void)
{
	printf("GPLR = %s\n", prbin(0, GPLR, 32, 1, 1, 0)); 
	printf("GPDR = %s\n", prbin(0, GPDR, 32, 1, 1, 0)); 
#ifdef PIO_DUMMY
	printf("GPSR = %s\n", prbin(0, GPSR, 32, 1, 1, 0)); 
	printf("GPCR = %s\n", prbin(0, GPCR, 32, 1, 1, 0)); 
#endif
	printf("GRER = %s\n", prbin(0, GRER, 32, 1, 1, 0)); 
	printf("GFER = %s\n", prbin(0, GFER, 32, 1, 1, 0)); 
	printf("GEDR = %s\n", prbin(0, GEDR, 32, 1, 1, 0)); 
	printf("GAFR = %s\n", prbin(0, GAFR, 32, 1, 1, 0)); 
}
#endif

/*************************************************************************/

static void
pioshow(ulong msk)
{
	int i;
	ulong sel;

	printf("PIN PLR PDR RER FER EDR AFR\n");
	for (i=0, sel = 1; i < 28; i++, sel <<= 1) {
		if (msk & sel) {
			printf("%3d %3s %3s %3s %3s %3s %3s\n",
				   i,
				   (GPLR & sel)?"on":"off",
				   (GPDR & sel)?"out":"in",
				   (GRER & sel)?"on":"off",
				   (GFER & sel)?"on":"off",
				   (GEDR & sel)?"on":"off",
				   (GAFR & sel)?"af":"gio");
		}
	}
}

/*************************************************************************/

int
pioget (char port, int bit)
{
	if (port != 'a' || port != 'A' || bit > 27 || bit < 0 )
		return -1;

	return GPLR & (1UL << bit);
}

/*************************************************************************/

void
pioset (char port, int bit)
{
	if (port != 'a' || port != 'A' || bit > 27 || bit < 0 )
		return;

	GPSR = 1UL << bit;

	return;
}

/*************************************************************************/

void
pioclr (char port, int bit)
{
	if (port != 'a' || port != 'A' || bit > 27 || bit < 0 )
		return;

	GPCR = 1UL << bit;

	return;
}

/*************************************************************************/

char *PioHelp[] = {
	"General Purpose IO Interface",
	"[{operation}... ({bit#}|m{mask})]",
	"{operation}s are:"
	"  -a   select alternate function",
	"  -g   use as gpio (no alternate function)",
	"  -s   set",
	"  -c   clear",
	"  -o   configure as output",
	"  -i   configure as input",
	"  -r   detect rising edge",
	"  -f   detect falling edge",
	"  -n   configure as interrupt source",
	"  -e   clear edge-detected flag",
    "  -w   wait for edge detection",
	"  -d   display detailed info on pin",
	"{bit#} is the bit number in decimal (0-27)",
	"{mask} is used to perform the operation(s) on multiple bits",
	"the order multiple operations are performed is the same as the order", 
	"  they appear in the list above",
	"if no operation is specified info on the pin(s) state(s) is given",
	"if no bits are specified info on the state of all pins is given",
	"  regardless of the operation flags",
	0,
};

/*************************************************************************/

int
Pio(argc,argv)
int argc;
char *argv[];
{
	int opt;
	int op_a, op_g, op_s, op_c, op_o, op_i,
		op_r, op_f, op_n, op_e, op_w, op_d;
	int op_any = 0;
	int bn;
	ulong msk;

	op_a = 0; op_g = 0; op_s = 0; op_c = 0; op_o = 0; op_i = 0;
	op_r = 0; op_f = 0; op_n = 0; op_e = 0; op_w = 0; op_d = 0;
	op_any = 0;

	while( ( opt = getopt(argc,argv,"agscoirfnewd") ) != -1 ) {
		switch(opt) {
		case 'a':
			op_a = 1;
			op_any = 1;
			break;
		case 'g':
			op_g = 1;
			op_any = 1;
			break;
		case 's':
			op_s = 1;
			op_any = 1;
			break;
		case 'c':
			op_c = 1;
			op_any = 1;
			break;
		case 'o':
			op_o = 1;
			op_any = 1;
			break;
		case 'i':
			op_i = 1;
			op_any = 1;
			break;
		case 'r':
			op_r = 1;
			op_any = 1;
			break;
		case 'f':
			op_f = 1;
			op_any = 1;
			break;
		case 'n':
			op_n = 1;
			op_any = 1;
			break;
		case 'e':
			op_e = 1;
			op_any = 1;
			break;
		case 'w':
			op_w = 1;
			op_any = 1;
			break;
		case 'd':
			op_d = 1;
			op_any = 1;
			break;
		default:
			return(0);
		}
	}

	if ( ! op_any ) {
		/* no operation(s) defined. default to -d */
		op_d = 1;
		op_any = 1;
	}

	if (argc < (optind+1)) {
		/* no bits defined. force operation to -d */
		op_a = 0; op_g = 0; op_s = 0; op_c = 0; op_o = 0; op_i = 0;
		op_r = 0; op_f = 0; op_n = 0; op_e = 0; op_w = 0; 
		op_d = 1;
		op_any = 1;
		msk = 0x0FFFFFFF;
	} else {
		if ( argv[optind][0] == 'm' ||  argv[optind][0] == 'M' ) {
			msk = strtoul(&(argv[optind][1]),(char **)0,0);
			msk &= 0x0FFFFFFF;
		} else {
			bn = strtoul(argv[optind],(char **)0,0);
			msk = 0x00000001UL << bn;
			msk &= 0x0FFFFFFF;
		}
	}

	if (op_a) {
		GAFR |= msk;
	}
	if (op_g) {
		GAFR &= ~msk;
	}
	if (op_s) {
		GPSR = msk;
	}
	if (op_c) {
		GPCR = msk;
	}
	if (op_o) {
		GPDR |= msk;
	}
	if (op_i) {
		GPDR &= ~msk;
	}
	if (op_r) {
		GRER = msk; 
	}
	if (op_f) {
		GFER = msk;
	}
	if (op_n) {
		printf ("Not yet!\n");
	}
	if (op_e) {
		GEDR = msk;
	}
	if (op_w) {
		while(GEDR & msk);
	}
	if (op_d) {
		pioshow(msk);
	}

#ifdef PIO_DEBUG
	piodump();
#endif

	return(0);
}

#else /* not INCLUDE_PIO */

int
pioget (char port, int bit)
{
	return -1;
}

void
pioset (char port, int bit)
{
	return;
}

void
pioclr (char port, int bit)
{
	return;
}

#endif /* of INCLUDE_PIO */

/*************************************************************************/

#ifdef PIO_DEBUG

int 
main (int argc, char * argv)
{
	Pio(argc, argv);

	return (0);
}

#endif /* of PIO_DEBUG */

/*************************************************************************/
