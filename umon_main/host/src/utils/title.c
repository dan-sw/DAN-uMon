#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <string.h>
#ifdef BUILD_WITH_VCC
#define WIN32_LEAN_AND_MEAN
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <io.h>
#include <winsock2.h>
#endif

#ifndef BUILD_WITH_VCC
void
SetConsoleTitle(char *title)
{
	char *term, *tp;

	term = getenv("TERM");
	if ((term == (char *)0) || (*term == 0)) {
		term = "xterm";
	}
	else {
		tp = term;
		while(*tp) {
			*tp = tolower(*tp);
			tp++;
		}
	}

	if(strcmp(term,"xterm") == 0) {
		printf("\033]2;%s\007",title);
	}
	else if(strcmp(term,"cygwin") == 0) {
		printf("\033]2;%s\007",title);
	}
	else if(strncmp(term,"vt10",4) == 0) {
		printf("\033]2;%s\007",title);
		printf("\033]1;%s\007",title);
	}
	else {
		fprintf(stderr,"title: Terminal type '%s' unrecognized\n",term);
	}
}
#endif

int
main(int argc,char *argv[])
{
	char	*title;

	if (argc == 1) {
		title = getenv("TITLE");
		if (title)
			SetConsoleTitle(title);
		else
			SetConsoleTitle("");
	}
	else if (argc == 2)
		SetConsoleTitle(argv[1]);
	else {
		fprintf(stderr,"%s [new title]\n",argv[0]);
		exit(1);
	}
	exit(0);
}
