/* bedit.c:
 * Binary editor...
 * This tool allows a user to insert, delete or modify a file one BIT at
 * a time.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>
#include "utils.h"
#ifdef BUILD_WITH_VCC
#include <io.h>
typedef unsigned long ulong;
typedef unsigned short ushort;
int	optind;
char *optarg;
int getopt(int,char **,char *);
#else
#include <unistd.h>
#endif

#ifndef O_BINARY
#define O_BINARY 0
#endif

typedef unsigned char uchar;

int verbose = 0;
int debug = 0;
uchar shiftmask[] = { 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff };
uchar ormask[]    = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
uchar andmask[]   = { 0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f };

/* lshift():
 * Take the incoming byte, shift it left on bit and then insert
 * a one or zero (based on value of bit) into the LSB position.
 * Return the value of the MSB prior to the shift.
 */
int
lshift(uchar *byte, int bit)
{
	int upperbit = *byte & 0x80;

	*byte <<= 1;
	if (bit)
		*byte |= 0x01;
	else
		*byte &= 0xfe;

	return(upperbit);
}

/* rshift():
 * Take the incoming byte, shift it right on bit and then insert
 * a one or zero (based on value of bit) into the MSB position.
 * Return the value of the LSB prior to the shift.
 */
int
rshift(uchar *byte, int bit)
{
	int lowerbit = *byte & 1;

	*byte >>= 1;
	if (bit)
		*byte |= 0x80;
	else
		*byte &= 0x7f;

	return(lowerbit);
}

/* insert_bit():
 * Insert a bit into the location specified by the offset into the buffer.
 */
int
insert_bit(uchar *buffer,int bsize,int eighths,int offset,int bitno,int one)
{
	uchar *cp, shiftportion, *end;
	int	lowerbit;

	if (verbose)
		printf("insert_bit %d at offset=%d, bitno=%d\n",one,offset,bitno);

	bsize += eighths/8;
	if (eighths % 8)
		bsize++;

	cp = buffer + offset;

	/* Insert the new bit:
	 */
	lowerbit = *cp & 1;
	shiftportion = *cp & shiftmask[bitno];
	*cp  &= ~shiftmask[bitno];
	shiftportion >>= 1;
	if (one)
		*cp |= ormask[bitno];
	else
		*cp &= andmask[bitno];
	*cp  |= shiftportion;

	/* Shift all remaining bits:
	 */
	cp++;
	end = buffer+bsize;
	for(;cp<end;cp++)
		lowerbit = rshift(cp,lowerbit);
	
	return(0);
}

/* delete_bit():
 * Delete a bit from the location specified by the offset into the buffer.
 */
int
delete_bit(uchar *buffer,int bsize,int eighths,int offset,int bitno)
{
	uchar *cp, shiftportion, *end;
	int	upperbit;

	if (verbose)
		printf("delete bit %d at offset %d\n",bitno,offset);

	bsize += eighths/8;
	if (eighths % 8)
		bsize++;

	cp = buffer + offset;

	/* Delete the specified bit:
	 */
	upperbit = *(cp+1) & 0x80;

	shiftportion = *cp & shiftmask[bitno];
	*cp  &= ~shiftmask[bitno];
	shiftportion <<= 1;
	*cp  |= shiftportion;
	if (upperbit)
		*cp |= 1;

	/* Shift all remaining bits:
	 */
	cp++;
	upperbit = 0;
	end = buffer+bsize;
	for(;cp<=end;end--) 
		upperbit = lshift(end,upperbit);
	
	return(0);
}

/* replace_bit():
 * Replace a bit at the location specified by the offset into the buffer.
 */
void
replace_bit(uchar *buffer,int offset,int bitno,int one)
{
	uchar *cp;

	if (verbose)
		printf("replace bit %d at offset %d with %d\n",bitno,offset,one);

	cp = buffer + offset;
	if (one)
		*cp |= ormask[bitno];
	else
		*cp &= andmask[bitno];
}

void
showsize(int fsize, int eighths)
{
	printf("File size: ");
	fsize += eighths/8;
	eighths = eighths % 8;
	printf("%d %d/8th bytes\n",fsize,eighths);
}

int
writefile(char *fname,uchar *data, int size, int eighths)
{
	int fd;

	size += eighths/8;
	eighths = eighths % 8;
	if (eighths)
		size += 1;

	printf("writing file: <%s>, size %d bytes\n",fname,size);
	fd = open(fname,O_WRONLY | O_BINARY | O_CREAT | O_TRUNC,0666);
	if (fd < 0)  {
		perror(fname);
		return(-1);
	}
	else {
		if (write(fd,data,size) != size)
			perror(fname);
		close(fd);
		return(0);
	}
}

char	*usage_txt[] = {
	"bedit - A bit-level file editor.",
	" Usage: bedit [options] {infile}",
	"",
	" Options:",
	"   -o {ofile}   out file (default=infile.bs)",
	" Commands:",
	"   1:{offset,bit}      insert 1 at offset/bitnumber",
	"   0:{offset,bit}      insert 0 at offset/bitnumber",
	"   d:{offset,bit}      delete offset/bitnumber",
	"   q                   quit and write new file",
	"   r:{offset,bit,val}  replace bit at offset with val",
	"   s                   show buffer",
	"   v                   toggle verbose mode",
	"   w                   write file but do not exit",
	"   x                   quit and do not write new file",
	0,
};

