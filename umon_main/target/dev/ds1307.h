//==========================================================================
//
//      ds1307.h
//
// Author(s):    Michael Kelly - Cogent Computer Systems, Inc.
// Date:         05-28-2002
// Description:  Defines and Dallas Semi DS1307 RTC with NVRAM
//
//
//	Modified by Technology Rescue.
//
//	Version 1.0:
//		24.11.2003 : Added RAM defs.
//==========================================================================

#include "bits.h"

//--------------------------------------------------------------------------
// I2C Address
//
#define DS1307				0x68	// I2C Address of DS1307
#define DS1307_RAM_START	0x08	// RAM start address.
#define	DS1307_RAM_SIZE		56		// Size of RAM.

//--------------------------------------------------------------------------
//
// Register Offsets
#define DS_SEC				0x00
#define DS_MIN				0x01
#define DS_HRS				0x02
#define DS_DAY				0x03
#define DS_DATE				0x04
#define DS_MONTH			0x05
#define DS_YEAR				0x06
#define DS_CTL				0x07
#define DS_DATA_START		0x08	// NVRAM
#define DS_DATA_END			0x3f

// Bit defines
#define DS_SEC_SEC(_x_)		(_x_ & 0x7f)
#define DS_SEC_CH			BIT7			// Clock Halt, 0 = Enable Clock
#define DS_MIN_MIN(_x_)		(_x_ & 0x7f)
#define DS_HRS_HRS_24(_x_)	(_x_ & 0x3f)	// Hours in 24-hour mode
#define DS_HRS_HRS_12(_x_)	(_x_ & 0x1f)	// Hours in 12-hour mode
#define DS_HRS_PM			BIT5			// AM/PM Indicator in 12 hour mode, 1 = PM
#define DS_HRS_12MODE		BIT6			// 1 = 12 hour mode
#define DS_DAY_DAY(_x_)		(_x_ & 0x07)
#define DS_DATE_DATE(_x_)	(_x_ & 0x3f)
#define DS_MONTH_MONTH(_x_)	(_x_ & 0x1f)
#define DS_YEAR_YEAR(_x_)	(_x_ & 0xff)
#define DS_CTL_RS_1HZ		0x00			// 1Hz on SQW out
#define DS_CTL_4KHZ			0x01			// 4.096Khz
#define DS_CTL_8KHZ			0x02			// 8.192Khz
#define DS_CTL_32KHZ		0x03			// 32.768Khz
#define DS_CTL_SQWE			BIT4			// 1 = enable SQW output
#define DS_CTL_OUT			BIT7			// inverted state of SQW when SQWE = 0
