#include "config.h"
#include "stddefs.h"

#define THR     0x00
#define RBR     0x00
#define DLL     0x00
#define DLH     0x04
#define IER     0x04
#define LCR     0x0c
#define MCR     0x10
#define LSR     0x14
#define SCR     0x1c
#define GCTL    0x24

#define DR		0x01    /* Data Ready */
#define THRE	0x20    /* THR Empty */

#define UART0_BASE  0xffc00400
#define UART1_BASE  0xffc02000
#define CONSOLE(reg) (UART0_BASE+reg)

void
ioInit(void)
{
	/* This is where the basic system IO would have to be
	 * initialized.  In this case, the underlying uMon is
	 * doing it, so we just blatently take it for granted 
	 * here... :-)
	 */
}

/* target-specific putchar():
 * When buffer has space available, load the incoming character
 * into the UART.
 */
int
target_putchar(char c)
{
	/* Wait for transmit ready bit */
	while(!(*(vushort *)CONSOLE(LSR) & THRE));

	*(vushort *)CONSOLE(THR) = (ushort)c;

	return((int)c);
}

/* target-specific gotachar():
 * Return 0 if no char is avaialable at UART rcv fifo; else 1.
 * Do NOT pull character out of fifo, just return status. 
 */
int
gotachar(void)
{
    if (*(vushort *)CONSOLE(LSR) & DR)
        return(1);
    return(0);	
}

/* target-specific getchar():
 * Assume there is a character in the UART's input buffer and just
 * pull it out and return it.
 */
int
target_getchar(void)
{
	return((int)*(vushort *)CONSOLE(RBR));
}
