/* cache.h:
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
 *	Refer to cache.c for comments.
 *
 *	Original author:	Ed Sutter
 *	Email:				esutter@lucent.com
 *	Phone:				908-582-2351
 */
#ifndef _CACHE_H_
#define _CACHE_H_

/* dcacheFlush, icacheInvalidate, dcacheDisable & icacheDisable:
 * Function pointers that should be initialized by target-specific code
 * to point to target-specific functions that can be accessed by the
 * generic monitor source.
 */
extern int (*dcacheFlush)(char *,int), (*icacheInvalidate)(char *,int);
extern int (*dcacheDisable)(void), (*icacheDisable)(void);

/* flushDcache():
 * Wrapper function for the target-specific d-cache flushing operation
 * (if one is appropriate).  If addr and size are both zero, then flush
 * the entire D-cache.
 */
extern int flushDcache(char *addr, int size);

/* invalidateIcache():
 * Wrapper function for the target-specific i-cache invalidation operation
 * (if one is appropriate).  If addr and size are both zero, then invalidate
 * the entire I-cache.
 */
extern int invalidateIcache(char *addr, int size);

/* disableDcache() & disableIcache():
 * Wrapper functions to call target-specific cache disable functions
 * (if available).
 */
extern int disableDcache(void);
extern int disableIcache(void);

/* cacheInit():
 * Called at startup.  This function calls cacheInitForTarget() which 
 * establishes the cache configuration and initializes the above
 * function pointers (if applicable).
 */
extern int cacheInit(void);
#endif
