#define UARTA	((struct uart405 *)0xef600300)
#define UARTB	((struct uart405 *)0xef600400)

#define DLAB	0x80
#define THRE	0x20	/* Transmit Holding Register Empty */
#define DRDY	0x01	/* Data ready */

struct uart405 {
	union {
		vuchar	data;		/* DATA_REG */
		vuchar	dllsb;		/* DL_LSB (when DLAB is set in linectl reg) */
	} b0;
	union {
		vuchar	inte;		/* INT_ENABLE */
		vuchar	dlmsb;		/* DL_MSB when DLAB is set in linectl reg) */
	} b1;
	vuchar	fifoctl;		/* FIFO_CONTROL */
	vuchar	linectl;		/* LINE_CONTROL */
	vuchar	modemctl;		/* MODEM_CONTROL */
	vuchar	linestat;		/* LINE_STATUS */
	vuchar	modemstat;		/* MODEM_STATUS */
	vuchar	scratch;		/* SCRATCH */
};

extern void Init405DUART(struct uart405 *uart, uchar dllsb);
extern unsigned char getDlLsb(int baud);
extern void setBaud(struct uart405 *uart, int baud);
extern void Init405UART(int baud);
extern int gotachar405_A(void);
extern int getchar405_A(void);
extern int rputchar405_A(char c);
extern int gotachar405_B(void);
extern int getchar405_B(void);
extern int rputchar405_B(char c);
extern int tty0init(unsigned long baud);
extern int tty0read(char *buf,int cnt);
extern int tty0write(char *buf,int cnt);
extern int tty0ctrl(int func,unsigned long arg0,unsigned long arg1);
extern int tty1init(unsigned long baud);
extern int tty1read(char *buf,int cnt);
extern int tty1write(char *buf,int cnt);
extern int tty1ctrl(int func,unsigned long arg0,unsigned long arg1);
