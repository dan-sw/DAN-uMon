/* xmodem.c:
 */
#include "config.h"
#include "genlib.h"
#include "stddefs.h"
#include "cli.h"
#include "timer.h"

#if INCLUDE_XMODEM

/* struct xinfo:
 * Used to contain information pertaining to the current transaction.
 * The structure is built by the command Xmodem, then passed to the other
 * support functions (Xup, Xdown, etc..) for reference and update.
 */
struct xinfo {
	uchar	sno;			/* Sequence number. */
	uchar	pad;			/* Unused, padding. */
	int		xfertot;		/* Running total of transfer. */
	int		pktlen;			/* Length of packet (128 or 1024). */
	int		pktcnt;			/* Running tally of number of packets processed. */
	int		filcnt;			/* Number of files transferred by ymodem. */
	long	size;			/* Size of upload. */
	ulong	flags;			/* Storage for various runtime flags. */
	ulong	base;			/* Starting address for data transfer. */
	ulong	dataddr;		/* Running address for data transfer. */
	int		errcnt;			/* Keep track of errors (used in verify mode). */
	char	*firsterrat;	/* Pointer to location of error detected when */
							/* transfer is in verify mode. */
};

/* Runtime flags: */
#define VERIFY	(1<<1)

/* Current xmodem operation: */
#define XNULL	0
#define XUP		1
#define XDOWN	2

/* X/Ymodem protocol: */
#define SOH		0x01
#define STX		0x02
#define EOT		0x04
#define ACK		0x06
#define NAK		0x15
#define CAN		0x18
#define EOF		0x1a
#define ESC		0x1b

#define PKTLEN_128	128
#define PKTLEN_1K	1024

#define BYTE_TIMEOUT	1000

#if INCLUDE_XUP
static int Xup(struct xinfo *);
static int putPacket(uchar *,struct xinfo *);
#endif
static int Xdown(struct xinfo *);
static int getPacket(uchar *,struct xinfo *);

char *XmodemHelp[] = {
	"Xmodem file transfer",
	"-[s:uv] {address}",
	"Options:",
	" -u {size}  transfer is upload (must specify size)",
	" -v         verify only",
	"Notes:",
	" * Default xfer direction is download.",
	(char *)0,
};

int
Xmodem(int argc,char *argv[])
{
	struct	xinfo xi;
	int	opt, xop;
	char	*info, *flags;

	xop = XDOWN;
	info = (char *)0;
	flags = (char *)0;
	xi.size = 0;
	xi.flags = 0;
	xi.filcnt = 0;
	xi.pktlen = 128;
	xi.base = xi.dataddr = 0xffffffff;
	while ((opt=getopt(argc,argv,"u:v")) != -1) {
		switch(opt) {
#if INCLUDE_XUP
		case 'u':
			xi.size = strtol(optarg,(char **)0,0);
			xop = XUP;
			break;
#endif
		case 'v':
			xi.flags |= VERIFY;
			break;
		default:
			return(CMD_PARAM_ERROR);
		}
	}

	/* There should one argument after the option list.
	 */
	if (argc != optind+1)
		return(CMD_PARAM_ERROR);

	xi.dataddr = xi.base = strtoul(argv[optind],(char **)0,0);

#if INCLUDE_XUP
	if (xop == XUP) {
		Xup(&xi);
	} else
#endif
	if (xop == XDOWN) {
		long	tmpsize;

		tmpsize = (long)Xdown(&xi);
		if ((!xi.size) || (tmpsize < 0))
			xi.size = tmpsize;
	}
	else
		return(CMD_PARAM_ERROR);
	return(CMD_SUCCESS);
}

#if INCLUDE_XUP
/* putPacket():
 * Used by Xup to send packets.
 */
static int
putPacket(uchar *tmppkt, struct xinfo *xip)
{
	int		c, i;
	uchar	*cp;
	ushort	chksm;

	cp = (uchar *)&chksm;
	chksm = 0;
	
	if (xip->pktlen == PKTLEN_128)
		target_putchar(SOH);
	else
		target_putchar(STX);

	target_putchar((char)(xip->sno));
	target_putchar((char)~(xip->sno));

	for(i=0;i<xip->pktlen;i++) {
		target_putchar((char)*tmppkt);
		chksm = ((chksm+*tmppkt++)&0xff);
	}
	target_putchar((char)(chksm&0x00ff));
	c = getchar();			/* Wait for ack */

	/* If pktcnt == -1, then this is the first packet sent by
	 * YMODEM (filename) and we must wait for one additional
	 * character in the response.
	 */
	if (xip->pktcnt == -1)
		c = getchar();
	return(c);
}
#endif

/* getPacket():
 * Used by Xdown to retrieve packets.
 */
