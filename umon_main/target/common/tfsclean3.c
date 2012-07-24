/* tfsclean3.c:
 *
 *	--- NOT READY YET ---
 *
 *	This version of defragmentation is power-hit safe and requires
 *	that there be double the amount of flash as is needed for use by
 *	TFS.  The basic idea is similar to tfsclean2.c...
 *  Copy all of the good files over to the "other" flash bank, then have
 *	TFS use the "other" bank as the storage area.
 *	The idea is that the defrag is simply a copy of the good stuff to
 *	the alternate flash block.  This requires that after the
 *	good stuff is copied, the now-dirty flash block must be erased in
 *	the background prior to the next tfsclean() call.  The fact that
 *	there is no sector erase is what makes this faster.
 *
 *	If both of these flash banks are in the same flash device, then
 *	having a background erase in progress means that it must be an
 * 	interruptible erase (device specific).  This is necessary because
 *	while the background erase is in progress there may be a need to
 *	interact with the flash and most devices don't let you do both at the
 *	same time.
 *	
 *	Note that this "background-erase" is what makes this method the
 *	fastest defrag method.  It does require that the erase operation be
 *	interruptible, and it requires that the application will provide the
 *	hooks to do this...
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
 */
#include "config.h"
#include "cpu.h"
#include "stddefs.h"
#include "genlib.h"
#include "tfs.h"
#include "tfsprivate.h"
#include "flash.h"
#include "monflags.h"

#if INCLUDE_TFS

int
tfsfixup(int verbose, int dontquery)
{
	return(TFSERR_NOTAVAILABLE);
}

#if DEFRAG_TEST_ENABLED
int
dumpDhdr(DEFRAGHDR *dhp)
{
	return(TFSERR_NOTAVAILABLE);
}

int
dumpDhdrTbl(DEFRAGHDR *dhp, int ftot)
{
	return(TFSERR_NOTAVAILABLE);
}
#endif


/* _tfsclean():
 *	This is an alternative to the complicated defragmentation above.
 *	It simply scans through the file list and copies all valid files
 *	to RAM; then flash is erased and the RAM is copied back to flash.
 *  <<< WARNING >>>
 *  THIS FUNCTION SHOULD NOT BE INTERRUPTED AND IT WILL BLOW AWAY
 *  ANY APPLICATION CURRENTLY IN CLIENT RAM SPACE.
 */
int
_tfsclean(TDEV *tdp, int notused, int verbose)
{
	TFILE	*tfp;
	uchar	*tbuf;
	ulong	appramstart;
	int		dtot, nfadd, len, err, chkstat;

	if (TfsCleanEnable < 0)
		return(TFSERR_CLEANOFF);

	appramstart = getAppRamStart();

	/* Determine how many "dead" files exist. */
	dtot = 0;
	tfp = (TFILE *)tdp->start;
	while(validtfshdr(tfp)) {
		if (!TFS_FILEEXISTS(tfp))
			dtot++;
		tfp = nextfp(tfp,tdp);
	}

	if (dtot == 0)
		return(TFS_OKAY);

	printf("Reconstructing device %s with %d dead file%s removed...\n",
		tdp->prefix, dtot,dtot>1 ? "s":"");

	tbuf = (char *)appramstart;
	tfp = (TFILE *)(tdp->start);
	nfadd = tdp->start;
	while(validtfshdr(tfp)) {
		if (TFS_FILEEXISTS(tfp)) {
			len = TFS_SIZE(tfp) + sizeof(struct tfshdr);
			if (len % TFS_FSIZEMOD)
				len += TFS_FSIZEMOD - (len % TFS_FSIZEMOD);
			nfadd += len;
			if (s_memcpy(tbuf,(uchar *)tfp,len,0,0) != 0)
				return(TFSERR_MEMFAIL);
			
			((struct tfshdr *)tbuf)->next = (struct tfshdr *)nfadd;
			tbuf += len;
		}
		tfp = nextfp(tfp,tdp);
	}

	/* Erase the flash device: */
	err = _tfsinit(tdp);
	if (err != TFS_OKAY)
		return(err);

	/* Copy data placed in RAM back to flash: */
	err = AppFlashWrite((ulong *)(tdp->start),(ulong *)appramstart,
		(tbuf-(uchar*)appramstart));
	if (err < 0)
		return(TFSERR_FLASHFAILURE);

	/* All defragmentation is done, so verify sanity of files... */
	chkstat = tfscheck(tdp,verbose);

	return(chkstat);
}
#endif
