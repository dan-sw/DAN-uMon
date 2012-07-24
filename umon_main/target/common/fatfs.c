/* vi: set ts=4: */
/* fatfs.c:
 * This is a front end (i.e. the CLI portion) of the dosfs.c source code.
 * The dosfs.c file is code from Lewin Edwards.  The website I got this
 * from originally was: http://www.zws.com/products/dosfs/index.html .
 *
 * The dosfs.c dosfs.h files in uMon common (i.e. here) are taken directly
 * from that website, with minimal modification (so as to remain compatible
 * with any DOSFS updates).  Much of this fatfs code is also shamelessly
 * cut-and-pasted from Lewin's source file main.c (part of the dosfs
 * tarball) used to demonstrate the dosfs interface.
 *
 * Discussion on uMon's philosophy of file-systems other than TFS...
 * The intent under the scope of uMon for support of file systems other than
 * TFS is to support two commands: a file-system-specific command and an
 * interface-specific command.  The goal is that members of the two different
 * command sets can be mixed and matched as needed in a particular design.
 * For example, if we have commands for compact flash (CF), SPI and I2C 
 * interfaces the "fatfs" command could use anyone of them to retrieve files
 * from that interface (assuming of course, that the interface has an FAT
 * FS on the other side).
 *
 * The way this works is simple, the only thing the fatfs command needs is
 * a set of interface-specific functions: readblock and writeblock. As
 * long as the CF/SPI/I2C commands support this, and also support the
 * ability to share those interface functions through the CLI, it simply
 * becomes a matter of assigning the function pointers in a startup script
 * when uMon boots up (probably in monrc).
 *
 * That being said, here's the model...
 *
 * File-system-specific Command:
 * Each command will support the ability to establish its read and write
 * pointers through the command line.  The -r and -w options do this in
 * the fatfs command, and it would be nice if other similar commands could
 * follow the same model.  The command should support (at minimum) the
 * ability to list the files in the FS (ls), query for the presence of a
 * specific file within the FS (qry), copy a file from the FS to TFS or
 * ram (get), and also dump the content of an ASCII file from the FS to the
 * console (cat).  If the command supports modification of the FS, then,
 * it should also support the ability remove (rm) and copy from ram or TFS
 * to the FS (put).  The command also supports default shell variables that
 * may contain the read write functions.
 *
 * Interface-specific Command:
 * Each command will support the potential of multiple interfaces of the 
 * same type.  The command must be able to initialize (with optional
 * verbosity) said interface and populate a defined set of shell variables
 * with the address of the read and write functions applicable to that
 * interface.  Ideally the interface command should also support the ability
 * to do raw reads (FS to memory) and writes (memory to FS) on the interface.
 *
 * With the above model we then have the ability to connect the "FS" command
 * with any "Interface" command; so as file system support in uMon grows,
 * it immediately becomes useful on a variety of interfaces, as long as
 * they adhere to the above simple model.  As of this writing the "cf" and
 * "fatfs" commands comply to this model and can be used as a template for
 * developing new commands that fit into this category.
 * 
 * Here is an example of their usage:
 *
 * uMON> cf init FATFS    # Initialize the compact flash interface and
 *                        # laod FATFS_RD and FATFS_WR with the read and
 *                        # write function pointers. 
 * uMON> fatfs cat file1  # Using the FATFS_RD and FATFS_WR shell variables
 *                        # previously set up, dump the content of the file 
 *                        # named 'file1'.
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
 *	Original author:	Ed Sutter			esutter@lucent.com 
 *	Contributor(s):		Graham Henderson
 */
 
#include "config.h"
#if INCLUDE_FATFS
#include "stddefs.h"
#include "genlib.h"
#include "cli.h"
#include "tfs.h"
#include "tfsprivate.h"
#include "dosfs.h"

static int	(*fatfsRead)(int interface, char *buf, int blknum, int count);
static int	(*fatfsWrite)(int interface, char *buf, int blknum, int count);

static char fatfs_initialized;
static char *fatfs_tmp_space;
static VOLINFO vi __attribute__((aligned (32)));
static uint8_t sector[SECTOR_SIZE]  __attribute__ ((aligned (32)));
static uint8_t sector2[SECTOR_SIZE] __attribute__ ((aligned (32)));

/* Support skipping LFN dir entries...
 */
#define ATTR_LONG_NAME  \
	(ATTR_READ_ONLY | ATTR_HIDDEN | ATTR_SYSTEM | ATTR_VOLUME_ID)

#define FATFS_FNAME_STR		"FATFSFNAME"
#define FATFS_FTOT_STR		"FATFSFTOT"
#define FATFS_FSIZE_STR		"FATFSFSIZE"

