/* 
 * ian_xmodem.c
 *
 * X and Y modem file download and upload protocols.
 *
 * by Nick Patavalis (npat@inaccessnetworks.com)
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
 *
 */

/***************************************************************************/

#include "config.h"
#include "genlib.h"
#include "stddefs.h"
#include "flash.h"
#include "tfs.h"
#include "tfsprivate.h"
#include "cli.h"
#include "ascii.h"

/***************************************************************************/

#if INCLUDE_XMODEM

/* These "wait" functions may be better placed in chario.c.  They
 * are here for now, because this version of xmodem is the only
 * module that uses them.
 */

int
waitchar(int tmo)
{
	int timeout;
	int rval;

	timeout = tmo * LoopsPerSecond;
	while(!gotachar() && timeout)
		timeout--;
	if (timeout)
		rval = getchar();
	else
		rval = -1;

	return(rval);
}

void
waitclear (void)
{
	ulong timeout;

	while (1) {
		timeout = 1 * LoopsPerSecond;

		while(!gotachar() && timeout)
			timeout--;
		if (!timeout)
			break;
		getchar();
	}
}

int
waitbytes (char *buf, int cnt, int tmo)
{
	ulong timeout;
	int	i;

	for(i=0; i<cnt; i++) {
		timeout = tmo * LoopsPerSecond;
		while(!gotachar() && timeout)
			timeout--;
	    if (!timeout)
			break;
		buf[i] = (char)getchar();
	}
	return (i < cnt) ? -1 : i;
}

/***************************************************************************/

/* RCV: Frame timeout. Time to wait for next frame, before sending a NAK */
#define FRAME_TMO  5
/* RCV: Character timeout. Characters in the same frame should normally 
   not be more than CHAR_TMO seconds appart */
#define CHAR_TMO   1
/* TRN: Transmiter timeout. Usually a large value. The standard suggests 
   something in the order of 1m */
#define TRANS_TMO  20
/* TRN: Number of times to retransmit a frame before quiting */
#define RETR_MAX   10
/* TRN: Number of invalid characters (i.e. not ACK or NAK) received 
   by the transmiter before quiting. This *not* the right way to do 
   things. The right way is to wait for a valid char up to TRANS_TMO 
   secs, and then quit. Unfortunatelly without timers, this behavior is 
   a bit tricky to implement. So this crude approxomation will have to do 
   for the moment. */
#define NOISE_MAX 100

/* Maximum number of files per Y-modem transfer */
#define YFILES_MAX 10

/***************************************************************************/

/* X/Ymodem protocol: */
#define SOH ASCII_SOH
#define STX ASCII_STX
#define EOT ASCII_EOT
#define ACK ASCII_ACK
#define NAK ASCII_NAK
#define CAN ASCII_CAN
#define ESC ASCII_ESC
#define BS  ASCII_BS

#define PKTLEN_128 128
#define PKTLEN_1K  1024

/***************************************************************************/

enum _xmodem_ops {
	XNULL = 0,
	XUP,
	XDOWN
};

enum _protocol_type {
	XMODEM = 0,
	YMODEM = 1
};

#define XERR_GEN  (-1)
#define XERR_TOUT (-2)
#define XERR_SYNC (-3)
#define XERR_CAN  (-4)
#define XERR_UCAN (-5)
#define XERR_RETR (-6)
#define XERR_HEAD (-7)
#define XERR_TFS  (-8)
#define XERR_NOFILE (-9)
#define XERR_NOSIZE (-10)

static char *xerr_msgs[] = {
	"E00:No Error",
	"E01:General Error",
	"E02:Timeout",
	"E03:Synchronization error",
	"E04:Operation canceled",
	"E05:Operation canceled by user",
	"E06:Maximum retransmitions exceeded",
	"E07:Invalid header-block",
	"E08:TFS error",
	"E09:File not found",
	"E10:Cannot determine file size"
};

/***************************************************************************/

/* struct yinfo:
 * used to pass information to and from the y-modem transfer functions.
 */
struct yinfo {	
	int usecrc;      /* use 16bit CRC instead of 8bit checksum */
	int onek;        /* use 1k blocks instead of 128bytes blocks */ 
	int verify;      /* operate in verification-mode */
	ulong baseaddr;  /* start address for the first file data transfer */
	char *flags;     /* TFS file flags (same for all files) */
	char *info;      /* TFS file info (same for all files) */

	int filecnt;                         /* number of files processed */
	char fname[YFILES_MAX][TFSNAMESIZE]; /* names of files processed */
	ulong dataddr[YFILES_MAX];           /* start address of each file */
	long size[YFILES_MAX];               /* size of each file in bytes */
	long pktcnt[YFILES_MAX];             /* number of frames exchanged */
	long errcnt[YFILES_MAX];             /* number of verification errors */
	ulong firsterrat[YFILES_MAX];        /* offset of first error */
};

/* struct xinfo:
 * used to pass information to and from the x-modem transfer functions.
 */
struct xinfo {
	int     usecrc;      /* use 16bit CRC instead of 8bit checksum */
	int     onek;        /* use 1k blocks instead of 128bytes blocks */ 
	int     verify;      /* operate in verification-mode */
	ulong	dataddr;	 /* start address for data transfer */
	long	size;	     /* size of the tansfer in bytes */
	int		pktcnt;		 /* number of packets processed */
	int		errcnt;		 /* number of errors (used in verify mode) */
	ulong	firsterrat;  /* pointer to location if first error (vrf mode) */
};

/***************************************************************************/

/* local prototypes */

static int Xup(struct xinfo *, int);
static int Xdown(struct xinfo *, int);
static int Ydown(struct yinfo *);
static int Yup(struct yinfo *);
static int getPacket(uchar *, int, int);
static void putPacket (uchar *, int, int, int);
static char *xerr_msg(int);
static void doCAN(void);

