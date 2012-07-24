/*
 * General notice:
 * This code is part of a boot-monitor package developed as a generic base
 * platform for embedded system designs.  As such, it is likely to be
 * distributed to various projects beyond the control of the original
 * author.  Please notify the author of any enhancements made or bugs found
 * so that all may benefit from the changes.  In addition, notification back
 * to the author will allow the new user to pick up changes that may have
 * been made by other users after this version of the code was distributed.
 *
 * Author:	Ed Sutter
 * email:	esutter@lucent.com
 * phone:	908-582-2351
 *
 * This file is used in conjunction with the "filldevice" make file target.
 * It provides an easy way to build a file (s-record or binary image) that
 * represents a "filled" memory device.  This is especially useful when
 * burning a flash device to be used with a CPU architecture that boots at
 * the top of memory.
 */

	.file "prefill.s"

	.byte 0xff
