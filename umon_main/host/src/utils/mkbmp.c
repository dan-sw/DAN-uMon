/* mkbmp:
 * Take an incoming file as raw gray-scale pixels and convert that data
 * to a .bmp file format.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef BUILD_WITH_VCC
extern char *optarg;
extern int optind;
#include <io.h>
#else
#include <unistd.h>
#endif

#include "utils.h"
#include "version.h"


#ifndef O_BINARY
#define O_BINARY 0
#endif

char	bfType[2];
long	bfSize, bfOffBits;
short	bfReserved1, bfReserved2;
short	biPlanes, biBitCount;
long	biSize, biHeight, biWidth, biCompression, biSizeImage;
long	biXPelsPerMeter, biYPelsPerMeter, biClrUsed, biClrImportant;

int debug;

void
bmpShowFileHeader(void)
{
	printf("BMP File header:\n");
	printf("  bfType:               %c%c (x%02x%02x)\n",
		bfType[0],bfType[1],bfType[0],bfType[1]);
	printf("  bfSize:          %7ld (x%lx)\n",bfSize,bfSize);
	printf("  bfReserved1:     %7d (x%04x)\n",bfReserved1,bfReserved1);
	printf("  bfReserved2:     %7d (x%04x)\n",bfReserved2,bfReserved2);
	printf("  bfOffBits:       %7ld (x%lx)\n",bfOffBits,bfOffBits);
}

void
bmpShowInfoHeader(void)
{
	printf("BMP Info header:\n");
	printf("  biSize:      	   %7ld (x%lx)\n",biSize,biSize);
	printf("  biWidth:     	   %7ld (x%lx)\n",biWidth,biWidth);
	printf("  biHeight:        %7ld (x%lx)\n",biHeight,biHeight);
	printf("  biPlanes:    	   %7d (x%04x)\n",biPlanes,biPlanes);
	printf("  biBitCount:  	   %7d (x%04x)\n",biBitCount,biBitCount);
	printf("  biCompression:   %7ld (x%lx)\n",biCompression,biCompression);
	printf("  biSizeImage:     %7ld (x%lx)\n",biSizeImage,biSizeImage);
	printf("  biXPelsPerMeter: %7ld (x%lx)\n",biXPelsPerMeter,biXPelsPerMeter);
	printf("  biYPelsPerMeter: %7ld (x%lx)\n",biYPelsPerMeter,biYPelsPerMeter);
	printf("  biClrUsed:       %7ld (x%lx)\n",biClrUsed,biClrUsed);
	printf("  biClrImportant:  %7ld (x%lx)\n",biClrImportant,biClrImportant);
}

char *usage_txt[] = {
	"Usage: mkbmp [options] {file}",
	"Options:",
	" -b##    bits per pixel",
	" -h##    image height (pixels)",
	" -R      read mode",
	" -w##    image width  (pixels)",
	(char *)0
};

int
main(int argc,char *argv[])
{
	short sval;
	struct	stat	mstat;
	int		ifd, ofd, opt, readmode;
	char	tmpfile[16], *infile;
	unsigned char	ibytes[2], obytes[3];

	readmode = 0;
	while((opt=getopt(argc,argv,"b:h:RVw:")) != EOF) {
		switch(opt) {
		case 'R':
			readmode = 1;
			break;
		case 'b':
			biBitCount = atoi(optarg);
			break;
		case 'h':
			biHeight = atol(optarg);
			break;
		case 'w':
			biWidth = atol(optarg);
			break;
//		case 'V':
//			showVersion();
//			break;
		default:
			usage(0);
		}
	}

	if (argc < (optind+1))
		usage(0);

	infile = argv[optind];

	
	ifd = open(infile,O_RDONLY | O_BINARY);
	if (ifd < 0) {
		perror(infile);
		usage(0);
	}
	fstat(ifd,&mstat);

	if (readmode) {
		read(ifd,bfType,2);
		read(ifd,(char *)&bfSize,4);
		read(ifd,(char *)&bfReserved1,2);
		read(ifd,(char *)&bfReserved2,2);
		read(ifd,(char *)&bfOffBits,4);
		read(ifd,(char *)&biSize,4);
		read(ifd,(char *)&biWidth,4);
		read(ifd,(char *)&biHeight,4);
		read(ifd,(char *)&biPlanes,2);
		read(ifd,(char *)&biBitCount,2);
		read(ifd,(char *)&biCompression,4);
		read(ifd,(char *)&biSizeImage,4);
		read(ifd,(char *)&biXPelsPerMeter,4);
		read(ifd,(char *)&biYPelsPerMeter,4);
		read(ifd,(char *)&biClrUsed,4);
		read(ifd,(char *)&biClrImportant,4);
		bmpShowFileHeader();
		bmpShowInfoHeader();
		close(ifd);
		exit(0);
	}


	sprintf(tmpfile,"%s.bmp",infile);
	ofd = open(tmpfile,O_WRONLY | O_BINARY | O_CREAT | O_TRUNC,0666);
	if (ofd < 0) {
		perror(tmpfile);
		usage(0);
	}

	/* Build the header prefix prior to the pixel data...
	 */
	bfType[0] = 'B';
	bfType[1] = 'M';
	bfSize = (mstat.st_size/2 * 3) + 54;
	bfReserved1 = bfReserved2 = 0;
	bfOffBits = 54;
	biSize = 40;
	biPlanes = 1;
	biCompression = 0;
	biSizeImage = bfSize - 54;
	biXPelsPerMeter = 2834;
	biYPelsPerMeter = 2834;
	biClrUsed = 0;
	biClrImportant = 0;
	write(ofd,bfType,2);
	write(ofd,&bfSize,4);
	write(ofd,&bfReserved1,2);
	write(ofd,&bfReserved2,2);
	write(ofd,&bfOffBits,4);
	write(ofd,&biSize,4);
	write(ofd,&biWidth,4);
	write(ofd,&biHeight,4);
	write(ofd,&biPlanes,2);
	write(ofd,&biBitCount,2);
	write(ofd,&biCompression,4);
	write(ofd,&biSizeImage,4);
	write(ofd,&biXPelsPerMeter,4);
	write(ofd,&biYPelsPerMeter,4);
	write(ofd,&biClrUsed,4);
	write(ofd,&biClrImportant,4);

	sval = 0;
	while(read(ifd,ibytes,2) == 2) {
		memcpy((char *)&sval,ibytes,2);
		sval /= 4;

		obytes[0] = sval;
		obytes[1] = sval;
		obytes[2] = sval;
		write(ofd,obytes,3);	
	}
	
	close(ifd);
	close(ofd);
	exit(0);
}

