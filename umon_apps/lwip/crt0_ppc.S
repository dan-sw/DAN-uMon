#include "cfg.h"
#define	sp   1
#define	r0   0
#define	r7   7

    .extern	AppStack
    .extern	Cstart
    .globl  start

#if USE_APPSTACK
start:
    lis     sp, (AppStack+(APPSTACKSIZE-4))@h
    addi    sp, sp, (AppStack+(APPSTACKSIZE-4))@l
    addi    r7, r0, -8
    and     sp, sp, r7		
	ba		Cstart
	nop
#endif
