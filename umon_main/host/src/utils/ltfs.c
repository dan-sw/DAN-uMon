/* ltfs.c:
 * This program is intended for execution on an embedded linux platform
 * that was booted with MicroMonitor and has an MTD partition that
 * spans the flash space allocated to TFS.
 *
 * It provides the ability to transfer files from TFS to Linux and
 * from Linux to TFS.  It assumes that all sectors within the partition
 * are the same size and that the spare sector is the last sector in
 * the space allocated to the MTD partition.
 *
 * Note that this tool does not take advantage of TFS's defragmentation;
 * hence, if there isn't enough space at the end of TFS's storage ared
 * for the file, then the transfer is aborted.  It is up to uMon to make
 * sure that TFS is kept clean prior to booting linux.
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
#include <unistd.h>

/* Note that these two files (tfs.h and tfsprivate.h) are included in the
 * host tools directory only for convenience (so that the host directory can
 * be isolated from the target directory).  The files tfs.h and tfsprivate.h
 * in this space should be identical to those in target/common.
 */
#include "tfs.h"
#include "tfsprivate.h"

extern int optind;
extern char *optarg;

static char             *tfs_in_ram, *end_of_tfs_in_ram;
static int              debug, verbose, partition_size;
static unsigned long    flashBase, tfs_erasesize;

char    *usage_txt[] = {
        "Usage: tfs [options] {infile} {command} [command args]",
        "Options:",
        "   -h              generate this help message",
        "   -M {path}       specify MTD path (default=\"/dev/mtd\")",
        "   -m {fname}      specify MTD proc file name (default=\"/proc/mtd\")",
        "   -t {pname}      specify TFS partition name (default=\"TFS\")",
        "   -v              additive verbosity",
        "",
        "Commands:",
        "   ls                                 :  list all files in TFS partition",
        "   ls {filename}                      :  list specified file and load exit status",
        "                                         (1 = file not found, 0 = file found)",
        "   dump                               :  dump headers of files in TFS partition",
        "   get {tfile} [lfile]                :  copy TFS file to Linux file",
        "   put {lfile} [tfile[,flags[,info]]] :  copy Linux file to TFS file",
        "   rm {tfile}                         :  remove file from TFS",
        0,
};

void
usage(int exitcode)
{
        int     i;

        for(i=0;usage_txt[i];i++)
                fprintf(stderr,"%s\n",usage_txt[i]);

        exit(exitcode);
}

struct tfsflg tfsflgtbl[] = {
        { TFS_BRUN,                     'b',    "run_at_boot",                  TFS_BRUN },
        { TFS_QRYBRUN,          'B',    "qry_run_at_boot",              TFS_QRYBRUN },
        { TFS_EXEC,                     'e',    "executable",                   TFS_EXEC },
        { TFS_SYMLINK,          'l',    "symbolic link",                TFS_SYMLINK },
        { TFS_EBIN,                     'E',    "exec-binary",                  TFS_EBIN },
        { TFS_IPMOD,            'i',    "inplace_modifiable",   TFS_IPMOD },
        { TFS_UNREAD,           'u',    "ulvl_unreadable",              TFS_UNREAD },
        { TFS_ULVL1,            '1',    "ulvl_1",                               TFS_ULVLMSK },
        { TFS_ULVL2,            '2',    "ulvl_2",                               TFS_ULVLMSK },
        { TFS_ULVL3,            '3',    "ulvl_3",                               TFS_ULVLMSK },
        { TFS_CPRS,                     'c',    "compressed",                   TFS_CPRS },
        { 0, 0, 0, 0 }
};

/* crc32tab[]:
 *      Used for calculating a 32-bit CRC.
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
        int     i;
        struct  tfsflg  *tfp;

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
        unsigned long   crc;
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
        ulong   next;

        next = (unsigned long)(tfp + 1);
        next += tfp->filsize;
        if (next & 0xf)
                next = (next | 0xf) + 1;
        return((TFILE *)next);
}

#define NOT_FF  1
#define IS_FF   2

/* postTFSTest():
 * This function attempts to analyze the state of the flash after
 * what has been previously determined to be the last file stored
 * in TFS.
 */
