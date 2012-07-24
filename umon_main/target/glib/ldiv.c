#include "config.h"
#include <stdlib.h>

ldiv_t
ldiv(long num,long den)
{
	ldiv_t rslt;

	rslt.quot = num/den;
	rslt.rem = num % den;
	return(rslt);
}
