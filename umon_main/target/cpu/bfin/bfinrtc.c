#include "config.h"
#include "genlib.h"
#include "cdefBF537.h"
#include "date.h"

char
DaysInMonth[] = {
    31, 28, 31, 30, 31, 30,     /* Jan, Feb, Mar, Apr, May, Jun */
    31, 31, 30, 31, 30, 31		/* Jul, Aug, Sep, Oct, Nov, Dec */
};

int
YearIsLeap(int year)
{
    if (year % 4)		/* if year not divisible by 4... */
        return(0);		/* it's not leap */
    if (year < 1582)	/* all years divisible by 4 were */
        return(1);		/* leap prior to 1582            */
    if (year % 100)		/* if year divisible by 4, */
        return(1);		/* but not by 100, it's leap */
    if (year % 400)		/* if year divisible by 100, */
        return(0);		/* but not by 400, it's not leap */
    else
        return(1);		/* if divisible by 400, it's leap */
}

int
getDaysInMonth(int month, int year)
{
	if ((month == 2) && YearIsLeap(year))
		return(29);
	else
		return(DaysInMonth[month-1]);
}

int
getDaysInYear(int year)
{
	if (YearIsLeap(year))
		return(366);
	return(365);
}

/* getDMY():
 * Take the incoming number of days and figure out what day/month/year
 * value it is based on dayzero being Jan1,2000...
 */
int
getDMY(int days, int *day, int *month, int *year)
{
	int diy, dim;
	int tmpyear, tmpmonth;

	tmpyear = 2000;
	tmpmonth = 1;

	do {
		diy = getDaysInYear(tmpyear);
		if (days > diy) {
			days -= diy;
			tmpyear++;
		}
	} while(days > diy);


	/* At this point we've got the year, and days has been reduced to
	 * some number of days within one year..
	 */
	dim = getDaysInMonth(tmpmonth,tmpyear);
	do {
		if (days > dim) {
			days -= dim;
			tmpmonth++;
		}
	} while(days > (dim = getDaysInMonth(tmpmonth,tmpyear)));

	/* At this point we've got the month, and days has been reduced to
	 * some number of days within one month..
	 */
	*year = tmpyear;
	*month = tmpmonth;
	*day = days;
	return(0);
}

/* getDAYS():
 *
 * Given:
 *	Day of month (dom), month of year, and year;
 *
 * Return:
 *  Total number of days;
 */
int
getDAYS(int *days, int day, int month, int year)
{
	int i, totdays = 0;
	int tmpyear = 2000;

	while(tmpyear < year) {
		totdays += getDaysInYear(tmpyear++);
	}
	for(i=1;i<month;i++)
		totdays += getDaysInMonth(i,year);

	totdays += day;
	*days = totdays;
	return(0);
}

int
setTimeAndDate(struct todinfo *now)
{
	unsigned long rtc;
	int days;

	/* We can assume that basic range checking has already been done.
	 * We just make sure that the incoming year is >= 2000 because that
	 * is how we store it for this device...
	 */
	if (now->year < 2000)
		return(-1);

	rtc = now->second;
	rtc |= (now->minute << 6);
	rtc |= (now->hour << 12);
	getDAYS(&days,now->date,now->month,now->year);
	rtc |= (days << 17);

//	printf("days=%d\n",days);
//	printf("Settting RTC to: 0x%08lx\n",rtc);

	*pRTC_STAT = rtc;
	while(*pRTC_ISTAT & WRITE_PENDING);
	return(0);
}

int
getTimeAndDate(struct todinfo *now)
{
	int days, date, month, year;
	unsigned long rtc;

	if (now == 0) {
		return(-1);
	}

	rtc = *pRTC_STAT;

//	printf("RTC: 0x%08lx\n",rtc);

	memset((char *)now,0,sizeof(struct todinfo));
	now->second = rtc & RTC_SEC;
	now->minute = (rtc & RTC_MIN) >> 6;
	now->hour = (rtc & RTC_HR) >> 12;
	days = (rtc & RTC_DAY) >> 17;
	
	getDMY(days,&date,&month,&year);

//	printf("%d days: date=%d,month=%d,year=%d\n",days,date,month,year);

	now->date = date;
	now->month = month;
	now->year = year;

	return(0);
}
