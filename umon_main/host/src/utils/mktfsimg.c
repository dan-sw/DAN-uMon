/* mktfsimg.c:
 * This host program is used to create TFS images that can be flashed
 * to micromonitor target flash space.
 *
 * General notice:
 *  This code is part of a boot-monitor package developed as a generic base
 *  platform for embedded system designs.  As such, it is likely to be
 *  distributed to various projects beyond the control of the original
 *  author.  Please notify the author of any enhancements made or bugs found
 *  so that all may benefit from the changes. In addition, notification back
 *  to the author will allow the new user to pick up changes that may have
 *  been made by other users after this version of the code was distributed.
 *
 * Author: Joshua Henderson <joshua.henderson@pgicorp.net>
 *
 * To use it, create a spec file in the following format specifying what
 * files to put in the image and look over the command line options:
 *
 * # <destination>                           <source>
 * startlinux,eB                             startlinux
 * zImage                                    zImage-2.6.24.4
 * rootfs.img                                rootfs.cramfs
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef BUILD_WITH_VCC
extern char *optarg;
extern int optind;
//#include <io.h>
#else
#ifndef O_BINARY
#define O_BINARY 0
#endif
#include <unistd.h>
#endif

#include "version.h"

/* Note that these two files (tfs.h and tfsprivate.h) are included in the
 * host tools directory only for convenience (so that the host directory can
 * be isolated from the target directory).  The files tfs.h and tfsprivate.h
 * in this space should be identical to those in target/common.
 */
#include "tfs.h"
#include "tfsprivate.h"
#include "utils.h"

char *tfs_in_ram, *end_of_tfs_in_ram;
int debug, verbose, partition_size;
unsigned long flashBase,tfs_erasesize = 0x20000;
int endianSwap = 1;

char* usage_txt[] = {
	"Usage: mktfsimg [-b:e:hsvV] {size} {list} {image}",
	"Options:",
	"   -b {addr}       flash partition address",
	"   -e {size}       flash erase size in bytes (default 0x20000)",
	"   -h              generate this help message",
	"   -s              disable endian swap on portions of TFS struct",
	"   -S              generate sample spec file",
	"   -v              additive verbosity",
	"   -V              show version (build date) of tool",
	0,
};

struct tfsflg tfsflgtbl[] = {
	{ TFS_BRUN,		'b',	"run_at_boot",			TFS_BRUN },
	{ TFS_QRYBRUN,	'B',	"qry_run_at_boot",		TFS_QRYBRUN },
	{ TFS_EXEC,		'e',	"executable",			TFS_EXEC },
	{ TFS_SYMLINK,	'l',	"symbolic link",		TFS_SYMLINK },
	{ TFS_EBIN,		'E',	"exec-binary",			TFS_EBIN },
	{ TFS_IPMOD,	'i',	"inplace_modifiable",	TFS_IPMOD },
	{ TFS_UNREAD,	'u',	"ulvl_unreadable", 		TFS_UNREAD },
	{ TFS_ULVL1,	'1',	"ulvl_1", 				TFS_ULVLMSK },
	{ TFS_ULVL2,	'2',	"ulvl_2", 				TFS_ULVLMSK },
	{ TFS_ULVL3,	'3',	"ulvl_3", 				TFS_ULVLMSK },
	{ TFS_CPRS,		'c',	"compressed", 			TFS_CPRS },
	{ 0, 0, 0, 0 }
};

/* crc32tab[]:
 *	Used for calculating a 32-bit CRC.
 */