/***************************************************************************/

/* Defined globaly to conserve stack-space. Ugly but effective. */
static struct yinfo yif;
static struct xinfo xif;
static uchar pktbuff[PKTLEN_1K];

/* Used to pass-back the TFS error code, in case of a TFS error */
int tfserr;

/***************************************************************************/

char *XmodemHelp[] = {
	"X/Y modem file transfer",
#if INCLUDE_TFS
#if INCLUDE_FLASH
	"-[a:BckdF:f:i:s:t:uvy]",
#else /* of INCLUDE_FLASH */
	"-[a:ckdF:f:i:s:t:uvy]",
#endif /* of INCLUDE_FLASH */
#else /* of INCLUDE_TFS */
#if INCLUDE_FLASH
	"-[a:Bckds:t:uvy]",
#else /* of INCLUDE_FLASH */
	"-[a:ckds:t:uvy]",
#endif /* of INCLUDE_FLASH */
#endif /* of INCLUDE_TFS */
	"Options:",
	" -a{##}     address (overrides default of APPRAMBASE)",
#if INCLUDE_FLASH
	" -B         Program boot-sector (i.e. uMon-binary reload)",
#endif
	" -c         have xmodem receiver request the use of CRC (def = chksum)",
	"            have ymodem receiver request the use of chksum (def = CRC)",
	" -k         send 1K frames (default = 128bytes)",
	" -d         download",
#if INCLUDE_TFS
	" -F{name}   filename",
	" -f{flags}  file flags (see tfs)",
	" -i{info}   file info (see tfs)",
#endif
	" -s{##}     size (overrides computed size)",
	" -u         upload",
	" -v         verify only",
	" -y         use YMODEM extensions",
	"Notes:",
	" * Either -d or -u must be specified (-B implies -d).",
	" * The -c argument affects only the receiver (download operations).",
    "   The -k option affects only the transmitter (upload operations).",
#if INCLUDE_TFS
    " * YMODEM downloads: The file-name and size arguments are ignored,",
    "   since they are supplied by the sender. Multiple files can be",
    "   downloaded. Each file is downloaded at the address specified by",
    "   the '-a' argument (or at APPRAMBASE), and then saved to the TFS.",
    "   A summary of the files downloaded, their sizes and the download",
	"   locations is displayed at the end of the transfer.", 
#else /* of INCLUDE_TFS */
    " * YMODEM downloads: The file-name and size arguments are ignored,",
    "   since they are supplied by the sender. Multiple files can be",
    "   downloaded. The files are stored sequencially starting at the", 
    "   address specified with '-a' (or at APPRAMBASE). A summary of the",
    "   files downloaded, their sizes and the download locations is",
    "   displayed at the end of the transfer.",
#endif /* of INCLUDE_TFS */
#if INCLUDE_FLASH
    "   If -B is given, the boot-sector is programmed with the contents",
    "   of the last file downloaded.",
#endif
#if INCLUDE_TFS
	" * XMODEM downloads: Only one file can be downloaded. A 128-byte",
    "   modulo is forced on file size. The file is downloaded at the address",
    "   specified by '-a' (or at APPRAMBASE), and then saved to a TFS file",
    "   if a file-name argument is given. The '-s' option can be used to", 
    "   override the 128-bytes modulo when transferring a file to TFS.",
#else /* of INCLUDE_TFS */
	" * XMODEM downloads: Only one file can be downloaded. A 128-byte",
    "   modulo is forced on file size. The file is downloaded at the address",
    "   specified by '-a' (or at APPRAMBASE).",
#endif /* of INCLUDE_TFS */
#if INCLUDE_FLASH
    "   If -B is given, the boot-sector is programmed with the contents",
    "   of the downloaded file.",
#endif /* of INCLUDE_FLASH */
#if INCLUDE_TFS
    " * X/YMODEM uploads: Only one file can be uploaded. The size argument",
    "   is ignored since it is taken form the TFS. Before the file is", 
	"   uploaded it is copied at the address specified by '-a' (or at", 
    "   APPRAMBASE).",
#else /* of INCLUDE_TFS */
    " * X/YMODEM uploads: Only one file can be uploaded. The size argument",
    "   is mandatory (since there is no other way to know the number of", 
    "   bytes to transfer). The data (file) to be uploaded are taken form",
    "   the address specified by '-a' (or from APPRAMBASE).",
#endif /* of INCLUDE_TFS */
	(char *)0,
};

/***************************************************************************/

