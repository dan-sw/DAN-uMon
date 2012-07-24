//==========================================================================
//
//      ps2_kbd.h
//
//      PS/2 Keyboard Controller definitions
//
// Author(s):    Michael Kelly, Cogent Computer Systems, Inc.
// Contributors: 
// Date:         02/25/2002
// Description:  This file contains defines, scan codes and other
//               miscellaneous stuff for the PS/2 keyboard controller
//
//--------------------------------------------------------------------------

#if INCLUDE_PS2KBD
//------------------------------------------------------------------------
// cpu specific code must define the following board specific macro
// Base address of controller on board - example address
// #define PS2_REG(_x_) *(vuchar *)(0xf0800000 + (_x_ * PS2_STEP))

// Register Offsets
#define PS2_DATA		  		0x00
#define PS2_CMD 				0x01
#define PS2_STAT				0x01

// 8542B Commands (Sent to the Command Port)
#define PS2_CMD_SET_BYTE		0x60	// Set the command byte
#define PS2_CMD_GET_BYTE		0x20	// Get the command byte
#define PS2_CMD_KBD_OBUFF		0xD2	// Write to HT8542 Keyboard Output Buffer
#define PS2_CMD_AUX_OBUFF		0xD3	// Write to HT8542 Mouse Output Buffer
#define PS2_CMD_AUX_WRITE		0xD4	// Write to Mouse Port
#define PS2_CMD_AUX_OFF			0xA7	// Disable Mouse Port
#define PS2_CMD_AUX_ON			0xA8	// Re-Enable Mouse Port
#define PS2_CMD_AUX_TEST		0xA9	// Test for the presence of a Mouse
#define PS2_CMD_DIAG			0xAA	// Start Diagnostics
#define PS2_CMD_KBD_TEST		0xAB	// Test for presence of a keyboard
#define PS2_CMD_KBD_OFF			0xAD	// Disable Keyboard Port
#define PS2_CMD_KBD_ON			0xAE	// Re-Enable Keyboard Port

// HT8542B command byte set by KBD_CMD_SET_BYTE and retrieved by KBD_CMD_GET_BYTE
#define PS2_CMD_BYTE_TRANS		0x40
#define PS2_CMD_BYTE_AUX_OFF	0x20	// 1 = mouse port disabled, 0 = enabled
#define PS2_CMD_BYTE_KBD_OFF	0x10	// 1 = keyboard port disabled, 0 = enabled
#define PS2_CMD_BYTE_OVER		0x08	// 1 = override keyboard lock
#define PS2_CMD_BYTE_RES		0x04	// reserved
#define PS2_CMD_BYTE_AUX_INT	0x02	// 1 = enable mouse interrupt
#define PS2_CMD_BYTE_KBD_INT	0x01	// 1 = enable keyboard interrupt

// Keyboard Commands (Sent to the Data Port)
#define PS2_KBD_CMD_LED			0xED	// Set Keyboard LEDS with next byte
#define PS2_KBD_CMD_ECHO		0xEE	// Echo - we get 0xFA, 0xEE back
#define PS2_KBD_CMD_MODE		0xF0	// set scan code mode with next byte
#define PS2_KBD_CMD_ID			0xF2	// get keyboard/mouse ID
#define PS2_KBD_CMD_RPT			0xF3	// Set Repeat Rate and Delay with Second Byte
#define PS2_KBD_CMD_ON			0xF4	// Enable keyboard
#define PS2_KBD_CMD_OFF			0xF5	// Disables Scanning and Resets to Defaults
#define PS2_KBD_CMD_DEF			0xF6	// Reverts keyboard to default settings
#define PS2_KBD_CMD_RST			0xFF	// Reset - we should get 0xFA, 0xAA back

// Set LED second bit defines
#define PS2_KBD_CMD_LED_SCROLL	0x01	// Set SCROLL LOCK LED on
#define PS2_KBD_CMD_LED_NUM		0x02	// Set NUM LOCK LED on
#define PS2_KBD_CMD_LED_CAPS	0x04	// Set CAPS LOCK LED on

