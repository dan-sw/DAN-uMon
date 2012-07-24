/* tsi.h:
 * Header file for the touch-screen-interface code.
 */
extern int tsi_init(void);
extern int tsi_active(void);
extern int tsi_getx(void);
extern int tsi_gety(void);

#ifndef PEN_MOVE_FILTER
#define PEN_MOVE_FILTER			15
#endif
