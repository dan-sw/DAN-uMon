/* PSR Bits:
 */
#define PSR_THUMB_STATE			0x00000020
#define PSR_IMASK_IRQ			0x00000080
#define PSR_IMASK_FRQ			0x00000040
#define PSR_CONDITION_NEGATIVE	0x80000000
#define PSR_CONDITION_ZERO		0x40000000
#define PSR_CONDITION_CARRY		0x20000000
#define PSR_CONDITION_OVERFLOW	0x10000000
#define PSR_MODE_MASK			0x0000001f

/* Mode bits within PSR:
 */
#define ABORT_MODE			0x00000017
#define FASTINTRQST_MODE	0x00000011
#define INTRQST_MODE		0x00000012
#define SUPERVISOR_MODE		0x00000013
#define SYSTEM_MODE			0x0000001f
#define UNDEFINED_MODE		0x0000001b
#define USER_MODE			0x00000010

/* Exception types:
 */
#define EXCTYPE_UNDEF		1
#define EXCTYPE_ABORTP		2
#define EXCTYPE_ABORTD		3
#define EXCTYPE_IRQ			4
#define EXCTYPE_FIRQ		5
#define EXCTYPE_SWI			6
#define EXCTYPE_NOTASSGN	7

/* Link register adjustments for each exception:
 * These adjustments are used by the exception handler to establish the
 * address at which the exception occurred.
 */
#define LRADJ_UNDEF			4
#define LRADJ_ABORTP		4
#define LRADJ_ABORTD		8
#define LRADJ_IRQ			4
#define LRADJ_FIRQ			4
#define LRADJ_SWI			4

/* Vector numbers used by assign_handler and the mon_assignhandler() 
 * API function...
 */
#define VEC_RST              0
#define VEC_UND              1
#define VEC_SWI              2
#define VEC_ABP              3
#define VEC_ABD              4
#define VEC_IRQ              5
#define VEC_RESERVED         6
#define VEC_FIQ              7
