/*
 * spif.c:
 * This is the generic portion of the spi-flash interface command.
 * It assumes there is an underlying SPI-flash device interface 
 * that provides the expected API (see spif.h).
 * The "tfs-specific" portions of this code support the model of having
 * non-volatile TFS storage in SPI-flash that is transferred to RAM for
 * general use, but also provides the ability to do the following:

 * tfsload:
 *  Transfer a SPIF_TFS_SIZE area in SPI-flash from SPIF_TFS_BASE to
 *	FLASHRAM_BASE in RAM.
 * tfserase:
 *  Erase the area in SPI-flash that is allocated to SPIF_TFS.
 * tfsstore:
 *  Transfer the RAM-based TFS space back to SPI-flash.
 * tfsls:
 *  List the files out of SPI_TFS directly.
 * tfsrm:
 *  Mark a file in SPI_TFS as deleted.
 * tfsadd:
 *  Append a file just after the last file currently stored in SPIF_TFS.
 *
 * NOTE: this does NOT support powersafe mode as of this writing.
 *
 * The spifmap.h file is port specific; hence would be part of the
 * port directory.
 */
#include "config.h"
#include "genlib.h"
#include "cli.h"
#include "tfs.h"
#include "tfsprivate.h"
#include "spif.h"
#include "spifmap.h"

#define BASE_OF_SERIALTFS SPIF_TFS_BASE
#define END_OF_SERIALTFS (SPIF_TFS_BASE+SPIF_TFS_SIZE)

#ifndef SPIF_TFSRAM_BASE
#define SPIF_TFSRAM_BASE FLASHRAM_BASE
#endif

char *SpifHelp[] = {
	"Interface with SPI-Flash",
	"-[v] {cmd} [arg1] [arg2] [...]",
#if INCLUDE_VERBOSEHELP
	"Options:",
	" -v   incrementing verbosity level",
	"",
	"Cmds:",
	" wen                       write enable",
	" wfr                       wait-for-ready",
	" stat                      show flash status register",
	" gprot                     global protect",
	" cerase                    chip erase",
	" gunprot                   global unprotect",
#if INCLUDE_TFS
	" tfsls                     list files directly out of SPI flash",
	" tfsload                   copy SPIF to TFSRAM",
	" tfsstat                   show state of SPI flash",
	" tfsstore                  copy TFSRAM to SPIF",
	" tfserase                  erase SPIF space allocated to TFS",
	" tfsrm {name}              remove file directly out of SPI flash",
	" tfsadd {name} [src sz]    add file directly to SPI flash",
#endif
	" berase {addr bsize}       block erase (bsize=4K,32K,64K)",
	" read {addr dest len}      read block",
	" write {addr src len}      write block",
#endif
	0,
};

