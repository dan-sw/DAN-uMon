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
#include "cpuio.h"
#include "ctype.h"
#include "genlib.h"
#include "mpc860.h"
#include "arch_ppc.h"

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;


#define PIO_IN	1
#define PIO_OUT	2
#define PIO_GP	3
#define PIO_DED	4
#define ALL_PIO	0x7f

void	pioshow(char);

/* Pio():
	Display/Configure/Modify MPC860 PIO pins.
	Syntax
	pio [options] {A|B|C|D} {bit #} [0|1]

	  For port A, bit # range = 0-15
	  For port B, bit # range = 14-31
	  For port C, bit # range = 4-15
	  For port D, bit # range = 3-15
	  Options:
		-i 	configure as in
		-o 	configure as out
		-g 	configure as gpio
		-d 	configure as dedicated

    Note: recall that the PowerPC documentation reverses the order of bits
	(for some ridiculous reason), so bit 1 in a 32 bit register corresponds
	to 0x80000000, not 0x00000001; likewise, bit 1 in a 16 bit register is
	0x8000, not 0x0001.
*/

extern EPPC	*IMMRBase;

#if INCLUDE_PIO

char *PioHelp[] = {
	"Programmable IO Interface",
	"-[iogdw] {PortName} [bit #] [0|1]",
	" -i   input",
	" -o   output",
	" -g   gpio",
	" -d   dedicated",
	" -w   waitonbit",
	0,
};

