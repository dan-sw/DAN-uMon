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
