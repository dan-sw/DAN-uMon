/* vi: set ts=4: */
/*	ffcmd.c:
 *	This is a front end (i.e. the CLI portion) of the elmfatfs.c source code.
 *	The general philosophy of this front-end is identical to that of
 *	fatfs.c; however, it is hooked to elmfatfs instead of dosfs.
 *	For complete overview of uMon's multi-FS/multi-driver philosophy
 *	refer to fatfs.c.
 *
 *	The elmfatfs-specific portions of this code are derived directly from
 *	sample code found at the elm-fatfs website:
 *
 *	http://elm-chan.org/fsw/ff/00index_e.html
 *
 */
 
#include "config.h"
#if INCLUDE_FATFS
#include "stddefs.h"
#include "genlib.h"
#include "cli.h"
#include "tfs.h"
#include "tfsprivate.h"
#include "ff.h"
#include "diskio.h"
#include "date.h"

static DRESULT	(*fatfsRead)(int interface, char *buf, int blknum, int count);
static DRESULT	(*fatfsWrite)(int interface, char *buf, int blknum, int count);

static char fatfs_initialized;
static char *fatfs_tmp_space;

#define FATFS_FNAME_STR		"FATFSFNAME"
#define FATFS_FTOT_STR		"FATFSFTOT"
#define FATFS_FSIZE_STR		"FATFSFSIZE"

static FATFS Fatfs[_DRIVES];	/* File system object for each logical drive */
static FILINFO Finfo;
static DWORD acc_size;				/* Work register for fs command */
static WORD acc_files, acc_dirs;
#if _USE_LFN
char Lfname[128];
#endif

DWORD get_fattime ()
{
#if INCLUDE_TIMEOFDAY
	struct todinfo tod;

	/* Get local time (if possible) */
	if (timeofday(TOD_GET,&tod) == 0)
		return	  ((DWORD)((DWORD)(tod.year - 1980) << 25)
			| ((DWORD)tod.month << 21)
			| ((DWORD)tod.date << 16)
			| ((DWORD)tod.hour << 11)
			| ((DWORD)tod.minute << 5)
			| ((DWORD)tod.second >> 1));
	else
#endif
	// If no timeofday capability, then just return Jan 1, 1980 12:00:00...
	return((0 << 25) | (1<<21) |  (1<<16) | (12<<11) | (00<<5) | 0); 
}

/* fatfsPerror():
 * Modified version of put_rc() from the original elm_fatfs...
 */
void
fatfsPerror(FRESULT rc)
{
	const char *p;
	static const char str[] =
		"OK\0" "DISK_ERR\0" "INT_ERR\0" "NOT_READY\0" "NO_FILE\0" "NO_PATH\0"
		"INVALID_NAME\0" "DENIED\0" "EXIST\0" "INVALID_OBJECT\0"
		"WRITE_PROTECTED\0" "INVALID_DRIVE\0" "NOT_ENABLED\0"
		"NO_FILE_SYSTEM\0" "MKFS_ABORTED\0" "TIMEOUT\0";
	FRESULT i;

	for (p = str, i = 0; i != rc && *p; i++) {
		while(*p++);
	}
	printf("rc=%u FR_%s\n", (UINT)rc, p);
}

static FRESULT
scan_files (char* path)
{
	DIR dirs;
	FRESULT res;
	BYTE i;
	char *fn;

	if ((res = f_opendir(&dirs, path)) == FR_OK) {
		i = strlen(path);
		while (((res = f_readdir(&dirs, &Finfo)) == FR_OK) && Finfo.fname[0]) {
			if (_FS_RPATH && Finfo.fname[0] == '.') continue;
#if _USE_LFN
			fn = *Finfo.lfname ? Finfo.lfname : Finfo.fname;
#else
			fn = Finfo.fname;
#endif
			if (Finfo.fattrib & AM_DIR) {
				acc_dirs++;
				*(path+i) = '/'; strcpy(path+i+1, fn);
				res = scan_files(path);
				*(path+i) = '\0';
				if (res != FR_OK) break;
			} else {
//				printf("%s/%s\n", path, fn);
				acc_files++;
				acc_size += Finfo.fsize;
			}
		}
	}

	return res;
}

