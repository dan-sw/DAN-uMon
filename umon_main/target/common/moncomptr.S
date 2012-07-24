/* moncomptr:
 * The tag moncomptr is a tag established at a point in the target's
 * memory map that is unlikely to change as a result of rebuilding the
 * monitor.   As a result, this file is included by most of the target
 * ports in the very beginning of the reset space.
 *
 * The address of moncom (moncomptr) is used by the monConnect()
 * function in application space when the application attempts to
 * connect to the monitor's API.
 */
	.extern moncom
	.global moncomptr

moncomptr:
    .long   moncom
