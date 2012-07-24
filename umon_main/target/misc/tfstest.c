/* tfstest.c:
 *
 *	This file is included with the common monitor code, but it is not
 *	actually part of the monitor.  It is included here because it is
 *	an integral part of the monitor's stability... TFS test suite.
 *	It can be built on any platform the monitor supports by making this
 *	file a simple application that runs out of TFS.
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
 *
 */

#include "monlib.h"
#include "tfs.h"
#include "tfsprivate.h"
#include "stddefs.h"
#include "genlib.h"

#define TMPFILE "newfile"
#define TRUNCATE_SIZE	5


#define die()			errexit(__LINE__);
#define tfsdie(errno)	errtfs(__LINE__,errno);

int	verbose;
char	buffer1[512];
char	buffer2[512];

/* data1 & new_data1:
 * These arrays are used to create files and test file operations.
 * Do NOT modify these arrays, their content is used to verify files
 * that are created and modified.
 */

char *data1 =
"abcdefghij\nBBBBBBBBBB\nCCCCCCCCCC\n1234567890\nEEEEEEEEEE\n";

char *new_data1 =
"abceefghij\nBBBBBBBBBB\nCCCCCCCCCC\n1234567890\nEEEEEEEEEE\n";

/* errexit() and errtfs():
 *	Functions used (via macros above) to report error conditions detected
 *	by the tests below.
 */
void
errexit(int line)
{
	mon_printf("ERROR line %d\n",line);
	mon_appexit(-1);
}

void
errtfs(int line, int tfserrno)
{
	mon_printf("TFSERROR_%d (line %d): %s\n",(int)tfserrno,
		line,(char *)mon_tfsctrl(TFS_ERRMSG,tfserrno,0));
	mon_appexit(-1);
}

/* 
 *	getlinetest():
 *	The incoming filename is assumed to have been previously created with
 *	the data pointed to by the incoming data pointer.  
 *	Lines are read from the file via tfsgetline() and the buffer is filled
 *	with what should be the same data as is pulled in by tfsgetline().
 */
void
getlinetest(char *file, char *data)
{
	int	llen, tfd, err;

	tfd = mon_tfsopen(file,TFS_RDONLY,0);
	if (tfd < 0)
		tfsdie(tfd);

	/* Retrieve a line from the file: */
	err = mon_tfsgetline(tfd,buffer1,sizeof(buffer1));
	if (err < 0)
		tfsdie(err);

	/* Retrieve the same line from the data: */
	for(llen=0;*data;llen++,data++) {
		buffer2[llen] = *data;
		if (*data == '\n')
			break;
	}
	buffer2[llen+1] = 0;	

	/* They should match... */
	if (memcmp(buffer1,buffer2,llen))
		die();

	/* Again... */

	/* Retrieve a line from the file: */
	err = mon_tfsgetline(tfd,buffer1,sizeof(buffer1));
	if (err < 0)
		tfsdie(err);

	/* Retrieve the same line from the data: */
	for(llen=0;*data;llen++,data++) {
		buffer2[llen] = *data;
		if (*data == '\n')
			break;
	}
	buffer2[llen+1] = 0;	

	/* They should match... */
	if (memcmp(buffer1,buffer2,llen))
		die();


	/* One more line... */

	/* Retrieve a line from the file: */
	err = mon_tfsgetline(tfd,buffer1,sizeof(buffer1));
	if (err < 0)
		tfsdie(err);

	/* Retrieve the same line from the data: */
	for(llen=0;*data;llen++,data++) {
		buffer2[llen] = *data;
		if (*data == '\n')
			break;
	}
	buffer2[llen+1] = 0;	

	/* They should match... */
	if (memcmp(buffer1,buffer2,llen))
		die();

	mon_tfsclose(tfd,0);
}

/*
 *	cmp():
 *	File Compare...
 *	Use the standard TFS facilities to compare two different incoming files.
 *	Return 0 if files match, else -1.
 */
