/* device structure:
 *	This structure defines the basic information needed for device
 *	control.  The table of devices is initialized at compile time
 *	with each device assigned an integer device descriptor.
 */
#ifndef _DEVICES_H_
#define _DEVICES_H_

struct device {
	int		(*init)(unsigned long);
	int		(*ctrl)(int,unsigned long,unsigned long);
	int		(*open)(unsigned long,unsigned long);
	int		(*close)(int);
	int		(*read)(char *,int);
	int		(*write)(char *,int);
	char	*name;
};

extern struct device	devtbl[];

/* Generic driver functions: */
extern	int	open(int, unsigned long, unsigned long);
extern	int	close(int);
extern	int	read(int, char *, int);
extern	int	write(int, char *, int);
extern	int	init(int, unsigned long);
extern	int	ioctl(int, int, unsigned long, unsigned long);
extern	int	devInit(int);
extern	int isbaddev(int);
extern	void devtbldump(void);

/* Common ioctl definitions: */
#define INIT		1
#define GOTACHAR	2
#define SETBAUD		3

#define nodriver	0

extern int ConsoleDevice;
#endif
