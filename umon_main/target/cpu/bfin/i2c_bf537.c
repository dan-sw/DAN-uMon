#include "config.h"
#include "stddefs.h"
#include "genlib.h"
#include "cpu.h"
#include "cdefBF537.h"
#include "i2c.h"

#define IMAX 1

char i2cInitialized[IMAX];

/* bfin_i2cshow() is in the port-specific directory because it is
 * port-specific.  It verbosely dumps information about each of
 * the devices hanging off the I2C bus.
 */
extern int bfin_i2cshow(int interface);

static int
bfin_i2cmodecheck(int addr)
{
	if (addr & OMIT_STOP) {
		if (i2cVerbose) {
			if (addr & OMIT_STOP)
				printf("OMIT_STOP ");
			printf("not supported by this driver yet\n"); 
		}
		return(-1);
	}
	return(0);
}

static int
bfin_i2cerrstat(int interface)
{
	int rc;
	short stat;

	/* Check status for any error conditions...
	 */
	stat = (*pTWI_MASTER_STAT & (LOSTARB|ANAK|DNAK|BUFRDERR|BUFWRERR));
	*pTWI_MASTER_STAT = stat;
	if (stat) {
		rc = -1;
		if (i2cVerbose) {
			printf("I2c errstat:%s%s%s%s%s\n",
				(stat & LOSTARB) ? " LOSTARB" : "",
				(stat & ANAK) ? " ANAK" : "",
				(stat & DNAK) ? " DNAK" : "",
				(stat & BUFRDERR) ? " BUFRDERR" : "",
				(stat & BUFWRERR) ? " BUFWRERR" : "");
		}
	}
	else
		rc = 0;
	return(rc);
}

static int
bfin_i2cmprogwait(int interface)
{
	volatile int timeout = I2C_TIMEOUT;
	
	do {
		if (bfin_i2cerrstat(interface) != 0)
			return(-1);

		if (--timeout <= 0) {
			if (i2cVerbose)
				printf("I2c bus mprog timeout\n");
			return(-1);
		}
	} while(*pTWI_MASTER_STAT & MPROG);
	return(0);
}

static int
bfin_i2cbusywait(int interface)
{
	volatile int timeout = I2C_TIMEOUT;
	
	while(*pTWI_MASTER_STAT & BUSBUSY) {
		if (--timeout <= 0) {
			if (i2cVerbose)
				printf("I2c bus busy timeout\n");
			return(-1);
		}
	}
	return(0);
}

static int
bfin_i2cinit(int interface)
{
	if (interface >= IMAX)
		return(-1);

	if (i2cInitialized[interface])
		return(0);

	/* Initialize I2C for master mode...
	 */
	*pTWI_CONTROL = (SYSCLKFREQ/10000000) | TWI_ENA;
	*pTWI_CLKDIV = (1000000000/I2C_SCL_FREQ)/100;
	*pTWI_MASTER_STAT = *pTWI_MASTER_STAT;
	*pTWI_FIFO_CTL = XMTFLUSH | RCVFLUSH;
	*pTWI_FIFO_CTL = 0;
	*pTWI_INT_MASK = 0;

	i2cInitialized[interface] = 1;
	return(0);
}

int
i2cCtrl(int interface,int cmd,unsigned long arg1,unsigned long arg2)
{
	int rc;

	if (interface >= IMAX) {
		printf("Invalid interface\n");
		return(-1);
	}

	switch(cmd) {
		case I2CCTRL_INIT:
			i2cInitialized[interface] = 0;
			rc = bfin_i2cinit(interface);
			break;
		case I2CCTRL_SHOW:
			rc = bfin_i2cshow(interface);
			break;
		default:
			printf("Invalid i2cctrl: 0x%x\n",cmd);
			rc = -1;
			break;
	}
	return(rc);
}

