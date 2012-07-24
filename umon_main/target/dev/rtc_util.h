/* rtc_util.h

    Author: David de la Cruz, Technology Rescue.
    email:  ddlc@techresc.net

	Modification History :

	Version 1.0:
		09.05.2003 : New.
*/



#define ORIGINYEAR  2000	/* 2000 */
#define JAN1WEEK    2       /* Tuesday */


typedef struct _SYSTEMTIME {  
	ulong year;
	ulong month;
	ulong dayofweek;
	ulong day;
	ulong hour;
	ulong minute;
	ulong second;
	ulong milliseconds;
} SYSTEMTIME, *PSYSTEMTIME;

//--------------------------------------------------------------------------
// function prototypes
//
void GetSystemTime(unsigned int cur_time, PSYSTEMTIME lpst);
void SetSystemTime(SYSTEMTIME ThisTime, unsigned int *cur_time);

void rtc_init(void);
