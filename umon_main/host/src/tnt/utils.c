/* This code is built based on text from the "UNIX Network Programming"
   book by Richard Stevens.  Refer to page 284.
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "utils.h"

int
readline(fd,ptr,maxlen)
int	fd, maxlen;
char	*ptr;
{
	int	n, rc;
	char	c;

	for(n=1;n<maxlen;n++) {
		if ((rc = read(fd,&c,1)) == 1) {
			*ptr = c;
			if (c == '\n')
				break;
			ptr++;
		}
		else if (rc == 0) {
			if (n == 1)
				return(0);
			else
				break;
		}
		else
			return(-1);
	}
	*ptr = 0;
	return(n);
}

int
readn(fd,ptr,nbytes)
int	fd, nbytes;
char	*ptr;
{
	int	nleft, nread;

	nleft = nbytes;
	while(nleft > 0) {
		nread = read(fd,ptr,nleft);
		if (nread < 0)
			return(nread);
		else if (nread == 0)
			break;
		nleft -= nread;
		ptr += nread;
	}
	return(nbytes - nleft);
}

int
writen(fd,ptr,nbytes)
int	fd, nbytes;
char	*ptr;
{
	int	nleft, nwritten;

	nleft = nbytes;
	while(nleft > 0) {
		nwritten = write(fd,ptr,nleft);
		if (nwritten < 0)
			return(nwritten);
		nleft -= nwritten;
		ptr += nwritten;
	}
	return(nbytes - nleft);
}

int
socprintf(int sfd, char *format, ...)
{
    int len;
    char buf[256], buf1[256], *bp, *bp1;
    va_list ap;

    va_start(ap,format);
	len = vsprintf(buf,format,ap);
	va_end(ap);
	bp = buf;
	bp1 = buf1;
	while(*bp) {
		if (*bp == '\n') {
			len++;
			*bp1++ = '\r';
		}
		*bp1++ = *bp++;
	}
	send(sfd,buf1,len,0);
	return(len);
}

void
err(char *msg)
{
	if (msg)
		fprintf(stderr,"%s, ",msg);
	perror("");
	exit(1);
}
