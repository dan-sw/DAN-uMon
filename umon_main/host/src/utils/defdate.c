#include <stdio.h>
#include <stdlib.h>
#ifdef BUILD_WITH_VCC
#else
#include <unistd.h>
#endif
#include <time.h>
#include "utils.h"

int debug;

char *usage_txt[] = {
 "Output a line destined for a header file to contain date/time information.",
 "With no options, simply specify a definition and defdate will print out",
 "a #define string using the specified name followed by some formatted",
 "date/time string.",
 "",
 "Usage: defdate [options] {definition_name}",
 " Options:",
 "   -f {format}   override default format = \"%b_%d,%Y@%H_%M\"",
 "      %a:   abbreviated weekday name",
 "      %A:   full weekday name",
 "      %b:   abbreviated month name",
 "      %B:   full month name",
 "      %c:   date and time representation appropriate for locale",
 "      %d:   day of month as decimal number (01-31)",
 "      %H:   hour in 24-hour format (00-23)",
 "      %I:   hour in 12-hour format (01-12)",
 "      %j:   day of year as decimal number (001-366)",
 "      %m:   month as decimal number (01-12)",
 "      %M:   minute as decimal number (00-59)",
 "      %p:   current locale's AM/PM indicator for 12-hour clock",
 "      %S:   second as decimal number (00-59)",
 "      %U:   week of year as decimal number, with Sunday as first day",
 "            of week (00-51).",
 "      %w:   weekday as decimal number (0-6; Sunday=0)",
 "      %W:   week of year as decimal number, with Monday as first day ",
 "            of week (00-51).",
 "      %x:   date representation of current locale",
 "      %X:   time representation for current locale",
 "      %y:   year without centure, as decimal number (00-99)",
 "      %Y:   year with century, as decimal number",
 "      %z:   time-zone name or abbreviation; no chars if time zone is",
 "            unknown",
 "      %Z:   all-caps version of %z",
 "   -n            do not append newline to end of date string",
 "   -V            show version of defdate.exe",
 "   -w            create a 'what' string (prepend '@(#)' to output)",
 0,
};

int
main(int argc,char *argv[])
{
	char	tstring[128], *format, *newline, *what;
	int		opt;
	time_t	ltime;
	struct  tm *tmtmp, now;
	
	format = "%b_%d,%Y@%H_%M";
	newline = "\n";
	what = "";
	debug = 0;
	while((opt=getopt(argc,argv,"f:nVw")) != EOF) { 
		switch(opt) {
		case 'f':
			format = optarg;
			break;
		case 'w':
			what = "@(#)";
			break;
		case 'n':
			newline = "";
			break;
		case 'V':
			printf(" Built %s @ %s\n",__DATE__,__TIME__);
			exit(0);
		default:
			usage(0);
		}
	}

	tzset();
	time(&ltime);
	tmtmp = localtime(&ltime);
	now = *tmtmp;
	strftime(tstring,sizeof(tstring),format,&now);
	if (argc == (optind+1))
		printf("#define %s \"%s%s\"%s",argv[optind],what,tstring,newline);
	else if (argc == optind)
		printf("%s%s%s",what,tstring,newline);
	else
		usage(0);
	exit(0);
}