int
cmp(char *f1, char *f2)
{
	TFILE	finfo1, *tfp1;
	TFILE	finfo2, *tfp2;
	int		fd1, fd2, size, ret;
	char	*buf1, *buf2;

	/* Check sizes first: */
	tfp1 = &finfo1;
	if (mon_tfsfstat(f1,tfp1) == -1)
		die();
	tfp2 = &finfo2;
	if (mon_tfsfstat(f2,tfp2) == -1)
		die();
	if (tfp1->filsize != tfp2->filsize)
		return(-1);

	/* Copy f1 to buffer: */
	buf1 = mon_malloc(TFS_SIZE(tfp1));
	if (!buf1)
		die();
	fd1 = mon_tfsopen(f1,TFS_RDONLY,0);
	if (fd1 < 0)
		tfsdie(fd1);
	size = mon_tfsread(fd1,buf1,TFS_SIZE(tfp1));
	if (size != TFS_SIZE(tfp1))
		tfsdie(size);
	ret = mon_tfsclose(fd1,0);
	if (ret != TFS_OKAY)
		tfsdie(ret);

	/* Copy f2 to buffer: */
	buf2 = mon_malloc(TFS_SIZE(tfp2));
	if (!buf2)
		die();
	fd2 = mon_tfsopen(f2,TFS_RDONLY,0);
	if (fd2 < 0)
		tfsdie(fd2);
	size = mon_tfsread(fd2,buf2,TFS_SIZE(tfp2));
	if (size != TFS_SIZE(tfp2))
		tfsdie(size);
	ret = mon_tfsclose(fd2,0);
	if (ret != TFS_OKAY)
		tfsdie(ret);

	/* Compare the buffers: */
	if (memcmp(buf1,buf2,TFS_SIZE(tfp2)))
		ret = -1;
	else
		ret = 0;

	mon_free(buf1);
	mon_free(buf2);
	return(ret);
}

/*
 *	cp():
 *	File Copy...
 *	Use standard TFS facilities to copy one file to another.
 *	If successful, return the size of the copy; else die.
 */
int
cp(char *to, char *from)
{
	TFILE	finfo, *tfp;
	int		ffd, tfd, size, ret;
	char	*buffer;

	/* Open the source file: */
	ffd = mon_tfsopen(from,TFS_RDONLY,0);
	if (ffd < 0)
		tfsdie(ffd);

	/* Retrieve stats of the source file: */
	tfp = &finfo;
	if (mon_tfsfstat(from,tfp) == -1)
		die();

	/* The buffer used to open the destination file must be as large as
	 * the source file ...
	 */
	buffer = mon_malloc(TFS_SIZE(tfp));
	if (!buffer)
		die();

	/* Open the destination file for creation with the same flags
	 * as the source file:
	 */
	tfd = mon_tfsopen(to,TFS_CREATE | finfo.flags,buffer);
	if (tfd < 0)
		tfsdie(tfd);

	/* Read the entire source file into buffer, then write the entire
	 * buffer to the destination file...
	 */
	size = mon_tfsread(ffd,buffer,TFS_SIZE(tfp));
	if (size < 0)  
		tfsdie(size);
	ret = mon_tfswrite(tfd,buffer,TFS_SIZE(tfp));
	if (ret != TFS_OKAY)
		tfsdie(ret);

	mon_tfsclose(ffd,0);
	mon_tfsclose(tfd,finfo.info);
	mon_free(buffer);
	return(TFS_SIZE(tfp));
}

/*
 *	inusetest():
 *	Open a file, then try to remove it or add a file with the same name.
 *	This should fail...
 */
int
inusetest(char *fname)
{
	int	tfd, err;

	/* Open a file, then try to run tfsadd() on it or try to remove it... */
	tfd = mon_tfsopen(fname,TFS_RDONLY,0);
	if (tfd < 0)
		tfsdie(tfd);

	err = mon_tfsunlink(fname);
	if (err != TFSERR_FILEINUSE)
		tfsdie(err);
	err = mon_tfsadd(fname,0,0,buffer1,10);
	if (err != TFSERR_FILEINUSE)
		tfsdie(err);

	mon_tfsclose(tfd,0);
	return(0);
}

/* seektest():
 *	Run some tests on tfsseek().
 *	Verify that the character in the file specified at the incoming offset
 *	is as expected.  Seek to beyond the end of file and verify error, etc...
 */
