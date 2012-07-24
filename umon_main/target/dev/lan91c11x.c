//==========================================================================
//
//      lan91c11x.c
//
//      lan91c11x Ethernet Code
//
// Author(s):    Michael Kelly, Cogent Computer Systems, Inc.
// Contributors: 
// Date:         05-26-2002
// Description:  This file contains intialization and support routines 
//               for the SMC LAN91c111 and 113 ethernet controllers found
//               on some CSB's.  Adapted from Ed Sutters original smc91c9x.c
//               The PHY code was modeled after the Linux lan91c11x.c code
//               with modifications.
//
//--------------------------------------------------------------------------


#include "config.h"
#if INCLUDE_ETHERNET
#include "cpuio.h"
#include "stddefs.h"
#include "genlib.h"
#include "ether.h"
#include "lan91c11x.h"

static int EtherRXOVRNCnt, EtherALGNERRCnt, EtherBADCRCCnt;
static int EtherTOOSHORTCnt, EtherTOOLONGCnt, EtherWFATMTCnt;
extern void enreset(), eninit();

ushort lan91c11x_phy(uchar phyreg, ushort phydata, int phy_dir);
int lan91c11x_phy_cfg(void);


//--------------------------------------------------------------------------
// lan91c11x_rd()
//
// This function does a read of the desired register in the lan91c11x
// using the upper 4 bits as the bank where the register is located
ushort lan91c11x_rd(ushort reg){


        // first set the bank register for this access
        LAN_REG(LAN_BANK) = ((reg & 0x00f0) >> 4);


        // now return the data from the desired register
        return LAN_REG((reg & 0x000f));
}


//--------------------------------------------------------------------------
// lan91c11x_wr()
//
// This function does a write to the desired register in the lan91c11x
// using the upper 4 bits as the bank where the register is located
void lan91c11x_wr(ushort reg, ushort data){


        // first set the bank register for this access
        LAN_REG(LAN_BANK) = ((reg & 0x00f0) >> 4);


        // now write the data to the desired register
        LAN_REG((reg & 0x000f)) = data;
}


//--------------------------------------------------------------------------
// lan91c11x_init()
//
//  This code performs the following steps:
//
//      1. Verify the presence of the lan91c11x
//      2. setup the operating mode
//  3. Set the ethernet address from BinEnetAdd
//  4. Disable all interrupts
//  5. Enable the transmitter and receiver
//      6. Return
//
int lan91c11x_init(void)
{
        int i;


//		printf("lan91c11x Init.\n");

        /* Initialize device-driver counters: */
        EtherRXOVRNCnt = 0;
        EtherALGNERRCnt = 0;
        EtherBADCRCCnt = 0;
        EtherTOOLONGCnt = 0;
        EtherTOOSHORTCnt = 0;
        EtherWFATMTCnt = 0;
        
        // first see if we can ID the device
        if((lan91c11x_rd(LAN_REV) & LAN_REV_CHIP_MASK) != LAN_REV_LAN91C11x){
                printf("Bad lan91c11x ID = %04x!\n", lan91c11x_rd(LAN_REV) & LAN_REV_CHIP_MASK);
                return -1;
        }


        // setup the operating mode
        lan91c11x_wr(LAN_TCR, 0);       // note - we pad the length in sendBuffer() in etherdev.c
        lan91c11x_wr(LAN_RCR, LAN_RCR_STRIP_CRC);       // strip CRC from rx packets before we get them
        lan91c11x_wr(LAN_CONFIG, LAN_CONFIG_NO_WAIT | LAN_CONFIG_EPH_PWREN);    // fast bus mode, power on


        // enable auto-release mode for transmitted packets
        lan91c11x_wr(LAN_CTRL, lan91c11x_rd(LAN_CTRL) | LAN_CTRL_AUTO_REL);


        // enable auto-negotiate mode.  Note that the PHY Control register
        // ANEG bit must also be set, which it is on reset.  Since we don't 
        // really care what mode we end up connecting in, we set the ANEG bit
        // and then move one.  We also set the LED's here
        lan91c11x_wr(LAN_RPCR, LAN_RPCR_LEDA_LINK       // The green led = any link, 10 or 100Mbit
                                                 | LAN_RPCR_LEDB_TXRX   // The yellow led = activity
                                                 | LAN_RPCR_ANEG);              // get whatever the link gives us!


        monDelay(2);


        // Store the ethernet address.
        lan91c11x_wr(LAN_IA01, *(ushort *)BinEnetAddr);
        lan91c11x_wr(LAN_IA23, *(ushort *)&BinEnetAddr[2]);
        lan91c11x_wr(LAN_IA45, *(ushort *)&BinEnetAddr[4]);


        // Disable Interrupts
        lan91c11x_wr(LAN_INT, 0x0000);


        // Enable the transmitter and receiver
        lan91c11x_wr(LAN_TCR, lan91c11x_rd(LAN_TCR) | LAN_TCR_TXENA);
        lan91c11x_wr(LAN_RCR, lan91c11x_rd(LAN_RCR) | LAN_RCR_RXEN);


        // configure the PHY and attempt auotnegotiate
        if (lan91c11x_phy_cfg())
        {
                // dump out the phy registers if it failed
                for (i = 0; i < 32; i++)
                {
                        printf("lan91c11x PHY Register %d = 0x%04x\n", i, lan91c11x_phy(i, i, 0));
                }
        }


        // done
        return 0;       
}


