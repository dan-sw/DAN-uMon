#include "cfg.h"
    
    .extern	AppStack
	.extern	Cstart
    .global start

    .text

#if USE_APPSTACK
start:
    move.l	#AppStack+(APPSTACKSIZE-32),%sp
    jsr		Cstart
#endif