int
Xmodem(int argc,char *argv[])
{
#if INCLUDE_TFS
	TFILE	*tfp;
#endif
	char fname[TFSNAMESIZE+1], *info, *flags;
	int	opt, xop, newboot, usecrc, verify, onek, ymodem;
	ulong dataddr;
	long size;
	int i, r;

	/* Initialize to defaults */
	dataddr = getAppRamStart();
	xop = XNULL;
	newboot = 0;
	usecrc = 0;
	fname[0]='\0';
	info = (char *)0;
	flags = (char *)0;
	onek = 0;
	size = 0;
	verify = 0;
	ymodem = 0;

	/* Parse command line arguments */
	while ((opt=getopt(argc,argv,"a:Bci:f:dF:ks:uvy")) != -1) {
		switch(opt) {
		case 'a':
			dataddr = strtoul(optarg,(char **)0,0);
			break;
#if INCLUDE_FLASH
		case 'B':
			xop = XDOWN;
			newboot = 1;
			break;
#endif
		case 'c':
			usecrc = 1;
			break;
		case 'd':
			xop = XDOWN;
			break;
#if INCLUDE_TFS
		case 'F':
			strncpy(fname, optarg, TFSNAMESIZE);
			break;
		case 'f':
			flags = optarg;
			break;
		case 'i':
			info = optarg;
			break;
#endif
		case 'k':
			onek = 1;
			break;
		case 's':
			size = (ulong)strtoul(optarg,(char **)0,0);
			break;
		case 'u':
			xop = XUP;
			break;
		case 'v':
			verify = 1;
			break;
		case 'y':
			ymodem = 1;
			break;
		default:
			return(CMD_PARAM_ERROR);
		}
	}

	/* There should be no arguments after the option list. */
	if (argc != optind)
		return(CMD_PARAM_ERROR);

	if ( xop == XUP ) {

		/* UPLOAD: Host <-- Micromonitor */

		if ( ymodem ) {

			/* we 're using the Y-Modem extensions */

			yif.onek = onek;
			yif.baseaddr = dataddr;
			yif.filecnt = 1;
			if (fname && fname[0]) {
				strcpy(yif.fname[0], fname);
			} else {
				strcpy(yif.fname[0], "noname");
			}
			yif.size[0] = size;

#if ! INCLUDE_TFS
			if ( yif.size[0] <= 0 ) {
				printf("%s\n", xerr_msg(XERR_NOSIZE));
				return(CMD_FAILURE);
			}
#endif

			r = Yup(&yif);

			printf("\n");

			if ( yif.filecnt > 0 ) {
				printf("Sent %d file%s.\n",
					   yif.filecnt, (yif.filecnt > 1) ? "s" : "");
				for (i = 0; i < yif.filecnt; i++) {
					printf(" %s: %ld bytes, %ld blocks, @ 0x%08lx\n",
						   yif.fname[i] ? yif.fname[i] : "<noname>",
						   yif.size[i],
						   yif.pktcnt[i],
						   yif.dataddr[i]);
				}
			}

			if ( r < 0 ) {
				printf("%s\n", xerr_msg(r));
				return(CMD_FAILURE);
			}			

		} else {

			/* plain X-modem */

			xif.dataddr = dataddr;
			xif.size = size;
			xif.onek = onek;

			if ( fname && fname[0] ) {
#if INCLUDE_TFS
				if ( ! (tfp = tfsstat(fname)) ) {
					printf("%s\n", xerr_msg(XERR_NOFILE));
					return(CMD_FAILURE);
				}
				memcpy((char *)(xif.dataddr), TFS_BASE(tfp), tfp->filsize);
				xif.size = size = tfp->filsize;
#endif
			}

			if ( xif.size <= 0 ) {
				printf("%s\n", xerr_msg(XERR_NOSIZE));
				return(CMD_FAILURE);
			}

			r = Xup(&xif, XMODEM);

			printf("\n");

			if ( r < 0 ) {
				printf("%s\n", xerr_msg(r));
				return(CMD_FAILURE);
			}

			printf("Sent 1 file.\n");

			printf(" %s: %ld bytes, %d blocks, @ 0x%08lx\n",
				   fname[0] ? fname : "<no-name>",
				   xif.size,
				   xif.pktcnt,
				   xif.dataddr);
		}

	} else if ( xop == XDOWN ) {

		/* DOWNLOAD: Host --> Micromonitor */

		if ( ymodem ) {

			/* Use Y-Modem extensions */

			yif.baseaddr = dataddr;
			yif.usecrc = !usecrc;
			yif.verify = verify;
			yif.flags = flags;
			yif.info = info;

			r = Ydown(&yif);

			printf("\n");

			if ( yif.filecnt ) {
				printf("Rcvd %d file%s.\n",
					   yif.filecnt, (yif.filecnt > 1) ? "s" : "");
				if ( yif.verify ) {
					for (i = 0; i < yif.filecnt; i++) {
						printf(" %s: %ld bytes, %ld blocks, %ld errors, " 
							   "first err @ 0x%08lx\n",
							   yif.fname[i] ? yif.fname[i] : "<noname>",
							   yif.size[i],
							   yif.pktcnt[i],
							   yif.errcnt[i], yif.firsterrat[i]);
					}
				} else {
					for (i = 0; i < yif.filecnt; i++) {
						printf(" %s: %ld bytes, %ld blocks, @ 0x%08lx\n",
							   yif.fname[i] ? yif.fname[i] : "<noname>",
							   yif.size[i],
							   yif.pktcnt[i],
							   yif.dataddr[i]);
					}
				}
			}

			if ( r < 0 ) {
				if ( r == XERR_TFS ) {
					printf("%s: %s: %s\n",
						   xerr_msg(XERR_TFS),
						   yif.fname[yif.filecnt], 
						   (char *)tfsctrl(TFS_ERRMSG,tfserr,0));
				} else {
					printf("%s\n", xerr_msg(r));
				}
				return(CMD_FAILURE);
			}

		} else {

			/* plain X-Modem */

			xif.dataddr = dataddr;
			xif.usecrc = usecrc;
			xif.verify = verify;

			if (verify && fname && fname[0]) {
#if INCLUDE_TFS
				if (! (tfp = tfsstat(fname)) ) {
					printf("%s\n", xerr_msg(XERR_NOFILE));
					return(CMD_FAILURE);
				}
				memcpy((char *)(xif.dataddr), TFS_BASE(tfp), tfp->filsize);
#endif
			}

			r = Xdown(&xif, XMODEM);

			if (size) {
				/* override size by user specified value */
				xif.size = size;
			}

			printf("\n");

			if ( r < 0 ) {
				printf("%s\n", xerr_msg(r));
				return(CMD_FAILURE);
			}

			if ( ! xif.verify ) {
#if INCLUDE_TFS
				if (xif.size > 0 && fname && fname[0]) {
					/* save file to TFS */
					r = tfsadd(fname, info, flags, 
							   (uchar *)xif.dataddr, xif.size);
					if (r != TFS_OKAY) {
						printf("%s: %s: %s\n",
							   xerr_msg(XERR_TFS),
							   fname, (char *)tfsctrl(TFS_ERRMSG,r,0));
						return(CMD_FAILURE);
					}
				}
#endif
			}
			
			printf("Rcvd 1 file.\n");

			if ( xif.verify ) {
				printf(" %s: %ld bytes, %d blocks, %d errors, "
					   "first err @ 0x%08lx\n",
					   fname[0] ? fname : "<noname>",
					   xif.size,
					   xif.pktcnt,
					   xif.errcnt, xif.firsterrat);
			} else {
				printf(" %s: %ld bytes, %d blocks, @ 0x%08lx\n",
					   fname[0] ? fname : "<no-name>",
					   xif.size,
					   xif.pktcnt,
					   xif.dataddr);
			}

		}

#if INCLUDE_FLASH
		/* whether it is an X or a Y modem download, if newboot
		   is set, the (last) downloaded file will be written
		   to the boot-sector */
		if (xif.size > 0 && newboot) {
			extern	int FlashProtectWindow;
			char	*bb;
			ulong	bootbase;
			
			bb = getenv("BOOTROMBASE");
			if (bb)
				bootbase = strtoul(bb,0,0);
			else
				bootbase = BOOTROM_BASE;
			
			FlashProtectWindow = 1;
			printf("Reprogramming boot @ 0x%lx from 0x%lx, %ld bytes.\n",
				   bootbase, xif.dataddr, xif.size);
			if (askuser("OK?")) {
				if (flashewrite(&FlashBank[0], (char *)bootbase,
					 (char *)xif.dataddr, xif.size) == -1) {
					/* flashewrite should never return */
					printf("failed\n");
					return(CMD_FAILURE);
				}
			}
		}
#endif
	} else {
		/* Neither Upload nor Download */
		return(CMD_PARAM_ERROR);
	}

	return(CMD_SUCCESS);
}

