/* vectors.S:
 */
#include "config.h"

	.extern _event
	.extern _regtbl
	.extern _evt_gen
	.global _evt0
	.global _evt1
	.global _evt2
	.global _evt3
	.global _evt4
	.global _evt5
	.global _evt6
	.global _evt7
	.global _evt8
	.global _evt9
	.global _evt10
	.global _evt11
	.global _evt12
	.global _evt13
	.global _evt14
	.global _evt15

	.balign	16
_evt0:
	[--sp] = r0			/* Store away r0 & p4 on stack so that they */
	[--sp] = p4			/* can immediately be used by this handler. */
	r0 = 0;				/* Load r0 with this handler number and jump to */
	jump	_evt_gen	/* generic part of the handler. */

	.balign	16
_evt1:
	[--sp] = r0
	[--sp] = p4
	r0 = 1
	jump	_evt_gen

	.balign	16
_evt2:
	[--sp] = r0
	[--sp] = p4
	r0 = 2
	jump	_evt_gen

	.balign	16
_evt3:
	[--sp] = r0
	[--sp] = p4
	r0 = 3
	jump	_evt_gen

	.balign	16
_evt4:
	[--sp] = r0
	[--sp] = p4
	r0 = 4
	jump	_evt_gen

	.balign	16
_evt5:
	[--sp] = r0
	[--sp] = p4
	r0 = 5
	jump	_evt_gen

	.balign	16
_evt6:
	[--sp] = r0
	[--sp] = p4
	r0 = 6
	jump	_evt_gen

	.balign	16
_evt7:
	[--sp] = r0
	[--sp] = p4
	r0 = 7
	jump	_evt_gen

	.balign	16
_evt8:
	[--sp] = r0
	[--sp] = p4
	r0 = 8
	jump	_evt_gen

	.balign	16
_evt9:
	[--sp] = r0
	[--sp] = p4
	r0 = 9
	jump	_evt_gen

	.balign	16
_evt10:
	[--sp] = r0
	[--sp] = p4
	r0 = 10
	jump	_evt_gen

	.balign	16
_evt11:
	[--sp] = r0
	[--sp] = p4
	r0 = 11
	jump	_evt_gen

	.balign	16
_evt12:
	[--sp] = r0
	[--sp] = p4
	r0 = 12
	jump	_evt_gen

	.balign	16
_evt13:
	[--sp] = r0
	[--sp] = p4
	r0 = 13
	jump	_evt_gen

	.balign	16
_evt14:
	[--sp] = r0
	[--sp] = p4
	r0 = 14
	jump	_evt_gen

	.balign	16
_evt15:
	[--sp] = r0
	[--sp] = p4
	r0 = 15
	jump	_evt_gen

	.balign	16
_evt_gen:
	p4.h = _regtbl		/* Set p4 to point to the C-level register table. */
	p4.l = _regtbl
	[p4] = sp
	[p4+4] = fp
	[p4+8] = p5			/* After pushing P5 to regtbl, use P5 to put the */
	p5 = [sp++]			/* original value of P4 (on stack) to regtbl[]. */
	[p4+12] = p5		/* Then push all remaining registers to regtbl[]. */
	[p4+16] = p3
	[p4+20] = p2
	[p4+24] = p1
	[p4+28] = p0
	[p4+32] = r7
	[p4+36] = r6
	[p4+40] = r5
	[p4+44] = r4
	[p4+48] = r3
	[p4+52] = r2
	[p4+56] = r1
	p5 = [sp++]			/* Pull R0 off stack and put it in regtbl[]. */ 
	[p4+60] = p5
	p5 = i3
	[p4+64] = p5
	p5 = i2
	[p4+68] = p5
	p5 = i1
	[p4+72] = p5
	p5 = i0
	[p4+76] = p5
	p5 = l3
	[p4+80] = p5
	p5 = l2
	[p4+84] = p5
	p5 = l1
	[p4+88] = p5
	p5 = l0
	[p4+92] = p5
	p5 = b3
	[p4+96] = p5
	p5 = b2
	[p4+100] = p5
	p5 = b1
	[p4+104] = p5
	p5 = b0
	[p4+108] = p5
	p5 = m3
	[p4+112] = p5
	p5 = m2
	[p4+116] = p5
	p5 = m1
	[p4+120] = p5
	p5 = m0
	[p4+124] = p5
	p5 = lc0
	[p4+128] = p5
	p5 = lc1
	[p4+132] = p5
	p5 = astat
	[p4+136] = p5
	p5 = seqstat
	[p4+140] = p5
	jump.l _event;		/* Jump to C-level event handler (R0 has vnum). */
