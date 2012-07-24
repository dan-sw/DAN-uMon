/* Status register bits for ATMEL device:
 */
#define SPRL	0x80	// Sector protection registers locked (if 1)
#define RES		0x40	// Reserved for future use
#define EPE		0x20	// Erase or program error (if 1)
#define WPP		0x10	// *WP pin is deasserted (if 1)
#define SWPS	0x0c	// Software protection status bits
#define SWPS00	0x00	//   00: all sectors unprotected
#define SWPS01	0x04	//   01: some sectors unprotected
#define SWPS10	0x08	//   10: reserved for future use
#define SWPS11	0x0c	//   11: all sectors protected (default)
#define WEL		0x02	// Write enable latch (1=write enabled)
#define BSY		0x01	// Busy (1=busy, 0=ready)

/* Baudrate could be much higher, but we keep it low here for easier
 * ability to trace out on the scope...
 */
#ifndef SPIFBAUD
#define SPIFBAUD 25000000
#endif

/* The ATMEL part supports three different erase block sizes (not
 * including the 'whole-chip' erase)...
 */
#define BLKSZ_4K	0x20
#define BLKSZ_32K	0x52
#define BLKSZ_64K	0xD8

extern void spifQinit(void);
extern int spifInit(void);
extern int spifWaitForReady(void);
extern int spifId(int verbose);
extern int spifWriteEnable(void);
extern int spifWriteDisable(void);
extern int spifChipErase(void);
extern int spifGlobalUnprotect(void);
extern int spifGlobalProtect(void);
extern int spifReadBlock(unsigned long addr,char *data,int len);
extern int spifWriteBlock(unsigned long addr, char *data, int len);
extern int spifBlockErase(int bsize, long addr);
extern unsigned short spifReadStatus(int verbose);
