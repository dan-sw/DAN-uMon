/* flash.c:
 *	This file contains the portions of the flash code that are device
 *	independent.  Refer to the appropriate device sub-directory for the
 *	code that is specific to the flash device on the target.
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
#if INCLUDE_FLASH
#include "flash.h"
#include "genlib.h"
#include "ctype.h"
#include "stddefs.h"
#include "cli.h"

extern struct flashdesc FlashNamId[];

int		CurrentFlashPage;
int		FlashTrace;
int		FlashCurrentBank;
int		sectortoaddr(int,int *,uchar **);

/* FlashBank[]:
 *	This table contains all of the information that is needed to keep the
 *	flash code somewhat generic across multiple flash devices.
 */
struct	flashinfo FlashBank[FLASHBANKS];

/* showlockop():
 * Return a string that verbosely represents the flash lock
 * operation...
 */
char *
showlockop(int operation)
{
	switch(operation) {
		case FLASH_LOCK:
			return("lock");
		case FLASH_UNLOCK:
			return("unlock");
		case FLASH_LOCKDWN:
			return("lock_down");
		case FLASH_LOCKQRY:
			return("lock_qry");
		case FLASH_LOCKABLE:
			return("lockable");
		default:
			return("???");
	}
}

#if INCLUDE_FLASHINFO
/* showflashtype():
 *	Find a match between the incoming id and an entry in the FlashNamId[]
 *	table.  The FlashNamId[] table is part of the device-specific code.
 */
int
showflashtype(ulong id, int showid)
{
	struct flashdesc *fdp;

	if (showid)
		printf("Flash ID: 0x%lx\n",id);

	fdp = FlashNamId;
	while(fdp->desc) {
		if (id == fdp->id) {
			printf("Device = %s\n",fdp->desc);
			return(0);
		}
		fdp++;
	}
	if (id == FLASHRAM) {
		printf("Device = FLASH-RAM\n");
		return(0);
	}
	printf("Flash id 0x%lx not recognized\n",id);
	return(-1);
}

void
showflashtotal(void)
{
#if FLASHBANKS > 1
	printf("Total of %d banks (0-%d), current default bank: %d\n",
		FLASHBANKS,FLASHBANKS-1,FlashCurrentBank);
#else
	printf("Current flash bank: 0\n");
#endif
}

/* showflashinfo():
 * Dump information about specified flash device.
 */
int
showflashinfo(char *devrange)
{
	uchar	*base;
	struct	sectorinfo *sp;
	struct	flashinfo *fdev;
	char	*range;
	int		first_sector_of_device, devtot;
	int		rc, locksupported, sector, lastsector, hdrprinted;

	rc = 0;
	devtot = 0;
	hdrprinted = 0;
	lastsector = lastflashsector();

	/* An incoming NULL range implies "any" sector...
	 */
	if (devrange == 0) {
		showflashtotal();
		range = "any";
	}
	else {
		range = devrange;
	}

	/* Loop through all sectors on all devices, picking only those
	 * sectors that fall in the specified range. 
	 */
	for(sector = 0; sector <= lastsector; sector++) {
		if (!inRange(range,sector))
			continue;

		if (!(fdev = snumtofdev(sector)))
			return(-1);

		first_sector_of_device = fdev->sectors[0].snum;

		if (flashlock(first_sector_of_device,FLASH_LOCKABLE) > 0)
			locksupported = 1;
		else
			locksupported = 0;

		/* If sector is first sector of a device, then print
		 * device info also...
		 */
		if ((sector == first_sector_of_device) && (!strcmp(range,"any"))) {
			if (showflashtype(fdev->id,0) < 0)
				return(-1);
			printf(" Bank width  : %d\n",fdev->width);
			printf(" Sectors     : %d\n",fdev->sectorcnt);
			printf(" Base addr   : 0x%08lx\n",(ulong)(fdev->base));
			hdrprinted = 0;
			devtot++;
		}

		if (!hdrprinted) {
			printf(" Sctr     Begin       End         Size    %s  %s",
				"Erased?", locksupported ? "  Locked?\n" : "\n");
			hdrprinted = 1;
		}

		sp = &fdev->sectors[sector - first_sector_of_device];
		sectortoaddr(sp->snum,0,&base);
		if ((range == 0) || inRange(range,sp->snum)) {
			printf(" %3d    0x%08lx  0x%08lx  0x%06lx   %s      ",
				sp->snum,(ulong)(sp->begin), (ulong)(sp->end), sp->size,
				flasherased(sp->begin,sp->end) ? "yes" : " no");
			if (locksupported) {
				switch(flashlock(sp->snum,FLASH_LOCKQRY)) {
					case -1:
						printf("???");
						break;
					case 1:
						printf("yes");
						break;
					case 0:
						printf(" no");
						break;
				}
			}
			printf("\n");
			if (gotachar())
				break;
		}
	}

	return(0);
}
#endif

