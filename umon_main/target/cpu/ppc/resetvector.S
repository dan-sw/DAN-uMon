/* resetvector.s:
 *
 * The PowerPC 403GCX (and others) resets at the top of its 32-bit
 * physical memory space at address 0xfffffffc.
 * This means that the monitor must be built with a branch instruction
 * at that location that will take it to the coldstart point of the
 * monitor executable.
 *
 * To allow the starting point of the monitor to grow downward, but still
 * not affect the location of the moncomptr, we also put moncomptr here.
 * The end result is that moncomptr will be at 0xfffffff0 and resetvector
 * will be at 0xfffffffc.
 *
 * General notice:
 * This code is part of a boot-monitor package developed as a generic base
 * platform for embedded system designs.  As such, it is likely to be
 * distributed to various projects beyond the control of the original
 * author.  Please notify the author of any enhancements made or bugs found
 * so that all may benefit from the changes.  In addition, notification back
 * to the author will allow the new user to pick up changes that may have
 * been made by other users after this version of the code was distributed.
 *
 * Author:	Ed Sutter
 * email:	esutter@lucent.com
 * phone:	908-582-2351
 *
 */
	.file "resetvector.s"
	.globl	resetvector
	.globl	moncomptr
	.extern coldstart
	.extern moncom

moncomptr:
	.long	moncom
	.long	0x11223344
	.long	0x55667788

resetvector:
	ba reset
