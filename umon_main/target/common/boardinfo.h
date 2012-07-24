/* boardinfo.h
 * Structures and data used by boardinfo facility.
 */
#ifndef _BOARDINFO_H_
#define _BOARDINFO_H_

struct boardinfo {
	unsigned char *array;
	short size;
	char *varname;
	char *def;
	char *prompt;
};

struct boardinfoverify {
	unsigned short len;
	unsigned short crc16;
}; 

#define BISIZE		sizeof(struct boardinfo)
#define BIVSIZE		sizeof(struct boardinfoverify)

/* boardinfotbl[]:
 * If the "boardinfo" facility in the monitor is to be used, then
 * this table must be included in the target-specific portion of
 * the monitor build.
 */
extern struct boardinfo boardinfotbl[];

extern void BoardInfoInit(void), BoardInfoEnvInit(void);
extern int BoardInfoVar(char *);
#endif
