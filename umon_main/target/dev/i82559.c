//==========================================================================
//
//      i82559.c
//
//      i82559 Ethernet Code
//
// Author(s):    Michael Kelly, Cogent Computer Systems, Inc.
// Contributors: 
// Date:         05-26-2002
// Description:  This file contains PCI intialization and support routines 
//               for the i82559 PCI ethernet controller found on some
//				 CSB's.
//
//--------------------------------------------------------------------------

#include "config.h"
#include "cpuio.h"
#include "stddefs.h"
#include "genlib.h"
#include "pci.h"
#include "cpu_pci.h"
#include "ether.h"
#include "i82559.h"

extern void udelay(vulong);
extern ushort pci_cfg_read_word (vulong bus, vulong dev_func, vulong offset);
extern void pci_cfg_write_word (vulong bus, vulong dev_func, vulong offset, vushort data);

extern vulong sh_mem_base;			// shared memory base address for all PCI devices
extern vulong pci_dev;
extern pci_device_structure pci_dev_info[PCI_DEV_MAX];

#if INCLUDE_ETHERNET

// global 82559 pointers
vulong i82559_scb;			// Status/Command Block (internal to chip)
vulong i82559_psb;			// Port Status Block (in shared memory)
vulong i82559_cbl;			// Command Block List (in shared memory)
vulong i82559_tfd_base;		// Transmit Frame Descriptor base address (in shared memory)
vulong i82559_rfd_base;		// Receive Frame Descriptor base address (in shared memory)
vulong i82559_found;		// clear if no i82559 is found

//--------------------------------------------------------------------------
// i82559_init()
//
//  This code performs the following steps:
//
//	1. Find the first 82559 or 82559er.
//	2. Enable it for IO, MEM and Mastering.
//  3. Allocate space for the shared memory structures.
//  4. Perform Self Test.
//	5. Issue a Port Selective Reset to insure the i82559 is
//     in the idle state.
//
//  Notes:
//  1. We use a predefined "window" for all shared memory accesses
//     between the i82559 and the CPU.  The base address of this
//     window is defined by i82559_SM_BASE.
//  2. A simple static pointer is maintained to "allocate" memory
//     to the functions.  This pointer is sh_mem_base.
//
//
ulong i82559_init(void)
{
	int i, timeout;
	vushort temp16;
	vulong dev, func, bus, temp32;
	
	i82559_scb = 0;
	i82559_found = 0;
	// scan the pci_dev_info structure for the first 82559 or 82559er
	for (i = 0; i < pci_dev; i++){
		if ((pci_dev_info[i].ven_id == I82559_VENID) &&
			((pci_dev_info[i].dev_id == I82559_DEVID) ||
			 (pci_dev_info[i].dev_id == I82559ER_DEVID))){

			// get the MEM base from BAR 0  (we don't use the IO base from BAR 1)
			i82559_scb = pci_dev_info[i].bar_map[0];

			// enable the 82559
			bus = pci_dev_info[i].bus;
			dev = pci_dev_info[i].dev;
			func = pci_dev_info[i].func;
			temp16 = pci_cfg_read_word (bus, PCI_MAKE_DEV_FUNC(dev, func), PCI_CFG_CMD_REG);
			temp16 = temp16 | PCI_CFG_CMD_MEM | PCI_CFG_CMD_MASTER;
			pci_cfg_write_word (bus, PCI_MAKE_DEV_FUNC(dev, func), PCI_CFG_CMD_REG, temp16);

			printf("Intel 82559 or 82559er Found in pci_dev_info\n");
			printf("PCI DEV %01d, @ 0x%08lx\n", i, i82559_scb);
			break;
		} // vendor and device id matched
	}  // pci_dev_info loop

	if(i82559_scb == 0){
		printf("Intel 82559 or 82559er not Found in pci_dev_info\n");
		return -1;
	}

	// grab space for the Port Status Block first since it must be 16 byte aligned		
	// all others are 32bit aligned		
	i82559_psb = sh_mem_base;		
	sh_mem_base = sh_mem_base + 1024;	// dump commands can return up to 600 bytes

	// Now get space enough for any Command Block List Command
	i82559_cbl = sh_mem_base;		
	sh_mem_base = sh_mem_base + 1024;

	// Reserve space for the Transmit and Receive Frame Descriptors		
	// allow for oversize frame reception
	i82559_rfd_base = sh_mem_base;		
	sh_mem_base = sh_mem_base + (RBUFCNT * RBUFSIZE);		

	i82559_tfd_base = sh_mem_base;		
	sh_mem_base = sh_mem_base + (XBUFCNT * XBUFSIZE);		

	// issue a self test to the i82559		
	WR_SM32(i82559_psb, 0xffffffff);		
	WR_SM32(i82559_psb + PORT_ST_STAT, 0xffffffff);		
	WR_SM32(i82559_scb + SCB_PORT, ((i82559_psb & 0x0fffffff) | SCB_PORT_ST));		

	// wait for i82559 to write the self test results
    timeout = 10000;
	while(timeout){
		// delay before each check
		udelay(10);
		RD_SM16(i82559_psb + PORT_ST_STAT, temp16);
    	if(temp16 != 0xffff) break; 
   		timeout--;
    }
	if (timeout == 0){
		printf("i82559 SelfTest Timed Out\n");	
		return SCB_PORT_ST_ERR;	
	}
	// Any bit status set means an error	
	RD_SM16(i82559_psb + PORT_ST_STAT, temp16);	
	if (temp16 & (PORT_ST_GEN | PORT_ST_DIAG | PORT_ST_REG | PORT_ST_ROM)){	
		printf("i82559 SelfTest Failed, Status = 0x%04x\n",temp16);	
		return SCB_PORT_ST_ERR;	
	}
	RD_SM32(i82559_psb, temp32);
	printf("i82559 SelfTest Passed, ROM Checksum = 0x%08lx\n", temp32);	

	// reset the i82559
	i82559_reset();
	 	
	// configure the i82559 via the Command Block List - Non-Promiscuous
	temp32 = i82559_cbl_cfg(0);
	if (temp32) return temp32;
			
	// set the mac address
	temp32 = i82559_cbl_ias(BinEnetAddr);
	if (temp32) return temp32;

	// setup a linked list of rx buffers
	i82559_rx_init();
			
	// we're done!	
	i82559_found = 1;
	return 0;	
}

