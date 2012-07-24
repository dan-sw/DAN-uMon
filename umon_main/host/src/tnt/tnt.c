/* tnt.c:
 * Telnet 'n' tty...
 * Very basic interface to a TTY (no terminal emulation whatsoever), plus
 * a telnet server backend so that a user can telnet into the machine
 * running this program and connect to the same TTY port that this
 * program is connected to.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <termio.h>
#include <sys/stat.h>
#include <signal.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/sysmacros.h>
#include "telnetd.h"
#include "utils.h"
#include "tnt.h"

extern int optind;
extern char *optarg;

#define LOCKDIR	"/var/lock"
#define CTRL_A	0x01
#define CTRL_C	0x03

int		runasdaemon = 0;
int		remote_cmd_ok = 0;
int		ttyfd;
int		verbose = 0;
int		ignore_usrctrl = 0;
int		stdin_fixed = 0;
int		baud, readonly, notelnet;
char	*password, *welcome, *progname;
struct termio newstdin, oldstdin;

char	lockfiles[3][256];
char	*lockdir = LOCKDIR;

/**
 *	\var errmsg
 *	\brief Initialized string table of usage message.
 */
char *errmsg[] = {
	"Usage: tc [options]",
	" Options:",
	"  -b###  baud rate (default = 9600)",
	"  -d     run as daemon",
	"  -n     no telnet daemon",
//	"  -i     ignore user-control commands (ctrl-a, ctrl-x, etc..)",
	"  -h     dump this help text",
	"  -p###  port number base used by telnet client (default = 8000)",
	"  -P###  specify telnet client password (default = none)",
	"  -R     enable telnet-remote command capability",
	"  -r     telnet clients are read-only",
	"  -t###  tty devname (default = /dev/ttyS0)",
	"  -v     verbose mode",
	"  -V     show version",
	"  -w###  specify telnet client welcome message (default = none)",
	0,
};

void
terminate_message(void)
{
	fprintf(stderr,"Terminate via ctrl-A\n");
}

char *
basename(char *name)
{
	char *lastslash;

	lastslash = strrchr(name,'/');
	if (lastslash)
		return(lastslash + 1);
	else
		return(name);
}

/** \fn void usage(char *error)
 *	\brief Called if a command line error was detected.
 *	\param error Optional error message.
 */
void
usage(char *error)
{
	int	i;

	if (error)
		fprintf(stderr,"ERROR: %s\n",error);

	fprintf(stderr,"Usage: %s [options]\n",progname);

	for(i=0;errmsg[i];i++)
		fprintf(stderr,"%s\n",errmsg[i]);
	exit(1);
}

/* check_lock():
 * Try to establish the same set of lock files that are used by
 * minicom, so that tnt and minicom will be aware of each other's
 * tty usage...
 *
 * If the lock files don't exist, create them and populate each with
 * an ascii equivalent of the running process id.
 * If they do exist then open one to get the process id of the owner.
 * Send sig 0 to that process id.  If kill() returns errno ESRCH, this
 * indicates that the process doesn't exist, so the lockfile can be
 * removed.
 *
 * NOTE THAT THIS ISN'T COMPLETE.
 */
