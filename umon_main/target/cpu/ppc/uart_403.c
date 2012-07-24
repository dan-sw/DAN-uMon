/*  uart_403.c:
 *	This file contains the PPC403 generic serial port driver.
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
#include "genlib.h"
#include "ether.h"
#include "stddefs.h"
#include "cpu_403.h"

int ConsoleBaudRate;
int	(*remoterawon)(), (*remoterawoff)();
int	(*remoteputchar)(), (*remotegetchar)(), (*remotegotachar)();

/* InitUART():
	Fed the baudrate.
*/
int
Init403UART(int baud)
{
	int divisor, brdl, brdh;

	if (baud == 0)
		baud = ConsoleBaudRate;

	/* Keep track of the last baud rate, so that it can be used if the */
	/* incoming baudrate is NULL. */
	ConsoleBaudRate = baud;

	/* Compute Baud Rate Divisor value */
	divisor = (MON_CPU_CLOCK/(16*baud)) - 1;
	brdl = divisor & 0xff;
	brdh = divisor / 0x100;

	/* Initialize 403GCX UART: */
	CPU_SPU_SPRC = 0;			/* Disable xmitter and receiver	*/
	CPU_SPU_SPTC = 0;

	CPU_SPU_SPLS = 0xf8;		/* Clear bits in line status reg */

	CPU_SPU_BRDL = brdl;		/* Set baud rate divisor lsb	*/
	CPU_SPU_BRDH = brdh;		/* Set baud rate divisor msb	*/

	CPU_SPU_SPCTL = 0x38;		/* Set DTR & RTS, 8 bits, no parity */

	CPU_SPU_SPRC = 0x80;		/* Enable receiver		*/
	CPU_SPU_SPTC = 0x80;		/* Enable transmitter		*/

	CPU_SPU_SPHS = 0xc0;		/* Clear bits in handshake reg	*/

	InitRemoteIO();

	return(0);
}

/* rputchar():
    Raw put char.
*/
int
rputchar403(char c)
{
    /* First check to see if the default rputchar() function has */
    /* been overridden... */
    if (remoteputchar) {
		remoteputchar(c);
		return((int)c);
    }

	while ((CPU_SPU_SPLS & CPU_SPLS_TBR) == 0);
   	CPU_SPU_SPTB = c;

#if INCLUDE_ETHERNET
    SendIPMonChar(c,0);
#endif

    return((int)c);
}

int
putchar403(char c)
{
	if (c == '\n')
		rputchar403('\r');
	return(rputchar403(c));
}

/* gotachar():
	Return 0 if no char is avaialable at UART rcv fifo; else 1.
	Do NOT pull character out of fifo, just return status. 
*/
int
gotachar403()
{
	if (remotegotachar)
		return(remotegotachar());

   	if ((CPU_SPU_SPLS & CPU_SPLS_RBR) != 0)
		return(1);
	return(0);
}

/* getchar():
	Block on the Uart's status until a byte is available in the 
	receive buffer, then return with it.
*/
int	
getchar403()
{
	char	c;

    /* First check to see if the default getchar() function has */
    /* been overridden... */
    if (remotegetchar)
		return(remotegetchar());
	
   	while(!gotachar403()) {
#if INCLUDE_ETHERNET
		pollethernet();
#endif
	}
	c = CPU_SPU_SPRB;

	return((int)c);
}
