#include "config.h"

	.global	start

#ifdef __ADSPLPBLACKFIN__
	.global	_target_timer
    .extern _MonStack
	.extern _main

start:
    /* Clear all DAG registers:
     */
    R0 = 0
    I0 = R0
    I1 = R0
    I2 = R0
    I3 = R0
    L0 = R0
    L1 = R0
    L2 = R0
    L3 = R0
    B0 = R0
    B1 = R0
    B2 = R0
    B3 = R0
    M0 = R0
    M1 = R0
    M2 = R0
    M3 = R0
    LC0 = R0;
    LC1 = R0;

    sp.h = _MonStack+(MONSTACKSIZE-4)
    sp.l = _MonStack+(MONSTACKSIZE-4)
    jump    _main

_target_timer:
	R0 = CYCLES;
	RTS

#else
    .extern MonStack
	.extern main

    /* Set stack pointer to end of MonStack and jump to main...
	 */
start:
    ldr sp, =(MonStack + MONSTACKSIZE - 4)

jump_to_c:
    bl main

#endif
