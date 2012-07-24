/* mkupdate.c:
 * This tool is used to build a script that can be TFTP'ed to a target
 * and then run on the target to determine if any other files need to
 * be downloaded.
 *
 * The script is created based on a set of files passed to mkupdate.
 * For each file, mkupdate creates logic in a uMon script to test the
 * crc of a file, and if that crc doesn't match, the file is downloaded
 * via tftp to the target.
 */
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "version.h"
#include "utils.h"
#include "crc32.h"
#ifndef BUILD_WITH_VCC
#include <unistd.h>
#endif

#include "../ttftp/ttftp.h"

#define NOSRVR_ERROR		"NOSRVR_ERROR"
#define DLDFAILURE_ERROR	"DLDFAIL_ERROR"

#ifndef O_BINARY
#define O_BINARY 0
#endif

int debug, verbose;
char prefix[128];
char *target_ip;

struct tfsfile {
	char host_fname[128];
	char tfs_fname[128];
	char flags[32];
	char info[32];
	struct tfsfile *next;
};

struct tfsfile *flist, *fbase = (struct tfsfile *)0;

/* mkupdate():
 * Takes the incoming argument list assumed to be filenames and turns
 * that into a script that can be put on a target and run to test a
 * set of on-board files and update them via tftp if necessary.
 */
void
mkupdate(void)
{
	unsigned long crc;

	flist = fbase;
	printf("\n#\n#\n");
	printf("if $TFTPSRVR seq \\$TFTPSRVR goto %s\n",NOSRVR_ERROR);
	printf("set FTOT 0\n\n");

	printf("\n#\n##############################################\n");
	printf("# File tests:\n");
	printf("##############################################\n#\n\n");

	while(flist) {
		if (fcrc32(flist->host_fname,&crc,0) < 0)
			usage(0);

		printf("\n# Test %s:\n",flist->tfs_fname);
		printf("set FILE %s\n",flist->tfs_fname);
		printf("tfs base %s BASE\n",flist->tfs_fname);
		printf("if $BASE seq \\$BASE gosub LOAD_%s\n",flist->tfs_fname);
		printf("tfs size %s SIZE\n",flist->tfs_fname);
		printf("if $SIZE seq \\$SIZE goto %s\n",DLDFAILURE_ERROR);
		printf("mt -C $BASE $SIZE\n");
		printf("if $MTCRC ne 0x%08lx gosub LOAD_%s\n",crc,flist->tfs_fname);
		flist = flist->next;
	}

	flist = fbase;
	printf("\n\n");
	printf("echo $FTOT files updated.\n");
	printf("exit\n");

	printf("\n#\n##############################################\n");
	printf("# Subroutines:\n");
	printf("##############################################\n#\n\n");
	while(flist) {
		char fbuf[32], ibuf[32];

		ibuf[0] = fbuf[0] = 0;
		if (flist->flags[0])
			sprintf(fbuf," -f %s",flist->flags);
		if (flist->info[0])
			sprintf(ibuf," -i %s",flist->info);

		printf("\n#\n# LOAD_%s:\n",flist->tfs_fname);

		if (prefix[0])
			printf("tftp -F %s%s%s $TFTPSRVR get %s/%s\n",
				flist->tfs_fname,fbuf,ibuf,prefix,flist->host_fname);
		else
			printf("tftp -F %s%s%s $TFTPSRVR get %s\n",
				flist->tfs_fname,fbuf,ibuf,flist->host_fname);

		printf("tfs base %s BASE\n",flist->tfs_fname);
		printf("set -i FTOT\nreturn\n");
		flist = flist->next;
	}

	printf("\n#\n##############################################\n");
	printf("# Error handlers:\n");
	printf("##############################################\n#\n\n");

	printf("\n");
	printf("# %s:\n",NOSRVR_ERROR);
	printf("echo This script requires the TFTPSRVR shell var be set.\n");
	printf("exit\n");

	printf("\n");
	printf("# %s:\n",DLDFAILURE_ERROR);
	printf("echo Failed to download ${FILE}, script aborted.\n");
	printf("exit\n");

}

/* mkflist_fromcli():
 * Turn the incoming list of filenames into a linked list of tfsfile
 * structure entries...
 */