/* fatfsInit():
 * This function will return if already initialized, unless force
 * is nonzero.
 */
static int
fatfsInit(int force, int verbose)
{
	long p2;
	BYTE res;
	FATFS *fs;				/* Pointer to file system object */

	if ((fatfs_initialized == 1) && (force == 0))
		return(0);

	f_mount(0, &Fatfs[0]);

	if ((res = f_getfree("", (DWORD*)&p2, &fs)) != FR_OK) {
		printf("f_getfree failed: ");
		fatfsPerror(res);
		return(-1);
	}

	if (verbose) {
		printf("FAT type = %u\nBytes/Cluster = %lu\nNumber of FATs = %u\n"
			"Root DIR entries = %u\nSectors/FAT = %lu\n"
			"Number of clusters= %lu\n"
			"FAT start (lba) = %lu\nDIR start (lba,clustor) = %lu\n"
			"Data start (lba) = %lu\n\n...",
			(WORD)fs->fs_type, (DWORD)fs->csize * 512, (WORD)fs->n_fats,
			fs->n_rootdir, fs->sects_fat, (DWORD)fs->max_clust - 2,
			fs->fatbase, fs->dirbase, fs->database
		);
		acc_size = acc_files = acc_dirs = 0;
#if _USE_LFN
		Finfo.lfname = Lfname;
		Finfo.lfsize = sizeof(Lfname);
#endif
		if ((res = scan_files("")) != FR_OK) {
			printf("scan_files failed: ");
			fatfsPerror(res);
			return(-1);
		}
		printf("\r%u files, %lu bytes.\n%u folders.\n"
			"%lu KB total disk space.\n%lu KB available.\n",
			acc_files, acc_size, acc_dirs,
			(fs->max_clust - 2) * (fs->csize / 2), p2 * (fs->csize / 2)
		);
	}

	fatfs_initialized = 1;
	return(0);
}

/* fatfsCat():
 * Dump the content of the specified fatfs file to the console.
 * This function assumes the data is ASCII.
 */
static int
fatfsCat(char *filepath)
{
	int i;
	UINT rsiz;
	FRESULT rc;
	FIL fobj;		/* File object */
	char mybuf[128];

	if ((rc = f_open(&fobj, filepath, (BYTE)FA_READ)) != FR_OK) {
		printf("f_open failed: ");
		fatfsPerror(rc);
		return(-1);
	}

	do {
		if ((rc = f_read(&fobj, mybuf, sizeof(mybuf), &rsiz)) != FR_OK) {
			printf("f_read failed: ");
			fatfsPerror(rc);
			f_close(&fobj);
			return(-1);
		}
		for(i=0;i<rsiz;i++)
			putchar(mybuf[i]);

	} while (rsiz == sizeof(mybuf));

	putchar('\n');

	if ((rc = f_close(&fobj)) != FR_OK) {
		printf("f_close failed: ");
		fatfsPerror(rc);
		return(-1);
	}
	return(0);
}

/* fatfsGet():
 * Retreive a file from the FATFS and place it in the specified destination.
 * The destination may be either a TFS filename or a hex address
 * (indicated by a leading "0x").
 *
 * Input:
 * char *fatfspath
 *	Pointer to FATFS file and path.
 * char *dest
 *	Pointer to TFS filename or a addr,len string.
 *	The expected "addr,len" syntax is 0xADDR,LEN or 0xADDR,0xLEN.
 */
