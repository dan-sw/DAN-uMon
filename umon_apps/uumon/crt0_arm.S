#include "config.h"

    .extern MonStack
	.extern main
	.global	start

    /* Set stack pointer to end of MonStack and jump to main...
	 */
start:
    ldr sp, =(MonStack + MONSTACKSIZE - 4)

jump_to_c:
    bl main
