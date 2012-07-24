//==========================================================================
//
//      ps2_kbd.c
//
//      PS/2 Keyboard Controller Code
//
// Author(s):    Michael Kelly, Cogent Computer Systems, Inc.
// Contributors: 
// Date:         02/05/2002
// Description:  This file contains Code to intialize the Holtek HT6542
//				 Keyboard Controller for keyboard operation as well
//				 as a basic scan code to ascii converter suitable as an
//				 input device
//
//--------------------------------------------------------------------------

#include "config.h"
#if INCLUDE_PS2KBD
#include "cpuio.h"
#include "stddefs.h"
#include "genlib.h"
#include "ps2_kbd.h"

//--------------------------------------------------------------------------
// Function Prototypes
//
int ps2_kbd_init(void);
void ps2_kbd_test(void);
void ps2_kbd_obf_flush(void);
ulong wait_ps2_kbd_ibf_clr(void);
ulong wait_ps2_kbd_ibf_set(void);
ulong wait_ps2_kbd_obf_clr(void);
ulong wait_ps2_kbd_obf_set(void);
ulong ps2_gotachar(void);
uchar ps2_getchar(void); 
uchar ps2_scan(void);
int ps2_tst(void);

//--------------------------------------------------------------------------
// Global to hold the state of the interface
//
static vulong ps2_kbd_ok;
static vuchar ps2_ctl_flag, ps2_alt_flag, ps2_shift_flag;
static vuchar ps2_led_flag, ps2_break_c;

//--------------------------------------------------------------------------
// Globals to hold the ps2 character and status
//
static vuchar ps2_char, ps2_char_ok;

//--------------------------------------------------------------------------
// ps2_kbd_init()
// 
// The following function initializes the PS/2 Controller and performs a
// a simple self test.  The global, ps2_kbd_ok, will be set based
// on the results.  This allows the ascii routine to determine
// if a keyboard (or even the keyboard controller) is present.
//
// Notes:
//
// 1. There are two peripherals we are communicating with.  The first
//    is the PS/2 controller itself located on the motherboard.  The
//    second is the PS/2 keyboard connected via the Data/Clock pin.
//
// 2. The term "output buffer" refers to the buffer that the PS/2
//    controller uses to send data to the CPU.  From our sw point
//    of view it is the input to us.  But to prevent even more
//    confusion, we maintain that designation.  Likewise, the
//    "input buffer" refers to the location where the host places
//    commands to the PS/2 controller.
//
// 3. The keyboard is communicated with via the Data port when we
//    have established communication with a keyboard
//

int ps2_kbd_init()
{
	
	vuchar temp8;
	int retry;

	ps2_kbd_ok = 0;

	retry = 10;
	while(retry)
	{
		// flush the output buffer first
		ps2_kbd_obf_flush();

		// turn the keyboard port off then on
		if (wait_ps2_kbd_ibf_clr()) return -1;
	    PS2_REG(PS2_CMD) = PS2_CMD_KBD_OFF;
		monDelay (100);

		if (wait_ps2_kbd_ibf_clr()) return -1;
	    PS2_REG(PS2_CMD) = PS2_CMD_KBD_ON;
		ps2_kbd_obf_flush();

#ifdef PS2_DEBUG
	    printf("PS2 Controller Self Test - Sent: %02x\n", PS2_CMD_DIAG);
#endif
	    // Send the Diagnostic Command
		if (wait_ps2_kbd_ibf_clr()) return -1;
	    PS2_REG(PS2_CMD) = PS2_CMD_DIAG;

		// check the results
		if (wait_ps2_kbd_obf_set()) return -1;
		temp8 = PS2_REG(PS2_DATA);
	    if (temp8 != PS2_KBD_STAT_DIAG_OK) {
	        printf("PS2 Controller Self Test - Fail: %x\n", temp8);
			return -1;
	    }
#ifdef PS2_DEBUG
	    printf("PS2 Controller Self Test - Pass: %02x\n", temp8);
	    printf("PS2 Controller Enable Keyboard: %02x\n", PS2_CMD_KBD_ON);
#endif

	    // Enable the Keyboard Port on the PS controller itself
		if (wait_ps2_kbd_ibf_clr()) return -1;
	    PS2_REG(PS2_CMD) = PS2_CMD_KBD_ON;
		ps2_kbd_obf_flush();

	    // Send a Reset command to the keyboard
		if (wait_ps2_kbd_ibf_clr()) return -1;
	    PS2_REG(PS2_DATA) = PS2_KBD_CMD_RST;

		// wait until we get a response, then check for timeout
		if (wait_ps2_kbd_obf_set()) return -1;
		temp8 = PS2_REG(PS2_STAT);
		if (temp8 & (PS2_STAT_TX | PS2_STAT_RX)){
			// did we get a re-send?
			temp8 = PS2_REG(PS2_DATA);
    		if (temp8 != PS2_KBD_STAT_RESEND) continue;
		}
		else
		{
			// now check for ack
			temp8 = PS2_REG(PS2_DATA);
		    if (temp8 == PS2_KBD_STAT_ACK) break;	
		}
#ifdef PS2_DEBUG
    	printf("PS2 Keyboard - Re-sending Reset\n");
#endif
		retry--;

	} // while retry

	if (retry == 0){
#ifdef PS2_DEBUG
		printf("Keyboard Reset Failed. No Keyboard Connected?\n");
#endif
		return -1;
	}

	// If we got an ACK, the next byte should be Reset OK
	if (wait_ps2_kbd_obf_set()) return -1;
	temp8 = PS2_REG(PS2_DATA);
    if (temp8 != PS2_KBD_STAT_RST_OK)
     {
#ifdef PS2_DEBUG
        printf("Keyboard Reset Failed - Expected Reset OK: got: %02x\n", temp8);
#endif
        return -1;
    }

#ifdef PS2_DEBUG
    printf("PS2 Keyboard Reset - Received OK\n");
#endif
    // Set the keyboard to scan mode 1
    // First Flush the Output Buffer - The keyboard may have
    // sent us scan codes already
	ps2_kbd_obf_flush();

	if (wait_ps2_kbd_ibf_clr()) return -1;
    PS2_REG(PS2_DATA) = PS2_KBD_CMD_MODE;

    // Again, flush the Input Buffer
	ps2_kbd_obf_flush();

	if (wait_ps2_kbd_ibf_clr()) return -1;
    PS2_REG(PS2_DATA) = PS2_KBD_CMD_MODE_SCAN1;
        
	printf("PS/2 Keyboard found and initialized.\n");
	// initialize the globals
    ps2_kbd_ok = 1;
	ps2_char = 0;
	ps2_char_ok = 0;
	ps2_shift_flag = 0;
	ps2_alt_flag = 0;
	ps2_ctl_flag = 0;
	ps2_led_flag = 0;

    return 0;
}