/***************************************************************************/

static char*
xerr_msg (int code)
{
	return (code >= 0) ? xerr_msgs[0] : xerr_msgs[-code];
}

/***************************************************************************/

static void
doCAN (void) 
{
	waitclear();

	/* send eight CANs */
	rputchar(CAN); rputchar(CAN);
	rputchar(CAN); rputchar(CAN);
	rputchar(CAN); rputchar(CAN);
	rputchar(CAN); rputchar(CAN);

	/* send eight backspaces */
	rputchar(BS); rputchar(BS);
	rputchar(BS); rputchar(BS);
	rputchar(BS); rputchar(BS);
	rputchar(BS); rputchar(BS);

	waitclear();
}

/***************************************************************************
 * DOWNLOAD STUFF
 **************************************************************************/

/* getPacket():
 * Used by Xdown to retrieve packets.
 */
static int
getPacket(uchar *pkt, int len, int usecrc)
{
	int	i;
	uchar seq[2];

	/* s: Get and check the frame sequence number */

	if ( waitbytes(seq, 2, CHAR_TMO) < 0 ) {
		Mtrace("T<seq>");
		return -1;
	}

	if ( seq[0] != (uchar)(~seq[1]) ) {
		Mtrace("~<%02x != %02x>", seq[0], seq[1]);
		return -1;
	}

	Mtrace("s");

	/* d: Get the frame's data */
				
	if ( waitbytes(pkt, len, CHAR_TMO) < 0 ) {
		Mtrace("T<data>");
		return -1;
	}

	Mtrace("d");

	/* d: Get the frame's CRC or checksum */

	if ( usecrc ) {
		int c;
		ushort	crc, xcrc;
		
		c = waitchar(CHAR_TMO);
		if ( c < 0 ) {
			Mtrace("T<crc>");
			return -1;
		}
		crc = (ushort)c << 8;

		c = (uchar)waitchar(CHAR_TMO);
		if ( c < 0 ) {
			Mtrace("T<crc>");
			return -1;
		}
		crc |= (ushort)c;

		xcrc = xcrc16((uchar *)pkt,(ulong)(len));
		if (crc != xcrc) {
			Mtrace("C<%04x != %04x>", crc, xcrc);
			return -1;
		}
	} else {
		uchar csum; 
		int xcsum;

		xcsum = waitchar(CHAR_TMO);
		if ( xcsum < 0 ) {
			Mtrace("T<xcsum>");
			return -1;
		}
		csum = 0;
		for(i=0; i < len; i++)
			csum += *pkt++;
		if (csum != xcsum) {
			Mtrace("c<%02x != %02x>",csum,xcsum);
			return -1;
		}
	}

	Mtrace("c");

	return seq[0];
}

/***************************************************************************/

/*
 * waitPacket
 *
 * Receive a X or Y modem packet. Step the reception state-machine.
 *
 * Acknowledge the previous frame (or 'kick' the transmiter with a 'C' 
 * or NAK if this is the first frame), and receive the next-one. Also 
 * handle frame retransmitions.
 *
 * Arguments:
 *   - pb - packet (frame) buffer. Write the packet-data here.
 *   - *len - packet's size in bytes will be written here
 *   - sno - packet sequence number to wait for.
 *   - usercrc - if non-zero, user 16-bit CRC instread of 8-bit checksum.
 *
 * Returns:
 *   0 - if packet was received successfully
 *   1 - if an EOT was received instead of a packet
 *   Something negative on fatal error:
 *     XERR_SYNC : Synchronization lost
 *     XERR_CAN : Operation canceled
 *     XERR_UCAN : Operation canceled by user
 */