//--------------------------------------------------------------------------
// lan91c11x_reset()
//
// Force a soft reset of the lan91c11x, then reset the mmu and fifos
void lan91c11x_reset(void)
{
        /* Reset the device: */
        lan91c11x_wr(LAN_RCR, LAN_RCR_SOFT_RST);
        monDelay(2);
        lan91c11x_wr(LAN_RCR, 0);
        monDelay(5);


        /* Reset MMU and TX fifos: */
        lan91c11x_wr(LAN_MMU_CMD, LAN_MMU_CMD_RESETMMU);
        lan91c11x_wr(LAN_MMU_CMD, LAN_MMU_CMD_RESETTXFIFOS);
}


//--------------------------------------------------------------------------
// lan91c11x_tx()
//
// This function puts the tx buffer into the lan91c11x as follows:
//
// 1. Write the Transmit Command
// 2. Write the Transmit Frame Length
// 3. Wait for CS_BusStat_TxRDY to go true
// 4. Copy txbuf into lan91c11x
// 5. Return
//
int lan91c11x_tx(uchar *txbuf, short len)
{
        ushort  temp16, *sptr, i;
        ulong   timeout;


        if (EtherVerbose == SHOW_ALL) {
                printf("lan91c11x: Transmit Buffer Address = 0x%08lx, Length = 0x%04x.\n", (ulong)txbuf, len);
        }


        // convert the length into the number of 256 byte
        // pages we need to request from the lan91c11x.
        temp16 = (ushort)(len >> 8);


        // don't request more the 5 pages (1280 bytes), since 
        // that's bigger than any legal frame should be.
        if (temp16 > 5) {
                printf("sendBuffer(): len too long\n");
                return(-1);
        }


        // Allocate the space and wait for the allocation to succeed.
        lan91c11x_wr(LAN_MMU_CMD, LAN_MMU_CMD_ALLOCTXMEM | temp16);


        // loop until the allocate interrupt bit is set or we exhaust the timeout
        for(timeout = 10000; timeout > 0; timeout--) {
                if (lan91c11x_rd(LAN_INT) & LAN_INT_ALLOC)
                        break;
        }
        // we should always succeed within the timeout period 
        // since we are only sending one packet at a time.  If
        // we don't it means we have either a chip problem or 
        // problems on the wire that are preventing the transmission
        if (timeout == 0) {
                printf("sendBuffer(%d) wait-for-alloc timeout\n",len);
                // keep track of how often this happens - good for diag purposes
                EtherWFATMTCnt++;
                // restart the chip
                EtherdevStartup(0);
                return(-1);
        }

        // give the LAN back the pointer to the memory it just allocated to us
        temp16 = lan91c11x_rd(LAN_PNRARR);
        lan91c11x_wr(LAN_PNRARR, LAN_PNRARR_ARRTOPNR(temp16));


        if (EtherVerbose == SHOW_ALL) {
                printf("lan91c11x: Transmit Allocate Pointer = 0x%02x.\n", temp16);
        }

        // increment the pointer to point to this new packet
        lan91c11x_wr(LAN_PTR, LAN_PTR_AUTOINC);


        // a delay is necessary between POINTER and DATA access.
        for(i = 0; i < 10; i++);


        lan91c11x_wr(LAN_DATA1, 0);                     // control word
        lan91c11x_wr(LAN_DATA2, len + 6);       // total byte count including control bytes

        // we do 16-bit writes, so get a 16-bit pointer to the tx buffer
        sptr = (ushort *)txbuf;

        // transfer the packet into the lan91c11x
        while(1) {
                // write the data until we have only one or no bytes left
                if (len > 1) {
                        lan91c11x_wr(LAN_DATA1,*sptr++);
                        len -= 2;
                }
                // last byte of an odd size packet?  set the odd length bit
                else if (len == 1) {
                        lan91c11x_wr(LAN_DATA1,(*sptr & 0xff00) | LAN_CTL_BYTE_ODD_LENGTH);
                        break;
                }
                // even packet, clear the control byte
                else {
                        lan91c11x_wr(LAN_DATA1,0);
                        break;
                }
        }
        // tell the LAN to queue the packet for transmission
        // remember: we use autorelease, so no cleanup of the
        // MMU is required.  It also means we have to rely on
        // the upper layers to handle re-transmission.
        lan91c11x_wr(LAN_MMU_CMD, LAN_MMU_CMD_ENQUEUETXPKT);

#if 0 	/* This block of code is only applicable if we do not
		 * use AUTO_RELEASE...
		 */
                // wait for TX Success bit, then check status
                // loop until the TX interrupt bit is set or we exhaust the timeout
                for(timeout = 10000; timeout > 0; timeout--) {
                        if (lan91c11x_rd(LAN_EPH) & LAN_EPH_TX_SUC)
                                break;
                }
                // we should always succeed within the timeout period 
                // since we are only sending one packet at a time.  If
                // we don't it means we have either a chip problem or 
                // problems on the wire that are preventing the transmission
                if (timeout == 0) {
                        printf("lan91c11x: Transmit Interrupt Timed Out\n");
                        // restart the chip
//                      EtherdevStartup(0);
//                      return(-1);
                }
                // clear the interrupt
//              lan91c11x_wr(LAN_INT, LAN_INT_TX);

                // check the status
                temp16 = lan91c11x_rd(LAN_EPH);
                if (!(temp16 & LAN_EPH_TX_SUC))
                {
                        if (!(temp16 & LAN_EPH_LINK_OK)) {
                                printf("lan91c11x: Transmit Phy Link Not OK!\n");
                        }
                        if (temp16 & LAN_EPH_TXUNRN) {
                                printf("lan91c11x: Transmit Underrun Error!\n");
                        }
                        if (temp16 & LAN_EPH_EXC_DEF) {
                                printf("lan91c11x: Transmit Ecsessive Deferral Error!\n");
                        }
                        if (temp16 & LAN_EPH_LOST_CARR) {
                                printf("lan91c11x: Transmit Lost Carrier Error!\n");
                        }
                        if (temp16 & LAN_EPH_LATECOL) {
                                printf("lan91c11x: Transmit Late Collision Error!\n");
                        }
                        if (temp16 & LAN_EPH_SQET) {
                                printf("lan91c11x: Transmit Signal Quality (SQEL) Error!\n");
                        }
                        if (temp16 & LAN_EPH_16COL) {
                                printf("lan91c11x: Transmit 16 Collision Error!\n");
                        }
                        return(-1);
                }
                
                // got through OK!              
                printf("lan91c11x: Transmit Completed OK.\n");
#endif

        // Enable the transmitter and receiver
        lan91c11x_wr(LAN_TCR, lan91c11x_rd(LAN_TCR) | LAN_TCR_TXENA);
        lan91c11x_wr(LAN_RCR, lan91c11x_rd(LAN_RCR) | LAN_RCR_RXEN);

        return 0;
}


