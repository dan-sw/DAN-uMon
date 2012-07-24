/***************************************************************************
 *
 * Blackfin Cycle Counting...
 *
 * Refer to cycles.h for more information.
 *
 ***************************************************************************
 */
	/* _bfin_cycles_lo:
	 * Retrieve the CYCLES register.
	 */
    .global _bfin_cycles_lo
_bfin_cycles_lo:
	R0 = CYCLES;
	RTS

	/* _bfin_cycles_his:
	 * Retrieve the CYCLES2 shadow register.
	 * This function assumes that _bfin_cycles_lo was called previously
	 * and the value to be read is the shadow of CYCLES2 at the time
	 * of that read.
	 */
    .global _bfin_cycles_his
_bfin_cycles_his:
	R0 = CYCLES2;
	RTS

	/* _bfin_cycles_hi:
	 * Retrieve the CYCLES2 register.
	 * Need to read CYCLES prior to CYCLES2 so that the shadow copy
	 * of CYCLES2 is updated (refer to programmer manual).
	 */
    .global _bfin_cycles_hi
_bfin_cycles_hi:
	R0 = CYCLES;
	R0 = CYCLES2;
	RTS

	/* _bfin_cycles_hi:
	 * Disable the cycle counter, then clear both CYCLES and CYCLES2
	 * to zero, then enable enable the counter.
	 */
	.global _bfin_cycles_init
_bfin_cycles_init:
	R2 = SYSCFG;
	BITCLR(R2,1);
	SYSCFG = R2;
	R2 = 0;
	CYCLES = R2;
	CYCLES2 = R2;
	R2 = SYSCFG;
	BITSET(R2,1);
	SYSCFG = R2;
	RTS
	
