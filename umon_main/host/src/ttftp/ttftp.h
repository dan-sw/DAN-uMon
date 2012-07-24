#ifdef BUILD_WITH_VCC
#define sleep(i) Sleep(i*1000)
#endif

#ifndef INADDR_NONE
#define INADDR_NONE	0xffffffff
#endif

/* Exit codes... */
#define EXIT_SUCCESS	0
#define EXIT_TIMEOUT	1
#define EXIT_ERROR		2

/* TFTP opcodes (see Stevens pg 466) */
#define TFTP_RRQ	1
#define TFTP_WRQ	2
#define TFTP_DAT	3
#define TFTP_ACK	4
#define TFTP_ERR	5
#define TFTP_OACK	6

#define TFTP_OCTET			1
#define TFTP_NETASCII		2
#define TFTP_DATAMAX		512
#define TFTP_FULLDATABLOCK	562	/* 512 + overhead */
#define TFTP_PKTOVERHEAD	(TFTP_FULLDATABLOCK-TFTP_DATAMAX)

#define TFTPTEST_SLEEP		1
#define TFTPTEST_CORRUPT	2
#define TFTPTEST_QUIT		3
#define TFTPTEST_BBNO		4

#define RETRY_MAX	10000	/* Give up after RETRY_MAX xmit retries */

typedef unsigned char uchar;


extern short tftpPort;
extern char	consoleTitle[256];
extern int	tftpVerbose, tftpQuiet, RetryCount, tftpSrvrTimeout, tftpPpd;
extern int	tftpRFC2349TsizeEnabled, tftpExitAfterRcv;
extern void	RetryInt();
extern int test_OpcodePassCount;
extern int test_Opcode;

extern int ttftp(char *,char *,char *,char *,char *);
extern void ttftp_init(void);

extern int	tftpsrvr();
extern int	testSetup(char *msg);
extern void testTftp(int,char *,int);

extern int	getopt(), optind;
extern char	*optarg;