int
check_lock(char *ttyname, struct stat *ttystat)
{
	char buf[256];
	struct stat s;
	int mask, i, fd[3], n, pid, len;

	setregid(getgid(), getegid());
	setreuid(getuid(), geteuid());

	if (stat(lockdir, &s) == 0) {
		sprintf(lockfiles[0], "%s/LCK..%s", lockdir, basename(ttyname));
		sprintf(lockfiles[1], "%s/LCK...%d", lockdir, getpid());
		sprintf(lockfiles[2], "%s/LCK.%03d.%03d", lockdir,
			major(ttystat->st_rdev),minor(ttystat->st_rdev));
	}
	else {
		fprintf(stderr,"No lock dir (%s), running without tty lock.\n",lockdir);
		return(0);
	}

	if ((fd[0] = open(lockfiles[0], O_RDONLY)) >= 0) {
		n = read(fd[0], buf, 127);
		close(fd[0]);
		if (n > 0) {
			pid = -1;
			if (n == 4) /* Kermit-style lockfile. */
				pid = *(int *)buf;
			else { /* Ascii lockfile. */
				buf[n] = 0;
				sscanf(buf, "%d", &pid);
			}
			if (pid > 0 && kill(pid, 0) < 0 && errno == ESRCH) {
				fprintf(stderr, "Lockfile is stale. Overriding it..\n");
				unlink(lockfiles[0]);
				unlink(lockfiles[2]);
				sprintf(buf, "%s/LCK...%d",lockdir,pid);
				unlink(buf);
			}
			else
				n = 0;
		}
		if (n == 0) {
			fprintf(stderr, "'%s' is locked.\n", ttyname);
			return(-1);
		}
	}
	len = sprintf(buf,"%10d\n", getpid());

	mask = umask(022);
	for(i=0;i<3;i++) {
		if ((fd[i] = open(lockfiles[i],O_WRONLY|O_CREAT|O_EXCL,0666)) < 0) {
			fprintf(stderr,"Can't create lockfile %s\n",lockfiles[i]);
			return(0);
		}
	}
	umask(mask);

	for(i=0;i<3;i++) {
		chown(lockfiles[i], getuid(), getgid());
		write(fd[i], buf, len);
		close(fd[i]);
	}

	/* To get access to the /var/lock directory, this program must
	 * startup with root priveledges; however, once the lock file
	 * is established, it is chowned to the current user's id and
	 * then this program's effective userid is also changed to
	 * that user's...
	seteuid(getuid());
	 */
	return(1);
}

void *
tty_reader(void *arg)
{
	int fd;
	char c;

	fd = (int)arg;

	if (verbose)
		fprintf(stderr,"tty reader thread...\n");

	while(read(fd,&c,1) == 1) {
		if (runasdaemon == 0)
			write(1,&c,1);
		net_write(&c,1);
	}

	perror("tty_reader terminating");
	return(0);
}

void
user_control(void)
{
	void stdin_cleanup(int);

	fprintf(stderr,"Wanna exit (y or n)?");
	if (getchar() == 'y')
			stdin_cleanup(0);
	fprintf(stderr,"\nOk, back to tnt...\n");
}

void *
stdin_reader(void *arg)
{
	int fd;
	char c;

	fd = (int) arg;

	if (verbose)
		fprintf(stderr,"stdin reader thread...\n");

	while(read(0,&c,1) == 1) {
		if ((!ignore_usrctrl) && (c == CTRL_A)) {
			user_control();
			continue;
		}
		write(fd,&c,1);
	}

	perror("stdin_reader terminating");
	return(0);
}

/* ttywritechar():
 * This is called by the telnet server as it receives characters from
 * the remote client.  This supports the ability to run remote commands
 * with the use of the '!' as the first character in the string.
 */
void
ttywritechar(char c)
{
	static char syscmd[128];
	static int sysidx=0, lookforcommand=0, lookforbang=0;

	if (lookforbang) {
		lookforbang = 0;
		if (c == '!') {
			lookforcommand = 1;
			sysidx = 0;
			return;
		}
	}
	if ((c == 0x0a) || (c == 0x0d)) {
		if (remote_cmd_ok) {
			lookforbang = 1;
			if (lookforcommand) {
				syscmd[sysidx] = 0;
				printf("Running: <%s>\n",syscmd);
				system(syscmd);
				lookforcommand = 0;
				return;
			}
		}
	}
	if (lookforcommand) {
		syscmd[sysidx++] = c;
		return;
	}
	write(ttyfd,&c,1);
}

void
ttywrite(char *str, int len)
{
	int i;

	for(i=0;i<len;i++)
		ttywritechar(str[i]);
}

