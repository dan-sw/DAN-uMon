/* 
 * dis_arm.c
 *
 * ARM disassembler.
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

#if INCLUDE_DISASSEMBLER

#include "genlib.h"
#include "dis_arm_instr.h"

/************************************************************************/

char dis_arm_cvsid[] = "$Id";

/************************************************************************/

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

extern long strtol();
extern char *optarg;

/**************************************************************************/

char *DisHelp[] = {
	"Disassemble memory",
	"[-m] [-t] [-r {regnaming}] {address} [linecount]",
    "{regnaming} = (raw|std|apcs|atpcs|special-atpcs)",
	0,
};

/**************************************************************************/

#ifndef stdout
#define stdout 0
#endif

int
Dis(argc, argv)
int	argc;
char *argv[];
{
	extern int optind;
	int opt, i, count, more, thumb;
	ulong *addr;
	ushort *taddr;

	more = 0;
	thumb = 0;
	disarm_set_regname_option(1);

	while ((opt = getopt(argc,argv,"mtr:")) != -1) {
		switch(opt) {
		case 'm':
			more = 1;
			break;
		case 't':
			thumb = 1;
			break;
		case 'r':
			if (!strcmp(optarg,"raw"))
				disarm_set_regname_option(0);
			else if (!strcmp(optarg,"std"))
				disarm_set_regname_option(1);
			else if (!strcmp(optarg,"apcs"))
				disarm_set_regname_option(2);
			else if (!strcmp(optarg,"atpcs"))
				disarm_set_regname_option(3);
			else if (!strcmp(optarg,"special-atpcs"))
				disarm_set_regname_option(4);
			else
				return 0;
		default:
			return(0);
		}
	}

	if (argc == optind+1)
		count = 1;
	else if (argc == optind+2)
		count = strtol(argv[optind+1],0,0);
	else
		return(-1);

	addr = (ulong *)strtoul(argv[optind],0,0);

again:
	if (! thumb) {
		/* align to word-boundary */
		addr = (ulong *)((ulong)addr & 0xfffffffc);
		for(i=0; i<count; i++) {
			printf("%08lx: %08lx ",(ulong)addr, *addr);
			disarm_print_arm_instr((ulong)addr, *addr);
			printf("\n");
			addr++;
		}
	} else {
		taddr = (ushort *)addr;
		/* align to half-word-boundary */
		taddr = (ushort *)((ulong)taddr & 0xfffffffe);
		for(i=0; i<count; i++) {
			printf("%08lx: %04x ",(ulong)taddr, *taddr);
			disarm_print_thumb_instr((ulong)taddr, (ulong)(*taddr));
			printf("\n");
			taddr++;
		}
	}

	if ( more ) {
		if ( More() )
			goto again;
	}
	
	return(0);
}

#undef stdout

/**************************************************************************/

#endif	/* INCLUDE_DISASSEMBLER */