static int
waitPacket (uchar *pb, int *len, ulong lsno, int usecrc)
{
	char nak;
	int done, rval;
	int c, r, sno, psize;
	
	if ( usecrc ) {
		nak = 'C';
	} else {
		nak = NAK;
	}
	if ( (lsno == 0 || lsno == 1) ) {
		/* if it is the first frame of a X or Y modem session,
		   'kick' the transmiter by sending a NAK or a 'C' */
		rputchar(nak);
	} else {
		/* if it is not the first frame of an X or Y modem session,
		   acknowledge the previous frame */
		rputchar(ACK);
	}

	sno = lsno % 0x100;

	/* loop exitst with:
	   done = 1: packet received succesfully
	   done = 2: EOT received and acknowledged
	   done < 0: Fatal error. 'done' holds error code. */
	do {
		done = 0;
		c = waitchar(FRAME_TMO);
		switch ( c ) {
		case STX:
		case SOH:
			/* ok, its a block */
			Mtrace("P[");
			*len = psize = (c == STX) ? 1024 : 128;
			r = getPacket(pb, psize, usecrc);
			Mtrace("]");
			if (r < 0) {
				/* block reception not ok. request retransmission */
				waitclear();
				rputchar(nak);
				break;
			}
			if ( r != sno ) {
				/* block sequence number is not quite right, 
				   but it might still do */
				if ( (lsno == 0) || ( sno - r != 1 && r - sno != 0xff ) ) {
					/* shit! desperately out of sync. 
					   CAN the whole thing */
					Mtrace("S<%02x != %02x>", r, sno);
					doCAN();
					done = XERR_SYNC;
				} else {
					/* no worries, it's just a retransmission */
					Mtrace("S");
					rputchar(ACK);
					if (lsno == 1) {
						Mtrace("0");
						rputchar(nak);
					}
				}
			} else {
				/* seq. number just fine */
				Mtrace("s");
				if ( lsno == 0 ) {
					Mtrace("0");
					rputchar(ACK);
				}
				done = 1;
			}
			break;
		case CAN:
			Mtrace("C");
			/* wait for a second CAN */
			c = (char)waitchar(CHAR_TMO);
			if (c != CAN) {
				Mtrace("<%02x>", c);
				waitclear();
				rputchar(nak);
			} else {
				Mtrace("C");
				done = XERR_CAN;
			}
			break;
		case ESC:
			/* User-invoked abort */
			Mtrace("X");
			/* wait for a second ESC */
			c = (char)waitchar(CHAR_TMO);
			if (c != ESC) {
				waitclear();
				rputchar(nak);
				done = 0;
			} else {
				done = XERR_UCAN;
			}
			break;
		case EOT:
			Mtrace("E");
			/* first make sure that this is really an EOT,
			   and not a corrupted start-frame; use the fact that
			   self-respecting EOTs always come alone, not followed 
               by other characters. */ 
			c = waitchar(CHAR_TMO);
			if ( c >= 0 ) {
				/* probably not an EOT, request retransmission */
				Mtrace("<%02x>", c);
				waitclear();
				rputchar(nak);
				break;
			}
			/* ok it is probably a true EOT, send an ACK */
			rputchar(ACK);
			Mtrace("a", c);
			if (lsno != 0) {
				done = 2;
			}
			break;
		default:
			/* request retransmission */
			Mtrace("?<%02x>",(uchar)c);
			waitclear();
			rputchar(nak);
			break;
		}
	} while (!done);

	if (done == 1) {
		rval = 0;
	} else if (done == 2) {
		rval = 1;
	} else {
		rval = done;
	}

	return rval;
}

/***************************************************************************/

/* Xdown():
 *
 * Download, or verify a file using the XMODEM protocol
 *
 * Arguments:
 *   - xip - structure containing transfer info:
 *	  usecrc       IN  - use 16bit-CRC instead of 8bit-checksum.
 *    verify       IN  - operate in verification mode.
 *	  dataddr:     IN  - downloaded file will be stored starting from here.
 *                       in verification mode, downloaded file will be 
 *                       compared with data starting from here.
 *	  size:        OUT - size of received file in bytes. 
 *	  pktcnt:      OUT - number of frames transfered.
 *	  errcnt:      OUT - number of errors, if mode is verification.
 *	  firsterrat:  OUT - address of first error, if mode is verification.
 *  - proto - protocol type (y-modem or x-modem); Used to make very small 
 *  adjustment to the operation of the function. 
 *
 * Returns:
 *    On success: something non-negative.
 *    On error: something negative.
 *      XERR_GEN : General error
 *      XERR_TOUT : Timeout
 *      XERR_SYNC : Lost synchronization
 *      XERR_CAN : Operation canceled
 *      XERR_UCAN : Operation canceled by user
 */

static int
Xdown(struct xinfo *xip, int proto)
{
	int i, r;
	ulong sno;
	int psize;
	char *tmppkt, *dataddr;

	Mtrace("--XD--");

	dataddr = (char *)(xip->dataddr);

	if ( xip->verify ) {
		tmppkt = pktbuff;
	} else {
		tmppkt = (char *)(xip->dataddr);
	}

	sno = 1;
	xip->pktcnt = 0;
	xip->errcnt = 0;
	xip->firsterrat = 0;
	xip->size = 0;

	do {
		r = waitPacket(tmppkt, &psize, sno, xip->usecrc);
		if ( r == 0 ) {
			sno++;
			xip->pktcnt++;
			xip->size += psize;
			if ( xip->verify ) {
				for (i = 0; i < psize; i++, dataddr++) {
					if (tmppkt[i] != *dataddr) {
						if (xip->errcnt++ == 0)
							xip->firsterrat = (ulong)dataddr;
					}
				}
			} else {
				tmppkt += psize;
			}
		}
	} while ( ! r );

	if ( r == 1 && proto == XMODEM ) {
		/* send a couple more ACKs to make sure that at least 
		   one reaches its destination */
		rputchar(ACK); rputchar(ACK); rputchar(ACK);
		rputchar(ACK); rputchar(ACK); rputchar(ACK);
		/* send backspaces in-case ACKs have reached the user's
		   terminal */
		rputchar(BS); rputchar(BS); rputchar(BS);
		rputchar(BS); rputchar(BS); rputchar(BS);
	}

	if (proto == XMODEM) {
		/* trash any final garbage send by the transmiter */
		waitclear();
	}

	return r;
}