//--------------------------------------------------------------------------
// wait_ps2_kbd_ibf_set()
//
// This routine waits until the Input Buffer Flag is Set,
// or time's out.  
ulong wait_ps2_kbd_ibf_set()
{
	int timeout;

	// check the results
    timeout = PS2_TIMEOUT;
	while(timeout){
		// delay before each check
		monDelay(1);
		if(PS2_REG(PS2_STAT) & PS2_STAT_IBF) break; 
   		timeout--;
    }
	if (timeout == 0){
		ps2_kbd_ok = 0;
        printf("PS2 Wait for Input Buffer Flag Set Timed Out - Controller Failure?\n");
		return 1;	
	}
	return 0;
}
	

//--------------------------------------------------------------------------
// wait_ps2_kbd_ibf_clr()
//
// This routine waits until the Input Buffer Flag is Clear,
// or time's out.  
ulong wait_ps2_kbd_ibf_clr()
{
	int timeout;

	// check the results
    timeout = PS2_TIMEOUT;
	while(timeout){
		// delay before each check
		monDelay(1);
		if((PS2_REG(PS2_STAT) & PS2_STAT_IBF) == 0) break; 
   		timeout--;
    }
	if (timeout == 0){
		ps2_kbd_ok = 0;
        printf("PS2 Wait for Input Buffer Flag Clear Timed Out - Controller Failure?\n");
		return 1;	
	}
	return 0;
}
	

//--------------------------------------------------------------------------
// ps2_kbd_obf_flush()
//
// This routine waits until the Output Buffer Flag is Clear,
// by reading the data port continuously
void ps2_kbd_obf_flush()
{
    // Clear the Output Buffer
    while ((PS2_REG(PS2_STAT) & PS2_STAT_OBF)) {
        PS2_REG(PS2_DATA);
    }
}
	

//--------------------------------------------------------------------------
// wait_ps2_kbd_obf_clr()
//
// This routine waits until the Output Buffer Flag is Cleared,
// or time's out
ulong wait_ps2_kbd_obf_clr()
{
	int timeout;

	// check the results
    timeout = PS2_TIMEOUT;
	while(timeout){
		// delay before each check
		monDelay(1);
		if ((PS2_REG(PS2_STAT) & PS2_STAT_OBF) == 0) break;
   		timeout--;
    }
	if (timeout == 0){
		ps2_kbd_ok = 0;
        printf("PS2 Wait for Output Buffer Flag Clear Timed Out - Controller Failure?\n");
		return -1;	
	}
	return 0;
}
	