static int
fatfsGet(char *fatfile, char *dest)
{
	FIL	fobj;
	int fsize;
	UINT rsiz;
	FRESULT rc;
	char *p, *p1, tfs;

	if ((rc = f_open(&fobj, fatfile, (BYTE)FA_READ)) != FR_OK) {
		printf("error opening file: ");
		fatfsPerror(rc);
		return(-1);
	}

	if (strncmp(dest,"0x",2) == 0) {
		tfs = 0;
		p = (char *)strtoul(dest,0,0);
	}
	else {
		tfs = 1;
		p = fatfs_tmp_space;
	}

	p1 = p;
	fsize = 0;
	do {
		if ((rc = f_read(&fobj, p, 512, &rsiz)) != FR_OK) {
			printf("f_read failed: ");
			fatfsPerror(rc);
			f_close(&fobj);
			return(-1);
		}
		fsize += rsiz;
		p += rsiz;

	} while (rsiz == 512); 

	if ((rc = f_close(&fobj)) != FR_OK) {
		printf("f_close failed: ");
		fatfsPerror(rc);
		return(-1);
	}

	if (tfs) {
#if INCLUDE_TFS
		int	tfserr;
		char *flags, *info;

		flags = info = (char *)0;
		if ((flags = strchr(dest,','))) {
			*flags++ = 0;
			if ((info = strchr(flags,',')))
				*info++ = 0;
		}
		tfserr = tfsadd(dest,info,flags,(uchar *)p1,fsize);
		if (tfserr != TFS_OKAY) {
			printf("TFS error: %s\n",tfserrmsg(tfserr));
			return(-1);
		}
#else
		printf("TFS not built in\n");
		return(-1);
#endif
	}
	return(0);
}

/* fatfsPut():
 * Copy data or file to a FATFS file.
 *
 * Input:
 * char *fatfspath
 *	Pointer to FATFS file and path.
 * char *src
 *	Pointer to TFS filename or a addr,len string.
 *	The expected "addr,len" syntax is 0xADDR,LEN or 0xADDR,0xLEN.
 */
static int
fatfsPut(char *src, char *fatfspath)
{
	FIL fobj;
	char tfs, *p, *cp;
	UINT wlen, len, rc;

	if (strncmp(src,"0x",2) == 0) {
		tfs = 0;
		p = (char *)strtoul(src,&cp,0);
		cp++;
		len = strtoul(cp,0,0);
	}
	else {
#if INCLUDE_TFS
		TFILE *tfp;

		tfs = 1;
		if ((tfp = tfsstat(src)) == 0) {
			printf("Can't find file '%s' in TFS\n",src);
			return(CMD_FAILURE);
		}
		p = TFS_BASE(tfp);
		len = TFS_SIZE(tfp);
#else
		printf("TFS not built in\n");
		return(-1);
#endif
	}

	/* Copy 'len' bytes from 'p' to FATFS file 'fatfspath'...
	 */
	rc = f_open(&fobj, fatfspath, FA_CREATE_ALWAYS | FA_WRITE);
	if (rc != FR_OK) {
		printf("error opening '%s': ",fatfspath);
		fatfsPerror(rc);
		return(-1);
	}
	rc = f_write(&fobj, p, len, &wlen);
	f_close(&fobj);
	if ((rc != FR_OK) || (wlen != len)) {
		printf("error writing '%s'\n",fatfspath);
		fatfsPerror(rc);
		return(-1);
	}
	return(0);
}

/* fatfsMkdir():
 * Create the specified directory in FATFS.
 */
static int
fatfsMkdir(char *fatfspath)
{
	int rc;

	if ((rc = f_mkdir(fatfspath)) != FR_OK) {
		printf("f_mkdir failed: ");
		fatfsPerror(rc);
		return(-1);
	}
	return(0);
}

/* fatfsRm():
 * Remove the specified file from the FATFS.
 * If the path is a directory, then don't remove anything.
 */
static int
fatfsRm(char *fatfspath)
{
	int rc;

	if ((rc = f_unlink(fatfspath)) != FR_OK) {
		printf("f_unlink failed: ");
		fatfsPerror(rc);
		return(-1);
	}
	return(0);
}

/* fatfsLs():
 * The incoming string is assumed to be some directory name.
 * If NULL, then it is assumed to be the CF's top-most level.
 */ 