int
i2cRead(int interface,int bigaddr,unsigned char *data,int len)
{
	int i;

	if (i2cVerbose)
		printf("i2cRead(%d,0x%x,%d)\n",interface,bigaddr,len);

	if (bfin_i2cmodecheck(bigaddr) < 0)
		return(-1);

	if (bfin_i2cinit(interface) < 0)
		return(-1);

	if (bfin_i2cbusywait(interface) < 0)
		return(-1);

	if (bfin_i2cerrstat(interface) < 0)
		return(-1);
	
	if (bigaddr & REPEATED_START) {
		*pTWI_FIFO_CTL = XMTFLUSH | RCVFLUSH;
		asm("ssync;");
		*pTWI_FIFO_CTL = 0;
		asm("ssync;");
	}

	*pTWI_MASTER_ADDR = ((bigaddr >> 1) & 0x7f);

	if (bigaddr & REPEATED_START) {
		short xlen = data[0];

		*pTWI_INT_STAT =  0x00ff;
		asm("ssync;");
		for(i=0;i<xlen;i++) {
			while((*pTWI_FIFO_STAT & XMTSTAT) == XMT_FULL);
			*pTWI_XMT_DATA8 = data[i+1];
			asm("ssync;");
			if (i == 0) {
				*pTWI_MASTER_CTL = (((xlen << 6) & DCNT) | MEN);
				asm("ssync;");
			}
		}
		while((*pTWI_INT_STAT & (XMTSERV | MERR)) == 0);
		if (*pTWI_INT_STAT & MERR) {
			printf("XMTSERV err...\n");
			return(-1);
		}
		*pTWI_INT_STAT = XMTSERV;
		*pTWI_MASTER_CTL |= (RSTART | MEN);

		while((*pTWI_INT_STAT & (MCOMP | MERR)) == 0);
		if (*pTWI_INT_STAT & MERR) {
			printf("MCOMP0 err...\n");
			return(-1);
		}
		*pTWI_INT_STAT = MCOMP;
	}

	*pTWI_MASTER_CTL |= (((len << 6) & DCNT) | MDIR | MEN);
	asm("ssync;");

	/* While pulling the data out of the fifo, make sure that the fifo
	 * has the data.  At the last byte received, issue STOP.
	 */
	for(i=0;i<len;i++) {
		while((*pTWI_FIFO_STAT & RCVSTAT) == 0);
		if (i==len-1) {
			*pTWI_MASTER_CTL &= ~MDIR;
			*pTWI_MASTER_CTL |= RSTART;
		}
		data[i] = *pTWI_RCV_DATA8;
		asm("ssync;");
	}

	while((*pTWI_FIFO_STAT & RCVSTAT) == RCVSTAT);

	while((*pTWI_INT_STAT & (MCOMP | MERR)) == 0);
	if (*pTWI_INT_STAT & MERR) {
		printf("MCOMP1 err...\n");
		return(-1);
	}
	
	bfin_i2cbusywait(interface);

	*pTWI_INT_STAT = *pTWI_INT_STAT;
	asm("ssync;");
	*pTWI_MASTER_STAT = *pTWI_MASTER_STAT;
	asm("ssync;");
	*pTWI_MASTER_CTL = 0;
	asm("ssync;");

	return(0);
}

int
i2cWrite(int interface,int bigaddr,unsigned char *data,int len)
{
	int i;

	if (i2cVerbose) {
		if (len == 1) {
			printf("i2cWrite(%d,0x%x,0x%02x)\n",interface,bigaddr,*data);
		}
		else {
			printf("i2cWrite(%d,0x%x)\n",interface,bigaddr);
			printMem(data,len,0);
		}
	}

	if (bfin_i2cmodecheck(bigaddr) < 0)
		return(-1);

	if (bfin_i2cinit(interface) < 0)
		return(-1);

	if (bfin_i2cbusywait(interface) < 0)
		return(-1);

	if (bfin_i2cerrstat(interface) < 0)
		return(-1);
	
	*pTWI_FIFO_CTL = XMTFLUSH | RCVFLUSH;
	asm("ssync;");
	*pTWI_FIFO_CTL = 0;
	asm("ssync;");

	*pTWI_MASTER_ADDR = ((bigaddr >> 1) & 0x7f);

	/* Load data into the fifo.  Then, after the first byte is loaded,
	 * write the byte count to the master control register.  This
	 * guarantees that at least one byte will be in the FIFO ready
	 * to be transferred when the count is loaded into the control reg.
	 */
	for(i=0;i<len;i++) {
		while((*pTWI_FIFO_STAT & XMTSTAT) == XMT_FULL);
		*pTWI_XMT_DATA8 = data[i];
		if (i == 0)
			*pTWI_MASTER_CTL = (((len & 0xff) << 6) | MEN);
		asm("ssync;");
	}

	if (bfin_i2cmprogwait(interface) < 0)
		return(-1);
		
	return(0);
}
