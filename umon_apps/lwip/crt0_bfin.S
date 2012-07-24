#include "cfg.h"
    
    .extern	_AppStack
	.extern	_Cstart
    .global start
    .global _clock_time

    .text
#if USE_APPSTACK
start:
    sp.h = _AppStack+(APPSTACKSIZE-32)
    sp.l = _AppStack+(APPSTACKSIZE-32)
    jump	_Cstart
#endif

_clock_time:
	R0 = CYCLES;
	RTS