int
mkflist_fromcli(int argc, char *argv[])
{
	int i;
	char *flags, *info;

	if (argc < optind + 1)
		usage(0);

	for(i=optind;i<argc;i++) {
		if (!fbase) {
			fbase = (struct tfsfile *)Malloc(sizeof(struct tfsfile));
			fbase->next = 0;
			flist = fbase;
		}
		else {
			flist->next = (struct tfsfile *)Malloc(sizeof(struct tfsfile));
			flist = flist->next;
			flist->next = 0;
		}

		strcpy(flist->host_fname,argv[i]);
		strcpy(flist->tfs_fname,argv[i]);

		flags = info = 0;
		flags = strchr(flist->tfs_fname,',');
		if (flags)
			info = strchr(flags+1,',');

		if (flags) {
			*flags = 0;
			if (*(flags+1) != ',')  {
				flags++;
			}
			else
				flags = 0;
		}

		if (info) {
			*info = 0;
			if (*(info+1) != 0)
				info++;
			else
				info = 0;
		}
		
		if (flags) 
			strcpy(flist->flags,flags);
		else
			flist->flags[0] = 0;

		if (info) 
			strcpy(flist->info,info);
		else
			flist->info[0] = 0;
	}

	return(0);
}

int
mkflist_fromtftp(char *ip)
{
	int rc;
	FILE *fp;
	char *flags, *info;
	char tmpfile[32], line[64];

	strcpy(tmpfile,"flist_XXXXXX");
#ifdef BUILD_WITH_VCC
	mktemp(tmpfile);
#else
	mkstemp(tmpfile);
#endif

	if (verbose)
		fprintf(stderr,"Retrieving file list from %s...\n",ip);

	if (ttftp(ip,"get",".",tmpfile,"octet") != EXIT_SUCCESS) {
		fprintf(stderr,"TFTP failed\n");
		return(-1);
	}
	if ((fp = fopen(tmpfile,"r")) == (FILE *)NULL) {
		perror(tmpfile);
		return(-1);
	}

	while(fgets(line,sizeof(line),fp)) {
		line[strlen(line)-1] = 0;

		if (!fbase) {
			fbase = (struct tfsfile *)Malloc(sizeof(struct tfsfile));
			fbase->next = 0;
			flist = fbase;
		}
		else {
			flist->next = (struct tfsfile *)Malloc(sizeof(struct tfsfile));
			flist = flist->next;
			flist->next = 0;
		}

		strcpy(flist->host_fname,line);
		strcpy(flist->tfs_fname,line);

		flags = info = 0;
		flags = strchr(flist->tfs_fname,',');
		if (flags)
			info = strchr(flags+1,',');

		if (flags) {
			*flags = 0;
			if (*(flags+1) != ',')  {
				flags++;
			}
			else
				flags = 0;
		}

		if (info) {
			*info = 0;
			if (*(info+1) != 0)
				info++;
			else
				info = 0;
		}
		
		if (flags) 
			strcpy(flist->flags,flags);
		else
			flist->flags[0] = 0;

		if (info) 
			strcpy(flist->info,info);
		else
			flist->info[0] = 0;

		if (verbose) {
			fprintf(stderr,"Retrieving <%s> from %s...\n",
				flist->tfs_fname,ip);
		}
		rc = ttftp(ip,"get",flist->tfs_fname,flist->host_fname,"octet");
		if (rc != EXIT_SUCCESS) {
			fprintf(stderr,"TFTP failed\n");
			return(-1);
		}
	}

	fclose(fp);
	unlink(tmpfile);
	return(0);
}

void
testTftp(int i, char *c, int j)
{
}

int
main(int argc,char *argv[])
{
	int opt;

	prefix[0] = 0;
	target_ip = 0;
	debug = verbose = 0;
	while((opt=getopt(argc,argv,"dD:iI:t:vV")) != EOF) {
		switch(opt) {
			case 'D':
				sprintf(prefix,"%s",optarg);
				break;
			case 'i':
				strcpy(prefix,"${IPADD}");
				break;
			case 'I':
				sprintf(prefix,"%s/${IPADD}",optarg);
				break;
			case 't':
				target_ip = optarg;
				ttftp_init();
				tftpQuiet = 1;
				break;
			case 'V':
				showVersion();
				break;
			case 'v':
				verbose = 1;
				break;
			default:
				usage(0);
		}
	}

	if (target_ip)
		mkflist_fromtftp(target_ip);
	else
		mkflist_fromcli(argc,argv);

	if (!fbase) {
		fprintf(stderr,"Failed to generate file list\n");
		exit(1);
	}

	if (debug) {
		flist = fbase;
		while(flist) {
			printf("Name: <%s> flgs: <%s> info: <%s>\n",
				flist->tfs_fname,flist->flags,flist->info);
			flist = flist->next;
		}
	}

	mkupdate();

	exit(0);
}

char *usage_txt[] = {
	"Usage: mkupdate [options] {file1} [file2] [file3] ...",
	" Create an update script that can be run by uMon",
	" Options:",
	" -d            debug mode (development only)",
	" -D {prefix}   specify a first-level directory for tftp get",
	" -i            use target's IP as first-level directory for tftp get",
	" -I {prefix}   similar to -i, but include a second prefix",
	" -v            verbose",
	" -V            show version (build date) of tool",
	(char *)0,
};
