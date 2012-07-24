/* pio.c:
 *	Support the basic ability to configure and set/clear parallel IO pins.
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
#include "stddefs.h"
#include "genlib.h"
#include "cpuio.h"

#define PIO_NULL	0
#define PIO_IN		1		/* Configured as input */
#define PIO_INP		2		/* Configured as input (with pullup) */
#define PIO_OUT		3		/* Configured as output */


#if INCLUDE_PIO

void pioshow();

/* Pio():
	Display/Configure/Modify SH4 PIO pins.
	Syntax
	pio [options] {p4 or a7} {bit #} [0|1]
	(p4 is non-cached)

	  Options:
		-i 		configure as in
		-p 		configure as in with pullup
		-o   	configure as out
		-w   	wait for bit	

	Note: for Hotline, pio 16-19 correspond to LEDs D3-D6, a 0 turns them on.
*/


char *PioHelp[] = {
	"Programmable IO Interface",
	"-[ipow] {p4 | a7} {bit #} [0|1]",
	" -i		in (no pullup)",
	" -p		in with pullup",
	" -o		out",
	" -w		waitonbit",
	0,
};


int
Pio(argc,argv)
int argc;
char *argv[];
{
	int	opt, bitnum, val, config, waitonbit;
	ulong	addrmask;
	volatile ulong	pupctrl_mask, ioctrl_mask, *ctrlreg;
	volatile ushort	data_mask, *datareg;

	config = PIO_NULL;
	waitonbit = 0;
	while((opt=getopt(argc,argv,"iof:w")) != -1) {
		switch(opt) {
		case 'o':
			config = PIO_OUT;
			break;
		case 'i':
			config = PIO_IN;
			break;
		case 'p':
			config = PIO_INP;
			break;
		case 'w':
			waitonbit = 1;
			break;
		default:
			return(0);
		}
	}

	if (argc < (optind+1))
		return(-1);

	if (!strcmp(argv[optind],"p4"))
		addrmask = P4MASK;
	else if (!strcmp(argv[optind],"a7"))
		addrmask = A7MASK;
	else
		return(-1);

	if (argc == 2) {
		pioshow(addrmask);
		return(0);
	}

	if (argc < (optind+2))
		return(-1);

	bitnum = (int)strtol(argv[optind+1],(char **)0,0);
	if (bitnum < 0 || bitnum > 19) {
		printf("Bad bit #\n");
		return(0);
	}

	if (bitnum > 15) {
		ctrlreg = (ulong *)(PCTRB | addrmask);
		datareg = (ushort *)(PDTRB | addrmask);
		data_mask = (0x00000001 << (bitnum-16));
		ioctrl_mask = (0x00000001 << ((bitnum-16)*2));
		pupctrl_mask = (0x00000002 << ((bitnum-16)*2));
	}
	else {
		ctrlreg = (ulong *)(PCTRA | addrmask);
		datareg = (ushort *)(PDTRA | addrmask);
		data_mask = (0x00000001 << bitnum);
		ioctrl_mask = (0x00000001 << (bitnum*2));
		pupctrl_mask = (0x00000002 << (bitnum*2));
	}

	if (waitonbit) {
		if (argc != (optind+3)) {
			printf("-w requires bit and value\n");
			return(-1);
		}
		val = argv[optind+2][0] & 1;
		
		if (val == 1)
			while((*datareg & ~data_mask) == 0);
		else
			while(*datareg & data_mask);
		return(0);
	}

	/* If config, then set up direction or control */
	if (config != PIO_NULL) {
		/* Start off by clearing (pulled up input) */
		*ctrlreg &= ~(pupctrl_mask | ioctrl_mask);

		switch(config) {
		case PIO_IN:			/* Input, not pulled up */
			*ctrlreg |= pupctrl_mask;
			break;
		case PIO_OUT:
			*ctrlreg |= ioctrl_mask;
			break;
		}
		return(0);
	}

	/* If config is not set, then it must be a read or write request... */
	/* If second arg is present, then write; else read. */
	if (argc == optind+3) {
		val = argv[optind+2][0] & 1;
		if (val) {
			*datareg |= data_mask;
		}
		else {
			*datareg &= ~data_mask;
		}
	}
	else {		/* Read a pio bit */
		printf("Bit_%d = %d\n",bitnum,(*datareg & data_mask) ? 1:0);
	}
	return(0);
}

void
pioshow(ulong addrmask)
{
	int	bitnum;
	volatile ushort	data_mask, *datareg;

	printf("PCTRA = 0x%08lx\n",*(ulong *)(PCTRA | addrmask));
	printf("PDTRA = 0x%08x\n",*(ushort *)(PDTRA | addrmask));
	printf("PCTRB = 0x%08lx\n",*(ulong *)(PCTRB | addrmask));
	printf("PDTRB = 0x%08x\n",*(ushort *)(PDTRB | addrmask));
	printf("Bits 0-19: ");
	for(bitnum=0; bitnum<20; bitnum++) {
		if (bitnum > 15) {
			datareg = (ushort *)(PDTRB | addrmask);
			data_mask = (0x0001 << (bitnum-16));
		}
		else {
			datareg = (ushort *)(PDTRA | addrmask);
			data_mask = (0x0001 << bitnum);
		}
		printf("%d",(*datareg & data_mask) ? 1:0);
	}
	printf("\n");
}
#endif

int
sh4pioget(ulong addrmask,int bitnum)
{
	volatile ushort	data_mask, *datareg;

	if (bitnum > 15) {
		datareg = (ushort *)(PDTRB | addrmask);
		data_mask = (0x0001 << (bitnum-16));
	}
	else {
		datareg = (ushort *)(PDTRA | addrmask);
		data_mask = (0x0001 << bitnum);
	}
	return((*datareg & data_mask) ? 1:0);
}

void
sh4pioclr(ulong addrmask,int bitnum)
{
	volatile ushort	data_mask, *datareg;

	if (bitnum > 15) {
		datareg = (ushort *)(PDTRB | addrmask);
		data_mask = (0x0001 << (bitnum-16));
	}
	else {
		datareg = (ushort *)(PDTRA | addrmask);
		data_mask = (0x0001 << bitnum);
	}
	*datareg &= ~data_mask;
}

void
sh4pioset(ulong addrmask,int bitnum)
{
	volatile ushort	data_mask, *datareg;

	if (bitnum > 15) {
		datareg = (ushort *)(PDTRB | addrmask);
		data_mask = (0x0001 << (bitnum-16));
	}
	else {
		datareg = (ushort *)(PDTRA | addrmask);
		data_mask = (0x0001 << bitnum);
	}
	*datareg |= data_mask;
}

/* WARNING:::::
 * These three functions must be mapped to the above sh4XXX functions
 * sooner or later.
 */
int
pioget(char port, int bitnum)
{
	return(-1);
}

void
pioset(char port, int bitnum)
{
}

void
pioclr(char port, int bitnum)
{
}
