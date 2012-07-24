/* newlib_hooks.c:
 * This is a basic set of hooks that allow some of the stuff
 * in newlib to interface to a MicroMonitor based application.
 *
 * It hooks newlib to:
 * 	- uMon's console io (mon_putchar() & mon_getchar())
 * 	- uMon's file system (TFS)
 *	- uMon's environment (mon_getenv() & mon_setenv())
 *	- uMon's memory allocator (mon_malloc(), mon_free(), mon_realloc())
 *
 * The code assumes that fds 0, 1 & 2 are in/out/err and open() will
 * not be called for these.  All fds above that are for TFS files.
 * This version simply maps the incoming flags to as close of a fit
 * to a corresponding TFS operation as possible.
 *
 * WARNING: not heavily tested.
 *
 * NOTE: this "may" be thread safe if tfdlock() and newlib_tfdunlock()
 * are defined; however, I haven't looked at it very close.
 *
 * Author: Ed Sutter, with quite a bit of help from an article written
 * by Bill Gatliff http://billgatliff.com/articles/newlib/newlib.
 */
#include <stdio.h>
#include <reent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "monlib.h"

#define REENT struct _reent
#define MAXFILESIZE 0x4000

/* NEWLIB_TRACE:
 * When set, the shell variable "NEWLIB_TRACE" controls verbosity
 * within this module.
 */
#ifndef NEWLIB_TRACE
#define NEWLIB_TRACE 0
#endif

#define MAXTFDS	15

/* Define these for thread safety...
 */
#ifndef newlib_tfdlock
#define newlib_tfdlock()
#endif

#ifndef newlib_tfdunlock
#define newlib_tfdunlock()
#endif

/* TFS file descriptor info:
 */
struct tfdinfo {
	int	inuse;
	int	tfd;
	char *buf;
	char name[TFSNAMESIZE+1];
	char info[TFSNAMESIZE+1];
} tfdtable[MAXTFDS];


static void
newlib_not_supported(REENT *rptr,char *hook)
{
	mon_printf("NEWLIB HOOK '%s', NOT YET SUPPORTED\n",hook);
	rptr->_errno = ENOTSUP;
}

/*******************************************************************
 *******************************************************************
 *
 * Hooks to TFS:
 * (read & write also hook to console IO)
 */

/* open():
 * Attempts to remap the incoming flags to TFS equivalent.
 * Its not a perfect mapping, but gets pretty close.
 * A comma-delimited path is supported to allow the user
 * to specify TFS-stuff (flag string, info string, and a buffer).
 * For example:
 *	abc,e,script,0x400000
 *	This is a file called "abc" that will have the TFS 'e' flag
 *	and the TFS info field of "script".  The storage buffer is
 *  supplied by the user at 0x400000.
 */