/* fatfsInit():
 * Start up the fs by reading in volume information.
 * This function will return if already initialized, unless force
 * is nonzero.
 */
static int
fatfsInit(int force, int verbose)
{
	uint8_t pactive, ptype;
	uint32_t pstart, psize;

	if ((fatfs_initialized == 1) && (force == 0))
		return(0);

	/* Obtain pointer to first partition on first (only) unit.
	 */
	pstart = DFS_GetPtnStart(0, sector, 0, &pactive, &ptype, &psize);
	if (pstart == 0xffffffff) {
		printf("Cannot find first partition\n");
		return(-1);
	}

	if (verbose) {
		printf("Partition 0...\n");
		printf("  Start sector 0x%08lx\n",pstart);
		printf("  Active %d\n",pactive);
		printf("  Type 0x%x\n",ptype);
		printf("  Size 0x%lx\n",psize);
	}

	if (DFS_GetVolInfo(0, sector, pstart, &vi)) {
		printf("Error getting volume information\n");
		return(-1);
	}

	if (verbose) {
		printf("Volume label '%-11s'\n", vi.label);
		printf("  %d sector(s) per cluster\n",vi.secperclus);
		printf("  %d reserved sector(s)\n",vi.reservedsecs);
		printf("  %d total volume sector(s)\n", vi.numsecs);
		printf("  %d sectors per FAT\n",vi.secperfat);
		printf("  first FAT at sector #%d\n",vi.fat1);
		printf("  root dir at #%d\n",vi.rootdir);

		printf("For.. FAT32 rootdir is CLUSTER #,\n");
		printf("      FAT12/16 rootdir is a SECTOR #.\n");
		printf("%d root dir entries, data area commences at sector #%d.\n",
			vi.rootentries,vi.dataarea);
		printf("%d clusters (%d bytes) in data area, filesystem IDd as ",
			vi.numclusters, vi.numclusters * vi.secperclus * SECTOR_SIZE);

		if (vi.filesystem == FAT12)
			printf("FAT12.\n");
		else if (vi.filesystem == FAT16)
			printf("FAT16.\n");
		else if (vi.filesystem == FAT32)
			printf("FAT32.\n");
		else
			printf("[unknown]\n");
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
	uint32_t i, j;
	FILEINFO fi;
	uint8_t *p;

	if (DFS_OpenFile(&vi, (uint8_t *)filepath, DFS_READ, sector, &fi)) {
		printf("error opening '%s'\n",filepath);
		return(-1);
	}
	p = (uint8_t *)fatfs_tmp_space;
	DFS_ReadFile(&fi, sector, p, &i, fi.filelen);
	for(j=0;j<fi.filelen;j++)
		putchar(*p++);
	putchar('\n');
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
	FILEINFO fi;
	char *p, tfs;
	uint32_t i;

	if (DFS_OpenFile(&vi, (uint8_t *)fatfile, DFS_READ, sector, &fi)) {
		printf("error opening file\n");
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

	DFS_ReadFile(&fi, sector, (uint8_t *)p, &i, fi.filelen);
	if (i != fi.filelen) {
		printf("read %d, expected %d\n",i,fi.filelen);
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
		tfserr = tfsadd(dest,info,flags,(uchar *)p,fi.filelen);
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
	FILEINFO fi;
	char tfs, *p, *cp;
	uint32_t successcount, len, size, rc;

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

	/* Copy 'len' bytes from 'p' to DOSFS file 'fatfspath'...
	 */
	if (DFS_OpenFile(&vi, (uint8_t *)fatfspath, DFS_WRITE, sector, &fi)) {
		printf("error opening '%s'\n",fatfspath);
		return(-1);
	}

	while(len > 0) {
		if (len >= SECTOR_SIZE)
			size = SECTOR_SIZE;
		else
			size = len;

		rc = DFS_WriteFile(&fi, sector2, (uint8_t *)p, &successcount, size);
		if ((rc != DFS_OK) || (successcount != size)) {
			printf("error writing '%s'\n",fatfspath);
			return(-1);
		}
		p += size;
		len -= size;
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
	DIRINFO di;

	di.scratch = sector;
	if (DFS_OpenDir(&vi, (uint8_t *)fatfspath, &di) == 0) {
		printf("can't remove directory '%s'\n",fatfspath);
		return(-1);
	}
	if (DFS_UnlinkFile(&vi, (uint8_t *)fatfspath, sector)) {
		printf("error unlinking file %s\n",fatfspath);
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
	int	ftot;
	DIRENT de;
	DIRINFO di;
	char *dir;

	if (dirname)
		dir = dirname;
	else
		dir = "";

	di.scratch = sector;
	if (DFS_OpenDir(&vi, (uint8_t *)dir, &di)) {
		printf("Can't open dir: <%s>\n",dirname);
		return(CMD_FAILURE);
	}

	ftot = 0;
	while (!DFS_GetNext(&vi, &di, &de)) {
		char tmp[32]; 

		if (de.name[0] && ((de.attr & ATTR_LONG_NAME) != ATTR_LONG_NAME)) {
			if ((de.attr & ATTR_VOLUME_ID) > 0) {
				de.name[8] = 0;
				printf("Volume:     %s\n",de.name);
				continue;
            }
			ftot++;

			printf("%02d/%02d/%4d  %02d:%02d   ",
				CREATE_MON(de), CREATE_DAY(de), CREATE_YEAR(de),
				CREATE_HR(de), CREATE_MIN(de));

			if ((de.attr & ATTR_DIRECTORY) > 0) {
				// truncate the extension
				de.name[8] = 0;

				printf("<DIR>            %-8s",de.name);
			}
			else {
				DFS_DirToCanonical((uint8_t *)tmp,de.name);
				printf("       %8d  %-12s",FILESIZE(de), tmp);
			}

			printf("  %c%c%c%c%c\n",
				(( de.attr & ATTR_VOLUME_ID) ? 'V' : ' '),
				(( de.attr & ATTR_READ_ONLY) ? 'R' : ' '),
				(( de.attr & ATTR_ARCHIVE  ) ? 'A' : ' '),
				(( de.attr & ATTR_SYSTEM   ) ? 'S' : ' '),
				(( de.attr & ATTR_HIDDEN   ) ? 'H' : ' '));
		}
	}
	shell_sprintf(FATFS_FTOT_STR,"%d",ftot);
	return(0);
}

static int
fatfsQry(char *fatfspath, int verbose)
{
	DIRENT de;
	DIRINFO di;
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
	lastslash = strrchr(pathcopy,DIR_SEPARATOR);

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

	di.scratch = sector;
	if (DFS_OpenDir(&vi, (uint8_t *)dir, &di)) {
		printf("error opening subdirectory\n");
		return(CMD_FAILURE);
	}

	match = fsize = ftot = flen1 = 0;
	while (!DFS_GetNext(&vi, &di, &de)) {
		int i;
		char dosname[16];

		memset(dosname,0,sizeof(dosname));
		if (de.name[0] && ((de.attr & ATTR_LONG_NAME) != ATTR_LONG_NAME)) {
			if ((de.attr & ATTR_VOLUME_ID) || (de.attr & ATTR_DIRECTORY)) {
				for(i=0;i<8;i++) {
					if (de.name[i] != ' ') {
						dosname[i] = de.name[i];
					}
					else {
						dosname[i] = 0;
						break;
					}
				}
				dosname[8] = 0;
			}
			else
				DFS_DirToCanonical((uint8_t *)dosname,de.name);
	
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
				fsize = FILESIZE(de);
				ftot++;
				match = 0;
				if (verbose)
					printf("  %s (%d)\n",dosname,fsize);
			}
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
		fatfsRead = (int(*)(int,char*,int,int))strtol(env,0,0);

	if ((env = getenv("FATFS_WR")) != 0)
		fatfsWrite = (int(*)(int,char*,int,int))strtol(env,0,0);

	verbose = 0;
	fatfs_tmp_space = (char *)0xffffffff;
	while ((opt=getopt(argc,argv,"r:s:vw:")) != -1) {
		switch(opt) {
			case 'r':
				fatfsRead = (int(*)(int,char*,int,int))strtoul(optarg,0,0);
				break;
			case 's':
				fatfs_tmp_space = (char *)strtoul(optarg,0,0);
				break;
			case 'w':
				fatfsWrite = (int(*)(int,char*,int,int))strtoul(optarg,0,0);
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
	else {
		printf("fatfs op <%s> not found\n",cmd);
		return(CMD_FAILURE);
	}

	return(CMD_SUCCESS);
}

/* Hookup between DOSFS and this command...
 * The dosfs.c code requires that the user provide two functions
 * (DFS_ReadSector() and DFS_WriteSector()) to hook it up to an
 * external interface.  These two functions are configured by
 * the fatfs command to point to whatever driver is to be established
 * as the FATFS physical interface.  For more information refer to the
 * discussion at the top of this file.
 */
uint32_t
DFS_ReadSector(uint8_t unit, uint8_t *data, uint32_t sec,uint32_t count)
{
	return((uint32_t)fatfsRead((int)unit, (char *)data, sec, count));
}

uint32_t
DFS_WriteSector(uint8_t unit, uint8_t *data, uint32_t sec,uint32_t count)
{
	return((uint32_t)fatfsWrite((int)unit, (char *)data, sec, count));
}

#endif
