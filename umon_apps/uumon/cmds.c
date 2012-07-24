/* cmds.c:
 * This file contains the commands used by this version of uuMon.
 */
#include "config.h"
#include "genlib.h"
#include "stddefs.h"
#include "ctype.h"
#include "cli.h"
#include "timer.h"

#if INCLUDE_PM

/* Pm():
 *	Put to memory.
 *
 *	Arguments...
 *	arg1:		address.
 *	arg2-argN:	data to put beginning at specified address (max of 8).
 *
 *	Options...
 *	-2	access as a short.
 *	-4	access as a long.
 */

#if INCLUDE_HELP
char *PmHelp[] = {
	"Put to Memory",
	"-[24] {addr} {data}",
	"Options:",
	" -2    access width = 16bit",
	" -4    access width = 32bit",
	0,
};
#endif

int
Pm(int argc,char *argv[])
{
	ushort	val2;
	uchar	val1;
	ulong	val4, add;
	int	opt, width, i;

	width = 1;
	while((opt=getopt(argc,argv,"24")) != -1) {
		switch(opt) {
		case '2':
			width = 2;
			break;
		case '4':
			width = 4;
			break;
		default:
			return(CMD_PARAM_ERROR);
		}
	}

	if (argc < (optind+2))
		return(CMD_PARAM_ERROR);

	add = strtoul(argv[optind],(char **)0,0);

	for(i=optind+1;i<argc;i++) {
		switch(width) {
		case 1:
			val1 = (uchar)strtoul(argv[i],(char **)0,0);
			*(uchar *)add = val1;
			add++;
			break;
		case 2:
			val2 = (ushort)strtoul(argv[i],(char **)0,0);
			*(ushort *)add = val2;
			add += 2;
			break;
		case 4:
			val4 = (ulong)strtoul(argv[i],(char **)0,0);
			*(ulong *)add = val4;
			add += 4;
			break;
		}
	}
	return(CMD_SUCCESS);
}
#endif

#if INCLUDE_DM
/* Dm():
 *	Display memory.  
 *
 *	Arguments...
 *	arg1: address to start display
 *	arg2: if present, specifies the number of units to be displayed.
 *
 *	Options...
 *	-2	a unit is a short.
 *	-4	a unit is a long.
 *	
 */

#if INCLUDE_HELP
char *DmHelp[] = {
	"Display Memory",
	"-[24] {addr} {len}",
	"Options:",
	" -2    access width = 16bit",
	" -4    access width = 32bit",
	0,
};
#endif

int
Dm(int argc,char *argv[])
{
	ushort	*sp;
	uchar	*cp, cbuf[128];
	ulong	*lp, add, count_rqst;
	int		i, count, width, opt, size;

	width = 1;
	while((opt=getopt(argc,argv,"24")) != -1) {
		switch(opt) {
		case '2':
			width = 2;
			break;
		case '4':
			width = 4;
			break;
		default:
			return(CMD_PARAM_ERROR);
		}
	}

	if (argc < (optind+1))
		return(CMD_PARAM_ERROR);

	add = strtoul(argv[optind],(char **)0,0);

	if (argc-(optind-1) == 3) {
		count_rqst = strtoul(argv[optind+1],(char **)0,0);
		count_rqst *= width;
	}
	else
		count_rqst = 128;
	
	count = count_rqst;

	if (width == 1) {
		cp = (uchar *)add;

		while(count > 0) {
			printf("%08lx: ",(ulong)cp);
			if (count > 16)
				size = 16;
			else	
				size = count;

			for(i=0;i<16;i++) {
				if (i >= size)
					puts("   ");
				else  {
					cbuf[i] = *cp;
					printf("%02X ",cbuf[i]);
				}
				if (i == 7)
					puts("  ");
				cp++;
			}
			puts("  ");
			putchar('\n');
			count -= size;
			add += size;
			cp = (uchar *)add;
		}
	}
	else if (width == 2) {
		sp = (ushort *)add;

		while(count>0) {
			printf("%08lx: ",(ulong)sp);
			if (count > 16)
				size = 16;
			else	
				size = count;

			for(i=0;i<size;i+=2) {
				printf("%04X ",*sp);
				sp++;
			}
			putchar('\n');
			count -= size;
			add += size;
			sp = (ushort *)add;
		}
	}
	else if (width == 4) {
		lp = (ulong *)add;
		
		while(count>0) {
			printf("%08lx: ",(ulong)lp);
			if (count > 16)
				size = 16;
			else	
				size = count;

			for(i=0;i<size;i+=4)
				printf("%08X  ",*lp++);
			
			putchar('\n');
			count -= size;
			add += size;
			lp = (ulong *)add;
		}
	}
	
	return(CMD_SUCCESS);
}
#endif

#if INCLUDE_MT

#define ADLER_PRIME 65521

/* Mt():
 *	Memory test
 *	Walking ones and address-in-address tests for data and address bus
 *	testing respectively.  This test, within the context of a monitor
 *	command, has limited value.  It must assume that the memory space
 *	from which this code is executing is sane (obviously, or the code
 *	would not be executing) and the ram used for stack and bss must
 *	already be somewhat useable.
 */
#if INCLUDE_HELP
char *MtHelp[] = {
	"Memory test",
	"-[as:t:] {addr} {len}",
	"Options:",
	" -a    adler cksum",
	" -s##  adr-in-adr delay",
	" -t##  toggle data on 32|64-bit bound",
	"",
	0,
};
#endif

