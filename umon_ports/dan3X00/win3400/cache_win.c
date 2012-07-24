#include "genlib.h"

void cacheInitForTarget()
{
	dcacheFlush      = 0;
	icacheInvalidate = 0;
}

