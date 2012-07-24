//==========================================================================
//
//      pci.c
//
//      PCI probing routine
//
// Author(s):    Michael Kelly, Cogent Computer Systems, Inc.
// Contributors: 
// Date:         02/05/2002
// Description:  This file contains the generic PCI probe routine
//				 and configuration accesses as well as a simple test		
//
//--------------------------------------------------------------------------

#include "config.h"
#include "cpuio.h"
#include "stddefs.h"
#include "genlib.h"
#include "pci.h"
#include "cpu_pci.h"	// bring in CPU specific defines for PCI

//--------------------------------------------------------------------------
// Function Prototypes
//
int pci_probe_bus(void);
int pci_tst(int argc,char *argv[]);

// external functions
extern ulong pci_cfg_read_dword (ulong bus, ulong dev_func, ulong offset);
extern ushort pci_cfg_read_word (ulong bus, ulong dev_func, ulong offset);
extern uchar pci_cfg_read_byte (ulong bus, ulong dev_func, ulong offset);
extern void pci_cfg_write_dword (ulong bus, ulong dev_func, ulong offset, ulong data);
extern void pci_cfg_write_word (ulong bus, ulong dev_func, ulong offset, ushort data);
extern void pci_cfg_write_byte (ulong bus, ulong dev_func, ulong offset, uchar  data);

extern void set_cfg_type_id(ulong bus, ulong dev_func);
extern int pci_master_abort(void);
extern void udelay(int), pci_init(void);

// globals to keep track of the pci devices we find
pci_device_structure pci_dev_info[PCI_DEV_MAX];
ulong next_bus, mem_base, io_base, pci_dev;
// global shared memory base address for all PCI devices
ulong sh_mem_base;

//--------------------------------------------------------------------------
// pci_probe_bus()
// 
// The following function scans the PCI bus and fills in a static structure
// with the various ID's and the BAR's.  This function is reentrant to 
// handle bus bridges.  It returns 0 if no error.  
//
// NOTES:
// 1. This function does not enable the devices it finds.  It merely assigns
//    address space to them and stores the information in the pci_dev_info 
//    structure.  It is up to the respective driver to fully setup each
//    device.  
// 2. The one exception to note 2. above is that the code will setup and
//    enable any bridges it finds.
// 3. All Memory space across bridges will be assigned as non-prefetchable.
//    We do not have enough memory space to consider true memory type devices
//    versus memory mapped I/O registers.  For simplicity and safety, we 
//    assume that all MEM space is not prefetchable.
// 4. We do not support bridges with only 64KB IO windows.  This does not seem
//    to be a limitation of most modern bridges.
// 5. This function will assign space to devices as it finds them without
//    regard to fragmentation.  That means if we find a 1Mbyte device (the
//    minimum we assign for memory) and then find a 16Mbyte device, the 1M
//    device will start at mem_base while the 16M device will start at
//    mem_base + 16M.  The space from 1M to 16M will be lost.
// 5. This function is reentrant to handle pci-pci bridges.

