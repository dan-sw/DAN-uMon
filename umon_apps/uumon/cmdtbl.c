/* cmdtbl.c:
 *	This is the command table used by the monitor.
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
#include "cli.h"
#include "genlib.h"

extern	int Call(int, char **);
extern	int	Dm(int, char **);
extern	int	Help(int, char **);
extern	int	FlashCmd(int, char **);
extern	int Mt(int, char **);
extern	int Pm(int, char **);
extern	int Xmodem(int, char **);
extern	int Enet(int, char **);
extern	int Tftp(int, char **);
extern	int Bootp(int, char **);

#if INCLUDE_HELP
extern	char *CallHelp[];
extern  char *DmHelp[];
extern	char *EnetHelp[];
extern	char *FlashHelp[];
extern	char *HelpHelp[];
extern	char *MtHelp[];
extern	char *PmHelp[];
extern	char *TftpHelp[];
extern	char *BootpHelp[];
extern	char *XmodemHelp[];
#else
#define CallHelp	0
#define	DmHelp		0
#define	EnetHelp	0
#define	HelpHelp	0
#define	FlashHelp	0
#define	MtHelp		0
#define	PmHelp		0
#define	TftpHelp	0
#define	BootpHelp	0
#define	XmodemHelp	0
#endif

struct monCommand cmdlist[] = {

#if INCLUDE_BOOTP
	{ "bootp",		Bootp,		BootpHelp },
#endif

#if INCLUDE_CALL
	{ "call",		Call,		CallHelp },
#endif

#if INCLUDE_DM
	{ "dm",			Dm,			DmHelp },
#endif

#if INCLUDE_ENETCORE
	{ "enet",		Enet,		EnetHelp },
#endif

#if INCLUDE_FLASH
	{ "flash",		FlashCmd,	FlashHelp },
#endif

#if INCLUDE_HELP
	{ "help",		Help,		HelpHelp },
#endif

#if INCLUDE_MT
	{ "mt",			Mt,			MtHelp },
#endif

#if INCLUDE_PM
	{ "pm",			Pm,			PmHelp },
#endif

#if INCLUDE_TFTP
	{ "tftp",		Tftp,		TftpHelp },
#endif

#if INCLUDE_XMODEM
	{ "xmodem",		Xmodem,		XmodemHelp },
#endif


	{ 0,0,0 },
};
