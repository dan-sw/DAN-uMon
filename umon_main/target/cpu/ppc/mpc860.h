/*-----------------------------------------------------------------------------				
*
* File:  MPC860.H
*
* Description:  
*
* Definitions of the parameter area RAM.
* Note that different structures are overlaid
* at the same offsets for the different modes
* of operation.
*
* History:
*
* 12 DEC 95	   lvn	Bundled multiple files from MSIL into this file
*
* 18 JUN 96    saw  Replaced:
*
*                   VUWORD	simt_tbscr;
*	                 VUWORD	simt_rtcsc;
*	                 With:
*
*                   VUHWORD	simt_tbscr;
*                   VUBYTE	RESERVED100[0x2];
*                   VUHWORD	simt_rtcsc;
*                   VUBYTE	RESERVED110[0x2];
*
* 04 OCT 96    saw  Filled in PCMCIA section.
*
* 21 NOV 96    saw  Changed address of PIP PBODR to 0xac2 from 0xac0.
*
* 18 DEC 96    saw  Renamed Registers:
*
*                        scc_gsmra -> scc_gsmr_l
*                        scc_gsmrb -> scc_gsmr_h
*
* 06 JAN 97    sgj  Created 860 version from 8xx code standard
*
* 18 JUL 97	   sgj  Corrected mapping of Ucode/BD DPRAM
*
* 08 AUG 97    sgj  Added definitions for key registers for System
*                   Integration timers, Clock, and Reset
*
* 26 AUG 97    sgj  Removed ADS-specific definitions and
*                   reorganized for readability
*
* 20 NOV 97    jay  Added to and cleaned up the parameter map definitions
* 
* 10 FEB 98		jay  Updated IDMA definitions
*
* 18 FEB 98		ggh  Added QMC definitions 
*					ecg
*
* 01 MAY 98    jay  fixed a parameter ram bug created by the previous 
*                   modifications on 18 FEB 98.
* 
*
*-----------------------------------------------------------------------------*/

#define UHWORD	volatile unsigned short
#define UWORD	volatile unsigned long
#define UBYTE	volatile unsigned char
#define VUBYTE	volatile unsigned char
#define VUHWORD	volatile unsigned short
#define VUWORD	volatile unsigned long
#define VHWORD	volatile short

/******************************************************************************
*
* Definitions of Parameter RAM entries for each peripheral and mode
*
******************************************************************************/

/*---------------------------------------------------------------------------*/
/*	                    HDLC parameter RAM (SCC)							           */
/*---------------------------------------------------------------------------*/

struct hdlc_pram 

{
       
   /*-------------------*/
   /* SCC parameter RAM */
   /*-------------------*/
   
   UHWORD	rbase;		/* RX BD base address */
   UHWORD	tbase;		/* TX BD base address */
   UBYTE	   rfcr;		   /* Rx function code */
   UBYTE	   tfcr;		   /* Tx function code */
   UHWORD	mrblr;		/* Rx buffer length */
   UWORD	   rstate;		/* Rx internal state */
   UWORD	   rptr;		   /* Rx internal data pointer */
   UHWORD	rbptr;		/* rb BD Pointer */
   UHWORD	rcount;		/* Rx internal byte count */
   UWORD	   rtemp;		/* Rx temp */
   UWORD	   tstate;		/* Tx internal state */
   UWORD	   tptr;		   /* Tx internal data pointer */
   UHWORD	tbptr;		/* Tx BD pointer */
   UHWORD	tcount;		/* Tx byte count */
   UWORD	   ttemp;		/* Tx temp */
   UWORD	   rcrc;		   /* temp receive CRC */
   UWORD	   tcrc;		   /* temp transmit CRC */
   
   /*-----------------------------*/
   /* HDLC specific parameter RAM */
   /*-----------------------------*/

   UBYTE	   RESERVED1[4];	/* Reserved area */
   UWORD	   c_mask;			/* CRC constant */
   UWORD	   c_pres;			/* CRC preset */
   UHWORD	disfc;			/* discarded frame counter */
   UHWORD	crcec;			/* CRC error counter */
   UHWORD	abtsc;			/* abort sequence counter */
   UHWORD	nmarc;			/* nonmatching address rx cnt */
   UHWORD	retrc;			/* frame retransmission cnt */
   UHWORD	mflr;			   /* maximum frame length reg */
   UHWORD	max_cnt;		   /* maximum length counter */
   UHWORD	rfthr;			/* received frames threshold */
   UHWORD	rfcnt;			/* received frames count */
   UHWORD	hmask;			/* user defined frm addr mask */
   UHWORD	haddr1;			/* user defined frm address 1 */
   UHWORD	haddr2;			/* user defined frm address 2 */
   UHWORD	haddr3;			/* user defined frm address 3 */
   UHWORD	haddr4;			/* user defined frm address 4 */
   UHWORD	tmp;			   /* temp */
   UHWORD	tmp_mb;			/* temp */
};


/*-------------------------------------------------------------------------*/
/*	                 ASYNC HDLC parameter RAM (SCC)						         */
/*-------------------------------------------------------------------------*/

struct async_hdlc_pram 

{
       
   /*-------------------*/
   /* SCC parameter RAM */
   /*-------------------*/

   UHWORD	rbase;		/* RX BD base address */
   UHWORD	tbase;		/* TX BD base address */
   UBYTE	   rfcr;		   /* Rx function code */
   UBYTE	   tfcr;		   /* Tx function code */
   UHWORD	mrblr;		/* Rx buffer length */
   UWORD	   rstate;		/* Rx internal state */
   UWORD	   rptr;		   /* Rx internal data pointer */
   UHWORD	rbptr;		/* rb BD Pointer */
   UHWORD	rcount;		/* Rx internal byte count */
   UWORD	   rtemp;		/* Rx temp */
   UWORD	   tstate;		/* Tx internal state */
   UWORD	   tptr;		   /* Tx internal data pointer */
   UHWORD	tbptr;		/* Tx BD pointer */
   UHWORD	tcount;		/* Tx byte count */
   UWORD 	ttemp;		/* Tx temp */
   UWORD	   rcrc;		   /* temp receive CRC */
   UWORD    tcrc;		   /* temp transmit CRC */
         
   /*-----------------------------------*/
   /* ASYNC HDLC specific parameter RAM */
   /*-----------------------------------*/

   UBYTE	   RESERVED2[4];	/* Reserved area */
   UWORD	   c_mask;			/* CRC constant */
   UWORD	   c_pres;			/* CRC preset */
   UHWORD	bof;			   /* begining of flag character */
   UHWORD	eof;			   /* end of flag character */
   UHWORD	esc;			   /* control escape character */
   UBYTE	   RESERVED3[4];	/* Reserved area */
   UHWORD	zero;			   /* zero */
   UBYTE	   RESERVED4[2];	/* Reserved area */
   UHWORD	rfthr;			/* received frames threshold */
   UBYTE	   RESERVED5[4];	/* Reserved area */
   UWORD	   txctl_tbl;		/* Tx ctl char mapping table */
   UWORD	   rxctl_tbl;		/* Rx ctl char mapping table */
   UHWORD	nof;			   /* Number of opening flags */
};


/*--------------------------------------------------------------------------*/
/*                    UART parameter RAM (SCC)								       */
/*--------------------------------------------------------------------------*/

/*----------------------------------------*/
/* bits in uart control characters table  */
/*----------------------------------------*/

