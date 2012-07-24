/* ecc.c:
 * This code is an implementation of the discussion and pseudo-code
 * of Application Note 1823 from STMicroelectronics titled:
 *
 * "Error correction code in single level cell NAND Flash memories"
 *
 * dated July 2007.
 *
 * Note:
 * I found with the original code that I was not able to successfully
 * detect/correct single bit errors in the 512-byte packets if the error
 * was in upper 256 bytes of the packet.  After some investigation I made
 * two changes to the code (see tags CHANGE_1 and CHANGE_2 below).
 * Referring to the line tagged CHANGE_1, this was "if (i & 0xa0)" in
 * the pseudo code.  Referring to the line tagged CHANGE_2, this is an
 * addition to pull in the next bit of the byte address out of the third
 * byte of the ECC code.  
 *
 * I've tested this pretty heavily using the STANDALONE build for this 
 * and it has passed completely for 256 and 512 byte packets.  Obviously,
 * without these changes, it did not pass.
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
 */
#ifdef STANDALONE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#endif

typedef unsigned char uchar;

int bit_addr, byte_addr;

/* ecc_gen():
 * Calculate a  3-byte ECC based on either 256 or 512 bytes of incoming
 * data.  Return void because this will never return an error.
 */
void
ecc_gen(int dsize, uchar *data, uchar *ecc_code)
{
	int i;

	char cp0, cp1, cp2, cp3, cp4, cp5;
	char b7, b6, b5, b4, b3, b2, b1, b0;
	char lp0, lp1, lp2, lp3, lp4, lp5, lp6, lp7, lp8, lp9;
	char lp10, lp11, lp12, lp13, lp14, lp15, lp16, lp17;

	cp0 = cp1 = cp2 = cp3 = cp4 = cp5 = 0;
	lp0 = lp1 = lp2 = lp3 = lp4 = lp5 = lp6 = lp7 = lp8 = lp9 = 0;
	lp10 = lp11 = lp12 = lp13 = lp14 = lp15 = lp16 = lp17 = 0;

	for(i=0;i<dsize;i++) {
		b0 = (*data & 0x01);
		b1 = ((*data & 0x02) >> 1);
		b2 = ((*data & 0x04) >> 2);
		b3 = ((*data & 0x08) >> 3);
		b4 = ((*data & 0x10) >> 4);
		b5 = ((*data & 0x20) >> 5);
		b6 = ((*data & 0x40) >> 6);
		b7 = ((*data & 0x80) >> 7);
		data++;

		if (i & 0x01) 
			lp1 = b7 ^ b6 ^ b5 ^ b4 ^ b3 ^ b2 ^ b1 ^ b0 ^ lp1;
		else
			lp0 = b7 ^ b6 ^ b5 ^ b4 ^ b3 ^ b2 ^ b1 ^ b0 ^ lp0;
		
		if (i & 0x02) 
			lp3 = b7 ^ b6 ^ b5 ^ b4 ^ b3 ^ b2 ^ b1 ^ b0 ^ lp3;
		else 
			lp2 = b7 ^ b6 ^ b5 ^ b4 ^ b3 ^ b2 ^ b1 ^ b0 ^ lp2;
		
		if (i & 0x04) 
			lp5 = b7 ^ b6 ^ b5 ^ b4 ^ b3 ^ b2 ^ b1 ^ b0 ^ lp5;
		else
			lp4 = b7 ^ b6 ^ b5 ^ b4 ^ b3 ^ b2 ^ b1 ^ b0 ^ lp4;
		
		if (i & 0x08)
			lp7 = b7 ^ b6 ^ b5 ^ b4 ^ b3 ^ b2 ^ b1 ^ b0 ^ lp7;
		else
			lp6 = b7 ^ b6 ^ b5 ^ b4 ^ b3 ^ b2 ^ b1 ^ b0 ^ lp6;
		
		if (i & 0x10)
			lp9 = b7 ^ b6 ^ b5 ^ b4 ^ b3 ^ b2 ^ b1 ^ b0 ^ lp9;
		else
			lp8 = b7 ^ b6 ^ b5 ^ b4 ^ b3 ^ b2 ^ b1 ^ b0 ^ lp8;
		
		if (i & 0x20)
			lp11 = b7 ^ b6 ^ b5 ^ b4 ^ b3 ^ b2 ^ b1 ^ b0 ^ lp11;
		else
			lp10 = b7 ^ b6 ^ b5 ^ b4 ^ b3 ^ b2 ^ b1 ^ b0 ^ lp10;
		
		if (i & 0x40)
			lp13 = b7 ^ b6 ^ b5 ^ b4 ^ b3 ^ b2 ^ b1 ^ b0 ^ lp13;
		else
			lp12 = b7 ^ b6 ^ b5 ^ b4 ^ b3 ^ b2 ^ b1 ^ b0 ^ lp12;
		
		if (i & 0x80)
			lp15 = b7 ^ b6 ^ b5 ^ b4 ^ b3 ^ b2 ^ b1 ^ b0 ^ lp15;
		else
			lp14 = b7 ^ b6 ^ b5 ^ b4 ^ b3 ^ b2 ^ b1 ^ b0 ^ lp14;
		
		if (dsize == 512) {
			if (i & 0x100)	/* CHANGE_1 (note at top of file) */
				lp17 = b7 ^ b6 ^ b5 ^ b4 ^ b3 ^ b2 ^ b1 ^ b0 ^ lp17;
			else
				lp16 = b7 ^ b6 ^ b5 ^ b4 ^ b3 ^ b2 ^ b1 ^ b0 ^ lp16;
		}
		
		cp0 = b6 ^ b4 ^ b2 ^ b0 ^ cp0;
		cp1 = b7 ^ b5 ^ b3 ^ b1 ^ cp1;
		cp2 = b5 ^ b4 ^ b1 ^ b0 ^ cp2;
		cp3 = b7 ^ b6 ^ b3 ^ b2 ^ cp3;
		cp4 = b3 ^ b2 ^ b1 ^ b0 ^ cp4;
		cp5 = b7 ^ b6 ^ b5 ^ b4 ^ cp5;
	}

	ecc_code[0] = ((lp7 << 7) | (lp6 << 6) | (lp5 << 5) | (lp4 << 4) |
					(lp3 << 3) | (lp2 << 2) | (lp1 << 1) | lp0);
	ecc_code[1] = ((lp15 << 7) | (lp14 << 6) | (lp13 << 5) | (lp12 << 4) |
					(lp11 << 3) | (lp10 << 2) | (lp9 << 1) | lp8);
	ecc_code[2] = ((cp5 << 7) | (cp4 << 6) | (cp3 << 5) | (cp2 << 4) |
					(cp1 << 3) | (cp0 << 2) | (lp17 << 1) | lp16);
}

