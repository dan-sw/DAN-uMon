/*----------------------------------------------------------------------*/
/* FAT file system sample project for FatFs            (C)ChaN, 2009    */
/*----------------------------------------------------------------------*/


#include <string.h>
#include "integer.h"
//#include "comm.h"
//#include "monitor.h"
//#include "rtc.h"
#include "diskio.h"
#include "ff.h"
#include "monlib.h"
#include "date.h"

#define xprintf mon_printf
#define xputs mon_printf
#define xputc mon_putchar
#define get_line(a,b)	mon_getline(a,b,0)

DWORD acc_size;				/* Work register for fs command */
WORD acc_files, acc_dirs;
FILINFO Finfo;
#if _USE_LFN
char Lfname[512];
#endif

char Line[120];				/* Console input buffer */

FATFS Fatfs[_DRIVES];		/* File system object for each logical drive */
FIL File1, File2;			/* File objects */
DIR Dir;					/* Directory object */
BYTE Buff[16384] __attribute__ ((aligned (4))) ;		/* Working buffer */

volatile UINT Timer;		/* Performance timer (1kHz increment) */


DWORD get_fattime ()
{
	struct todinfo tod;

	/* Get local time (if possible) */
	if (mon_timeofday(TOD_GET,&tod) < 0)
		return((29 << 25) | (12<<21) |  (10<<16) | (20<<11) | (30<<5) | 0); 

	
	return	  ((DWORD)((DWORD)(tod.year - 1980) << 25)
			| ((DWORD)tod.month << 21)
			| ((DWORD)tod.date << 16)
			| ((DWORD)tod.hour << 11)
			| ((DWORD)tod.minute << 5)
			| ((DWORD)tod.second >> 1));
}

int xatoi (char **str, long *res)
{
	DWORD val;
	BYTE c, radix, s = 0;


	while ((c = **str) == ' ') (*str)++;
	if (c == '-') {
		s = 1;
		c = *(++(*str));
	}
	if (c == '0') {
		c = *(++(*str));
		if (c <= ' ') {
			*res = 0; return 1;
		}
		if (c == 'x') {
			radix = 16;
			c = *(++(*str));
		} else {
			if (c == 'b') {
				radix = 2;
				c = *(++(*str));
			} else {
				if ((c >= '0')&&(c <= '9'))
					radix = 8;
				else
					return 0;
			}
		}
	} else {
		if ((c < '1')||(c > '9'))
			return 0;
		radix = 10;
	}
	val = 0;
	while (c > ' ') {
		if (c >= 'a') c -= 0x20;
		c -= '0';
		if (c >= 17) {
			c -= 7;
			if (c <= 9) return 0;
		}
		if (c >= radix) return 0;
		val = val * radix + c;
		c = *(++(*str));
	}
	if (s) val = -val;
	*res = val;
	return 1;
}

void put_dump (const BYTE *buff, DWORD ofs, int cnt)
{
	BYTE n;


	xprintf("%08lX ", ofs);
	for(n = 0; n < cnt; n++)
		xprintf(" %02X", buff[n]);
	xputc(' ');
	for(n = 0; n < cnt; n++) {
		if ((buff[n] < 0x20)||(buff[n] >= 0x7F))
			xputc('.');
		else
			xputc(buff[n]);
	}
	xputc('\n');
}

/*--------------------------------------------------------------------------*/
/* Monitor                                                                  */
/*--------------------------------------------------------------------------*/

static
FRESULT scan_files (
	char* path		/* Pointer to the path name working buffer */
)
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
//				xprintf("%s/%s\n", path, fn);
				acc_files++;
				acc_size += Finfo.fsize;
			}
		}
	}

	return res;
}



static
void put_rc (FRESULT rc)
{
	const char *p;
	static const char str[] =
		"OK\0" "DISK_ERR\0" "INT_ERR\0" "NOT_READY\0" "NO_FILE\0" "NO_PATH\0"
		"INVALID_NAME\0" "DENIED\0" "EXIST\0" "INVALID_OBJECT\0" "WRITE_PROTECTED\0"
		"INVALID_DRIVE\0" "NOT_ENABLED\0" "NO_FILE_SYSTEM\0" "MKFS_ABORTED\0" "TIMEOUT\0";
	FRESULT i;

	for (p = str, i = 0; i != rc && *p; i++) {
		while(*p++);
	}
	xprintf("rc=%u FR_%s\n", (UINT)rc, p);
}