int
seektest(char *fname, int offset, char value)
{
	char	c, buf1[16], buf2[16];
	TFILE	*tfp;
	int		tfd, end, err;

	/* Open the source file: */
	tfd = mon_tfsopen(fname,TFS_RDONLY,0);
	if (tfd < 0)
		tfsdie(tfd);

	if (mon_tfsseek(tfd,offset,TFS_BEGIN) != offset)
		tfsdie(tfd);
	
	if (mon_tfsread(tfd,&c,1) != 1)
		tfsdie(tfd);

	if (c != value)
		die();

	tfp = mon_tfsstat(fname);
	if (!tfp)
		die();
	end = TFS_SIZE(tfp);

	/* 
	 * Seek to various limits in the file and verify proper return
	 * value...
	 */
	err = mon_tfsseek(tfd,0,TFS_BEGIN);
	if (err != 0)
		tfsdie(err);
	err = mon_tfsseek(tfd,end,TFS_CURRENT);
	if (err != end)
		tfsdie(err);
	err = mon_tfsseek(tfd,1,TFS_CURRENT);
	if (err != TFSERR_EOF)
		tfsdie(err);
	err = mon_tfsseek(tfd,end,TFS_BEGIN);
	if (err != end)
		tfsdie(err);
	err = mon_tfsseek(tfd,end+1,TFS_BEGIN);
	if (err != TFSERR_EOF)
		tfsdie(err);

	err = mon_tfsseek(tfd,0,TFS_BEGIN);
	if (err != 0)
		tfsdie(err);
	err = mon_tfsseek(tfd,-1,TFS_CURRENT);
	if (err != TFSERR_EOF)
		tfsdie(err);

	err = mon_tfsseek(tfd,end,TFS_BEGIN);
	if (err != end)
		tfsdie(err);
	err = mon_tfsseek(tfd,-1,TFS_CURRENT);
	if (err != end-1)
		tfsdie(err);
	err = mon_tfsseek(tfd,2,TFS_CURRENT);
	if (err != TFSERR_EOF)
		tfsdie(err);

	/* Seek to beginning, read 10, seek to beginning again read 10 again.
	 * Verify that both reads have same data.
	 */
	err = mon_tfsseek(tfd,0,TFS_BEGIN);
	if (err != 0)
		tfsdie(err);
	err = mon_tfsread(tfd,buf1,10);
	if (err != 10)
		tfsdie(err);
	err = mon_tfsseek(tfd,0,TFS_BEGIN);
	if (err != 0)
		tfsdie(err);
	err = mon_tfsread(tfd,buf2,10);
	if (err != 10)
		tfsdie(err);
	if (memcmp(buf1,buf2,10))
		die();

	/*
	 * Seek to end, then verify that read() returns EOF and tfs_eof()
	 * returns true.
	 */
	err = mon_tfsseek(tfd,end,TFS_BEGIN);
	if (err != end)
		tfsdie(err);
	err = mon_tfsread(tfd,buf1,1);
	if (err != TFSERR_EOF)
		tfsdie(err);
	if (mon_tfseof(tfd) != 1)
		die();

	mon_tfsclose(tfd,0);
	return(0);
}

/* appendtest():
 *	Create a file, then append to it and verify content after
 *	closing the file.
 */
int
appendtest(char *fname, char *data1, char *data2)
{
	TFILE	finfo, *tfp;
	int	err, tfd, len;

	/* Create the first version of the file:
	 */
	err = mon_tfsadd(fname,0,0,data1,strlen(data1));
	if (err != TFS_OKAY)
		tfsdie(err);

	/* Append to that first file:
	 */
	tfd = mon_tfsopen(fname,TFS_APPEND,buffer1);
	if (tfd < 0)
		tfsdie(tfd);

	err = mon_tfswrite(tfd,data2,strlen(data2));
	if (err != TFS_OKAY)
		tfsdie(err);
	
	err = mon_tfsclose(tfd,0);
	if (err != TFS_OKAY)
		tfsdie(err);

	/* Verify that the file was properly appended to:
	 */
	tfp = &finfo;
	if (mon_tfsfstat(fname,tfp) == -1)
		die();
	len = strlen(data1) + strlen(data2);
	if (len != TFS_SIZE(tfp))
		die();

	tfd = mon_tfsopen(fname,TFS_RDONLY,0);
	if (tfd < 0)
		tfsdie(tfd);

	err = mon_tfsread(tfd,buffer1,len);
	if (err != len)
		tfsdie(err);
	
	strcpy(buffer2,data1);
	strcat(buffer2,data2);
	if (memcmp(buffer1,buffer2,len) != 0)
		die();
	
	mon_tfsclose(tfd,0);
	return(0);
}