//--------------------------------------------------------------------------
// lan91c11x_rx()
//
// This function checks to see if the lan91c11x has a receive buffer
// ready.  If so, it copies it into the buffer pointed to by pktbuf
//
int lan91c11x_rx(uchar *pktbuf)
{
        ushort  packet_num, rfsw, pktsize, count, *data, i, istat;

        // check the RX interrupt to see if we have a packet
        istat = lan91c11x_rd(LAN_INT);
        
        // if it's an overrun, then we just reset the MMU to free
        // up the memory, increment the error counter and leave
        if (istat & LAN_INT_RXOVRN) {
                lan91c11x_wr(LAN_MMU_CMD, LAN_MMU_CMD_RESETMMU);
                if (EtherVerbose)
	                printf("lan91c11x Receive Overrun Error.\n");

                // clear the over run interrupt bit
                lan91c11x_wr(LAN_INT, LAN_INT_RXOVRN);
                EtherRXOVRNCnt++;
                return 0;
        }
        if (istat & LAN_INT_RCV)
        {
                // clear the RCV interrupt bit
                lan91c11x_wr(LAN_INT, LAN_INT_RCV);

                packet_num = lan91c11x_rd(LAN_FIFOPORT);

                if (packet_num & LAN_FIFOPORT_RCVFIFO_EMPTY)
                {
                		if (EtherVerbose)
                    	    printf("lan91c11x WARNING: nothing on rx FIFO. \n");
                        return 0;
                }
                // definitely got a packet
                EtherRFRAMECnt++;

                // tell the lan91c11x to move the received packet to the
                // data area so we can process it
                lan91c11x_wr(LAN_PTR, LAN_PTR_RCV
                                                        | LAN_PTR_AUTOINC
                                                        | LAN_PTR_READ);

                for(i = 0; i < 100; i++);                       /* delay necessary between POINTER */
                                                                                        /* and DATA access. */
                rfsw = lan91c11x_rd(LAN_DATA1);         /* Receive frame status word */
                count = lan91c11x_rd(LAN_DATA1);        /* Byte count. */
                if (count < 16)
                {
                		if (EtherVerbose)
                        	printf("lan91c11x Invalid receive count = 0x%04x, Status Word = 0x%04x.\n", count, rfsw);
                        return 0;
                }
                count &= 0x7ff;                                         /* Upper 5 bits are reserved. */
                count -= 4;                                                     /* Deduct for RFSW and ByteCount */
                if (!(rfsw & LAN_RFSW_ODD_LEN))         /* Deduct for CTRL byte and possibly */
                        pktsize = count - 2;                    /* one more because len != odd. */
                else
                        pktsize = count - 1;
                data = (ushort *)pktbuf;                        // we do 16-bit reads
                if (count >= 1600)
                {
                		if (EtherVerbose)
                        	printf("lan91c11x geteinbuf() overflow (cnt=0x%04x)\n",count);
                        enreset();
                        eninit();
                        return 0;       // no packets available
                }

                // retreive the receieve packet
                if (count & 1) count++;
                while(count > 0)
                {
                        *data++ = lan91c11x_rd(LAN_DATA1);
                        count -= 2;
                }

                // tell the lan91c11x to release the memory used by this packet
                lan91c11x_wr(LAN_MMU_CMD, LAN_MMU_CMD_RMRELRXFRAME);

                // check the frame for errors and update the appropriate counters
                if (rfsw & (LAN_RFSW_ALGN_ERR
                                  | LAN_RFSW_BADCRC
                                  | LAN_RFSW_TOOLNG
                                  | LAN_RFSW_TOOSHORT))
                {
                        if (rfsw & LAN_RFSW_ALGN_ERR) {
                                EtherALGNERRCnt++;
                				if (EtherVerbose)
                            	    printf("Frame alignment error!\n");
                        }
                        if (rfsw & LAN_RFSW_BADCRC) {
                                EtherBADCRCCnt++;
                				if (EtherVerbose)
                                	printf("Bad CRC!\n");
                        }
                        if (rfsw & LAN_RFSW_TOOLNG) {
                                EtherTOOLONGCnt++;
                				if (EtherVerbose)
                                	printf("Frame too long!\n");
                        }
                        if (rfsw & LAN_RFSW_TOOLNG) {
                                EtherTOOSHORTCnt++;
                				if (EtherVerbose)
                                	printf("Frame too short!\n");
                        }
                        return(-1);
                }
                processPACKET((struct ether_header *)pktbuf, pktsize);

                if (EtherVerbose == SHOW_ALL){
                        printf("lan91c11x: Receive Packet Complete.\n");
                }
                return count;
        } // if RCV_INT
        return 0;
}
                                        