/***************************************************************************/

/* Ydown():
 *
 * Download (uMon <-- Host) a file using the YMODEM protocol
 *
 * Arguments:
 *   - yip - structure containing transfer info:
 *    usecrc       IN  - use 16bit CRC instead of 8bit checksum.
 *	  onek         x
 *    verify       IN  - verify the downloaded file against stored data.    
 *	  baseaddr:    IN  - download the first file here.
 *    flags        IN  - TFS flags to use when writing to TFS. 
 *    info         IN  - TFS info to use when writing to TFS.
 *    filecnt      OUT - Number of files successfully downloaded.
 *    fname[i]     OUT - Name of the i'th file downloaded.
 *	  size[i]:     OUT - size of the i'th file downloaded in bytes.
 *	  pktcnt[i]:   OUT - nr of frames transfered when downloading i'th file.
 *	  errcnt[i]:   OUT - number of errors detected when verifying i'th file.
 *	  firsterrat[i]: OUT - address of the first error of the i'th file.
 *
 * Returns:
 *    On success: something non-negative.
 *    On error: something negative.
 *      XERR_GEN : General error
 *      XERR_TOUT : Timeout
 *      XERR_NOFILE : File not found (verification mode)
 *      XERR_TFS : TFS error
 *      XERR_SYNC : Synchronization error
 *      XERR_CAN : Operation canceled
 *      XERR_UCAN : Operation canceled by user
 */

static int
Ydown(struct yinfo *yip)
{
	TFILE *tfp;
	int rval, r, psize;
	char *tmppkt;
	ulong dataddr;
	char *fname, *basename, *s1, *fsize;

	rval = 0;
	yip->filecnt = 0;
	tmppkt = pktbuff;
	dataddr = yip->baseaddr;

	do {
		yip->dataddr[yip->filecnt] = dataddr;
		yip->size[yip->filecnt] = 0;
		yip->pktcnt[yip->filecnt] = 0;
		yip->errcnt[yip->filecnt] = 0;
		yip->firsterrat[yip->filecnt] = 0;

		/* 
		 * receive and parse the header-block 
		 */

		r = waitPacket(tmppkt, &psize, 0, 1);
		if ( r < 0 ) {
			rval = r;
			break;
		}

		/* first comes the file-name */
		fname = (char *)(tmppkt);
		/* buggy ymodems delimit fields with SP instead of NULL.
		   Cope. */
		s1 = strchr(fname,' ');
		if (s1) *s1 = '\0';
		/* if file-name contains path, cut it off and just keep
		   the stem */
		basename = strrchr(fname,'/');
		if (! basename) basename = fname;

		/* after the file-name comes the file-size */
		fsize = basename + strlen(basename) + 1;
		/* buggy ymodems delimit fields with SP instead of NULL.
		   cope. */
		s1 = strchr(fsize,' ');
		if (s1) *s1 = '\0';
		
		/* copy info to xip fields */
		yip->size[yip->filecnt] = atoi(fsize);
		strcpy(yip->fname[yip->filecnt], basename);

		Mtrace("<f=%s>",yip->fname[yip->filecnt]);
		Mtrace("<z=%d>",yip->size[yip->filecnt]);
		Mtrace("]");

		if ( ! yip->fname[yip->filecnt][0] )
			break;
		
		/* 
		 * receive file-data
		 */
		xif.dataddr = dataddr;
		xif.usecrc = yip->usecrc;
		xif.verify = yip->verify;

		if ( yip->verify ) {
#if INCLUDE_TFS
			if (! (tfp = tfsstat(yip->fname[yip->filecnt])) ) {
				waitclear();
				doCAN();
				rval = XERR_NOFILE;
				break;
			}
			memcpy((char *)(xif.dataddr), TFS_BASE(tfp), tfp->filsize);
#endif
		}
		
		r = Xdown(&xif, YMODEM);

		if ( r < 0 ) {
			rval = r;
			break;
		}

		if (yip->size[yip->filecnt] <= 0) {
			yip->size[yip->filecnt] = xif.size;
		}
		yip->pktcnt[yip->filecnt] = xif.pktcnt;
		yip->errcnt[yip->filecnt] = xif.errcnt;
		yip->firsterrat[yip->filecnt] = xif.firsterrat;

		
		/* 
		 * write file to TFS 
		 */
		if ( ! yif.verify ) {
#if INCLUDE_TFS
			if ( yip->size[yip->filecnt] > 0 ) {
				/* save file to TFS */
				r = tfsadd(yip->fname[yip->filecnt], 
						   yip->info, yip->flags, 
						   (uchar *)(yip->dataddr[yip->filecnt]), 
						   yip->size[yip->filecnt]);
				if (r != TFS_OKAY) {
					doCAN();
					rval = XERR_TFS;
					tfserr = r;
					break;
				}
			}
#endif
		}

#if INCLUDE_TFS
		dataddr = yip->baseaddr;
#else
		dataddr += yip->size[yip->filecnt];
#endif
		yip->filecnt++;

	} while (yip->filecnt < YFILES_MAX);

	if ( yip->filecnt >= YFILES_MAX ) {
		/* Cancel the transaction. Let the transmiter know that we 
           can receiver no more files */
		waitclear();
		doCAN();
	}

	/* trash any final garbage send by the transmiter */
	waitclear();

	return rval;
}

