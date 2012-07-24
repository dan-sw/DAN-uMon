//==========================================================================
//
//      cs8900a.c
//
//      cs8900a Ethernet Code
//
// Author(s):    Michael Kelly, Cogent Computer Systems, Inc.
// Contributors: 
// Date:         05-26-2002
// Description:  This file contains intialization and support routines 
//               for the cs8900a ethernet controller found on some CSB's.
//
//--------------------------------------------------------------------------

#include "config.h"
#if INCLUDE_ETHERNET
#include "cpuio.h"
#include "stddefs.h"
#include "genlib.h"
#include "ether.h"
#include "cs8900a.h"


//--------------------------------------------------------------------------
// cs8900a_page_rd()
//
// This function does an inderect read of the desired register in the cs8900a
ushort
cs8900a_page_rd(ushort reg)
{
	CS_PADD = reg;
	return CS_PDAT;
}

//--------------------------------------------------------------------------
// cs8900a_page_wr()
//
// This function does an indirect write to the desired register in the cs8900a
void
cs8900a_page_wr(ushort reg, ushort data)
{
	CS_PADD = reg;
	CS_PDAT = data;
}

//--------------------------------------------------------------------------
// cs8900a_init()
//
//  This code performs the following steps:
//
//	1. Verify the presence of the CS8900A at CS8900A_BASE_ADDRESS 
//	2. Force a software reset.
//  3. Wait until reset completes or timeouts
//  4. Enable rx of broadcast and frames that match our mac address
//  5. Disable all interrupts
//  6. Enable TX and RX and set 10Base-T mode
//  7. Set mac address from BinEnetAdd
//	8. Return
//
int
cs8900a_init(void)
{
	volatile ushort temp16;
	
	// reset the cs8900a
	cs8900a_reset();
	 	
	// see if we can ID the device
	temp16 = cs8900a_page_rd(CS_ChipID);
	if(temp16 != 0x630E){
		printf("Bad CS8900A ID = %04x!\n", temp16);
		return -1;
	}

	// clear the interrupt enables for the receiver
	cs8900a_page_wr(CS_RxCFG, 0);	// no interrupts on rx events

	// set the test modes
//	cs8900a_page_wr(CS_TestCtl, CS_TestCtl_FDX);			// good frames

	// set the rx filtering
	cs8900a_page_wr(CS_RxCTL, CS_RxCTL_RxOK			// good frames
							| CS_RxCTL_MAC			// frames for our mac
//							| CS_RxCTL_Promiscuous
							| CS_RxCTL_Broadcast);	// broadcast frames

	// clear the interrupt enables for the transmitter
	cs8900a_page_wr(CS_TxCFG, 0);	// no interrupts on tx events

	// clear the interrupt enables for the buffer
	cs8900a_page_wr(CS_BufCFG, 0);	// no interrupts on buffer events

	// set the mac address
	cs8900a_set_mac();

	// enable the transmitter and receiver, this also selects 10Base-T mode
	cs8900a_page_wr(CS_LineCTL, CS_LineCTL_Rx
							  | CS_LineCTL_Tx);

	// we're done!	
	return 0;	
}

//--------------------------------------------------------------------------
// cs8900a_reset()
//
// This function sets the CS_SelfCtl_Reset bit in CS_SelfCTL then
// waits for CS_SelfStat_InitD in CS_SelfStat to go true
int
cs8900a_reset(void)
{

	volatile int timeout = CS_TIMEOUT;
	volatile ushort temp16;

	cs8900a_page_wr(CS_SelfCtl, CS_SelfCtl_Reset);
	while (timeout){
		temp16 = cs8900a_page_rd(CS_SelfStat);
		if (temp16 & CS_SelfStat_InitD) break;
		timeout--;
	}
	if (timeout == 0){
		printf("CS8900A Reset Time Out Error, Status = %04x!\n", temp16);
		return -1;
	}
	return(0);
}

//--------------------------------------------------------------------------
// cs8900a_set_mac()
//
// This function puts the mac address from BinEnetAddr into the CS8900a
void
cs8900a_set_mac(void)
{
	volatile int i;
	
	for (i = 0; i < 6; i += 2){
		cs8900a_page_wr(CS_MAC + i,BinEnetAddr[i] | (BinEnetAddr[i + 1] << 8));
	}	

	// set the logical address mask to accept all addresses
    for (i = 0;  i < 8;  i += 2) {
		cs8900a_page_wr(CS_LAF + i, 0xFFFF);
	}
	return;
}