/* writetest():
 *  Open the specified file in APPEND (modify) mode.  Seek into the file
 *	and read 1 byte.  Increment that byte by one and then write it back
 *	to the same location from which it was read.  Then, close the file.
 *	Build a new file that is what "should" be the content of the file we
 *	just modified and compare the two files.  They better match.
 *  This function also verifies TFS_HEADROOM.
 */
int
writetest(char *fname, char *newdata)
{
	TFILE *tfp;
	char c;
	long	headroom;
	int	size, tfd, err;

	/* Open the source file: */
	tfp = mon_tfsstat(fname);
	if (!tfp)
		die();
	size = TFS_SIZE(tfp);
	tfd = mon_tfsopen(fname,TFS_APPEND,buffer1);
	if (tfd < 0)
		tfsdie(tfd);

	headroom = mon_tfsctrl(TFS_HEADROOM,tfd,0);
	if (headroom != 0)
		die();

	err = mon_tfsseek(tfd,3,TFS_BEGIN);
	if (err != 3)
		tfsdie(err);

	headroom = mon_tfsctrl(TFS_HEADROOM,tfd,0);
	if (headroom != (size-3))
		die();

	err = mon_tfsread(tfd,&c,1);
	if (err != 1)
		tfsdie(err);

	headroom = mon_tfsctrl(TFS_HEADROOM,tfd,0);
	if (headroom != (size-4))
		die();

	c++;
	err = mon_tfsseek(tfd,-1,TFS_CURRENT);
	if (err != 3)
		tfsdie(err);

	headroom = mon_tfsctrl(TFS_HEADROOM,tfd,0);
	if (headroom != (size-3))
		die();

	err = mon_tfswrite(tfd,&c,1);
	if (err != TFS_OKAY)
		tfsdie(err);

	headroom = mon_tfsctrl(TFS_HEADROOM,tfd,0);
	if (headroom != (size-4))
		die();

	mon_tfsclose(tfd,0);

	/* Add a new file that "should" be identical to the modified file;
	 * then compare them and delete the newfile...
	 */
	err = mon_tfsadd(TMPFILE,"newdata1","2",newdata,strlen(newdata));
	if (err != TFS_OKAY)
		tfsdie(err);
	if (cmp(fname,TMPFILE) != 0)
		die();
	err = mon_tfsunlink(TMPFILE);
	if (err != TFS_OKAY)
		tfsdie(err);
	
	return(0);
}

/*
 *	ctrltest():
 *	Test various aspects of the tfsctrl() function.
 */