#ifdef FLASH_COPY_TO_RAM

/* flashopload():
 *	Copy flash operation to ram space.  
 *	Note that this function assumes that cache is disabled at this point.
 *	This is important because we are copying text into bss space and if
 *	cache was on, there could be a coherency problem.
 */
int
flashopload(ulong *begin,ulong *end,ulong *copy,int size)
{
	volatile ulong	*bp;
	int	ret;

	/* Verify space availability: */
	if (((int)end - (int)begin) >= size) {
		printf("flashopload overflow ((0x%lx-0x%lx) > 0x%x)\n",
			(ulong)end,(ulong)begin,size);
		return(-1);
	}

	/* Initially fill the copy space with 0xff so that the space
	 * remaining is viewable...
	 */
	memset((char *)copy,0xff,size);

	ret = 0;
	/* Copy function() to RAM, then verify: */
	bp = begin;
	while(bp <= end) {
		*copy = *bp;
		if (*copy++ != *bp++) {
			printf("flashopload failed\n");
			ret = -1;
			break;
		}
	}

	return(ret);
}

#endif

/* flashtype():
 *	Use the device-specific function pointer to call the routine
 *	relocated to RAM space.
 */
int
flashtype(struct flashinfo *fdev)
{
	return(fdev->fltype(fdev));
}

/* flasherased():
 * Return 1 if range of memory is all 0xff; else 0.
 * Scan through the range of memory specified by begin-end (inclusive)
 * looking for anything that is not 0xff.  Do this in three sections so
 * that the pointers can be 4-byte aligned for the bulk of the comparison
 * range...
 * The beginning steps through as a char pointer until aligned on a 4-byte
 * boundary.  Then do ulong * comparisons until the just before the end
 * where we once again use char pointers to align on the last few
 * non-aligned bytes (if any).
 */

int
flasherased(uchar *begin, uchar *end)
{
	ulong *lp, *lp1, ltmp;

	/* If begin is greater than end, the range is illegal.  The only
	 * exception to this is the case where end may be zero.  This is
	 * considered an exception because in cases where we are dealing
	 * with the last sector of a device that sits at the end of memory
	 * space, the end point will wrap.
	 */
	if ((begin > end) && (end != 0)) {
		printf("flasherased(): bad range\n");
		return(0);
	}

	WATCHDOG_MACRO;

	/* Get pointers aligned so that we can do the bulk of the comparison
	 * with long pointers...
	 */
	while(((long)begin & 3) && (begin <= end)) {
		if (*begin != 0xff)
			return(0);
		begin++;
	}
	if (begin > end)
		return(1);

	lp = (ulong *)begin;
	ltmp = (ulong)end;
	ltmp &= ~3;
	lp1 = (ulong *)ltmp;

	while(lp != lp1) {
		if (*lp != 0xffffffff) 
			return(0);
		lp++;
	}
	if (lp > (ulong *)end)
		return(1);
	
	begin = (uchar *)lp;
	do {
		if (*begin != 0xff) 
			return(0);
	} while(begin++ != end);
	return(*end == 0xff);
}

