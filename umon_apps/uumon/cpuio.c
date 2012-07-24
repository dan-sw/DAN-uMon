#include "config.h"

#define DBGU_BASE	0xfffff200
#define DBGU_REG(x)	*(volatile unsigned long *)(DBGU_BASE + x)
#define DBGU_CSR	0x14		// Channel Status Register
#define DBGU_RHR	0x18		// Receiver Holding Register
#define DBGU_THR	0x1C		// Transmitter Holding Register
#define DBGU_INT_RXRDY	1		// RXRDY Interrupt
#define DBGU_INT_TXRDY	2		// TXRDY Interrupt

/* Do any target-specific C-level initialiation here...
 */
void
ioInit(void)
{
}

/* target-specific putchar():
 * When buffer has space available, load the incoming character
 * into the UART.
 */
int
target_putchar(char c)
{
	/* Wait for transmit ready bit
	 */
	while(1) {
		if (DBGU_REG(DBGU_CSR) & DBGU_INT_TXRDY)
			break;
	}
	DBGU_REG(DBGU_THR) = c;
	return((int)c);
}

/* target-specific gotachar():
 * Return 0 if no char is avaialable at UART rcv fifo; else 1.
 * Do NOT pull character out of fifo, just return status. 
 */
int
gotachar(void)
{
	if (!(DBGU_REG(DBGU_CSR) & DBGU_INT_RXRDY))
		return(0);
	return(1);
}

/* target-specific getchar():
 * Assume there is a character in the UART's input buffer and just
 * pull it out and return it.
 */
int 
target_getchar(void)
{
	char    c;

	/* Retrieve character */
	c = DBGU_REG(DBGU_RHR);
	return((int)c);
}
