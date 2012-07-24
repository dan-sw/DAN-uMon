#include "config.h"
#include "ctype.h"
#include "genlib.h"
#include "stddefs.h"

/* lceil():
 * returns the nearest integral multiple of 'to' not smaller than 'n'.
 */
long lceil(long n, long to)
{
  n = (n + to / 2) / to;
  return n * to;
}
