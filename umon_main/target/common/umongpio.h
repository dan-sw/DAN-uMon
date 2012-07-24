/*
 * gpio.h
 * A common way of looking at general purpose IO.
 *
 * This header is included by all code that uses uMon's general purpose
 * GPIO interface mechanism.  The idea is as follows:
 * Each bit has two values associated with it: 
 *
 * bit:
 *	The bit number within the register it is assigned (starting from 0).
 *
 * vbit:
 *  The bit number within a device which may have multiple registers of GPIO.
 *  For example, if a device has three 32-bit registers for GPIO, then there
 *  would be a total of 96 port numbers where 0-31 represent the bits of
 *  register 0, 32-63 represetnt the bits of register 1 and 64
 * 
 * For each bit, the GPIO(vbit,bit) macro uses fields within a 32-bit value to 
 * define this information; hence, one 32-bit value contains both the register
 * and device specific bit position.
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
 * Note: the majority of this code was edited with 4-space tabs.
 *
 * This is a derivative of work originally written by Mike Kelly from
 * Cogent Computer Systems.
 *
 */
#ifndef _UMONGPIO_H_
#define _UMONGPIO_H_

#define GPIO_BITNUM_MASK	0x000000ff
#define GPIO_VBITNUM_MASK	0xffff0000
#define GPIO(vbit,bit)		(((vbit << 16) & GPIO_VBITNUM_MASK) | \
							 (bit & GPIO_BITNUM_MASK))
#define GPIOVBIT(val)		((val & GPIO_VBITNUM_MASK) >> 16)
#define GPIOBIT(val)		(1 << (val & GPIO_BITNUM_MASK))

extern void GPIO_init(void);

/* Each of these functions uses the "virtual" bit number...
 */
extern int GPIO_set(int vbit);
extern int GPIO_clr(int vbit);
extern int GPIO_tst(int vbit);
extern int GPIO_in(int  vbit);
extern int GPIO_out(int vbit);

#endif
