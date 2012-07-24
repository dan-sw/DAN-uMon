#include "config.h"
#if INCLUDE_BMEM
#include "genlib.h"
#include "cli.h"
#include "bmem.h"

char *BmemHelp[] = {
	"Block memory interface",
	"-[d:v] [cmd] [args]",
#if INCLUDE_VERBOSEHELP
	"Options:",
	" -d{devname}  specify the bmem devname",
	" -v           additive verbosity",
	"",
	"Commands:",
	" errlst",
	" erase {blkno_rng}",
	" read  {blkno} {pgno} {dst_addr} [size]",
	" write {blkno} {pgno} {src_addr} [size]",
#endif
	0,
};

int
bmemNameToId(char *name)
{
	struct bmem *bptr = bmemtbl;

	while(bptr->name) {
		if (strcmp(bptr->name,name) == 0)
			return(bptr->id);
		bptr++;
	}
	return(-1);
}

int
BmemCmd(int argc,char *argv[])
{
	char 	*addr;
	struct	bmem *bptr;
	int		opt, rc, devid, size, bkno, pgno, verbose;

	verbose = devid = 0;
	while((opt=getopt(argc,argv,"d:v")) != -1) {
		switch(opt) {
		case 'd':
			if ((devid = bmemNameToId(optarg)) < 0)
				return(CMD_PARAM_ERROR);
			break;
		case 'v':
			verbose++;
			break;
		default:
			return(CMD_PARAM_ERROR);
		}
	}

	if (argc == optind) {
		bptr = bmemtbl;
		printf(" ID  Name             PgSiz  SpSiz  PgPerBlk BlkTot\n");
		while(bptr->name) {
			printf(" %02d: %-16s  %4d   %4d      %4d   %4d\n",
				bptr->id, bptr->name, bptr->pgsize,
				bptr->spsize, bptr->ppb, bptr->btot);
			bptr++;
		}
		return(CMD_SUCCESS);
	}

	bptr = &bmemtbl[devid];

	if (strcmp(argv[optind],"read") == 0) {
		if (bptr->bmemread == 0) {
			printf("Err: read not implemented for %s\n",bptr->name);
			return(CMD_SUCCESS);
		}

		if (argc < optind+4)
			return(CMD_PARAM_ERROR);

		bkno = strtol(argv[optind+1],0,0);
		pgno = strtol(argv[optind+2],0,0);
		addr = (char *)strtoul(argv[optind+3],0,0);
		if (argc == optind+5)
			size = strtol(argv[optind+4],0,0);
		else
			size = bptr->pgsize;

		rc = bptr->bmemread(addr,bkno,pgno,size,verbose);
		if (rc != size)
			printf("Read err: %d\n",rc);

		return(CMD_SUCCESS);
	}
	else if (strcmp(argv[optind],"write") == 0) {
		if (bptr->bmemwrite == 0) {
			printf("Err: write not implemented for %s\n",bptr->name);
			return(CMD_SUCCESS);
		}

		if (argc < optind+4)
			return(CMD_PARAM_ERROR);

		if (argc == optind+5) {
			size = strtol(argv[optind+4],0,0);
			if (size > bptr->pgsize)
				size = bptr->pgsize;
		}
		else
			size = bptr->pgsize;

		bkno = strtol(argv[optind+1],0,0);
		pgno = strtol(argv[optind+2],0,0);
		addr = (char *)strtoul(argv[optind+3],0,0);
		if (argc == optind+5) {
			size = strtol(argv[optind+4],0,0);
			if (size > bptr->pgsize)
				size = bptr->pgsize;
		}
		else
			size = bptr->pgsize;

		rc = bptr->bmemwrite(addr,bkno,pgno,size,verbose);
		if (rc != size)
			printf("Write err: %d\n",rc);

		return(CMD_SUCCESS);
	}
	else if (strcmp(argv[optind],"erase") == 0) {
		if (bptr->bmemerase == 0) {
			printf("Err: erase not implemented for %s\n",bptr->name);
			return(CMD_SUCCESS);
		}

		if (argc != optind+2)
			return(CMD_PARAM_ERROR);

		return(CMD_SUCCESS);
	}
	else
		return(CMD_PARAM_ERROR);

	return(CMD_SUCCESS);
}
#endif
