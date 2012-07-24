//==========================================================================
//
//      fs6377.c
//
// Author(s):   Michael Kelly - Cogent Computer Systems, Inc.
// Date:        05-24-2002
// Description:	Init Code for AMI FS6377 programmable PLL.
//
//==========================================================================

#include "config.h"
#include "cpuio.h"
#include "stddefs.h"
#include "genlib.h"
#include "fs6377.h"


//#define FS6377_DBG

//--------------------------------------------------------------------------
// function prototypes
//
int fs6377_init(int);

extern void udelay(int delay);
extern ulong i2c_wr_device(uchar dev, uchar reg, uchar data);
extern ulong i2c_rd_device(uchar dev, uchar reg, uchar *data);

//--------------------------------------------------------------------------
// fs6377_init
//
// This function copies the data from fs6377_data_xxx into the fs6377
// using low level i2c primitives.  This code assumes the i2c hardware
// has been initialized already.
//
int fs6377_init(int mode)
{
	
	int i;
	
	// copy the data into the fs6377
	for (i = 0; i < 16; i++){

		// configure based on mode
		if (mode){
			if(i2c_wr_device(FS6377_I2C_ADD, i, fs6377_data_a[i])) return -1;
		}
		else {
			if(i2c_wr_device(FS6377_I2C_ADD, i, fs6377_data_b[i])) return -1;
		}
	}

#ifdef FS6377_DBG
	// readback the data
	for (i = 0; i < 16; i++){
		if(i2c_rd_device(FS6377_I2C_ADD, i, &temp8)) return -1;
		else printf("FS6377 Readback at Reg %d, Wrote 0x%02x, Read 0x%02x.\n", i, fs6377_data_lcd[i], temp8);
	}
#endif

	// wait for FS6377 to settle to new values before we use them
	monDelay(100);

	return 0;
}
