/*
a.out - format of executable binary files

     The include file a.out.h declares three structures and several macros.
     The structures describe the format of executable machine code files
     (`binaries') on the system.

     A binary file consists of up to 7 sections.  In order, these sections
     are:

     exec header       Contains parameters used by the kernel to load a binary
                       file into memory and execute it, and by the link editor
                       ld(1) to combine a binary file with other binary files.
                       This section is the only mandatory one.

     text segment      Contains machine code and related data that are loaded
                       into memory when a program executes.  May be loaded
                       read-only.

     data segment      Contains initialized data; always loaded into writable
                       memory.

     text relocations  Contains records used by the link editor to update
                       pointers in the text segment when combining binary
                       files.

     data relocations  Like the text relocation section, but for data segment
                       pointers.

     symbol table      Contains records used by the link editor to cross ref-
                       erence the addresses of named variables and functions
                       (`symbols') between binary files.

     string table      Contains the character strings corresponding to the
                       symbol names.

     Every binary file begins with an exec structure:

           struct exec {
                   unsigned short  a_mid;
                   unsigned short  a_magic;
                   unsigned long   a_text;
                   unsigned long   a_data;
                   unsigned long   a_bss;
                   unsigned long   a_syms;
                   unsigned long   a_entry;
                   unsigned long   a_trsize;
                   unsigned long   a_drsize;
           };

     The fields have the following functions:

     a_mid     Contains a bit pattern that identifies binaries that were built
               fies binary files and distinguishes different loading conven-
               tions.  The field must contain one of the following values:

               OMAGIC  The text and data segments immediately follow the head-
                       er and are contiguous.  The kernel loads both text and
                       data segments into writable memory.

               NMAGIC  As with OMAGIC, text and data segments immediately fol-
                       low the header and are contiguous.  However, the kernel
                       loads the text into read-only memory and loads the data
                       into writable memory at the next page boundary after
                       the text.

               ZMAGIC  The kernel loads individual pages on demand from the
                       binary.  The header, text segment and data segment are
                       all padded by the link editor to a multiple of the page
                       size.  Pages that the kernel loads from the text seg-
                       ment are read-only, while pages from the data segment
                       are writable.

               QMAGIC  Like ZMAGIC, but the executable header is merged into
                       the start of the text segment.  This tactic saves a
                       page's worth of disk space for most binaries.  Also,
                       the kernel loads the image at an offset of one page
                       from the start of the address space, to permit trapping
                       of null pointer dereferences.

     a_text    Contains the size of the text segment in bytes.  Includes the
               size of the executable header for QMAGIC binaries.

     a_data    Contains the size of the data segment in bytes.

     a_bss     Contains the number of bytes in the `bss segment' and is used
               by the kernel to set the initial break (brk(2))  after the data
               segment.  The kernel loads the program so that this amount of
               writable memory appears to follow the data segment and initial-
               ly reads as zeroes.

     a_syms    Contains the size in bytes of the symbol table section.

     a_entry   Contains the address in memory of the entry point of the pro-
               gram after the kernel has loaded it; the kernel starts the exe-
               cution of the program from the machine instruction at this ad-
               dress.

     a_trsize  Contains the size in bytes of the text relocation table.

     a_drsize  Contains the size in bytes of the data relocation table.

     The a.out.h include file defines several macros which use an exec struc-
     ture to test consistency or to locate section offsets in the binary file.

     N_BADMAG(exec)   Nonzero if the a_magic field does not contain a recog-
                      nized value.

     N_TXTOFF(exec)   The byte offset in the binary file of the beginning of
                      the text segment.

     N_DATADDR(exec)  Similarly, the address of the start of the data segment.

     Relocation records have a standard format which is described by the
     relocation_info structure:

           struct relocation_info {
                   int             r_address;
                   unsigned int    r_symbolnum : 24,
                                   r_pcrel : 1,
                                   r_length : 2,
                                   r_extern : 1,
                                   : 4;
           };

     The relocation_info fields are used as follows:

     r_address    Contains the byte offset of a pointer that needs to be link-
                  edited.  Text relocation offsets are reckoned from the start
                  of the text segment, and data relocation offsets from the
                  start of the data segment.  The link editor adds the value
                  that is already stored at this offset into the new value
                  that it computes using this relocation record.

     r_symbolnum  Contains the ordinal number of a symbol structure in the
                  symbol table (it is not a byte offset).  After the link edi-
                  tor resolves the absolute address for this symbol, it adds
                  that address to the pointer that is undergoing relocation.
                  (If the r_extern bit is clear, the situation is different;
                  see below.)

     r_pcrel      If this is set, the link editor assumes that it is updating
                  a pointer that is part of a machine code instruction using
                  pc-relative addressing.  The address of the relocated point-
                  er is implicitly added to its value when the running program
                  uses it.

     r_length     Contains the log base 2 of the length of the pointer in
                  bytes; 0 for 1-byte displacements, 1 for 2-byte displace-
                  ments, 2 for 4-byte displacements.

     r_extern     Set if this relocation requires an external reference; the
                  link editor must use a symbol address to update the pointer.
                  When the r_extern bit is clear, the relocation is `local';
                  the link editor updates the pointer to reflect changes in
                  the load addresses of the various segments, rather than
                  changes in the value of a symbol.  In this case, the content
                  of the r_symbolnum field is an n_type value (see below);
                  this type field tells the link editor what segment the relo-
                  cated pointer points into.

     Symbols map names to addresses (or more generally, strings to values).
     Since the link-editor adjusts addresses, a symbol's name must be used to
     stand for its address until an absolute value has been assigned.  Symbols
     consist of a fixed-length record in the symbol table and a variable-
     length name in the string table.  The symbol table is an array of nlist
     structures:

           };

     The fields are used as follows:

     n_un.n_strx  Contains a byte offset into the string table for the name of
                  this symbol.  When a program accesses a symbol table with
                  the nlist(3) function, this field is replaced with the
                  n_un.n_name field, which is a pointer to the string in memo-
                  ry.

     n_type       Used by the link editor to determine how to update the sym-
                  bol's value.  The n_type field is broken down into three
                  sub-fields using bitmasks.  The link editor treats symbols
                  with the N_EXT type bit set as `external' symbols and per-
                  mits references to them from other binary files.  The N_TYPE
                  mask selects bits of interest to the link editor:

                  N_UNDF  An undefined symbol.  The link editor must locate an
                          external symbol with the same name in another binary
                          file to determine the absolute value of this symbol.
                          As a special case, if the n_value field is nonzero
                          and no binary file in the link-edit defines this
                          symbol, the link-editor will resolve this symbol to
                          an address in the bss segment, reserving an amount
                          of bytes equal to n_value. If this symbol is unde-
                          fined in more than one binary file and the binary
                          files do not agree on the size, the link editor
                          chooses the greatest size found across all binaries.

                  N_ABS   An absolute symbol.  The link editor does not update
                          an absolute symbol.

                  N_TEXT  A text symbol.  This symbol's value is a text ad-
                          dress and the link editor will update it when it
                          merges binary files.

                  N_DATA  A data symbol; similar to N_TEXT but for data ad-
                          dresses.  The values for text and data symbols are
                          not file offsets but addresses; to recover the file
                          offsets, it is necessary to identify the loaded ad-
                          dress of the beginning of the corresponding section
                          and subtract it, then add the offset of the section.

                  N_BSS   A bss symbol; like text or data symbols but has no
                          corresponding offset in the binary file.

                  N_FN    A filename symbol.  The link editor inserts this
                          symbol before the other symbols from a binary file
                          when merging binary files.  The name of the symbol
                          is the filename given to the link editor, and its
                          value is the first text address from that binary
                          file.  Filename symbols are not needed for link-
                          editing or loading, but are useful for debuggers.

                  The N_STAB mask selects bits of interest to symbolic debug-
                  gers such as gdb(1);  the values are described in stab(5).

                  symbols, this is an address; for other symbols (such as de-
                  bugger symbols), the value may be arbitrary.

     The string table consists of an unsigned long length followed by null-
     terminated symbol strings.  The length represents the size of the entire
     table in bytes, so its minimum value (or the offset of the first string)
     is always 4 on 32-bit machines.



SEE ALSO
     ld(1),  execve(2),  nlist(3),  core(5),  dbx(5),  stab(5)



HISTORY
     The a.out.h include file appeared in Version 7 AT&amp;T UNIX.



BUGS
     Since not all of the supported architectures use the a_mid field, it can
     be difficult to determine what architecture a binary will execute on
     without examining its actual machine code.  Even with a machine identifi-
     er, the byte order of the exec header is machine-dependent.

     Nobody seems to agree on what bss stands for.

     New binary file formats may be supported in the future, and they probably
     will not be compatible at any level with this ancient format.

BSDI BSD/OS                      June 5, 1993                                5


*/

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
	int				r_address;
	unsigned long	r_info;
};

/* Fields within r_info: */
#define SYMNUM_MSK	0xffffff00
#define PCREL_MSK	0x00000080
#define LENGTH_MSK	0x00000060
#define EXTERN_MSK	0x00000010
