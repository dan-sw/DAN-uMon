#include "config.h"
#include "ctype.h"
#include "genlib.h"
#include "stddefs.h"

int
abs(int arg)
{
	return (arg >= 0 ? arg : -arg);
}