/* ecc_check():
 * Detect and correct a 1-bit error; where:
 *    'stored_ecc' is the ECC stored in the NAND flash memory
 *    'new_ecc'    is the ECC code generated on the data page read
 *
 * Return:
 *	 0 if no error
 *	 1 if exactly 1 error (the error is corrected in the data)
 *  -2 if the error is in the ECC itself
 *  -1 if the error is not correctable
 */
int
ecc_check(int dsize, uchar *data, uchar *stored_ecc, uchar *new_ecc)
{
	uchar mask;
	int i, j, btot;
	ushort ecc_xor[3];

	ecc_xor[0] = stored_ecc[0] ^ new_ecc[0];
	ecc_xor[1] = stored_ecc[1] ^ new_ecc[1];
	ecc_xor[2] = stored_ecc[2] ^ new_ecc[2];

	if ((ecc_xor[0] == 0) && (ecc_xor[1] == 0) && (ecc_xor[2] == 0))
		return(0);	/* No errors */

	btot = 0;
	for(i=0;i<3;i++) {
		mask = 0x80;

		for(j=0;j<8;j++) {
			if (ecc_xor[i] & mask)
				btot++;
			mask >>= 1;
		}
	}

//	printf("ecc_xor: %02x %02x %02x; btot=%d\n",
//		ecc_xor[0],ecc_xor[1],ecc_xor[2],btot);

	if (((dsize == 256) && (btot == 11)) ||
		((dsize == 512) && (btot == 12))) {

		bit_addr = (((ecc_xor[2] >> 3) & 0x01) |
					((ecc_xor[2] >> 4) & 0x02) |
					((ecc_xor[2] >> 5) & 0x04));

		byte_addr =
				(((ecc_xor[0] >> 1) & 0x01) |
				 ((ecc_xor[0] >> 2) & 0x02) |
				 ((ecc_xor[0] >> 3) & 0x04) |
				 ((ecc_xor[0] >> 4) & 0x08) |
				 ((ecc_xor[1] << 3) & 0x10) |
				 ((ecc_xor[1] << 2) & 0x20) |
				 ((ecc_xor[1] << 1) & 0x40) |
				  (ecc_xor[1] & 0x80));

		if (dsize == 512)	/* CHANGE_2 (see note at top of file) */
			byte_addr |= ((ecc_xor[2] << 7) & 0x100);

//		printf("Error: bit %d of byte %d\n",bit_addr, byte_addr);
		data[byte_addr] = data[byte_addr] ^ (1 << bit_addr);
		return(1);
	}
	else if (btot == 1) {
		stored_ecc[0] = new_ecc[0];
		return(-2);	/* Error in ECC */
	}
	else {
		return(-1);	/* Uncorrectable error in data */
	}
}