int pci_probe_bus(void){

	uchar temp8;
	ushort temp16;
	ulong temp32;
	ulong bus, dev, func;
	ulong bar_size, bar_type, bar_boundry, bar;
	ulong old_bus, old_mem_base, old_io_base;

	bus = next_bus++;

	// scan the chosen bus for devices
	for (dev = DEV_START; dev < DEV_START + 7; dev++){
		for (func = 0; func < 7; func++){

			// read the vendor ID.  A 0xFFFF means no device
			temp16 = pci_cfg_read_word (bus, PCI_MAKE_DEV_FUNC(dev, func), PCI_CFG_VEN_ID_REG);
#ifdef PCI_DBG
			printf("PCI Probe Read %04x\n", temp16);
#endif
			if (temp16 != 0xFFFF){

				// got a live one! Now see if it's a bridge
				temp8 = pci_cfg_read_byte (bus, PCI_MAKE_DEV_FUNC(dev, func), PCI_CFG_CLASS_REG);
				if (temp8 == PCI_CLASS_BRIDGE){

#ifdef PCI_DBG
				printf("PCI Probe Found Bridge\n");
#endif
					// if it's not a pci to pci bridge bail out.
					if (pci_cfg_read_byte (bus, PCI_MAKE_DEV_FUNC(dev, func), PCI_CFG_SUBCLASS_REG) != PCI_SUBCLASS_BRIDGE_PCI_PCI) 
						return PCI_BRIDGE_TYPE_ERR;
					 
					else { // probe it

						// save the info for the bus we are on - we'll need this when we return
						old_bus = bus;
						old_mem_base = mem_base;
						old_io_base = io_base;

						// setup the bridge's bus registers so we can probe the other side
						pci_cfg_write_byte(bus, PCI_MAKE_DEV_FUNC(dev, func), PCI_CFG_PRI_BUS_REG, bus);
						pci_cfg_write_byte(bus, PCI_MAKE_DEV_FUNC(dev, func), PCI_CFG_SEC_BUS_REG, next_bus);

						// reenter pci_probe_bus to scan the next bus
						temp32 = pci_probe_bus();
						if (temp32) return temp32;	// non-zero means something went wrong

						bus = old_bus;

						// after returning from a bridge probe, we need to set the subordinate bus number to
						// the highest bus found on the other side of this bridge.  That will be next_bus -1.
						pci_cfg_write_byte(bus, PCI_MAKE_DEV_FUNC(dev, func), PCI_CFG_SUB_BUS_REG, (next_bus - 1));

						// fill in the bridge registers
						// round up the final mem_base if necessary
						if (mem_base & 0x000fffff) mem_base = (mem_base + 0x00100000) & 0xfff00000;

						// disable prefetch bys setting the base higher than the limit
						pci_cfg_write_word(bus, PCI_MAKE_DEV_FUNC(dev, func), PCI_CFG_PREFETCH_BASE_REG, 0xFFFF);
						pci_cfg_write_word(bus, PCI_MAKE_DEV_FUNC(dev, func), PCI_CFG_PREFETCH_LIMIT_REG, 0x0000);

						// rounded mem_base is the beginning of the bridge's MEM window, mem_base is the end
						pci_cfg_write_word(bus, PCI_MAKE_DEV_FUNC(dev, func), PCI_CFG_MEM_BASE_REG, ((old_mem_base >> 16) & 0xFFFF));
						pci_cfg_write_word(bus, PCI_MAKE_DEV_FUNC(dev, func), PCI_CFG_MEM_LIMIT_REG, ((mem_base  >> 16) & 0xFFFF));

						// enable the bridge to handle memory accesses and mastering
						temp16 = pci_cfg_read_word (bus, PCI_MAKE_DEV_FUNC(dev, func), PCI_CFG_CMD_REG);
						temp16 = temp16 | PCI_CFG_CMD_MEM | PCI_CFG_CMD_MASTER;
						pci_cfg_write_word (bus, PCI_MAKE_DEV_FUNC(dev, func), PCI_CFG_CMD_REG, temp16);

						// fill in the bridge registers
						// round up the final io_base if necessary
						if (io_base & 0x00000fff) io_base = (io_base + 0x00001000) & 0xfffff000;

						// make sure this bridge supports 32-bit IO addressing
						if ((pci_cfg_read_byte(bus, PCI_MAKE_DEV_FUNC(dev, func), PCI_CFG_IO_BASE_LO_REG) & 0x0f) != 0x01)
							return PCI_BRIDGE_TYPE_ERR;

						// rounded io_base is the beginning of the bridge's IO window, io_base is the end
						pci_cfg_write_word(bus, PCI_MAKE_DEV_FUNC(dev, func), PCI_CFG_IO_BASE_HI_REG, ((old_io_base >> 16) & 0xFFFF));
						pci_cfg_write_word(bus, PCI_MAKE_DEV_FUNC(dev, func), PCI_CFG_IO_LIMIT_HI_REG, ((io_base  >> 16) & 0xFFFF));

						// enable the bridge to handle io accesses and mastering
						temp16 = pci_cfg_read_word (bus, PCI_MAKE_DEV_FUNC(dev, func), PCI_CFG_CMD_REG);
						temp16 = temp16 | PCI_CFG_CMD_IO | PCI_CFG_CMD_MASTER;
						pci_cfg_write_word (bus, PCI_MAKE_DEV_FUNC(dev, func), PCI_CFG_CMD_REG, temp16);

					} // else - pci to pci bridge
				} // if bridge
				else { // not a bridge
#ifdef PCI_DBG
					printf("PCI Probe Found Non-Bridge\n");
#endif
					pci_dev_info[pci_dev].ven_id = pci_cfg_read_word(bus, PCI_MAKE_DEV_FUNC(dev, func), PCI_CFG_VEN_ID_REG);
					pci_dev_info[pci_dev].dev_id = pci_cfg_read_word(bus, PCI_MAKE_DEV_FUNC(dev, func), PCI_CFG_DEV_ID_REG);
					pci_dev_info[pci_dev].class = pci_cfg_read_byte(bus, PCI_MAKE_DEV_FUNC(dev, func), PCI_CFG_CLASS_REG);
					pci_dev_info[pci_dev].subclass = pci_cfg_read_byte(bus, PCI_MAKE_DEV_FUNC(dev, func), PCI_CFG_SUBCLASS_REG);

#ifdef PCI_DBG
					printf("PCI Probe Vendor ID = %08x\n", pci_dev_info[pci_dev].ven_id);
					printf("PCI Probe Device ID = %08x\n", pci_dev_info[pci_dev].dev_id);
					printf("PCI Probe Class 	= %08x\n", pci_dev_info[pci_dev].class);
					printf("PCI Probe Subclass 	= %08x\n", pci_dev_info[pci_dev].subclass);
#endif
					// scan each BAR and assign memory or io as appropriate
					for (bar = 0; bar < 6; bar++){
						// Write all 1's to the BAR and then read back to get size, type
						pci_cfg_write_dword(bus, PCI_MAKE_DEV_FUNC(dev, func), ((bar << 2) + PCI_CFG_BAR0_REG), 0xffffffff);
						bar_size = pci_cfg_read_dword(bus, PCI_MAKE_DEV_FUNC(dev, func), ((bar << 2) + PCI_CFG_BAR0_REG));

						if (bar_size != 0) {
							// is it IO or MEM?
							if (bar_size & 0x01){

								// assign a minimum of 4K for I/O
								if(bar_size < 0x00001000) bar_size = 0xfffff000;

								// save the boundry as a mask for use later
								bar_boundry = ~(bar_size & 0xfffffffc);

								// IO - convert raw bar size to final size
								bar_size = bar_boundry + 1;

								// sanity check it against maximum size IO we allow
								if (bar_size > PCI_IO_BAR_MAX) return PCI_IO_BAR_MAX_ERR;

								// All allocation must occur on the boundry specificed by the size
								if (io_base & ~bar_boundry){
									io_base = io_base + bar_size;
									io_base = io_base & ~bar_boundry;
									pci_dev_info[pci_dev].bar_size[bar] = bar_size;
									pci_dev_info[pci_dev].bar_map[bar] = io_base;
									pci_cfg_write_dword(bus, PCI_MAKE_DEV_FUNC(dev, func), ((bar << 2) + PCI_CFG_BAR0_REG), io_base);
								}
								else {
									pci_dev_info[pci_dev].bar_size[bar] = bar_size;
									pci_dev_info[pci_dev].bar_map[bar] = io_base;
									pci_cfg_write_dword(bus, PCI_MAKE_DEV_FUNC(dev, func), ((bar << 2) + PCI_CFG_BAR0_REG), io_base);
								}
								// do we really have the space?
								if (io_base > PCI_IO_BASE + PCI_IO_SIZE) return PCI_IO_SPACE_ERR;
								// Yes
								io_base = io_base + bar_size;
							}
							else { // MEM
#ifdef PCI_DBG
								printf("PCI Probe Raw Mem BAR = %08x\n", bar_size);
#endif
								// assign a minimum of 1M for Memory
								if(bar_size < 0x00100000) bar_size = 0xffff0000;

								// save the boundry as a mask for use later
								bar_boundry = ~(bar_size & 0xfffffff0);

#ifdef PCI_DBG
								printf("PCI Probe BAR Boundry = %08x\n", bar_boundry);
#endif
								// Check the BAR type as we only support 32 or 64-bit BAR's
								bar_type = (bar_size & 0x06) >> 1;
								if ((bar_type != PCI_MEM_BAR_TYPE_32BIT) && (bar_type != PCI_MEM_BAR_TYPE_64BIT)) return PCI_MEM_BAR_TYPE_ERR;

								// MEM - convert raw bar size to final power of 2 size
								bar_size = bar_boundry + 1;

#ifdef PCI_DBG
								printf("PCI Probe Final BAR Size = %08x\n", bar_size);
#endif
								// sanity check it against maximum size MEM we allow
								if (bar_size > PCI_MEM_BAR_MAX)
								{
									return PCI_MEM_BAR_MAX_ERR;
								}

								// All allocation must occur on the boundry specificed by the size
								if (mem_base & ~bar_boundry){
									mem_base = mem_base + bar_size;
									mem_base = mem_base & ~bar_boundry;
									pci_dev_info[pci_dev].bar_size[bar] = bar_size;
									pci_dev_info[pci_dev].bar_map[bar] = mem_base;
									pci_cfg_write_dword(bus, PCI_MAKE_DEV_FUNC(dev, func), ((bar << 2) + PCI_CFG_BAR0_REG), mem_base);
								}
								else {
									pci_dev_info[pci_dev].bar_size[bar] = bar_size;
									pci_dev_info[pci_dev].bar_map[bar] = mem_base;
									pci_cfg_write_dword(bus, PCI_MAKE_DEV_FUNC(dev, func), ((bar << 2) + PCI_CFG_BAR0_REG), mem_base);
								}
								// do we really have the space?
								if (mem_base > PCI_MEM_BASE + PCI_MEM_SIZE) return PCI_MEM_SPACE_ERR;
								// Yes
								mem_base = mem_base + bar_size;

								// if this was a 64-bit BAR, fill the next BAR with all 0's
								if (bar_type == PCI_MEM_BAR_TYPE_64BIT){
									bar++;
									pci_cfg_write_dword(bus, PCI_MAKE_DEV_FUNC(dev, func), ((bar << 2) + PCI_CFG_BAR0_REG), 0x00000000);
									pci_dev_info[pci_dev].bar_size[bar] = 0x0;
									pci_dev_info[pci_dev].bar_map[bar] = 0x0;
								} // if 64-bit BAR
							} // IO or MEM
						} // if bar_size != 0
						else { // invalid bar
							pci_dev_info[pci_dev].bar_size[bar] = 0x0;
							pci_dev_info[pci_dev].bar_map[bar] = 0x0;
						}
					} // for BAR
					pci_dev_info[pci_dev].bus = bus;
					pci_dev_info[pci_dev].dev = dev;
					pci_dev_info[pci_dev].func = func;
					pci_dev++;
					if (pci_dev > PCI_DEV_MAX) return PCI_DEV_MAX_ERR;  // is this really an error?
				
					// Some devices ignore the function number, so check the multifunction bit in the header type reg
					if ((pci_cfg_read_byte (bus, PCI_MAKE_DEV_FUNC(dev, func), PCI_CFG_HDR_TYPE_REG) & 0x80) == 0x00)
						break;	// not a mutlifunction header so break out of the function loop
				} // not a bridge
			} // if non-0xFFFF vendor
			else // 0xFFFF returned 
				break;	// don't bother checking the remaining functions for the current device
		} // for function
	} // for device
	return 0;
}

