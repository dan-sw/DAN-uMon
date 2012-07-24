/* sbrk.c:
 *	Used by malloc to get memory from "somewhere".
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
#include "config.h"
#if INCLUDE_MALLOC
#include "genlib.h"
#define NULL 0

extern	int releaseExtendedHeap(int);

static long allocbuf[ALLOCSIZE/sizeof(long)];
static char *allocp1;
static char *ext_heapbase, *ext_heapspace, *ext_heapend;

/* GetMemory():
 *	This function is called by the guts of malloc when it needs to expand
 *	the size of the heap.  Initially, GetMemory() will allocate memory
 *	from a static array (allocbuf[]) that is allocated memory space when the 
 *	monitor is built.   If the variable ext_heapbase is non-zero
 *	at the point when GetMemory() runs out of space in allocbuf[], it
 *	will start allocating memory from the block pointed to by ext_heapspase
 *	and ext_heapsize.
 *	WARNING: this feature can only be used if the malloc()/free() code
 *	can handle the fact that memory within its heap will be different 
 *	blocks of non-contiguous space.
*/
char *
GetMemory(int n)
{
	if (!allocp1)
		allocp1 = (char *)allocbuf;

	/* First try to allocate from allocbuf[]... */
	if (allocp1 + n <= (char *)allocbuf + ALLOCSIZE) {
		allocp1 += n;
		return (allocp1 - n);
	}
	/* Else try to allocated from the extended heap (if one)... */
	else if (ext_heapbase) {
		if (ext_heapspace + n <= ext_heapend) {
			ext_heapspace += n;
			return(ext_heapspace - n);
		}
		else {
			return(NULL);
		}
	}
	/* Else, no space left to allocate from. */
	else {
		return (NULL);
	}
}

/* ExtendHeap():
 *	Called by the heap command to provide GetMemory() with more space.
 *	This function can be called through the monitor API.
 */
int
extendHeap(char *start, int size)
{
	/* If the size is -1, then assume this is a release request. */
	if (size == -1)
		return(releaseExtendedHeap(0));

	/* If extension is already loaded, then return -1 for failure. */
	if (ext_heapbase)
		return(-1);

	if (inUmonBssSpace(start,start+size-1))
		return(-2);

	ext_heapbase = ext_heapspace = start;
	ext_heapend = start + size;
	return(0);
}

/* UnextendHeap():
 *	Called by the heap command to "undo" the memory extension.
 */
void
unExtendHeap(void)
{
	ext_heapbase = ext_heapspace = ext_heapend = 0;
}

char *
getExtHeapBase(void)
{
	return(ext_heapbase);
}

/* GetMemoryLeft():
 *	Return the amount of memory that has yet to be allocated from 
 *	the static and extended heap (if one).
*/
int
GetMemoryLeft(void)
{
	int		spaceleft;

	if (!allocp1)
		allocp1 = (char *)allocbuf;

	spaceleft = ((char *)allocbuf + ALLOCSIZE) - allocp1;

	if (ext_heapbase)
		spaceleft += (ext_heapend - ext_heapspace);

	return(spaceleft);
}

#else

int
extendHeap(char *start, int size)
{
	return(-1);
}

#endif
