#include "config.h"
#include "stddefs.h"
#include "genlib.h"

void set_version()
{
	char *buf = BUILD_ID;
	setenv("BUILD_ID", buf);
	printf("\t\t\t\tBuild id: %s\n", buf);
}
