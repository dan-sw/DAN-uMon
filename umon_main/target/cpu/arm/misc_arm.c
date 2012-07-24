
/*
 * Set the Current Program Status Register.
 * old way assumed argument was in R0:
 *  asm("   msr CPSR_c, r0");
 */
void
putpsr(unsigned long psr)
{
	volatile unsigned register reg;

	reg = psr;
	asm volatile ("msr CPSR_c, %0" : "=r" (reg)); 
}

/*
 * Return the Current Program Status Register.
 * old way assumed return in R0:
 *  asm("   mrs r0, CPSR");
 */
unsigned long
getpsr()
{
	volatile unsigned register reg;
	asm volatile ("mrs %0, CPSR" : "=r" (reg)); 
	return(reg);
}

/* getsp():
 * Return the current stack pointer.
 *  oldway: asm("   mov r0, r13");
 */
unsigned long
getsp()
{
	volatile unsigned register reg;
	asm volatile ("mov %0, r13" : "=r" (reg)); 
	return(reg);
}