void
ctrltest(char *fname, char *data)
{
	int		tfd, ret;
	char	flags[16];

	/* Copy fname to TMPFILE... */
	cp (TMPFILE,fname);

	/* Verify TFS_UNOPEN...
	 *	Open a file, modify it, then prior to calling tfsclose(), call
	 *	tfsctrl(TFS_UNOPEN) on that file descriptor and make sure the
	 *	file is in its original state and the file descriptor has been
	 *	closed...
	 */
	tfd = mon_tfsopen(fname,TFS_APPEND,buffer1);
	if (tfd < 0)
		tfsdie(tfd);
	ret = mon_tfswrite(tfd,"abcdefg",7);
	if (ret != TFS_OKAY)
		tfsdie(ret);
	ret = mon_tfsctrl(TFS_UNOPEN,tfd,0);
	if (ret != TFS_OKAY)
		tfsdie(ret);
	ret = mon_tfsclose(tfd,0);
	if (ret != TFSERR_BADFD)
		tfsdie(ret);
	if (cmp(TMPFILE,fname))
		die();

	/* Verify TFS_TELL...
	 *	Open a file, seek to a known point, then make sure that
	 *	tfsctrl(TFS_TELL) returns the expected offset. 
	 */
	tfd = mon_tfsopen(fname,TFS_RDONLY,0);
	if (tfd < 0)
		tfsdie(tfd);
	ret = mon_tfsseek(tfd,5,TFS_BEGIN);
	if (ret != 5)
		tfsdie(ret);
	ret = mon_tfsctrl(TFS_TELL,tfd,0);
	if (ret != 5)
		tfsdie(ret);
	ret = mon_tfsread(tfd,buffer1,3);
	if (ret != 3)
		tfsdie(ret);
	ret = mon_tfsctrl(TFS_TELL,tfd,0);
	if (ret != 5+3)
		tfsdie(ret);

	ret = mon_tfsclose(tfd,0);
	if (ret != TFS_OKAY)
		tfsdie(ret);
	

	/* Test all "flag-ascii-to-binary" conversions... */
	if ((ret = mon_tfsctrl(TFS_FATOB,(long)"e",0)) !=  TFS_EXEC)
		tfsdie(ret);
	if ((ret = mon_tfsctrl(TFS_FATOB,(long)"b",0)) !=  TFS_BRUN)
		tfsdie(ret);
	if ((ret = mon_tfsctrl(TFS_FATOB,(long)"B",0)) !=  TFS_QRYBRUN)
		tfsdie(ret);
#if 0
	if ((ret = mon_tfsctrl(TFS_FATOB,(long)"C",0)) !=  TFS_COFF)
		tfsdie(ret);
	if ((ret = mon_tfsctrl(TFS_FATOB,(long)"E",0)) !=  TFS_ELF)
		tfsdie(ret);
	if ((ret = mon_tfsctrl(TFS_FATOB,(long)"A",0)) !=  TFS_AOUT)
		tfsdie(ret);
#else
	if ((ret = mon_tfsctrl(TFS_FATOB,(long)"E",0)) !=  TFS_EBIN)
		tfsdie(ret);
#endif
	if ((ret = mon_tfsctrl(TFS_FATOB,(long)"c",0)) !=  TFS_CPRS)
		tfsdie(ret);
	if ((ret = mon_tfsctrl(TFS_FATOB,(long)"i",0)) !=  TFS_IPMOD)
		tfsdie(ret);
	if ((ret = mon_tfsctrl(TFS_FATOB,(long)"u",0)) !=  TFS_UNREAD)
		tfsdie(ret);
	if ((ret = mon_tfsctrl(TFS_FATOB,(long)"1",0)) !=  TFS_ULVL1)
		tfsdie(ret);
	if ((ret = mon_tfsctrl(TFS_FATOB,(long)"2",0)) !=  TFS_ULVL2)
		tfsdie(ret);
	if ((ret = mon_tfsctrl(TFS_FATOB,(long)"3",0)) !=  TFS_ULVL3)
		tfsdie(ret);

	/* Test all "flag-binary-to-ascii" conversions... */
	ret = mon_tfsctrl(TFS_FBTOA,TFS_EXEC,(long)flags);
	if ((ret == TFSERR_BADARG) || (strcmp(flags,"e")))
		tfsdie(ret);
	ret = mon_tfsctrl(TFS_FBTOA,TFS_BRUN,(long)flags);
	if ((ret == TFSERR_BADARG) || (strcmp(flags,"b")))
		tfsdie(ret);
	ret = mon_tfsctrl(TFS_FBTOA,TFS_QRYBRUN,(long)flags);
	if ((ret == TFSERR_BADARG) || (strcmp(flags,"B")))
		tfsdie(ret);
#if 0
	ret = mon_tfsctrl(TFS_FBTOA,TFS_COFF,(long)flags);
	if ((ret == TFSERR_BADARG) || (strcmp(flags,"C")))
		tfsdie(ret);
	ret = mon_tfsctrl(TFS_FBTOA,TFS_ELF,(long)flags);
	if ((ret == TFSERR_BADARG) || (strcmp(flags,"E")))
		tfsdie(ret);
	ret = mon_tfsctrl(TFS_FBTOA,TFS_AOUT,(long)flags);
	if ((ret == TFSERR_BADARG) || (strcmp(flags,"A")))
		tfsdie(ret);
#else
	ret = mon_tfsctrl(TFS_FBTOA,TFS_EBIN,(long)flags);
	if ((ret == TFSERR_BADARG) || (strcmp(flags,"E")))
		tfsdie(ret);
#endif
	ret = mon_tfsctrl(TFS_FBTOA,TFS_CPRS,(long)flags);
	if ((ret == TFSERR_BADARG) || (strcmp(flags,"c")))
		tfsdie(ret);
	ret = mon_tfsctrl(TFS_FBTOA,TFS_IPMOD,(long)flags);
	if ((ret == TFSERR_BADARG) || (strcmp(flags,"i")))
		tfsdie(ret);
	ret = mon_tfsctrl(TFS_FBTOA,TFS_UNREAD,(long)flags);
	if ((ret == TFSERR_BADARG) || (strcmp(flags,"u")))
		tfsdie(ret);
	ret = mon_tfsctrl(TFS_FBTOA,TFS_ULVL1,(long)flags);
	if ((ret == TFSERR_BADARG) || (strcmp(flags,"1")))
		tfsdie(ret);
	ret = mon_tfsctrl(TFS_FBTOA,TFS_ULVL2,(long)flags);
	if ((ret == TFSERR_BADARG) || (strcmp(flags,"2")))
		tfsdie(ret);
	ret = mon_tfsctrl(TFS_FBTOA,TFS_ULVL3,(long)flags);
	if ((ret == TFSERR_BADARG) || (strcmp(flags,"3")))
		tfsdie(ret);


	ret = mon_tfsunlink(TMPFILE);
	if (ret != TFS_OKAY)
		tfsdie(ret);
}