int
_open_r(REENT *rptr, const char *path, int flags, int mode)
{
	static int beenhere = 0;
	long flagmode;
	int	tfdidx, tfd;
	struct tfdinfo *tip;
	char *buf, *fstr, *istr, *bstr, pathcopy[TFSNAMESIZE*3+1];

#if NEWLIB_TRACE
	if (mon_getenv("NEWLIB_TRACE"))
		mon_printf("_open_r(%s,0x%x,0x%x)\n",path,flags,mode);
#endif

	if (!beenhere) {
		newlib_tfdlock();
		for(tfdidx=0;tfdidx<MAXTFDS;tfdidx++)
			tfdtable[tfdidx].inuse = 0;		

		tfdtable[0].inuse = 1;		/* fake entry for stdin */
		tfdtable[1].inuse = 1;		/* fake entry for stdout */
		tfdtable[2].inuse = 1;		/* fake entry for stderr */
		newlib_tfdunlock();
		beenhere = 1;
	}

	istr = fstr = bstr = buf = (char *)0;

	/* Copy the incoming path to a local array so that we can safely
	 * modify the string...
 	 */
	if (strlen(path) > TFSNAMESIZE*3) {
		rptr->_errno = ENAMETOOLONG;
		return(-1);
	}
	strcpy(pathcopy,path);

	/* The incoming string may have commas that are used to delimit the
	 * name from the TFS flag string, TFS info string and buffer.
	 * Check for the commas and test for maximum string length...
	 */
	fstr = strchr(pathcopy,',');
	if (fstr)  {
		*fstr++ = 0;
		istr = strchr(fstr,',');
		if (istr) {
			*istr++ = 0;
			bstr = strchr(istr,',');
			if (bstr)
				*bstr++ = 0;
		}
	}
	if (strlen(pathcopy) > TFSNAMESIZE) {
		rptr->_errno = ENAMETOOLONG;
		return(-1);
	}
	if (istr) {
		if (strlen(istr) > TFSNAMESIZE) {
			rptr->_errno = ENAMETOOLONG;
			return(-1);
		}
	}

	/* If O_EXCL and O_CREAT are set, then fail if the file exists...
	 */
	if ((flags & (O_CREAT | O_EXCL)) == (O_CREAT | O_EXCL)) {
		if (mon_tfsstat((char *)pathcopy)) {
			rptr->_errno = EEXIST;
			return(-1);
		}
	}

	/* Only a few flag combinations are supported...
	 * O_RDONLY						 Simple read-only
	 * O_WRONLY | O_APPEND			 Each write starts at end of file
	 * O_WRONLY | O_TRUNC			 If file exists, truncate it
	 * O_WRONLY | O_CREAT			 Create if it doesn't exist
	 * O_WRONLY | O_CREAT | O_EXCL	 Fail if file exists
	 */
	switch(flags) {
		case O_RDONLY:
			flagmode = TFS_RDONLY;
			break;
		case O_WRONLY|O_APPEND:
			flagmode = TFS_APPEND;
			break;
		case O_WRONLY|O_TRUNC:
		case O_WRONLY|O_CREAT|O_TRUNC:
			mon_tfsunlink((char *)pathcopy);
			flagmode = TFS_CREATE|TFS_APPEND;
			break;
		case O_WRONLY|O_CREAT:
		case O_WRONLY|O_CREAT|O_APPEND:
			flagmode = TFS_CREATE|TFS_APPEND;
			break;
		case O_RDWR:
		case O_WRONLY|O_CREAT|O_EXCL:
			flagmode = TFS_CREATE|TFS_APPEND;
			break;
		default:
			mon_printf("_open_r(): flag 0x%x not supported\n",flags);
			rptr->_errno = ENOTSUP;
			return(-1);
	}

	/* Find an open slot in our tfd table:
	 */
	newlib_tfdlock();
	for(tfdidx=0;tfdidx<MAXTFDS;tfdidx++) {
		if (tfdtable[tfdidx].inuse == 0)
			break;
	}
	if (tfdidx == MAXTFDS) {
		newlib_tfdunlock();
		rptr->_errno = EMFILE;
		return(-1);
	}
	tip = &tfdtable[tfdidx];
	tip->inuse = 1;
	newlib_tfdunlock();

	/* If file is opened for something other than O_RDONLY, then
	 * we need to allocate a buffer for the file..
	 * WARNING: It is the user's responsibility to make sure that
	 * the file size does not exceed this buffer.  Note that the
	 * buffer may be specified as part of the comma-delimited path.
	 */
	if (flagmode == TFS_RDONLY) {
		buf = (char *)0;
	}
	else {
		if (bstr)
			buf = (char *)strtol(bstr,0,0);
		else
			buf = mon_malloc(MAXFILESIZE);
		if (!buf) {
			newlib_tfdlock();
			tip->inuse = 0;	
			newlib_tfdunlock();
			rptr->_errno = ENOMEM;
			return(-1);
		}
	}

	/* Deal with tfs flags and tfs info fields if necessary:
	 */
	if (fstr) {
		long bflag;

		bflag = mon_tfsctrl(TFS_FATOB,(long)fstr,0);
		if (bflag == -1) {
			rptr->_errno = EINVAL;
			return(-1);
		}
		flagmode |= bflag;
	}

	if (istr) 
		strcpy(tip->info,istr);
	else 
		tip->info[0] = 0;

	tfd = mon_tfsopen((char *)pathcopy,flagmode,buf);
	if (tfd >= 0) {
		tip->tfd = tfd;	
		tip->buf = buf;	
		strcpy(tip->name,pathcopy);
		return(tfdidx);
	}
	else {
		mon_printf("%s: %s\n",pathcopy,
			(char *)mon_tfsctrl(TFS_ERRMSG,tfd,0));
	}

	if (buf)
		mon_free(buf);

	newlib_tfdlock();
	tip->inuse = 0;	
	newlib_tfdunlock();
	rptr->_errno = EINVAL;
	return(-1);
}