static int
getPacket(uchar *tmppkt, struct xinfo *xip)
{
	char	*pkt;
	int		i,rcvd;
	uchar	csum, xcsum, seq[2];

	if ((rcvd = getbytes((char *)seq,2)) != 2) {
		target_putchar(NAK);
		return(0);
	}

	if (xip->flags & VERIFY) {
		rcvd = getbytes((char *)tmppkt,xip->pktlen);
		if (rcvd != xip->pktlen) {
			target_putchar(NAK);
			return(0);
		}
		for(i=0;i<xip->pktlen;i++) {
			if (tmppkt[i] != ((char *)xip->dataddr)[i]) {
				if (xip->errcnt++ == 0)
					xip->firsterrat = (char *)(xip->dataddr+i);
			}
		}
		pkt = (char *)tmppkt;
	}
	else {
		rcvd = getbytes((char *)xip->dataddr,xip->pktlen);
		if (rcvd != xip->pktlen) {
			target_putchar(NAK);
			return(0);
		}
		pkt = (char *)xip->dataddr;
	}


	xcsum = (uchar)getchar();
	csum = 0;
	for(i=0;i<xip->pktlen;i++)
		csum += *pkt++;
	if (csum != xcsum) {
		target_putchar(NAK);
		return(0);
	}

	/* Test the sequence number compliment...
	 */
	if ((uchar)seq[0] != (uchar)~seq[1]) {
		target_putchar(NAK);
		return(0);
	}

	/* Verify that the incoming sequence number is the expected value...
	 */
	if ((uchar)seq[0] !=  xip->sno) {
		/* If the incoming sequence number is one less than the expected
		 * sequence number, then we assume that the sender did not recieve
		 * our previous ACK, and they are resending the previously received
		 * packet.  In that case, we send  ACK and don't process the
		 * incoming packet...
	 	 */
		if ((uchar)seq[0] == xip->sno-1) {
			target_putchar(ACK);
			return(0);
		}

		/* Otherwise, something's messed up...
		 */
		target_putchar(CAN);
		return(-1);
	}

	xip->dataddr += xip->pktlen;
	xip->sno++;
	xip->pktcnt++;
	xip->xfertot += xip->pktlen;
	target_putchar(ACK);
	return(0);
}

#if INCLUDE_XUP
/* Xup():
 * Called when a transfer from target to host is being made (considered
 * an upload).
 */
static int
Xup(struct xinfo *xip)
{
	uchar	c;
	int		done;
	long	actualsize;

	actualsize = xip->size;

	if (xip->size & 0x7f) {
		xip->size += 128;
		xip->size &= 0xffffff80L;
	}

	printf("Upload %ld bytes from 0x%lx\n",xip->size,(ulong)xip->base);

	/* Startup synchronization... */
	/* Wait to receive a NAK or 'C' from receiver. */
	done = 0;
	while(!done) {
		c = (uchar)getchar();
		switch(c) {
		case NAK:
			done = 1;
			break;
		case 'q':	/* ELS addition, not part of XMODEM spec. */
			return(0);
		default:
			break;
		}
	}

	done = 0;
	xip->sno = 1;
	xip->pktcnt = 0;
	while(!done) {
		c = (uchar)putPacket((uchar *)(xip->dataddr),xip);
		switch(c) {
		case ACK:
			xip->sno++;
			xip->pktcnt++;
			xip->size -= xip->pktlen;
			xip->dataddr += xip->pktlen;
			break;
		case NAK:
			break;
		case CAN:
			done = -1;
			break;
		case EOT:
			done = -1;
			break;
		default:
			done = -1;
			break;
		}
		if (xip->size <= 0) {
			target_putchar(EOT);
			getchar();	/* Flush the ACK */
			break;
		}
	}

	return(0);
}
#endif


/* Xdown():
 * Called when a transfer from host to target is being made (considered
 * an download).
 */

static int
Xdown(struct xinfo *xip)
{
	int		done;
	char	c, *tmppkt;
    struct elapsed_tmr tmr;
	static char pkt[PKTLEN_128];

	tmppkt = pkt;
	xip->sno = 0x01;
	xip->pktcnt = 0;
	xip->errcnt = 0;
	xip->xfertot = 0;
	xip->firsterrat = 0;

	/* Startup synchronization... */
	/* Continuously send NAK or 'C' until sender responds. */
	while(1) {
		target_putchar(NAK);

    	startElapsedTimer(&tmr,1000);
		while(!gotachar()) {
    		if(msecElapsed(&tmr))
				break;
		}
		if (gotachar())
			break;
	}

	done = 0;
	while(done == 0) {
		c = (char)getchar();
		switch(c) {
		case SOH:				/* 128-byte incoming packet */
			xip->pktlen = 128;
			done = getPacket((uchar *)tmppkt,xip);
			break;
		case CAN:
			done = -1;
			break;
		case EOT:
			target_putchar(ACK);
			done = xip->xfertot;
			printf("\nRcvd %d pkt%c (%d bytes)\n",xip->pktcnt,
					xip->pktcnt > 1 ? 's' : ' ',xip->xfertot);
			break;
		case ESC:		/* User-invoked abort */
			done = -1;
			break;
		case EOF:		/* 0x1a sent by MiniCom, just ignore it. */
			break;
		default:
			done = -1;
			break;
		}
	}
	if (xip->flags & VERIFY) {
		if (xip->errcnt)
			printf("%d errors, first at 0x%lx\n",
				xip->errcnt,(ulong)(xip->firsterrat));
		else
			printf("verification passed\n");
	}
	return(done);
}


#endif
