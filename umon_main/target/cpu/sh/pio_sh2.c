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
#include "cpuio.h"
#include "genlib.h"
#include "ctype.h"


#define PIO_NULL	0
#define PIO_IN		1
#define PIO_OUT		2
#define FUNCTION1	4
#define FUNCTION2	5
#define FUNCTION3	6
#define FUNCTION4	7


void pioshow();

struct portinfo {
	uchar	name;			/* port name */
	uchar	width;			/* port width */
	ushort	*iohigh;		/* address IO register HI */
	ushort	*iolow;			/* address IO register LO */
	ushort	*ctrlhigh2;		/* address CTRL register HI 2 */
	ushort	*ctrlhigh1;		/* address CTRL register HI 1 */
	ushort	*ctrllow2;		/* address CTRL register LO 2 */
	ushort	*ctrllow1;		/* address CTRL register LO 1 */
	ushort	*datahigh;		/* address DATA register HI */
	ushort	*datalow;		/* address DATA register LO */
};

#define volatile

struct portinfo pdata[] = {
	{ 'A',	24,
		PAIORH,	PAIORL, 0,		PACRH,	PACRL2,	PACRL1, PADRH,	PADRL }, 

	{ 'B',	10,
		0, 		PBIOR,	0,		0,		PBCR2,	PBCR1,	0,		PBDR },

	{ 'C',	16,
		0, 		PCIOR,	0,		0,		0,		PCCR,	0,		PCDR },

	{ 'D',	32,
		PDIORH,	PDIORL, PDCRH2, PDCRH1, 0,		PDCRL,	PDDRH,	PDDRL },

	{ 'E',	16,
		0, 		PEIOR,	0,		0,		PECR2,	PECR1,	0,		PEDR },

	{ 'F',	8,
		0,		0,		0,		0,		0,		0,		0,		PFDR },

	{ 0,0,0,0,0,0,0,0,0,0 }
};

#undef volatile

#if INCLUDE_PIO

/* Pio():
	Display/Configure/Modify SH2 PIO pins.
	Syntax
	pio [options] {A-F} {bit #} [0|1]

	  Options:
		-i 		configure as in
		-o   	configure as out
		-f# 	configure function number
		-w   	wait for bit	
*/


char *PioHelp[] = {
	"Programmable IO Interface",
	"-[f:iow] {A-F} {bit #} [0|1]",
	" -f {1-4}	muxed pin function",
	" -i		in",
	" -o		out",
	" -w		waitonbit",
	0,
};


int
Pio(argc,argv)
int argc;
char *argv[];
{
	int	opt, bitnum, val, config, portName, waitonbit;
	struct	portinfo	*pip;
	ushort	mask, *datareg, *ioreg, *ctrl1reg, *ctrl2reg;

	if (argc == 1) {
		pioshow();
		return(0);
	}

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
		case 'f':			/* Refer to 7040 hardware manual chap 17 */
			switch(*optarg) {
			case '0':
				config = FUNCTION1;
				break;
			case '1':
				config = FUNCTION2;
				break;
			case '2':
				config = FUNCTION3;
				break;
			case '3':
				config = FUNCTION4;
				break;
			default:
				return(-1);
			}
			break;
		case 'w':
			waitonbit = 1;
			break;
		default:
			return(0);
		}
	}

	if (argc < (optind+2))
		return(-1);

	portName = toupper(argv[optind][0]);
	bitnum = (int)strtol(argv[optind+1],(char **)0,0);

	for (pip=pdata;pip->name;pip++) {
		if (portName == pip->name)
			break;
	}
	if (!pip->name)
		return(-1);

	if (bitnum > pip->width) {
		printf("Bit # out of range\n");
		return(0);
	}

	if (bitnum > 15) {
		mask = 0x0001 << (bitnum-15);
		datareg = pip->datahigh;
		ctrl1reg = pip->ctrlhigh1;
		ctrl2reg = pip->ctrlhigh2;
		ioreg = pip->iohigh;
	}
	else {
		mask = 0x0001 << bitnum;
		datareg = pip->datalow;
		ctrl1reg = pip->ctrllow1;
		ctrl2reg = pip->ctrllow2;
		ioreg = pip->iolow;
	}

	if (waitonbit) {
		if (argc != (optind+3)) {
			printf("-w requires port, bit and value\n");
			return(-1);
		}
		val = argv[optind+2][0] & 1;
		
		if (portName == 'F') {
			if (val == 1)
				while((*(uchar *)datareg && (uchar)~mask) == 0);
			else
				while(*(uchar *)datareg && (uchar)mask);
		}
		else {
			if (val == 1)
				while((*datareg && (uchar)~mask) == 0);
			else
				while(*datareg && mask);
		}
		return(0);
	}

	/* If config, then set up direction or control */
	if (config != PIO_NULL) {
/*
		if (argc != (optind+2))
			return(-1);
*/
		switch(config) {
		case PIO_IN:
			*ioreg &= ~mask;
			break;
		case PIO_OUT:
			*ioreg |= mask;
			break;
		case FUNCTION1:
		case FUNCTION2:
		case FUNCTION3:
		case FUNCTION4:
			printf("Not ready yet!\n");
			break;
		}
		return(0);
	}

	/* If config is not set, then it must be a read or write request... */
	/* If third arg is present, then write; else read. */
	if (argc == optind+3) {
		val = argv[optind+2][0] & 1;
		if (portName == 'F') {
			if (val)
				*(uchar *)datareg |= (uchar)mask;
			else
				*(uchar *)datareg &= ~(uchar)mask;
		}
		else {
			if (val)
				*datareg |= mask;
			else
				*datareg &= ~mask;
		}
	}
	else {		/* Read a pio bit */
		if (portName == 'F')
			printf("P%c%d = %d\n",
				pip->name,bitnum,(*(uchar *)datareg & (uchar)mask) ? 1:0);
		else
			printf("P%c%d = %d\n",pip->name,bitnum,(*datareg & mask) ? 1:0);
	}
	return(0);
}