int
_close_r(REENT *rptr, int fd)
{
	char *info;
	struct tfdinfo *tip;

#if NEWLIB_TRACE
	if (mon_getenv("NEWLIB_TRACE"))
		mon_printf("_close_r(%d)\n",fd);
#endif

	if ((fd < 3) || (fd >= MAXTFDS)) {
		rptr->_errno = EBADF;
		return(-1);
	}

	tip = &tfdtable[fd];

	if (tip->info[0])
		info = tip->info;
	else
		info = (char *)0;

	mon_tfsclose(tip->tfd,info);

	if (tip->buf)
		mon_free(tip->buf);

	newlib_tfdlock();
	tip->inuse = 0;	
	newlib_tfdunlock();
	rptr->_errno = 0;
	return(0);
}

_ssize_t
_write_r(REENT *rptr, int fd, const void *ptr, size_t len)
{
	int	i, ret;

#if NEWLIB_TRACE
	if (mon_getenv("NEWLIB_TRACE"))
		mon_printf("_write_r(%d,%d)\n",fd,len);
#endif

	if ((fd == 1) || (fd == 2)) {
		for(i=0;i<len;i++) {
			if (*(char *)ptr == '\n')
				mon_putchar('\r');
			mon_putchar(*(char *)ptr++);
		}
		return(len);
	}
	if ((fd <= 0) || (fd >= MAXTFDS)) {
		rptr->_errno = EBADF;
		return(-1);
	}

	ret = mon_tfswrite(tfdtable[fd].tfd,(char *)ptr,len);
	if (ret < 0)
		return(-1);
	else
		return(ret);
}

_ssize_t
_read_r(REENT *rptr, int fd, void *ptr, size_t len)
{
	int	i, ret;
	char c, *array;

#if NEWLIB_TRACE
	if (mon_getenv("NEWLIB_TRACE"))
		mon_printf("_read_r(%d,%d)\n",fd,len);
#endif

	array = (char *)ptr;

	if (fd == 0) {				/* stdin? */
		for(i=0;i<len;i++) {
			c = (char)mon_getchar();
			if (c == '\b') {
				if (i > 0) {
					mon_printf("\b \b");
					i--;
				}
				i--;
			}
			else if ((c == '\r') || (c == '\n')) {
				array[i] = '\n';
				mon_printf("\r\n");
				break;
			}
			else {
				array[i] = c;
				mon_putchar(c);
			}
		}
		ret = i+1;
	}
	else {
		if ((fd < 3) || (fd >= MAXTFDS)) {
			rptr->_errno = EBADF;
			return(-1);
		}

		ret = mon_tfsread(tfdtable[fd].tfd,ptr,len);
		if (ret == TFSERR_EOF)
			ret = 0;
	}
	return(ret);
}

int
_fstat_r(REENT *rptr, int fd, struct stat *sp)
{
#if NEWLIB_TRACE
	if (mon_getenv("NEWLIB_TRACE"))
		printf("_fstat_r(%d)\n",fd);
#endif
	newlib_not_supported(rptr,"_fstat_r");
	return(-1);
}

int
stat(const char *path, struct stat *buf)
{
	TFILE	stat;

#if NEWLIB_TRACE
	if (mon_getenv("NEWLIB_TRACE"))
		printf("stat(%s)\n",path);
#endif

	if (mon_tfsfstat((char *)path,&stat) == -1)
		return(-1);

	/* Needs more work; however, in most apps, this is all
	 * stat is used for...
	 */
	buf->st_size = stat.filsize;
	return(0);
}