static unsigned long crc32tab[] = {
	0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F,
	0xE963A535, 0x9E6495A3, 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
	0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91, 0x1DB71064, 0x6AB020F2,
	0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
	0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9,
	0xFA0F3D63, 0x8D080DF5, 0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
	0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B, 0x35B5A8FA, 0x42B2986C,
	0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
	0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423,
	0xCFBA9599, 0xB8BDA50F, 0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
	0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D, 0x76DC4190, 0x01DB7106,
	0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
	0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D,
	0x91646C97, 0xE6635C01, 0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
	0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457, 0x65B0D9C6, 0x12B7E950,
	0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
	0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7,
	0xA4D1C46D, 0xD3D6F4FB, 0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
	0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9, 0x5005713C, 0x270241AA,
	0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
	0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81,
	0xB7BD5C3B, 0xC0BA6CAD, 0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
	0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683, 0xE3630B12, 0x94643B84,
	0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
	0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB,
	0x196C3671, 0x6E6B06E7, 0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
	0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5, 0xD6D6A3E8, 0xA1D1937E,
	0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
	0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55,
	0x316E8EEF, 0x4669BE79, 0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
	0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F, 0xC5BA3BBE, 0xB2BD0B28,
	0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
	0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F,
	0x72076785, 0x05005713, 0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
	0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21, 0x86D3D2D4, 0xF1D4E242,
	0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
	0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69,
	0x616BFFD3, 0x166CCF45, 0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
	0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB, 0xAED16A4A, 0xD9D65ADC,
	0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
	0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693,
	0x54DE5729, 0x23D967BF, 0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
	0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

static unsigned long
crc32(unsigned char *buffer,unsigned long nbytes)
{
	unsigned long crc_rslt, temp;

	crc_rslt = 0xffffffff;
	while(nbytes) {
		temp = (crc_rslt ^ *buffer++) & 0x000000FFL;
		crc_rslt = ((crc_rslt >> 8) & 0x00FFFFFFL) ^ crc32tab[temp];
		nbytes--;
	}
	return(~crc_rslt);
}

/* tfsflagsbtoa():
 * Convert the incoming flag value to a basic ASCII string.
 */
char *
tfsflagsbtoa(long flags,char *fstr)
{
	int	i;
	struct	tfsflg	*tfp;

	if ((!flags) || (!fstr))
		return((char *)0);

	i = 0;
	tfp = tfsflgtbl;
	*fstr = 0;
	while(tfp->sdesc) {
		if ((flags & tfp->mask) == tfp->flag)
			fstr[i++] = tfp->sdesc;
		tfp++;
	}
	fstr[i] = 0;
	return(fstr);
}

/* tfshdrcrc():
 * This function takes a pointer to a tfs header and runs the same
 * crc32 routine on the header that is used by TFS.
 */
unsigned long
tfshdrcrc(TFILE *hdr)
{
	unsigned long	crc;
	TFILE hdrcpy;

	hdrcpy = *hdr;
	hdrcpy.next = 0;
	hdrcpy.hdrcrc = 0;
	hdrcpy.flags |= (TFS_NSTALE | TFS_ACTIVE);
	crc = crc32((unsigned char *)&hdrcpy,TFSHDRSIZ);
	return(crc);
}

/* showHdr():
 * Dump the content of a TFS header.
 */
static void
showHdr(struct tfshdr *hdr)
{
	char buf[128], *state;

	buf[0] = 0;
	printf("   hdrsize: %d%s\n", hdr->hdrsize,
		hdr->hdrsize == TFSHDRSIZ ? "" : " (bad hdr size)");

	printf("   hdrvrsn: 0x%04x\n", hdr->hdrvrsn);
	printf("   filsize: %ld\n",hdr->filsize);
	tfsflagsbtoa(hdr->flags,buf);
	if (TFS_DELETED(hdr))
		state = "(deleted)";
	else if (TFS_STALE(hdr))
		state = "(stale)";
	else
		state = "";
	printf("   flags:   0x%08lx %s %s\n",hdr->flags,buf,state);
	printf("   hdrcrc:  0x%08lx\n",hdr->hdrcrc);
	printf("   filcrc:  0x%08lx\n",hdr->filcrc);
	printf("   modtime: 0x%08lx\n",hdr->modtime);
	printf("   next:    0x%08lx\n",(long)hdr->next);
	printf("   name:    <%s>\n",hdr->name);
	printf("   info:    <%s>\n",hdr->info);
	printf("   rsvd[0]: 0x%08lx\n",hdr->rsvd[0]);
	printf("   rsvd[1]: 0x%08lx\n",hdr->rsvd[1]);
	printf("   rsvd[2]: 0x%08lx\n",hdr->rsvd[2]);
	printf("   rsvd[3]: 0x%08lx\n",hdr->rsvd[3]);
}

/* nexthdr():
* Given a pointer to a TFS file header, this function returns the location
* of the next TFS file header.
*/
static TFILE *
nexthdr(TFILE *tfp)
{
	unsigned long	next;

	next = (unsigned long)(tfp + 1);
	next += tfp->filsize;
	if (next & 0xf)
		next = (next | 0xf) + 1;
	return((TFILE *)next);
}

/* eob_check():
 * Check to see if the incoming 'next' pointer is passed the incoming
 * 'end' pointer.  If yes, then this indicates that the next file
 * (or header) is beyond the end of the partition.  This indicates one
 * of two likely problems:
 * 1. The allocated partition is smaller than the space used by TFS.
 * 2. TFS space is corrupted.
 */
int
eob_check(char *next, char *end, int hdr)
{
	char *str;

	if (hdr)
		next += TFSHDRSIZ;

	if (next >= end) {
		if (hdr)
			str = "Header";
		else
			str = "File";

		fprintf(stderr,"\nError: %s passes end of partition.\n",str);
		fprintf(stderr,"Hint : TFS space may be larger than allocated\n");
		fprintf(stderr,"       partition or TFS space may be corrupted.\n");
		return(-1);
	}
	return(0);
}

/* get_physical_tfs_base():
 * If we have the first TFS header, then we can determine the physical
 * base of TFS in flash space by subtracting the size of the first file
 * plus the size of the header from the location of the next file
 * (all members of the TFS header).
 */
unsigned long
get_physical_tfs_base(char *tfsbase)
{
	TFILE *tfp;
	unsigned long physical_tfs_base;

	tfp = (TFILE *)tfsbase;
	if (tfshdrcrc(tfp) != tfp->hdrcrc)
		return(0xffffffffL);

	physical_tfs_base = (unsigned long)tfp->next - tfp->filsize - tfp->hdrsize;
	physical_tfs_base &= 0xfffffff0;
	return(physical_tfs_base);
}


/* get_file_offset():
 * Given a filename, return the offset of that file's header within the
 * TFS space.  If file doesn't exist, return -1.
 * To have this code also support the ability to return the offset into
 * TFS space at which the next file will be added, if the filename is
 * NULL, then we return the offset at which the NEXT file could be added.
 */
long
get_file_offset(char *filename)
{
	int match = 0;
	TFILE *nxttfp, *tfp;

	/* First we have to find the file in TFS...
	*/
	nxttfp = tfp = (struct tfshdr *)tfs_in_ram;
	while(1) {
		if (tfp->hdrsize == 0xffff) {
			if (filename == 0)
				match = 1;
			break;
		}

		nxttfp = nexthdr(tfp);

		if (TFS_DELETED(tfp)) {
			tfp = nxttfp;
			continue;
		}

		if (eob_check((char *)nxttfp, end_of_tfs_in_ram,0) < 0)
			break;

		if (tfshdrcrc(tfp) != tfp->hdrcrc) {
			fprintf(stderr,"\n\nHeader CRC failure.\n");
			fprintf(stderr,"Terminating at offset 0x%lx\n",
				(unsigned long)tfp-(unsigned long)tfs_in_ram);
			break;
		}

		if ((tfp->filsize > partition_size) || (tfp->filsize < 0) ||
			(crc32((unsigned char *)(tfp+1),tfp->filsize) != tfp->filcrc)) {
			fprintf(stderr,"\n\nFile CRC failure.\n");
			fprintf(stderr,"Terminating at offset 0x%lx\n",
				(unsigned long)tfp-(unsigned long)tfs_in_ram);
			break;
		}

		if (filename) {
			if (strcmp(filename,TFS_NAME(tfp)) == 0) {
				match = 1;
				break;
			}
		}

		tfp = nxttfp;
		if (eob_check((char *)nxttfp, end_of_tfs_in_ram,1) < 0)
			break;
	}

	/* If the file was found, return the offset; else return -1.
	*/
	if (match) {
		return((char *)tfp - tfs_in_ram);
	}
	else {
		return(-1);
	}
}

void do_swap()
{
	TFILE *tfp;
	TFILE old;
	tfp = (struct tfshdr *)tfs_in_ram;
	while(tfp->hdrsize != 0xffff) {
		unsigned long next;

		memcpy(&old,tfp,sizeof(old));

		if (!TFS_DELETED(tfp)) {
			tfp->hdrsize = otherEnd16(endianSwap,tfp->hdrsize);
			tfp->filsize = otherEnd32(endianSwap,tfp->filsize);
			tfp->filcrc = otherEnd32(endianSwap,tfp->filcrc);
			tfp->flags = otherEnd32(endianSwap,tfp->flags);
			tfp->hdrvrsn = otherEnd16(endianSwap,tfp->hdrvrsn);
			tfp->hdrcrc = otherEnd32(endianSwap,tfp->hdrcrc);
			tfp->modtime = otherEnd32(endianSwap,tfp->modtime);
			tfp->next = (struct tfshdr *)otherEnd32(endianSwap,
								(unsigned long)(tfp->next));
		}

        /* get the location of the next header */
		next = (unsigned long)(tfp + 1);
		next += old.filsize;
		if (next & 0xf)
		    next = (next | 0xf) + 1;
		tfp = (TFILE *)next;
	}
}

/* get_file_tot():
 * Return the number of active files in TFS.
 */
int
get_file_tot(void)
{
	int ftot = 0;
	TFILE *tfp;

	tfp = (struct tfshdr *)tfs_in_ram;
	while(tfp->hdrsize != 0xffff) {
		if (!TFS_DELETED(tfp))
			ftot++;
		tfp = nexthdr(tfp);
	}
	return(ftot);
}

/* tfsname(), tfsflags() & tfsinfo():
 * Taking the incoming string and parse it for one of the fields
 * (name, flags or info).  The syntax of the incoming fullname is...
 *
 * 			"name,flags,info"
 *
 *	where flags and/or info can be empty.
 * The tfsname() function parses the comma-delimted string and loads the
 * name member of the incoming TFILE structure with the name.
 * The tfsinfo() and tfsflags() functions do essentially the same thing
 * on their respective field (info and flags).
 */
char *
tfsname(char *fullname, TFILE *tfp)
{
	char copy[TFSNAMESIZE+1], *from, *to;

	strcpy(copy,fullname);
	from = copy;

	to = tfp->name;
	while(1) {
		if ((*from == ',') || (*from == 0)) {
			*to = 0;
			break;
		}
		*to++ = *from++;
	}
	return(tfp->name);
}

long
tfsflags(char *fullname, TFILE *tfp)
{
	struct tfsflg *flgp;
	char copy[TFSNAMESIZE+1], *from, *comma;

	strcpy(copy,fullname);
	fullname = copy;

	/* Skip to characters after second comma (if any)...
	*/
	comma = strchr(fullname,',');
	if (!comma) {
		tfp->flags = 0;
		return(tfp->flags);
	}
	from = comma+1;
	tfp->flags = 0;

	while((*from) && (*from != ',')) {
		flgp = tfsflgtbl;
		while(flgp->sdesc) {
			if (*from == flgp->sdesc) {
				tfp->flags |= flgp->flag;
				break;
			}
			flgp++;
		}
		if (!flgp->flag) {
			return(-1);
		}
		from++;
	}
	return(tfp->flags);
}

char *
tfsinfo(char *fullname, TFILE *tfp)
{
	char copy[TFSNAMESIZE+1], *from, *to, *comma;

	strcpy(copy,fullname);
	fullname = copy;

	/* Skip to characters after second comma (if any)...
	*/
	comma = strchr(fullname,',');
	if (!comma) {
		tfp->info[0] = 0;
		return(tfp->info);
	}
	comma = strchr(comma+1,',');
	if (!comma) {
		tfp->info[0] = 0;
		return(tfp->info);
	}

	from = comma+1;
	to = tfp->info;
	while(1) {
		if (*from == 0) {
			*to = 0;
			break;
		}
		*to++ = *from++;
	}
	return(tfp->info);
}

void create_image(char* image_filename)
{
	int fd;

	if (verbose)
		fprintf(stderr,"Creating image %s ...\n",image_filename);

	fd = open(image_filename,O_RDWR|O_CREAT|O_BINARY|O_TRUNC, 0666);
	if (fd >= 0) {
		if (write(fd,tfs_in_ram,partition_size) != partition_size) {
			fprintf(stderr,"Failed to write image file '%s'\n",image_filename);
			exit(1);
		}
	}
	close(fd);
}

void add_file(char* src, char* dst)
{
	long offset;
	struct stat fstat;
	int ifd, rc;
	char* filedata;

	if (verbose)
		printf("Adding file %s from %s...\n",dst,src);

	if (strlen(dst) >= TFSNAMESIZE) {
		fprintf(stderr,"TFS filename '%s' too long\n",dst);
		exit(1);
	}

	/* Get size of file to be copied to TFS and determine
	* if there is enough space in TFS to add the file...
	*/
	if (stat(src,&fstat) < 0) {
		fprintf(stderr,"Can't find file '%s'\n",src);
		exit(1);
	}

	if ((ifd = open(src,O_RDONLY | O_BINARY)) < 0) {
		fprintf(stderr,"Failed to open file '%s'\n",src);
		exit(1);
	}

	if ((filedata = malloc(fstat.st_size)) == 0) {
		fprintf(stderr,"malloc\n");
		exit(1);
	}

	if ((rc = read(ifd,filedata,fstat.st_size)) != fstat.st_size) {
		fprintf(stderr,"Failed to read file '%s' (%d %d)\n",src,rc,fstat.st_size);
		exit(1);
	}

	/* If file exists, force user to remove it (don't do it
	* automatically)...
	*/
	offset = get_file_offset(dst);
	if (offset != -1) {
		fprintf(stderr,"File '%s' already exists in TFS\n",dst);
		exit(1);
	}
	else {
		int ftot, sectortot, rsvd;
		TFILE tfshdr;
		unsigned long next, state_table_overhead;

		offset = get_file_offset(0);
		ftot = get_file_tot();

		/* Make sure there's enough space in TFS to write
		* the file.  This includes making sure that DSI space
		* is available, plus normal data space...
		*/
		sectortot = (partition_size / tfs_erasesize) - 1;
		state_table_overhead = ((ftot+1) * DEFRAGHDRSIZ) +
			sectortot * sizeof(struct sectorcrc);

		if (state_table_overhead > tfs_erasesize) {
			fprintf(stderr,"DSI space maxed out\n");
			exit(1);
		}

		if ((offset + TFSHDRSIZ + fstat.st_size + 16) >
			(partition_size - tfs_erasesize - state_table_overhead)) {
			fprintf(stderr,"Not enough space left in TFS\n");
			exit(1);
		}

		/* Build the header...
		*/
		memset((char *)&tfshdr,0,TFSHDRSIZ);
		tfsname(dst,&tfshdr);
		tfsinfo(dst,&tfshdr);
		tfsflags(dst,&tfshdr);
		if (tfshdr.flags == -1)
			tfshdr.flags = 0;
		tfshdr.flags |= (TFS_ACTIVE | TFS_NSTALE);
		if (!(tfshdr.flags & TFS_IPMOD))
		    tfshdr.filcrc = crc32((unsigned char*)filedata,fstat.st_size);
		else
		    tfshdr.filcrc = 0xffffffff;

		tfshdr.hdrsize = TFSHDRSIZ;
		tfshdr.hdrvrsn = TFSHDRVERSION;
		tfshdr.filsize = fstat.st_size;
		tfshdr.modtime = 0xffffffff;
		for(rsvd=0;rsvd<TFS_RESERVED;rsvd++)
			tfshdr.rsvd[rsvd] = 0xffffffff;
		tfshdr.next = 0;
		tfshdr.hdrcrc = 0;
		tfshdr.hdrcrc = crc32((unsigned char *)&tfshdr,TFSHDRSIZ);
		next = flashBase+offset+TFSHDRSIZ+fstat.st_size;
		if (next & 0xf)
			next = (next | 0xf) + 1;
		tfshdr.next = (TFILE *)next;

		/* copy the file information to the partition */
		memcpy(tfs_in_ram + (offset),&tfshdr,TFSHDRSIZ);
		memcpy(tfs_in_ram + (offset+TFSHDRSIZ),filedata,fstat.st_size);

		if (verbose > 1)
			showHdr(&tfshdr);
	}

	free(filedata);
	close(ifd);
}

void
specFile(void)
{
	printf("#\n");
	printf("# Example mktfsimg specfile\n");
	printf("#\n");
	printf("\n");
	printf("# <destination (in TFS)>      <source (on host)>\n");
	printf("startlinux,eB                 startlinux\n");
	printf("zImage                        zImage-2.6.24.4\n");
	printf("rootfs.img                    rootfs.cramfs\n");
	exit(0);
}

int main(int argc, char *argv[])
{
	FILE* in;
	char line[256];
	int lnum = 0;
	long reloc;

	int opt, rc;
	char *image_filename,*spec_filename;

	verbose = rc = 0;

	while((opt=getopt(argc,argv,"dhe:sSb:vV")) != EOF) {
	switch(opt) {
		case 'd':
			debug = 1;
			break;
		case 'h':
			usage(0);
			break;
		case 'e':
            tfs_erasesize = strtoul(optarg,(char**)0,0);
		    break;
        case 'S':
            specFile();
            break;
        case 's':	/* disable endian swap */
            endianSwap = 0;
            break;
        case 'b':
            flashBase = strtoul(optarg,(char **)0,0);
            break;
        case 'v':
            verbose++;
            break;
        case 'V':
            showVersion();
            break;
        default:
            usage(0);
		}
	}

	if (argc < optind+3)
		usage(0);

    partition_size = strtoul(argv[optind++],(char **)0,0);
	spec_filename = argv[optind++];
	image_filename = argv[optind++];

	/* Allocate space for local storage of the file, but make sure that
	 * the base address of the data is 16-byte aligned...
	 */
	if ((tfs_in_ram = malloc(partition_size + 64)) == 0) {
		fprintf(stderr,"malloc\n");
		exit(1);
	}

	end_of_tfs_in_ram = tfs_in_ram + partition_size + 64;

	/* Make sure the base of the allocated space starts on a 16-byte
	 * boundary (makes things easier for TFS stuff because TFS assumes
	 * it is aligned that way).
	 */
	tfs_in_ram += 32;
	reloc = (long)tfs_in_ram;
	reloc &= ~0xf;
	tfs_in_ram = (char*)reloc;

	/* set entire partition to 0xFF */
	memset(tfs_in_ram,0xFF,partition_size);

	if (verbose > 1)
		fprintf(stderr,"TFS Flash Base: 0x%lx\n",flashBase);

	in = fopen(spec_filename, "r");
	if (in == NULL) {
		fprintf(stderr,"Failed to open specfile '%s'\n",spec_filename);
		exit(1);
	}

	while(fgets(line,sizeof(line),in) != NULL)
	{
		lnum++;
		if (strlen(line)) {
			static const char* SPEC_FILE_DELIMS = "\n\r\t ";
			char* dst = strtok(line,SPEC_FILE_DELIMS);
			if (dst) {
				char* src;

				if (dst[0] == '#')
					continue;

				src = strtok(NULL,SPEC_FILE_DELIMS);

				if (src && dst)
					add_file(src,dst);
				else {
					fprintf(stderr,"invalid specfile line at %s:%d\n",
						image_filename,lnum);
					exit(1);
				}
			}
		}
	}

	do_swap();
	create_image(image_filename);

	return 0;
}
