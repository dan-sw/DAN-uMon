#define CFI_MODE_UNKNOWN	0

/* Three portions of information are contained within the
 * device's mode...
 * 1. The device's maximum configurable width (MAXWIDTH).
 * 2. The device's configured width in this system (CFGWIDTH).
 * 3. The number of devices in one addressable parallel block (PARTOT).
 */

#define CFI_MAXWIDTH_MASK	0x000f
#define CFI_MAXWIDTH_NULL	0
#define CFI_MAXWIDTH_8		1
#define CFI_MAXWIDTH_16		2
#define CFI_MAXWIDTH_32		3
#define CFI_MAXWIDTH_64		4
#define CFI_MAXWIDTH(a)		(a & CFI_MAXWIDTH_MASK)

#define CFI_CFGWIDTH_MASK	0x00f0
#define CFI_CFGWIDTH_NULL	0
#define CFI_CFGWIDTH_8		(1 << 4)
#define CFI_CFGWIDTH_16		(2 << 4)
#define CFI_CFGWIDTH_32		(3 << 4)
#define CFI_CFGWIDTH_64		(4 << 4)
#define CFI_CFGWIDTH(a)		((a & CFI_CFGWIDTH_MASK) >> 4)

#define CFI_PARTOT_MASK		0x0f00
#define CFI_PARTOT_NULL		0
#define CFI_PARTOT_1		(1 << 8)
#define CFI_PARTOT_2		(2 << 8)
#define CFI_PARTOT_4		(3 << 8)
#define CFI_PARTOT(a)		((a & CFI_PARTOT_MASK) >> 8)


/* Some typical modes:
 * MODE_X_Y_Z 
 * where...
 *		X is MAXWIDTH
 *		Y is CFGWIDTH
 *		Z is PARTOT
 */
#define MODE_8_8_1		(CFI_MAXWIDTH_8 | CFI_CFGWIDTH_8 | CFI_PARTOT_1)
#define MODE_8_8_2		(CFI_MAXWIDTH_8 | CFI_CFGWIDTH_8 | CFI_PARTOT_2)
#define MODE_8_8_4		(CFI_MAXWIDTH_8 | CFI_CFGWIDTH_8 | CFI_PARTOT_4)
#define MODE_16_8_1		(CFI_MAXWIDTH_16 | CFI_CFGWIDTH_8 | CFI_PARTOT_1)
#define MODE_16_8_2		(CFI_MAXWIDTH_16 | CFI_CFGWIDTH_8 | CFI_PARTOT_2)
#define MODE_16_8_4		(CFI_MAXWIDTH_16 | CFI_CFGWIDTH_8 | CFI_PARTOT_4)
#define MODE_16_16_1	(CFI_MAXWIDTH_16 | CFI_CFGWIDTH_16 | CFI_PARTOT_1)
#define MODE_16_16_2	(CFI_MAXWIDTH_16 | CFI_CFGWIDTH_16 | CFI_PARTOT_2)
#define MODE_32_8_1		(CFI_MAXWIDTH_32 | CFI_CFGWIDTH_8 | CFI_PARTOT_1)
#define MODE_32_8_2		(CFI_MAXWIDTH_32 | CFI_CFGWIDTH_8 | CFI_PARTOT_2)
#define MODE_32_8_4		(CFI_MAXWIDTH_32 | CFI_CFGWIDTH_8 | CFI_PARTOT_4)
#define MODE_32_16_1	(CFI_MAXWIDTH_32 | CFI_CFGWIDTH_16 | CFI_PARTOT_1)
#define MODE_32_16_2	(CFI_MAXWIDTH_32 | CFI_CFGWIDTH_16 | CFI_PARTOT_2)
#define MODE_32_32_1	(CFI_MAXWIDTH_32 | CFI_CFGWIDTH_32 | CFI_PARTOT_1)

#define CFI_DELAY_COUNT	1000000

extern int getCFIMode(unsigned long devbase, int verbose);
extern void printCfiMode(int mode);

extern int cfi_mode(unsigned long devbase);
