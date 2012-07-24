 /*
 * umonshell.c:
 * This is a replacement for the original shell.c that comes with uIP.
 * It provides the hooks needed to allow uIP's telnet daemon to connect
 * to uMon's command set.
 *
 * Created:
 *  April 9, 2008	(Ed Sutter)
 *
 * Modifications:
 *
 */

#include "shell.h"
#include "monlib.h"
#include "uip.h"
#include "uipopt.h"

#include <string.h>

#define SHELL_PROMPT "uMON/uIP> "

static int lidx;
static char line[TELNETD_CONF_LINELEN];

/* uipshell_putchar():
 * uMon supports the ability to replace the putchar/getchar/gotachar
 * functions with similar functions that run in the application's space.
 * This is done with the call to mon_com() below.
 * With the replacement in place, uMon's console interaction is now
 * done through uIP's telnet port...
 *
 * NOTE:
 * I've studied this slightly, and it appears that it isn't possible
 * (at least not conveniently possible) to support GOTACHAR and GETCHAR
 * with mon_com().  As a result, this shell does not support any 
 * interactive commands that would be part of uMon.  If you need that to
 * work, then please fix it for me!!  :-)
 */
int
uipshell_putchar(int c)
{
	if (c == '\r')
		return(c);

	if (c == '\n') {
		line[lidx] = 0;
		shell_output(line,"");
		lidx = 0;
		return(c);
	}
	line[lidx] = c;
	if (++lidx >= sizeof(line)-1) {
		shell_output(line,"");
		lidx = 0;
	}
	return(c);
}

/* uipshell_gotachar() & uipshell_getchar():
 * These two functions would only be called if a command within uMon
 * wanted to interact with the user (for example 'edit').  
 * Unfortunately, because of the simplicity (i.e. beauty) of uIP, I
 * don't think I can support this.  The function shell_input() (below)
 * is called by the telnetd code when a command line has been received.
 * Then mon_docommand() is called, and it would be while inside
 * mon_docommand() that these functions would be called. 
 * To make them work properly, they would have to call telnetd_appcall()
 * (somehow) to see if any new data has been received.
 * I've attempted this, but can't get it to work, without digging deep into
 * telnetd (something I don't care to do). 
 * So the funtions...
 *
    mon_com(CHARFUNC_GOTACHAR,uipshell_gotachar,0,0);
    mon_com(CHARFUNC_GETCHAR,uipshell_getchar,0,0);
 *
 * don't do their job.
 * So, for now, we just don't support any interactive commands through
 * this telnet interface.
 *
int
uipshell_gotachar(void)
{
}

int
uipshell_getchar(void)
{
	return(0);
}
 */

void
shell_init(void)
{
}

/*---------------------------------------------------------------------------*/
void
shell_start(void)
{
  lidx = 0;
  mon_printf("Telnet client attached\n");
  shell_output("uMON with uIP 1.0 command shell", "");
  shell_prompt(SHELL_PROMPT);

}

/*---------------------------------------------------------------------------*/
void
shell_input(char *cmd)
{
  if (strcmp(cmd,"exit") == 0) {
	shell_quit(0);
    mon_printf("Telnet client exited\n");
	return;
  }
  if ((*cmd != '\r') && (*cmd != '\n')) {
    /* Wrap mon_docommand() with code that will redirect uMon's standard
     * output to this telnet connection...
     */
    mon_com(CHARFUNC_PUTCHAR,uipshell_putchar,0,0);
//  mon_com(CHARFUNC_GETCHAR,uipshell_getchar,0,0);
//  mon_com(CHARFUNC_GOTACHAR,uipshell_gotachar,0,0);
    mon_docommand(cmd,0);
    mon_com(CHARFUNC_PUTCHAR,0,0,0);
//  mon_com(CHARFUNC_GETCHAR,0,0,0);
//  mon_com(CHARFUNC_GOTACHAR,0,0,0);
  }
  shell_prompt(SHELL_PROMPT);
}
/*---------------------------------------------------------------------------*/
