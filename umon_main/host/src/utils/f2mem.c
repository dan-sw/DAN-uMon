/* f2mem.c:
 * This program takes as input a group of files and builds them into
 * one file that can be burned into embedded system memory.
 * The first file specified is assumed to be a binary file containing
 * the embedded system's monitor.  All remaining files are assumed to
 * be the TFS files that will be seen by the monitor as files in TFS.
 * 
 * General notice:
 * This code is part of a boot-monitor package developed as a generic base
 * platform for embedded system designs.  As such, it is likely to be
 * distributed to various projects beyond the control of the original
 * author.  Please notify the author of any enhancements made or bugs found
 * so that all may benefit from the changes.  In addition, notification back
 * to the author will allow the new user to pick up changes that may have
 * been made by other users after this version of the code was distributed.
 * 
 * Author:	Ed Sutter
 * email:	esutter@lucent.com
 * phone:	908-582-2351
 */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef BUILD_WITH_VCC
#else
#include <unistd.h>
#endif
#include "version.h"
#include "utils.h"

#ifdef BUILD_WITH_VCC
typedef unsigned short ushort;
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

typedef unsigned char uchar;

#define TYPE_ASCII		1
#define TYPE_BINARY		2
#define TYPE_MONITOR	3

#define MAXFILES		64
#define TFSNAMESIZE		23
#define TFSHDRSIZ		sizeof(struct tfshdr)

#define TFS_RESERVED		4

/* Flags: */
#define TFS_EXEC	0x00000001	/* 'e': Executable. */
#define TFS_BRUN	0x00000002	/* 'b': To be executed at boot. */
#define TFS_QRYBRUN	0x00000004	/* 'B': To be executed at boot if */
								/*      query passes. */
#define TFS_COFF	0x00000008	/* 'C': COFF absolute. */
#define TFS_ELF		0x00000010	/* 'E': ELF absolute. */
#define TFS_AOUT	0x00000020	/* 'A': AOUT absolute. */
#define TFS_CPRS	0x00000040	/* 'c': File is compressed. */
#define TFS_IPMOD	0x00000080	/* 'i': File is in-place modifiable. */
#define TFS_UNREAD	0x00000100	/* 'u':	File is not even readable if the */
								/*		user-level requirement is not met; */
								/*		else, it is read-only. */
#define TFS_ULVLMSK	0x00000600	/*	User level mask defines 4 access levels: */
#define TFS_ULVL0	0x00000000	/* '0'	level 0 */
#define TFS_ULVL1	0x00000200	/* '1'	level 1 */
#define TFS_ULVL2	0x00000400	/* '2'	level 2 */
#define TFS_ULVL3	0x00000600	/* '3'	level 3 */
#define TFS_AIPNOT	0x00000800	/* Append not in progress, invisible to user. */
								/* When this bit is clear, AIP is in	*/
								/* progress for the file. */
								/* See notes in tfsclose() for this. */
#define TFS_ACTIVE	0x00008000	/* Used to indicate that file is not deleted. */

struct tfsflg {
	long	flag;
	char	sdesc;
};

struct tfshdr {
	unsigned short	hdrsize;		/* Size of this header.					*/
	unsigned short	hdrvrsn;		/* Header version #.					*/
	long	filsize;				/* Size of the file.					*/
	long	flags;					/* Flags describing the file.			*/
	unsigned long  filcrc;			/* 32 bit CRC of file.					*/
	unsigned long  hdrcrc;			/* 32 bit CRC of header.				*/
	unsigned long	modtime;		/* Time when file was last modified.	*/
	struct	tfshdr	*next;			/* Pointer to next file in list.		*/
	char	name[TFSNAMESIZE+1];	/* Name of file.						*/
	char	info[TFSNAMESIZE+1];	/* Miscellaneous info field.			*/
	unsigned long	rsvd[TFS_RESERVED];
};

struct finfo {
	char *name;
	char *filecontent;
	char *flags;
	char *info;
	int	type;
	int	size;
};

struct tfsflg tfsflgtbl[] = {
	{ TFS_BRUN,			'b' },
	{ TFS_QRYBRUN,		'B' },
	{ TFS_EXEC,			'e' },
	{ TFS_AOUT,			'A' },
	{ TFS_COFF,			'C' },
	{ TFS_ELF,			'E' },
	{ TFS_IPMOD,		'i' },
	{ TFS_UNREAD,		'u' },
	{ TFS_ULVL0,		'0' },
	{ TFS_ULVL1,		'1' },
	{ TFS_ULVL2,		'2' },
	{ TFS_ULVL3,		'3' },
	{ TFS_CPRS,			'c' },
	{ 0, 0 }
};