//--------------------------------------------------------------------------
// i82559_cbl_cfg()
//
// This function sends a configuration block to the i82559.  It can be
// called with promiscuous modes enabled (promisc = nonzero) or
// disabled (promisc = zero).  It returns non-zero if the command
// failed to complete within the timeout or if it completed without
// the OK bit set.

ulong i82559_cbl_cfg(vulong promisc)
{
	vulong timeout;
	vushort temp16;
	vulong temp32;

	// Fill the i82559 Command Block List
	WR_SM16(i82559_cbl + CBL_STAT, 0);
	WR_SM16(i82559_cbl + CBL_CMD, (CBL_CMD_CFG | CBL_CMD_EL));
	WR_SM32(i82559_cbl + CBL_LINK, (i82559_cbl & 0x0fffffff));
	WR_SM8(i82559_cbl + CBL_DATA +  0, 0x13);
	WR_SM8(i82559_cbl + CBL_DATA +  1, 0x0c);
	WR_SM8(i82559_cbl + CBL_DATA +  2, 0x00);
	WR_SM8(i82559_cbl + CBL_DATA +  3, 0x00);
	WR_SM8(i82559_cbl + CBL_DATA +  4, 0x00);
	WR_SM8(i82559_cbl + CBL_DATA +  5, 0x00);
	WR_SM8(i82559_cbl + CBL_DATA +  6, 0x32 | (promisc ? 0x80 : 0x00));
	WR_SM8(i82559_cbl + CBL_DATA +  7, 0x00 | (promisc ? 0x01 : 0x00));
	WR_SM8(i82559_cbl + CBL_DATA +  8, 0x01);
	WR_SM8(i82559_cbl + CBL_DATA +  9, 0x00);
	WR_SM8(i82559_cbl + CBL_DATA + 10, 0x28);
	WR_SM8(i82559_cbl + CBL_DATA + 11, 0x00);
	WR_SM8(i82559_cbl + CBL_DATA + 12, 0x60);
	WR_SM8(i82559_cbl + CBL_DATA + 13, 0x00);
	WR_SM8(i82559_cbl + CBL_DATA + 14, 0x00);
	WR_SM8(i82559_cbl + CBL_DATA + 15, 0x80 | (promisc ? 0x01 : 0x00));
	WR_SM8(i82559_cbl + CBL_DATA + 16, 0x00);
	WR_SM8(i82559_cbl + CBL_DATA + 17, 0x40);
	WR_SM8(i82559_cbl + CBL_DATA + 18, 0x02);

	// wait for the CU to be idle
	temp32 = i82559_wait_for_cu_cmd();
	if (temp32) return temp32;

	// issue the command to the i82559
	WR_SM32(i82559_scb + SCB_PTR, (i82559_cbl & 0x0fffffff));
	RD_SM32(i82559_scb + SCB_PTR, temp32);
	WR_SM16(i82559_scb + SCB_CMD, SCB_CU_START | SCB_IM_M);
	RD_SM16(i82559_scb + SCB_CMD, temp16);

	// Now check the CBL Status word
    timeout = 10000;
	while(timeout){
		// delay before each check
		udelay(10);
		RD_SM16(i82559_cbl + CBL_STAT, temp16);
		if (temp16 & CBL_STAT_C){
			if (temp16 & CBL_STAT_OK){
				return 0;
			}
			else {
				printf("i82559 CBL Configure Command Returned Not OK\n");
				return CBL_CMD_CFG_ERR;
			}
		}
   		timeout--;
    }
	if (timeout == 0){
		printf("i82559 CBL Configure Command Timed Out\n");
		return CBL_CMD_CFG_TIMEOUT;	
	}
	return 0;
}