//--------------------------------------------------------------------------
// wait_ps2_kbd_obf_set()
//
// This routine waits until the Output Buffer Flag is Set,
// or time's out
ulong wait_ps2_kbd_obf_set()
{
	int timeout;

	// check the results
    timeout = PS2_TIMEOUT;
	while(timeout){
		// delay before each check
		monDelay(1);
		if (PS2_REG(PS2_STAT) & PS2_STAT_OBF) break;
   		timeout--;
    }
	if (timeout == 0){
		ps2_kbd_ok = 0;
        printf("PS2 Wait for Output Buffer Flag Set Timed Out - Controller Failure?\n");
		return -1;	
	}
	return 0;
}
	
//--------------------------------------------------------------------------
// ps2_gotachar()
//
// This routine will retreive a character from the PS/2 keyboard.  We
// have to do a scan code conversion to see if we have a returnable
// character, thus removing it from the ps/2 controller.  This routine
// will save the converted value, if valid, in ps2_char for retrieval
// by ps2_getchar.
//
ulong ps2_gotachar()
{
	vuchar sc, c;

	// make sure we even have a keyboard
	if(ps2_kbd_ok == 0) return 0;

	// return true immediately if we still have a
	// character that has not been read
	if(ps2_char_ok == 1) return 1;

	// get a scan code
	sc = ps2_scan();
	if(sc){
		// make sure we have a valid code
		// if not we just ignore it
		if (sc <= LAST_SCAN_CODE) {

			// check the state of caps, ctl and shift to get the
			// right array
			if (ps2_ctl_flag) {
				c = ps2_scan_code[sc][3];
			}
			else if(ps2_shift_flag) {
				c = ps2_scan_code[sc][1];
			}
			else if(ps2_led_flag & PS2_KBD_CMD_LED_CAPS){
				c = ps2_scan_code[sc][2];
			}
			else {
				c = ps2_scan_code[sc][0];
			}
			// see if we have a valid ascii lookup	
			if(c != 0xff) {	
				ps2_char = c; 
				ps2_char_ok = 1;
				return 1;
			}
		} // if sc <= LAST_SCAN_CODE
	} // if sc
	return 0;
}	

//--------------------------------------------------------------------------
// ps2_getchar()
//
// This routine returns ps2_char
//
uchar ps2_getchar()
{
	if((ps2_kbd_ok) && (ps2_char_ok)) {
		ps2_char_ok = 0;
		return ps2_char;
	}
	return 0;
}

//--------------------------------------------------------------------------
// ps2_scan()
//
// This routine will return a scan code from the keyboard if there is
// one.  If not, or if it is a control key, it will return 0.  For control
// keys it will update the global flags so the calling routine can
// interpret the next non-control key appropriately. 
//
uchar ps2_scan()
{
	vuchar c;

	if(PS2_REG(PS2_STAT) & PS2_STAT_OBF){
		c = PS2_REG(PS2_DATA);
		// if it is a break, we still process it to handle de-assertion of shift, alt, etc,
		if(c & 0x80) ps2_break_c = 1;
		// strip the break bit
		c = c & 0x7f;
		switch (c){
			case KEY_EXT0 :
			case KEY_EXT1 :
				break;
			// set the flags for shift, ctl and alt when the key
			// is pressed, clear when the key is released
			case KEY_R_SHIFT :
			case KEY_L_SHIFT : 
				if(ps2_break_c == 0) ps2_shift_flag = 1;
				else{ ps2_shift_flag = 0; ps2_break_c = 0; }
				break;
			case KEY_ALT : 
				if(ps2_break_c == 0) ps2_alt_flag = 1;
				else { ps2_alt_flag = 0; ps2_break_c = 0; }
				break;
			case KEY_CTL : 
				if(ps2_break_c == 0) ps2_ctl_flag = 1;
				else { ps2_ctl_flag = 0; ps2_break_c = 0; }
				break;
			// update led_flag when CAPS, NUM or Scroll lock are pressed,
			// maintain state when they are released.
			case KEY_CAPS :
				if(ps2_break_c == 0) ps2_led_flag = ps2_led_flag ^ PS2_KBD_CMD_LED_CAPS;
				else ps2_break_c = 0;
				// update the LED's
				if (wait_ps2_kbd_ibf_clr()) return 0;
			    PS2_REG(PS2_DATA) = PS2_KBD_CMD_LED;
				if (wait_ps2_kbd_ibf_clr()) return 0;
			    PS2_REG(PS2_DATA) = ps2_led_flag & 0x07;
				ps2_kbd_obf_flush();
				break;
			case KEY_NUM_LK : 
				if(ps2_break_c == 0) ps2_led_flag = ps2_led_flag ^ PS2_KBD_CMD_LED_NUM;
				else ps2_break_c = 0;
				// update the LED's
				if (wait_ps2_kbd_ibf_clr()) return 0;
			    PS2_REG(PS2_DATA) = PS2_KBD_CMD_LED;
				if (wait_ps2_kbd_ibf_clr()) return 0;
			    PS2_REG(PS2_DATA) = ps2_led_flag & 0x07;
				ps2_kbd_obf_flush();
				break;
			case KEY_SCROLL_LK : 
				if(ps2_break_c == 0) ps2_led_flag = ps2_led_flag ^ PS2_KBD_CMD_LED_SCROLL;
				else ps2_break_c = 0;
				// update the LED's
				if (wait_ps2_kbd_ibf_clr()) return 0;
			    PS2_REG(PS2_DATA) = PS2_KBD_CMD_LED;
				if (wait_ps2_kbd_ibf_clr()) return 0;
			    PS2_REG(PS2_DATA) = ps2_led_flag & 0x07;
				ps2_kbd_obf_flush();
				break;
			default : 
//				printf("c = 0x%02x\n", c);
				if(ps2_break_c == 0) return c;
				else { ps2_break_c = 0; break; }
		} // switch
	} // if	PS2_STAT
	return 0;
}