#define	CC_INVALID	0x8000		/* control character is valid */
#define	CC_REJ		0x4000		/* don't store char in buffer */
#define	CC_CHAR		0x00ff		/* control character */

/*------*/
/* UART */
/*------*/

struct uart_pram 

{
   /*-------------------*/
   /* SCC parameter RAM */
   /*-------------------*/

   UHWORD	rbase;		/* RX BD base address */
   UHWORD	tbase;		/* TX BD base address */
   UBYTE	   rfcr;		   /* Rx function code */
   UBYTE	   tfcr;		   /* Tx function code */
   UHWORD	mrblr;		/* Rx buffer length */
   UWORD	   rstate;		/* Rx internal state */
   UWORD	   rptr;		   /* Rx internal data pointer */
   UHWORD	rbptr;		/* rb BD Pointer */
   UHWORD	rcount;		/* Rx internal byte count */
   UWORD	   rx_temp;	   /* Rx temp */
   UWORD	   tstate;		/* Tx internal state */
   UWORD	   tptr;		   /* Tx internal data pointer */
   UHWORD	tbptr;		/* Tx BD pointer */
   UHWORD	tcount;		/* Tx byte count */
   UWORD	   ttemp;		/* Tx temp */
   UWORD	   rcrc;		   /* temp receive CRC */
   UWORD	   tcrc;		   /* temp transmit CRC */

   /*------------------------------*/
   /*	UART specific parameter RAM  */
   /*------------------------------*/

   UBYTE	   RESERVED6[8];	/* Reserved area */
   UHWORD	max_idl;		   /* maximum idle characters */
   UHWORD	idlc;			   /* rx idle counter (internal) */
   UHWORD	brkcr;			/* break count register */

   UHWORD	parec;			/* Rx parity error counter */
   UHWORD	frmec;			/* Rx framing error counter */
   UHWORD	nosec;			/* Rx noise counter */
   UHWORD	brkec;			/* Rx break character counter */
   UHWORD	brkln;			/* Reaceive break length */
                                           
   UHWORD	uaddr1;			/* address character 1 */
   UHWORD	uaddr2;			/* address character 2 */
   UHWORD	rtemp;			/* temp storage */
   UHWORD	toseq;			/* Tx out of sequence char */
   UHWORD	cc[8];			/* Rx control characters */
   UHWORD	rccm;			   /* Rx control char mask */
   UHWORD	rccr;			   /* Rx control char register */
   UHWORD	rlbc;			   /* Receive last break char */
};


/*---------------------------------------------------------------------------
*                    BISYNC parameter RAM (SCC)
*--------------------------------------------------------------------------*/

struct bisync_pram 

{
   /*-------------------*/
   /* SCC parameter RAM */
   /*-------------------*/

   UHWORD	rbase;		/* RX BD base address */
   UHWORD	tbase;		/* TX BD base address */
   UBYTE	   rfcr;		   /* Rx function code */
   UBYTE	   tfcr;		   /* Tx function code */
   UHWORD	mrblr;		/* Rx buffer length */
   UWORD	   rstate;		/* Rx internal state */
   UWORD	   rptr;		   /* Rx internal data pointer */
   UHWORD	rbptr;		/* rb BD Pointer */
   UHWORD	rcount;		/* Rx internal byte count */
   UWORD	   rtemp;		/* Rx temp */
   UWORD	   tstate;		/* Tx internal state */
   UWORD	   tptr;		   /* Tx internal data pointer */
   UHWORD	tbptr;		/* Tx BD pointer */
   UHWORD	tcount;		/* Tx byte count */
   UWORD	   ttemp;		/* Tx temp */
   UWORD	   rcrc;		   /* temp receive CRC */
   UWORD	   tcrc;		   /* temp transmit CRC */

   /*--------------------------------*/
   /*	BISYNC specific parameter RAM */
   /*--------------------------------*/

   UBYTE	   RESERVED7[4];	/* Reserved area */
   UWORD	   crcc;			   /* CRC Constant Temp Value */
   UHWORD	prcrc;			/* Preset Receiver CRC-16/LRC */
   UHWORD	ptcrc;			/* Preset Transmitter CRC-16/LRC */
   UHWORD	parec;			/* Receive Parity Error Counter */
   UHWORD	bsync;			/* BISYNC SYNC Character */
   UHWORD	bdle;			   /* BISYNC DLE Character */
   UHWORD	cc[8];			/* Rx control characters */
   UHWORD	rccm;			   /* Receive Control Character Mask */
};


/*-------------------------------------------------------------------------*/
/*             Transparent mode parameter RAM (SCC)						   */
/*-------------------------------------------------------------------------*/

struct transparent_pram 

{
   /*--------------------*/
   /*	SCC parameter RAM */
   /*--------------------*/

   UHWORD	rbase;		/* RX BD base address */
   UHWORD	tbase;		/* TX BD base address */
   UBYTE	   rfcr;		   /* Rx function code */
   UBYTE	   tfcr;		   /* Tx function code */
   UHWORD	mrblr;		/* Rx buffer length */
   UWORD	   rstate;		/* Rx internal state */
   UWORD	   rptr;		   /* Rx internal data pointer */
   UHWORD	rbptr;		/* rb BD Pointer */
   UHWORD	rcount;		/* Rx internal byte count */
   UWORD	   rtemp;		/* Rx temp */
   UWORD	   tstate;		/* Tx internal state */
   UWORD	   tptr;		   /* Tx internal data pointer */
   UHWORD	tbptr;		/* Tx BD pointer */
   UHWORD	tcount;		/* Tx byte count */
   UWORD	   ttemp;		/* Tx temp */
   UWORD	   rcrc;		   /* temp receive CRC */
   UWORD	   tcrc;		   /* temp transmit CRC */

   /*-------------------------------------*/
   /*	TRANSPARENT specific parameter RAM */
   /*-------------------------------------*/

   UWORD	crc_p;		/* CRC Preset */
   UWORD	crc_c;		/* CRC constant */
};


/*-------------------------------------------------------------------------*/ 
/*                    Ethernet parameter RAM (SCC)						         */
/*-------------------------------------------------------------------------*/

struct ethernet_pram 

{
   /*--------------------*/
   /*	SCC parameter RAM */
   /*--------------------*/

   UHWORD	rbase;		/* RX BD base address */
   UHWORD	tbase;		/* TX BD base address */
   UBYTE	   rfcr;		   /* Rx function code */
   UBYTE	   tfcr;		   /* Tx function code */
   UHWORD	mrblr;		/* Rx buffer length */
   UWORD	   rstate;	 	/* Rx internal state */
   UWORD	   rptr;		   /* Rx internal data pointer */
   UHWORD	rbptr;		/* rb BD Pointer */
   UHWORD	rcount;		/* Rx internal byte count */
   UWORD	   rtemp;		/* Rx temp */
   UWORD	   tstate;		/* Tx internal state */
   UWORD	   tptr;		   /* Tx internal data pointer */
   UHWORD	tbptr;		/* Tx BD pointer */
   UHWORD	tcount;		/* Tx byte count */
   UWORD	   ttemp;		/* Tx temp */
   UWORD	   rcrc;		   /* temp receive CRC */
   UWORD	   tcrc;		   /* temp transmit CRC */

   /*---------------------------------*/
   /*	ETHERNET specific parameter RAM */
   /*---------------------------------*/