int sample (void)
{
	char *ptr, *ptr2;
	long p1, p2, p3;
	BYTE res, b1;
	WORD w1;
	UINT s1, s2, cnt, blen = sizeof(Buff);
	DWORD ofs = 0, sect = 0;
	FATFS *fs;				/* Pointer to file system object */

	xputs("\nFatFs module test monitor for LPC2300/MCI\n");
	xputs("\n(run 'di' and 'fi' to initialize)\n");

	for (;;) {
		xputc('>');
		ptr = Line;
		get_line(ptr, sizeof(Line));

		switch (*ptr++) {

		case 'm' :
			switch (*ptr++) {
			case 'd' :	/* md <address> [<count>] - Dump memory */
				if (!xatoi(&ptr, &p1)) break;
				if (!xatoi(&ptr, &p2)) p2 = 128;
				for (ptr=(char*)p1; p2 >= 16; ptr += 16, p2 -= 16)
					put_dump((BYTE*)ptr, (UINT)ptr, 16);
				if (p2) put_dump((BYTE*)ptr, (UINT)ptr, p2);
				break;
			}
			break;

		case 'd' :
			switch (*ptr++) {
			case 'd' :	/* dd [<lba>] - Dump secrtor */
				if (!xatoi(&ptr, &p2)) p2 = sect;
				res = disk_read(0, Buff, p2, 1);
				if (res) { xprintf("rc=%d\n", (WORD)res); break; }
				sect = p2 + 1;
				xprintf("Sector:%lu\n", p2);
				for (ptr=(char*)Buff, ofs = 0; ofs < 0x200; ptr+=16, ofs+=16)
					put_dump((BYTE*)ptr, ofs, 16);
				break;

			case 'i' :	/* di - Initialize disk */
				xprintf("rc=%d\n", (WORD)disk_initialize(0));
				break;

			case 's' :	/* ds - Show disk status */
				if (disk_ioctl(0, GET_SECTOR_COUNT, &p2) == RES_OK)
					{ xprintf("Drive size: %lu sectors\n", p2); }
				if (disk_ioctl(0, GET_SECTOR_SIZE, &w1) == RES_OK)
					{ xprintf("Sector size: %u\n", w1); }
				if (disk_ioctl(0, GET_BLOCK_SIZE, &p2) == RES_OK)
					{ xprintf("Erase block size: %lu sectors\n", p2); }
				if (disk_ioctl(0, MMC_GET_TYPE, &b1) == RES_OK)
					{ xprintf("MMC/SDC type: %u\n", b1); }
				if (disk_ioctl(0, MMC_GET_CSD, Buff) == RES_OK)
					{ xputs("CSD:\n"); put_dump(Buff, 0, 16); }
				if (disk_ioctl(0, MMC_GET_CID, Buff) == RES_OK)
					{ xputs("CID:\n"); put_dump(Buff, 0, 16); }
				if (disk_ioctl(0, MMC_GET_OCR, Buff) == RES_OK)
					{ xputs("OCR:\n"); put_dump(Buff, 0, 4); }
				if (disk_ioctl(0, MMC_GET_SDSTAT, Buff) == RES_OK) {
					xputs("SD Status:\n");
					for (s1 = 0; s1 < 64; s1 += 16) put_dump(Buff+s1, s1, 16);
				}
				break;
			}
			break;

		case 'b' :
			switch (*ptr++) {
			case 'd' :	/* bd <addr> - Dump R/W buffer */
				if (!xatoi(&ptr, &p1)) break;
				for (ptr=(char*)&Buff[p1], ofs = p1, cnt = 32; cnt; cnt--, ptr+=16, ofs+=16)
					put_dump((BYTE*)ptr, ofs, 16);
				break;

			case 'e' :	/* be <addr> [<data>] ... - Edit R/W buffer */
				if (!xatoi(&ptr, &p1)) break;
				if (xatoi(&ptr, &p2)) {
					do {
						Buff[p1++] = (BYTE)p2;
					} while (xatoi(&ptr, &p2));
					break;
				}
				for (;;) {
					xprintf("%04X %02X-", (WORD)(p1), (WORD)Buff[p1]);
					get_line(Line, sizeof(Line));
					ptr = Line;
					if (*ptr == '.') break;
					if (*ptr < ' ') { p1++; continue; }
					if (xatoi(&ptr, &p2))
						Buff[p1++] = (BYTE)p2;
					else
						xputs("???\n");
				}
				break;

			case 'r' :	/* br <lba> [<num>] - Read disk into R/W buffer */
				if (!xatoi(&ptr, &p2)) break;
				if (!xatoi(&ptr, &p3)) p3 = 1;
				xprintf("rc=%u\n", (WORD)disk_read(0, Buff, p2, p3));
				break;

			case 'w' :	/* bw <lba> [<num>] - Write R/W buffer into disk */
				if (!xatoi(&ptr, &p2)) break;
				if (!xatoi(&ptr, &p3)) p3 = 1;
				xprintf("rc=%u\n", (WORD)disk_write(0, Buff, p2, p3));
				break;

			case 'f' :	/* bf <val> - Fill working buffer */
				if (!xatoi(&ptr, &p1)) break;
				memset(Buff, (BYTE)p1, sizeof(Buff));
				break;

			}
			break;

		case 'f' :
			switch (*ptr++) {

			case 'i' :	/* fi - Force initialized the logical drive */
				put_rc(f_mount(0, &Fatfs[0]));
				break;

			case 's' :	/* fs - Show logical drive status */
				res = f_getfree("", (DWORD*)&p2, &fs);
				if (res) { put_rc(res); break; }
				xprintf("FAT type = %u\nBytes/Cluster = %lu\nNumber of FATs = %u\n"
						"Root DIR entries = %u\nSectors/FAT = %lu\nNumber of clusters = %lu\n"
						"FAT start (lba) = %lu\nDIR start (lba,clustor) = %lu\nData start (lba) = %lu\n\n...",
						(WORD)fs->fs_type, (DWORD)fs->csize * 512, (WORD)fs->n_fats,
						fs->n_rootdir, fs->sects_fat, (DWORD)fs->max_clust - 2,
						fs->fatbase, fs->dirbase, fs->database
				);
				acc_size = acc_files = acc_dirs = 0;
#if _USE_LFN
				Finfo.lfname = Lfname;
				Finfo.lfsize = sizeof(Lfname);
#endif
				res = scan_files(ptr);
				if (res) { put_rc(res); break; }
				xprintf("\r%u files, %lu bytes.\n%u folders.\n"
						"%lu KB total disk space.\n%lu KB available.\n",
						acc_files, acc_size, acc_dirs,
						(fs->max_clust - 2) * (fs->csize / 2), p2 * (fs->csize / 2)
				);
				break;

			case 'l' :	/* fl [<path>] - Directory listing */
				while (*ptr == ' ') ptr++;
				res = f_opendir(&Dir, ptr);
				if (res) { put_rc(res); break; }
				p1 = s1 = s2 = 0;
				for(;;) {
#if _USE_LFN
					Finfo.lfname = Lfname;
					Finfo.lfsize = sizeof(Lfname);
#endif
					res = f_readdir(&Dir, &Finfo);
					if ((res != FR_OK) || !Finfo.fname[0]) break;
					if (Finfo.fattrib & AM_DIR) {
						s2++;
					} else {
						s1++; p1 += Finfo.fsize;
					}
					xprintf("%c%c%c%c%c %u/%02u/%02u %02u:%02u %9lu  %s",
							(Finfo.fattrib & AM_DIR) ? 'D' : '-',
							(Finfo.fattrib & AM_RDO) ? 'R' : '-',
							(Finfo.fattrib & AM_HID) ? 'H' : '-',
							(Finfo.fattrib & AM_SYS) ? 'S' : '-',
							(Finfo.fattrib & AM_ARC) ? 'A' : '-',
							(Finfo.fdate >> 9) + 1980, (Finfo.fdate >> 5) & 15, Finfo.fdate & 31,
							(Finfo.ftime >> 11), (Finfo.ftime >> 5) & 63,
							Finfo.fsize, &(Finfo.fname[0]));
#if _USE_LFN
					for (p2 = strlen(Finfo.fname); p2 < 14; p2++)
						xputc(' ');
					xprintf("%s\n", Lfname);
#else
					xputc('\n');
#endif
				}
				xprintf("%4u File(s),%10lu bytes total\n%4u Dir(s)", s1, p1, s2);
				if (f_getfree(ptr, (DWORD*)&p1, &fs) == FR_OK)
					xprintf(", %10lu bytes free\n", p1 * fs->csize * 512);
				break;

            case 'C':		/* Concatenate file */
				{
				int i;
				char mybuf[1024];
				while (*ptr == ' ') ptr++;
				put_rc(f_open(&File1, ptr, (BYTE)FA_READ));
more:
				res = f_read(&File1, mybuf, sizeof(mybuf), &s2);
				if (res != FR_OK) { put_rc(res); break; }
				for(i=0;i<s2;i++) {
					mon_putchar(mybuf[i]);
				}
				if (s2 == sizeof(mybuf)) goto more;
				put_rc(f_close(&File1));
				}
				break;
			case 'o' :	/* fo <mode> <file> - Open a file */
				if (!xatoi(&ptr, &p1)) break;
				while (*ptr == ' ') ptr++;
				put_rc(f_open(&File1, ptr, (BYTE)p1));
				break;

			case 'c' :	/* fc - Close a file */
				put_rc(f_close(&File1));
				break;

			case 'e' :	/* fe - Seek file pointer */
				if (!xatoi(&ptr, &p1)) break;
				res = f_lseek(&File1, p1);
				put_rc(res);
				if (res == FR_OK)
					xprintf("fptr=%lu(0x%lX)\n", File1.fptr, File1.fptr);
				break;

			case 'd' :	/* fd <len> - read and dump file from current fp */
				if (!xatoi(&ptr, &p1)) break;
				ofs = File1.fptr;
				while (p1) {
					if ((UINT)p1 >= 16) { cnt = 16; p1 -= 16; }
					else 				{ cnt = p1; p1 = 0; }
					res = f_read(&File1, Buff, cnt, &cnt);
					if (res != FR_OK) { put_rc(res); break; }
					if (!cnt) break;
					put_dump(Buff, ofs, cnt);
					ofs += 16;
				}
				break;

			case 'r' :	/* fr <len> - read file */
				if (!xatoi(&ptr, &p1)) break;
				p2 = 0;
				Timer = 0;
				while (p1) {
					if ((UINT)p1 >= blen) {
						cnt = blen; p1 -= blen;
					} else {
						cnt = p1; p1 = 0;
					}
					res = f_read(&File1, Buff, cnt, &s2);
					if (res != FR_OK) { put_rc(res); break; }
					p2 += s2;
					if (cnt != s2) break;
				}
				xprintf("%lu bytes read with %lu kB/sec.\n", p2, p2 / Timer);
				break;

			case 'w' :	/* fw <len> <val> - write file */
				if (!xatoi(&ptr, &p1) || !xatoi(&ptr, &p2)) break;
				memset(Buff, (BYTE)p2, blen);
				p2 = 0;
				Timer = 0;
				while (p1) {
					if ((UINT)p1 >= blen) {
						cnt = blen; p1 -= blen;
					} else {
						cnt = p1; p1 = 0;
					}
					res = f_write(&File1, Buff, cnt, &s2);
					if (res != FR_OK) { put_rc(res); break; }
					p2 += s2;
					if (cnt != s2) break;
				}
				xprintf("%lu bytes written with %lu kB/sec.\n", p2, p2 / Timer);
				break;

			case 'n' :	/* fn <old_name> <new_name> - Change file/dir name */
				while (*ptr == ' ') ptr++;
				ptr2 = strchr(ptr, ' ');
				if (!ptr2) break;
				*ptr2++ = 0;
				while (*ptr2 == ' ') ptr2++;
				put_rc(f_rename(ptr, ptr2));
				break;

			case 'u' :	/* fu <name> - Unlink a file or dir */
				while (*ptr == ' ') ptr++;
				put_rc(f_unlink(ptr));
				break;

			case 'v' :	/* fv - Truncate file */
				put_rc(f_truncate(&File1));
				break;

			case 'k' :	/* fk <name> - Create a directory */
				while (*ptr == ' ') ptr++;
				put_rc(f_mkdir(ptr));
				break;

			case 'a' :	/* fa <atrr> <mask> <name> - Change file/dir attribute */
				if (!xatoi(&ptr, &p1) || !xatoi(&ptr, &p2)) break;
				while (*ptr == ' ') ptr++;
				put_rc(f_chmod(ptr, p1, p2));
				break;

			case 't' :	/* ft <year> <month> <day> <hour> <min> <sec> <name> - Change timestamp */
				if (!xatoi(&ptr, &p1) || !xatoi(&ptr, &p2) || !xatoi(&ptr, &p3)) break;
				Finfo.fdate = ((p1 - 1980) << 9) | ((p2 & 15) << 5) | (p3 & 31);
				if (!xatoi(&ptr, &p1) || !xatoi(&ptr, &p2) || !xatoi(&ptr, &p3)) break;
				Finfo.ftime = ((p1 & 31) << 11) | ((p2 & 63) << 5) | ((p3 >> 1) & 31);
				put_rc(f_utime(ptr, &Finfo));
				break;

			case 'x' : /* fx <src_name> <dst_name> - Copy file */
				while (*ptr == ' ') ptr++;
				ptr2 = strchr(ptr, ' ');
				if (!ptr2) break;
				*ptr2++ = 0;
				while (*ptr2 == ' ') ptr2++;
				xprintf("Opening \"%s\"", ptr);
				res = f_open(&File1, ptr, FA_OPEN_EXISTING | FA_READ);
				xputc('\n');
				if (res) {
					put_rc(res);
					break;
				}
				xprintf("Creating \"%s\"", ptr2);
				res = f_open(&File2, ptr2, FA_CREATE_ALWAYS | FA_WRITE);
				xputc('\n');
				if (res) {
					put_rc(res);
					f_close(&File1);
					break;
				}
				xprintf("Copying file...");
				Timer = 0;
				p1 = 0;
				for (;;) {
					res = f_read(&File1, Buff, blen, &s1);
					if (res || s1 == 0) break;   /* error or eof */
					res = f_write(&File2, Buff, s1, &s2);
					p1 += s2;
					if (res || s2 < s1) break;   /* error or disk full */
				}
				xprintf("%lu bytes copied with %lu kB/sec.\n", p1, p1 / Timer);
				f_close(&File1);
				f_close(&File2);
				break;
#if _FS_RPATH
			case 'g' :	/* fg <path> - Change current directory */
				while (*ptr == ' ') ptr++;
				put_rc(f_chdir(ptr));
				break;

			case 'j' :	/* fj <drive#> - Change current drive */
				if (xatoi(&ptr, &p1)) {
					put_rc(f_chdrive((BYTE)p1));
				}
				break;
#endif
#if _USE_MKFS
			case 'm' :	/* fm <partition rule> <cluster size> - Create file system */
				if (!xatoi(&ptr, &p2) || !xatoi(&ptr, &p3)) break;
				xprintf("The card will be formatted. Are you sure? (Y/n)=");
				get_line(ptr, sizeof(Line));
				if (*ptr == 'Y')
					put_rc(f_mkfs(0, (BYTE)p2, (WORD)p3));
				break;
#endif
			case 'z' :	/* fz [<rw size>] - Change R/W length for fr/fw/fx command */
				if (xatoi(&ptr, &p1) && p1 >= 1 && p1 <= sizeof(Buff))
					blen = p1;
				xprintf("blen=%u\n", blen);
				break;
			}
			break;

#if 0
		case 't' :	/* t [<year> <mon> <mday> <hour> <min> <sec>] */
			if (xatoi(&ptr, &p1)) {
				rtc.year = (WORD)p1;
				xatoi(&ptr, &p1); rtc.month = (BYTE)p1;
				xatoi(&ptr, &p1); rtc.mday = (BYTE)p1;
				xatoi(&ptr, &p1); rtc.hour = (BYTE)p1;
				xatoi(&ptr, &p1); rtc.min = (BYTE)p1;
				if (!xatoi(&ptr, &p1)) break;
				rtc.sec = (BYTE)p1;
				rtc_settime(&rtc);
			}
			rtc_gettime(&rtc);
			xprintf("%u/%u/%u %02u:%02u:%02u\n", rtc.year, rtc.month, rtc.mday, rtc.hour, rtc.min, rtc.sec);
			break;
#endif
		case 'x':
			mon_appexit(0);
			break;

		}
	}
}

