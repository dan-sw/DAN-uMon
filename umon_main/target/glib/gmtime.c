/*
 */
#include <time.h>

#define EPOCH_YR 1970
#define HOURS_PER_DAY 24L
#define MIN_PER_HR 60L
#define SECS_PER_MIN 60L
#define SECS_PER_HR (SECS_PER_MIN * MIN_PER_HR)
#define SECS_PER_DAY (HOURS_PER_DAY * MIN_PER_HR * SECS_PER_MIN)
#define DAYS_PER_WK 7

#define LEAPYEAR(year) (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YEARSIZE(year) (LEAPYEAR(year) ? 366 : 365)


static const char days_per_mo[2][12] = {
  {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
  {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
};

struct tm *
gmtime_r(const time_t t, struct tm *ret)
{
  unsigned long dayfrac, day;
  int year = EPOCH_YR;
  
  dayfrac = t % SECS_PER_DAY;
  day = t / SECS_PER_DAY;
  
  ret->tm_sec = dayfrac % SECS_PER_MIN;
  ret->tm_min = (dayfrac % SECS_PER_HR) / SECS_PER_MIN;
  ret->tm_hour = dayfrac / SECS_PER_HR;
  ret->tm_wday = (day + 4) % DAYS_PER_WK; /* the "epoch" was a Thursday */
  while (day >= YEARSIZE(year)) {
    day -= YEARSIZE(year);
    year++;
  }

  ret->tm_year = year - EPOCH_YR;
  ret->tm_yday = day;
  ret->tm_mon = 0;
  while (day >= days_per_mo[LEAPYEAR(year)][ret->tm_mon]) {
    day -= days_per_mo[LEAPYEAR(year)][ret->tm_mon];
    ret->tm_mon++;
  }
  ret->tm_mday = day + 1;
  ret->tm_isdst = 0;
  return ret;
}
