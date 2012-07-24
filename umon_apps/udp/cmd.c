#include <string.h>
#include <stdlib.h>
#include "monlib.h"
#include "cfg.h"
#include "unet.h"
#include "cmd.h"
#include "cli.h"

extern struct udpinfo udpInfo;

static char *udprp, udpresp[256];

int
sendResponseLine(struct udpinfo *up, char *line, int len)
{
	int rc;
	struct udpinfo u1;
	static unsigned long pkt[64];

	u1.dport = up->sport;
	u1.dip.s_addr = up->sip.s_addr;
	u1.packet = (char *)pkt;
	u1.plen = sizeof(pkt);
	u1.udata = line;
	u1.ulen = len;
	if ((rc = udpSendTo(&u1)) < 0)
		unetError("sendResponseLine",rc,0);

	return(0);
}

/* udp_putchar():
 * This function is used when this application is processing an incoming
 * moncmd request.  It allows this application to temporarily redirect
 * the uMon output to this udp interface.
 */
int
udp_putchar(int c)
{
	/* First check for buffer overflow...
	 */
	if ((udprp - udpresp) >= (sizeof(udpresp)-3)) {
		udprp = udpresp;
	}
	else {
		if (c == '\n') {
			*udprp++ = '\r';
			*udprp++ = '\n';
			*udprp = 0;
			sendResponseLine(&udpInfo,udpresp,udprp-udpresp);
			udprp = udpresp;
		}
		else
			*udprp++ = c;
	}
	return((int)c);
}

/* udp_umoncmd():
 * A wrapper for the udp-based invocation of a monitor command.
 */
int
udp_umoncmd(char *cmd)
{
	int rc;

	udprp = udpresp;
	mon_com(CHARFUNC_PUTCHAR,udp_putchar,0,0);
	rc = mon_docommand((char *)cmd,0);
	mon_com(CHARFUNC_PUTCHAR,0,0,0);
	sendResponseLine(&udpInfo,"",1);
	return(rc);
}

/* processChar():
 * Used by the main polling loop to deal with incoming characters
 * on the console.
 */
void
processChar(int c)
{
	static char cmd[64];
	static int cmdidx = 0;

	if (c == '\b') {
		if (cmdidx > 0) {
			cmdidx--;
			mon_printf("\b \b");
		}
	}
	else if ((c == '\n') || (c == '\r')) {
		if (cmdidx != 0) {
			cmd[cmdidx] = 0;
			mon_printf("\r\n");
			mon_docommand(cmd,0);
			cmdidx = 0;
		}
		mon_printf("\r\nPROMPT:");
	}
	else {
		if (cmdidx < (sizeof(cmd)-1)) {
			cmd[cmdidx++] = (char)c;
			mon_putchar(c);
		}
		else {
			mon_printf("\ncmd buffer overflow\n");
			cmdidx = 0;
		}
	}
}

/***********************************************************
 * 
 * The remaining code in this file is dedicated to establishing a
 * set of application-specific commands to be inserted into the
 * monitor's command processor.
 * This code just sets up two dummy commands (cmd1 & cmd2) to
 * show how its done.
 */
char *help_CMD1[] = {
	"Do command #1",
	"{cmd_1 args}",
	0
};

int
cmd_CMD1(int argc, char *argv[])
{
	int i;

	mon_printf("CMD1:\n");
	for(i=0;i<argc;i++)
		mon_printf("arg[%d]: <%s>\n",i,argv[i]);

	return(CMD_SUCCESS);
}

char *help_CMD2[] = {
	"Do command #2",
	"{cmd_2 args}",
	0
};

int
cmd_CMD2(int argc, char *argv[])
{
	int i;

	mon_printf("CMD2:\n");
	for(i=0;i<argc;i++)
		mon_printf("arg[%d]: <%s>\n",i,argv[i]);

	return(CMD_SUCCESS);
}


struct monCommand cmdtbl[] = {
    { "cmd1",		cmd_CMD1,		help_CMD1,	0 },
    { "cmd2",		cmd_CMD2,		help_CMD2,	0 },
    { 0,0,0,0}
};

#define CMDTOT (sizeof(cmdtbl)/sizeof(struct monCommand))

char cmdlvls[CMDTOT];

void
cmdInit(void)
{
	int i;

	for(i=0;i<CMDTOT;i++)
		cmdlvls[i] = 0;

	mon_addcommand(cmdtbl,cmdlvls);
}