int
Mt(int argc,char *argv[])
{
	volatile ulong *addr;
	int		adler, errcnt, len, opt;
	ulong	*end, walker, readback, shouldbe;
	ulong	arg1, arg2, *start, rwsleep, togglemask;

	adler = rwsleep = 0;
	togglemask = 0;
	while((opt=getopt(argc,argv,"as:t:")) != -1) {
		switch(opt) {
		case 'a':
			adler = 1;
			break;
		case 's':
			rwsleep = strtoul(optarg,0,0);
			break;
		case 't':
			switch(atoi(optarg)) {
				case 32:
					togglemask = 0x00000004;
					break;
				case 64:
					togglemask = 0x00000008;
					break;
				default:
					return(CMD_PARAM_ERROR);
			}
			break;
		default:
			return(CMD_PARAM_ERROR);
		}
	}

	if (argc != optind+2)
		return(CMD_PARAM_ERROR);

	arg1 = strtoul(argv[optind],(char **)0,0);
	arg2 = strtoul(argv[optind+1],(char **)0,0);

	arg1 &= ~0x3;	/* Word align */
	arg2 &= ~0x3;

	if (adler) {
		unsigned long i;
		unsigned long A, B, adler;
		unsigned char* addr = (unsigned char *)arg1;

		A = 1;
		B = 0;
		for (i = 0; i < arg2; ++i) {
			A += *addr++;
			while (A >= ADLER_PRIME) A -= ADLER_PRIME;
			B += A;
			while (B >= ADLER_PRIME) B -= ADLER_PRIME;
		}
		adler = A + (B << 16);
		printf("0x%08x\n",adler);
		return(CMD_SUCCESS);
	}

	printf("Testing 0x%lx .. 0x%lx\n",arg1,arg1+arg2);

	start = (ulong *)arg1;
	len = (int)arg2;
	errcnt = 0;

	/* Walking Ones test:
	 * This test is done to verify that no data bits are shorted.
	 * Write 32 locations, each with a different bit set, then
	 * read back those 32 locations and make sure that bit (and only
	 * that bit) is set.
	 */
	walker = 1;
	end = start + 32;
	for (addr=start;addr<end;addr++) {
		*addr = walker;
		walker <<= 1;
	}

	walker = 1;
	for (addr=start;addr<end;addr++) {
		readback = *addr;
		if (readback != walker) {
			errcnt++;
			printf("WalkingOneErr @ x%lx: read x%lx expected x%lx\n",
				(ulong)addr,readback,walker);
			goto done;
		}
		walker <<= 1;
	}

	/* Address-in-address test:
	 * This test serves three purposes...
	 * 1. the "address-in-address" tests for stuck address bits.
	 * 2. the "not-address-in-address" (-t option) exercises the
	 *	  data bus.
	 * 3. the sleep between read and write tests for valid memory
	 *    refresh in SDRAM based systems.
	 *
	 * The togglemask is used to determine at what rate the data
	 * should be flipped... every 32 bits or every 64 bits.
	 */
	/* In each 32-bit address, store either the address or the
	 * complimented address...
	 */
	end = (ulong *)((int)start + len);
	for (addr=start;addr<end;addr++) {
		if (((ulong)addr & 0x3ffff) == 0) {
			if (gotachar()) 
				goto done;
		}

		if ((ulong)addr & togglemask)
			*addr = ~(ulong)addr;
		else
			*addr = (ulong)addr;
	}

	/* If -s is specified, then delay for the specified number
	 * of seconds.  This option just allows the ram to "sit"
	 * for a a while; hence, verifying automatic refresh in
	 * the case of SDRAM.
	 */
	if (rwsleep) {
		int i;
		for(i=0;i<rwsleep;i++) {
			monDelay(1000);
			if (gotachar())
				goto done;
		}
	}

	/* For each 32-bit address, verify either the address or the
	 * complimented address...
	 */
	for (addr=start;addr<end;addr++) {
		if (((ulong)addr & 0x3ffff) == 0) {
			if (gotachar())
				goto done;
		}

		readback = *addr;
		if ((ulong)addr & togglemask)
			shouldbe = ~(ulong)addr;
		else 
			shouldbe = (ulong)addr;
		if (readback != shouldbe) {
			errcnt++;
			printf("AdrInAdrErr @ x%lx: read x%lx expected x%lx\n",
				(ulong)addr,readback,shouldbe);
			break;
		}
	}

done:
	printf("%d errors.\n", errcnt);

	if (errcnt)
		return(CMD_FAILURE);
	else
		return(CMD_SUCCESS);
}
#endif

#if INCLUDE_CALL
/* Call():
 *	This function is called when the user wants to execute an 
 *	embedded function. 
 *  The the argument is preceded by an ampersand, then a pointer
 *  to the argument is passed to the function instead of a 
 *	strtol() conversion.
 */
#if INCLUDE_HELP
char *CallHelp[] = {
	"Call embedded function",
	"{address}",
	0,
};
#endif

int
Call(int argc,char *argv[])
{
	int		ret;
	int		(*func)(void);

	if ((argc < optind+1) || (argc > optind+11))
		return(CMD_PARAM_ERROR);

	func = (int(*)(void))strtol(argv[optind],(char **)0,0);

	ret = func();

	printf("Ret: %d (0x%x)\n",ret,ret);

	return(CMD_SUCCESS);
}
#endif