/* flasherase():
 *	Use the device-specific function pointer to call the routine
 *	relocated to RAM space.
 *	Note that flasherase() is called with a sector number.  The sector
 *	number is relative to the entire system, not just the particular device.
 *	This means that if there is more than one flash device in the system that
 *	the actual sector number (relative to the device) may not be the same
 *	value.  This adjustment is made here so that the underlying code that is
 *	pumped into ram for execution does not have to be aware of this.
 * Return...
 *  1 if successful
 * -1 if failure
 *  0 if sector is protected or locked
 */
int
flasherase(int snum)
{
	uchar *tmp;
	ulong *base, *end;
	int	size, rc, dev_snum;
	struct flashinfo *fdev;
	struct sectorinfo *sinfo;

	if (!(fdev = snumtofdev(snum)))
		return(-1);
	
	/* If the device type is RAM, the erase is a bit different...
	 */
	if (fdev->id == FLASHRAM) {
		// Use 'tmp' here to eliminate a 3.4 toolset warning.
		sectortoaddr(snum,&size,&tmp);
		base = (ulong *)tmp;
		end = base + (size/sizeof(long));
		while(base < end) {
			*base = 0xffffffff;
			if (*base != 0xffffffff)
				return(-1);
			base++;
		}
		return(1);
	}

	/* If the sector is soft-protected or locked, return negative
	 * and print failure.  If the sector is already erased, then
	 * there is no need to issue the device-specific erase algorithm.
	 */
	dev_snum = snum - fdev->sectors[0].snum;
	sinfo = &fdev->sectors[dev_snum];
	if (!flasherased(sinfo->begin,sinfo->end)) {
			
		if (flashlocked(snum,1)) 
			return(0);

		rc = fdev->flerase(fdev,dev_snum);
		if (rc < 0)
			return(rc);
	}
	return(1);
}


/* flashwrite():
 *	Use the device-specific function pointer to call the routine
 *	relocated to RAM space.
 *	First make a few checks on the request, then write to flash if all
 *	checks succeed.
 */
int
flashwrite(struct flashinfo *fdev,uchar *dest,uchar *src,long bytecnt)
{
	int	rc, first_sector_of_device;
	register uchar	*dp, *sp, *edp;

	if (FlashTrace)
		printf("flashwrite(0x%lx,0x%lx,%ld)\n",(long)dest,(long)src,bytecnt);

	if (fdev->id == FLASHRAM) {
		uchar *sp, *dp, *end;
		sp = src;
		dp = dest;
		end = dp+bytecnt;
		while(dp < end) {
			*dp = *sp;
			if (*dp != *sp)
				return(-1);
			dp++; sp++;
		}
		return(0);
	}

	dp = dest;
	sp = src;
	edp = (dest + bytecnt) - 1;
	first_sector_of_device = fdev->sectors[0].snum;

	/* If outside the devices space, return failed.. */
	if ((edp < fdev->sectors[0].begin) ||
	    (dp > fdev->sectors[fdev->sectorcnt-1].end)) {
		printf("flashwrite() failed: dest out of flash range\n");
		return(-1);
	}

	/* Make sure that there is no attempt to transition a bit
	 * in the affected range from 0 to 1...  A flash write can only
	 * bring bits low (erase brings them  high).
	 */
	while(dp < edp) {
		if ((*dp & *sp) != *sp) {
			printf("flashwrite(0x%lx) failed: bit 0->1 rqst denied.\n",
				(long)dp);
			return(-1);
		}
		dp++; 
		sp++;
	}
	rc = fdev->flwrite(fdev,dest,src,bytecnt);
	if (rc < 0)
		return(rc);

	/* Assuming everything else appears to have passed, make sure the
	 * source and destination addresses match...
	 */
	if (memcmp((char *)dest,(char *)src,(int)bytecnt) != 0) {
		printf("flashwrite() post-verify failed.\n");
		return(-1);
	}
	return(0);
}

