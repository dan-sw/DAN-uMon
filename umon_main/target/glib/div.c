#include "config.h"
#include <stdlib.h>

div_t
div(int num,int den)
{
	div_t rslt;

	rslt.quot = num/den;
	rslt.rem = num % den;
	return(rslt);
}
