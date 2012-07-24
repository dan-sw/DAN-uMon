/* ledit_vt.c:
 *  This is an alternative to lineedit.c.  For those unfamiliar with the
 *  KSH VI-like editing, this is certainly a more intuitive mechanism
 *  for CLI edit.  It uses the VT100 terminal arrow keys for basic 
 *  line and history traversal...
 *
 *   - UP/DOWN move through the CLI history.
 *   - RIGHT/LEFT move through the current line.
 *   - DEL deletes character that cursor is on top of.
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
#if INCLUDE_LINEEDIT
#include "ctype.h"
#include "genlib.h"
#include "stddefs.h"
#include "cli.h"

#define GOT_NUTTIN	0
#define GOT_ESCAPE	1
#define GOT_BRACKET	2

#define HMAX			16
#define ESC				0x1B
#define CTLC			0x03
#define BACKSPACE		0x08

#define OPEN_BRACKET	'['
#define VT100_DEL		0x7f
#define VT100_UP		'A'
#define VT100_DOWN		'B'
#define VT100_RIGHT		'C'
#define VT100_LEFT		'D'

#define EDITFILELINE	1
#define EDITCMDLINE		2

static int		stridx;			/* store index */
static int		shwidx;			/* show index */
static int		srchidx;		/* search index */
static int		lastsize;		/* size of last command */
static char		curChar;		/* latest input character */
static char		*curPos;		/* current position on command line */
static char		*startOfLine;	/* start of command line */
static int		lineLen;		/* length of line */
static char		cmdhistory[HMAX+1][CMDLINESIZE];/* array for command history */

static void		shownext(), showprev(), ldelete(), backspace();
static void		newchar(char c);
static void		backup(int count);

/* lineeditor():
 * This is a simpler version of lineeditor() (as found in lineedit.c).
 * It does not have the capability of the vi-like version; however, for
 * those not familiar with ksh, this one is probably a lot more intuitive.
 *
 * The line is modified in place so, if successful, the function
 * returns the same pointer but with its contents modified based
 * on the editor commands executed.
 * If failure, the function returns (char *)0.
 */
static char *
lineeditor(char *line_to_edit,int type)
{
	int	state;

	if (type == EDITCMDLINE) {
		if (getchar() != OPEN_BRACKET) {
			putchar('\n');
			*line_to_edit = 0;
			return((char *)0);
		}
	}

	startOfLine = line_to_edit;
	curPos = line_to_edit;
	while(*curPos != ESC) 
		curPos++;

	*curPos = 0;	/* Remove the escape character from the line */
	lineLen = (ulong)curPos - (ulong)startOfLine;
	if (lineLen > 0) {
		curPos--;
		putstr(" \b\b");
	}
	else
		putstr(" \b");

	state = GOT_BRACKET;
	lastsize = 0;
	shwidx = stridx;
	srchidx = stridx;
	while(1) {
		curChar = getchar();
		switch(curChar) {
			case CTLC:
				putchar('\n');
				*line_to_edit = 0;
				return((char *)0);
			case VT100_UP:
				if (state == GOT_BRACKET) {
					if (type == EDITCMDLINE) 
						showprev();
					state = GOT_NUTTIN;
				}
				else {
					newchar(curChar);
				}
				break;
			case VT100_DOWN:
				if (state == GOT_BRACKET) {
					if (type == EDITCMDLINE) 
						shownext();
					state = GOT_NUTTIN;
				}
				else {
					newchar(curChar);
				}
				break;
			case VT100_RIGHT:
				if (state == GOT_BRACKET) {
					if (curPos < startOfLine+lineLen) {
						putchar(*curPos);
						curPos++;
					}
					state = GOT_NUTTIN;
				}
				else {
					newchar(curChar);
				}
				break;
			case VT100_LEFT:
				if (state == GOT_BRACKET) {
					if (curPos > startOfLine) {
						putchar('\b');
						curPos--;
					}
					state = GOT_NUTTIN;
				}
				else {
					newchar(curChar);
				}
				break;
			case OPEN_BRACKET:
				if (state == GOT_ESCAPE) {
					state = GOT_BRACKET;
				}
				else {
					newchar(curChar);
				}
				break;
			case ESC:
				state = GOT_ESCAPE;
				break;
			case VT100_DEL:
				if (curPos != (startOfLine + lineLen))
					ldelete();
				break;
			case '\b':
				if (curPos > startOfLine)
					backspace();
				break;
			case '\n':
			case '\r':
				putchar('\n');
				if (lineLen == 0) 
					return((char *)0);
				*(char *)(startOfLine + lineLen) = '\0';
				return(startOfLine);
			default:
				newchar(curChar);
				break;
		}
	}
	return((char *)0);
}

/* line_edit() & file_line_edit():
 * These two functions are simply front-ends to the lineeditor()
 * function.  The line_edit() function is called by the command line
 * interface and file_line_edit() is called by the flash file editor
 * to provide a convenient single line editor when modifying a file.
 */

char *
line_edit(char *line_to_edit)
{
	return(lineeditor(line_to_edit,EDITCMDLINE));
}