int
Pio(argc,argv)
int argc;
char *argv[];
{
	char	portName;
	int	opt, bitnum, val, config, waitonbit, boor;
	volatile ushort	*dat16, *dir16, *par16;
	volatile ulong	*dat32, *dir32, *par32;

	dat16 = dir16 = par16 = (ushort *)0;
	dat32 = dir32 = par32 = (ulong *)0;

	/* Set IMMRBase pointing to the MPC860's internal register memory map */
	/* (should have already been done by InitUART(), but just do it again */
	/* to be safe). */
   	IMMRBase = (EPPC *)(ppcMfimmr() & 0xFFFF0000);

	/* Show all ports: */
	if (argc == 1) {
		pioshow(ALL_PIO);
		return(0);
	}

	/* Show one port: */
	if (argc == 2) {
		portName = (char)toupper(argv[optind][0]);
		switch(portName) {
		case 'A':
		case 'B':
		case 'C':
		case 'D':
			pioshow(portName);
			return(0);
		default:
			return(-1);
		}
	}

	config = 0;
	waitonbit = 0;
	boor = 0;
	while((opt=getopt(argc,argv,"iogdw")) != -1) {
		switch(opt) {
		case 'o':
			config = PIO_OUT;
			break;
		case 'g':
			config = PIO_GP;
			break;
		case 'd':
			config = PIO_DED;
			break;
		case 'i':
			config = PIO_IN;
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

	portName = (char)toupper(argv[optind][0]);
	bitnum = (int)strtol(argv[optind+1],(char **)0,0);

	/* Clear bit-out-of-range flag, then check for bit out of range... */
	switch(portName) {
		case 'A':
			if ((bitnum < 0) || (bitnum > 15)) 
				boor = 1;
			else {
				dat16 = &IMMRBase->pio_padat;			/* data */
				dir16 = &IMMRBase->pio_padir;			/* direction */
				par16 = &IMMRBase->pio_papar;			/* pin assignment */
			}
			break;
		case 'B':
			if ((bitnum < 14) || (bitnum > 31)) 
				boor = 1;
			else {
				dat32 = &IMMRBase->pip_pbdat;
				dir32 = &IMMRBase->pip_pbdir;
				par32 = &IMMRBase->pip_pbpar;
			}
			break;
		case 'C':
			if ((bitnum < 4) || (bitnum > 15)) 
				boor = 1;
			else {
				dat16 = &IMMRBase->pio_pcdat;
				dir16 = &IMMRBase->pio_pcdir;
				par16 = &IMMRBase->pio_pcpar;
			}
			break;
		case 'D':
			if ((bitnum < 3) || (bitnum > 15)) 
				boor = 1;
			else {
				dat16 = &IMMRBase->pio_pddat;
				dir16 = &IMMRBase->pio_pddir;
				par16 = &IMMRBase->pio_pdpar;
			}
			break;
		default:
			printf("Invalid port: %c\n",portName);
			return(0);
	}

	if (boor) {
		printf("Bit %d out of range for port %c\n",bitnum,portName);
		return(0);
	}

	if (waitonbit) {
		if (argc != (optind+3)) {
			printf("-w requires port, bit and value\n");
			return(-1);
		}
		val = argv[optind+2][0] & 1;
		if (portName == 'B') {
			while(1) {
				if ((val == 1) && (*dat32 & (ulong)(0x80000000>>bitnum)))
					break;
				else if ((val == 0) && !(*dat32 & (ulong)(0x80000000>>bitnum)))
					break;
			}
		}
		else {
			while(1) {
				if ((val == 1) && (*dat16 & (ushort)(0x8000 >> bitnum)))
					break;
				else if ((val == 0) && !(*dat16 & (ushort)(0x8000 >> bitnum)))
					break;
			}
		}
		return(0);
	}
	/* If config, then set up direction or control */
	if (config) {
		if (argc != (optind+2))
			return(-1);
		if (portName == 'B') {
			switch(config) {
			case PIO_OUT:
				*dir32 |= (ulong)(0x80000000 >> bitnum);
				break;
			case PIO_IN:
				*dir32 &= (ulong)~(0x80000000 >> bitnum);
				break;
			case PIO_GP:
				*par32 &= (ulong)~(0x80000000 >> bitnum);
				break;
			case PIO_DED:
				*par32 |= (ulong)(0x80000000 >> bitnum);
				break;
			}
		}
		else {
			switch(config) {
			case PIO_OUT:
				*dir16 |= (ushort)(0x8000 >> bitnum);
				break;
			case PIO_IN:
				*dir16 &= (ushort)~(0x8000 >> bitnum);
				break;
			case PIO_GP:
				*par16 &= (ushort)~(0x8000 >> bitnum);
				break;
			case PIO_DED:
				*par16 |= (ushort)(0x8000 >> bitnum);
				break;
			}
		}
		return(0);
	}

	/* If config is not set, then it must be a read or write request... */
	/* If third arg is present, then write; else read. */
	if (argc == optind+3) {
		val = argv[optind+2][0] & 1;
		if (portName == 'B') {
			if (val)
				*dat32 |= (ulong)(0x80000000 >> bitnum);
			else
				*dat32 &= (ulong)~(0x80000000 >> bitnum);
		}
		else {
			if (val)
				*dat16 |= (ushort)(0x8000 >> bitnum);
			else
				*dat16 &= (ushort)~(0x8000 >> bitnum);
		}
	}
	else {			/* Read a pio bit */
		if (portName == 'B') {
			printf("P%c%d = %d\n",portName,bitnum,
			    (*dat32 & (ulong)(0x80000000 >> bitnum)) ? 1:0);
		}
		else {
			printf("P%c%d = %d\n",portName,bitnum,
			    (*dat16 & (ushort)(0x8000000 >> bitnum)) ? 1:0);
		}
	}
	return(0);
}

void
pioshow(char port)
{
	int i;
	ulong	mask;

	if (port == 'A' || port == ALL_PIO) {
		printf("PORTA bits 0-15...\n");
		printf("PAPAR: 0x%04x, PADIR: 0x%04x, PADAT: 0x%04x\n",
		    IMMRBase->pio_papar, IMMRBase->pio_padir, IMMRBase->pio_padat);
		for (mask=0x8000,i=0;i<16;i++,mask >>= 1) {
			printf("  PA%02d: %s %s %d\n",i,
			    (IMMRBase->pio_papar & mask) ? "dedicated" : "gpio",
			    (IMMRBase->pio_padir & mask) ? "output" : "input",
			    (IMMRBase->pio_padat & mask) ? 1 : 0);
		}
	}

	if (port == 'B' || port == ALL_PIO) {
		printf("\nPORTB bits 14-31...\n");
		printf("PBPAR: 0x%08x, PBDIR: 0x%08x, PBDAT: 0x%08x\n",
		    IMMRBase->pip_pbpar, IMMRBase->pip_pbdir, IMMRBase->pip_pbdat);
		for (mask=0x80000000>>14,i=14;i<32;i++,mask >>= 1) {
			printf("  PB%02d: %s %s %d\n",i,
			    (IMMRBase->pip_pbpar & mask) ? "dedicated" : "gpio",
			    (IMMRBase->pip_pbdir & mask) ? "output" : "input",
			    (IMMRBase->pip_pbdat & mask) ? 1 : 0);
		}
	}

	if (port == 'C' || port == ALL_PIO) {
		printf("PORTC bits 4-15...\n");
		printf("PCPAR: 0x%04x, PCDIR: 0x%04x, PCDAT: 0x%04x\n",
		    IMMRBase->pio_pcpar, IMMRBase->pio_pcdir, IMMRBase->pio_pcdat);
		for (mask=0x8000>>4,i=4;i<16;i++,mask >>= 1) {
			printf("  PC%02d: %s %s %d\n",i,
			    (IMMRBase->pio_pcpar & mask) ? "dedicated" : "gpio",
			    (IMMRBase->pio_pcdir & mask) ? "output" : "input",
			    (IMMRBase->pio_pcdat & mask) ? 1 : 0);
		}
	}

	if (port == 'D' || port == ALL_PIO) {
		printf("PORTD bits 3-15...\n");
		printf("PDPAR: 0x%04x, PDDIR: 0x%04x, PDDAT: 0x%04x\n",
		    IMMRBase->pio_pdpar, IMMRBase->pio_pddir, IMMRBase->pio_pddat);
		for (mask=0x8000>>3,i=3;i<16;i++,mask >>= 1) {
			printf("  PD%02d: %s %s %d\n",i,
			    (IMMRBase->pio_pdpar & mask) ? "dedicated" : "gpio",
			    (IMMRBase->pio_pddir & mask) ? "output" : "input",
			    (IMMRBase->pio_pddat & mask) ? 1 : 0);
		}
	}
}
#endif

int
pioget(char port,int bitnum)
{
	ulong	mask;

	mask = 0x8000 >> bitnum;
	switch(tolower(port)) {
		case 'a':
			return((IMMRBase->pio_padat & (ushort)mask) ? 1:0);
		case 'b':
			mask = 0x80000000 >> bitnum;
			return((IMMRBase->pip_pbdat & (ulong)mask) ? 1:0);
		case 'c':
			return((IMMRBase->pio_pcdat & (ushort)mask) ? 1:0);
		case 'd':
			return((IMMRBase->pio_pddat & (ushort)mask) ? 1:0);
	}
	return(-1);
}

void
pioclr(char port,int bitnum)
{
	ulong	mask;

	mask = 0x8000 >> bitnum;
	switch(tolower(port)) {
		case 'a':
			IMMRBase->pio_padat &= (ushort)~mask;
			break;
		case 'b':
			mask = 0x80000000 >> bitnum;
			IMMRBase->pip_pbdat &= (ulong)~mask;
			break;
		case 'c':
			IMMRBase->pio_pcdat &= (ushort)~mask;
			break;
		case 'd':
			IMMRBase->pio_pddat &= (ushort)~mask;
			break;
	}
}

void
pioset(char port,int bitnum)
{
	ulong	mask;

	mask = 0x8000 >> bitnum;
	switch(tolower(port)) {
		case 'a':
			IMMRBase->pio_padat |= (ushort)mask;
			break;
		case 'b':
			mask = 0x80000000 >> bitnum;
			IMMRBase->pip_pbdat |= (ulong)mask;
			break;
		case 'c':
			IMMRBase->pio_pcdat |= (ushort)mask;
			break;
		case 'd':
			IMMRBase->pio_pddat |= (ushort)mask;
			break;
	}
}