_off_t
_lseek_r(REENT *rptr, int fd, _off_t offset, int whence)
{
	int ret;

#if NEWLIB_TRACE
	if (mon_getenv("NEWLIB_TRACE"))
		printf("_lseek_r(%d,%ld,%d)\n",fd,offset,whence);
#endif

	switch (whence) {
		case SEEK_END:
			mon_printf("_lseek_r() doesn't support SEEK_END\n");
			return(-1);
		case SEEK_CUR:
			whence = TFS_CURRENT;
			break;
		case SEEK_SET:
			whence = TFS_BEGIN;
			break;
	}
	ret = mon_tfsseek(tfdtable[fd].tfd,offset,whence);

	if (ret < 0)
		return(-1);

	return(ret);
}

int
_link_r(REENT *rptr, const char *oldname, const char *newname)
{
#if NEWLIB_TRACE
	if (mon_getenv("NEWLIB_TRACE"))
		printf("_link_r(%s,%s)\n",oldname,newname);
#endif

	newlib_not_supported(rptr,"_link_r");
	return(-1);
}

int
_unlink_r(REENT *rptr, const char *name)
{
#if NEWLIB_TRACE
	if (mon_getenv("NEWLIB_TRACE"))
		printf("_unlink_r(%s)\n",name);
#endif

	mon_tfsunlink((char *)name);
	return(-1);
}

int
unlink(const char *name)
{
#if NEWLIB_TRACE
	if (mon_getenv("NEWLIB_TRACE"))
		printf("unlink(%s)\n",name);
#endif

	mon_tfsunlink((char *)name);
	return(0);
}

/*******************************************************************
 *******************************************************************
 *
 * Environment variable stuff:
 */
char *
getenv(const char *name)
{
	return(mon_getenv((char *)name));
}

int
setenv(const char *name, const char *val, int clobber)
{
	if (clobber && mon_getenv((char *)name))
		return(0);

	mon_setenv((char *)name,(char *)val);
	return(1);
}

int
putenv(const char *string)	/* takes a "name=value" string */
{
	extern char *strdup(), *strchr();
	char *copy, *eq;
	
	copy = strdup(string);
	if (copy) {
		eq = strchr(copy,'=');
		if (eq) {
			*eq = 0;
			mon_setenv(copy,eq+1);
			mon_free(copy);
			return(0);		/* Succeeded */
		}
		mon_free(copy);
	}
	return(1);				/* Failed */
}


/*******************************************************************
 *******************************************************************
 *
 * Memory allocation stuff:
 * The deallocXXX functions support the optional capability to have
 * this code automatically release memory that was allocated by the
 * application but not freed...
 *
 */
#ifdef DEALLOC_MAX
static void *dealloc_tbl[DEALLOC_MAX];
static int dealloc_hwm, dealloc_tot;

static void
dealloc_add(void *entry)
{
	void **vptr = dealloc_tbl;

	if (++dealloc_tot > dealloc_hwm)
		dealloc_hwm = dealloc_tot;

	while (vptr < &dealloc_tbl[DEALLOC_MAX]) {
		if (*vptr  == 0) {
			*vptr = entry;
			return;
		}
		vptr++;
	}
}

static void
dealloc_del(void *entry)
{
	void **vptr = dealloc_tbl;

	dealloc_tot--;
	while (vptr < &dealloc_tbl[DEALLOC_MAX]) {
		if (*vptr  == entry) {
			*vptr = 0;
			return;
		}
		vptr++;
	}
}

static void
deallocate(void)
{
	void **vptr = dealloc_tbl;

	while (vptr < &dealloc_tbl[DEALLOC_MAX]) {
		if (*vptr != 0) {
			mon_free((char *)*vptr);
		}
		vptr++;
	}
	if (mon_getenv("NEWLIB_TRACE")) {
		if (dealloc_hwm > DEALLOC_MAX) {
			mon_printf("Dealloc@exit high water mark = %d (max=%d)\n",
				dealloc_hwm,DEALLOC_MAX);
		}
		mon_printf("Dealloc@exit released %d blocks\n",dealloc_tot);
	}
}
#else
#define dealloc_add(a)
#define dealloc_del(a)
#define deallocate()
#endif