char *
file_line_edit(char *line_to_edit)
{
	return(lineeditor(line_to_edit,EDITFILELINE));
}

static void
ldelete()
{
	char	*eol, *now;
	int	cnt;

	if (lineLen == 0)
		return;
	cnt = 0;
	eol = startOfLine + lineLen - 1;
	now = curPos;
#if 0
	if (curPos != eol) {
		while(curPos <= eol) {
			*curPos = *(curPos+1);
			curPos++;
			cnt++;
		}
		putbytes(now,cnt-1);
		putchar(' ');
		backup((int)cnt);
	}
	else {
		putstr(" \b\b");
		*eol = '\0';
		now--;
	}
#else
	while(curPos <= eol) {
		*curPos = *(curPos+1);
		curPos++;
		cnt++;
	}
	putbytes(now,cnt-1);
	putchar(' ');
	backup((int)cnt);
#endif
	curPos = now;
	lineLen--;
	if (lineLen == 0)
		curPos = startOfLine;
}

static void
backup(int count)
{
	char	string[CMDLINESIZE];
	int	i;

	if (count <= 0)
		return;
	*string = '\0';
	for(i=0;i<count;i++)
		strcat(string,"\b");
	putbytes(string,count);
}

static void
backspace()
{
	curPos--;
	putchar('\b');
   	ldelete();
}

static void
newchar(char c)
{
	char	string[CMDLINESIZE];

	if (curPos == startOfLine + lineLen) {
		putchar(c);
		*curPos++ = c;
		lineLen++;
	}
	else {
		if (!isprint(c))
			return;
		putchar(c);
		putstr(curPos);
		backup((int)strlen(curPos));
		strncpy(string,curPos,CMDLINESIZE-1);
		*curPos++ = c;
		strcpy(curPos,string);
		lineLen++;
	}
}

static void
lerase(int count)
{
	char	string[CMDLINESIZE];
	int	i;

	if (count <= 0)
		return;
	*string = '\0';
	for(i=0;i<count;i++)
		strcat(string," ");
	for(i=0;i<count;i++)
		strcat(string,"\b");
	putbytes(string,count*2);
}

/* showdone():
 * Used as common completion code for the showprev() and
 * shownext() functions below.
 */
static void
showdone(int idx)
{
	if (idx == HMAX) {
		printf("History buffer empty.\007\n");
		return;
	}

	backup((int)(curPos - startOfLine));
	lineLen = strlen(cmdhistory[shwidx]);
	putbytes(cmdhistory[shwidx],lineLen);
	lerase((int)(lastsize-lineLen));
	strcpy(startOfLine,cmdhistory[shwidx]);
	curPos = startOfLine + lineLen;
	lastsize = lineLen;
}

/* showprev() & shownext():
 * Show previous or next command in history list based on 
 * the current position in the list being established by
 * the shwidx variable.
 */
static void
showprev()
{
	int	i;

	if (shwidx == 0)
		shwidx = HMAX-1;
	else
		shwidx--;

	for(i=0;i<HMAX;i++) {
		if (*cmdhistory[shwidx])
			break;
		if (shwidx == 0)
			shwidx = HMAX-1;
		else
			shwidx--;
	}
	showdone(i);
}

static void
shownext()
{
	int	i;

	if (shwidx == HMAX-1)
		shwidx = 0;
	else
		shwidx++;

	for(i=0;i<HMAX;i++) {
		if (*cmdhistory[shwidx])
			break;
		if (shwidx == HMAX)
			shwidx = 0;
		else
			shwidx++;
	}
	showdone(i);
}

/* History():
 * Command used at the CLI to allow the user to dump the content
 * of the history buffer.
 */
char * HistoryHelp[] = {
		"Display command history",
		"",
		0,
};
	
int
History(int argc,char *argv[])
{
	int	i;

	for(i=stridx;i<HMAX;i++) {
		if (cmdhistory[i][0])
			printf("%s\n",cmdhistory[i]);
	}
	if (stridx) {
		for(i=0;i<stridx;i++) {
			if (cmdhistory[i][0])
				printf("%s\n",cmdhistory[i]);
		}
	}
	return(CMD_SUCCESS);
}

/* historyinit():
 * Initialize the command history...
 */
void
historyinit()
{
	int	i;

	shwidx = stridx = 0;
	for (i=0;i<HMAX;i++)
		cmdhistory[i][0] = 0;
}

/* historylog():
 * This function is called by the CLI retrieval code to store away
 * the command in the CLI history list, a circular queue
 * (size HMAX) of most recent commands.
 */
void
historylog(char *cmdline)
{
	int	idx;

	if (strlen(cmdline) >= CMDLINESIZE)
		return;

	if (stridx == 0)
		idx = HMAX-1;
	else
		idx = stridx -1;

	/* don't store if this command is same as last command */
	if (strcmp(cmdhistory[idx],cmdline) == 0)
		return;

	if (stridx == HMAX)
		stridx = 0;

	strcpy(cmdhistory[stridx++],cmdline);
}

#endif