/*
 * trunctest():
 *	Test file truncation.  Verify that file truncates to expected size
 *	and that the data in the file is as expected.
 */
int
trunctest(char *fname, char *data)
{
	int	tfd, err, size;
	TFILE	*tfp;

	/* Copy incoming file name to a tempoary file... */
	cp(TMPFILE,fname);

	/* Open the temporary file and truncate it.
	 * First verify that a truncation size too big will fail, then
	 * do a real truncation.  Close the file then verify that the
	 * file has been trunated.
	 */
	tfd = mon_tfsopen(TMPFILE,TFS_APPEND,buffer1);
	if (tfd < 0)
		tfsdie(tfd);
	err = mon_tfstruncate(tfd,9999999);
	if (err != TFSERR_BADARG)
		tfsdie(err);
	err = mon_tfstruncate(tfd,TRUNCATE_SIZE);
	if (err != TFS_OKAY)
		tfsdie(err);
	err = mon_tfsclose(tfd,0);
	if (err < 0)
		tfsdie(err);

	/* Make sure that the file was truncated to the proper size. */
	tfp = mon_tfsstat(TMPFILE);
	if (!tfp)
		die();
	if (TFS_SIZE(tfp) != TRUNCATE_SIZE)
		die();
	
	/* Now reopen the file and verify that the data is correct... */
	tfd = mon_tfsopen(TMPFILE,TFS_RDONLY,0);
	if (tfd < 0)
		tfsdie(tfd);
	
	size = mon_tfsread(tfd,buffer1,TFS_SIZE(tfp));
	if (size != TFS_SIZE(tfp))
		tfsdie(size);

	if (memcmp(buffer1,data,TRUNCATE_SIZE))
		die();

	/* Close and remove the temporary file. */
	mon_tfsclose(tfd,0);
	err = mon_tfsunlink(TMPFILE);
	if (err != TFS_OKAY)
		tfsdie(err);
	return(0);
}

/* ls():
 *	Just list current set of files in TFS...
 */
void
ls(void)
{
	TFILE	*tfp;

	tfp = (TFILE *)0;
	while((tfp = mon_tfsnext(tfp)))
		mon_printf("%s\n",TFS_NAME(tfp));

	mon_printf("There are currently %d files in TFS\n",
		(int)mon_tfsctrl(TFS_FCOUNT,0,0));
}


char *usage_text[] = {
	"Usage: tfstest [options] {file1} {file2}",
	" Options...",
	"  -l   run 'ls' at the end",
	"  -r   don't remove files when done",
	"  -v   verbosity level (additive)",
	" Note: neither file1 nor file2 can be the string 'newfile'",
	(char *)0,
};

void
usage(char *msg)
{
	char **use;

	if (msg)
		mon_printf("%s\n",msg);
	use = usage_text;

	while(*use) {
		mon_printf("%s\n",*use);
		use++;
	}
	mon_appexit(-1);
}

