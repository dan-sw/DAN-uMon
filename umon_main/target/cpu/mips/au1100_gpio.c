//==========================================================================
//
// au1100_gpio.c
//
//
// Author(s):    Michael Kelly, Cogent Computer Systems, Inc.
// Contributors: 
// Date:         03/20/03
// Description:  This file contains code to intialize the Au1100 GPIO
//               section as well as set, clear and test functions
//               for manipulating the GPIO bits on the CSB351
//
//--------------------------------------------------------------------------

#include "config.h"
#include "cpuio.h"
#include "stddefs.h"
#include "genlib.h"
#include "bits.h"
#include "au1100_gpio.h"

// Function Prototypes
void GPIO_init(void);
int GPIO1_set(int);
int GPIO1_clr(int);
int GPIO1_tst(int);
int GPIO1_in(int);
int GPIO1_out(int);

//--------------------------------------------------------
// GPIO_init()
//
// This function sets the startup state for the Au1100
// GPIO Ports 1 and 2
//
void
GPIO_init()
{

	// GPIO1 init
	// for GPIO1 we use most of the bits for their peripheral function
	// not as GPIO.  Although umon does not use interrupts, we
	// still enable them as inputs here.

	// set the default alternate functions first
	GPIO1(GPIO_PINFUNC) = 0x00000200;		// enable BCLK on EXTCLK0

	GPIO1(GPIO1_TRI) = 0xffffffff;			// tristate all of GPIO 1

	GPIO1(GPIO1_INPUTEN) = 0;	  			// enable GPIO1 inputs


}

//--------------------------------------------------------
// GPIO1_set()
//
// This function sets the desired bit passed in.  It first 
// tests to see if the bit is an actual output.  If not,
// it returns a -1, otherwise it returns 0.
//

int GPIO1_set(int gpio_bit)
{
	// quick sanity test
	if (gpio_bit > 31) return -1;

	// if yes, then set it
	GPIO1(GPIO1_OUTPUT_SET) = (1 << gpio_bit);

	return 0;
}

//--------------------------------------------------------
// GPIO1_clr()
//
// This function clears the desired bit passed in.  It first 
// tests to see if the bit is an actual output.  If not,
// it returns a -1, otherwise it returns 0.  NOTE: We do not
// test to see if clearing the bit would screw up any alternate
// functions.  Use this function with caution!
//

int GPIO1_clr(int gpio_bit)
{
	// quick sanity test
	if (gpio_bit > 31) return -1;

	// if yes, then clear it
	GPIO1(GPIO1_OUTPUT_CLR) = (1 << gpio_bit);

	return 0;
}
//--------------------------------------------------------
// GPIO1_tst()
//
// This function returns the state of desired bit passed in.
// It does not test to see if it's an input or output and thus
// can be used to verify if an output set/clr has taken place
// as well as for testing an input state.
//

int GPIO1_tst(int gpio_bit)
{
	// quick sanity test
	if (gpio_bit > 31) return -1;

	// get the state
	if (GPIO1(GPIO1_PINSTATE) & (1 << gpio_bit)) return 1;

	return 0;

}

//--------------------------------------------------------
// GPIO_in()
//
// This function changes the direction of the desired bit 
// to input.  NOTE: We do not test to see if changing the
// direction of the bit would screw up anything.  Use this
// function with caution!
int GPIO1_in(int gpio_bit)
{
	// quick sanity test
	if (gpio_bit > 31) return -1;

	// clear the direction bit
	GPIO1(GPIO1_TRI) &= ~(1 << gpio_bit);

	return 0;
}

//--------------------------------------------------------
// GPIO1_out()
//
// This function changes the direction of the desired bit 
// to output.  NOTE: We do not test to see if changing the
// direction of the bit would screw up anything.  Use this
// function with caution!
int GPIO1_out(int gpio_bit)
{
	// quick sanity test
	if (gpio_bit > 31) return -1;

	// set the direction bit
	GPIO1(GPIO1_TRI) |= (1 << gpio_bit);

	return 0;
}

