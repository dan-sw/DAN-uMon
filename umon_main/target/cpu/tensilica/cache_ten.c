#include <xtensa/config/core.h>

#include "genlib.h"


typedef int (*UMON_CACH_FUNC) (char*,int);


void cacheInitForTarget()
{
	dcacheFlush      = (UMON_CACH_FUNC) xthal_dcache_region_writeback;
	icacheInvalidate = (UMON_CACH_FUNC) xthal_icache_region_invalidate;
}

