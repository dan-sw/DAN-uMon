//==========================================================================
//
//      i2c.c
//
// Author(s):   Michael Kelly - Cogent Computer Systems, Inc.
// Date:        07/22/2002
// Description:	Generic bit banging I2C routines
//
//==========================================================================

#include "config.h"
#include "cpuio.h"
#include "genlib.h"
#include "stddefs.h"
#include "cpu_i2c.h"
#include "i2c.h"

//--------------------------------------------------------------------------
// The following low level I2C macros must be defined for this file
//
// examples from CSB250 cpuio.h
// #define I2C_SCL_CLR		GPIO_out(GPIO_SCL)	// puts 0 on pin
// #define I2C_SCL_SET		GPIO_in(GPIO_SCL)	// releases pin to be pulled high
// #define I2C_SDA_CLR		GPIO_out(GPIO_SDA)	// puts 0 on pin		
// #define I2C_SDA_SET		GPIO_in(GPIO_SDA)	// releases pin to be pulled high		
// #define I2C_SCL_RD		GPIO_tst(GPIO_SCL)	// return state of pin
// #define I2C_SDA_RD		GPIO_tst(GPIO_SDA)
// #define I2C_DELAY 		udelay(100)

//--------------------------------------------------------------------------
// function prototypes
//
ulong i2c_init(void);
ulong i2c_wr_device(uchar dev, uchar reg, uchar data);
ulong i2c_rd_device(uchar dev, uchar reg, uchar *data);
ulong i2c_wr_byte(uchar data);
uchar i2c_rd_byte(void);
void i2c_start(void);
void i2c_stop(void);

extern void udelay(int delay);
extern int GPIO_set(int);
extern int GPIO_clr(int);
extern int GPIO_in(int);
extern int GPIO_out(int);
extern int GPIO_tst(int);

//--------------------------------------------------------------------------
// i2c_wr_device()
//
// This function writes an 8-bit value to the I2C device at the requested
// register.
//
ulong i2c_wr_device(uchar dev, uchar reg, uchar data)
{

	// issue a start command
	i2c_start();

	// write the 7-bit device address with write = 0
	if(i2c_wr_byte((dev << 1) & 0xfe)){
		return -1;
	}
	// Write the 8-bit register address
	if(i2c_wr_byte(reg)){
		return -1;
	}
	// Write the 8-bit data value
	if(i2c_wr_byte(data)){
		return -1;
	}

	// issue a stop
	i2c_stop();
	
	return 0;
}

//--------------------------------------------------------------------------
// i2c_rd_device()
//
// This function reads an 8-bit value from the I2C device at the requested
// register.
//
ulong i2c_rd_device(uchar dev, uchar reg, uchar *data)
{

	// issue a start command
	i2c_start();

	// write the 7-bit device address with write = 0
	if(i2c_wr_byte((dev << 1) & 0xfe)){
		return -1;
	}
	// Write the 8-bit register address
	if(i2c_wr_byte(reg)){
		return -1;
	}
	// repeat the start command
	i2c_start();
	// write the 7-bit device address again plus data direction (read = 1)
	if(i2c_wr_byte((dev << 1) | 0x01)){
		return -1;
	}
	*data = i2c_rd_byte();

	// issue a stop
	i2c_stop();

	return 0;
}

//--------------------------------------------------------------------------
// i2c_wr_byte()
//
// This function writes an 8-bit value to the I2C bus, MSB first.
// Data is written by changing SDA during SCL low, then bringing
// SCL high.  SCL is returned low to setup for the next transition.
//
ulong i2c_wr_byte(uchar data)
{

	int i;

	for (i = 0; i < 8; i++){
		if (data & 0x80) {
			// write a 1 bit
			I2C_SDA_SET;
			I2C_DELAY;
			I2C_SCL_SET;
			I2C_DELAY;
			I2C_SCL_CLR;
			I2C_DELAY;
		}
		else {
			// write a 0 bit
			I2C_SDA_CLR;
			I2C_DELAY;
			I2C_SCL_SET;
			I2C_DELAY;
			I2C_SCL_CLR;
			I2C_DELAY;
		}
		data = data << 1;
	}
	// Release SDA, bring SCL high, then read SDA.
	// A low indicates an acknowledge.
	I2C_SDA_SET;
	I2C_DELAY;
   	I2C_SCL_SET;
	I2C_DELAY;
	if(I2C_SDA_RD){	// a high means no ack
		// re-enable SDA for output
	   	I2C_SCL_CLR;
		I2C_DELAY;
		return -1;
	}

   	I2C_SCL_CLR;
	I2C_DELAY;

	return 0;
}

