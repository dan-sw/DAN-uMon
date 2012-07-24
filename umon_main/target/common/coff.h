/* coff.h:
 *	Header file used for the COFF file format of TFS.
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
#ifndef _COFF_H_
#define _COFF_H_

/* File header... */
struct filehdr {
	unsigned short	f_magic;	/* magic number */
	unsigned short	f_nscns;	/* number of sections */
	long			f_timdat;	/* time & date stamp */
	long			f_symptr;	/* file pointer to symtab */
	long			f_nsyms;	/* number of symtab entries */
	unsigned short	f_opthdr;	/* sizeof(optional hdr) */
	unsigned short	f_flags;	/* flags */
};

#define	FILHDR	struct filehdr
#define	FILHSZ	sizeof(FILHDR)
#define	F_EXEC	0000002

/* Optional header... */
struct aouthdr {
	short	magic;	
	short	vstamp;		/* version stamp			*/
	long	tsize;		/* text size in bytes, padded to FW
						   bdry					*/
	long	dsize;		/* initialized data "  "		*/
	long	bsize;		/* uninitialized data "   "		*/
	long	entry;		/* entry pt.				*/
	long	text_start;	/* base of text used for this file	*/
	long	data_start;	/* base of data used for this file	*/
};

#define	AOUTHDR	struct aouthdr
#define	AOUTHSZ	sizeof(AOUTHDR)

struct scnhdr {
	char		s_name[8];	/* section name */
	long		s_paddr;	/* physical address */
	long		s_vaddr;	/* virtual address */
	long		s_size;		/* section size */
	long		s_scnptr;	/* file ptr to raw data for section */
	long		s_relptr;	/* file ptr to relocation */
	long		s_lnnoptr;	/* file ptr to line numbers */
	unsigned short	s_nreloc;	/* number of relocation entries */
	unsigned short	s_nlnno;	/* number of line number entries */
	long		s_flags;	/* flags */
};

#define	SCNHDR	struct scnhdr
#define	SCNHSZ	sizeof(SCNHDR)

/*
 * The low 4 bits of s_flags is used as a section "type"
 */

#define STYP_REG	0x00		/* "regular" section:
									allocated, relocated, loaded */
#define STYP_DSECT	0x01		/* "dummy" section:
									not allocated, relocated,
									not loaded */
#define STYP_NOLOAD	0x02		/* "noload" section:
									allocated, relocated,
									 not loaded */
#define STYP_GROUP	0x04		/* "grouped" section:
									formed of input sections */
#define STYP_PAD	0x08		/* "padding" section:
									not allocated, not relocated,
									 loaded */
#define STYP_COPY	0x10		/* "copy" section:
									for decision function used
									by field update;  not
									allocated, not relocated,
									loaded;  reloc & lineno
									entries processed normally */
#define	STYP_TEXT	0x20		/* section contains text only */
#define STYP_DATA	0x40		/* section contains data only */
#define STYP_BSS	0x80		/* section contains bss only */
#define STYP_INFO	0x0200
#define STYP_LIT	0x8020
#define STYP_ABS	0x4000
#define STYP_BSSREG	0x1200
#define STYP_ENVIR	0x2200

#define ISLOADABLE(flags) \
	(flags & (STYP_ABS | STYP_TEXT | STYP_LIT | STYP_DATA))
#define ISBSS(flags) (flags & (STYP_BSS))
#define ISTEXT(flags) (flags & (STYP_TEXT))
#endif