//--------------------------------------------------------------------------
// cs8900a_tx()
//
// This function puts the tx buffer into the cs8900a as follows:
//
// 1. Write the Transmit Command
// 2. Write the Transmit Frame Length
// 3. Wait for CS_BusStat_TxRDY to go true
// 4. Copy txbuf into cs8900a
// 5. Return
//
int
cs8900a_tx(uchar *txbuf, ulong length)
{
	volatile int i, timeout;
	volatile ushort temp16;
	volatile uchar *from_p;
	volatile uchar temp8;

	if (EtherVerbose == SHOW_ALL) {
		printf("CS8900a: Xmit Buffer Address = 0x%08lx, Length = 0x%04lx.\n",
			(ulong)txbuf, length);
	}

	// clear any left over status before we can send
	// just reading CS_TER does it 
//	temp16 = cs8900a_page_rd(CS_TER) & 0xffc0;
//	if (temp16 & ~CS_TER_TxOK){ 	// any error?
//		if (EtherVerbose == SHOW_ALL) {
//			printf("CS8900a: Prior Transmit Failed with Status = %04x.\n", temp16);
//		}
//	}

	// write the TX Command and Length
	CS_TxCMD = CS_TxCmd_TxStart_Full;
	CS_TxLEN = length;

	// Now check the CS_BusStat_TxRDY bit
    timeout = CS_TIMEOUT;
	while (timeout){
		temp16 = cs8900a_page_rd(CS_BusStat);
		if (temp16 & CS_BusStat_TxRDY) break;
		timeout--;
	}
	if (timeout == 0){
		printf("CS8900a: Transmit Time Out Error, Status = %04x!\n", temp16);
		return -1;
	}

	from_p = txbuf;

	// write the packet to the cs8900a transmit buffer
	i = 0;
	while(length){
		if (length == 1){
			CS_RTDATA = (*from_p++ & 0xff);
			length--;
		}
		else {
			temp8 = *from_p++;
			CS_RTDATA = ((*from_p++ << 8) | temp8);
		length = length - 2;
		}
	}
	
	if (EtherVerbose == SHOW_ALL) {
		printf("CS8900a: Transmit Complete.\n");
	}
	return 0;
}

//--------------------------------------------------------------------------
// cs8900a_rx()
//
// This function checks to see if the cs8900a has a receive buffer
// ready.  If so, it copies it into the buffer pointed to by pktbuf
//
int
cs8900a_rx(uchar *pktbuf)
{
	volatile int i;
	volatile ushort temp16, length;

	// Check to see if we have an rx frame of any kind
	temp16 = cs8900a_page_rd(CS_RER) & 0xffc0;
	if (temp16 == 0){
		return 0;	// no frame so just leave
	}

	if (EtherVerbose == SHOW_ALL)  {
		printf("CS8900a: Receiving Packet Status = 0x%04x.\n", temp16);
	}

	// did we get an error?  if so tell the cs8900a to skip it, then leave
	if ((temp16 & CS_RER_RxOK) == 0){
		cs8900a_page_wr(CS_RxCFG, CS_RxCFG_Skip1);	// skip bad frame
		if (EtherVerbose == SHOW_ALL) {
			printf("CS8900a: Receive Packet Error Status = 0x%04x.\n",
				(temp16 & 0xffc0));
		}
		return 0;
	}
	// now get the frame
	// first word in the receive buffer is the status.
	// the second word is the length.
	temp16 = CS_RTDATA;
	if (EtherVerbose == SHOW_ALL) {
		printf("CS8900a: Receiving Packet Status again = 0x%04x.\n", temp16 & 0xffc0);
	}

	length = CS_RTDATA;
	if ((length >= 1600) || (length == 0)) {
		if (EtherVerbose == SHOW_ALL)  {
			printf("CS8900a: Rcv pkt addr = 0x%08lx, Bad len = 0x%04x.\n",
				(ulong)pktbuf, length);
			return 0;
		}
	}

	i = length;
	if (EtherVerbose == SHOW_ALL){
		printf("CS8900a: Rcv pkt addr = 0x%08lx, Length = 0x%04x.\n",
			(ulong)pktbuf, i);
	}
	while(1){
		temp16 = CS_RTDATA;
		*pktbuf++ = temp16 & 0xff;
		i--;
		if (i == 0) break;
		*pktbuf++ = (temp16 & 0xff00 ) >> 8;
		i--;
		if (i == 0) break;
	}

//	to_p16 = (ushort *)pktbuf;
//	i = length;
//	if (EtherVerbose == SHOW_ALL){
//		printf("CS8900a: Receiving Packet Address = 0x%08x, Length = 0x%04x.\n", to_p16, i);
//	}
//	while(i){
//		if (i == 1){
//			to_p8 = (uchar *)to_p16;
//			temp8 = CS_RTDATA;
//			*to_p8 = temp8;
//			i--;
//		}
//		else{
//			temp16 = CS_RTDATA;
//			*to_p16++ = SWAP16(temp16);
//			i -= 2;
//		}
//	}

	if (EtherVerbose == SHOW_ALL){
		printf("CS8900a: Receive Packet Complete.\n");
	}
	return length;
}
				   	

#endif // INCLUDE_ETHERNET