static int
fatfsLs(char *dirname)
{
	long btot;
	char *dir;
	DIR Dir;
	FRESULT rc;
	long long fsiztot;
	int	dirtot, filtot;
	FATFS *fs;

	if (dirname)
		dir = dirname;
	else
		dir = "";

	if ((rc = f_opendir(&Dir, dir)) != FR_OK) {
		printf("f_opendir failed: ");
		fatfsPerror(rc);
		return(-1);
	}

	fsiztot = 0;
	dirtot = filtot = 0;
	for(;;) {
#if _USE_LFN
		Finfo.lfname = Lfname;
		Finfo.lfsize = sizeof(Lfname);
#endif
		rc = f_readdir(&Dir, &Finfo);
		if ((rc != FR_OK) || !Finfo.fname[0]) break;
		if (Finfo.fattrib & AM_DIR) {
			dirtot++;
		} else {
			filtot++;
			fsiztot += Finfo.fsize;
		}
		printf("%c%c%c%c%c %u/%02u/%02u %02u:%02u %9lu  ",
			(Finfo.fattrib & AM_DIR) ? 'D' : '-',
			(Finfo.fattrib & AM_RDO) ? 'R' : '-',
			(Finfo.fattrib & AM_HID) ? 'H' : '-',
			(Finfo.fattrib & AM_SYS) ? 'S' : '-',
			(Finfo.fattrib & AM_ARC) ? 'A' : '-',
			(Finfo.fdate >> 9) + 1980, (Finfo.fdate >> 5) & 15,
			Finfo.fdate & 31, (Finfo.ftime >> 11),
			(Finfo.ftime >> 5) & 63, Finfo.fsize);
#if _USE_LFN
		if (Lfname[0])
			printf("%s\n", Lfname);
		else
#endif
		printf("%s\n", &(Finfo.fname[0]));
	}
	printf("%4u File(s),%11lld bytes total\n%4u Dir(s)",
		filtot, fsiztot, dirtot);
	if (f_getfree(dir, (DWORD*)&btot, &fs) == FR_OK) {
		printf(", %11lld bytes free\n",
			(long long)((long long)btot * (long long)fs->csize * 512LL));
	}

	shell_sprintf(FATFS_FTOT_STR,"%d",filtot);
	return(0);
}


static int
fatfsQry(char *fatfspath, int verbose)
{
	DIR Dir;
	FILINFO filinfo;
	FRESULT rc;
	char *lastslash, *fname, *dir, pathcopy[80], matchname[80];
	int flen, flen1, fsize, pathlen, match, ftot;

	if (!fatfspath)
		return(-1);

	/* Prior to each 'ls', clear the content of the name and
	 * size shell variables...
	 */
	setenv(FATFS_FNAME_STR,0);
	setenv(FATFS_FSIZE_STR,0);
	setenv(FATFS_FTOT_STR,0);

	pathlen = strlen(fatfspath);
	if (pathlen > sizeof(pathcopy)) {
		printf("path too big\n");
		return(-1);
	}
	strcpy(pathcopy, fatfspath);
	lastslash = strrchr(pathcopy,'/');

	if (lastslash == 0) {
		dir = "";
		fname = pathcopy;
	}
	else {
		*lastslash = 0;
		dir = pathcopy;
		fname = lastslash+1;
	}

	flen = strlen(fname);
	if (verbose > 1)
		printf("Qry opening dir <%s>, fname <%s>...\n",dir,fname);

	if ((rc = f_opendir(&Dir, dir)) != FR_OK) {
		printf("f_opendir failed: ");
		fatfsPerror(rc);
		return(-1);
	}

	match = fsize = ftot = flen1 = 0;
	while (1) {
		char dosname[sizeof(Lfname)];

#if _USE_LFN
		filinfo.lfname = Lfname;
		filinfo.lfsize = sizeof(Lfname);
#endif
		rc = f_readdir(&Dir, &filinfo);
		if (rc != FR_OK) {
			printf("f_readdir failed: ");
			fatfsPerror(rc);
			break;
		}
		if (!filinfo.fname[0])
			break;

#if _USE_LFN
		if (Lfname[0])
			strcpy(dosname,Lfname);
		else
#endif
		strcpy(dosname,&(filinfo.fname[0]));
		flen1 = strlen(dosname);

		if ((fname[0] == '*') && (fname[flen-1] == '*')) {
			fname[flen-1] = 0;
			if (strstr(dosname,fname+1))
				match = 1;
			fname[flen-1] = '*';
		}
		else if (fname[0] == '*') {
			if (!strcmp(dosname+(flen1-flen+1),fname+1))
				match = 1;
		}
		else if (fname[flen-1] == '*') {
			fname[flen-1] = 0;
			if (!strncmp(dosname,fname,flen-1))
				match = 1;
			fname[flen-1] = '*';
		}
		else if (!strcmp(dosname,fname)) {
			match = 1;
		}
		if (match) {
			strcpy(matchname,dosname);
			fsize = filinfo.fsize;
			ftot++;
			match = 0;
			if (verbose)
				printf("  %s (%d)\n",dosname,fsize);
		}
	}
	shell_sprintf(FATFS_FTOT_STR,"%d",ftot);
	if (ftot) {
		shell_sprintf(FATFS_FSIZE_STR,"%d",fsize);
		setenv(FATFS_FNAME_STR,matchname);
	}
	return(0);
}


