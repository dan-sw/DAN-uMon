/* This file should be included in config.h if the monitor is
 * to be built with malloc-debug enabled.  The malloc-debug feature
 * simply adds the filename and file line number to the mhdr structure.
 *
 * This makes it easier to find a memory leak because the location of
 * the violating malloc call will be dumped by "heap -v".
 */
#ifndef _MALLOCDEBUG_H_
#define _MALLOCDEBUG_H_

#ifndef ASSEMBLY_ONLY

#define MALLOC_DEBUG
extern  char *malloc(int, char *, int);
extern  char *realloc(char *, int, char *, int);

#define malloc(a) malloc(a,__FILE__,__LINE__)
#define realloc(a,b) realloc(a,b,__FILE__,__LINE__)

#endif	/* ASSEMBLY_ONLY */

#endif