void
lan91c11x_show_stats(void)
{
	ushort status;

	printf("Receiver overrun errors: %d\n",EtherRXOVRNCnt);
	printf("Alignment errors:        %d\n",EtherALGNERRCnt);
	printf("Bad CRC errors:          %d\n",EtherBADCRCCnt);
	printf("Packet too long errors:  %d\n",EtherTOOLONGCnt);
	printf("Packet too short errors: %d\n",EtherTOOSHORTCnt);
	printf("Wait-for-alloc timeouts: %d\n",EtherWFATMTCnt);

	status = lan91c11x_phy(PHY_INT, 0, 0);
	printf("Link: %dBaseT %s duplex\n",
		status & PHY_INT_SPDDET ? 100 : 10,
		status & PHY_INT_DPLXDET ? "full" : "half");
}

//--------------------------------------------------------------------------
// lan91c11x_phy()
//
// This function reads or writes a register in the lan91c11x PHY
//
ushort lan91c11x_phy(uchar phyreg, ushort phydata, int phy_dir)
{
        int i;
        ushort mask;
        ushort bits[64];        // array to hold the writes to the LAN_MGMT Register
        int clk_idx = 0;
        int input_idx = 0;

        // 32 consecutive ones on MDO to establish sync
        for (i = 0; i < 32; ++i)
                bits[clk_idx++] = LAN_MGMT_MDOE | LAN_MGMT_MDO;

        // Start code <01>
        bits[clk_idx++] = LAN_MGMT_MDOE;
        bits[clk_idx++] = LAN_MGMT_MDOE | LAN_MGMT_MDO;

        if (phy_dir == 0)       // read
        {
                // Read command <10>
                bits[clk_idx++] = LAN_MGMT_MDOE | LAN_MGMT_MDO;
                bits[clk_idx++] = LAN_MGMT_MDOE;
        }
        else
        {
                // Write command <01>
                bits[clk_idx++] = LAN_MGMT_MDOE;
                bits[clk_idx++] = LAN_MGMT_MDOE | LAN_MGMT_MDO;
        }

        // Output the PHY address, msb first - Internal PHY is address 0
        for (i = 0; i < 5; ++i)
        {
                        bits[clk_idx++] = LAN_MGMT_MDOE;
        }

        // Output the phy register number, msb first
        mask = 0x10;
        for (i = 0; i < 5; ++i)
                {
                if (phyreg & mask)
                        bits[clk_idx++] = LAN_MGMT_MDOE | LAN_MGMT_MDO;
                else
                        bits[clk_idx++] = LAN_MGMT_MDOE;


                // Shift to next lowest bit
                mask >>= 1;
                }

        if (phy_dir == 0)       // read
        {
                // 1 extra bit time for turnaround
                bits[clk_idx++] = 0;


                // Input starts at this bit time
                input_idx = clk_idx;

                // Will input 16 bits
                for (i = 0; i < 16; ++i)
                        bits[clk_idx++] = 0;
        }
        else    // write
        {
                // 2 extra bit times for turnaround
                bits[clk_idx++] = 0;
                bits[clk_idx++] = 0;

                // Write out 16 bits of data, msb first
                mask = 0x8000;
                for (i = 0; i < 16; ++i)
                {
                        if (phydata & mask)
                                bits[clk_idx++] = LAN_MGMT_MDOE | LAN_MGMT_MDO;
                        else
                                bits[clk_idx++] = LAN_MGMT_MDOE;

                        // Shift to next lowest bit
                        mask >>= 1;
                }
        } // else write

        // Final clock bit
        bits[clk_idx++] = 0;

        // Turn off all MII Interface bits
        lan91c11x_wr(LAN_MGMT, lan91c11x_rd(LAN_MGMT) & 0xfff0);

        // Clock all 64 cycles
        for (i = 0; i < sizeof bits; ++i)
                {
                // Clock Low - output data
                lan91c11x_wr(LAN_MGMT, bits[i]);
                monDelay(2);

                // Clock Hi - input data
                lan91c11x_wr(LAN_MGMT, bits[i] | LAN_MGMT_MCLK);
                monDelay(2);
                bits[i] |= lan91c11x_rd(LAN_MGMT) & LAN_MGMT_MDI;
                }

        // Return to idle state
        // Set clock to low, data to low, and output tristated
        lan91c11x_wr(LAN_MGMT, lan91c11x_rd(LAN_MGMT) & 0xfff0);
        monDelay(2);

        if (phy_dir == 0)       // read
        {
                // Recover input data
                phydata = 0;
                for (i = 0; i < 16; ++i)
                {
                        phydata <<= 1;


                        if (bits[input_idx++] & LAN_MGMT_MDI)
                                phydata |= 0x0001;
                }
                if (EtherVerbose == SHOW_ALL)
                {
                        printf("Read lan91c11x_phy_register(): phyreg=%x,phydata=%x\n",
                        phyreg, phydata);
                }
        } // if read
        else // write
        {
                if (EtherVerbose == SHOW_ALL)
                {
                        printf("Wrote lan91c11x_phy_register(): phyreg=%x,phydata=%x\n",
                        phyreg, phydata);
                }
        }
        return(phydata);        
}

