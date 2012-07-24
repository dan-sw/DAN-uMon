/* chario.c:
 *	This code supports some basic character io functionality.
 *
 *	General notice:
 *	This code is part of a boot-monitor package developed as a generic base
 *	platform for embedded system designs.  As such, it is likely to be
 *	distributed to various projects beyond the control of the original
 *	author.  Please notify the author of any enhancements made or bugs found
 *	so that all may benefit from the changes.  In addition, notification back
 *	to the author will allow the new user to pick up changes that may have
 *	been made by other users after this version of the code was distributed.
 *
 *	Note1: the majority of this code was edited with 4-space tabs.
 *	Note2: as more and more contributions are accepted, the term "author"
 *		   is becoming a mis-representation of credit.
 *
 *	Original author:	Ed Sutter
 *	Email:				esutter@lucent.com
 *	Phone:				908-582-2351
 */
#include "config.h"
#include "genlib.h"
#include "stddefs.h"

#define CTLC	0x03	/* control-c */

int
getchar(void)
{
	while(!gotachar());
	return(target_getchar());
}

int
putchar(char c)
{
	if (c == '\n')
		target_putchar('\r');
	target_putchar(c);
	return((int)c);
}

/* puts():
 *	Use putchar() to output an "assumed-to-be" null-terminated string.
*/
void
puts(char *string)
{
	while(*string) {
		putchar(*string);
		string++;
	}
}

/* getline():
 * Basic command line retrieval.
 * Args...
 *	buf:	 pointer to buffer to be used to place the incoming characters.
 *  max:	 size of the buffer.
 */
int
getline(char *buf,int max)
{
	char	*base;
	static	unsigned char  crlf;
	int		tot, idx;

	tot = idx = 0;
	base = buf;
	max -= 1;		/* Make sure there is space for the null terminator. */

	for(;idx<max;idx++) {
		*buf = (char)getchar();
		if (!*buf) {
			idx--;
			continue;
		}
		if ((*buf == '\r') || (*buf == '\n')) {
			if ((crlf) && (*buf != crlf)) {
				crlf = 0;
				continue;
			}
			puts("\r\n");
			crlf = *buf;
			*buf = 0;
			break;
		}
		if (*buf == '\b') {
			if (tot) {
				idx -= 2;
				buf--; 
				tot--;
				puts("\b \b");
			}
		}
		else if (*buf == CTLC) {
			puts("^C\n");
			*base = 0;
			return(0);
		}
		else {
			putchar(*buf);
			tot++; 
			buf++;
		}
		crlf = 0;
	}
	if (idx == max) {
		puts("\nInput too long.\n");
		*buf = 0;
		return(0);
	}
	return(strlen(base));
}

int
getbytes(char *buf, int tot)
{
	int i;

	for(i=0;i<tot;i++)
		buf[i] = getchar();
	return(tot);
}