/* flashewrite():
 *	Use the device-specific function pointer to call the routine
 *	relocated to RAM space.
 */
int
flashewrite(uchar *dest,uchar *src,long bytecnt)
{
	struct flashinfo *fdev;

	if ((fdev = addrtobank(dest)) == 0)
		return(-1);

	/* Source and destination addresses must be long-aligned. */
	if (((int)src & 3) || ((int)dest & 3))
		return(-1);

	return(fdev->flewrite(fdev,dest,src,bytecnt));
}

/* flashlock():
   Use a function pointer to call the routine relocated to RAM space.
*/
int
flashlock(int snum,int operation)
{
	int dev_snum;
	struct flashinfo *fdev;

	fdev = snumtofdev(snum);
	dev_snum = snum - fdev->sectors[0].snum;
	return(fdev->fllock(fdev,dev_snum,operation));
}

int
flashlocked(int snum, int verbose)
{
	if ((flashlock(snum,FLASH_LOCKABLE) > 0) &&
		(flashlock(snum,FLASH_LOCKQRY) == 1)) {
			if (verbose)
				printf("Sector %d locked\n",snum);
			return(1);
	}
	return(0);
}

/* snumtofdev():
 * Return the flash device pointer that corresponds to the incoming
 * sector number.
 */
struct flashinfo *
snumtofdev(int snum)
{
	int dev;
	struct flashinfo *fbnk;

	fbnk = FlashBank;
	for(dev=0;dev<FLASHBANKS;dev++,fbnk++) {
		if ((snum >= fbnk->sectors[0].snum) &&
			(snum <= fbnk->sectors[fbnk->sectorcnt-1].snum))
			return(fbnk);
	}
	printf("snumtfdev(%d) failed\n",snum);
	return(0);
}

/* addrtosector():
 *	Incoming address is translated to sector number, size of sector
 *	and base of sector.
 *	Return 0 if successful; else -1.
 */
int
addrtosector(uchar *addr,int *sector,int *size,uchar **base)
{
	struct flashinfo *fbnk;
	struct	sectorinfo *sinfo;
	int		dev, sec, i;

	sec = 0;
	fbnk = FlashBank;
	for(dev=0;dev<FLASHBANKS;dev++,fbnk++) {
		for(i=0;i<fbnk->sectorcnt;i++,sec++) {
			sinfo = &fbnk->sectors[i];
			if ((addr >= sinfo->begin) && (addr <= sinfo->end)) {
				if (sector) {
					*sector = sec;
				}
				if (base) {
					*base = sinfo->begin;
				}
				if (size) {
					*size = sinfo->size;
				}
				return(0);
			}
		}
	}
	printf("addrtosector(0x%lx) failed\n",(ulong)addr);
	return(-1);
}

/* addrtobank():
 *	From the incoming address, return a pointer to the flash bank that
 *	this address is within.
 */
struct flashinfo *
addrtobank(uchar *addr)
{
	struct flashinfo *fbnk;
	int		dev;

	fbnk = FlashBank;
	for(dev=0;dev<FLASHBANKS;dev++,fbnk++) {
		if ((addr >= fbnk->base) && (addr <= fbnk->end))
			return(fbnk);
	}
	printf("addrtobank(0x%lx) failed\n",(ulong)addr);
	return(0);
}

int
sectortoaddr(int sector,int *size,uchar **base)
{
	struct flashinfo *fbnk;
	struct	sectorinfo *sinfo;
	int		dev, sec, i;

	sec = 0;
	fbnk = FlashBank;
	for(dev=0;dev<FLASHBANKS;dev++,fbnk++) {
		for(i=0;i<fbnk->sectorcnt;i++,sec++) {
			if (sec == sector) {
				sinfo = &fbnk->sectors[i];
				if (base) *base = sinfo->begin;
				if (size) *size = sinfo->size;
				return(0);
			}
		}
	}
	printf("sectortoaddr(%d) failed\n",sector);
	return(-1);
}

