struct todinfo {
	char day;		/* 1-7 (sun = 1) */
	char date;		/* 1-31 */
	char month;		/* 1-12 */
	char hour;		/* 0-23 */
	char minute;	/* 0-59 */
	char second;	/* 0-59 */
	short year;		/* full number (i.e. 2006) */
};

extern int	showDate(int center);
extern int	timeofday(int cmd, void *arg);
extern int	setTimeAndDate(struct todinfo *now);
extern int	getTimeAndDate(struct todinfo *now);

/* #defines used by the uMon API mon_timeofday()...
 */
#define TOD_ON		1
#define TOD_OFF		2
#define TOD_SET		3
#define TOD_GET		4