   UWORD	   c_pres;			/* preset CRC */
   UWORD	   c_mask;			/* constant mask for CRC */
   UWORD	   crcec;			/* CRC error counter */
   UWORD	   alec;			   /* alighnment error counter */
   UWORD	   disfc;			/* discard frame counter */
   UHWORD	pads;			   /* short frame PAD characters */
   UHWORD	ret_lim;		   /* retry limit threshold */
   UHWORD	ret_cnt;		   /* retry limit counter */
   UHWORD	mflr;			   /* maximum frame length reg */
   UHWORD	minflr;			/* minimum frame length reg */
   UHWORD	maxd1;			/* maximum DMA1 length reg */
   UHWORD	maxd2;			/* maximum DMA2 length reg */
   UHWORD	maxd;			   /* rx max DMA */
   UHWORD	dma_cnt;		   /* rx dma counter */
   UHWORD	max_b;			/* max bd byte count */
   UHWORD	gaddr1;			/* group address filter 1 */
   UHWORD	gaddr2;			/* group address filter 2 */
   UHWORD	gaddr3;			/* group address filter 3 */
   UHWORD	gaddr4;			/* group address filter 4 */
   UWORD	   tbuf0_data0;	/* save area 0 - current frm */
   UWORD	   tbuf0_data1;	/* save area 1 - current frm */
   UWORD	   tbuf0_rba0;
   UWORD	   tbuf0_crc;
   UHWORD	tbuf0_bcnt;
   UHWORD	paddr_h;		   /* physical address (MSB) */
   UHWORD	paddr_m;		   /* physical address */
   UHWORD	paddr_l;		   /* physical address (LSB) */
   UHWORD	p_per;			/* persistence */
   UHWORD	rfbd_ptr;		/* rx first bd pointer */
   UHWORD	tfbd_ptr;		/* tx first bd pointer */
   UHWORD	tlbd_ptr;		/* tx last bd pointer */
   UWORD	   tbuf1_data0;	/* save area 0 - next frame */
   UWORD	   tbuf1_data1;	/* save area 1 - next frame */
   UWORD	   tbuf1_rba0;
   UWORD	   tbuf1_crc;
   UHWORD	tbuf1_bcnt;
   UHWORD	tx_len;			/* tx frame length counter */
   UHWORD	iaddr1;			/* individual address filter 1*/
   UHWORD	iaddr2;			/* individual address filter 2*/
   UHWORD	iaddr3;			/* individual address filter 3*/
   UHWORD	iaddr4;			/* individual address filter 4*/
   UHWORD	boff_cnt;		/* back-off counter */
   UHWORD	taddr_h;		   /* temp address (MSB) */
   UHWORD	taddr_m;		   /* temp address */
   UHWORD	taddr_l;		   /* temp address (LSB) */
};


/*------------------------------------------------------------------*/
/*                          QMC  definitions 				           */
/*------------------------------------------------------------------*/


struct global_qmc_pram {

    UWORD   mcbase;		/* Multichannel Base pointer */
    UHWORD  qmcstate;	/* Multichannel Controller state */
    UHWORD  mrblr;		/* Maximum Receive Buffer Length */
    UHWORD  tx_s_ptr;   /* TSATTx Pointer */
    UHWORD  rxptr;	   /* Current Time slot entry in TSATRx */
    UHWORD  grfthr;		/* Global Receive frame threshold */
    UHWORD  grfcnt;		/* Global Receive Frame Count */
    UWORD   intbase;		/* Multichannel Base address */
    UWORD   intptr;		/* Pointer to interrupt queue */
    UHWORD  rx_s_ptr;   /* TSATRx Pointer */
    UHWORD  txptr;	   /* Current Time slot entry in TSATTx */
    UWORD   c_mask32;	/* CRC Constant (debb20e3) */
    UHWORD  tsatrx[32];	/* Time Slot Assignment Table Rx */
    UHWORD  tsattx[32];	/* Time Slot Assignment Table Tx */
    UHWORD  c_mask16;	/* CRC Constant (f0b8) */

};


   /*------------------------------------------*/
   /* QMC HDLC channel specific parameter RAM  */
   /*------------------------------------------*/

struct qmc_hdlc_pram {

    UHWORD  tbase;	/* Tx Buffer Descriptors Base Address */
    UHWORD  chamr;	/* Channel Mode Register */
    UWORD   tstate;	/* Tx Internal State */
    UWORD   txintr;	/* Tx Internal Data Pointer */
    UHWORD  tbptr;	/* Tx Buffer Descriptor Pointer */
    UHWORD  txcntr;	/* Tx Internal Byte Count */
    UWORD   tupack;	/* (Tx Temp) */
    UWORD   zistate;	/* Zero Insertion machine state */
    UWORD   tcrc;		/* Temp Transmit CRC */
    UHWORD  intmsk;	/* Channel's interrupt mask flags */
    UHWORD  bdflags;		
    UHWORD  rbase;	/* Rx Buffer Descriptors Base Address */
    UHWORD  mflr;		/* Max Frame Length Register */
    UWORD   rstate;	/* Rx Internal State */
    UWORD   rxintr;	/* Rx Internal Data Pointer */
    UHWORD  rbptr;	/* Rx Buffer Descriptor Pointer */
    UHWORD  rxbyc;	/* Rx Internal Byte Count */
    UWORD   rpack;	/* (Rx Temp) */
    UWORD   zdstate;	/* Zero Deletion machine state */
    UWORD   rcrc;		/* Temp Transmit CRC */
    UHWORD  maxc;		/* Max_length counter */
    UHWORD  tmp_mb;	/* Temp */
};


	 /*-------------------------------------------------*/
	 /* QMC Transparent channel specific parameter RAM  */
	 /*-------------------------------------------------*/

struct qmc_tran_pram {
    
	UHWORD	tbase;		/* Tx Bufer Descriptors Base Address */
	UHWORD	chamr;		/* Channel Mode Register */
	UWORD   tstate;		/* Tx Internal State */
    UWORD   txintr;		/* Tx Internal Data Pointer */
    UHWORD  tbptr;		/* Tx Buffer Descriptor Pointer */
    UHWORD  txcntr;		/* Tx Internal Byte Count */
    UWORD   tupack;		/* (Tx Temp) */
    UWORD   zistate;		/* Zero Insertion machine state */
    UWORD   RESERVED8;	
    UHWORD  intmsk;		/* Channel's interrupt mask flags */
    UHWORD  bdflags;		
    UHWORD  rbase;		/* Rx Buffer Descriptors Base Address */
    UHWORD  tmrblr;		/* Max receive buffer length */
    UWORD   rstate;		/* Rx Internal State */
    UWORD   rxintr;		/* Rx Internal Data Pointer */
    UHWORD  rbptr;		/* Rx Buffer Descriptor Pointer */
    UHWORD  rxbyc;		/* Rx Internal Byte Count */
    UWORD   rpack;		/* (Rx Temp) */
    UWORD   zdstate;		/* Zero Deletion machine state */
    UWORD   RESERVED9;	/* Temp Transmit CRC */
    UHWORD  trnsync;		/* Max_length counter */
    UHWORD  RESERVED10;	/* Temp */

};