/***************************************************************************
 * UPLOAD STUFF
 **************************************************************************/

/*
 * putPacket
 *
 * Send an X-Modem or Y-Modem packet
 */

static void
putPacket (uchar *pkt, int len, int sno, int usecrc)
{
	int i;
	ushort chksm;

	chksm = 0;

	/* Send start of frame. */
	if ( (len == PKTLEN_128) || (sno == 0) ) {
		rputchar(SOH);
	} else {
		rputchar(STX);
	}

	/* Send sequence number and its complement */
	rputchar(sno);
	rputchar((uchar)~(sno));

	/* Send data. Calculate CRC or checksum. Send CRC or checksum. */ 
	if ( usecrc ) {
		for(i=0; i < len; i++) {
			rputchar(*pkt);
			chksm = (chksm << 8) ^ xcrc16tab[ (chksm>>8) ^ *pkt++ ];
		}
		rputchar((uchar)(chksm >> 8));
		rputchar((uchar)(chksm & 0x00ff));
	} else {
		for(i=0; i < len; i++) {
			rputchar(*pkt);
			chksm = ((chksm+*pkt++)&0xff);
		}
		rputchar((uchar)(chksm&0x00ff));
	}
}

/***************************************************************************/

/*
 * sendPacket
 *
 * Send and X or Y modem protocol packet.
 *
 * Transmits the given frame and waits for acknowledgment. 
 * Handles retransmitions and cancel requests as well.
 *
 * Arguments:
 *   - pb - packet (frame) buffer. The packet to send is here.
 *   - len - length (in bytes) of the frame to send.
 *   - sno - sequence number of the frame to send
 *   - userce - if non-zero, use 16-bit CRC instead of 8-bit checksum.
 * 
 * Returns:
 *   Non-negative on success (packet sent and acknowledged)
 *   Something negative on error:
 *     XERR_RETR : Max. number of retransmissions exceeded
 *     XERR_TOUT : Timeout (too many 'noise' chars received)
 *     XERR_CAN : Operation canceled
 *     XERR_UCAN : Operation canceled by user
 */

static int
sendPacket (uchar *pb, int len, ulong lsno, int usecrc)
{
	int sno, c, noise, count, done;

	Mtrace("P");
	sno = lsno % 0x100;
	putPacket(pb, len, sno, usecrc);

	noise = 0;
	count = RETR_MAX;
	done = 0;
	do {
		c = waitchar(TRANS_TMO);
		switch ( c ) {
		case ACK:
			Mtrace("A");
			done = 1;
			break;
		case NAK:
			Mtrace("K");
			if ( --count ) {
				putPacket(pb, len, sno, usecrc);
			} else {
				Mtrace("R");
				waitclear();
				doCAN();
				done = XERR_RETR;
			}
			break;
		case CAN:
			Mtrace("N");
			c = (char)waitchar(CHAR_TMO);
			if (c != CAN) {
				Mtrace("<%02x>", c);
				noise++;
			} else {
				Mtrace("N");
				done = XERR_CAN;
				waitclear();
			}
			break;
		case ESC:
			Mtrace("X");
			c = (char)waitchar(CHAR_TMO);
			if (c != ESC) {
				Mtrace("<%02x>", c);
				noise++;
			} else {
				Mtrace("N");
				done = XERR_UCAN;
				waitclear();
			}
			break;
		default:
			Mtrace("<%02x>", c);
			noise++;
			break;
		}
	} while ( ! done && noise < NOISE_MAX );
			
	if ( noise >= NOISE_MAX ) {
		done = XERR_TOUT;
	}

	return done;
}

/***************************************************************************/

/*
 * sync_trx
 * 
 * synchronize transmiter
 *
 * Called by the upload fucntions and performs initial synchronization
 * with the receiver.
 *
 * Returns:
 *   On success: Synchronization character send by the receiver (i.e. C 
 *     or NAK).
 *   On error: something negative.
 *     XERR_TOUT - Timeout waiting for sync.  
 *     XERR_CAN - Operation canceled.
 *     XERR_UCAN - Operation canceled by user.
 */

static int
sync_tx (void)
{
	int done, c;

	Mtrace("r[");
	done = 0;
	do {
		c = waitchar(TRANS_TMO);
		if ( c < 0 ) {
			Mtrace("T");
			done = XERR_TOUT;
			waitclear();
			doCAN();
			break;
		}
				
		switch ( c ) {
		case NAK:
			Mtrace("K");
			done = NAK;
			break;
		case 'C':
			Mtrace("C");
			done = 'C';
			break;
		case CAN:
			Mtrace("N");
			c = waitchar(CHAR_TMO);
			if ( c != CAN ) {
				Mtrace("<%02x>", c);
			} else {
				Mtrace("N", c);
				done = XERR_CAN;
				waitclear();
			}
			break;
		case ESC:
			Mtrace("X");
			c = waitchar(CHAR_TMO);
			if ( c != ESC ) {
				Mtrace("<%02x>", c);
			} else {
				Mtrace("X", c);
				done = XERR_UCAN;
				waitclear();
			}
			break;
		default:
			Mtrace("<%02x>", c);
			break;
		}
	} while ( ! done );
	
	Mtrace("]");
			
	return done;
}

/***************************************************************************/

/* Xup():
 *
 * Upload (uMon --> Host) a file using the XMODEM protocol
 *
 * Arguments:
 *   - xip - structure containing transfer info:
 *	  usecrc       OUT - use 16bit-CRC instead of 8bit-checksum.
 *    verify       X    
 *	  dataddr:     IN  - The file to upload is stored here.
 *	  size:        IN  - size of the file to upload in bytes. 
 *	  pktcnt:      OUT - number of frames transfered.
 *	  errcnt:      x   
 *	  firsterrat:  x
 *
 * Returns:
 *    On success: something non-negative.
 *    On error: something negative.
 *      XERR_GEN : General error
 *      XERR_TOUT : Timeout
 *      XERR_CAN : Operation canceled
 *      XERR_RETR : Maximum retransmitions reached
 */

