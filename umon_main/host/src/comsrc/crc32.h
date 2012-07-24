extern unsigned long partial_crc32(unsigned long crcval,unsigned char *buffer,unsigned long nbytes);
extern unsigned long crc32(unsigned char *buffer,unsigned long nbytes);
extern int fcrc32(char *filename, unsigned long *crc, int verbose);
