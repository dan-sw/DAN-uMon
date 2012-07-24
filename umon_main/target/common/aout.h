/*	aout.h:
 *	Header file for the AOUT file format used by TFS.
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
#ifndef _AOUT_H_
#define _AOUT_H_

struct exec {
	unsigned short	a_mid;
	unsigned short	a_magic;
	unsigned long	a_text;		/* Size of text segment in bytes */
	unsigned long	a_data;		/* Size of data segment in bytes */
	unsigned long	a_bss;		/* Size of bss segment in bytes */
	unsigned long	a_syms;		/* Size of symbol table in bytes */
	unsigned long	a_entry;	/* Entry point address */
	unsigned long	a_trsize;	/* Size of text relocation table */
	unsigned long	a_drsize;	/* Size of data relocation table */
};

struct relocation_info {
	int		r_address;
	ulong		r_info;
};

/* Fields within r_info: */
#define SYMNUM_MSK	0xffffff00
#define PCREL_MSK	0x00000080
#define LENGTH_MSK	0x00000060
#define EXTERN_MSK	0x00000010
#endif
