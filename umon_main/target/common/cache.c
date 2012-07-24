/* cache.c:
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
 *	This code is the front-end to CPU-specific code that may or may not
 *	support basic I & D cache operations.  Various facilities in the
 *  monitor call the wrapper functions below for carrying out cpu-specific
 *  cache operations.  This code provides that wrapper allowing the other
 *	portions of the monitor to be unaware of what the specific CPU actually
 *	supports.
 *
 *	The cacheInit() function must be called at monitor startup, then
 *	cacheInit() calls cacheInitForTarget(), a function that must be provided
 *	by the target-specific code to fill in the CPU-specific functionality.
 *	This function should establish the cache as it is to be used in
 * 	the monitor (typically i-cache enabled, d-cache disabled), plus, if
 *	applicable, the dcacheFlush and icacheInvalidate function pointers should
 *	be initialized to CPU-specific functions that match the API...
 *	
 *		int	(*dcacheDisable)(void);
 *		int (*icacheDisable)(void);
 *		int	(*dcacheFlush)(char *base,int size);
 *		int (*icacheInvalidate)(char *base, int size);
 *
 *	Original author:	Ed Sutter
 *	Email:				esutter@lucent.com
 *	Phone:				908-582-2351
 */
#include "genlib.h"
#include "cache.h"

int	(*dcacheFlush)(char *,int), (*icacheInvalidate)(char *, int);
int	(*dcacheDisable)(void), (*icacheDisable)(void);

/* flushDcache():
 * Flush the address range specified, or if address and size are both
 * zero, flush the entire D-Cache.
 */
int
flushDcache(char *addr, int size)
{
	/* The dcacheFlush function pointer should be initialized in the
	 * port-specific function "cacheInitForTarget".
	 */
	if (dcacheFlush)
		return(dcacheFlush(addr,size));
	return(0);
}

/* disableDcache():
 * Disable data cache.
 */
int
disableDcache(void)
{
	/* The dcacheDisable function pointer should be initialized in the
	 * port-specific function "cacheInitForTarget".
	 */
	if (dcacheDisable)
		return(dcacheDisable());
	return(0);
}

/* invalidateIcache():
 * Invalidate the address range specified, or if address and size are both
 * zero, invalidate the entire I-Cache.
 */
int
invalidateIcache(char *addr, int size)
{
	/* The icacheInvalidate function pointer should be initialized in the
	 * port-specific function "cacheInitForTarget".
	 */
	if (icacheInvalidate)
		return(icacheInvalidate(addr,size));
	return(0);
}

/* disableIcache():
 * Disable instruction cache.
 */
int
disableIcache(void)
{
	/* The icacheDisable function pointer should be initialized in the
	 * port-specific function "cacheInitForTarget".
	 */
	if (icacheDisable)
		return(icacheDisable());
	return(0);
}

int
cacheInit()
{
	dcacheDisable = (int(*)(void))0;
	icacheDisable = (int(*)(void))0;
	dcacheFlush = (int(*)(char *,int))0;
	icacheInvalidate = (int(*)(char *,int))0;
	cacheInitForTarget();			/* Target-specific initialization. */
	return(0);
}
