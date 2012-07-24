/* cache_sh3.c:
 *	SH3 specific cache setting functions.
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
 *	Author:	Torsten Tetz, Bosch Security Systems
 */
#include "genlib.h"
#include "cpuio.h"

/* Prototypes */
void enableCache(int CacheSize);
int flushCache(char *addr,int size);
int disableCache(void);
void cacheInitForTarget(void);

/* Function pointers used by the common code. */
int	(*dcacheFlush)(char *,int), (*icacheInvalidate)(char *,int);
int	(*dcacheDisable)(void), (*icacheDisable)(void);

/* cacheInitForTarget():
 * Called from init2() in "..target/common/start.c".
 * Set pointers to use flash functions here and enable the cache.
 */
void
cacheInitForTarget(void)
{
	dcacheDisable = (int(*)(void)) disableCache();
	icacheDisable = (int(*)(void)) disableCache();
	dcacheFlush = (int(*)(char *,int)) flushCache;
	icacheInvalidate = (int(*)(char *,int)) flushCache;

	/* Enable the cache in 16-kbyte mode. */
	enableCache(16);
}

/* enableCache():
 * Set cache size to 32-kbyte or 16-kbyte mode, flush complete cache, 
 * set cache to write-through mode and enable the cache.
 *
 * The "write-through" mode is used instead of "write-back" mode, because 
 * it makes it simpler to implement the "flushCache()" function. 
 * In "write-through" mode writing always happens immediately 
 * to the real memory and the values are cached for faster reading.
 * In "write-back" mode values are only put into the cache and 
 * written to the real memory when the cache line is needed for something
 * else. This would require to ensure that all cache lines have been 
 * written back to memory before calling "flushCache()",
 * or all write operations that are currently being cached are lost.
 */
void
enableCache(int CacheSize)
{
	if(CacheSize == 32) {
		ctrl_bit_set_outl(CCR3_CSIZE_32,CCR3);	/* Set cache size to 32-kbyte. */
	}
	else {
		ctrl_bit_set_outl(CCR3_CSIZE_16,CCR3);	/* Set cache size to 16-kbyte by default. */
	}

	flushCache(0,0);			/* Flush cache. */
	ctrl_bit_set_outl(CCR1_WT,CCR1);	/* Set cache to write-trough mode. */
	ctrl_bit_set_outl(CCR1_CE,CCR1);	/* Enable cache. */
}

/* flushCache():
 * Flush the address range specified, or if address and size are both
 * zero, flush the entire D-Cache.
 * This implementation always flushes the complete cache.
 */
int
flushCache(char *addr,int size)
{
	/* Writing 1 to the "Cache Flush" bit flushes all cache entries. */
	ctrl_bit_set_outl(CCR1_CF,CCR1);
	return(1);
}

/* disableCache():
 */
int
disableCache(void)
{
	ctrl_bit_clear_outl(CCR1_CE, CCR1);
	return(1);
}

