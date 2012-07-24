#include "stddefs.h"
#include "genlib.h"
#include "pci.h"

/* The PCI Configuration registers are all in MMIO (memory mapped IO)...
 */
#define PCICFGADR		0xEEC00000
#define PCICFGDATA		0xEEC00004
#define PMM0LA			0xEF400000
#define PMM0MA			0xEF400004
#define PMM0PCILA		0xEF400008
#define PMM0PCIHA		0xEF40000C
#define PMM1LA			0xEF400010
#define PMM1MA			0xEF400014
#define PMM1PCILA		0xEF400018
#define PMM1PCIHA		0xEF40001C
#define PMM2LA			0xEF400020
#define PMM2MA			0xEF400024
#define PMM2PCILA		0xEF400028
#define PMM2PCIHA		0xEF40002C
#define PTM1MS			0xEF400030
#define PTM1LA			0xEF400034
#define PTM2MS			0xEF400038
#define PTM2LA			0xEF40003C

int
pciCfgWrite(int interface,int bus,int device,int func,int regno,ulong value)
{
	*(ulong *)PCICFGADR = swap4(pciCfgAddress(bus,device,0,regno));
    *(ulong *)(PCICFGDATA | (regno << 2)) = swap4(value);

	return(0);
}

ulong
pciCfgRead(int interface,int bus,int device,int func,int regno)
{
	*(ulong *)PCICFGADR = swap4(pciCfgAddress(bus,device,0,regno));
    return(swap4(*(ulong *)(PCICFGDATA | (regno << 2))));
}

int
pciCtrl(int interface, int cmd, unsigned long arg1, unsigned long arg2)
{
	if (cmd == PCICTRL_INIT) {
		return(0);
	}
	else
		return(-1);
}

