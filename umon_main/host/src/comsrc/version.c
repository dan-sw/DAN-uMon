#include <stdio.h>
#include <stdlib.h>

void
showVersion(void)
{
	printf(" Built: %s @ %s with ",__DATE__,__TIME__);
#ifdef BUILD_WITH_VCC
	fprintf(stderr,"VCC\n");
#else
	fprintf(stderr,"GCC\n");
#endif
	exit(1);
}