char *usage_txt[] = {
	"f2mem (files to memory translator) is a tool that allows the user to",
	"take a group of files and generate a file that is suitable for a flash",
	"programmer.  The intent is to feed it a monitor binary followed by a",
	"list of files that are destined to exist in TFS (Tiny File System).",
	"The monitor binary is placed at the base of the memory, then, starting",
	"at the offset specified by -t, each of the remaining files are processed",
	"and made to look as if they were residing in flash memory under TFS.",
	"",
	"Usage: f2mem [options] ",
	" Options:",
	" -a tfsname   ascii file for TFS",
	" -B address   address flash base in CPU memory (default=0)",
	" -b tfsname   binary file for TFS",
	" -e           endian swap on portions of TFS struct",
	" -f hexbyte   byte to be used as filler (default=0xff)",
	" -m filename  monitor file",
	" -O 's|b'     output type: S3-record or binary (default=S3)",
	" -o filename  output file (default = mem.bin | mem.srec)",
	" -p padto     pad-to size (default is no padding; uses -f hexbyte as pad)",
	" -s {m|a}     swap bytes in monitor binary (m) or all bytes (a)",
	" -S address   s-record base address (default=0)",
	" -t ###       tfs offset",
	" -T ###       tfs header version (default = 1)",
	" -V           show version",
	" -v           verbose",
	"",
	" Notes:",
	"  * The -m and -t options are required.",
	"  * Syntax for tfsname:  name,flags[,info]",
	"  * This tool creates an image that could also be created by populating",
	"    the target with all files, then using TFTP to retrieve the content",
	"    of that freshly populated memory space.",
	" Example:",
	"  * f2mem -v -Ob -mmonppc.bin -amonrc,e -t0x40000 -B0x80000000",
	"",
	(char *)0,
};

