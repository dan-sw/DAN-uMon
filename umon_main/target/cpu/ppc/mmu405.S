#include "config.h"
#include "arch_ppc.h"

/* I found that some versions of GCC didn't support the
 * tlbre instruction for some reason, so instead of
 * using "tlbre	r3,r3,1" and "tlbre r3,r3,0", use the following
 * macros...
 */
#define TLBRE_R3_R3_1()	.long 	0x7c630f64
#define TLBRE_R3_R3_0()	.long 	0x7c630764

/*
 * TLB Access Functions 
 */

	.text
	.align	2
	.globl	ppctlbia
ppctlbia:
	tlbia
	sync
	blr
	.type  ppctlbia,@function
	.size  ppctlbia,.-ppctlbia

	.text
	.align	2
	.globl	ppcMftlblo
ppcMftlblo:
#if 0
	tlbre	r3,r3,1
#else
	TLBRE_R3_R3_1()
#endif
	sync
	blr
	.type  ppcMftlblo,@function
	.size  ppcMftlblo,.-ppcMftlblo

	.text
	.align	2
	.globl	ppcMftlbhi
ppcMftlbhi:
#if 0
	tlbre	r3,r3,0
#else
	TLBRE_R3_R3_0()
#endif
	sync
	blr
	.type  ppcMftlbhi,@function
	.size  ppcMftlbhi,.-ppcMftlbhi

	.text
	.align	2
	.globl	ppcMttlblo
ppcMttlblo:
	tlbwe	r4,r3,1
	sync
	blr
	.type  ppcMttlblo,@function
	.size  ppcMttlblo,.-ppcMttlblo

	.text
	.align	2
	.globl	ppcMttlbhi
ppcMttlbhi:
	tlbwe	r4,r3,0
	sync
	blr
	.type  ppcMttlbhi,@function
	.size  ppcMttlbhi,.-ppcMttlbhi

	.text
	.align	2
	.globl	ppcMfpid
ppcMfpid:
	mfspr	r3,945
	blr
	.type  ppcMfpid,@function
	.size  ppcMfpid,.-ppcMfpid

	.text
	.align	2
	.globl	ppcMtpid
ppcMtpid:
	mtspr	945,r3
	blr
	.type  ppcMtpid,@function
	.size  ppcMtpid,.-ppcMtpid

	.text
	.align	2
	.globl	ppcMfzpr
ppcMfzpr:
	mfspr	r3,944
	blr
	.type  ppcMfzpr,@function
	.size  ppcMfzpr,.-ppcMfzpr

	.text
	.align	2
	.globl	ppcMtzpr
ppcMtzpr:
	mtspr	944,r3
	blr
	.type  ppcMtzpr,@function
	.size  ppcMtzpr,.-ppcMtzpr
