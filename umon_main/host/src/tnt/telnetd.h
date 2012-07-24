/* Telnet command encoding...
 * These codes only have meaning if preceded by the TELNET_IAC.
 * See Comer TCP/IP pg 413.
 */
#define TELNET_IAC		((char)255)	/* interpret as command */
#define TELNET_DONT		((char)254)	/* denial of request to perform some action */
#define TELNET_DO		((char)253)	/* approval to allow specified option */
#define TELNET_WONT		((char)252)	/* refusal to perform specified option */
#define TELNET_WILL		((char)251)	/* agreement to perform specified option */
#define TELNET_SB		((char)250)	/* start of option sub-negotiation */
#define TELNET_GA		((char)249)	/* the "go ahead" signal */
#define TELNET_EL		((char)248)	/* the "erase line" signal */
#define TELNET_EC		((char)247)	/* the "erase character" signal */
#define TELNET_AYT		((char)246)	/* the "are you there?" signal */
#define TELNET_AO		((char)245)	/* the "abort output" signal */
#define TELNET_IP		((char)244)	/* the "interrupt process" signal */
#define TELNET_BRK		((char)243)	/* the "break" signal */
#define TELNET_DMARK	((char)242)	/* data-stream portion of a synch */
#define TELNET_NOP		((char)241)	/* no operation */
#define TELNET_SE		((char)240)	/* end of option sub-negotiation */
#define TELNET_EOR		((char)239)	/* end of record */


/* Telnet options.  For more details refer to:
 * "Internet Official Protocol Standards" (STD 1).
   0     Binary Transmission                                 [RFC856]
   1     Echo                                                [RFC857]
   2     Reconnection                                      [NIC50005]
   3     Suppress Go Ahead                                   [RFC858]
   4     Approx Message Size Negotiation                   [ETHERNET]
   5     Status                                              [RFC859]
   6     Timing Mark                                         [RFC860]
   7     Remote Controlled Trans and Echo                    [RFC726]
   8     Output Line Width                                 [NIC50005]
   9     Output Page Size                                  [NIC50005]
  10     Output Carriage-Return Disposition                  [RFC652]
  11     Output Horizontal Tab Stops                         [RFC653]
  12     Output Horizontal Tab Disposition                   [RFC654]
  13     Output Formfeed Disposition                         [RFC655]
  14     Output Vertical Tabstops                            [RFC656]
  15     Output Vertical Tab Disposition                     [RFC657]
  16     Output Linefeed Disposition                         [RFC658]
  17     Extended ASCII                                      [RFC698]
  18     Logout                                              [RFC727]
  19     Byte Macro                                          [RFC735]
  20     Data Entry Terminal                         [RFC1043,RFC732]
  21     SUPDUP                                       [RFC736,RFC734]
  22     SUPDUP Output                                       [RFC749]
  23     Send Location                                       [RFC779]
  24     Terminal Type                                      [RFC1091]
  25     End of Record                                       [RFC885]
  26     TACACS User Identification                          [RFC927]
  27     Output Marking                                      [RFC933]
  28     Terminal Location Number                            [RFC946]
  29     Telnet 3270 Regime                                 [RFC1041]
  30     X.3 PAD                                            [RFC1053]
  31     Negotiate About Window Size                        [RFC1073]
  32     Terminal Speed                                     [RFC1079]
  33     Remote Flow Control                                [RFC1372]
  34     Linemode                                           [RFC1184]
  35     X Display Location                                 [RFC1096]
  36     Environment Option                                 [RFC1408]
  37     Authentication Option                              [RFC2941]
  38     Encryption Option                                  [RFC2946]
  39     New Environment Option                             [RFC1572]
  40     TN3270E                                            [RFC1647]
  41     XAUTH                                              [Earhart]
  42     CHARSET                                            [RFC2066]
  43	 Telnet Remote Serial Port (RSP)                     [Barnes]
  44     Com Port Control Option                            [RFC2217]
  45     Telnet Suppress Local Echo                           [Atmar]
  46     Telnet Start TLS                                       [Boe]
  47     KERMIT                                             [RFC2840]
  48     SEND-URL                                             [Croft]
  49	 FORWARD_X					                         [Altman]
 50-137  Unassigned					                           [IANA]
 138     TELOPT PRAGMA LOGON				              [McGregory]
 139	 TELOPT SSPI LOGON                                [McGregory]
 140     TELOPT PRAGMA HEARTBEAT                          [McGregory]
 255     Extended-Options-List                               [RFC861]
 */

#define TELNET_OPTION_ECHO		((char)1)
#define TELNET_OPTION_TERMTYPE	((char)24)
#define TELNET_OPTION_LINEMODE	((char)34)

struct sockinfo {
	struct		sockaddr_in	from;
	int			socket;
	int			warning;
	pthread_t	handle;
} SockTbl[32];


extern int telnetd(char *, char *, short, int);
extern void servercleanup(void);
extern void processIAC(int);
extern int net_write(char *buffer, int len);
