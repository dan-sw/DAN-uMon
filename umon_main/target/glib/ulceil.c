#include "config.h"
#include "ctype.h"
#include "genlib.h"
#include "stddefs.h"

/* lceil():
 * returns the nearest integral multiple of 'to' not smaller than 'n'.
 */
unsigned long ulceil(unsigned long n, unsigned long to)
{
  unsigned long adj = n % to;
  if (adj) return n + (to - adj);
  return n;
}