static void
postTFSTest(unsigned char *base, unsigned char *curpos, int size)
{
        int             notFF, state;
        unsigned char   *flashend, *flashp;
        int             err_tot, err_block_tot, err_block_size, err_block_maxsize;

        flashp = curpos;
        flashend = base + size;

        notFF = 0;
        state = NOT_FF;
        err_tot = 0;
        err_block_tot = 0;
        err_block_size = 0;
        err_block_maxsize = 0;

        while(flashp < flashend) {
                switch(state) {
                case NOT_FF:
                        if (*flashp == 0xFF) {
                                state = IS_FF;
                                if (err_block_size > err_block_maxsize)
                                        err_block_maxsize = err_block_size;
                        }
                        else {
                                err_tot++;
                                err_block_size++;
                        }
                        break;
                case IS_FF:
                        if (*flashp != 0xFF) {
                                state = NOT_FF;
                                err_tot++;
                                err_block_tot++;
                                err_block_size = 1;
                        }
                        break;
                }
                flashp++;
        }
        if (err_tot) {
                printf("\n\nPOST TFS SPACE Analysis:\n");
                printf("Size of space analyzed: %d bytes\n",size - (curpos - base));
                printf("Total blocks of non-FF space in post-TFS area: %d (%d bytes)\n",
                        err_block_tot,err_tot);
                printf("Largest errored block size: %d\n",err_block_maxsize);
        }
        else
                printf("Space after end of TFS is clear.\n");
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

/* Open the /proc/mtd file and find out what MTD partition is
 * allocated to TFS.  We do this by assyming the format of each
 * line of /proc/mtd to be...
 *
 * mtd<N>:  <BASE> <SIZE> "<NAME>"
 *
 * where each '<>' enclosed item is as follows:
 *
 * N            the mtd partition number
 * BASE         offset (relative to base of MTD) of the partition
 * SIZE         size of the partition
 * NAME         name of the partition
 *
 * For example...
 *
 *  # cat /proc/mtd
 *  dev:    size   erasesize  name
 *  mtd0: 00580000 00020000 "FPGA Image"
 *  mtd1: 00060000 00020000 "uMon"
 *  mtd2: 00120000 00020000 "TFS"
 *  mtd3: 000c0000 00020000 "u-boot"
 *  mtd4: 00020000 00020000 "environment"
 *  mtd5: 00020000 00020000 "inventory"
 *  mtd6: 00800000 00020000 "Not yet assigned"
 *  mtd7: 01000000 00020000 "OS1 Image"
 *  mtd8: 01000000 00020000 "OS2 Image"
 *  mtd9: 01000000 00020000 "JFFS2 Filesystem"
 *  #
 *
 * With the above /proc/mtd file, this function would return 2 (assuming
 * the tfs_partition_name was "TFS").
 */
int
get_tfs_partition(char *mtd_proc_file, char *tfs_partition_name)
{
        FILE *fp;
        int tfs_pnum;
        char line[128], *name_in_quotes, *cp, *cp1;

        name_in_quotes = malloc((int)strlen(tfs_partition_name) + 8);
        if (!name_in_quotes) {
                perror("get_tfs_partition malloc");
                return(-1);
        }

        if ((fp = fopen(mtd_proc_file,"r")) == (FILE *)NULL) {
                perror(mtd_proc_file);
                return(-1);
        }

        sprintf(name_in_quotes,"\"%s\"",tfs_partition_name);

        tfs_pnum = -1;
        while(fgets(line,sizeof(line),fp)) {
                if (strstr(line,name_in_quotes)) {
                        line[strlen(line)-1] = 0;
                        if (verbose > 1)
                                fprintf(stderr,"%s TFS line: <%s>\n",mtd_proc_file,line);
                        if (memcmp(line,"mtd",3) == 0) {
                                /* retrieve the partition number from the "mtdX" string
                                 * at the beginning of the line...
                                 */
                                tfs_pnum = atoi(line+3);

                                /* Then parse through the line to retrieve the erase
                                 * size of the TFS partition...
                                 */
                                cp = line;
                                while(*cp != ' ') cp++;
                                cp++;
                                while(*cp != ' ') cp++;
                                cp++;
                                cp1 = cp;
                                while(*cp != ' ') cp++;
                                *cp = 0;
                                tfs_erasesize = strtol(cp1,0,16);
                                break;
                        }
                }
        }
        fclose(fp);
        if (tfs_pnum == -1)
                fprintf(stderr,"Can't find %s in %s\n",name_in_quotes,mtd_proc_file);
        return(tfs_pnum);
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
        ulong physical_tfs_base;

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
 *                      "name,flags,info"
 *
 *      where flags and/or info can be empty.
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
        char copy[TFSNAMESIZE+1], *from, *to, *comma;

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

/* ltfs main():
 * This is a tool that allows a linux-based application to have
 * read/write access to TFS.  This is done through the MTD interface
 * of linux; hence, it assumes that the space allocated to TFS in
 * flash is one (or more) MTD partitions.
 */
int
main(int argc, char *argv[])
{
        long offset;
        struct  tfshdr  *nxttfp, *tfp;
        int opt, ifd, i, pnum, rc;
        int ofd, ftot, size, hdrcrcfail, filcrcfail;
        char buf[128], mtd_filename[128], fbuf[16], *flags;
        char *mtd_proc_file, *tfs_partition_name, *cmd;
        char *mtd_path;

        verbose = rc = 0;
        mtd_proc_file = "/proc/mtd";
        mtd_path = "/dev/mtd";
        tfs_partition_name = "TFS";
        while((opt=getopt(argc,argv,"dm:M:t:v")) != EOF) {
                switch(opt) {
                case 'd':
                        debug = 1;
                        break;
                case 'h':
                        usage(0);
                        break;
                case 'm':
                        mtd_proc_file = optarg;
                        break;
                case 'M':
                        mtd_path = optarg;
                        break;
                case 't':
                        tfs_partition_name = optarg;
                        break;
                case 'v':
                        verbose++;
                        break;
                default:
                        usage(1);
                }
        }

        if (argc < optind+1)
                usage(1);

        cmd = argv[optind];

        pnum = get_tfs_partition(mtd_proc_file,tfs_partition_name);
        if (pnum < 0)
                exit(1);

        /* Open mtd device file that corresponds to TFS flash:
         *
         * First try to open it with slash...
         */
        sprintf(mtd_filename,"%s/%d",mtd_path,pnum);

        ifd = open(mtd_filename,O_RDONLY);
        if (ifd == -1) {
        /* ... then if that fails,
         * try one more time without...
         */
                sprintf(mtd_filename,"%s%d",mtd_path,pnum);
                ifd = open(mtd_filename,O_RDONLY);
                if (ifd == -1) {
                        perror(mtd_filename);
                        exit(1);
                }
        }

        /* Can't use stat to retrieve the size because it's under /dev,
         * so use lseek (thanks Mike!)...
         */
        partition_size = lseek(ifd,0,SEEK_END);

        /* rewind...
         */
        if (lseek(ifd,0,SEEK_SET) < 0) {
                perror("lseek");
                exit(1);
        }

        if (verbose > 1) {
                fprintf(stderr,"%s partition size=%d\n",
                        tfs_partition_name,partition_size);
        }

        /* Allocate space for local storage of the file, but make sure that
         * the base address of the data is 16-byte aligned...
         */
        if ((tfs_in_ram = malloc(partition_size + 64)) == 0) {
                perror("malloc");
                exit(1);
        }
        end_of_tfs_in_ram = tfs_in_ram + partition_size + 64;

        /* Make sure the base of the allocated space starts on a 16-byte
         * boundary (makes things easier for TFS stuff because TFS assumes
         * it is aligned that way).
         */
        tfs_in_ram += 32;
        {
                long    ltfs_in_ram = (long)tfs_in_ram;

                ltfs_in_ram &= ~0xf;
                tfs_in_ram = (char *)ltfs_in_ram;
        }

        /* Copy the entire partition to the allocated ram space:
         */
        if (read(ifd,tfs_in_ram,partition_size) != partition_size) {
                perror("read");
                exit(1);
        }
        close(ifd);

        /* Retrieve the physical base address of TFS in flash:
         */
        flashBase = get_physical_tfs_base(tfs_in_ram);
        if (flashBase == 0xffffffff) {
                fprintf(stderr,"Could not determine base address of TFS\n");
                exit(1);
        }
        if (verbose > 1)
                fprintf(stderr,"TFS Flash Base: 0x%lx\n",flashBase);

        if (strcmp(cmd,"ls") == 0) {
                int match = 0;
                char *matchname = 0;

                if (argc == (optind + 2))
                        matchname = argv[optind+1];
                else if (argc != (optind + 1))
                        usage(1);

                ftot = 0;
                nxttfp = tfp = (struct tfshdr *)tfs_in_ram;
                while(1) {
                        if (tfp->hdrsize == 0xffff)
                                break;

                        nxttfp = nexthdr(tfp);

                        if (TFS_DELETED(tfp)) {
                                tfp = nxttfp;
                                continue;
                        }

                        if (eob_check((char *)nxttfp, end_of_tfs_in_ram,0) < 0)
                                break;

                        hdrcrcfail = filcrcfail = 0;
                        if (tfshdrcrc(tfp) != tfp->hdrcrc)
                                hdrcrcfail = 1;

                        if ((tfp->filsize > partition_size) || (tfp->filsize < 0) ||
                                (crc32((unsigned char *)(tfp+1),tfp->filsize) != tfp->filcrc))
                                filcrcfail = 1;

                        if (hdrcrcfail) {
                                fprintf(stderr,"\n\nHeader CRC failure.\n");
                                fprintf(stderr,"Terminating at offset 0x%lx\n",
                                        (unsigned long)tfp-(unsigned long)tfs_in_ram);
                                break;
                        }

                        if (matchname) {
                                if (strcmp(matchname,TFS_NAME(tfp)) == 0) {
                                        printf("%s\n",matchname);
                                        match = 1;
                                        break;
                                }
                        }
                        else {
                                if (verbose) {
                                        if (ftot == 0)
                                                printf(" Name                        Size   Location   Flags  Info\n");
                                        flags = tfsflagsbtoa(TFS_FLAGS(tfp),fbuf);
                                        if ((!flags) || (!fbuf[0]))
                                                flags = " ";
                                        printf(" %-23s  %7ld  0x%08lx  %-5s  %s\n",TFS_NAME(tfp),
                                                TFS_SIZE(tfp),
                                                (ulong)(tfp+1)-(ulong)tfs_in_ram+flashBase,
                                                flags,TFS_INFO(tfp));
                                }
                                else {
                                        printf(" %s\n",TFS_NAME(tfp));
                                }
                        }

                        tfp = nxttfp;
                        if (eob_check((char *)nxttfp, end_of_tfs_in_ram,1) < 0)
                                break;
                        ftot++;
                }
                if (verbose && !hdrcrcfail && !matchname)
                        printf("\nTotal files: %d\n",ftot);

                if (matchname && !match)
                        rc = 1;
        }
        else if (strcmp(cmd,"dump") == 0) {
                unsigned long   hdrstart;

                ftot = 0;
                nxttfp = tfp = (struct tfshdr *)tfs_in_ram;
                while(1) {
                        if (debug) {
                                fprintf(stderr,"hdr offset: 0x%lx\n",
                                (long)tfp-(long)tfs_in_ram);
                        }
                        nxttfp = nexthdr(tfp);

                        if (tfp->hdrsize == 0xffff) {
                                if (debug)
                                        showHdr(tfp);
                                break;
                        }
                        if (eob_check((char *)nxttfp, end_of_tfs_in_ram,0) < 0)
                                break;

                        hdrcrcfail = filcrcfail = 0;
                        if (tfshdrcrc(tfp) != tfp->hdrcrc)
                                hdrcrcfail = 1;

                        if ((tfp->filsize > partition_size) || (tfp->filsize < 0) ||
                                (crc32((unsigned char *)(tfp+1),tfp->filsize) != tfp->filcrc))
                                filcrcfail = 1;

                        hdrstart = flashBase + ((char *)tfp - (char *)tfs_in_ram);
                        printf("\nFile%3d:\n Hdr at: 0x%08lx %s %s\n",++ftot,hdrstart,
                                hdrcrcfail ? "hdr-crc-failure" : "",
                                filcrcfail ? "file-crc-failure" : "");

                        if (hdrcrcfail) {
                                fprintf(stderr,"\n\nTerminating at file offset 0x%lx\n",
                                        (unsigned long)tfp);
                                break;
                        }

                        showHdr(tfp);
                        tfp = nxttfp;
                        if (eob_check((char *)nxttfp, end_of_tfs_in_ram,1) < 0)
                                break;
                }
                if (!hdrcrcfail) {
                        printf("\n\n");
                        printf("Total files: %d\n",ftot);
                        printf("Last file ends at file offset 0x%x\n",
                                (char *)tfp-(char *)tfs_in_ram);
                        postTFSTest(tfs_in_ram,(unsigned char *)tfp,partition_size);
                }
        }
        else if (strcmp(cmd,"rm") == 0) {

                if (argc != (optind + 2))
                        usage(1);

                /* First we have to find the file in TFS...
                 */
                offset = get_file_offset(argv[optind+1]);
                if (offset == -1) {
                        fprintf(stderr,"Can't find file: %s\n",argv[optind+1]);
                        rc = 1;
                }
                else {
                        int fd;
                        TFILE tfshdr;

                        fd = open(mtd_filename,O_RDWR);
                        if (fd == -1) {
                                fprintf(stderr,"Partition (%s) spanning TFS is read-only\n",
                                        mtd_filename);
                        }
                        if (lseek(fd,offset,SEEK_SET) < 0) {
                                perror("rm lseek1");
                                exit(1);
                        }
                        if (read(fd,(char *)&tfshdr,sizeof(TFILE)) != sizeof(TFILE)) {
                                perror("rm read");
                                exit(1);
                        }
                        tfshdr.flags &= ~TFS_ACTIVE;

                        if (lseek(fd,offset,SEEK_SET) < 0) {
                                perror("rm lseek2");
                                exit(1);
                        }
                        if (write(fd,(char *)&tfshdr,sizeof(TFILE)) != sizeof(TFILE)) {
                                perror("rm write");
                                exit(1);
                        }
                        close(fd);
                }
        }
        else if (strcmp(cmd,"put") == 0) {      /* Copy from Linux FS to TFS */
                char *from, *to, *filedata;
                struct stat fstat;
                TFILE tfshdr;

                if (argc == optind+2)
                        from = to = argv[optind+1];
                else if (argc == (optind + 3)) {
                        from = argv[optind+1];
                        to = argv[optind+2];
                }
                else
                        usage(1);

                if (strlen(to) >= TFSNAMESIZE) {
                        fprintf(stderr,"TFS filename '%s' too long\n",to);
                        rc = 1;
                        goto done;
                }

                /* Get size of file to be copied to TFS and determine
                 * if there is enough space in TFS to add the file...
                 */
                if (stat(from,&fstat) < 0) {
                        fprintf(stderr,"Can't find file '%s' in Linux\n",from);
                        rc = 1;
                        goto done;
                }
                if ((filedata = malloc(fstat.st_size)) == 0) {
                        perror("malloc");
                        rc = 1;
                        goto done;
                }
                if ((ifd = open(from,O_RDONLY)) < 0) {
                        perror("open");
                        rc = 1;
                        goto done;
                }
                if (read(ifd,filedata,fstat.st_size) != fstat.st_size) {
                        perror("read");
                        rc = 1;
                        goto done;
                }

                /* If file exists, force user to remove it (don't do it
                 * automatically)...
                 */
                offset = get_file_offset(tfsname(to,&tfshdr));
                if (offset != -1) {
                        fprintf(stderr,"File '%s' exists in TFS\n",to);
                        rc = 1;
                }
                else {
                        int fd, ftot, sectortot;
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
                        tfsname(to,&tfshdr);
                        tfsinfo(to,&tfshdr);
                        tfsflags(to,&tfshdr);
                        tfshdr.hdrsize = TFSHDRSIZ;
                        tfshdr.hdrvrsn = TFSHDRVERSION;
                        tfshdr.filsize = fstat.st_size;
                        tfshdr.flags |= (TFS_ACTIVE | TFS_NSTALE);
                        tfshdr.filcrc = crc32(filedata,fstat.st_size);
                        tfshdr.modtime = 0xffffffff;
#if TFS_RESERVED
                        {
                        int rsvd;
                        for(rsvd=0;rsvd<TFS_RESERVED;rsvd++)
                                tfshdr.rsvd[rsvd] = 0xffffffff;
                        }
#endif
                        tfshdr.next = 0;
                        tfshdr.hdrcrc = 0;
                        tfshdr.hdrcrc = crc32((char *)&tfshdr,TFSHDRSIZ);
                        next = flashBase+offset+TFSHDRSIZ+fstat.st_size;
                        if (next & 0xf)
                                next = (next | 0xf) + 1;
                        tfshdr.next = (TFILE *)next;

                        /* Open the mtd file to write the new data...
                         */
                        fd = open(mtd_filename,O_RDWR);
                        if (fd == -1) {
                                fprintf(stderr,"Partition (%s) spanning TFS is read-only\n",
                                        mtd_filename);
                        }
                        if (lseek(fd,offset+TFSHDRSIZ,SEEK_SET) < 0) {
                                perror("rm lseek1");
                                exit(1);
                        }
                        if (write(fd,filedata,fstat.st_size) != fstat.st_size) {
                                perror("rm write");
                                exit(1);
                        }

                        if (lseek(fd,offset,SEEK_SET) < 0) {
                                perror("rm lseek2");
                                exit(1);
                        }
                        if (write(fd,(char *)&tfshdr,TFSHDRSIZ) != sizeof(TFILE)) {
                                perror("rm write");
                                exit(1);
                        }
                        close(fd);
                }
        }
        else if (strcmp(cmd,"get") == 0) {      /* Copy from TFS to Linux */
                char *from, *to;

                if (argc == optind+2)
                        from = to = argv[optind+1];
                else if (argc == (optind + 3)) {
                        from = argv[optind+1];
                        to = argv[optind+2];
                }
                else
                        usage(1);

                offset = get_file_offset(from);
                if (offset == -1) {
                        fprintf(stderr,"Can't find file: '%s' in TFS\n",from);
                        rc = 1;
                }
                else {
                        tfp = (TFILE *)(tfs_in_ram+offset);

                        ofd = open(to,O_RDWR | O_CREAT | O_TRUNC,0777);
                        if (ofd < 0) {
                                perror("open");
                                rc = 1;
                        }
                        size = write(ofd,(char *)(tfp+1),tfp->filsize);

                        if (size != tfp->filsize) {
                                perror("write");
                                rc = 1;
                        }
                        close(ofd);
                }
        }
        else {
                fprintf(stderr,"Cmd: %s not recognized\n",cmd);
                rc = 1;
        }

done:
        free(tfs_in_ram);
        exit(rc);
}
