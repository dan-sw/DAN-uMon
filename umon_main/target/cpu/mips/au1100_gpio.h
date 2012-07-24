//==========================================================================
//
// au1100_gpio.h
//
//
// Author(s):    Michael Kelly, Cogent Computer Systems, Inc.
// Contributors: 
// Date:         07/22/2002
// Description:  Include file for au1100 GPIO 1 and 2 on CSB351
//
//--------------------------------------------------------------------------

#define GPIO1_BASE_ADDRESS	0xb1900000
#define GPIO1(_x_)  		*(vulong *)(GPIO1_BASE_ADDRESS + _x_)
#define GPIO1_TRI 			0x0000 // GPIO1 Tristate (0 = tristates)
#define GPIO1_OUTPUT_SET	0x0108 // GPIO1 Data Output Set, 1 sets the corresponding GPIO1 output
#define GPIO1_OUTPUT_CLR	0x010c // GPIO1 Data Output Clear, 1 clears the corresponding GPIO1 output
#define GPIO1_PINSTATE 		0x0110 // GPIO1 Pin State - Read
#define GPIO1_INPUTEN		0x0110 // GPIO1 Input Enable, write a 0 to enable all GPIO1's as possible inputs

// pin function register covers bits from GPIO1 and GPIO2
#define GPIO_PINFUNC		0x002C // GPIO1 and GPIO2 Pin Function

#define GPIO2_BASE_ADDRESS	0xb1700000
#define GPIO2(_x_)  		*(vulong *)(GPIO2_BASE_ADDRESS + _x_)
#define GPIO2_DIR 			0x0000 // GPIO2 Direction			
#define GPIO2_OUTPUT 		0x0008 // GPIO2 Data Output
#define GPIO2_PINSTATE 		0x000C // GPIO2 Pin State
#define GPIO2_INTEN			0x0010 // GPIO2 Interrupt Enable
#define GPIO2_ENABLE		0x0014 // GPIO2 Enable

//--------------------------------------------------------------------------
//
// GPIO Port 1 and 2 bit assignments for the CSB351 are as follows.
// Active indicates what level is considered active for that
// signal, if any.  The numbers are used by the various GPIO
// functions
//
// 							GPIO	   Direction	Active	Description
//							0		// Input		Low		IRQ0 - Interrupt from I2C Expander
//							1		// Input		Low		IRQ1 - Battery Interrupt from WM9712
#define GPIO_USER_LED0		2		// Output		Low		User LED 0
#define GPIO_USER_LED1		3		// Output		Low		User LED 1
//							4		// Input		Low		IRQ2 - Pendown Interrupt from WM9712
//							5		// Input		Low		IRQ3 - AM79C874 PHY Interrupt
#define GPIO_VCC5_EN		6		// Output		High	IRQ4 - 5 Volt Enable
//							7		// Input		Low		IRQ5 - Unused
//							8		// Input		Low		IRQ6 - Unused
//							9-15	// Used for alternate I/O functions
#define GPIO_SCL			16		// I/O			N/A		I2C Bus Clock
#define GPIO_SDA			17		// I/O			N/A		I2C Bus Data
//							18-23	// Unused for now
//							24-31	// Used for alternate I/O functions



// GPIO1 pin functions
#define GPIO1_PF_PC			BIT18	// PCMCIA Control Strobes, 0 = PCMCIA control, 1 = GPIO204-207
#define GPIO1_PF_LC			BIT17	// LCD Control Strobes, 0 = LCD control, 1 = GPIO200-203
#define GPIO1_PF_CS			BIT16	// GPIO2 output clock select, 0 = EXTCLK0, 1 = 32Khz Clock
#define GPIO1_PF_USB 		BIT15	// USB Port 1 Functionality, 0 = USB Device, 1 = USB Host
#define GPIO1_PF_U3_TXD		BIT14 	// GPIO23 Select, 0 = UART3 TXD, 1 = GPIO
#define GPIO1_PF_SRC		BIT11	// GPI06 Select, 0 = GPIO, 1 = SMROMCKE
#define GPIO1_PF_EX1		BIT10	// GPI03 Select, 0 = GPIO, 1 = EXTCLK1
#define GPIO1_PF_EX0		BIT9	// GPI02 Select, 0 = GPIO, 1 = EXTCLK1/32Khz
#define GPIO1_PF_U3_CTL		BIT7	// GPIO9-14 Select, 0 = GPIO, 1 = UART3 Control (DSR, DTR, CTS, RTS)
#define GPIO1_PF_NI2		BIT4	// GPIO24-28 Select, 0 = Enet Port 2, 1 = GPIO
#define GPIO1_PF_U0_TXD3	BIT3	// GPIO20 Select, 0 = UART0 TXD, 1 = GPIO

#define GPIO2_EN_MR			BIT1	// module reset for GPIO2 block
#define GPIO2_EN_CE			BIT0	// clock enable for GPIO2 block

// macro to convert a GPIO number(200-215) to a bit position (0x0001)
#define GPIO_NUM2BIT(_x_) 	(1 << (_x_ - 200))