/* AppFlashWrite():
 *	Takes in a source, destination and byte count and converts that to
 *	the appropriate flashwrite() call.  This function supports the possibility
 *	of having one write request span across multiple devices in contiguous
 *	memory space.
 */
int
AppFlashWrite(uchar *dest,uchar *src,long bytecnt)
{
	int		ret;
	long	tmpcnt;
	struct flashinfo *fbnk;

	ret = 0;
	while(bytecnt > 0) {
		fbnk = addrtobank((uchar *)dest);
		if (!fbnk)
			return(-1);
	
		if ((dest + bytecnt) <= fbnk->end)
			tmpcnt = bytecnt;
		else
			tmpcnt = (fbnk->end - dest) + 1;
	
		ret = flashwrite(fbnk,dest,src,tmpcnt);
		if (ret < 0) {
			printf("AppFlashWrite(0x%lx,0x%lx,%ld) failed (%d)\n",
				(ulong)dest,(ulong)src,bytecnt,ret);
			break;
		}
		src += tmpcnt;
		dest += tmpcnt;
		bytecnt -= tmpcnt;
	}
	return(ret);
}

int
lastflashsector(void)
{
	int	lastsnum;
	struct	flashinfo *lastfbnk;

	lastfbnk = &FlashBank[FLASHBANKS-1];
	lastsnum = lastfbnk->sectors[lastfbnk->sectorcnt-1].snum;
	return(lastsnum);
}

char *FlashHelp[] = {
	"Flash memory operations",
	"{op} [args]",
	"Ops...",
#if INCLUDE_FLASHINFO
	"  info {rnge}",
#endif
	"  lock/unlock/erase {rnge}",
	"  write/ewrite {dest} {src} {byte_cnt}",
	0,
};

/* FlashCmd():
 *	Code that handles the user interface.  See FlashHelp[] below for usage.
 */
