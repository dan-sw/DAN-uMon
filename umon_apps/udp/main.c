/* 
 * Example UDP code independent of the ethernet interface.
 * This udp hookup uses uMon's ethernet API functions
 *
 *	mon_sendenetpkt(), mon_recvenetpkt()
 */

#include <string.h>
#include <stdlib.h>
#include "monlib.h"
#include "cfg.h"
#include "unet.h"
#include "cmd.h"

struct udpinfo udpInfo;
static unsigned long unetPacket[400];


/* main():
 * The equivalent of the MONCMD server built into uMon...
 * Blocks on port 777 waiting for an incoming message, then
 * responds with "thanks".
 */
int
main(int argc, char *argv[])
{
	int rc = 0;

	unetStart();
	cmdInit();
	mon_printf("%s: MONCMD server using uMon's ethernet API...\n",
		argv[0]);

	/* This loop processes incoming UDP and incoming serial port
	 * (console) activity...  Terminate on reception of ctrl-c
	 * (0x03) from console.
	 */
	while(1) {
		if (mon_gotachar()) {
			int c = mon_getchar();

			if (c == 0x03) {
				mon_printf("\n<ctrl-c>\n");
				break;
			}
			else
				processChar(c);
		}

		/* Set up the server to receive packets on any port...
		 * Then after udpRecvFrom returns, use the value of u.dport
		 * to determine which port the packet came in on.
		 */
		udpInfo.dport = ANY_UDP_PORT;
		udpInfo.packet = (char *)unetPacket;
		udpInfo.plen = sizeof(unetPacket);

		/* Check for an incoming packet.  This does not block.  It will
		 * return negative if there is an error, zero of there is no packet
		 * pending; else some positive number representing the incoming
		 * packet size.
		 */
		rc = udpRecvFrom(&udpInfo);

		if (rc > 0) {
			if (udpInfo.dport == 777) {
				udp_umoncmd((char *)udpInfo.udata);
			}
			else {
				mon_printf("\nUDP rcv'd on port %d, from %d.%d.%d.%d:%d ...\n",
					udpInfo.dport,
					IP1(udpInfo.sip.s_addr), IP2(udpInfo.sip.s_addr),
					IP3(udpInfo.sip.s_addr), IP4(udpInfo.sip.s_addr),
					udpInfo.sport);
				mon_printmem((char *)udpInfo.udata,rc,1);
			}
		}
		else if (rc < 0) {
			unetError("recv",rc,UNETACT_ALL);
		}
	}
	unetStop();
	return(0);
}

void
__gccmain()
{
}

unsigned long AppStack[APPSTACKSIZE/4];

int
Cstart(void)
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
	 */
	main(argc,argv);

	mon_appexit(0);

	/* Won't get here. */
	return(0);
}

/* CstartAlt():
 * Demonstrates the use of the "call -a" command in uMon. 
 * For example, if for some reason you wanted to do this...
 * Load the application then load the symtbl file using
 * "make TARGET_IP=1.2.3.4 sym", then issue these commands:
 *
 *  tfs -v ld app
 *  call -a %CstartAlt one two three
 *
 * The "call -a" command in uMon correctly sets up the function
 * call parameters so that the following function would see 4
 * arguments (including arg0), with argv[1] thru argv[3] being
 * pointers to each of the number strings (i.e. "one", "two", "three")
 * and argv[0] being the ascii-coded-hex address of the function
 * CstartAlt.
 */
int
CstartAlt(int argc, char *argv[])
{
	monConnect((int(*)())(*(unsigned long *)MONCOMPTR),(void *)0,(void *)0);
	main(argc,argv);
	mon_appexit(0);
	return(0);
}