void *
_malloc_r(REENT *rptr, size_t size)
{
	void *mem;

	mem = mon_malloc(size);
	dealloc_add(mem);
	return(mem);
}

void *
_calloc_r(REENT *rptr, size_t nelem, size_t sizeelem)
{
	char *mem;
	int	size;

	size = nelem*sizeelem;
	mem = mon_malloc(size);
	if (mem) {
		memset(mem,0,size);
		dealloc_add(mem);
	}
	return(mem);
}

void
_free_r(REENT *rptr, void *ptr)
{
	dealloc_del(ptr);
	return(mon_free(ptr));
}

void *
_realloc_r(REENT *rptr, void *ptr, size_t size)
{
	void *mem;

	mem = mon_realloc(ptr,size);
	dealloc_add(mem);
	return(mem);
}

void *
#ifdef USE_PTRDIFF
_sbrk_r(REENT *rptr, ptrdiff_t size)
#else
_sbrk_r(REENT *rptr, size_t size)
#endif
{
#if NEWLIB_TRACE
	if (mon_getenv("NEWLIB_TRACE"))
		printf("_sbrk_r(%ld)\n",size);
#endif

	newlib_not_supported(rptr,"_sbrk_r");
	return(0);
}

/*******************************************************************
 *******************************************************************
 *
 * Miscellaneous stuff:
 */
#define MAX_EXIT_FUNCS	16

static int		exitftot;
static void		(*exitftbl[MAX_EXIT_FUNCS])();

void
_exit(int val)
{
	int	idx;

#if NEWLIB_TRACE
	if (mon_getenv("NEWLIB_TRACE"))
		printf("_exit(%d)\n",val);
#endif

	newlib_tfdlock();
	/* Ignore stdin/stdout/stderr slots: */
	for(idx=3;idx<MAXTFDS;idx++) {
		if (tfdtable[idx].inuse) {
			mon_printf("WARNING: exit with '%s' left opened\n",
				tfdtable[idx].name);
		}
	}
	newlib_tfdunlock();

	/* Call each of the functions loaded by atexit()...
	 */
	for(idx=0;idx<exitftot;idx++)
		exitftbl[idx]();

	/* Release any memory that was allocated, but not freed by
	 * the application...
	 */
	deallocate();

	mon_appexit(val);
}

int
atexit(void (*func)(void))
{
#if NEWLIB_TRACE
	if (mon_getenv("NEWLIB_TRACE"))
		printf("atexit(%lx)\n",(long)func);
#endif

	if (exitftot >= MAX_EXIT_FUNCS) {
		mon_printf("atexit() overflow\n");
		return(-1);
	}

	exitftbl[exitftot++] = func;
	return(0);
}


/*
 * Allocate a file buffer, or switch to unbuffered I/O.
 * Per the ANSI C standard, ALL tty devices default to line buffered.
 *
 * As a side effect, we set __SOPT or __SNPT (en/dis-able fseek
 * optimisation) right after the fstat() that finds the buffer size.
 */
void
__smakebuf(FILE *fp)
{
	void *p;
	int flags = __SNPT;
	size_t size = 256;

	if (fp->_flags & __SNBF) {
		fp->_bf._base = fp->_p = fp->_nbuf;
		fp->_bf._size = 1;
		return;
	}
	if ((p = malloc(size)) == NULL) {
		fp->_flags |= __SNBF;
		fp->_bf._base = fp->_p = fp->_nbuf;
		fp->_bf._size = 1;
		return;
	}
	flags |= __SMBF;
	fp->_bf._base = fp->_p = p;
	fp->_bf._size = size;

	/* Not sure how to tell if the FP is a tty, so we just assume
	 * it is here (not sure if this is ok).
	 */
// 	if (isatty(fp->_file))
 		flags |= __SLBF;

	fp->_flags |= flags;
}

/*
 * system():
 * Not really a newlib hook, but this is a convenient place for it...
 */
int
system(const char *cmd)
{
#if NEWLIB_TRACE
	if (mon_getenv("NEWLIB_TRACE"))
		printf("system(%s)\n",cmd);
#endif

	mon_docommand((char *)cmd,0);
	return(0);
};
