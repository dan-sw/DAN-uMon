/* crypt.c:
 *
 * Simple one-way encryption function:
 *
 *	Use 256 bytes of random data as a very simple means of encrypting some
 *	data.  The result can be a maximum size of twice the incoming string.
 *
 *	If a character from the incoming string has either '00000110' bits set,
 *	then use that character twice.  This allows the encrypted string
 *	to be a varying length based on the content of the string.
 *
 *	Use the first two characters of "setting" plus the checksum of the 
 *	characters pointed to by result as the initial offset into the data table.
 *	This makes is less likely that passwords with similar starting characters
 *	will have similar starting characters in the encrypted result.
 *
 *  This function is coordinated with the maccrypt.exe tool that comes
 *  with the monitor.  It contains the same array as is defined below.
 *
 *  If you really plan to use this in a project that needs some password
 *  encryption; I strongly recommend that you at least change the content
 *  of the umon_crypt_datatbl[] defined below and in maccrypt.c.  To use
 *	your own data table, just define UNIQUE_DATATBL in config.h and then
 *	provide a umon_crypt_datatbl[] in main.c (part of the port-specific
 *	code).  Alternatively, if you want to use the password protection
 *	capability in uMon, but you don't necessarily like this encryption
 *	technique, you also have the option to simply replace this umon_crypt.c
 *	with your own.
 *
 *	General notice:
 *	This code is part of a boot-monitor package developed as a generic base
 *	platform for embedded system designs.  As such, it is likely to be
 *	distributed to various projects beyond the control of the original
 *	author.  Please notify the author of any enhancements made or bugs found
 *	so that all may benefit from the changes.  In addition, notification back
 *	to the author will allow the new user to pick up changes that may have
 *	been made by other users after this version of the code was distributed.
 *
 *	Note1: the majority of this code was edited with 4-space tabs.
 *	Note2: as more and more contributions are accepted, the term "author"
 *		   is becoming a mis-representation of credit.
 *
 *	Original author:	Ed Sutter
 *	Email:				esutter@lucent.com
 *	Phone:				908-582-2351
 */
#include "config.h"

#if INCLUDE_USRLVL

#ifdef UNIQUE_DATATBL
#include "unique_data_file.c"
#else
static char *umon_crypt_datatbl = "5gfHHJ^RVGJF#@FNM_NJE#W)(UJH(&T*%Ec54evX6t-098yun-9um9u8yv785rc5e43w3xw543wx43wx54ex7%R*^&RC*&TV_*(U_)(UN_)U*B76rc764ex643wx34ec6t098yn=9ui=n09im[pim[pimo(UB*)&GVB*&^FC$EX^$&*Y_(U*N(_*UN)9h8b087gv87f6cuytfghfvu654dx#SX53q2ax74f0p8u-[0in=-i-9ub098v087gv987g";
#endif

char *
crypt(char *string, char *setting, char *result)
{
	int	offset, csum;
	char *rp, *sp;

	csum = 0;
	sp = string;
	while(*sp)
		csum += *sp++;
	rp = result;

	/* Set up an offset into the data table that is based on the checksum
	 * of the incoming string plus the sum of the two setting characters...
	 */
	offset = (csum + setting[0] + setting[1]) % 255;

	/* For each character in the incoming string, replace it with a
	 * character in the data table.  If the incoming character has either
	 * 00000110 bit set, then use that character twice.
	 */
	while(*string) {
		offset += (int)*string;
		offset = offset % 255;
		*rp = umon_crypt_datatbl[offset];
		if (*string & 6) {
			rp++;
			offset += (int)*string;
			offset = offset % 255;
			*rp = umon_crypt_datatbl[offset];
		}
		string++;
		rp++;
	}
	*rp = 0;
	return(result);
}
#endif