int
getbaud(int baud)
{
	switch(baud) {
		case 1200:
			return(B1200);
		case 2400:
			return(B2400);
		case 4800:
			return(B4800);
		case 9600:
			return(B9600);
		case 19200:
			return(B19200);
		case 38400:
			return(B38400);
		case 57600:
			return(B57600);
		case 115200:
			return(B115200);
	}
	fprintf(stderr,"Invalid baud: %d\n",baud);
	exit(1);
}

int
ttyopen(void *ttyname, int baud)
{
	int fd, flags;
	struct termio tty;

//	fd = open((char *)ttyname,O_RDWR | O_NDELAY);
	fd = open((char *)ttyname,O_RDWR);
	if (fd < 0) {
		perror(ttyname);
		return(-1);
	}

	if (ioctl(fd,TCGETA,&tty) < 0) {
		perror("ttyopen ioctl set");
		return(-1);
	}

	/* Set up configuration tty:
	 */
	tty.c_iflag = 0;
	tty.c_oflag = 0;
	tty.c_cflag = getbaud(baud) | CS8 | CREAD | HUPCL | CLOCAL;
	tty.c_lflag = 0;
	tty.c_cc[ VMIN ] = 1;
	tty.c_cc[ VTIME ] = 0;

	/* Initialize io control of remote tty: */
	if (ioctl(fd,TCSETA,&tty) < 0) {
		perror("ttyopen ioctl set");
		return(-1);
	}

#if 0
	{
	struct termios termioS;
	/* To accomodate some hardware that  allows input and output
	 * baudrates to differ, make sure they are the same... */
	if (tcgetattr(fd,&termioS) == -1) {
		perror("tcgetattr");
		return(-1);
	}
	if (cfsetispeed(&termioS,(speed_t)baud) == -1) {
		perror("cfsetispeed");
		return(-1);
	}
	if (cfsetospeed(&termioS,(speed_t)baud) == -1) {
		perror("cfsetospeed");
		return(-1);
	}
	if (tcsetattr(fd,TCSANOW,&termioS) == -1) {
		perror("tcsetattr");
		return(-1);
	}
	}
#endif

	/* After open() is complete, reconfigure port for
	 * blocking reads: */
	if ((flags = fcntl(fd,F_GETFL,0)) == -1) {
		perror("ttyopen fcntl get");
		return(-1);
	}
	if ((flags = fcntl(fd,F_SETFL,flags&~O_NDELAY)) == -1) {
		perror("ttyopen fcntl set");
		return(-1);
	}

	/* Empty the input queue: */
	ioctl(fd,TCFLSH,2);
	if (ioctl(fd,TCFLSH,2) < 0) {
		perror("ttyopen ioctl flush");
		return(-1);
	}
	fprintf(stderr,"Serial: %s @ %d baud\n",(char *)ttyname,baud);
	return(fd);
}

void
stdin_cleanup(int sig)
{
	static int beenhere = 0;

	if (!beenhere) {
		beenhere = 1;
		if (stdin_fixed) 
			ioctl(0,TCSETA,&oldstdin);
		if (lockfiles[0])
			unlink(lockfiles[0]);
		if (lockfiles[1])
			unlink(lockfiles[1]);
		if (lockfiles[2])
			unlink(lockfiles[2]);

		fprintf(stderr,"\nbye!\n");
		exit(1);
	}
}

#ifndef NCC
#define NCC NCCS
#endif

void
send_ctrlc(int ignore)
{
	char c = CTRL_C;
	write(ttyfd,&c,1);
}