#ifdef STANDALONE
/* main():
 * This main() code simply demonstrates the use of the two above functions.
 * In a real system, the data and calculated ecc would have been written
 * to NAND.  At some time later, the data and ecc would be read back out
 * and verified by passing the data, the written ecc and the new ecc to
 * ecc_check()...
 * To simulate this, we pass the content of a file to ecc_gen() to generate
 * the inititial ecc (as would have been stored in NAND with the data).  Then
 * we toggle one bit in the buffer and send it to ecc_gen() a second time
 * (as would have been done if the data was read back out of NAND some time
 * later).  After reading the data and calculating the second ecc, the
 * ecc_check() function is called with the data, the original ecc and the
 * new ecc.  It determines if there was an error, and if possible, makes
 * the correction.
 */
int
main(int argc, char *argv[])
{
	struct stat	fstat;
	char		*fname;
	uchar		buf[512], cpy[512], ecc1[3], ecc2[3];
	int			fd, size, rc, bytenum, bitnum;

	if (argc != 3) {
		fprintf(stderr,"Usage: %s {filename} {256|512}\n",argv[0]);
		exit(1);
	}
	fname = argv[1];
	size = atoi(argv[2]);

	if ((size != 256) && (size != 512)) {
		fprintf(stderr,"Error: size must be 256 or 512\n");
		exit(1);
	}

	if ((fd = open(fname,O_RDONLY | O_BINARY)) == -1) {
		perror(fname);
		exit(1);
	}

	if (stat(fname,&fstat) == -1) {
		perror(fname);
		return(1);
	}
	if (fstat.st_size < size) {
		fprintf(stderr,"File isn't %d bytes in size\n",size);
		exit(1);
	}

	if (read(fd,(char *)buf,size) != size) {
		perror("read");
		exit(1);
	}
	close(fd);
	memcpy(cpy,buf,size);

	ecc_gen(size, buf, ecc1);

	for(bytenum=0;bytenum<size;bytenum++) {
		if (memcmp(buf,cpy,size) != 0) {
			printf("CMP %d %d\n",bitnum,bytenum);
			exit(1);
		}
		for(bitnum=0;bitnum<8;bitnum++) {
			/* Insert an error:
			 */
			if (buf[bytenum] & (1 << bitnum))
				buf[bytenum] &= ~(1 << bitnum);
			else
				buf[bytenum] |= (1 << bitnum);

			/* Compute ecc2 (with the error):
			 */
			ecc_gen(size, buf, ecc2);

			/* Find the error:
			 */
			rc = ecc_check(size,buf, ecc1, ecc2);
			if (rc != 1) { 
				printf("NOPE %d %d\n", bitnum,bytenum);
				exit(1);
			}
			if (bit_addr != bitnum) {
				printf("BIT__ERR %d %d\n", bit_addr,bitnum);
				exit(1);
			}
			if (byte_addr != bytenum) {
				printf("BYTE_ERR %d %d\n", byte_addr,bytenum);
				exit(1);
			}
		}
	}
	exit(0);
}
#endif
