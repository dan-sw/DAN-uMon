#include "monlib.h"
#include "string.h"

static int lidx;
static char line[128];

void
console_prompt(void)
{
	mon_printf("APP:");
}

void
console_init()
{
	lidx = 0;
	console_prompt();
}

/* etherCheck():
 * Used by the console here to warn the user that they are invoking a 
 * uMon command that touches ethernet (which is probably not a good
 * idea, since this app is ethernet based).
 */
int
etherCheck(char *line)
{
	if ((strncmp(line,"ether ",6) == 0) ||
	    (strncmp(line,"arp ",4) == 0) ||
	    (strncmp(line,"dhcp ",5) == 0) ||
	    (strncmp(line,"icmp ",5) == 0) ||
	    (strncmp(line,"tftp ",5) == 0)) {
		mon_printf("WARNING: this is an 'ethernet-touching' command.\n");
		mon_printf("You sure you wanna do this (y or n)?\n");
		if (mon_getchar() == 'y')
			return(0);
		else
			return(1);
	}
	return(0);
}

int
console_poll(void)
{
	char c;

	if (!mon_gotachar())
		return(0);

	c = mon_getchar();

	if (c == '\b') {
		if (lidx > 0) {
			mon_printf("\b \b");
			lidx--;
		}
		return(0);
	}
	if ((lidx == (sizeof(line)-1)) || (c == '\r') || (c == '\n')) {
		mon_printf("\r\n");
		if (lidx == 0) {
			console_prompt();
			return(0);
		}
		line[lidx] = 0;
		if (strcmp(line,"exit") == 0)
			return(-1);

		if (etherCheck(line) == 0)
			mon_docommand(line,0);
		console_prompt();
		lidx = 0;
	}
	else {
		mon_putchar(c);
		if (lidx < (sizeof(line)-2))
			line[lidx++] = c;
	}
	return(0);
}