struct mpc866fec
{
	VUWORD	addr_low;
	VUWORD	addr_high;
	VUWORD	hash_table_high;
	VUWORD	hash_table_low;
	VUWORD	r_des_start;
	VUWORD	x_des_start;
	VUWORD	r_buff_size;
	VUWORD	rsvd1[9];
	VUWORD	ecntrl;
	VUWORD	ievent;
	VUWORD	imask;
	VUWORD	ivec;
	VUWORD	r_des_active;
	VUWORD	x_des_active;
	VUWORD	rsvd2[10];
	VUWORD	mii_data;
	VUWORD	mii_speed;
	VUWORD	rsvd3[17];
	VUWORD	r_bound;
	VUWORD	r_fstart;
	VUWORD	rsvd4[4];
	VUWORD	x_wmrk;
	VUWORD	rsvd5;
	VUWORD	x_fstart;
	VUWORD	rsvd6[17];
	VUWORD	fun_code;
	VUWORD	rsvd7[3];
	VUWORD	r_cntrl;
	VUWORD	r_hash;
	VUWORD	rsvd8[14];
	VUWORD	x_cntrl;
};

/*----------------------------------------------------------*/
/* allows multiprotocol array declaration in the memory map */
/*----------------------------------------------------------*/

struct qmc_chan_pram
{
	union
	{
		struct qmc_hdlc_pram h;
		struct qmc_tran_pram t;
	}h_or_t;
};



/*--------------------------------------------------------------------*/
/*	                    SMC UART parameter RAM	                      */
/*--------------------------------------------------------------------*/

struct smc_uart_pram 

{
   UHWORD	rbase;		/* Rx BD Base Address */
   UHWORD	tbase;		/* Tx BD Base Address */
   UBYTE	   rfcr;		   /* Rx function code */
   UBYTE	   tfcr;		   /* Tx function code */
   UHWORD	mrblr;		/* Rx buffer length */
   UWORD	   rstate;		/* Rx internal state */
   UWORD	   rptr;		   /* Rx internal data pointer */
   UHWORD	rbptr;		/* rb BD Pointer */
   UHWORD	rcount;		/* Rx internal byte count */
   UWORD	   rtemp;		/* Rx temp */
   UWORD	   tstate;		/* Tx internal state */
   UWORD	   tptr;		   /* Tx internal data pointer */
   UHWORD	tbptr;		/* Tx BD pointer */
   UHWORD	tcount;		/* Tx byte count */
   UWORD	   ttemp;		/* Tx temp */
   UHWORD	max_idl;	   /* Maximum IDLE Characters */
   UHWORD	idlc;		   /* Temporary IDLE Counter */
   UHWORD	brkln;		/* Last Rx Break Length */
   UHWORD	brkec;		/* Rx Break Condition Counter */
   UHWORD	brkcr;		/* Break Count Register (Tx) */
   UHWORD	r_mask;		/* Temporary bit mask */
};


/*--------------------------------------------------------------------------*/
/*                  SMC Transparent mode parameter RAM						    */
/*--------------------------------------------------------------------------*/

struct smc_trnsp_pram 

{
   UHWORD	rbase;			/* Rx BD Base Address */
   UHWORD	tbase;			/* Tx BD Base Address */
   UBYTE	   rfcr;			   /* Rx function code */
   UBYTE	   tfcr;			   /* Tx function code */
   UHWORD	mrblr;			/* Rx buffer length */
   UWORD	   rstate;			/* Rx internal state */
   UWORD	   rptr;			   /* Rx internal data pointer */
   UHWORD	rbptr;			/* rb BD Pointer */
   UHWORD	rcount;			/* Rx internal byte count */
   UWORD	   rtemp;			/* Rx temp */
   UWORD	   tstate;			/* Tx internal state */
   UWORD	   tptr;			   /* Tx internal data pointer */
   UHWORD	tbptr;			/* Tx BD pointer */
   UHWORD	tcount;			/* Tx byte count */
   UWORD	   ttemp;			/* Tx temp */
   UHWORD   RESERVED11[5];	/* Reserved */
};


/*--------------------------------------------------------------------------*/
/*                      SPI parameter RAM									          */
/*--------------------------------------------------------------------------*/

#define	SPI_R	0x8000		/* Ready bit in BD */

struct spi_pram 

{
   UHWORD	rbase;		/* Rx BD Base Address */
   UHWORD	tbase;		/* Tx BD Base Address */
   UBYTE	   rfcr;		   /* Rx function code */
   UBYTE	   tfcr;		   /* Tx function code */
   UHWORD	mrblr;		/* Rx buffer length */
   UWORD	   rstate;		/* Rx internal state */
   UWORD	   rptr;		   /* Rx internal data pointer */
   UHWORD	rbptr;		/* rb BD Pointer */
   UHWORD	rcount;		/* Rx internal byte count */
   UWORD	   rtemp;		/* Rx temp */
   UWORD	   tstate;		/* Tx internal state */
   UWORD	   tptr;		   /* Tx internal data pointer */
   UHWORD	tbptr;		/* Tx BD pointer */
   UHWORD	tcount;		/* Tx byte count */
   UWORD	   ttemp;		/* Tx temp */
};


/*--------------------------------------------------------------------------*/
/*                       I2C parameter RAM									       */
/*--------------------------------------------------------------------------*/

struct i2c_pram 

{
   /*--------------------*/
   /*	I2C parameter RAM */
   /*--------------------*/

   UHWORD	rbase;		/* RX BD base address */
   UHWORD	tbase;		/* TX BD base address */
   UBYTE	   rfcr;		   /* Rx function code */
   UBYTE	   tfcr;		   /* Tx function code */
   UHWORD	mrblr;		/* Rx buffer length */
   UWORD	   rstate;		/* Rx internal state */
   UWORD	   rptr;		   /* Rx internal data pointer */
   UHWORD	rbptr;		/* rb BD Pointer */
   UHWORD	rcount;		/* Rx internal byte count */
   UWORD	   rtemp;		/* Rx temp */
   UWORD	   tstate;		/* Tx internal state */
   UWORD	   tptr;		   /* Tx internal data pointer */
   UHWORD	tbptr;		/* Tx BD pointer */
   UHWORD	tcount;		/* Tx byte count */
   UWORD	   ttemp;		/* Tx temp */
};


/*--------------------------------------------------------------------------*/
/*                      PIP Centronics parameter RAM						       */
/*--------------------------------------------------------------------------*/

struct centronics_pram 

{
   UHWORD	rbase;		/* Rx BD Base Address */
   UHWORD	tbase;		/* Tx BD Base Address */
   UBYTE	   fcr;		   /* function code */
   UBYTE	   smask;		/* Status Mask */
   UHWORD	mrblr;		/* Rx buffer length */
   UWORD	   rstate;		/* Rx internal state */
   UWORD	   rptr;		   /* Rx internal data pointer */
   UHWORD	rbptr;		/* rb BD Pointer */
   UHWORD	rcount;		/* Rx internal byte count */
   UWORD	   rtemp;		/* Rx temp */
   UWORD	   tstate;		/* Tx internal state */
   UWORD	   tptr;		   /* Tx internal data pointer */
   UHWORD	tbptr;		/* Tx BD pointer */
   UHWORD	tcount;		/* Tx byte count */
   UWORD	   ttemp;		/* Tx temp */
   UHWORD	max_sl;		/* Maximum Silence period */
   UHWORD	sl_cnt;		/* Silence Counter */
   UHWORD	char1;		/* CONTROL char 1 */
   UHWORD	char2;		/* CONTROL char 2 */
   UHWORD	char3;		/* CONTROL char 3 */
   UHWORD	char4;		/* CONTROL char 4 */
   UHWORD	char5;		/* CONTROL char 5 */
   UHWORD	char6;		/* CONTROL char 6 */
   UHWORD	char7;		/* CONTROL char 7 */
   UHWORD	char8;		/* CONTROL char 8 */
   UHWORD	rccm;		   /* Rx Control Char Mask */
   UHWORD	rccr;		   /* Rx Char Control Register */
};