char *FatfsHelp[] = {
	"Fat 12|16|32 File System Support",
	"[r:s:w:v] {operation} [args]...",
#if INCLUDE_VERBOSEHELP
	"",
	"Options:",
	" -r {addr}   set read-block func ptr (default: FATFS_RD)",
	" -s {addr}   base of ram scratch space used by fatfs",
	"             (default is APPRAMBASE)",
	" -w {addr}   set write-block func ptr (default: FATFS_WR)",
	" -v          additive verbosity",
	"",
	"Operations:",
	" cat {fname}",
	" mkdir {dirname}",
	" get {fat_file} {tfs_file | addr}",
	"              (get from fatfs)",
	" init         initialize internal structures",
	" put {tfs_file | addr,size} {fat_file}",
	"              (put to fatfs)",
	" ls  [dir]    list files within specified directory (or top)",
	" qry {fltr}   qry for file or directory presence",
	"              - fltr format: sss, *sss, sss*, *sss* ('sss' is any string)",
	"              - loads " FATFS_FNAME_STR " & " FATFS_FSIZE_STR " based on most recent match",
	"              - loads " FATFS_FTOT_STR " with number of files listed",
	" rm  {fname}",
#endif
	0
};

/* FatfsCmd():
 * This command provides a front end (command line interface) to the
 * dosfs.c code.  The intent is that this be a template for other
 * "fs-ish" commands within uMon (refer to discussion at the top of this
 * file).
 */
