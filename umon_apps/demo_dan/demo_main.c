/**********************************************************************\
 Library    :  uMON (Micro Monitor)
 Filename   :  demo_main.c
 Purpose    :  DAN demo hello-world small application
 Owner		:  Sergey Krasnitsky
 Created    :  8.11.2010
\**********************************************************************/


int putchar (char c)
{
	#if 0 // UMON_TARGET == UMON_TARGET_XT_SIM

	register int a2		__asm__ ("a2")	= 4;			// SYS_write, from simcall.h
	register int a3		__asm__ ("a3")	= 1;			// TEN_IO_STDOUT
	register char *a4	__asm__ ("a4")	= &c;			// 1 data byte
	register int a5		__asm__ ("a5")	= 1;			// 1 byte len
	register int retval __asm__ ("a2");
	register int reterr __asm__ ("a3");
	__asm__ ("simcall"
		: "=a" (retval), "=a" (reterr)
		: "a" (a2), "a" (a3), "a" (a4), "a" (a5));
	
	#else // UMON_TARGET == UMON_TARGET_XT_JTAG

	register int a2		__asm__ ("a2")	= -5;			// GDBIO_TARGET_SYSCALL_WRITE
	register char *a3	__asm__ ("a3")	= &c;			// 1 data byte
	register int a4		__asm__ ("a4")	= 1;			// 1 byte len
	register int a6		__asm__ ("a6")	= 1;			// TEN_IO_STDOUT
	register int retval __asm__ ("a2");
	register int reterr __asm__ ("a3");
	__asm__ ("break 1,14"
		: "=a" (retval), "=a" (reterr)
		: "a" (a2), "a" (a3), "a" (a4), "a" (a6));
	#endif

    return c;
}

int main(int argc,char *argv[])
{
	putchar ('H');
	putchar ('e');
	putchar ('l');
	putchar ('l');
	putchar ('o');
	putchar ('\n');

	while(1);
	return 0;
}