int
main(int argc,char *argv[])
{
	int	err, opt, removefiles, list;
	char *file1, *file2;

	verbose = 0;
	list = 0;
	removefiles = 1;
	getoptinit();
	while((opt=getopt(argc,argv,"lrv")) != -1) {
		switch(opt) {
		case 'l':
			list = 1;
			break;
		case 'r':
			removefiles = 0;
			break;
		case 'v':
			verbose++;
			break;
		default:
			usage(0);
		}
	}

	if (argc != optind+2)
		usage("Bad arg count");

	/* Test all aspects of TFS API calls: */
	file1 = argv[optind];
	file2 = argv[optind+1];
	if ((!strcmp(file1,TMPFILE)) || (!strcmp(file2,TMPFILE)))
		usage(TMPFILE);


	if (verbose)
		mon_printf("tfstest %s %s...\n",file1,file2);
	

	/* 
	 *	Start by removing files to be created later...
	 */
	if (mon_tfsstat(TMPFILE)) {
		if (verbose)
			mon_printf("Removing %s...\n",TMPFILE);
		err = mon_tfsunlink(TMPFILE);
		if (err != TFS_OKAY)
			tfsdie(err);
	}
	if (mon_tfsstat(file1)) {
		if (verbose)
			mon_printf("Removing %s...\n",file1);
		err = mon_tfsunlink(file1);
		if (err != TFS_OKAY)
			tfsdie(err);
	}
	if (mon_tfsstat(file2)) {
		if (verbose)
			mon_printf("Removing %s...\n",file2);
		err = mon_tfsunlink(file2);
		if (err != TFS_OKAY)
			tfsdie(err);
	}

	/*
	 *	Create a file...
	 */
	if (verbose)
		mon_printf("Creating %s...\n",file1);
	err = mon_tfsadd(file1,"data1","2",data1,strlen(data1));
	if (err != TFS_OKAY)
		tfsdie(err);

	/*
	 *	Basic getline test...
	 */
	if (verbose)
		mon_printf("Checking 'getline'...\n");
	getlinetest(file1,data1);

	/*
	 *	Now copy the file...
	 */
	if (verbose)
		mon_printf("Copying %s to %s...\n",file1,file2);
	cp(file2,file1);

	/*
	 *	Now compare the two...
	 *	(they should be identical)
	 */
	if (verbose)
		mon_printf("Comparing %s to %s...\n",file1,file2);
	if (cmp(file1,file2) != 0)
		die();

	/*
	 *	Seek test...
	 *  Verify that data at a specified offset is as expected based on the
	 *	file (file1) initially created from the data1 array...
	 */
	if (verbose)
		mon_printf("Running seek test on %s...\n",file1);
	seektest(file1,38,data1[38]);

	/* 
	 *	Truncateion test...
	 *	Truncate a file and verify.
	 */
	if (verbose)
		mon_printf("Running truncation test on %s...\n",file1);
	trunctest(file1,data1);

	/*
	 *	Tfsctrl() function test...
	 */
	if (verbose)
		mon_printf("Running tfsctrl test...\n");
	ctrltest(file1,data1);

	/* 
	 *	Write test...
	 *	Modify a file in a few different ways and verify the modification...
	 *	Note that after this point, file1 and data1 are not the same.
	 *	The content of file1 will be the same as the new_data1 array.
	 */
	if (verbose)
		mon_printf("Running write test on %s...\n",file1);
	writetest(file1,new_data1);
	
	/*
	 *	File in-use test...
	 *	Verify that if a file is in-use, it cannot be removed.
	 */
	if (verbose)
		mon_printf("Running in-use test on %s...\n",file1);
	inusetest(file1);

	/*
	 *	Append test...
	 *	Verify that a file can be properly appended to.
	 */
	if (verbose)
		mon_printf("Running append test on %s...\n",file1);
	appendtest(file1,"this_is_some_data","this_is_the_appended_data");

	/*
	 * If the -r option is not set, then remove the files...
	 */
	if (removefiles) {
		if (mon_tfsstat(file1)) {
			err = mon_tfsunlink(file1);
			if (err != TFS_OKAY)
				tfsdie(err);
		}
		if (mon_tfsstat(file2)) {
			err = mon_tfsunlink(file2);
			if (err != TFS_OKAY)
				tfsdie(err);
		}
		if (mon_tfsstat(TMPFILE)) {
			err = mon_tfsunlink(TMPFILE);
			if (err != TFS_OKAY)
				tfsdie(err);
		}
	}

	if (list)
		ls();

	/* All error cases checked above would have resulted in an exit
	 * of this application, so if we got here, the testing must
	 * have succeeded...
	 */
	mon_printf("TFS test on %s & %s PASSED\n",file1,file2);
	mon_appexit(0);
	return(0);
}