// Set Mode second byte defines
#define PS2_KBD_CMD_MODE_STAT	0x00	// get current scan code mode
#define PS2_KBD_CMD_MODE_SCAN1	0x01	// set mode to scan code 1 - PC/XT
#define PS2_KBD_CMD_MODE_SCAN2	0x02	// set mode to scan code 2
#define PS2_KBD_CMD_MODE_SCAN3	0x03	// set mode to scan code 3

// Keyboard/Mouse ID Codes
#define PS2_KBD_CMD_ID_1ST		0xAB	// first byte is 0xAB, second is actual ID
#define PS2_KBD_CMD_ID_KBD		0x83	// Keyboard
#define PS2_KBD_CMD_ID_MOUSE	0x00	// Mouse

// Keyboard Data Return Defines
#define PS2_KBD_STAT_OVER		0x00	// Buffer Overrun
#define PS2_KBD_STAT_DIAG_OK	0x55	// Internal Self Test OK
#define PS2_KBD_STAT_RST_OK		0xAA	// Reset Complete
#define PS2_KBD_STAT_ECHO		0xEE	// Echo Command Return
#define PS2_KBD_STAT_BRK		0xF0	// Prefix for Break Key Code
#define PS2_KBD_STAT_ACK		0xFA	// Received after all commands
#define PS2_KBD_STAT_DIAG_FAIL	0xFD	// Internal Self Test Failed
#define PS2_KBD_STAT_RESEND		0xFE	// Resend Last Command

// HT8542B Status Register Bit Defines
#define PS2_STAT_OBF			0x01	// 1 = output buffer (controller to cpu) has data
#define PS2_STAT_IBF			0x02	// 1 = input buffer (cpu to controller) has data
#define PS2_STAT_SYS			0x04	// system flag - unused
#define PS2_STAT_CMD			0x08	// 1 = command in input buffer, 0 = data
#define PS2_STAT_INH			0x10	// 1 = Inhibit - unused
#define PS2_STAT_TX				0x20	// 1 = Transmit Timeout has occured
#define PS2_STAT_RX				0x40	// 1 = Receive Timeout has occured
#define PS2_STAT_PERR			0x80	// 1 = Parity Error from Keyboard

// Key codes for control keys
#define KEY_L_SHIFT		0x2a
#define KEY_R_SHIFT		0x36
#define KEY_CTL			0x1d
#define KEY_ALT			0x38
#define KEY_CAPS		0x3a
#define KEY_NUM_LK		0x45
#define KEY_SCROLL_LK	0x46
#define KEY_EXT0		0xE0
#define KEY_EXT1		0xE1
#define KEY_END			0x4f

