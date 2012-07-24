/* backtrace.c:
 * This tool takes a list of addresses (as they would be dumped by
 * a linux crash's call backtrace) and searches for a match in a
 * sorted symbol list.
 * To use this, run...
 *
 *    nm --numeric-sort {elf-file} >nm.out
 *
 * Then, when a crash occurs like the one shown here...
 *
Oops: kernel access of bad area, sig: 11
NIP: C0013A4C XER: 00000000 LR: C0013A38 SP: C03D1E90 REGS: c03d1de0 TRAP: 0800
   Not tainted
MSR: 00001030 EE: 0 PR: 0 FP: 0 ME: 1 IR/DR: 11
DEAR: FFFFFFFC, ESR: 00000000
TASK = c03d0000[7] 'mtdblockd' Last syscall: -1
last math 00000000 last altivec 00000000
GPR00: 00001030 C03D1E90 C03D0000 00000001 00009030 00000001 C7F7C1FC FFFFFFFF
GPR08: FFFFFFFF C0170000 00000034 60000000 C0170000 100CD7B0 00000000 00000000
GPR16: 00000000 C014FB54 C03D1F58 C03D8240 C7F7C000 00000000 C03D8240 C0170000
GPR24: 00000200 00000200 C0170000 00000003 00000001 C03D8294 00000000 C03D1E90
Call backtrace:
C00C16EC C00BE13C C00B8BEC C00BB268 C00BB820 C00BB984 C0006CCC
Kernel panic: Aiee, killing interrupt handler!
In interrupt handler - not syncing
 <0>Rebooting in 180 seconds..<NULL>

 *
 * Take the address list dumped after "Call backtrace:" and pass it in as
 * arguments to this tool (after first specifying the nm.out).
 * For example, assuming the nm.out file is created and the above crash is
 * to be analyzed, the command line would be...
 *
 * backtrace nm.out C00C16EC C00BE13C C00B8BEC C00BB268 C00BB820 C00BB984 C0006CCC
 * and the output would be something like this...
 *
 c00c1714 t simple_map_copy_to
 c00be1a4 t cfi_amdstd_secsi_read
 c00b8c64 t part_point
 c00bb2e4 t mtdblock_readsect
 c00bb84c t mtd_blktrans_thread
 c00bba7c t mtd_blktrans_request
 c0006cd8 T __main
 *
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <string.h>
#ifdef BUILD_WITH_VCC
#else
#include <unistd.h>
#endif
#include "version.h"
#include "utils.h"

#define	LINESIZE 256

extern	int optind;
extern	char *optarg;

int		debug;

char *usage_txt[] = {
	"Usage: backtrace {nm-output-file} {addr} [addr] [addr] ...",
	" Parse the output of nm --numeric-sort <file>, to symbolically",
	" resolve the addresses dumped by a linux coredump backtrace output.",
	"",
	" Note, to force an inline crash, this usually works...",
	""
	" {",
	"    void     (*func)(void);",
	"    func = (void(*)(void))1;",
	"    func();",
	" }",
	"",
	" Options:",
	" -V               display version of tool",
	0,
};

int
main(int argc,char *argv[])
{
	int opt, i;
	FILE *symfp;
	char lastline[LINESIZE], line[LINESIZE];
	unsigned long addr, addr_in_func;

	while((opt=getopt(argc,argv,"V")) != EOF) {
		switch(opt) {
		case 'V':
			showVersion();
			break;
		default:
			usage(0);
		}
	}

	if (argc < optind+2)
		usage(0);

	/* Open the symbol file (built by nm): */
	if ((symfp = fopen(argv[optind],"r")) == (FILE *)NULL) {
		perror(argv[optind]);
		usage(0);
	}

	lastline[0] = 0;
	for(i=optind+1;i<argc;i++) {
		addr_in_func = strtoul(argv[i],0,16);

		while(1)
		{
			if (fgets(line,LINESIZE,symfp) == NULL)
				break;
		
			/* Assume the first whitespace delimited token is the
			 * address and as soon as the address in the file is 
			 * greater than the address on our command line, we print
			 * the previous line...
			 */
			addr = strtoul(line,0,16);
			if (addr_in_func <= addr) {
				printf(" %s",lastline);
				break;
			}
			strcpy(lastline,line);
		}
		rewind(symfp);
	}
	fclose(symfp);
	return(0);
}