//--------------------------------------------------------------------------
// ps2_tst()
//
// This routine will display an ascii string corresponding to the 
// received scan code.  The END key will end the test.
//
char *ps2Help[] = {
	"PS/2 compatible port test",
	"",
	"Notes:",
	" * For each keypress (except control keys), this test will display",
	"   an ascii string describing the scan code.  If the scan code is",
	"   not recognized the hex value of the scan code will be displayed",
	"   instead.",
	" * Pressing the END Key terminates the test.  It also updates the",
	"   LEDs when CAPS, Num Lock or Scroll Lock are pressed.",
	(char *)0,
};

int ps2_tst()
{
	vuchar c, sc;

	ps2_kbd_init();

	if (ps2_kbd_ok == 0){
		printf("PS/2 Keyboard Init Failed.\n");
		return 0;
	}
	printf("Keyboard Detected - Turning LEDs off.\n");

	// Flush the Output buffer
	ps2_kbd_obf_flush();	

	// Blink the LED's
	if (wait_ps2_kbd_ibf_clr()) return 0;
    PS2_REG(PS2_DATA) = PS2_KBD_CMD_LED;
	if (wait_ps2_kbd_ibf_clr()) return 0;
    PS2_REG(PS2_DATA) = 0x00;

	monDelay(100);

	printf("Turning LEDs on.\n");
	ps2_kbd_obf_flush();	
	if (wait_ps2_kbd_ibf_clr()) return 0;
    PS2_REG(PS2_DATA) = PS2_KBD_CMD_LED;
	if (wait_ps2_kbd_ibf_clr()) return 0;
    PS2_REG(PS2_DATA) = PS2_KBD_CMD_LED_SCROLL |
    					PS2_KBD_CMD_LED_NUM |
    					PS2_KBD_CMD_LED_CAPS;

	monDelay(100);

	printf("Turning LEDs off.\n");
	ps2_kbd_obf_flush();	
	if (wait_ps2_kbd_ibf_clr()) return 0;
    PS2_REG(PS2_DATA) = PS2_KBD_CMD_LED;
	if (wait_ps2_kbd_ibf_clr()) return 0;
    PS2_REG(PS2_DATA) = 0x00;

	if (wait_ps2_kbd_ibf_clr()) return 0;
	ps2_kbd_obf_flush();
		
	ps2_shift_flag = 0;
	ps2_alt_flag = 0;
	ps2_ctl_flag = 0;
	ps2_led_flag = 0;

	printf("Entering Scan Code Display Loop.  Press END key to terminate.\n");
	// loop until the END key or an error (which will clear ps2_kbd_ok)      
	while(ps2_kbd_ok == 1){
		sc = ps2_scan();
		if(sc){
			// make sure we have a valid code
			// if not we just ignore it
			if (sc <= LAST_SCAN_CODE) {

				printf("Raw Keypress = 0x%02x, ", sc);
				printf("Converted Keypress = ");

				// print a leading CTL- or ALT- based on the flags
				if(ps2_ctl_flag) printf("CTL-");
				if(ps2_alt_flag) printf("ALT-");

				// check the state of caps and shift to get the
				// right array
				if(ps2_shift_flag) {
					c = ps2_scan_code[sc][1];
				}
				else if(ps2_led_flag & PS2_KBD_CMD_LED_CAPS){
					c = ps2_scan_code[sc][2];
				}
				else {
					c = ps2_scan_code[sc][0];
				}
				// only print it if it is printable
				if ((c < 0x7f) && (c > 0x1f)) printf("%c\n", c); 
				else printf("\n");
			} // if c <= LAST_SCAN_CODE
		} // if ps2_scan
	 	if(c == KEY_END) break;
	} // while
	return 0; 
}

//--------------------------------------------------------------------------
// End of ps2_kbd.c

#endif
