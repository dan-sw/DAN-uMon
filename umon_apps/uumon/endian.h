/* endian.h:
 *	macros used for endian conversion.
 *	The intent is that the macros have nil effect on Big_Endian
 *	systems...
 *	ecs:	endian-convert short
 *	ecl:	endian-convert long
 */
#ifndef _ENDIAN_H_
#define _ENDIAN_H_

#ifdef CPU_LE
#define ecs(a) (((a & 0x00ff) << 8) | ((a & 0xff00) >> 8))
#define ecl(a) (((a & 0x000000ff) << 24) | ((a & 0x0000ff00) << 8) | \
		  ((a & 0x00ff0000) >> 8) | ((a & 0xff000000) >> 24))
#define self_ecs(a)	(a = ecs(a))
#define self_ecl(a)	(a = ecl(a))
#else
#define ecs(a)  a
#define ecl(a)  a
#define self_ecs(a)
#define self_ecl(a)
#endif

#endif