//--------------------------------------------------------------------------
// lan91c11x_phy()
//
// This function configures the lan91c11x PHY using autonegotiation
//
int lan91c11x_phy_cfg()
{
        int timeout;
        ushort my_phy_caps; // My PHY capabilities
        ushort my_ad_caps; // My Advertised capabilities
        ushort status = 0;

        // Reset the PHY, setting all other bits to zero
        lan91c11x_phy(PHY_CNTL, PHY_CNTL_RST, 1);

        // Wait for the reset to complete, or time out
        timeout = 3000; // Wait up to 3 seconds
        while (timeout--)
        {
                if (!(lan91c11x_phy(PHY_CNTL, 0, 0) & PHY_CNTL_RST)) break;
                monDelay(2); // wait 2 millisec
        }


        if (timeout < 1)
        {
                printf("lan91c11x PHY Reset Timed Out!\n");
                return -1;
        }

        // Copy our capabilities from PHY_STAT to PHY_AD
        my_phy_caps = lan91c11x_phy(PHY_STAT, 0, 0);
        my_ad_caps  = PHY_AD_CSMA; // I am CSMA capable

        if (my_phy_caps & PHY_STAT_CAP_T4)
                my_ad_caps |= PHY_AD_T4;

        if (my_phy_caps & PHY_STAT_CAP_TXF)
                my_ad_caps |= PHY_AD_TX_FDX;

        if (my_phy_caps & PHY_STAT_CAP_TXH)
                my_ad_caps |= PHY_AD_TX_HDX;

        if (my_phy_caps & PHY_STAT_CAP_TF)
                my_ad_caps |= PHY_AD_10_FDX;

        if (my_phy_caps & PHY_STAT_CAP_TH)
                my_ad_caps |= PHY_AD_10_HDX;

        lan91c11x_phy(PHY_AD, my_ad_caps, 1);

        if (EtherVerbose)
 	       printf("lan91c11x phy advertised caps=%x\n", my_ad_caps);

        // Restart auto-negotiation process in order to advertise my caps
        lan91c11x_phy(PHY_CNTL, (PHY_CNTL_ANEG_EN | PHY_CNTL_ANEG_RST), 1);

        // Wait for the auto-negotiation to complete.  This may take from
        // 2 to 3 seconds.
        timeout = 50; 
        while (timeout--)
                {
                status = lan91c11x_phy(PHY_STAT, 0, 0);
                if (status & PHY_STAT_ANEG_ACK)
                {
                        // auto-negotiate complete
                        break;
                }

                monDelay(2); // wait 2 millisec

                // Restart auto-negotiation again if remote fault
                if (status & PHY_STAT_REM_FLT)
                {
                        printf("lan91c11x phy remote fault detected.\n");

                        // Restart auto-negotiation
                        printf("lan91c11x phy restarting auto-negotiation.\n");
                        lan91c11x_phy(PHY_CNTL, (PHY_CNTL_ANEG_EN 
                                                                   | PHY_CNTL_ANEG_RST 
                                                                   | PHY_CNTL_SPEED 
                                                                   | PHY_CNTL_DPLX), 1);

                } // if remote fault
        }       // while autonegotiate

        if (timeout < 1)
        {
                printf("lan91c11x phy auto-negotiate timed out.\n");
                lan91c11x_phy(PHY_CNTL,0,1);
        }

        // Fail if we detected an auto-negotiate remote fault
        if (status & PHY_STAT_REM_FLT)
                {
                printf("lan91c11x phy remote fault detected, aborting autonegotiate process.\n");
                return -1;
                }

        // Display auto-negotiation results
        status = lan91c11x_phy(PHY_INT, 0, 0);
		if (EtherVerbose) {
	        if ( status & PHY_INT_SPDDET )
                printf("lan91c11x phy Link Speed = 100BaseT\n");
	        else
                printf("lan91c11x phy Link Speed = 10BaseT\n");

	        if ( status & PHY_INT_DPLXDET )
                printf("lan91c11x phy Link Duplex = Full\n");
	        else
                printf("lan91c11x phy Link Duplex = Half\n");
		}
        return 0;
}


#endif // INCLUDE_ETHERNET

