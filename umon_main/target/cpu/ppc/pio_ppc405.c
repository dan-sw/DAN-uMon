/* pio.c:
 *	Support the basic ability to configure and set/clear parallel IO pins
 *	on the PPC405GP processor.
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
#include "stddefs.h"
#include "ctype.h"
#include "genlib.h"
#include "arch_ppc.h"


#define NULL_COMMAND			0
#define CONFIG_AS_NOTGPIO		1
#define CONFIG_AS_OD			2
#define CONFIG_AS_NOD			3
#define CONFIG_AS_TS			4
#define CONFIG_AS_NTS			5
#define WAIT_ON_BIT				6

#if INCLUDE_PIO

void
pioshow(char port_notused)
{
	printf("cntrl0      (DCR 0xb1): 0x%08lx\n",ppcMfcntrl0());
	printf("GPIO0_OR  (0xEF600700): 0x%08lx\n",*(ulong *)(GPIO0_OR));
	printf("GPIO0_TCR (0xEF600704): 0x%08lx\n",*(ulong *)(GPIO0_TCR));
	printf("GPIO0_ODR (0xEF600718): 0x%08lx\n",*(ulong *)(GPIO0_ODR));
	printf("GPIO0_IR  (0xEF60071C): 0x%08lx\n",*(ulong *)(GPIO0_IR));
}

static ulong
maskbit(int bitnum)
{
	return(1 << (31-bitnum));
}

/* isgpio():
 *	Return 1 if the specified bit is configured to be gpio;
 *	else return 0.
 */
int
isgpio(int bitnum, int verbose)
{
	if ((bitnum >= 1) && (bitnum <= 9)) {
		if ((ppcMfcntrl0() & maskbit(4)) == 0)
			return(1);
	}
	else if ((bitnum >= 5) && (bitnum <= 23)) {
		if (ppcMfcntrl0() & maskbit(bitnum-5))
			return(1);
	}
	if (verbose)
		printf("Bit is not configured as GPIO\n");
	return(0);
}

int
pioget(char port_notused,int bitnum)
{
	if (!isgpio(bitnum,0))
		return(-1);

	if (*(ulong *)(GPIO0_IR) & maskbit(bitnum))
		return(1);
	else
		return(0);
}

void
pioclr(char port_notused,int bitnum)
{
	if (!isgpio(bitnum,0))
		return;
	*(ulong *)(GPIO0_OR) = (*(ulong *)(GPIO0_OR) & ~maskbit(bitnum));
}

void
pioset(char port_notused,int bitnum)
{
	if (!isgpio(bitnum,0))
		return;
	*(ulong *)(GPIO0_OR) = (*(ulong *)(GPIO0_OR) | maskbit(bitnum));
}

char *PioHelp[] = {
	"Programmable IO Interface",
	"-[DdnTtw] [bit #] [0|1]",
	" -Dd  enable/disable open drain",
	" -n   configure as non-gpio",
	" -Tt  enable/disable tri-state",
	" -w   wait-on-bit",
	"",
	" Note: valid bit #: 0-23",
	0,
};

/* Pio():
	Display/Configure/Modify PPC405GP PIO pins.
*/

int
Pio(argc,argv)
int argc;
char *argv[];
{
	int	opt, bitnum;
	int	cmd = NULL_COMMAND;

	if (argc == 1) {
		pioshow(0);
		return(0);
	}

    while ((opt=getopt(argc,argv,"dDntTw")) != -1) {
		switch(opt) {
		    case 'd':
				cmd = CONFIG_AS_NOD;
				break;
		    case 'D':
				cmd = CONFIG_AS_OD;
				break;
			case 'n':
				cmd = CONFIG_AS_NOTGPIO;
				break;
		    case 't':
				cmd = CONFIG_AS_NTS;
				break;
		    case 'T':
				cmd = CONFIG_AS_TS;
				break;
			case 'w':
				cmd = WAIT_ON_BIT;
				break;
			default:
				return(0);
		}
	}
	bitnum = atoi(argv[optind]);
	if (cmd != NULL_COMMAND) {
		if (cmd == WAIT_ON_BIT) {
			if (!isgpio(bitnum,1))
				return(0);
			if (argv[optind+1][0] == '0')
				while(pioget(0,bitnum) == 0);
			else
				while(pioget(0,bitnum) == 1);
			return(0);
		}
		else if (argc != optind+1)
			return(-1);

		/* First, wet establish the value of bit to be put in the 
		 * chip control register to indicate if the bit is GPIO or not.
		 */
		if (cmd != CONFIG_AS_NOTGPIO) {
			if ((bitnum >= 1) && (bitnum <= 9))
				ppcMtcntrl0(ppcMfcntrl0() & ~maskbit(4));
			else
				ppcMtcntrl0(ppcMfcntrl0() | maskbit(bitnum-5));
		}
		else {
			if ((bitnum >= 1) && (bitnum <= 9))
				ppcMtcntrl0(ppcMfcntrl0() | maskbit(4));
			else
				ppcMtcntrl0(ppcMfcntrl0() & ~maskbit(bitnum-5));
		}

		if (cmd == CONFIG_AS_OD) {
			*(ulong *)(GPIO0_ODR) = (*(ulong *)(GPIO0_ODR) | maskbit(bitnum));
		}
		else if (cmd == CONFIG_AS_NOD) {
			*(ulong *)(GPIO0_ODR) = (*(ulong *)(GPIO0_ODR) & ~maskbit(bitnum));
		}
		else if (cmd == CONFIG_AS_TS) {
			*(ulong *)(GPIO0_TCR) = (*(ulong *)(GPIO0_TCR) & ~maskbit(bitnum));
		}
		else if (cmd == CONFIG_AS_NTS) {
			*(ulong *)(GPIO0_TCR) = (*(ulong *)(GPIO0_TCR) | maskbit(bitnum));
		}
	}
	else {
		if (!isgpio(bitnum,1))
			return(0);
		if (argc == optind+1) {	/* Read */
			printf("pio %d == %d\n",bitnum,pioget(0,bitnum));
			return(0);
		}
		if (argc == optind+2) {	/* Set or clear */
			if (argv[optind+1][0] == '0')
				pioclr(0,bitnum);
			else
				pioset(0,bitnum);
			return(0);
		}
		else
			return(-1);
	}
	return(0);
}

#else
int
pioget(char port_notused,int bitnum)
{
	return(-1);
}

void
pioclr(char port_notused,int bitnum)
{
}

void
pioset(char port_notused,int bitnum)
{
}
#endif