void
showcommands(void)
{
	int idx;

	idx = 5;
	printf("\n");
	while(usage_txt[idx])
		printf("%s\n",usage_txt[idx++]);

	printf("Notes:\n");
	printf(" *  File offset starts at 0\n");
	printf(" *  Byte 0x04 as bits: bit 7 -> 00000100 <- bit 0\n");
	printf("\n");
}


int
main(int argc,char *argv[])
{
	uchar	*data;
	char	command[128];
	int		fd, opt, eighths, fsize, offset, bitno, val;
	struct	stat	mstat;
	char	*ifile, ofile[128], *cp;
	
	ofile[0] = 0;
	while((opt=getopt(argc,argv,"o:V")) != EOF) {
		switch(opt) {
		case 'o':
			strcpy(ofile,optarg);
			exit(1);
		case 'V':
			printf(" Built: %s @ %s\n",__DATE__,__TIME__);
			exit(1);
		default:
			usage(0);
			break;
		}
	}

	if (argc < (optind + 1)) 
		usage("Bad argcnt");

	ifile = argv[optind];

	/* Open binary input file: */
	fd = open(ifile,O_RDONLY | O_BINARY);
	if (fd == -1) {
		perror(ifile);
		usage(0);
	}
	fstat(fd,&mstat);
	fprintf(stderr,"Binary file size=%d\n",mstat.st_size);
	data = calloc(mstat.st_size + 64,1);
	if (data == (uchar *)0) {
		perror("malloc");
		exit(1);
	}
	if (read(fd,data,mstat.st_size) != mstat.st_size) {
		perror("read");
		exit(1);
	}
	close(fd);

	/* Build the out-file name...
	 */
	if (ofile[0] == 0)
		sprintf(ofile,"%s.bs",ifile);

	fsize = mstat.st_size - 1;
	eighths = 8;
	showcommands();
	while(1) {
		printf("BEDIT> ");
		fflush(stdout);
		fgets(command,sizeof(command),stdin);
		command[strlen(command)-1] = 0;
		if (verbose)
			printf("CMD=%c\n",command[0]);
		if (strncmp(command,"1:",2) == 0) {
			if (eighths == 0) {
				fsize++;
				eighths = 1;
			}
			else
				eighths++;

			offset = strtol(command+2,&cp,0);
			bitno = strtol(cp+1,0,0);
			insert_bit(data,fsize,eighths,offset,bitno,1);
		}
		else if (strncmp(command,"0:",2) == 0) {
			if (eighths == 0) {
				fsize++;
				eighths = 1;
			}
			else
				eighths++;

			offset = strtol(command+2,&cp,0);
			bitno = strtol(cp+1,0,0);
			insert_bit(data,fsize,eighths,offset,bitno,0);
		}
		else if (strcmp(command,"?") == 0) {
			showcommands();
		}
		else if (strncmp(command,"d:",2) == 0) {
			if (eighths == 0) {
				fsize--;
				eighths = 7;
			}
			else
				eighths--;

			offset = strtol(command+2,&cp,0);
			bitno = strtol(cp+1,0,0);
			delete_bit(data,fsize,eighths,offset,bitno);
		}
		else if ((strcmp(command,"quit") == 0) ||
				 (strcmp(command,"q") == 0)) {
			break;
		}
		else if (strncmp(command,"r:",2) == 0) {
			offset = strtol(command+2,&cp,0);
			bitno = strtol(cp+1,&cp,0);
			val = strtol(cp+1,0,0);
			replace_bit(data,offset,bitno,val);
		}
		else if (strcmp(command,"s") == 0) {
			int i, row, col, tmpsize, tmpeighths;

			tmpsize = fsize;
			tmpeighths = eighths;
			tmpsize += tmpeighths/8;
			tmpeighths = tmpeighths % 8;
			if (tmpeighths)
				tmpsize += 1;

			printf("00000: ");
			for(row=0,col=0,i=0;i<tmpsize;i++,col++) {
				printf("%02x ",data[i]);
				if (col == 7)
					printf("  ");
				else if (col == 15) {
					printf("\n");
					col = -1;
					if (++row == 24) {
						row = 0;
						printf("-hit enter to continue-");
						fflush(stdout);
						fgets(command,sizeof(command),stdin);
						command[strlen(command)-1] = 0;
						if (command[0] != 0)
							break;
					}
					printf("%05x: ",i+1);
				}
			}
			printf("\n");
			showsize(fsize,eighths);
		}
		else if (strcmp(command,"v") == 0) {
			verbose = ~verbose;
		}
		else if (strcmp(command,"w") == 0) {
			if (writefile(ofile,data,fsize,eighths) == 0)
				showsize(fsize,eighths);
		}
		else if (strcmp(command,"x") == 0) {
			showsize(fsize,eighths);
			free(data);
			exit(0);
		}
		else if (command[0] != 0)
			fprintf(stderr,"Cmd: %s not recognized\n",command);
	}

	if (writefile(ofile,data,fsize,eighths) == 0)
		showsize(fsize,eighths);
	
	free(data);
	return(0);
}

