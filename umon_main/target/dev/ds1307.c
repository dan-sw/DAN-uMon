//==========================================================================
//
//      ds1307.c
//
// Author(s):   Michael Kelly - Cogent Computer Systems, Inc.
// Date:        05-24-2002
// Description:	Init Code for AMI ds1307 programmable PLL.
//
//
// Modified by Technology Rescue.
//	09.05.2003 : Added system time structure.
//==========================================================================

#include "config.h"
#include "cpuio.h"
#include "stddefs.h"
#include "genlib.h"
#include "rtc_util.h"
#include "ds1307.h"
#include "cli.h"

// Masks for various portions of the time/date
#define YEAR10_MASK     0xf0
#define MONTH10_MASK    0x10
#define DATE10_MASK     0x30
#define DAY10_MASK      0x00
#define HOUR10_MASK     0x30
#define MINUTE10_MASK   0x70
#define SECOND10_MASK   0x70
#define CENTURY10_MASK  0x30
#define YEAR_MASK       0x0f    /* Year is 0-99 */
#define MONTH_MASK      0x0f    /* Month is 1-12 */
#define DATE_MASK       0x0f    /* Date is 1-31 */
#define DAY_MASK        0x07    /* Day is 1-7 */
#define HOUR_MASK       0x0f    /* Hour is 0-23 */
#define MINUTE_MASK     0x0f    /* Minutes is 0-59 */
#define SECOND_MASK     0x0f    /* Seconds is 0-59 */

//--------------------------------------------------------------------------
// function prototypes
//
int date(int argc,char *argv[]);
static int rangecheck(int value, char *name, int low, int high);
static uchar to_dsdatefmt(int value);
static int from_dsdatefmt(unsigned char value, unsigned char mask10);
int showDate(int);

extern ulong i2c_wr_device(uchar dev, uchar reg, uchar data);
extern ulong i2c_rd_device(uchar dev, uchar reg, uchar *data);

char *dateHelp[] = {
    "Display (mm/dd/yyyy@hh:mm:ss) or modify time and date.",
    "[{day date month year hour min sec}]",
    "Where...",
    " day:   1-7 (sun=1)",
    " date:  1-31",
    " month: 1-12", 
    " year:  0-3899", 
    " hour:  0-23", 
    " min:   0-59", 
    " sec:   0-59", 
    "Note: 'date off' disables the oscillator",
    0
};
int date(int argc,char *argv[])
{
	uchar temp8;
    SYSTEMTIME ThisTime;
	int rngchk;

    if (argc == 1) {
		i2c_rd_device(DS1307, DS_SEC, &temp8);
        if (temp8 & DS_SEC_CH)
            printf("Warning: oscillator disabled.\n");
        if(showDate(0)) return (CMD_FAILURE);
        return(CMD_SUCCESS);
    }
    else if ((argc == 2) && !strcmp(argv[1],"off")) {
		// disable oscillator to save battery life
        i2c_wr_device(DS1307, DS_SEC, DS_SEC_CH);
        return(CMD_SUCCESS);
    }
    else if (argc != 8)
        return(CMD_PARAM_ERROR);

    ThisTime.dayofweek = atoi(argv[1]);
    ThisTime.day = atoi(argv[2]);
    ThisTime.month = atoi(argv[3]);
    ThisTime.year = atoi(argv[4]);
    ThisTime.hour = atoi(argv[5]);
    ThisTime.minute = atoi(argv[6]);
    ThisTime.second = atoi(argv[7]);

    rngchk = 0;
    rngchk += rangecheck(ThisTime.dayofweek,"day",1,7);
    rngchk += rangecheck(ThisTime.day,"date",1,31);
    rngchk += rangecheck(ThisTime.month,"month",1,12);
    rngchk += rangecheck(ThisTime.year,"year",0,3899);
    rngchk += rangecheck(ThisTime.hour,"hour",0,23);
    rngchk += rangecheck(ThisTime.minute,"minute",0,59);
    rngchk += rangecheck(ThisTime.second,"second",0,59);

    if (rngchk != 7)
        return(CMD_PARAM_ERROR);
    
    i2c_wr_device(DS1307, DS_SEC, (to_dsdatefmt(ThisTime.second) & ~DS_SEC_CH));
    i2c_wr_device(DS1307, DS_MIN, to_dsdatefmt(ThisTime.minute));
    i2c_wr_device(DS1307, DS_HRS, to_dsdatefmt(ThisTime.hour));
    i2c_wr_device(DS1307, DS_DAY, ThisTime.dayofweek);
    i2c_wr_device(DS1307, DS_DATE, to_dsdatefmt(ThisTime.day));
    i2c_wr_device(DS1307, DS_MONTH, to_dsdatefmt(ThisTime.month));
    ThisTime.year = ThisTime.year % 100;
    i2c_wr_device(DS1307, DS_YEAR, to_dsdatefmt(ThisTime.year));

#ifdef RTCINIT
	rtc_init();
#endif

    return(CMD_SUCCESS);
}