int
FatfsCmd(int argc, char *argv[])
{
	uint32_t i, j;
	int		opt, verbose;
	char	*cmd, *env, *arg1, *arg2, *arg3;

	/* Establish default read/write functions to access underlying
	 * storage media.  These defaults can be overridden by the 
	 * command line options -r & -w below.
	 */
	if ((env = getenv("FATFS_RD")) != 0)
		fatfsRead = (DRESULT(*)(int,char*,int,int))strtol(env,0,0);

	if ((env = getenv("FATFS_WR")) != 0)
		fatfsWrite = (DRESULT(*)(int,char*,int,int))strtol(env,0,0);

	verbose = 0;
	fatfs_tmp_space = (char *)0xffffffff;
	while ((opt=getopt(argc,argv,"r:s:vw:")) != -1) {
		switch(opt) {
			case 'r':
				fatfsRead = (DRESULT(*)(int,char*,int,int))strtoul(optarg,0,0);
				break;
			case 's':
				fatfs_tmp_space = (char *)strtoul(optarg,0,0);
				break;
			case 'w':
				fatfsWrite = (DRESULT(*)(int,char*,int,int))strtoul(optarg,0,0);
				break;
			case 'v':
				verbose++;
				break;
			default:
				return(CMD_PARAM_ERROR);
		}
	}

	if ((fatfsRead == 0) || (fatfsWrite == 0)) {
		printf("FATFS access pointers not initialized\n");
		return(CMD_FAILURE);
	}

	if (argc < optind + 1)
		return(CMD_PARAM_ERROR);

	if (fatfs_tmp_space == (char *)0xffffffff)
		fatfs_tmp_space = (char *)getAppRamStart();

	cmd = argv[optind];
	arg1 = argv[optind+1];
	arg2 = argv[optind+2];
	arg3 = argv[optind+3];

	if (strcmp(cmd,"init") == 0) {
		if (fatfsInit(1,verbose) < 0)
			return(CMD_FAILURE);
	}
	else if (strcmp(cmd,"cat") == 0) {
		if (argc != optind + 2) 
			return(CMD_PARAM_ERROR);

		if (fatfsInit(0,0) < 0)
			return(CMD_FAILURE);

		for(i=optind+1;i<argc;i++) {
			if (fatfsCat(argv[i]) < 0)
				return(CMD_FAILURE);
		}
	}
	else if (strcmp(cmd,"get") == 0) {
		if (argc != optind + 3) 
			return(CMD_PARAM_ERROR);

		if (fatfsInit(0,0) < 0)
			return(CMD_FAILURE);

		if (fatfsGet(arg1,arg2) < 0)
			return(CMD_FAILURE);

	}
	else if (strcmp(cmd,"put") == 0) {
		if (argc != optind + 3) 
			return(CMD_PARAM_ERROR);

		if (fatfsInit(0,0) < 0)
			return(CMD_FAILURE);

		if (fatfsPut(arg1,arg2) < 0)
			return(CMD_FAILURE);
	}
	else if (strcmp(cmd,"qry") == 0) {
		if (argc != optind + 2) 
			return(CMD_PARAM_ERROR);

		if (fatfsInit(0,0) < 0)
			return(CMD_FAILURE);

		if (fatfsQry(arg1,verbose) < 0)
			return(CMD_FAILURE);
	}
	else if (strcmp(cmd,"ls") == 0) {
		if (fatfsInit(0,0) < 0)
			return(CMD_FAILURE);

		if (fatfsLs(arg1) < 0)
			return(CMD_FAILURE);
	}
	else if (strcmp(cmd,"rm") == 0) {
		if (fatfsInit(0,0) < 0)
			return(CMD_FAILURE);

		for(j=optind+1;j<argc;j++) {
			if (fatfsRm(argv[j]) < 0)
				return(CMD_FAILURE);
		}
	}
	else if (strcmp(cmd,"mkdir") == 0) {
		if (fatfsInit(0,0) < 0)
			return(CMD_FAILURE);

		if (fatfsMkdir(arg1) < 0)
			return(CMD_FAILURE);
	}
	else {
		printf("fatfs op <%s> not found\n",cmd);
		return(CMD_FAILURE);
	}

	return(CMD_SUCCESS);
}

/* Hookup between EMLFATFS and this command...
 * The elmfatfs code assumes a generic underlying block read/write/control
 * type of interface to hook it up to an external device.
 * These two functions are configured by the fatfs command to point to
 * whatever driver is to be established as the FATFS physical interface.
 * For more information refer to the discussion at the top of the fatfs.c
 * file.
 */
DRESULT
disk_read(BYTE unit, BYTE *data, DWORD sec, BYTE count)
{
	return((DRESULT)fatfsRead((int)unit, (char *)data, sec, (int)count));
}

#if	_READONLY == 0
DRESULT
disk_write(BYTE unit, const BYTE *data, DWORD sec, BYTE count)
{
	return((DRESULT)fatfsWrite((int)unit, (char *)data, sec, (int)count));
}
#endif

DSTATUS
disk_initialize (BYTE ignored)
{
	return(0);
}

DSTATUS
disk_status (BYTE ignored)
{
	return(0);
}

DRESULT
disk_ioctl (BYTE ignored1, BYTE ignored2, void* ignored3)
{
	return(0);
}
#endif