//--------------------------------------------------------------------------
// i82559_cbl_ias()
//
// This function sends a mac address to the i82559.  It returns
// non-zero if the command failed to complete within the timeout
// or if it completed without the OK bit set.

ulong i82559_cbl_ias(uchar *mac_addr)
{
	vulong timeout;
	vushort temp16;
	vulong temp32;

	// Fill the i82559 Command Block List
	WR_SM16(i82559_cbl + CBL_STAT, 0);
	WR_SM16(i82559_cbl + CBL_CMD, (CBL_CMD_IAS | CBL_CMD_EL));
	WR_SM32(i82559_cbl + CBL_LINK, (i82559_cbl & 0x0fffffff));
	WR_SM8(i82559_cbl + CBL_DATA +  0, mac_addr[0]);
	WR_SM8(i82559_cbl + CBL_DATA +  1, mac_addr[1]);
	WR_SM8(i82559_cbl + CBL_DATA +  2, mac_addr[2]);
	WR_SM8(i82559_cbl + CBL_DATA +  3, mac_addr[3]);
	WR_SM8(i82559_cbl + CBL_DATA +  4, mac_addr[4]);
	WR_SM8(i82559_cbl + CBL_DATA +  5, mac_addr[5]);
	WR_SM8(i82559_cbl + CBL_DATA +  6, 0);
	WR_SM8(i82559_cbl + CBL_DATA +  7, 0);

	// wait for the CU to be idle
	temp32 = i82559_wait_for_cu_cmd();
	if (temp32) return temp32;

	// issue the command to the i82559
	WR_SM32(i82559_scb + SCB_PTR, (i82559_cbl & 0x0fffffff));
	WR_SM16(i82559_scb + SCB_CMD, SCB_CU_START | SCB_IM_M);

	// Now check the CBL Status word
    timeout = 10000;
	while(timeout){
		// delay before each check
		udelay(10);
		RD_SM16(i82559_cbl + CBL_STAT, temp16);
		if (temp16 & CBL_STAT_C){
			if (temp16 & CBL_STAT_OK){
				return 0;
			}
			else {
				printf("i82559 CBL Configure Command Returned Not OK\n");
				return CBL_CMD_CFG_ERR;
			}
		}
   		timeout--;
    }
	if (timeout == 0){
		printf("i82559 CBL Configure Command Timed Out\n");
		return CBL_CMD_CFG_TIMEOUT;	
	}
	return 0;
}