/* rangecheck():
    Return 0 if value is outside the range of high and low; else 1.
*/
static int
rangecheck(int value, char *name, int low, int high)
{
    if ((value < low) || (value > high)) {
        printf("%s outside valid range of %d - %d\n",
            name,low,high);
        return(0);
    }
    return(1);
}

/* to_dsdatefmt():
 * Take an incoming integer and convert it to the dallas time date
 * format using a 10-multiplier for the upper nibble.
 */
static unsigned char
to_dsdatefmt(int value)
{
    int tens;

    tens = 0;
    while (value >= 10) {
        tens++;
        value -= 10;
    }
    return((tens << 4) | value);
}

static int
from_dsdatefmt(unsigned char value, unsigned char mask10)
{
    int newvalue;
    
    newvalue = value & 0x0f;
    newvalue += 10 * ((value & mask10) >> 4);
    return(newvalue);
}

int
showDate(int center)
{
	int	(*pfunc)(char *, ...);
	uchar temp8;
	SYSTEMTIME ThisTime;

    if (i2c_rd_device(DS1307, DS_SEC, &temp8)) return -1;
	ThisTime.second = from_dsdatefmt(temp8,SECOND10_MASK);

	if (i2c_rd_device(DS1307, DS_MIN, &temp8)) return -1;
    ThisTime.minute = from_dsdatefmt(temp8,MINUTE10_MASK);

	if (i2c_rd_device(DS1307, DS_HRS, &temp8)) return -1;
    ThisTime.hour = from_dsdatefmt(temp8,HOUR10_MASK);

	if (i2c_rd_device(DS1307, DS_DAY, &temp8)) return -1;
    ThisTime.dayofweek = from_dsdatefmt(temp8,DAY10_MASK);

	if (i2c_rd_device(DS1307, DS_DATE, &temp8)) return -1;
    ThisTime.day = from_dsdatefmt(temp8,DATE10_MASK);

	if (i2c_rd_device(DS1307, DS_MONTH, &temp8)) return -1;
    ThisTime.month = from_dsdatefmt(temp8,MONTH10_MASK);

	if (i2c_rd_device(DS1307, DS_YEAR, &temp8)) return -1;
    ThisTime.year = ((((temp8 & 0xf0) >> 4) * 10) + (temp8 & 0x0f)) + ORIGINYEAR;

	if (center)
		pfunc = cprintf;
	else
		pfunc = printf;

    pfunc("%02ld/%02ld/%02ld @ %02ld:%02ld:%02ld\n",
        ThisTime.month,ThisTime.day,ThisTime.year,
		ThisTime.hour,ThisTime.minute,ThisTime.second);

	return(0);
}

int readtime(PSYSTEMTIME pThisTime)
{
	uchar temp8;

	if (i2c_rd_device(DS1307, DS_SEC, &temp8)) return -1;
	pThisTime->second = from_dsdatefmt(temp8,SECOND10_MASK);

	if (i2c_rd_device(DS1307, DS_MIN, &temp8)) return -1;
    pThisTime->minute = from_dsdatefmt(temp8,MINUTE10_MASK);

	if (i2c_rd_device(DS1307, DS_HRS, &temp8)) return -1;
    pThisTime->hour = from_dsdatefmt(temp8,HOUR10_MASK);

	if (i2c_rd_device(DS1307, DS_DAY, &temp8)) return -1;
    pThisTime->dayofweek = from_dsdatefmt(temp8,DAY10_MASK);

	if (i2c_rd_device(DS1307, DS_DATE, &temp8)) return -1;
    pThisTime->day = from_dsdatefmt(temp8,DATE10_MASK);

	if (i2c_rd_device(DS1307, DS_MONTH, &temp8)) return -1;
    pThisTime->month = from_dsdatefmt(temp8,MONTH10_MASK);

	if (i2c_rd_device(DS1307, DS_YEAR, &temp8)) return -1;
    pThisTime->year = ((((temp8 & 0xf0) >> 4) * 10) + (temp8 & 0x0f)) + ORIGINYEAR;

	
	return(CMD_SUCCESS);

}
