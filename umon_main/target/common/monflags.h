#ifndef _MONFLAGS_H_
#define _MONFLAGS_H_

struct monflag {
	unsigned long	bit;
	char			*flagname;
};

extern int	monitorFlags;

#define	NOMONHEADER		(1 << 0)	/* Don't print header at startup */
#define NODEFRAGPRN		(1 << 1)	/* Don't print defrag message */
#define NOTFTPPRN		(1 << 2)	/* Don't print tftp message */
#define NOMONCMDPRN		(1 << 3)	/* Don't print MONCMD message */
#define NOTFTPOVW		(1 << 4)	/* Don't allow tftp srvr to overwrite */
									/* an existing file. */
#define MONCOMVERBOSE	(1 << 5)	/* The moncom() function will print */
									/* status. */
#define NOEXITSTATUS	(1 << 6)	/* Don't print app exit status. */

#define MFLAGS_NOMONHEADER()	(monitorFlags & NOMONHEADER)
#define MFLAGS_NODEFRAGPRN()	(monitorFlags & NODEFRAGPRN)
#define MFLAGS_NOTFTPPRN()		(monitorFlags & NOTFTPPRN)
#define MFLAGS_NOMONCMDPRN()	(monitorFlags & NOMONCMDPRN)
#define MFLAGS_NOTFTPOVW()		(monitorFlags & NOTFTPOVW)
#define MFLAGS_MONCOMVERBOSE()	(monitorFlags & MONCOMVERBOSE)
#define MFLAGS_NOEXITSTATUS()	(monitorFlags & NOEXITSTATUS)

extern void InitMonitorFlags(void);

#endif
