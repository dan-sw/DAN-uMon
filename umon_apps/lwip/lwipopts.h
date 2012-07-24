#define LWIP_NETCONN		0
#define LWIP_SOCKET			0
#define LWIP_SNMP			0
#define LWIP_STATS			0
#define NO_SYS				1
#define TCP_MSS				(1460)
#define TCP_SND_BUF			(TCP_MSS * 42)	/* can't exceed 64K */
#define TCP_SND_QUEUELEN	(8 * (TCP_SND_BUF/TCP_MSS))
#define TCP_WND				(24*1024)

#define MEM_ALIGNMENT	4


/**************************************************************************
 **************************************************************************
 *
 * The following text is taken from the lwip email archive.
 * It is a good summary of some of the things that can be done
 * to tune up TCP in LWIP.  Note I found that the biggest helper
 * was to increase the size of TCP_SND_BUF to be many times greater
 * than TCP_MSS (as stated below).
 *
On Fri, Nov 03, 2006 at 03:40:31PM +0100, Fr?d?ric BERNON wrote:
> Hi group,
> 

> - TCP_SND_BUF have to be greater than TCP_MSS if TCP_SND_QUEUELEN is defined 
> as (2 * TCP_SND_BUF/TCP_MSS). ("TCP_MSS > 1024 No connection" thread)

Ideally TCP_SND_BUF should be many times greater than TCP_MSS.

> - "TCP_SND_QUEUELEN... must be less than 256" ("Assertion : "tcp_enqueue: no 
> pbufs on queue => both queues empty"" thread)

This is due to the queuelen being stored in an 8 bit value.  Someone
posted a simple modification earlier in the week should you wish to
use a larger queuelen, but in practice most people will be limited by
the SND_BUF before the QUEUELEN plays any part.

> - TCP_FAST_INTERVAL seems to be a "good" value to reduce to get a minimum 
> delay, but increase cycles consume ("TCP performance in receiving" thread and 
> "tcp_tmr - TCP_TMR_INTERVAL" thread)

Be careful about setting this too small.  lwIP just counts the number
of times its timer is called and uses this to control time sensitive
operations (such as TCP retransmissions), rather than actually
measuring time using something more accurate.  If you call the timer
functions very frequently you may see things (such as retransmissions)
happening sooner than they should.

> - TCP_WND have to be big enough to avoid to reduce exchanges when this 
> "window" is full ("TCP performance in receiving" thread)

Yes.  Ideally TCP_WND should be your link bandwidth multiplied by the round 
trip time.

> - in tcp.c, tcp_recved, reduce TCP_WND/2 can improve perf, in  line "else if 
> (pcb->flags & TF_ACK_DELAY && pcb->rcv_wnd >= TCP_WND/2) {" ("TCP performance 
> in receiving" thread)

This will send window updates more often which will in some
circumstances improve performance, but could in others reduce it.
Sadly this is true of much in TCP - it's impossible to tune it for all
situations!

> - prioritize the API_MSG_RECV... if your mailbox interface supports this 
> ("TCP performance in receiving" thread)
>
> - MSS: 1460, TCP_WND: 24000, TCP_SND_BUF: 16*MSS, TCP_SND_QUEUELEN: 16, this 
> settings have increase perf form 9 to 12.9Mbps on a uCOS device ("About the 
> throughput of LWIP" thread)

Those all sound like pretty good, sensible values for the parameters.
THanks for posting your summary and results.  I'm sure others will
find it useful.
  
> - PBUF_POOL_BUFSIZE set to 2048 increase perf but consume more RAM ("About 
> the throughput of LWIP" thread)

Not sure about this, but can believe that's true.

> - TCP_WND have to be at least a couple of segments ("lwip connect to normal 
> socket applicationveryvery slowly" thread)
>

yep.

> - TCP_NODELAY disable the Nagle algorithm but not in raw layer. A quick 
> change in do_write() can also imcrease perf. ("Sending too slow" thread)

yep.

> - Sending two small packets (730 bytes if MSS is 1460) can give better perf 
> than 1, because ACK is forced to be send by peer. ("Long(er) delay to PC ACK 
> when serving images" thread)


That's a bit of a hack, and not particularly reliable (depends on the stack 
you're sending to).  Much better to configure your window size large enough so 
that TCP can send more than one packet before becoming window limited.
>  
> Do you see more "rules" to respect ? (I hope I don't have write silly things)

-1- For those who have to write a new ethernet driver (ethernetif.c) for a MAC 
which does not support DMA copy: creating/checking the checksums for IP/UDP/TCP 
can be done on-the-fly so you don't have to load the same data twice into the 
processor. This speeded things up for me, but it's not that easy to implement 
(and DMA copy might be faster anyway).

-2- When using the socket- (or callback-) interface, input data is copied from 
the pbufs into the application buffer using single byte copies. Depending on 
your platform, this can take much longer than copying with the width of your 
data bus. I implemented this change in api_lib.c/netbuf_copy_partial() and 
submitted a patch for this, but I don't think it's included in CVS for now (or 
will be :). But it did boost my receive performance.

-3- Although you probably already have done this, I think it's worth to mention 
again and again: The biggest performance boost (at least for me) came by 
selecting the best lwip_standard_chksum() in inet.c. (Of course this is only 
valid if you don't take option -1- as described above...)

This is a great summary for others who are trying to do the same thing.

 *
 *
 **************************************************************************
 **************************************************************************
 */