/*--------------------------------------------------------------------------*/
/*							IDMA parameter RAM								             */
/*--------------------------------------------------------------------------*/

struct idma_pram 

{
   UHWORD ibase;	    /* IDMA BD Base Address */
   UHWORD dcmr;       /* DMA Channel Mode Register */
   UWORD  sapr;       /* Source Internal Data Pointer */
   UWORD  dapr;       /* Destination Internal Data Pointer */
   UHWORD ibptr;      /* Buffer Descriptor Pointer */
   UHWORD write_sp;   /* No description given in manual */
   UWORD  s_byte_c;   /* Internal Source Byte Count */
   UWORD  d_byte_c;   /* Internal Destination Byte Count */
   UWORD  s_state;    /* Internal State */
   UWORD  itemp0;     /* Temp Data Storage */
   UWORD  itemp1;     /* Temp Data Storage */
   UWORD  itemp2;     /* Temp Data Storage */
   UWORD  itemp3;     /* Temp Data Storage */
   UWORD  sr_mem;     /* Data Storage for Peripherial Write */
   UHWORD read_sp;    /* No description given in manual */
   UHWORD nodesc0;    /* Diff Between Source and Destination Residue*/
   UHWORD nodesc1;    /* Temp Storage Address Pointer */
   UHWORD nodesc2;    /* SR_MEM Byte Count */
   UWORD  d_state;    /* Internal State */
};



/*--------------------------------------------------------------------------*/
/*   					RISC timer parameter RAM							             */
/*--------------------------------------------------------------------------*/

struct timer_pram 

{
   /*----------------------------*/
   /*	RISC timers parameter RAM */
   /*----------------------------*/

   UHWORD	tm_base;	   /* RISC timer table base adr */
   UHWORD	tm_ptr;		/* RISC timer table pointer */
   UHWORD	r_tmr;		/* RISC timer mode register */
   UHWORD	r_tmv;		/* RISC timer valid register */
   UWORD	   tm_cmd;		/* RISC timer cmd register */
   UWORD	   tm_cnt;		/* RISC timer internal cnt */
};


/*--------------------------------------------------------------------------*/
/*						ROM Microcode parameter RAM							          */
/*--------------------------------------------------------------------------*/

struct ucode_pram 

{
   /*---------------------------*/
   /*	RISC ucode parameter RAM */
   /*---------------------------*/

   UHWORD	rev_num;    /* Ucode Revision Number */
   UHWORD	d_ptr;		/* MISC Dump area pointer */
   UWORD	   temp1;		/* MISC Temp1 */
   UWORD	   temp2;		/* MISC Temp2 */
};


/*---------------------------------------------------------------------------*/
/* Example structuring of user data area of memory at 0x2000 (base of DPRAM) */
/* Note that this area can also be used by microcodes and the QMC channel 	 */
/* specific parameter ram.													 */
/*---------------------------------------------------------------------------*/

struct user_data

{

   VUBYTE	udata_bd_ucode[0x200];	/* user data bd's or Ucode (small)  */
   VUBYTE	udata_bd_ucode2[0x200];	/* user data bd's or Ucode (medium) */
   VUBYTE	udata_bd_ucode3[0x400];	/* user data bd's or Ucode (large)  */
   VUBYTE	udata_bd[0x700];			/* user data bd's*/
   VUBYTE	ucode_ext[0x100];			/* Ucode Extension ram*/
   VUBYTE	RESERVED12[0x0c00];		/* Reserved area */

};

/*************************************************************************/
/* Structures added to support the use of DPRAM by Micro Monitor SMC1    */
/* (uart console) and SCC1 (ethernet driver).                            */
/* The number of ethernet buffer descriptors is based on the fact that   */
/* the micromon_data structure can use a maximum of 0x1c00 bytes in the  */
/* DPRAM.  This is shared between ethernet Rx and Tx buffer descriptors. */
/* sizeof(struct smc_uart_bd) * 2 */

#define EBDSZ	(sizeof(struct smc_uart_bd))
#define UBDSZ	(sizeof(struct enet_bd))
#define MAXEBDS	((0x1c00 - ((UBDSZ * 2) + 8))/EBDSZ)

struct smc_uart_bd {
	unsigned short	status;
	unsigned short	length;
	unsigned char	*baddr;
};

struct enet_bd {
	unsigned short	status;
	unsigned short	length;
	unsigned char	*baddr;
};

struct micromon_data
{
	struct	smc_uart_bd	u_rbd;			/* Uart rcv buffer descriptor */
	struct	smc_uart_bd	u_tbd;			/* Uart xmt buffer descriptor */
	unsigned char	u_rxbuf[4];			/* Uart rcv buffer */
	unsigned char	u_txbuf[4];			/* Uart xmt buffer */
	struct enet_bd	e_bdtbl[MAXEBDS];	/* Ethernet buffer descriptor tbl */
};

/* End of structures added to support Micro Monitor. */
/*************************************************************************/


/***************************************************************************/
/*																		                     */
/*	Definitions of Embedded PowerPC (EPPC) internal memory structures,	   */
/*  including registers and dual-port RAM								            */
/*																		                     */
/***************************************************************************/

typedef struct eppc 

