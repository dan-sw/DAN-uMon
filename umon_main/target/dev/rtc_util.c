/* rtc_util.c

    Author: David de la Cruz, Technology Rescue.
    email:  ddlc@techresc.net

	Modification History :

	Version 1.0:
		09.05.2003 : New.
*/

#include "config.h"
#include "cpuio.h"
#include "stddefs.h"
#include "genlib.h"
#include "cli.h"
#include "rtc_util.h"

static unsigned int monthtable[] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static unsigned int monthtable_leap[] = {
	31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

//SYSTEMTIME defst = {
//	ORIGINYEAR,1,2,1,23,59,50,0
//};


static int
isleap(int year)
{
    int leap;

    leap = 0;
    if ((year % 4) == 0) {
        leap = 1;
        if ((year % 100) == 0) {
            leap = (year%400) ? 0 : 1;
        }
    }

    return leap;
}

//
// SetSystemTime
//

void SetSystemTime(SYSTEMTIME ThisTime, unsigned int *cur_time)
{
    unsigned int    min, day, month, year;
    unsigned int    *mtbl;

    day = 0;
    for (year=ORIGINYEAR; year<ThisTime.year; year++) {
        day += 365 + isleap(year);
    }
    mtbl = isleap(year) ? monthtable_leap : monthtable;
    for (month=0; month<(unsigned int)(ThisTime.month-1); month++) {
        day += mtbl[month];
    }
    day += ThisTime.day - 1;

    min = (day * 24 + ThisTime.hour) * 60 + ThisTime.minute;

    *cur_time = min * 60 + ThisTime.second;

    return;

}

//
// GetSystemTime
//
// Takes the RTC time and formats it into the
// SYSTEMTIME format.
//
void GetSystemTime(unsigned int cur_time, PSYSTEMTIME pThisTime)
{
    unsigned int    ms, sec, min, hour, day, month, year, leap;
    unsigned int    *mtbl;

    ms = 0;
    sec = cur_time % 60;
    min = (cur_time / 60);
    hour = min / 60;
    day = hour / 24;

    pThisTime->milliseconds = ms;
    pThisTime->second = sec;
    pThisTime->minute = min % 60;
    pThisTime->hour = hour % 24;

    pThisTime->dayofweek = (day + JAN1WEEK) % 7;

    year = ORIGINYEAR;
    while (1) {
        leap = isleap(year);
        if (day < 365+leap)
            break;
        day -= 365+leap;
        year++;
    }
    pThisTime->year = year;

    mtbl = leap ? monthtable_leap : monthtable;
    for (month=0; month<12; month++) {
        if (day < mtbl[month])
            break;
        day -= mtbl[month];
    }

    pThisTime->day = day+1;
    pThisTime->month = month+1;

    return;
}
