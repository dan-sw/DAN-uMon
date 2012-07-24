#ifndef _MONPROF_H_
#define _MONPROF_H_

#define MONPROF_FUNCLOG		(1 << 0)
#define MONPROF_TIDLOG		(1 << 1)		
#define MONPROF_PCLOG		(1 << 2)		

struct monprof {
	unsigned long	type;
	unsigned long	pc;
	unsigned long	tid;
};

#endif
