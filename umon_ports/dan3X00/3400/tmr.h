/*
All files except if stated otherwise in the begining of the file are under the GPLv2 license:
-----------------------------------------------------------------------------------

Copyright (c) 2010-2012 Design Art Networks Ltd.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

-----------------------------------------------------------------------------------
*/
/*-----------------------------------------------------------
 * TMR.c
 *
 * This file implements the HW timer driver.
 *
 * Design Art Networks Copyright (c) 2012
 *
 * OWNER:                     Igor
 *
 *-----------------------------------------------------------
 */

#ifndef _TMR_H
#define _TMR_H

/*
 * -----------------------------------------------------------
 * Include section
 * -----------------------------------------------------------
 */

#include "config.h"
#include "stddefs.h"

/*
 * -----------------------------------------------------------
 * MACRO (define) section
 * -----------------------------------------------------------
 */



/*
 * -----------------------------------------------------------
 * Type definition section
 * -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------
 * Global prototypes section
 * -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------
 * Local prototypes section
 * -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------
 * Global data section
 * -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------
 * Local (static) data section
 * -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------
 * Local (static) and inline functions section
 * -----------------------------------------------------------
 */


/*
 * -----------------------------------------------------------
 * Global functions section
 * -----------------------------------------------------------
 */

void TMR_init(void);

uint32 TMR_get_clk(void); 

void TMR_tick_delay(uint32 tick);  

void TMR_usec_delay(uint32 usec);

#endif /* _TMR_H */
