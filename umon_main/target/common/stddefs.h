/*	stddefs.h:
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
#ifndef _STDDEFS_H_
#define _STDDEFS_H_

typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned long		uint32_t;
typedef unsigned long long	uint64_t;

typedef signed char			int8_t;
typedef signed short		int16_t;
typedef signed long			int32_t;
typedef signed long long	int64_t;

typedef volatile unsigned char		vuint8_t;
typedef volatile unsigned short		vuint16_t;
typedef volatile unsigned long		vuint32_t;
typedef volatile unsigned long long	vuint64_t;

typedef volatile signed char		vint8_t;
typedef volatile signed short		vint16_t;
typedef volatile signed long		vint32_t;
typedef volatile signed long long	vint64_t;


typedef unsigned char		uchar;
typedef unsigned short		ushort;
typedef unsigned long		ulong;
typedef unsigned int		uint;

typedef volatile unsigned char	vuchar;
typedef volatile unsigned short	vushort;
typedef volatile unsigned long	vulong;
typedef volatile unsigned int	vuint;
typedef volatile int			vint;


#endif