static int
Xup (struct xinfo *xip, int proto)
{
	int r, done, c, count, usecrc;
	ulong dataddr;
	int pktlen;
	ulong sno;
	long size;

	Mtrace("--Xup--");

	do { /* dummy */

		/* Synchronize */

		usecrc = 0;
		done = sync_tx();
		if ( done < 0 ) {
			break;
		}
		if ( done == 'C' ) {
			usecrc = 1;
		}

		/* Send data */
		
		Mtrace("t[");
		
		sno = 1;
		xip->pktcnt = 0;
		dataddr = xip->dataddr;
		size = xip->size;
		pktlen = xip->onek ? PKTLEN_1K : PKTLEN_128;
			
		done = 0;
		do {
			r = sendPacket((uchar *)dataddr, pktlen, sno, usecrc);
			if ( r < 0 ) {
				done = r;
			} else {
				sno++;
				xip->pktcnt++;
				size -= pktlen;
				dataddr += pktlen;
				if ( size <= 0 ) {
					done = 1;
				}
			}
		} while ( !done );

		Mtrace("]");

		if (done < 0) {
			break;
		}
		
		/* Send EOT, and wait for ACK */
		
		count = RETR_MAX;
		do {
			Mtrace("E");
			rputchar(EOT);
			c = waitchar(FRAME_TMO);
			Mtrace("<%02x>", c);
		} while ( --count && c != ACK);
		
		/* no matter if ACK was never received. 
		   consider the deed done. */

	} while (0);

	if ( proto == XMODEM ) {
		/* trash any final garbage from the transmiter */
		waitclear();
	}

	return done;
}

/***************************************************************************/

/* Yup():
 *
 * Upload (uMon --> Host) a file using the YMODEM protocol
 *
 * Arguments:
 *   - yip - structure containing transfer info:
 *    usecrc       x
 *	  onek         IN  - use 1K blocks (frames) instead of 128bytes ones
 *    verify       x    
 *	  baseaddr:    IN  - The first file to upload is stored here.
 *    flags        x
 *    info         x
 *    filecnt      IN  - Number of files to upload
 *                 OUT - Number of files successfully uploaded     
 *    fname[i]     IN  - Name of the i'th file to upload.
 *	  size[i]:     IN  - size of the i'th file to upload in bytes.
 *                 OUT - set to the TFS file-size if TFS support is included.
 *	  pktcnt[i]:   OUT - nr of frames transfered when uploading i'th file.
 *	  errcnt:      x
 *	  firsterrat:  x
 *
 * Returns:
 *    On success: something non-negative.
 *    On error: something negative.
 *      XERR_GEN : General error
 *      XERR_TOUT : Timeout
 *      XERR_CAN : Operation canceled
 *      XERR_RETR : Maximum retransmitions reached
 */

static int
Yup (struct yinfo *yip)
{
	TFILE *tfp;
	int r, rval, done, cfn;
	ulong dataddr;
	uchar *tmppkt;

	rval = 0;
	tmppkt = pktbuff;
	dataddr = yip->baseaddr;

	do { /* dummy */

		/* 
		 * send the files 
		 */

		for (cfn = 0; cfn < yip->filecnt; cfn++) {
						
#if INCLUDE_TFS
			/* read in the file from TFS */
			if (! (tfp = tfsstat(yip->fname[cfn])) ) {
				waitclear();
				doCAN();
				rval = XERR_NOFILE;
				break;
			}
			memcpy((char *)(dataddr), TFS_BASE(tfp), tfp->filsize);
			/* override given size */
			yip->size[cfn] = tfp->filsize;
#endif

			/* Synchronize with the receiver */
			done = sync_tx();
			if ( done < 0 ) {
				rval = done;
				break;
			}
			if ( done != 'C' ) {
				waitclear();
				doCAN();
				rval = XERR_SYNC;
				break;
			}

			/* Prepare and send header block */
			Mtrace("h[");
			memset(tmppkt, 0, PKTLEN_128);
			sprintf(tmppkt, "%s", yip->fname[cfn]);
			sprintf(tmppkt + strlen(tmppkt) + 1,
					"%ld", yip->size[cfn]);

			r = sendPacket(tmppkt, PKTLEN_128, 0, 1);
			if ( r < 0 ) {
				rval = r;
				break;
			}

			/* prepare an xinfo structure */
			xif.dataddr = dataddr;
			xif.size = yip->size[cfn];
			xif.onek = yip->onek;
			
			/* send the file data */
			rval = Xup(&xif, YMODEM);
			if ( rval < 0 ) {
				waitclear();
				doCAN();
				break;
			}

#if INCLUDE_TFS
			dataddr += yip->size[cfn];
#endif

		} /* of files loop */

		if ( rval < 0 )
			break;

		/*
		 * Send the footer
		 */
		
		/* synchronize with the receiver */
		done = sync_tx();
		if ( done < 0 ) {
			rval = done;
			break;
		}
		if ( done != 'C' ) {
			waitclear();
			doCAN();
			rval = XERR_SYNC;
			break;
		}
				
		/* Prepare and send the footer block */
		Mtrace("f[");
		memset(tmppkt, 0, PKTLEN_128);
		r = sendPacket(tmppkt, PKTLEN_128, 0, 1);
		if ( r < 0 ) {
			rval = r;
			break;
		}

		Mtrace("]");
		
	} while (0);

	yip->filecnt = cfn;

	/* trash any final garbage from the transmiter */
	waitclear();

	return rval;
}

#endif /* of INCLUDE_XMODEM */
