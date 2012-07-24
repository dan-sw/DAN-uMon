/* xuartlite.c:
 * This code assumes it will be included by the file cpuio.c for a
 * port that uses the UART-Lite IP from Xilinx.
 */

/* target_putchar():
 * When buffer has space available, load the incoming character
 * into the UART.
 */
int
target_putchar(char c)
{
	XUartLite_SendByte(UART_BASE, c);
	return((int)c);
}

/* target_gotachar():
 * Return 0 if no char is avaialable at UART rcv fifo; else 1.
 * Do NOT pull character out of fifo, just return status. 
 */
int
target_gotachar(void)
{
	if (XUartLite_mIsReceiveEmpty(UART_BASE))
		return(0);
	return(1);
}

/* target_getchar():
 * Assume there is a character in the UART's input buffer and just
 * pull it out and return it.
 */
int 
target_getchar(void)
{
	return((int)XUartLite_RecvByte(UART_BASE));
}

int
target_console_empty(void)
{
	if (XUartLite_mIsTransmitEmpty(UART_BASE))
		return(1);
	return(0);
}