int
FlashCmd(int argc,char *argv[])
{
	int		ret;
	ulong	dest, src;
	long	bytecnt, rslt;
	struct	flashinfo *fbnk;

	fbnk = &FlashBank[FlashCurrentBank];
	ret = CMD_SUCCESS;

	if (strcmp(argv[1],"init") == 0) {
		FlashInit();
	}
#if INCLUDE_FLASHINFO
	else if (strcmp(argv[1],"info") == 0) {
		showflashinfo(argv[2]);
	}
#endif
	else if (strcmp(argv[1],"ewrite") == 0) {
		if (argc == 5) {
			dest = strtoul(argv[2],(char **)0,0);
			src = strtoul(argv[3],(char **)0,0);
			bytecnt = (long)strtoul(argv[4],(char **)0,0);
			rslt = flashewrite((uchar *)dest,(uchar *)src,bytecnt);
			if (rslt < 0) {
				printf("ewrite failed (%ld)\n",rslt);
				ret = CMD_FAILURE;
			}
		}
		else
			ret = CMD_PARAM_ERROR;
	}
	else if (!strcmp(argv[1],"write")) {
		if (argc == 5) {
			dest = strtoul(argv[2],(char **)0,0);
			src = strtoul(argv[3],(char **)0,0);
			bytecnt = (long)strtoul(argv[4],(char **)0,0);
			rslt = AppFlashWrite((uchar *)dest,(uchar *)src,bytecnt);
			if (rslt < 0) {
				printf("Write failed (%ld)\n",rslt);
				ret = CMD_FAILURE;
			}
		}
		else
			ret = CMD_PARAM_ERROR;
	}
	else if (!strcmp(argv[1],"erase")) {
		if (argc != 3) {
			ret = CMD_PARAM_ERROR;
		}
		else {
			uchar *base;
			int	rc, snum, size, stot = 0;

			if (strncmp(argv[2],"0x",2) == 0) {
				ulong begin, end;
				char *dash = strchr(argv[2],'-');

				begin = end = strtoul(argv[2],0,0);
				if (dash)
					end = strtoul(dash+1,0,0);

				while(begin <= end) {
					if (addrtosector((uchar *)begin,&snum,&size,&base) < 0)
						break;
					begin = (ulong)base;
					rc = flasherase(snum);
					if (rc != 1) {
						printf("Erase failed (%d)\n",rc);
						ret = CMD_FAILURE;
						break;
					}
					stot++;
					begin += size;
				}
			}
			else {
				int	last;

				last = lastflashsector();
				for(snum=0;snum<=last;snum++) {
					int rc;

					if ((argv[2] == 0) || inRange(argv[2],snum)) {
						ticktock();
						rc = flasherase(snum);
						if (rc != 1) {
							printf("Erase failed (%d)\n",rc);
							ret = CMD_FAILURE;
							break;
						}
						stot++;
					}
				}
			}
			printf("%d sectors erased\n",stot);
		}
	}
	else if ((!strcmp(argv[1],"lock")) || (!strcmp(argv[1],"unlock")) ||
		(!strcmp(argv[1],"lockdwn"))) {
		int	operation, snum;

		if (!strcmp(argv[1],"lock")) 
			operation = FLASH_LOCK;
		else if (!strcmp(argv[1],"unlock"))
			operation = FLASH_UNLOCK;	
		else
			operation = FLASH_LOCKDWN;

		if (argc == 2) {				
#ifdef FLASH_PROTECT_RANGE
			argv[2] = FLASH_PROTECT_RANGE;
			argc = 3;					
			printf("Applying %s to sector(s) %s...\n",argv[1],argv[2]);
#else
			printf("Monitor not built with specified protection range\n");
			ret = CMD_FAILURE;
#endif
		}
	
		if (argc != 3) {
			ret = CMD_PARAM_ERROR;
		}
		else {
			int	last;
			struct flashinfo *fdev;

			last = lastflashsector();
			for(snum=0;snum<=last;snum++) {
				if (inRange(argv[2],snum)) {
					ticktock();
					if ((fdev = snumtofdev(snum)) == 0) {
						ret = CMD_FAILURE;
						break;
					}
					if (flashlock(fdev->sectors[0].snum,FLASH_LOCKABLE) <= 0) {
						printf("Sector %d does not support %s\n",snum,argv[1]);
						ret = CMD_FAILURE;
						break;
					}
					rslt = flashlock(snum,operation);
					if (rslt < 0) {
						printf("%s failed (%ld) at sector %d\n",
							argv[1],rslt,snum);
						ret = CMD_FAILURE;
						break;
					}
				}
			}
		}
	}
	else {
		ret = CMD_PARAM_ERROR;
	}
	return(ret);
}

/* Used as a placeholder for the flash drivers that don't
 * support flash lock...
 */
int
FlashLockNotSupported(struct flashinfo *fdev,int snum,int operation)
{
	if (operation == FLASH_LOCKABLE)
		return(0);
	else
	return(-1);
}

/* sectorProtect():
 *  Set or clear (based on value of protect) the protected flag for the
 *  specified range of sectors...
 *  This supports incoming ranges that can be dash and/or comma delimited.
 *  For example a range can be "0", "0-3", or "0,2-4", etc...
 */
int
sectorProtect(char *range, int protect)
{
	struct  flashinfo *fbnk;
	int i, dev, snum;

	snum = 0;
	for(dev = 0;dev < FLASHBANKS;dev++) {
		fbnk = &FlashBank[dev];
		for(i = 0;i < fbnk->sectorcnt;i++,snum++) {
			if ((range == 0) || (*range == 0) || inRange(range,snum))
				fbnk->sectors[i].protected = protect;
		}
	}
	return(0);
}

#endif