void
stdin_fix(void)
{
	int i;

	/* Re-configure stdin */
	if (ioctl(0,TCGETA,&oldstdin) == -1) {
		perror("unable to save stdin");
		stdin_cleanup(0);
	}
	newstdin.c_iflag = oldstdin.c_iflag;
	newstdin.c_oflag = oldstdin.c_oflag;
	newstdin.c_cflag = oldstdin.c_cflag;
	newstdin.c_lflag = oldstdin.c_lflag;
	newstdin.c_line = oldstdin.c_line;
	for (i=0;i<NCC;i++)
		newstdin.c_cc[i] = oldstdin.c_cc[i];

	newstdin.c_lflag &= ~ICANON;
	newstdin.c_lflag &= ~ECHO;
	newstdin.c_cc[VMIN] = 1;
	if (ioctl(0,TCSETA,&newstdin) == -1) {
		perror("unable to re-define stdin");
		stdin_cleanup(0);
	}

	signal(SIGINT,send_ctrlc);
	stdin_fixed = 1;
}

void
tnt_daemonize(void)
{
	pid_t pid, sid; /* Our process ID and Session ID */

	/* Fork off the parent process
	 */
	pid = fork();
	if (pid < 0)
		exit(EXIT_FAILURE);
	
	/* If we got a good PID, then we can exit the parent process. */
	if (pid > 0)
		exit(EXIT_SUCCESS);

	/* Change the file mode mask */
	umask(0);

	/* Create a new SID for the child process */
	sid = setsid();
	if (sid < 0) {
		perror("daemonize setsid()");
		exit(-1);
	}

	if ((chdir("/")) < 0) {
		perror("daemonize chdir()");
		exit(-1);
	}
}

int
main(int argc,char **argv)
{
	struct	stat s;
	char	*ttyname;
	int		opt, port;
	pthread_attr_t attr;
	pthread_t tid = (pthread_t)0;

	lockfiles[0][0] = 0;
	lockfiles[1][0] = 0;
	lockfiles[2][0] = 0;
	notelnet = readonly = 0;
	baud = 9600;
	
	port = 8000;
	password = welcome = 0;
	ttyname = "/dev/ttyS0";
	progname = argv[0];

	while((opt=getopt(argc,argv,"b:dhil:P:p:nrRt:Vvw:")) != EOF) { 
		switch(opt) {
		case 'b':
			baud = atoi(optarg);
			break;
		case 'd':
			runasdaemon = 1;
			break;
		case 'p':
			port = atoi(optarg);
			break;
		case 'P':
			password = optarg;
			break;
		case 'h':
			usage(0);
			break;
		case 'i':
			ignore_usrctrl = 1;
			break;
		case 'l':
			lockdir = optarg;
			break;
		case 'n':
			notelnet = 1;
			break;
		case 'R':
			remote_cmd_ok = 1;
			break;
		case 'r':
			readonly = 1;
			break;
		case 't':
			ttyname = optarg;
			break;
		case 'V':
			printf("Built : %s\n",__DATE__);
			break;
		case 'v':
			verbose = 1;
			break;
		case 'w':
			welcome = optarg;
			break;
		default:
			usage("bad option");
		}
	}

	if ((runasdaemon) && (notelnet)) {
		fprintf(stderr,"Can't run as daemon if telnet is disabled\n");
		exit(1);
	}

	if (stat(ttyname,&s) == -1) {
		perror(ttyname);
		exit(1);
	}

#ifdef USE_LOCKFILES
	if (check_lock(ttyname,&s) == -1)
		exit(1);
#endif

	if ((ttyfd = ttyopen(ttyname,baud)) < 0)
		exit(1);

	if (runasdaemon)
		tnt_daemonize();
	else
		stdin_fix();

	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr,0x1000);
	pthread_create(&tid,&attr,tty_reader,(void *)ttyfd);

	if (notelnet) {
		terminate_message();
		stdin_reader((void *)ttyfd);
	}
	else {
		pthread_attr_init(&attr);
		pthread_attr_setstacksize(&attr,0x1000);
		if (runasdaemon == 0)
			pthread_create(&tid,&attr,stdin_reader,(void *)ttyfd);
		telnetd(password, welcome,port,readonly);
		perror("tc terminating");
	}
	return(0);
}