//--------------------------------------------------------------------------
// i2c_rd_byte()
//
// This function reads an 8-bit data value from the I2C bus, MSB first. 
// Data is read from SDA after each low to high SCL transition.
//
uchar i2c_rd_byte()
{

	int i;
	uchar volatile data;

	data = 0;

	for (i = 0; i < 8; i++){
		data = data << 1;
		data = data & 0xfe;
		// clock the data out of the slave
		I2C_SCL_SET;
		I2C_DELAY;
		// check it
		if (I2C_SDA_RD){
			data = data | 0x01;
		}
		I2C_SCL_CLR;
		I2C_DELAY;
	}
	// generate an extra SCL transition
	// The slave generates no acknowledge for reads.
	I2C_SCL_SET;
	I2C_DELAY;
	I2C_SCL_CLR;
	I2C_DELAY;

	return data;
}


//--------------------------------------------------------------------------
// i2c_start()
//
// This function issues an I2C start command which is a high to low
// transition on SDA while SCL is high.
//
void i2c_start()
{

	I2C_SDA_SET;
	I2C_DELAY;
   	I2C_SCL_SET;
	I2C_DELAY;
   	I2C_SDA_CLR;
	I2C_DELAY;
   	I2C_SCL_CLR;
	I2C_DELAY;
	I2C_SDA_SET;
	I2C_DELAY;
}

//--------------------------------------------------------------------------
// i2c_stop()
//
// This function issues an I2C stop command which is a low to high
// transition on SDA while SCL is high.
//
void i2c_stop()
{
	I2C_SDA_CLR;
	I2C_DELAY;
	I2C_SCL_SET;
	I2C_DELAY;
	I2C_SDA_SET;
	I2C_DELAY;
	I2C_SCL_CLR;
	I2C_DELAY;
}

/* The following three fucntions have been added to hook into
 * the MicroMonitor's I2C command.
 */
int
i2cCtrl(int interface, int cmd, unsigned long arg1, unsigned long arg2)
{
	int	rslt;

	rslt = 0;
	switch(cmd) {
		case I2CCTRL_INIT:
			i2c_init();
			break;
		case I2CCTRL_SHOW:
			i2cShow(0);	/* target-specific */
			break;
		default:
			rslt = -1;
			break;
	}
	return(rslt);
}

/* i2cRead():
 * For general information, see notes in common/monitor/i2c.h.
 *
 * If len is greater than 1, just do a loop.  We don't support
 * burst (multiple reads within one START/STOP) here.
 */
int
i2cRead(int interface,int bigaddr,unsigned char *data,int len)
{
	int	i;
#if 0
	/* If the REPEATED_START bit is set in the address, then we
	 * do a write prior to the read.
	 */ 
	if (bigaddr & REPEATED_START) {
		i2c_start();
		if(i2c_wr_byte(*(data+1)))
			return(-1);
	}
#endif
	/* Steps for each iteration of the loop:
	 * 1. Issue start
	 * 2. Write 7-bit device address and direction bit = 1 (read)
	 * 3. Retrieve byte
	 * 4. Issue stop.
	 */
	for(i=0;i<len;i++) {
		i2c_start();

		if(i2c_wr_byte(((uchar)bigaddr << 1) | 0x01))
			return(-2);

		*data = i2c_rd_byte();

		i2c_stop();
	}

	return(len);
}

int
i2cWrite(int interface,int bigaddr,unsigned char *data,int len)
{
	int	i;

	/* Steps for each iteration of the loop:
	 * 1. Issue a start command
	 * 2. Write 7-bit device address with direction bit = 0 (write)
	 * 3. Write the 8-bit data value.
	 * 4. Issue stop.
	 */
	for(i=0;i<len;i++) {
		i2c_start();

		if(i2c_wr_byte(((uchar)bigaddr << 1) & 0xfe))
			return(-2);

		if(i2c_wr_byte(*data))
			return(-3);

		i2c_stop();
	}
	
	return(1);
}