void
pioshow()
{
	int i;
	struct portinfo *pip;

	pip = pdata;
	for(i=0;i<6;i++,pip++) {
		printf("Port %c (%dbits):\n",pip->name,pip->width);
		if (pip->iohigh) {
			printf(" P%cIORH: 0x%04x,",pip->name,*(pip->iohigh));
			printf(" P%cIORL: 0x%04x\n",pip->name,*(pip->iolow));
		}
		else if (pip->iolow) 
			printf(" P%cIOR:  0x%04x\n",pip->name,*(pip->iolow));

		if (pip->ctrlhigh2) {
			printf(" P%cCRH2: 0x%04x,",pip->name,*(pip->ctrlhigh2));
			printf(" P%cCRH1: 0x%04x,",pip->name,*(pip->ctrlhigh1));
		}
		else if (pip->ctrlhigh1)
			printf(" P%cCRH:  0x%04x,",pip->name,*(pip->ctrlhigh1));

		if (pip->ctrllow2) {
			printf(" P%cCRL2: 0x%04x,",pip->name,*(pip->ctrllow2));
			printf(" P%cCRL1: 0x%04x\n",pip->name,*(pip->ctrllow1));
		}
		else if (pip->ctrllow1)
			printf(" P%cCRL:  0x%04x\n",pip->name,*(pip->ctrllow1));

		if (pip->datahigh) {
			printf(" P%cDRH:  0x%04x,",pip->name,*(pip->iohigh));
			printf(" P%cDRL:  0x%04x\n",pip->name,*(pip->iolow));
		}
		else if (pip->datalow)
			printf(" P%cDR:   0x%04x\n",pip->name,*(pip->iolow));
	}
}
#endif

int
pioget(port,bitnum)
char	port;
int	bitnum;
{
	struct portinfo *pip;
	ushort	mask, *datareg;

	port = toupper(port);
	for (pip=pdata;pip->name;pip++) {
		if (port == pip->name)
			break;
	}
	if (!pip->name)
		return(-1);
	if (bitnum > 15) {
		mask = 0x0001 << (bitnum-15);
		datareg = pip->datahigh;
	}
	else {
		mask = 0x0001 << bitnum;
		datareg = pip->datalow;
	}
	return((*datareg & mask) ? 1:0);
}

void
pioclr(port,bitnum)
char	port;
int	bitnum;
{
	struct portinfo *pip;
	ushort	mask, *datareg;

	port = toupper(port);
	for (pip=pdata;pip->name;pip++) {
		if (port == pip->name)
			break;
	}
	if (!pip->name) {
		printf("pioclr error\n");
		return;
	}
	if (bitnum > 15) {
		mask = 0x0001 << (bitnum-15);
		datareg = pip->datahigh;
	}
	else {
		mask = 0x0001 << bitnum;
		datareg = pip->datalow;
	}
	*datareg &= ~mask;
}

void
pioset(port,bitnum)
char	port;
int	bitnum;
{
	struct portinfo *pip;
	ushort	mask, *datareg;

	port = toupper(port);
	for (pip=pdata;pip->name;pip++) {
		if (port == pip->name)
			break;
	}
	if (!pip->name) {
		printf("pioset error\n");
		return;
	}
	if (bitnum > 15) {
		mask = 0x0001 << (bitnum-15);
		datareg = pip->datahigh;
	}
	else {
		mask = 0x0001 << bitnum;
		datareg = pip->datalow;
	}
	*datareg |= mask;
}
