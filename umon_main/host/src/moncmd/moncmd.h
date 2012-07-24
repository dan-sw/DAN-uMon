/** 
 *	\file moncmd.h:
 *	\brief Header file for moncmd.c
 */
#ifndef INADDR_NONE
#define INADDR_NONE	0xffffffff
#endif

/** \def EXIT_SUCCESS
 *	\brief Exit code if program is successful.
 */
#define EXIT_SUCCESS		0	

/** \def EXIT_CMP_TIMEOUT
 *	\brief Exit code if program times out waiting for completion.
 */
#define EXIT_CMP_TIMEOUT	1

/** \def EXIT_ERROR
 *	\brief Exit code if program has an error.
 */
#define EXIT_ERROR			2

/** \def EXIT_ACK_TIMEOUT
 *	\brief Exit code if program times out waiting for command ack.
 */
#define EXIT_ACK_TIMEOUT	3

/** \def IPPORT_MONCMD
 *	\brief Default UDP port number used by the program.
 */
#define IPPORT_MONCMD	777

extern int		sfd, retryFlag, retryCount, quietMode;
extern int		multirespMode, interactiveMode;
extern int		verbose, waitTime, cmdRcvd;
extern double	fwaitTime;
extern char		*thisProgname, *msgString, *targetHost;
extern struct	sockaddr_in targetAddr;

extern	int 	do_moncmd(char *,char *,short);
extern	void	moncmd_init(char *);
extern	void	moncmd_buffer_last_line(char *buf, int size);

extern int		optind, getopt(), close();
extern char		*optarg;

