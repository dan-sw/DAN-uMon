#include "config.h"
#include "devices.h"
#include "stddefs.h"
#include "ether.h"
#include "uart_405.h"

void
Init405DUART(struct uart405 *uart, uchar dllsb)
{
	uchar	tmp;

	uart->linectl = DLAB;			/* set DLAB bit */
	uart->b0.dllsb = dllsb;			/* Set baud */
	uart->b1.dlmsb = 0x00;
	uart->linectl = 0x03;			/* 8-bits, no parity */
	uart->fifoctl = 0x00;			/* fifo disabled */
	uart->modemctl = 0x03;			/* DTR/RTS */
	tmp = uart->linestat;			/* clear line stat */
	tmp = uart->b0.data;			/* read receive buffer */
	uart->scratch = 0x00;			/* set scratch pad */
	uart->b1.inte = 0x00;			/* no ints used */
}

uchar
getDlLsb(int baud)
{
	uchar	dllsb;

	switch(baud) {
		case 115200:
			dllsb = BRD_115200;
			break;
		case 57600:
			dllsb = BRD_57600;
			break;
		case 38400:
			dllsb = BRD_38400;
			break;
		case 19200:
			dllsb = BRD_19200;
			break;
		default:
		case 9600:
			dllsb = BRD_9600;
			break;
	}
	return(dllsb);
}

void
setBaud(struct uart405 *uart, int baud)
{
	uchar	tmp;

	tmp = uart->linectl;				/* Save linectl reg */
	uart->linectl = DLAB;				/* set DLAB bit */
	uart->b0.dllsb = getDlLsb(baud);	/* Set baud */
	uart->b1.dlmsb = 0x00;
	uart->linectl = tmp;				/* Restore linectl reg */
}

void
Init405UART(int baud)
{
	Init405DUART(UARTA,getDlLsb(baud));
	Init405DUART(UARTB,getDlLsb(baud));
}

int
gotachar405_A()
{
	if (UARTA->linestat & DRDY)
		return(1);
	return(0);
}

int
getchar405_A()
{
	while(!gotachar405_A());
	return((int)(UARTA->b0.data));
}

int
rputchar405_A(char c)
{
	UARTA->b0.data = c;
	while(!(UARTA->linestat & THRE));
	return(0);
}

int
gotachar405_B()
{
	if (UARTB->linestat & DRDY)
		return(1);
	return(0);
}

int
getchar405_B()
{
	while(!gotachar405_B());
	return((int)(UARTB->b0.data));
}

int
rputchar405_B(char c)
{
	UARTB->b0.data = c;
	while(!(UARTB->linestat & THRE));
	return(0);
}

int
tty0init(ulong baud)
{
	extern int InitUART(int);

	InitUART(baud);
	Init405DUART(UARTA,getDlLsb(baud));
	return(0);
}

int
tty0read(char *buf,int cnt)
{
	int i;

	for(i=0;i<cnt;i++) {
		while(!gotachar405_A()) {
#if INCLUDE_ETHERNET
			pollethernet();
#endif
		}
		buf[i] = getchar405_A();
	}
	return(cnt);
}

int
tty0write(char *buf,int cnt)
{
	int i;

	for(i=0;i<cnt;i++)
		rputchar405_A(buf[i]);
	return(cnt);
}

int
tty0ctrl(int func,ulong arg0,ulong arg1)
{
	switch(func) {
		case GOTACHAR:
			return(gotachar405_A());
			break;
		case SETBAUD:
			setBaud(UARTA,(int)arg0);
			break;
		case INIT:
			Init405DUART(UARTA,getDlLsb(arg0));
			break;
		default:
			return(-1);
	}
	return(0);
}

int
tty1init(ulong baud)
{
	Init405DUART(UARTB,getDlLsb(baud));
	return(0);
}

int
tty1read(char *buf,int cnt)
{
	int i;

	for(i=0;i<cnt;i++) {
		while(!gotachar405_B()) {
#if INCLUDE_ETHERNET
			pollethernet();
#endif
		}
		buf[i] = getchar405_B();
	}
	return(cnt);
}

int
tty1write(char *buf,int cnt)
{
	int i;

	for(i=0;i<cnt;i++)
		rputchar405_B(buf[i]);
	return(cnt);
}

int
tty1ctrl(int func,ulong arg0,ulong arg1)
{
	switch(func) {
		case GOTACHAR:
			return(gotachar405_B());
			break;
		case SETBAUD:
			setBaud(UARTB,(int)arg0);
			break;
		case INIT:
			Init405DUART(UARTB,getDlLsb(arg0));
			break;
		default:
			return(-1);
	}
	return(0);
}