//--------------------------------------------------------------------------
// i82559_send()
//
// This function tells the i82559 to send the tx buffer pointed to by tx_buf
//
ulong i82559_send(uchar *txbuf, vulong length)
{
	vulong i, timeout;
	vushort temp16;
	vuchar *from_p, *to_p;

	if (i82559_found == 0) return 0;

	// build the header
	WR_SM16(i82559_tfd_base + CBL_STAT, 0);
	WR_SM16(i82559_tfd_base + CBL_CMD, (CBL_CMD_TX | CBL_CMD_EL));
	WR_SM32(i82559_tfd_base + CBL_LINK, (i82559_tfd_base & 0x0fffffff));
	WR_SM32(i82559_tfd_base + CBL_TxCB_TBD, 0xffffffff);
	WR_SM16(i82559_tfd_base + CBL_TxCB_CNT, (length | CBL_TxCB_CNT_EOF));
	WR_SM8(i82559_tfd_base + CBL_TxCB_THRESH, 16);
	WR_SM8(i82559_tfd_base + CBL_TxCB_TBD_NUM, 0);

	from_p = txbuf;
	to_p = (vuchar *)(i82559_tfd_base + CBL_TxCB_DATA);

	// copy the data into the shared memory area
	for (i = 0; i < length; i++){
		WR_SM8(to_p++, *from_p++);
	}

	// wait for the CU to be idle
	i82559_wait_for_cu_cmd();

	// issue the command to the i82559
	WR_SM32(i82559_scb + SCB_PTR, (i82559_tfd_base & 0x0fffffff));
	WR_SM16(i82559_scb + SCB_CMD, SCB_CU_START | SCB_IM_M);

	// wait for the CU to be idle
	i82559_wait_for_cu_cmd();

	// Now check the CBL Status word
    timeout = 10000;
	while(timeout){
		// delay before each check
		udelay(10);
		RD_SM16(i82559_cbl + CBL_STAT, temp16);
		if (temp16 & CBL_STAT_C){
			if (temp16 & CBL_STAT_OK){
				return 0;
			}
			else {
				return CBL_CMD_TX_ERR;
			}
		}
   		timeout--;
    }
	if (timeout == 0){
		return CBL_CMD_TX_TIMEOUT;	
	}
	return 0;
}

//--------------------------------------------------------------------------
// i82559_rx_get()
//
// This function checks the list of rx buffers to see if any of them are
// ready to be received.  If so, it copies the first ready buffer to
// pktbuf.  It then resumes the RU if it was suspended and returns the
// packet length.  This function also acts to start the receiver when
// called for the first time.
//
ushort i82559_rx_get(vuchar *pktbuf)
{
	vulong i, x, rfd;
	vushort temp16, rx_cmd, length;
	vuchar *from_p, *to_p;

	if (i82559_found == 0) return 0;

	rfd = i82559_rfd_base;

	// scan the rx list looking for ready buffers
	for (i = 0; i < RBUFCNT; i++){
		RD_SM16(rfd + RFD_STAT, temp16);
		// check the OK and Complete bits first
		if(temp16 & (RFD_STAT_C | RFD_STAT_OK)){
			// get the size 
			RD_SM16(rfd + RFD_CNT, length);
			// check the data valid flag
			if (length & RFD_CNT_EOF){
				length = length & RFD_CNT_MASK;
				// copy the data into pktbuf
				from_p = (vuchar *)(rfd + RFD_DATA);
				to_p = (vuchar *)(pktbuf);
				for(x = 0; x < length; x++){
					RD_SM8(from_p++, *to_p++);
				}
				// fixup this rfd for re-use
				WR_SM16(rfd + RFD_STAT, 0);
				WR_SM16(rfd + RFD_CNT, 0);
				break;
			}
		}
		rfd = rfd + RBUFSIZE;
		length = 0;
	}
	// Check the state of the RU before we leave
	RD_SM16(i82559_scb + SCB_STAT, temp16);
	temp16 = (temp16 & SCB_STAT_RU_MASK) >> SCB_STAT_RU_SHIFT;

	// Issue an RU command appropriate to the state.
	switch(temp16) {
		case RUS_IDLE: rx_cmd = (SCB_RU_START | SCB_IM_M); break;
		case RUS_SUSP: rx_cmd = (SCB_RU_RESUME | SCB_IM_M); break;
		case RUS_NORES: rx_cmd = (SCB_RU_START | SCB_IM_M); break;
		case RUS_SUSP_NO_RBD: rx_cmd = (SCB_RU_RESUME_RBD | SCB_IM_M); break;
		case RUS_NO_RES_NO_RBD: rx_cmd = (SCB_RU_RESUME_RBD | SCB_IM_M); break;
		case RUS_READY_NO_RBD: rx_cmd = (SCB_RU_START | SCB_IM_M); break;
		default: rx_cmd = (SCB_RU_NOP | SCB_IM_M);
	}
	// make sure the RU is ready to receive a command
	i82559_wait_for_ru_cmd();

	// issue the command
	WR_SM32(i82559_scb + SCB_PTR, (i82559_rfd_base & 0x0fffffff));
	WR_SM16(i82559_scb + SCB_CMD, rx_cmd);
	return length;
}
					   	
