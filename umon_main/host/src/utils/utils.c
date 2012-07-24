#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef BUILD_WITH_VCC
#include <unistd.h>
#endif
#include "utils.h"

#ifdef BUILD_WITH_VCC
typedef unsigned long ulong;
typedef unsigned short ushort;
#endif
typedef unsigned char uchar;

extern int debug;

/* fileexists():
	Return1 if name is a valid file; else 0.
*/
int
fileexists(char *name)
{
	struct	stat buf;

	if (stat(name,&buf) == 0)
		return(1);
	else
		return(0);
}

/* otherEnd16() & otherEnd32():
   Conversion routines to go from one endian to the other...
*/
unsigned short
otherEnd16(int doconversion, unsigned short this_end)
{
	unsigned char	*tcp, *ocp;
	unsigned short	other_end;

	if (!doconversion)
		return(this_end);

	tcp = (uchar *)&this_end;
	ocp = (uchar *)&other_end;
	ocp[0] = tcp[1];
	ocp[1] = tcp[0];
	return(other_end);
}

unsigned long
otherEnd32(int doconversion,unsigned long this_end)
{
	unsigned char	*tcp, *ocp;
	unsigned long	other_end;

	if (!doconversion)
		return(this_end);

	tcp = (uchar *)&this_end;
	ocp = (uchar *)&other_end;
	ocp[0] = tcp[3];
	ocp[1] = tcp[2];
	ocp[2] = tcp[1];
	ocp[3] = tcp[0];
	return(other_end);
}

char *
StrAllocAndCopy(char *string)
{
	int	len;
	char	*cp;

	if (debug >= 2)
		fprintf(stderr,"  StrAllocAndCopy(%s)\n",string);

	len = strlen(string);
	len += 16;
	cp = malloc(len);
	if (!cp) {
		perror("strallocandcopy malloc");
		exit(1);
	}
	strcpy(cp,string);
	return(cp);
}

char *
Malloc(int size)
{
	char	*cp;

	if (debug >= 2)
		fprintf(stderr,"  Malloc(size=%d)\n",size);

	cp = malloc(size);
	if (!cp) {
		perror("malloc");
		exit(1);
	}
	return(cp);
}

int
Write(int fd,char *buf,int size)
{
	int ret;

	if (debug >= 2)
		fprintf(stderr,"  Write(size=%d)\n",size);

	ret = write(fd,buf,size);
	if (ret != size) {
		fprintf(stderr,"write %d failed (ret=%d)\n",size,ret);
		perror("");
		exit(1);
	}
	return(ret);
}

int
Read(int fd,char *buf,int size)
{
	int	i;

	if (debug >= 2)
		fprintf(stderr,"  Read(size=%d)\n",size);

	i = read(fd,buf,size);
	if (i < 0) {
		fprintf(stderr,"read returned %d...",i);
		perror("");
		exit(1);
	}
	return(i);
	
}

int
Lseek(int fd,int offset,int type)
{
	int	ret;

	if (debug >= 2)
		fprintf(stderr,"  Lseek(offset=%d)\n",offset);

	ret = lseek(fd,offset,type);
	if (ret < 0) {
		perror("lseek");
		exit(1);
	}
	return(ret);
}

void
usage(char *msg)
{
	extern	char *usage_txt[];
	int	i;

	if (msg)
		fprintf(stderr,"%s\n",msg);

	for(i=0;usage_txt[i];i++)
		fprintf(stderr,"%s\n",usage_txt[i]);

#include "builtwith.c"

	exit(1);
}


#ifdef BUILD_WITH_VCC

int	getopt_sp = 1;
int	optopt, optind = 1;
char	*optarg;

int
getopt(int argc,char *argv[],char *opts)
{
	extern char	*strchr();
	register int c;
	register char *cp;

	if (getopt_sp == 1) {
		char	c0, c1;

		if (optind >= argc)
			return(EOF);

		c0 = argv[optind][0];
		c1 = argv[optind][1];

		if (c0 != '-')
			return(EOF);
		if (c1 == '\0')
			return(EOF);
		if (c0 == '-') {
			if ((isdigit(c1)) && (!strchr(opts,c1)))
				return(EOF);
			if (c1 == '-') {
				optind++;
				return(EOF);
			}
		}
	}
	optopt = c = argv[optind][getopt_sp];
	if (c == ':' || ((cp=strchr(opts, c)) == NULL)) {
		fprintf(stderr,"Illegal '%s' option: %c.\n",argv[0],c);
		if(argv[optind][++(getopt_sp)] == '\0') {
			optind++;
			getopt_sp = 1;
		}
		return('?');
	}
	if (*++cp == ':') {
		if(argv[optind][getopt_sp+1] != '\0')
			optarg = &argv[optind++][getopt_sp+1];
		else if(++(optind) >= argc) {
			fprintf(stderr,
				"Option '%c' of '%s' requires argument.\n",
				c,argv[0]);
			getopt_sp = 1;
			return('?');
		} else
			optarg = argv[optind++];
		getopt_sp = 1;
	} else {
		if(argv[optind][++(getopt_sp)] == '\0') {
			getopt_sp = 1;
			optind++;
		}
		optarg = NULL;
	}
	return(c);
}

#endif