{
   /*-----------------------------------*/
   /* BASE + 0x0000: INTERNAL REGISTERS */
   /*-----------------------------------*/

   /*-----*/
   /* SIU */
   /*-----*/

   VUWORD	siu_mcr;		      /* module configuration reg */
   VUWORD	siu_sypcr;		   /* System protection cnt */
   UBYTE    RESERVED13[0x6];
   VUHWORD	siu_swsr;		   /* sw service */
   VUWORD	siu_sipend;		   /* Interrupt pend reg */
   VUWORD	siu_simask;		   /* Interrupt mask reg */
   VUWORD	siu_siel;		   /* Interrupt edge level mask reg */
   VUWORD	siu_sivec;		   /* Interrupt vector */
   VUWORD	siu_tesr;		   /* Transfer error status */
   VUBYTE	RESERVED14[0xc];  /* Reserved area */
   VUWORD	dma_sdcr;		   /* SDMA configuration reg */
   UBYTE    RESERVED15[0x4c];

   /*--------*/
   /* PCMCIA */
   /*--------*/

   VUWORD  pcmcia_pbr0;      /* PCMCIA Base Reg: Window 0 */
   VUWORD  pcmcia_por0;      /* PCMCIA Option Reg: Window 0 */
   VUWORD  pcmcia_pbr1;      /* PCMCIA Base Reg: Window 1 */
   VUWORD  pcmcia_por1;      /* PCMCIA Option Reg: Window 1 */
   VUWORD  pcmcia_pbr2;      /* PCMCIA Base Reg: Window 2 */
   VUWORD  pcmcia_por2;      /* PCMCIA Option Reg: Window 2 */
   VUWORD  pcmcia_pbr3;      /* PCMCIA Base Reg: Window 3 */
   VUWORD  pcmcia_por3;      /* PCMCIA Option Reg: Window 3 */
   VUWORD  pcmcia_pbr4;      /* PCMCIA Base Reg: Window 4 */
   VUWORD  pcmcia_por4;      /* PCMCIA Option Reg: Window 4 */
   VUWORD  pcmcia_pbr5;      /* PCMCIA Base Reg: Window 5 */
   VUWORD  pcmcia_por5;      /* PCMCIA Option Reg: Window 5 */
   VUWORD  pcmcia_pbr6;      /* PCMCIA Base Reg: Window 6 */
   VUWORD  pcmcia_por6;      /* PCMCIA Option Reg: Window 6 */
   VUWORD  pcmcia_pbr7;      /* PCMCIA Base Reg: Window 7 */
   VUWORD  pcmcia_por7;      /* PCMCIA Option Reg: Window 7 */
   VUBYTE  RESERVED16[0x20];  /* Reserved area */
   VUWORD  pcmcia_pgcra;     /* PCMCIA Slot A Control  Reg */
   VUWORD  pcmcia_pgcrb;     /* PCMCIA Slot B Control  Reg */
   VUWORD  pcmcia_pscr;      /* PCMCIA Status Reg */
   VUBYTE  RESERVED17[0x4];  /* Reserved area */
   VUWORD  pcmcia_pipr;      /* PCMCIA Pins Value Reg */
   VUBYTE  RESERVED18[0x4];  /* Reserved area */
   VUWORD  pcmcia_per;       /* PCMCIA Enable Reg */
   VUBYTE  RESERVED19[0x4];  /* Reserved area */

   /*------*/
   /* MEMC */
   /*------*/

   VUWORD	memc_br0;			/* base register 0 */
   VUWORD	memc_or0;			/* option register 0 */
   VUWORD	memc_br1;			/* base register 1 */
   VUWORD	memc_or1;			/* option register 1 */
   VUWORD	memc_br2;			/* base register 2 */
   VUWORD	memc_or2;			/* option register 2 */
   VUWORD	memc_br3;			/* base register 3 */
   VUWORD	memc_or3;			/* option register 3 */
   VUWORD	memc_br4;			/* base register 4 */
   VUWORD	memc_or4;			/* option register 4 */
   VUWORD	memc_br5;			/* base register 5 */
   VUWORD	memc_or5;			/* option register 5 */
   VUWORD	memc_br6;			/* base register 6 */
   VUWORD	memc_or6;			/* option register 6 */
   VUWORD	memc_br7;			/* base register 7 */
   VUWORD	memc_or7;			/* option register 7 */
   VUBYTE 	RESERVED20[0x24];	/* Reserved area */
   VUWORD	memc_mar;			/* Memory address */
   VUWORD	memc_mcr;			/* Memory command */
   VUBYTE	RESERVED21[0x4];	/* Reserved area */
   VUWORD	memc_mamr;			/* Machine A mode */
   VUWORD	memc_mbmr;			/* Machine B mode */
   VUHWORD	memc_mstat;			/* Memory status */
   VUHWORD	memc_mptpr;			/* Memory preidic timer prescalar */
   VUWORD	memc_mdr;			/* Memory data */
   VUBYTE	RESERVED22[0x80];	/* Reserved area */

   /*---------------------------*/
   /* SYSTEM INTEGRATION TIMERS */
   /*---------------------------*/

   VUHWORD	simt_tbscr;			/* Time base stat&ctr */
   VUBYTE	RESERVED23[0x2];	/* Reserved area */
   VUWORD	simt_tbreff0;		/* Time base reference 0 */
   VUWORD	simt_tbreff1;		/* Time base reference 1 */
   VUBYTE	RESERVED24[0x14];	/* Reserved area */
   VUHWORD	simt_rtcsc;			/* Realtime clk stat&cntr 1 */
   VUBYTE	RESERVED25[0x2];	/* Reserved area */
   VUWORD	simt_rtc;			/* Realtime clock */
   VUWORD	simt_rtsec;			/* Realtime alarm seconds */
   VUWORD	simt_rtcal;			/* Realtime alarm */
   VUBYTE	RESERVED26[0x10];	/* Reserved area */
   VUWORD	simt_piscr;			/* PIT stat&ctrl */
   VUWORD	simt_pitc;			/* PIT counter */
   VUWORD	simt_pitr;			/* PIT */
   VUBYTE	RESERVED27[0x34];	/* Reserved area */

   /*---------------*/
   /* CLOCKS, RESET */
   /*---------------*/
   
   VUWORD	clkr_sccr;			/* System clk cntrl */
   VUWORD	clkr_plprcr;		/* PLL reset&ctrl */
   VUWORD	clkr_rsr;			/* reset status */
   UBYTE    RESERVED28[0x74];	/* Reserved area */

   /*--------------------------------*/
   /* System Integration Timers Keys */
   /*--------------------------------*/

   VUWORD  simt_tbscrk;   		/* Timebase Status&Ctrl Key */
   VUWORD  simt_tbreff0k; 		/* Timebase Reference 0 Key */
   VUWORD  simt_tbreff1k; 		/* Timebase Reference 1 Key */
   VUWORD  simt_tbk;      		/* Timebase and Decrementer Key */
   UBYTE   RESERVED29[0x10];	/* Reserved area */
   VUWORD  simt_rtcsck;   		/* Real-Time Clock Status&Ctrl Key */

   VUWORD  simt_rtck;     		/* Real-Time Clock Key */
   VUWORD  simt_rtseck;   		/* Real-Time Alarm Seconds Key */
   VUWORD  simt_rtcalk;   		/* Real-Time Alarm Key */
   UBYTE   RESERVED30[0x10];	/* Reserved area */
   VUWORD  simt_piscrk;   		/* Periodic Interrupt Status&Ctrl Key */
   VUWORD  simt_pitck;    		/* Periodic Interrupt Count Key */
   UBYTE   RESERVED31[0x38];	/* Reserved area */
        
   /*----------------------*/
   /* Clock and Reset Keys */
   /*----------------------*/

   VUWORD  clkr_sccrk;    	      /* System Clock Control Key */
   VUWORD  clkr_plprcrk;  	      /* PLL, Low Power and Reset Control Key */
   VUWORD  clkr_rsrk;     		   /* Reset Status Key */
   UBYTE	  RESERVED32[0x4d4];	/* Reserved area */
              
   /*-----*/
   /* I2C */
   /*-----*/
   
   VUBYTE	i2c_i2mod;			/* i2c mode */
   UBYTE		RESERVED33[3];
   VUBYTE	i2c_i2add;			/* i2c address */
   UBYTE		RESERVED34[3];
   VUBYTE	i2c_i2brg;			/* i2c brg */
   UBYTE		RESERVED35[3];
   VUBYTE	i2c_i2com;			/* i2c command */
   UBYTE		RESERVED36[3];
   VUBYTE	i2c_i2cer;			/* i2c event */
   UBYTE		RESERVED37[3];
   VUBYTE	i2c_i2cmr;			/* i2c mask */
   VUBYTE	RESERVED38[0x8b];	/* Reserved area */

   /*-----*/
   /* DMA */
   /*-----*/

   VUBYTE	RESERVED39[0x4];	/* Reserved area */
   VUWORD	dma_sdar;			/* SDMA address reg */
   VUBYTE	RESERVED40[0x2];	/* Reserved area */
   VUBYTE	dma_sdsr;			/* SDMA status reg */
   VUBYTE	RESERVED41[0x3];	/* Reserved area */
   VUBYTE	dma_sdmr;			/* SDMA mask reg */
   VUBYTE	RESERVED42[0x1];	/* Reserved area */			   
   VUBYTE	dma_idsr1;			/* IDMA1 status reg */
   VUBYTE	RESERVED43[0x3];	/* Reserved area */
   VUBYTE	dma_idmr1;			/* IDMA1 mask reg */
   VUBYTE	RESERVED44[0x3];	/* Reserved area */
   VUBYTE	dma_idsr2;			/* IDMA2 status reg */
   VUBYTE	RESERVED45[0x3];	/* Reserved area */
   VUBYTE	dma_idmr2;			/* IDMA2 mask reg */
   VUBYTE	RESERVED46[0x13];	/* Reserved area */

   /*--------------------------*/
   /* CPM Interrupt Controller */
   /*--------------------------*/

   VUHWORD	cpmi_civr;			/* CP interrupt vector reg */
   VUBYTE	RESERVED47[0xe];	/* Reserved area */
   VUWORD	cpmi_cicr;			/* CP interrupt configuration reg */
   VUWORD	cpmi_cipr;			/* CP interrupt pending reg */
   VUWORD	cpmi_cimr;			/* CP interrupt mask reg */
   VUWORD	cpmi_cisr;			/* CP interrupt in-service reg */

   /*----------*/
   /* I/O port */
   /*----------*/

   VUHWORD	pio_padir;			/* port A data direction reg */
   VUHWORD	pio_papar;			/* port A pin assignment reg */
   VUHWORD	pio_paodr;			/* port A open drain reg */
   VUHWORD	pio_padat;			/* port A data register */
   VUBYTE	RESERVED48[0x8];	/* Reserved area */
   VUHWORD	pio_pcdir;			/* port C data direction reg */
   VUHWORD	pio_pcpar;			/* port C pin assignment reg */
   VUHWORD	pio_pcso;			/* port C special options */
   VUHWORD	pio_pcdat;			/* port C data register */
   VUHWORD	pio_pcint;			/* port C interrupt cntrl reg */
   UBYTE		RESERVED49[6];
   VUHWORD	pio_pddir;			/* port D Data Direction reg */
   VUHWORD	pio_pdpar;			/* port D pin assignment reg */
   UBYTE		RESERVED50[2];
   VUHWORD	pio_pddat;			/* port D data reg */
   VUBYTE	RESERVED51[0x8];	/* Reserved area */

   /*-----------*/
   /* CPM Timer */
   /*-----------*/

   VUHWORD	timer_tgcr;			/* timer global configuration reg */
   VUBYTE	RESERVED52[0xe];	/* Reserved area */
   VUHWORD	timer_tmr1;			/* timer 1 mode reg */
   VUHWORD	timer_tmr2;			/* timer 2 mode reg */
   VUHWORD	timer_trr1;			/* timer 1 referance reg */
   VUHWORD	timer_trr2;			/* timer 2 referance reg */
   VUHWORD	timer_tcr1;			/* timer 1 capture reg */
   VUHWORD	timer_tcr2;			/* timer 2 capture reg */
   VUHWORD	timer_tcn1;			/* timer 1 counter reg */
   VUHWORD	timer_tcn2;			/* timer 2 counter reg */
   VUHWORD	timer_tmr3;			/* timer 3 mode reg */
   VHWORD	timer_tmr4;			/* timer 4 mode reg */
   VUHWORD	timer_trr3;			/* timer 3 referance reg */
   VUHWORD	timer_trr4;			/* timer 4 referance reg */
   VUHWORD	timer_tcr3;			/* timer 3 capture reg */
   VUHWORD	timer_tcr4;			/* timer 4 capture reg */
   VUHWORD	timer_tcn3;			/* timer 3 counter reg */
   VUHWORD	timer_tcn4;			/* timer 4 counter reg */
   VUHWORD	timer_ter1;			/* timer 1 event reg */
   VUHWORD	timer_ter2;			/* timer 2 event reg */
   VUHWORD	timer_ter3;			/* timer 3 event reg */
   VUHWORD	timer_ter4;			/* timer 4 event reg */
   VUBYTE	RESERVED53[0x8];	/* Reserved area */

   /*----*/
   /* CP */
   /*----*/

   VUHWORD	cp_cr;				/* command register */
   VUBYTE	RESERVED54[0x2];	/* Reserved area */
   VUHWORD	cp_rccr;				/* main configuration reg */
   VUBYTE	RESERVED55;			/* Reserved area */
   VUBYTE	cp_resv1;			/* Reserved reg */
   VUWORD	cp_resv2;			/* Reserved reg */
   VUHWORD	cp_rctr1;			/* ram break register 1 */
   VUHWORD	cp_rctr2;			/* ram break register 2 */
   VUHWORD	cp_rctr3;			/* ram break register 3 */
   VUHWORD	cp_rctr4;			/* ram break register 4 */
   VUBYTE	RESERVED56[0x2];	/* Reserved area */
   VUHWORD	cp_rter;				/* RISC timers event reg */
   VUBYTE	RESERVED57[0x2];	/* Reserved area */
   VUHWORD	cp_rtmr;				/* RISC timers mask reg */
   VUBYTE	RESERVED58[0x14];	/* Reserved area */

   /*-----*/
   /* BRG */
   /*-----*/

   VUWORD	brgc1;		/* BRG1 configuration reg */
   VUWORD	brgc2;		/* BRG2 configuration reg */
   VUWORD	brgc3;		/* BRG3 configuration reg */
   VUWORD	brgc4;		/* BRG4 configuration reg */

   /*---------------*/
   /* SCC registers */
   /*---------------*/

   struct scc_regs 
   
   {
       VUWORD	scc_gsmr_l; 		/* SCC Gen mode (LOW) */
       VUWORD	scc_gsmr_h; 		/* SCC Gen mode (HIGH) */
       VUHWORD	scc_psmr;			/* protocol specific mode register */
       VUBYTE	RESERVED59[0x2]; 	/* Reserved area */
       VUHWORD	scc_todr; 			/* SCC transmit on demand */
       VUHWORD	scc_dsr;				/* SCC data sync reg */
       VUHWORD	scc_scce;			/* SCC event reg */
       VUBYTE	RESERVED60[0x2];	/* Reserved area */
       VUHWORD	scc_sccm;			/* SCC mask reg */
       VUBYTE	RESERVED61[0x1];	/* Reserved area */
       VUBYTE	scc_sccs;			/* SCC status reg */
       VUBYTE	RESERVED62[0x8]; 	/* Reserved area */

   } scc_regs[4];

   
   /*-----*/
   /* SMC */
   /*-----*/

   struct smc_regs 
   
   {
       VUBYTE	RESERVED63[0x2]; 	/* Reserved area */
       VUHWORD	smc_smcmr;			/* SMC mode reg */
       VUBYTE	RESERVED64[0x2];	/* Reserved area */
       VUBYTE	smc_smce;			/* SMC event reg */
       VUBYTE	RESERVED65[0x3]; 	/* Reserved area */
       VUBYTE	smc_smcm;			/* SMC mask reg */
       VUBYTE	RESERVED66[0x5]; 	/* Reserved area */

   } smc_regs[2];


   /*-----*/
   /* SPI */
   /*-----*/

   VUHWORD	spi_spmode;			/* SPI mode reg */
   VUBYTE	RESERVED67[0x4];	/* Reserved area */
   VUBYTE	spi_spie;			/* SPI event reg */
   VUBYTE	RESERVED68[0x3];	/* Reserved area */
   VUBYTE	spi_spim;			/* SPI mask reg */
   VUBYTE	RESERVED69[0x2];	/* Reserved area */
   VUBYTE	spi_spcom;			/* SPI command reg */
   VUBYTE	RESERVED70[0x4];	/* Reserved area */

   /*-----*/
   /* PIP */
   /*-----*/

   VUHWORD	pip_pipc;			/* pip configuration reg */
   VUBYTE	RESERVED71[0x2];	/* Reserved area */
   VUHWORD	pip_ptpr;			/* pip timing parameters reg */
   VUWORD	pip_pbdir;			/* port b data direction reg */
   VUWORD	pip_pbpar;			/* port b pin assignment reg */
   VUBYTE	RESERVED72[0x2];	/* Reserved area */
   VUHWORD	pip_pbodr;			/* port b open drain reg */
   VUWORD	pip_pbdat;			/* port b data reg */
   VUBYTE	RESERVED73[0x18];	/* Reserved area */


   /*------------------*/
   /* Serial Interface */
   /*------------------*/

   VUWORD	si_simode;				/* SI mode register */
   VUBYTE	si_sigmr;				/* SI global mode register */
   VUBYTE	RESERVED74; 			/* Reserved area */
   VUBYTE	si_sistr;				/* SI status register */
   VUBYTE	si_sicmr;				/* SI command register */
   VUBYTE	RESERVED75[0x4]; 		/* Reserved area */
   VUWORD	si_sicr;					/* SI clock routing */
   VUWORD	si_sirp;					/* SI ram pointers */
   VUBYTE	RESERVED76[0x10c];	/* Reserved area */
   VUBYTE	si_siram[0x200]; 		/* SI routing ram */
   union
   {
		struct mpc866fec fec;			/* MPC866 FEC */
   		VUBYTE	RESERVED77[0x1200]; 	/* Reserved area */
   } fec_or_rsvd;

   /*-----------------------------------------------------------------*/
   /* BASE + 0x2000: user data memory, microcode, or QMC channel PRAM */
   /*-----------------------------------------------------------------*/
	
	union
	{
		struct qmc_chan_pram qcp[64];
		struct user_data ud;
		struct micromon_data umd;
		UBYTE RESERVED[0x1c00];
	} qcp_or_ud;


   /*-----------------------------------------------------------------------*/
   /* BASE + 0x3c00: PARAMETER RAM. This main union defines 4 memory blocks */
   /* of an identical size. See the Parameter RAM definition in the MPC860  */
   /* user's manual.                                                        */
   /*-----------------------------------------------------------------------*/

   /*------------------------*/
   /* Base + 0x3C00 (page 1) */
   /*      + 0x3D00 (page 2) */
   /*      + 0x3E00 (page 3) */
   /*      + 0x3F00 (page 4) */
   /*------------------------*/

   union 
      
   {
      struct page_of_pram 
       
      {
         /*------------------------------------------------------------*/
         /* scc parameter area - 1st memory block (protocol dependent) */
         /*------------------------------------------------------------*/

         union 
           
         {
            struct hdlc_pram	      h;
            struct uart_pram	      u;
            struct bisync_pram     	b;
            struct transparent_pram	t;
            struct async_hdlc_pram	a;
            UBYTE	RESERVED78[0x80];

         } scc;		

         /*----------------------------------------------------------------*/
         /* Other protocol areas for the rest of the memory blocks in each */
         /* page.                                                          */
         /*----------------------------------------------------------------*/

         union 
                 
         {
            /*---------------------------------------------------------------*/
            /* This structure defines the rest of the blocks on the 1st page */
            /*---------------------------------------------------------------*/ 
         
            struct 
         
            {
               struct i2c_pram	i2c;     /* I2C   */
               struct idma_pram	idma1;   /* IDMA1 */

            } i2c_idma; 

            /*---------------------------------------------------------------*/
            /* This structure defines the rest of the blocks on the 2nd page */
            /*---------------------------------------------------------------*/
 
            struct 
         
            {
               struct spi_pram	spi;     /* SPI    */
               struct timer_pram	timer;   /* Timers */
               struct idma_pram	idma2;   /* IDMA2  */

            } spi_timer_idma; 

            /*---------------------------------------------------------------*/
            /* This structure defines the rest of the blocks on the 3rd page */
            /*---------------------------------------------------------------*/
 
            struct 
         
            {
               union 
          
               {
                  struct smc_uart_pram u1;   /* SMC1 */
                  struct smc_trnsp_pram t1;  /* SMC1 */
                  UBYTE	RESERVED78[0x80];     /* declare full block */

               } psmc1; 

            } smc_dsp1;


            /*---------------------------------------------------------------*/
            /* This structure defines the rest of the blocks on the 4th page */
            /*---------------------------------------------------------------*/ 
 
            struct 
         
            {
               union 
          
               {
                  struct smc_uart_pram u2;   /* SMC2 */
                  struct smc_trnsp_pram t2;  /* SMC2 */
                  struct centronics_pram c;  /* Uses SM2's space */
                  UBYTE	RESERVED79[0x80];    /* declare full block */

               } psmc2;

            } smc_dsp2; 

            UBYTE	RESERVED80[0x80];    /* declare full block */

         } other;

      } pg;

      /*---------------------------------------------------------------*/
      /* When selecting Ethernet as protocol for an SCC, this protocol */
      /* uses a complete page of Parameter RAM memory.                 */
      /*---------------------------------------------------------------*/

      struct ethernet_pram	enet_scc;

	/*---------------------------------------------------------------*/
	/* When using QMC as a mode for an SCC, the QMC global parameter */
	/* ram uses from SCC BASE to BASE+AC.                            */
	/*---------------------------------------------------------------*/
      
      struct global_qmc_pram  gqp; 

      /*--------------------------------------------------------*/
      /* declaration to guarantee a page of memory is allocated */
      /*--------------------------------------------------------*/

      UBYTE   RESERVED83[0x100]; 

   } PRAM[4]; /* end of union */

} EPPC;


/***************************************************************************/
/*                   General Global Definitions                            */
/***************************************************************************/


#define PAGE1 		0	/* SCC1 Index into SCC Param RAM Array */
#define PAGE2 		1	/* SCC2 Index into SCC Param RAM Array */
#define PAGE3 		2	/* SCC3 Index into SCC Param RAM Array */
#define PAGE4 		3	/* SCC4 Index into SCC Param RAM Array */

#define SMC1_REG 		0 	/* SMC1 Index into SMC Regs Array  */				
#define SMC2_REG 		1 	/* SMC2 Index into SMC Regs Array  */				

#define SCC1_REG 		0 	/* SCC1 Index into SCC Regs Array  */				
#define SCC2_REG 		1 	/* SCC2 Index into SCC Regs Array  */				
#define SCC3_REG 		2 	/* SCC3 Index into SCC Regs Array  */				
#define SCC4_REG 		3 	/* SCC4 Index into SCC Regs Array  */				


/*--------------------------------*/
/* KEEP ALIVE POWER REGISTERS KEY */
/*--------------------------------*/

#define KEEP_ALIVE_KEY 0x55ccaa33