//--------------------------------------------------------------------------
// i82559_rx_init()
//
// This function builds/rebuilds the rx buffer list, then enables the i82559
// for receive
//
void i82559_rx_init()
{
	vulong i, rfd;

	rfd = i82559_rfd_base;

	for (i = 0; i < RBUFCNT - 1; i++){
		WR_SM16(rfd + RFD_CMD, 0); 
		WR_SM16(rfd + RFD_STAT, 0); 
		WR_SM32(rfd + RFD_LINK, ((rfd + RBUFSIZE) & 0x0fffffff)); 
		WR_SM16(rfd + RFD_CNT, 0); 
		WR_SM16(rfd + RFD_SIZE, RBUFSIZE); 
		rfd = rfd + RBUFSIZE;
	}
	// mark the last one as End Of List
	WR_SM16(rfd + RFD_CMD, RFD_CMD_EL); 
	WR_SM16(rfd + RFD_STAT, 0); 
	WR_SM32(rfd + RFD_LINK, ((rfd + RBUFSIZE) & 0x0fffffff)); 
	WR_SM16(rfd + RFD_CNT, 0); 
	WR_SM16(rfd + RFD_SIZE, RBUFSIZE); 
}

//--------------------------------------------------------------------------
// i82559_wait_for_cu_cmd()
//
// This function polls the SCB Status word waiting for the CU
// command to be accepted, then for the CU status to go idle
//
ulong i82559_wait_for_cu_cmd(void)
{
	vulong timeout;
	vushort temp16;

    timeout = 10000;
	while(timeout){
		// delay before each check
		udelay(10);
		RD_SM16(i82559_scb + SCB_CMD, temp16);
		temp16 = temp16 & SCB_CMD_CU_MASK;
    	if(temp16 == 0) break; 
   		timeout--;
    }
	if(timeout == 0){
		// Issue a selective reset to the i82559 and check again
		WR_SM32(i82559_scb + SCB_PORT, SCB_PORT_SEL_RST);
		udelay(10);
  	    timeout = 10000;
		while(timeout){
			// delay before each check
			udelay(10);
			RD_SM16(i82559_scb + SCB_CMD, temp16);
			temp16 = temp16 & SCB_CMD_CU_MASK;
	    	if(temp16 == 0) break; 
	   		timeout--;
	    }
		if (timeout == 0){
			return CU_CMD_TIMEOUT;	
		}
	}

	if(timeout == 0){
		// Issue a selective reset to the i82559 and check again
		WR_SM32(i82559_scb + SCB_PORT, SCB_PORT_SEL_RST);
		udelay(10);
  	    timeout = 10000;
		while(timeout){
			// delay before each check
			udelay(10);
			RD_SM16(i82559_scb + SCB_CMD, temp16);
			temp16 = temp16 & SCB_CMD_CU_MASK;
	    	if(temp16 == 0) break; 
	   		timeout--;
	    }
		if (timeout == 0){
			return CU_STAT_TIMEOUT;	
		}
	}

	return 0;
}

//--------------------------------------------------------------------------
// i82559_wait_for_ru_cmd()
//
// This function polls the SCB Status word waiting for the RU
// command to be accepted
//
ulong i82559_wait_for_ru_cmd(void)
{
	vulong timeout;
	vushort temp16;

    timeout = 100000;
	while(timeout){
		// delay before each check
		udelay(10);
		RD_SM16(i82559_scb + SCB_CMD, temp16);
		temp16 = temp16 & SCB_CMD_RU_MASK;
    	if(temp16 == 0) break; 
   		timeout--;
    }
	if (timeout == 0){
		return RU_CMD_TIMEOUT;	
	}

	return 0;
}

//--------------------------------------------------------------------------
// i82559_reset()
//
// This function issues a selective reset, then a full reset to the i82559.
// It then clears the CU and RU base pointers
//
void i82559_reset()
{

//	if (i82559_found == 0) return;

	// First issue a selective reset to the i82559 to force the i82559
	// off the pci bus
	WR_SM32(i82559_scb + SCB_PORT, SCB_PORT_SEL_RST);
	udelay(10);
  
    // Do hard reset now that the controller is off the PCI bus.
	WR_SM32(i82559_scb + SCB_PORT, SCB_PORT_RST);
    udelay(10);

	// zero the base addresses in the Command Unit and the Receive Unit.  
	WR_SM32(i82559_scb + SCB_PTR, 0x00000000);
	WR_SM16(i82559_scb + SCB_CMD, SCB_CU_BASE_ADD | SCB_IM_M);
	udelay(10);

	// wait for the RU to be ready
	i82559_wait_for_ru_cmd();

	WR_SM32(i82559_scb + SCB_PTR, 0x00000000);
	WR_SM16(i82559_scb + SCB_CMD, SCB_RU_BASE_ADD | SCB_IM_M);
	udelay(10);

}


#endif // INCLUDE_ETHERNET