//--------------------------------------------------------------------------
// pci_tst()
//
// This routine inits and then probes the PCI bus
// then displays the results
//
char *pciHelp[] = {
	"PCI bus test",
    "-[i]",
    "Options:",
    " -i Get info only, does not run test",
	(char *)0,
};

int pci_tst(int argc,char *argv[])
{
    int  next_bus;
	int temp, i, x, opt;
	int get_info = 0;

    while ((opt=getopt(argc,argv,"i")) != -1) {
        switch(opt) {
	        case 'i':   // get info only
				get_info = 1;
			break;
			default:	// test with current mode
				break;
		}
	}
	if (get_info == 0)
	{
		// Init the PCI Interface
		pci_init();
		printf("Begin PCI Bus Probe.\n");
		// Now probe the bus
		next_bus = 0;
		mem_base = PCI_MEM_BASE;
		io_base = PCI_IO_BASE;
		pci_dev = 0;
		
		temp = pci_probe_bus();
		if (temp){
			printf("PCI Probe Failed, Returned Status = %08x\n", temp);
			printf("Next Bus = %08x\n", next_bus);
			printf("IO_BASE  = 0x%08lX\n", io_base);
			printf("MEM_BASE = 0x%08lX\n", mem_base);
			printf("PCI_DEV  = %ld\n", pci_dev);
			printf("    PCI_DEV.VENDOR ID       = 0x%04X\n",
				pci_dev_info[pci_dev].ven_id);
			printf("    PCI_DEV.DEVICE ID       = 0x%04X\n",
				pci_dev_info[pci_dev].dev_id);
			printf("    PCI_DEV.DEVICE CLASS    = 0x%02X\n",
				pci_dev_info[pci_dev].class);
			printf("    PCI_DEV.DEVICE SUBCLASS = 0x%02X\n",
				pci_dev_info[pci_dev].subclass);
			printf("    PCI_DEV.FUNCTION        = %ld\n",
				(ulong)pci_dev_info[pci_dev].func);
			printf("    PCI_DEV.DEVICE          = %ld\n",
				(ulong)pci_dev_info[pci_dev].dev);
			printf("    PCI_DEV.BUS             = %ld\n",
				(ulong)pci_dev_info[pci_dev].bus);
			printf("    PCI_DEV BAR Info:\n");
			for (x = 0; x < 6; x++){
				printf("          PCI_DEV.BAR_SIZE[%d] = 0x%08lX\n",
					x, pci_dev_info[pci_dev].bar_size[x]);
				printf("          PCI_DEV.BAR_MAP[%d]  = 0x%08lX\n",
					x, pci_dev_info[pci_dev].bar_map[x]);
			}
		}
	} // get info == 0
	if (pci_dev == 0){ 
		printf("\nPCI Probe Found No PCI Devices\n");
	}
	else {
		printf("\nPCI Probe Found The Following %ld PCI Device(s)\n", pci_dev);
		// dump the contents of pci_dev_info
		for (i = 0; i < pci_dev; i++){
			printf("PCI_DEV = %01d\n", i);
			printf("    PCI_DEV.VENDOR ID       = 0x%04X\n",
				pci_dev_info[i].ven_id);
			printf("    PCI_DEV.DEVICE ID       = 0x%04X\n",
				pci_dev_info[i].dev_id);
			printf("    PCI_DEV.DEVICE CLASS    = 0x%02X\n",
				pci_dev_info[i].class);
			printf("    PCI_DEV.DEVICE SUBCLASS = 0x%02X\n",
				pci_dev_info[i].subclass);
			printf("    PCI_DEV.BUS             = %ld\n",
				pci_dev_info[i].bus);
			printf("    PCI_DEV.DEVICE          = %ld\n",
				pci_dev_info[i].dev);
			printf("    PCI_DEV.FUNCTION        = %ld\n",
				pci_dev_info[i].func);
			printf("    PCI_DEV BAR Info:\n");
			for (x = 0; x < 6; x++){
				printf("        PCI_DEV.BAR_SIZE[%d] = 0x%08lX\n",
					x, pci_dev_info[i].bar_size[x]);
				printf("        PCI_DEV.BAR_MAP[%d]  = 0x%08lX\n",
					x, pci_dev_info[i].bar_map[x]);
			}
		} // for i < pci_dev
		printf("Final IO_BASE  = 0x%08lX\n", io_base);
		printf("Final MEM_BASE = 0x%08lX\n", mem_base);

	}  // if-else pci_dev
	return 0;
}

//--------------------------------------------------------------------------
// End of pci.c
