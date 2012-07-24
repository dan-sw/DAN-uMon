/* tfsclean2.c:
 *	This version of defragmentation is not power-hit safe and does not
 *	require any flash overhead.  The defragmentation simply copies all
 *	good files to an allocated block of ram erases the flash, then copies
 *	the concatenated data back to the flash.  Simple but dangerous.
 *
 *	If automatic defragmentation (through tfsadd()) is to be used in this
 *	mode, then the application must reside in ram space that is above 
 *	APPRAMSTART + SIZEOF_TFSFLASH.  This version of defragmentation assumes
 *	that the ram space needed for defrag will start at APPRAMBASE.
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
	ulong	appramstart;
	TFILE	*tfp, *lasttfp;
	uchar	*tbuf, *cp1, *cp2;
	int		dtot, nfadd, len, chkstat;

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

	printf("TFS device '%s' non-powersafe defragmentation\n",tdp->prefix);

	tbuf = (uchar *)appramstart;
	lasttfp = tfp = (TFILE *)(tdp->start);
	nfadd = tdp->start;
	while(validtfshdr(tfp)) {
		if (TFS_FILEEXISTS(tfp)) {
			len = TFS_SIZE(tfp) + sizeof(struct tfshdr);
			if (len % TFS_FSIZEMOD)
				len += TFS_FSIZEMOD - (len % TFS_FSIZEMOD);
			nfadd += len;
			if (s_memcpy((char *)tbuf,(char *)tfp,len,0,0) != 0)
				return(TFSERR_MEMFAIL);
			
			((struct tfshdr *)tbuf)->next = (struct tfshdr *)nfadd;
			tbuf += len;
		}
		lasttfp = tfp;
		tfp = nextfp(tfp,tdp);
	}

	/* We've now copied all of the active files from flash to ram.
	 * Now we want to see how much of the flash space needs to be
	 * erased.  We only need to erase the sectors that have changed...
	 */
	cp1 = (uchar *)tdp->start;
	cp2 = (uchar *)appramstart;
	while(cp2 < tbuf) {
		if (*cp1 != *cp2)
			break;
		cp1++; cp2++;
	}
#if INCLUDE_FLASH
	if ((cp2 != tbuf) || (!TFS_FILEEXISTS(lasttfp))) {
		int first, last;
		
		if (addrtosector(cp1,&first,0,0) == -1)
			return(TFSERR_FLASHFAILURE);
		
		if (addrtosector((uchar *)tdp->end,&last,0,0) == -1)
			return(TFSERR_FLASHFAILURE);
		printf("Erasing sectors %d-%d...\n",first,last);
		while(first<=last) {  // DAN: bug fixed  '<=' istead of '='
			if (flasherase(first++) == 0)
				return(TFSERR_FLASHFAILURE);
		}
	}
#endif

	/* Copy data placed in RAM back to flash: */
	printf("Restoring flash...\n");
	if (TFS_DEVTYPE_ISRAM(tdp)) {
		memcpy((char *)(tdp->start),(char *)appramstart,
			(tbuf-(uchar*)appramstart));
	}
	else {
#if INCLUDE_FLASH
		int err;

		err = AppFlashWrite((uchar *)(tdp->start),(uchar *)appramstart,
			(tbuf-(uchar*)appramstart));
		if (err < 0)
#endif
			return(TFSERR_FLASHFAILURE);
	}

	/* All defragmentation is done, so verify sanity of files... */
	chkstat = tfscheck(tdp,verbose);

	return(chkstat);
}
#endif