// Scan Code to ASCII Conversion Table
// for Scan Code 1 - PC-XT Compatible
// We drop any characters after the max code
#define LAST_SCAN_CODE 0x40
static	uchar	ps2_scan_code[LAST_SCAN_CODE][4] = {
/*	        NORM, SHFT, CAPS,  CTL */
/* 0x00 */ {0xff, 0xff, 0xff, 0xff,},
/* 0x01 */ {0x1b, 0x1b, 0x1b, 0x1b,},	// ESC
/* 0x02 */ {'1',  '!',  '1',  0xff,},
/* 0x03 */ {'2',  '"',  '2',  0xff,},
/* 0x04 */ {'3',  '#',  '3',  0xff,},
/* 0x05 */ {'4',  '$',  '4',  0xff,},
/* 0x06 */ {'5',  '%',  '5',  0xff,},
/* 0x07 */ {'6',  '^',  '6',  0xff,},
/* 0x08 */ {'7',  '&',  '7',  0xff,},
/* 0x09 */ {'8',  '*',  '8',  0xff,},
/* 0x0a */ {'9',  '(',  '9',  0xff,},
/* 0x0b */ {'0',  ')',  '0',  0xff,},
/* 0x0c */ {'-',  '_',  '-',  0xff,},
/* 0x0d */ {'=',  '+',  '+',  0xff,},
/* 0x0e */ {'\b', '\b', '\b', 0xff,},	// BackSpace
/* 0x0f */ {'\t', '\t', '\t', 0xff,},	// TAB
/* 0x10 */ {'q',  'Q',  'Q',  0x11,},
/* 0x11 */ {'w',  'W',  'W',  0x17,},
/* 0x12 */ {'e',  'E',  'E',  0x05,},
/* 0x13 */ {'r',  'R',  'R',  0x12,},
/* 0x14 */ {'t',  'T',  'T',  0x14,},
/* 0x15 */ {'y',  'Y',  'Y',  0x19,},
/* 0x16 */ {'u',  'U',  'U',  0x15,},
/* 0x17 */ {'i',  'I',  'I',  0x09,},
/* 0x18 */ {'o',  'O',  'O',  0x0f,},
/* 0x19 */ {'p',  'P',  'P',  0x10,},
/* 0x1a */ {'[',  '{',  '[',  0x1b,},
/* 0x1b */ {']',  '}',  ']',  0x1d,},
/* 0x1c */ {'\r', '\r', '\r', '\n',},	// ENTER
/* 0x1d */ {0xff, 0xff, 0xff, 0xff,},	// CTL
/* 0x1e */ {'a',  'A',  'A',  0x01,},
/* 0x1f */ {'s',  'S',  'S',  0x13,},
/* 0x20 */ {'d',  'D',  'D',  0x04,},
/* 0x21 */ {'f',  'F',  'F',  0x06,},
/* 0x22 */ {'g',  'G',  'G',  0x07,},
/* 0x23 */ {'h',  'H',  'H',  0x08,},
/* 0x24 */ {'j',  'J',  'J',  0x0a,},
/* 0x25 */ {'k',  'K',  'K',  0x0b,},
/* 0x26 */ {'l',  'L',  'L',  0x0c,},
/* 0x27 */ {';',  ':',  ';',  0xff,},
/* 0x28 */ {'\'', '@',  '\'', 0xff,},
/* 0x29 */ {'#',  '~',  '#',  0xff,},
/* 0x2a */ {'`',  '~',  '`',  0xff,},
/* 0x2b */ {'\\', '|',  '\\', 0x1c,},
/* 0x2c */ {'z',  'Z',  'Z',  0x1a,},
/* 0x2d */ {'x',  'X',  'X',  0x18,},
/* 0x2e */ {'c',  'C',  'C',  0x03,},
/* 0x2f */ {'v',  'V',  'V',  0x16,},
/* 0x30 */ {'b',  'B',  'B',  0x02,},
/* 0x31 */ {'n',  'N',  'N',  0x0e,},
/* 0x32 */ {'m',  'M',  'M',  0x0d,},
/* 0x33 */ {',',  '<',  ',',  0xff,},
/* 0x34 */ {'.',  '>',  '.',  0xff,},
/* 0x35 */ {'/',  '?',  '/',  0xff,},
/* 0x36 */ {0xff, 0xff, 0xff, 0xff,},	// Right Shift
/* 0x37 */ {0xff, 0xff, 0xff, 0xff,},	// * PrtSc
/* 0x38 */ {0xff, 0xff, 0xff, 0xff,},	// ALT
/* 0x39 */ {' ',  ' ',  ' ',  ' ', },	// SpaceBar
/* 0x3a */ {0xff, 0xff, 0xff, 0xff,},	// CAPS Lock
/* 0x3b */ {0xf1, 0xe1, 0xf1, 0xff,},	// F1
/* 0x3c */ {0xf2, 0xe2, 0xf2, 0xff,},	// F2
/* 0x3d */ {0xf3, 0xe3, 0xf3, 0xff,},	// F3
/* 0x3e */ {0xf4, 0xe4, 0xf4, 0xff,},	// F4
/* 0x3f */ {0xf5, 0xe5, 0xf5, 0xff,},	// F5
};
#endif
