/*
 * at91sam9263_gpio.c
 *
 * Generic API set for AT91SAM9263 CPU GPIO.
 * Adapted from code originally written by Cogent Computer Systems, Inc.
 */

#include "stddefs.h"
#include "umongpio.h"
#include "at91sam9263.h"
#include "at91sam9263_gpio.h"

/*
 * GPIO_set():
 *  Set the bit that corresponds to the incoming virtual (i.e. device
 *  specific) bit number.
 *
 *  NOTE: We do not test to see if setting the bit would screw up any
 *  alternate functions.  Use this function with caution!
 */

int
GPIO_set(int bit)
{
	int gpio_bit;

	// The incoming value is the GPIO() macro as defined in umongpio.h, so
	// we start by converting it to the virtual bit number...
	gpio_bit = GPIOVBIT(bit);

	// quick sanity test
	if (gpio_bit > 160) return -1;

	if (gpio_bit < 32)
	{
		// Port A
		PIOA_REG(PIO_SODR) = (1 << (gpio_bit - 0));
	}
	else if (gpio_bit < 64)
	{
		// Port B
		PIOB_REG(PIO_SODR) = (1 << (gpio_bit - 32));
	}
	else if (gpio_bit < 96)
	{
		// Port C
		PIOC_REG(PIO_SODR) = (1 << (gpio_bit - 64));
	}
	else if (gpio_bit < 128)
	{
		// Port D
		PIOD_REG(PIO_SODR) = (1 << (gpio_bit - 96));
	}
	else
	{
		// Port E
		PIOE_REG(PIO_SODR) = (1 << (gpio_bit - 128));
	}
	return 0;
}

/*
 * GPIO_clr():
 *  Clear the bit that corresponds to the incoming virtual (i.e. device
 *  specific) bit number.
 */

int
GPIO_clr(int bit)
{
	int gpio_bit;

	// The incoming value is the GPIO() macro as defined in umongpio.h, so
	// we start by converting it to the virtual bit number...
	gpio_bit = GPIOVBIT(bit);

	// quick sanity test
	if (gpio_bit > 160) return -1;

	if (gpio_bit < 32)
	{
		// Port A
		PIOA_REG(PIO_CODR) = (1 << (gpio_bit - 0));
	}
	else if (gpio_bit < 64)
	{
		// Port B
		PIOB_REG(PIO_CODR) = (1 << (gpio_bit - 32));
	}
	else if (gpio_bit < 96)
	{
		// Port C
		PIOC_REG(PIO_CODR) = (1 << (gpio_bit - 64));
	}
	else if (gpio_bit < 128)
	{
		// Port D
		PIOD_REG(PIO_CODR) = (1 << (gpio_bit - 96));
	}
	else
	{
		// Port E
		PIOE_REG(PIO_CODR) = (1 << (gpio_bit - 128));
	}
	return 0;
}

/*
 * GPIO_tst():
 *  Return the state of the bit that corresponds to the incoming
 *  virtual (i.e. device specific) bit number.
 *
 *  It does not test to see if it's an input or output and thus can
 *  be used to verify if an output set/clr has taken place as well as
 *  for testing an input state.
 */

int
GPIO_tst(int bit)
{
	int gpio_bit;

	// The incoming value is the GPIO() macro as defined in umongpio.h, so
	// we start by converting it to the virtual bit number...
	gpio_bit = GPIOVBIT(bit);

	if (gpio_bit < 32)
	{
		// Port A
		if (PIOA_REG(PIO_PDSR) & (1 << (gpio_bit - 0))) return 1;
	}
	else if (gpio_bit < 64)
	{
		// Port B
		if (PIOB_REG(PIO_PDSR) & (1 << (gpio_bit - 32))) return 1;
	}
	else if (gpio_bit < 96)
	{
		// Port C
		if (PIOC_REG(PIO_PDSR) & (1 << (gpio_bit - 64))) return 1;
	}
	else if (gpio_bit < 128)
	{
		// Port D
		if (PIOD_REG(PIO_PDSR) & (1 << (gpio_bit - 96))) return 1;
	}
	else
	{
		// Port E
		if (PIOE_REG(PIO_PDSR) & (1 << (gpio_bit - 128))) return 1;
	}
	return 0; // bit was not set
}

/*
 * GPIO_in():
 *  Configure the direction of the bit that corresponds to the incoming
 *  virtual (i.e. device specific) bit number to input.
 *
 *  NOTE: We do not test to see if changing the direction of the bit
 *  would screw up anything.  Use this function with caution!
 */
int
GPIO_in(int bit)
{
	int gpio_bit;

	// The incoming value is the GPIO() macro as defined in umongpio.h, so
	// we start by converting it to the virtual bit number...
	gpio_bit = GPIOVBIT(bit);

	// quick sanity test
	if (gpio_bit > 160) return -1;

	if (gpio_bit < 32)
	{
		// Port A
		PIOA_REG(PIO_ODR) = (1 << (gpio_bit - 0));
	}
	else if (gpio_bit < 64)
	{
		// Port B
		PIOB_REG(PIO_ODR) = (1 << (gpio_bit - 32));
	}
	else if (gpio_bit < 96)
	{
		// Port C
		PIOC_REG(PIO_ODR) = (1 << (gpio_bit - 64));
	}
	else if (gpio_bit < 128)
	{
		// Port D
		PIOD_REG(PIO_ODR) = (1 << (gpio_bit - 96));
	}
	else
	{
		// Port E
		PIOE_REG(PIO_ODR) = (1 << (gpio_bit - 128));
	}
	return 0;
}

/*
 * GPIO_out():
 *  Configure the direction of the bit that corresponds to the incoming
 *  virtual (i.e. device specific) bit number to output.
 *
 *  NOTE: We do not test to see if changing the direction of the bit
 *  would screw up anything.  Use this function with caution!
 */
int
GPIO_out(int bit)
{
	int gpio_bit;

	// The incoming value is the GPIO() macro as defined in umongpio.h, so
	// we start by converting it to the virtual bit number...
	gpio_bit = GPIOVBIT(bit);

	// quick sanity test
	if (gpio_bit > 160) return -1;

	if (gpio_bit < 32)
	{
		// Port A
		PIOA_REG(PIO_PER) = (1 << (gpio_bit - 0));
		PIOA_REG(PIO_OER) = (1 << (gpio_bit - 0));
	}
	else if (gpio_bit < 64)
	{
		// Port B
		PIOB_REG(PIO_PER) = (1 << (gpio_bit - 32));
		PIOB_REG(PIO_OER) = (1 << (gpio_bit - 32));
	}
	else if (gpio_bit < 96)
	{
		// Port C
		PIOC_REG(PIO_PER) = (1 << (gpio_bit - 64));
		PIOC_REG(PIO_OER) = (1 << (gpio_bit - 64));
	}
	else if (gpio_bit < 128)
	{
		// Port D
		PIOD_REG(PIO_PER) = (1 << (gpio_bit - 96));
		PIOD_REG(PIO_OER) = (1 << (gpio_bit - 96));
	}
	else
	{
		// Port E
		PIOE_REG(PIO_PER) = (1 << (gpio_bit - 128));
		PIOE_REG(PIO_OER) = (1 << (gpio_bit - 128));
	}
	return 0;
}

