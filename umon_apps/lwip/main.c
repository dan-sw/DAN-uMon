/* 
 * main.c:
 * This is the code that initializes LWIP and hooks it to uMon's
 * packet interface.  This code is *almost* entirely target
 * independent.  The only code that is cpu/target specific is
 * the code that retrieves the time base.
 */

#include <string.h>
#include <stdlib.h>
#include "monlib.h"
#include "cfg.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "netif/etharp.h"
#include "lwip/init.h"
#include "lwip/ip.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "lwip/inet.h"
#include "lwip/tcpip.h"
#include "lwip/netif.h"
#include "lwip/stats.h"
#include "httpd.h"
#include "telnet.h"
#include "monsrvr.h"
#include "umonlwip.h"

#if USE_APPSTACK
unsigned long AppStack[APPSTACKSIZE/4];
#endif

extern int console_poll(void);
extern int console_init(void);

void
usage(char *arg0)
{
	mon_printf("Usage: %s {command} [command-specific-args]\n",arg0);
	mon_printf(" Commands:\n");
	mon_printf("  * srvrs\n");
	mon_printf("    run HTTP & MONCMD servers\n");
	mon_printf("  * telnetc {srvrip} [port]\n");
	mon_printf("    run TELNET client to specified server ip\n");
	mon_printf("  * httpget {srvrip} {filename} {dest_addr}\n");
	mon_printf("    run HTTP-GET requet to specified server ip and file;\n");
	mon_printf("    place data at 'dest_addr'\n");
	mon_printf("    load HTTPGET shellvar with size of transfer\n");
	mon_printf("\n\n");
}

int
main(int argc,char *argv[])
{
	short port;
	char *srvr, *fname, *addr;

	if (argc < 2) {
		usage(argv[0]);
		return(-1);
	}
	if (strcmp(argv[1],"httpget") == 0) {
		if (argc != 5) {
			usage(argv[0]);
			return(-1);
		}
		
		srvr = argv[2];
		fname = argv[3];
		addr = (char *)strtoul(argv[4],0,0);

		mon_printf("Http get: %s %s 0x%lx\n",srvr,fname,addr);
		lwip_umon_startup();
		httpget_init(srvr,fname,addr);

		while(1) {
			if (mon_gotachar()) {
				mon_printf("HttpGet terminted by user\n");
				httpget_close();
				break;
			}
			if (!httpget_isactive()) {
				mon_printf("HttpGet completed\n");
				break;
			}
			lwip_poll();
		}
		lwip_umon_shutdown();
	}
	else if (strcmp(argv[1],"telnetc") == 0) {
		if (argc == 3) {
			port = 23;
			srvr = argv[2];
		}
		else if (argc == 4) {
			srvr = argv[2];
			port = atoi(argv[3]);
		}
		else {
			usage(argv[0]);
			return(-1);
		}

		lwip_umon_startup();
		telnetc_init(srvr,port);
		while(1) {
			if (mon_gotachar())
				telnetc_putchar((char)mon_getchar());
			if (!telnetc_isactive()) {
				mon_printf("Telnetc completed\n");
				break;
			}
			lwip_poll();
		}
		lwip_umon_shutdown();
	}
	else if (strcmp(argv[1],"srvrs") == 0) {
		mon_printf("Running HTTP & MONCMD servers (hitakey to terminate)...\n");
		lwip_umon_startup();
		httpd_init();
		monsrvr_init();
		console_init();
		do {
			lwip_poll();
		} while (console_poll() != -1);
		lwip_umon_shutdown();
	}
	else {
		usage(argv[0]);
	}

	return(0);
}

void
__gccmain()
{
}

int
#if USE_APPSTACK
Cstart(void)
#else
start(void)
#endif
{
	char	**argv;
	int		argc;

	/* Connect the application to the monitor.  This must be done
	 * prior to the application making any other attempts to use the
	 * "mon_" functions provided by the monitor.
	 */
	monConnect((int(*)())(*(unsigned long *)MONCOMPTR),(void *)0,(void *)0);

	/* When the monitor starts up an application, it stores the argument
	 * list internally.  The call to mon_getargv() retrieves the arg list
	 * for use by this application...
	 */
	mon_getargv(&argc,&argv);

	/* Call main, then exit to monitor.
	 * Wrap the call with the Lwip startup/shutdown calls...
	 */
#if USE_APPSTACK
	mon_appexit(main(argc,argv));
#else
	main(argc,argv);
#endif

	/* We won't get here if USE_APPSTACK is set.
	 */
	return(0);
}

#if CPU_IS_PPC
unsigned long
clock_time(void)
{
    volatile unsigned register tbl;

    // Retrieve the current value of the TBL register:
    asm volatile ("mfspr %0, 0x10c" : "=r" (tbl));

    return((unsigned long)tbl);
}
#elif CPU_IS_MIPS
#if CLOCK_MS
#error: Gotta write clock_time for MIPS.
#endif
#elif CPU_IS_ARM
#if CLOCK_MS
#error: Gotta write clock_time for ARM.
#endif
#elif CPU_IS_BFIN
#if CLOCK_MS
/* clock_time is in assembler */
#endif
#elif CPU_IS_MICROBLAZE
#if CLOCK_MS
#error: Gotta write clock_time for MICROBLAZE.
#endif
#elif CPU_IS_68K
#if CLOCK_MS
#error: Gotta write clock_time for 68K.
#endif
#else
#error: Invalid CPU specification.
#endif