unsigned long crc32tab[] = {
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

struct finfo FileTable[MAXFILES];
int	verbose, debug, endianSwap;
short TFSHeaderVersion;
char *Outfile;
unsigned long	SrecBase;

/* tfsflagsatob():
   Convert ascii flags to binary and return the long.
*/
static long
tfsflagsatob(char *fstr)
{
	long	flag;
	struct	tfsflg	*tfp;

	if (!fstr)
		return(0);

	flag = 0;
	while(*fstr) {
		tfp = tfsflgtbl;
		while(tfp->sdesc) {
			if (*fstr == tfp->sdesc) {
				flag |= tfp->flag;
				break;
			}
			tfp++;
		}
		if (!tfp->flag)
			return(-1);
		fstr++;
	}
	return(flag);
}

/* crc32():
*/
unsigned long
crc32(uchar *buffer,unsigned long nbytes)
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

void
err(int exitval, char *msg1, char *msg2)
{
	if (!msg2)
		msg2 = "";
	fprintf(stderr,"ERROR: %s %s.\n",msg1,msg2);
	exit(exitval);
}

/* buildTfsHeader():
	Build the header into the buffer pointed to by buf.
	Use the incoming fip pointer to retrieve file information and
	assume the incoming value of address is the physical location
	at which the file will be in CPU memory.
	Return the number of bytes of adjustment made so that the next
	file is properly aligned.
*/
int
buildTfsHeader(unsigned long address, struct finfo *fip, struct tfshdr *tfp)
{
	int	modnext, alignadjust, rsvd;
	
	memset((char *)tfp,0,TFSHDRSIZ);
	strncpy(tfp->name,fip->name,TFSNAMESIZE);
	if (fip->info)
		strncpy(tfp->info,fip->info,TFSNAMESIZE);
	tfp->name[TFSNAMESIZE] = 0;
	tfp->info[TFSNAMESIZE] = 0;
	tfp->hdrsize = otherEnd16(endianSwap,TFSHDRSIZ);
	tfp->filsize = otherEnd32(endianSwap,fip->size);
	tfp->flags = tfsflagsatob(fip->flags);
	if (tfp->flags == -1)
		tfp->flags = 0;
	tfp->flags |= (TFS_ACTIVE | TFS_AIPNOT);
	if (!(tfp->flags & TFS_IPMOD))
		tfp->filcrc = otherEnd32(endianSwap,
			crc32((uchar *)fip->filecontent,fip->size));
	else
		tfp->filcrc = 0xffffffff;

	tfp->flags = otherEnd32(endianSwap,tfp->flags);
	tfp->hdrvrsn = otherEnd16(endianSwap,TFSHeaderVersion);
	tfp->next = 0;
	tfp->hdrcrc = 0;
	tfp->modtime = 0xffffffff;
	for(rsvd=0;rsvd<TFS_RESERVED;rsvd++)
		tfp->rsvd[rsvd] = 0xffffffff;
	tfp->hdrcrc = otherEnd32(endianSwap,crc32((uchar *)tfp,TFSHDRSIZ));

	/* Put next pointer on mod 16 address... */
	modnext = address + TFSHDRSIZ + fip->size;
	if (modnext % 16)
		alignadjust = 16 - ((unsigned)modnext % 16);
	else
		alignadjust = 0;

	tfp->next = (struct tfshdr *)(address+TFSHDRSIZ+fip->size+alignadjust);
	tfp->next = (struct tfshdr *)otherEnd32(endianSwap,
		(unsigned long)(tfp->next));

	return(alignadjust);
}

void
storeFileinfo(int idx,char *fstring,int type)
{
	char *comma1, *comma2;
	struct	finfo *fip;

	fip = &FileTable[idx];
	comma1 = strchr(fstring,',');

	if (!comma1)
		err(1,"file name must have flags appended:",fstring);
	*comma1 = 0;
	fip->name = fstring;
	fip->flags = comma1+1;
	comma2 = strchr(comma1+1,',');
	if (comma2) {
		*comma2 = 0;
		fip->info = comma2+1;
	}
	else
		fip->info = (char *)0;
	fip->type = type;
}

void
buf2bin(uchar *data, int size)
{
	int	fd;

	if (verbose)
		fprintf(stderr,"Building binary file...\n");

	fd = open(Outfile,O_WRONLY|O_BINARY|O_CREAT|O_TRUNC,0777);
	if (fd < 0)
		err(1,"Can't open output file:",Outfile);

	if (write(fd,data,size) != size)
		err(1,"Can't write file:",Outfile);

	close(fd);
}

void
buf2srec(uchar *data, int size)
{
	FILE	*fp;
	uchar	*cp, *dp;
	unsigned long	address;
	int		cnt, csum, i;

	if (verbose)
		fprintf(stderr,"Building S-record file...\n");
	
	fp = fopen(Outfile,"w+");
	if (fp == NULL)
		err(1,"Can't open output file:",Outfile);

	/* Create Srecord output file: */
	fprintf(fp,"S0030000FC\n");
	address = SrecBase;
	dp = data;

	while(size) {
		if (size > 32)
			cnt = 32;
		else
			cnt = size;

		/* Print S3 idendifier with byte count for line plus
		   4-byte address: */
		fprintf(fp,"S3%02X%08X",cnt+5,address);

		/* Accumlate checksum subtotal: */
		csum = 0;
		cp = (uchar *)&address;
		csum += (cnt+5);
		csum += cp[0]; csum += cp[1]; csum += cp[2]; csum += cp[3];

		/* Print each byte of data: */
		for(i=0;i<cnt;i++) {
			fprintf(fp,"%02X",dp[i]);
			csum += (int)dp[i];
		}

		/* Print sumcheck (includes bytecount, address & data): */
		/* (binary one's compliment) */
		fprintf(fp,"%02X\n",255 - (csum & 0xff));
		size -= cnt;
		dp += cnt;
		address += cnt;
	}
	fprintf(fp,"S705000019E001\n");
	fclose(fp);
}

int
main(int argc,char *argv[])
{
	unsigned long	tfsoffset, flashbase, padto;
	struct	stat statbuf;
	struct	finfo	*fip;
	int		ftot, totalmem;
	int		i, opt, ifd, swapmonitor, swapall;
	uchar	fillbyte;
	struct	tfshdr	hdrbuf;
	char	*membase, *memptr, outtype;

	if (argc == 1)
		usage(0);

	endianSwap = 1;
	swapmonitor = swapall = 0;
	TFSHeaderVersion = 1;
	SrecBase = 0;
	verbose = 0;
	padto = 0;
	outtype = 's';
	tfsoffset = 0;
	flashbase = 0;
	Outfile = (char *)0;
	fillbyte = 0xff;
	for(i=0;i<MAXFILES;i++)
		FileTable[0].name = (char *)0;
	ftot = 1;

	while ((opt=getopt(argc,argv,"a:B:b:ef:m:O:o:p:s:S:t:T:vV")) != EOF) {
		switch(opt) {
		case 'a':	/* ascii file */
			storeFileinfo(ftot++,optarg,TYPE_ASCII);
			break;
		case 'B':	/* CPU address that base of flash resides at */
			flashbase = strtoul(optarg,(char **)0,0);
			break;
		case 'b':	/* binary file */
			storeFileinfo(ftot++,optarg,TYPE_BINARY);
			break;
		case 'e':	/* disable endian swap */
			endianSwap = 0;
			break;
		case 'f':	/* filler byte */
			fillbyte = (uchar)strtoul(optarg,(char **)0,0);
			break;
		case 'm':	/* monitor file */
			/* Slot 0 of the file table is reserved for the monitor. */
			if (FileTable[0].name)
				err(1,"only one monitor file should be specified",0);
			FileTable[0].name = optarg;
			FileTable[0].type = TYPE_MONITOR;
			break;
		case 'O':	/* output file type */
			outtype = *optarg;
			if ((outtype != 's') && (outtype != 'b'))
				err(1,"output type must be 's' or 'b'",0);
			break;
		case 'o':	/* output file */
			Outfile = optarg;
			break;
		case 'p':	/* output file */
			padto = strtoul(optarg,(char **)0,0);
			break;
		case 's':	/* swap binary */
			if (*optarg == 'm')
				swapmonitor = 1;
			else if (*optarg == 'a')
				swapall = 1;
			else {
				fprintf(stderr,"Invalid argument to 's' option\n");
				usage(0);
			}
			break;
		case 'S':	/* S-record base */
			SrecBase = strtoul(optarg,(char **)0,0);
			break;
		case 'T':	/* TFS header version */
			TFSHeaderVersion = atoi(optarg);
			break;
		case 't':	/* TFS offset */
			tfsoffset = strtoul(optarg,(char **)0,0);
			break;
		case 'V':
			showVersion();
			break;
		case 'v':
			verbose++;
			break;
		default:
			usage(0);
		}
	}

	if (!FileTable[0].name)
		err(1,"monitor file must be specified",0);
	
	if (stat(FileTable[0].name,&statbuf) < 0)
		err(1,"can't find file:",FileTable[0].name);

	if (statbuf.st_size > tfsoffset) {
		fprintf(stderr,"TFS offset = 0x%x, monitor size = 0x%x\n",
			tfsoffset,statbuf.st_size);
		err(1,"TFS offset must be greater than size of monitor",0);
	}

	if (swapall & swapmonitor)
		err(1,"Can't swap monitor, then swap all... Do one or the other.",0);

	FileTable[0].size = statbuf.st_size;

	/* Now build each of the FileTable entries after the monitor... */
	/* Determine the size of the file, and copy the entire file into a */
	/* newly allocated buffer pointed to by filecontent... */
	/* Also keep track of just how much memory must be allocated to */
	/* fit the entire image into (store this in totalmem). */
	totalmem = tfsoffset;
	fip = &FileTable[1];
	for(i=1;i<ftot;i++) {
		if (stat(fip->name,&statbuf) < 0)
			err(1,"can't find file:",fip->name);
		totalmem += statbuf.st_size + TFSHDRSIZ;
		fip->size = statbuf.st_size;
		
		if ((ifd = open(fip->name,O_RDONLY | O_BINARY)) < 0)
			err(1,"can't open file:",fip->name);
		fip->filecontent = malloc(fip->size);
		if (!fip->filecontent)
			err(1,"can't allocate memory",0);

		if (read(ifd,fip->filecontent,fip->size) != fip->size)
			err(1,"read failed:",fip->name);
		close(ifd);
		fip++;
	}

	memptr = membase = malloc(totalmem+1024);
	if (!memptr)
		err(1,"can't allocate memory",0);

	/* Load the memory with the fill byte prior to loading anything else... */
	memset(membase,fillbyte,totalmem+1024);

	if (verbose > 1)
		fprintf(stderr,"TFS header size: %d\n",TFSHDRSIZ);

	/* First transfer the monitor file directly into allocated space... */
	fip = &FileTable[0];
	if ((ifd = open(fip->name,O_RDONLY | O_BINARY)) < 0)
		err(1,"can't open file:",fip->name);

	if (read(ifd,memptr,fip->size) != fip->size)
		err(1,"read failed:",fip->name);

	close(ifd);

	if (swapmonitor) {
		char tmp;
		i = 0;
		while(i < fip->size) {
			tmp = membase[i];
			membase[i] = membase[i+1];
			membase[i+1] = tmp;
			i += 2;
		}
	}

	if (verbose) {
		fprintf(stderr,"Processing files...\n");
		fprintf(stderr," 0: %-20s @ 0x%08x (%d bytes)\n",
			fip->name,flashbase,fip->size);
	}

	/* Now that we have loaded the monitor binary into ram, adjust the		*/
	/* memory pointer to the start of TFS and begin loading each file and	*/
	/* create the TFS header for each... */
	memptr += tfsoffset;
	fip = &FileTable[1];
	for(i=1;i<ftot;i++,fip++) {
		int	alignadjust;

		/* If filetype is TYPE_ASCII, strip out all CRs first... */
		if (fip->type == TYPE_ASCII) {
			int	j, choppedCRcnt;
			char *from, *to;

			choppedCRcnt = 0;
			from = to = fip->filecontent;
			for(j=0;j<fip->size;j++,from++) {
				if (*from == 0x0d)
					choppedCRcnt++;
				else
					*to++ = *from;
			}
			fip->size -= choppedCRcnt;
			totalmem -= choppedCRcnt;
		}

		if (verbose)
			fprintf(stderr,"%2d: %-20s @ 0x%08x (%d bytes)\n",
				i,fip->name,flashbase+(memptr-membase)+TFSHDRSIZ,fip->size);

		/* Build the header: */
		alignadjust = buildTfsHeader(flashbase+(memptr-membase),fip,&hdrbuf);

		/* Transfer header to memory: */
		memcpy(memptr,(char *)&hdrbuf,TFSHDRSIZ);
		memptr += TFSHDRSIZ;

		/* Copy file content to buffer... */
		memcpy(memptr,fip->filecontent,fip->size);
		memptr += fip->size;
		totalmem += alignadjust;
		memptr += alignadjust;
	}

	/* If padto is non-zero, and it is smaller than the current value of */
	/* totalmem, then use the fillbyte value to padd the memory out to the */
	/* size specified by padto... */
	if (padto) {
		char *paddedspace;

		if (padto > totalmem) {
			paddedspace = realloc(membase,padto);
			if (!paddedspace)
				fprintf(stderr,"Can't allocate space for padding\n");
			else {
				memset(paddedspace+totalmem,fillbyte,padto-totalmem);
				totalmem = padto;
				membase = paddedspace;
			}
		}
		else {
			fprintf(stderr,
				"Warning: pad-to value (0x%x) <= total memspace (0x%x)\n", 
				padto,totalmem);
		}
	}

	if (verbose)
		fprintf(stderr,"Total flash space required: %d (0x%x) bytes.\n",
			totalmem,totalmem);
	if (verbose > 1) {
		unsigned long	gap;
		gap = tfsoffset - FileTable[0].size;
		fprintf(stderr,
			"Gap between end of monitor and TFS: %d (0x%x) bytes.\n",gap,gap);
	}
			
	if (swapall) {
		char tmp;
		i = 0;
		while(i < totalmem) {
			tmp = membase[i];
			membase[i] = membase[i+1];
			membase[i+1] = tmp;
			i += 2;
		}
	}

	/* Now convert buffer to Srecord... */
	if (outtype == 's') {
		if (!Outfile)
			Outfile = "mem.srec";
		buf2srec((uchar *)membase,totalmem);
	}
	else {
		if (!Outfile)
			Outfile = "mem.bin";
		buf2bin((uchar *)membase,totalmem);
	}

	if (verbose)
		fprintf(stderr,"Output saved to file %s.\n",Outfile);

	/* Cleanup... */
	for(i=1;i<ftot;i++) 
		free(FileTable[i].filecontent);
	
	free(membase);
	return(0);
}