int
SpifCmd(int argc,char *argv[])
{
	unsigned long addr;
	char *cmd, *dest, *src;
	int opt, verbose, len, bsize, rc;


	verbose = 0;
	while((opt=getopt(argc,argv,"v")) != -1) {
		switch(opt) {
		case 'v':
			verbose++;
			break;
		default:
			return(CMD_PARAM_ERROR);
		}
	}
	
	if (argc < optind+1)
		return(CMD_PARAM_ERROR);

	cmd = argv[optind];

	if (verbose)
		printf("CMD: %s\n",cmd);

	// Establish the SPI configuration to match that of the SPI flash...
	spifQinit();

	if (strcmp(cmd,"stat") == 0) {
		spifId(1);	
		spifReadStatus(verbose+1);	
	}
	else if (strcmp(cmd,"gprot") == 0) {
		spifGlobalProtect();	
	}
	else if (strcmp(cmd,"berase") == 0) {

		if (argc != optind+3)
			return(CMD_PARAM_ERROR);

		addr = strtoul(argv[optind+1],0,0);

		if (strcmp(argv[optind+2],"4K") == 0)
			bsize = 0x1000;
		else if (strcmp(argv[optind+2],"32K") == 0)
			bsize = 0x8000;
		else if (strcmp(argv[optind+2],"64K") == 0)
			bsize = 0x10000;
		else
			return(CMD_PARAM_ERROR);

		spifBlockErase(bsize,addr);	
	}
	else if (strcmp(cmd,"cerase") == 0) {
		spifChipErase();	
	}
	else if (strcmp(cmd,"wfr") == 0) {
		spifWaitForReady();	
	}
	else if (strcmp(cmd,"gunprot") == 0) {
		spifGlobalUnprotect();	
	}
	else if (strcmp(cmd,"write") == 0) {
		if (argc != optind+4)
			return(CMD_PARAM_ERROR);
		addr = strtoul(argv[optind+1],0,0);
		src = (char *)strtoul(argv[optind+2],0,0);
		len = (int)strtol(argv[optind+3],0,0);
		if ((rc = spifWriteBlock(addr,src,len)) < 0)
			printf("spifWriteBlock returned %d\n",rc);
			
	}
	else if (strcmp(cmd,"read") == 0) {
		if (argc != optind+4)
			return(CMD_PARAM_ERROR);
		addr = strtoul(argv[optind+1],0,0);
		dest = (char *)strtoul(argv[optind+2],0,0);
		len = (int)strtol(argv[optind+3],0,0);
		if ((rc = spifReadBlock(addr,dest,len)) < 0)
			printf("spifReadBlock returned %d\n",rc);
	}
	else if (strcmp(cmd,"wen") == 0) {
		spifWriteEnable();	
	}
#if INCLUDE_TFS
	else if (strcmp(cmd,"tfsload") == 0) {
		rc = spifReadBlock(SPIF_TFS_BASE,(char *)FLASHRAM_BASE,SPIF_TFS_SIZE);	
		if (rc < 0)
			printf("spifReadBlock returned %d\n",rc);
	}
	else if (strcmp(cmd,"tfsstore") == 0) {
		spifWriteEnable();	
		spifGlobalUnprotect();	
		rc = spifWriteBlock(SPIF_TFS_BASE,(char *)FLASHRAM_BASE,SPIF_TFS_SIZE);	
		if (rc < 0)
			printf("spifWriteBlock returned %d\n",rc);
		spifGlobalProtect();	
	}
	else if (strcmp(cmd,"tfserase") == 0) {
		spifWriteEnable();	
		spifGlobalUnprotect();	
		addr = SPIF_TFS_BASE;
		while (addr < (SPIF_TFS_BASE+SPIF_TFS_SIZE)) {
			spifWriteEnable();	
			rc = spifBlockErase(0x10000,addr);	
			if (rc < 0) {
				printf("spifBlockErase(0x%lx) returned %d\n",addr,rc);
				break;
			}
			if (verbose)
				//ticktock();
				printf("%lx ",addr);
			addr += 0x10000;
		}
		spifGlobalProtect();	
	}
	else if (strcmp(cmd, "tfsls") == 0) {
		int ftot;
		unsigned long addr;
		TFILE tfshdr, *fp;

		ftot = 0;
		fp = &tfshdr;
		addr = BASE_OF_SERIALTFS;
		while(addr < END_OF_SERIALTFS) {
			char fbuf[32], *flags;

			if ((rc = spifReadBlock(addr,(char *)fp,TFSHDRSIZ)) < 0) {
				printf("spifReadBlock failed %d\n",rc);
				break;
			}
			if (fp->hdrsize == 0xffff)
				break;
			if (TFS_FILEEXISTS(fp)) {
				if (ftot == 0)
					printf(" Name                        Size    Offset    Flags  Info\n");
				ftot++;
				flags = tfsflagsbtoa(TFS_FLAGS(fp),fbuf);
				if ((!flags) || (!fbuf[0]))
					flags = " ";
				printf(" %-23s  %7ld  0x%08lx  %-5s  %s\n",TFS_NAME(fp),
					TFS_SIZE(fp),(unsigned long)(addr+TFSHDRSIZ),
					flags,TFS_INFO(fp));
			}
			addr += TFS_SIZE(fp);
			addr += TFSHDRSIZ;
			while(addr & 0xf) addr++;
		}
	}
	else if (strcmp(cmd, "tfsrm") == 0) {
		unsigned long addr;
		TFILE tfshdr, *fp;
		char *arg2 = argv[optind+1];

		fp = &tfshdr;
		addr = BASE_OF_SERIALTFS;
		while(addr < END_OF_SERIALTFS) {
			if ((rc = spifReadBlock(addr,(char *)fp,TFSHDRSIZ)) < 0) {
				printf("spifReadBlock failed %d\n",rc);
				break;
			}
			if (fp->hdrsize == 0xffff) {
				printf("%s not found\n",arg2);
				break;
			}
			if (strcmp(TFS_NAME(fp),arg2) == 0) {
				if (TFS_FILEEXISTS(fp)) {
					fp->flags &= ~TFS_ACTIVE;
					spifWriteEnable();	
					spifGlobalUnprotect();	
					if ((rc = spifWriteBlock(addr,(char *)fp,TFSHDRSIZ)) < 0)
						printf(" write_hdr failed %d\n",rc);
					spifGlobalProtect();	
					break;
				}
			}
			addr += TFS_SIZE(fp);
			addr += TFSHDRSIZ;
			while(addr & 0xf) addr++;
		}
	}
	else if (strcmp(cmd, "tfsadd") == 0) {
		int size;
		long	bflags;
		TFILE tfshdr, *fp;
		unsigned long addr;
		char *src, *name, *info;
		char *arg2 = argv[optind+1];
		char *arg3 = argv[optind+2];
		char *arg4 = argv[optind+3];
		char *icomma, *fcomma;

		info = "";
		bflags = 0;
		name = arg2;
		addr = BASE_OF_SERIALTFS;

		/* The incoming arguments can be either just the filename (in which
		 * case we assume the source is the file in TFS with the same name),
		 * or the filename, source address and size...
		 */
		if (argc == optind+2) {			// Just filename?
			if ((fp = tfsstat(name)) == (TFILE *)0) {
				printf("File '%s' not in TFS\n",name);
				return(CMD_FAILURE);
			}
			name = fp->name;
			info = fp->info;
			bflags = fp->flags;
			size = fp->filsize;
			src = (char *)(fp + 1);
			fp = &tfshdr;
			memset((char *)fp,0,TFSHDRSIZ);
		}
		else if (argc == optind+4) {	// Filename with addr and len
			// Extract flags and info fields (if any) from the name...
			fcomma = strchr(name,',');
			if (fcomma) {
				icomma = strchr(fcomma+1,',');
				if (icomma) {
					*icomma = 0;
					info = icomma+1;
				}
				*fcomma = 0;
				bflags = tfsctrl(TFS_FATOB,(long)(fcomma+1),0);
			}
	
			fp = &tfshdr;
			memset((char *)fp,0,TFSHDRSIZ);
			size = (int)strtol(arg4,0,0);
			src = (char *)strtoul(arg3,0,0);
		}
		else
			return(CMD_PARAM_ERROR);

		while(addr < END_OF_SERIALTFS) {
			if ((rc = spifReadBlock(addr,(char *)fp,TFSHDRSIZ)) < 0)
				break;
			if (fp->hdrsize == 0xffff) {
				unsigned long nextfileaddr;

				/* We're at the address in SPIF where we can add the new
				 * file, but first we need to make sure there's enough
				 * room...
				 */
				if ((TFSHDRSIZ + size + 16) >= (END_OF_SERIALTFS - addr)) {
					printf(" not enough space\n");
					return(CMD_FAILURE);
				}

				/* Copy name and info data to header.
				 */
				strcpy(fp->name, name);
				strcpy(fp->info, info);
				fp->hdrsize = TFSHDRSIZ;
				fp->hdrvrsn = TFSHDRVERSION;
				fp->filsize = size;
				fp->flags = bflags;
				fp->flags |= (TFS_ACTIVE | TFS_NSTALE);
				fp->filcrc = crc32((unsigned char *)src,size);
				fp->modtime = tfsGetLtime();
#if TFS_RESERVED
				{
				int	rsvd;
				for(rsvd=0;rsvd<TFS_RESERVED;rsvd++)
					fp->rsvd[rsvd] = 0xffffffff;
				}
#endif
				fp->next = 0;
				fp->hdrcrc = 0;
				fp->hdrcrc = crc32((unsigned char *)fp,TFSHDRSIZ);
				nextfileaddr = SPIF_TFSRAM_BASE - SPIF_TFS_BASE + addr + TFSHDRSIZ + size;
				if (nextfileaddr & 0xf)
					nextfileaddr = (nextfileaddr | 0xf) + 1;
				
				fp->next = (TFILE *)nextfileaddr;

				printf(" writing %s...\n",arg2);
				spifWriteEnable();	
				spifGlobalUnprotect();	
				spifWaitForReady();	

				spifWriteEnable();	
				if ((rc = spifWriteBlock(addr,(char *)fp,TFSHDRSIZ)) < 0)
					printf(" write_hdr failed %d\n",rc);
				spifWaitForReady();	

				spifWriteEnable();	
				spifGlobalUnprotect();	
				if ((rc = spifWriteBlock(addr+TFSHDRSIZ,src,size)) < 0)
					printf(" write_file failed %d\n",rc);
				spifWaitForReady();	
				spifGlobalProtect();	
				break;
			}
			if (strcmp(TFS_NAME(fp),arg2) == 0) {
				if (TFS_FILEEXISTS(fp)) {
					printf(" removing %s...\n",arg2);
					fp->flags &= ~TFS_ACTIVE;
					spifWriteEnable();	
					spifGlobalUnprotect();	
					spifWaitForReady();	
					spifWriteEnable();	
					if ((rc = spifWriteBlock(addr,(char *)fp,TFSHDRSIZ)) < 0)
						printf(" write_hdr failed %d\n",rc);
					spifGlobalProtect();	
				}
			}
			addr += TFS_SIZE(fp);
			addr += TFSHDRSIZ;
			while(addr & 0xf) addr++;
		}
	}
	else if (strcmp(cmd, "tfsstat") == 0) {
		unsigned long oaddr, addr, meminuse, memdead;
		TFILE tfshdr, *fp;

		fp = &tfshdr;
		meminuse = memdead = 0;
		addr = BASE_OF_SERIALTFS;
		while(addr < END_OF_SERIALTFS) {
			if ((rc = spifReadBlock(addr,(char *)fp,TFSHDRSIZ)) < 0) {
				printf("spifReadBlock failed %d\n",rc);
				break;
			}
			if (fp->hdrsize == 0xffff)
				break;

			oaddr = addr;
			addr += TFS_SIZE(fp);
			addr += TFSHDRSIZ;
			while(addr & 0xf) addr++;

			if (TFS_FILEEXISTS(fp))
				meminuse += addr - oaddr;
			else
				memdead += addr - oaddr;
		}
		printf("Total: 0x%x, used: 0x%x, dead: 0x%x, avail: 0x%x\n",
			(END_OF_SERIALTFS - BASE_OF_SERIALTFS),
			meminuse, memdead,
			(END_OF_SERIALTFS - BASE_OF_SERIALTFS) - (meminuse + memdead));
	}
#endif
	else
		return(CMD_PARAM_ERROR);

	return(CMD_SUCCESS);
}
