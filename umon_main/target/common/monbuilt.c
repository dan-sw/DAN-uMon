/* monbuilt.c:
 *	This file contains all of the monitor code that constructs
 *	the time and date of the build.
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

/* whatbuild:
 * A 'what' string to allow the 'what' tool to query the binary
 * image built with this source code.
 */
#define WHAT_PREFIX	"@(#)"

char *whatbuild = WHAT_PREFIX __DATE__ " @ " __TIME__;

/* monBuilt():
 * Return a pointer to a string that is a concatenation of the
 * build date and build time.
 */

char *
monBuilt(void)
{
	return(whatbuild+sizeof(WHAT_PREFIX)-1);
}
