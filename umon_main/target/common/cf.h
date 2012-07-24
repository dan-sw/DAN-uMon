/* cf.h:
 * Header file supporting compact flash command.
 */

#ifndef CF_BLKSIZE
#define CF_BLKSIZE	512
#endif


/* These two functions must be supplied by the port-specific code.
 */
int	cfInit(int interface,int verbose);
int	cfRead(int interface,char *buf,int bknum,int bkcnt,int verbose);
int	cfWrite(int interface,char *buf,int bknum,int bkcnt,int verbose);

