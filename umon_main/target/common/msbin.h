/* msbin.h:
 * The following information was retrieved from the MSDN technical
 * support web site...
 *
 * Windows CE Binary Image Data Format (.bin):
 * The binary image file format organizes data by sections.
 * Each section contains a section header that specifies the starting
 * address, length, and checksum for that section. Romimage.exe writes
 * data organized by logical sections, such as an application's text or
 * .data region, to the .bin file. The image terminates with an image
 * record header with the physical address and checksum set to zero.
 * A configuration file formatted as a .bin file is small and fast.
 * A .bin file is about half the size of an .sre file.
 * This smaller size allows a .bin file to download faster than an
 * .sre file when you are using the Windows CE Console Debug Shell
 * tool (Cesh.exe).
 *
 * The following table shows the .bin file format. 
 *
 * FIELD						LENGTH 	DESCRIPTION
 *								(bytes)
 * Sync bytes (optional)		  7 	Byte 0 is B, indicating a .bin file
 *										format. Bytes 1-6 are reserved
 *										and set to 0, 0, 0, F, F, \n.
 *										Or in hex: 0x4230303046460a
 * Image header, consisting of:
 *  Image address				  4		Start address of image.
 *  Image length				  4		Length, in bytes, of image.
 *  One or more records of:
 *   Record address				  4		Starting address of data record.
 *										If this value is zero, the record
 *										address is the end of the file,
 *										and record length contains the
 *										starting address of the image.
 *   Record length				  4		Length of record data, in bytes.
 *   Record checksum			  4		Signed 32-bit sum of record data bytes.
 *   Record data 				  N		Record data
 */
#ifndef _MSBIN_H_
#define _MSBIN_H_

#define MSBIN_SYNC_SIZE		7
#define MSBIN_SYNC_DATA		"B000FF\n"

#define MSBINFHDR	struct msbin_imghdr
#define MSBINSHDR	struct msbin_record

/* File header:
 */
struct msbin_imghdr {
	ulong	imageaddr;
	ulong	imagelen;
};

/* Section header:
 */
struct msbin_record {
	ulong	addr;
	ulong	len;
	ulong	csum;
};

#endif
