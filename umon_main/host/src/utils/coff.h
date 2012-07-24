
#if HOST_IS_WINNT | HOST_IS_WIN95
typedef unsigned long ulong;
#endif
typedef unsigned char uchar;

/* This header file is a conglamoration of stuff pulled together from
	filehdr.h, scnhdr.h, etc...
*/
struct filehdr {
	unsigned short	f_magic;	/* magic number */
	unsigned short	f_nscns;	/* number of sections */
	long		f_timdat;	/* time & date stamp */
	long		f_symptr;	/* file pointer to symtab */
	long		f_nsyms;	/* number of symtab entries */
	unsigned short	f_opthdr;	/* sizeof(optional hdr) */
	unsigned short	f_flags;	/* flags */
};


/*
 *   Bits for f_flags:
 *
 *	F_RELFLG	relocation info stripped from file
 *	F_EXEC		file is executable  (i.e. no unresolved
 *				externel references)
 *	F_LNNO		line nunbers stripped from file
 *	F_LSYMS		local symbols stripped from file
 *	F_MINMAL	this is a minimal object file (".m") output of fextract
 *	F_UPDATE	this is a fully bound update file, output of ogen
 *	F_SWABD		this file has had its bytes swabbed (in names)
 *	F_AR16WR	this file has the byte ordering of an AR16WR (e.g. 11/70) machine
 *				(it was created there, or was produced by conv)
 *	F_AR32WR	this file has the byte ordering of an AR32WR machine(e.g. vax)
 *	F_AR32W		this file has the byte ordering of an AR32W machine (e.g. 3b,maxi)
 *	F_PATCH		file contains "patch" list in optional header
 *	F_NODF		(minimal file only) no decision functions for
 *				replaced functions
 */

#define  F_RELFLG	0000001
#define  F_EXEC		0000002
#define  F_LNNO		0000004
#define  F_LSYMS	0000010
#define  F_MINMAL	0000020
#define  F_UPDATE	0000040
#define  F_SWABD	0000100
#define  F_AR16WR	0000200
#define  F_AR32WR	0000400
#define  F_AR32W	0001000
#define  F_PATCH	0002000
#define  F_NODF		0002000

#define	FILHDR	struct filehdr
#define	FILHSZ	sizeof(FILHDR)

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

struct aouthdr {
	short	magic;		/* see magic.h				*/
	short	vstamp;		/* version stamp			*/
	long	tsize;		/* text size in bytes, padded to FW
						   bdry					*/
	long	dsize;		/* initialized data "  "		*/
	long	bsize;		/* uninitialized data "   "		*/
	long	entry;		/* entry pt.				*/
	long	text_start;	/* base of text used for this file	*/
	long	data_start;	/* base of data used for this file	*/
};
