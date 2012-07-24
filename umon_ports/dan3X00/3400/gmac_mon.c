/**********************************************************************\
 Library    :  uMON (Micro Monitor)
 Filename   :  gmac_mon.h
 Purpose    :  DAN3400 GMAC Ethernet driver for uMON only
 Owner		:  Sergey Krasnitsky
 Created    :  21.11.2010
\**********************************************************************/

#include "config.h"
#include "genlib.h"
#include "gmac_mon.h"
#include "cpu.h"
#include "queue.h"


/**********************************************************************\
							Local definitions
\**********************************************************************/

#define GMAC_REG_WRITE(reg, data) 				(*(volatile uint32*)(GMAC_BASE + (reg)) = (uint32)(data))
#define GMAC_REG_READ(reg) 						(*(volatile uint32*)(GMAC_BASE + (reg)))

#define GMAC_DMAREG_WRITE(reg, data)			(*(volatile uint32*)(GMAC_BASE + GMAC_DMA_OFFSET + reg) = (uint32)(data))
#define GMAC_DMAREG_READ(reg)					(*(volatile uint32*)(GMAC_BASE + GMAC_DMA_OFFSET + reg))
#define GMAC_DMAREG_SET(reg, data) 				(*(volatile uint32*)(GMAC_BASE + GMAC_DMA_OFFSET + reg) |= (uint32)(data))

#ifdef CPU_BE
#define DESC_READ(desc, field)                  swap_bytes((desc)->field)
#define DESC_WRITE(desc, field, value)          (desc)->field  =  swap_bytes((uint32)(value))
#define DESC_UPDATE_OR(desc, field, value)      (desc)->field |=  swap_bytes((uint32)(value))
#define DESC_UPDATE_AND(desc, field, value)     (desc)->field &=  swap_bytes((uint32)(value))
#define DESC_UPDATE_NAND(desc, field, value)    (desc)->field &= ~swap_bytes((uint32)(value))
#else
#define DESC_READ(desc, field)					((desc)->field)
#define DESC_WRITE(desc, field, value)			(desc)->field  =  ((uint32)(value))
#define DESC_UPDATE_OR(desc, field, value)		(desc)->field |=  ((uint32)(value))
#define DESC_UPDATE_AND(desc, field, value)		(desc)->field &=  ((uint32)(value))
#define DESC_UPDATE_NAND(desc, field, value)	(desc)->field &= ~((uint32)(value))
#endif

#define GMAC_BUF_REP_SIZE						0x0610	// KS: ???

typedef struct
{
	SNODE_t	 node;
	uint8	*buf;
} GmacBufDesc;

uint8 GMAC_BUFs[GMAC_BUF_NUM * GMAC_BUF_SIZE] 
	__attribute__((aligned(CACHE_LINE_SIZE)));
GmacDmaDesc GMAC_DESCs[GMAC_RX_DESC_NUM + GMAC_TX_DESC_NUM] 
	__attribute__((aligned(CACHE_LINE_SIZE)));


/**********************************************************************\
							Static data
\**********************************************************************/

static const uint8	gmac_AddrMac[6];
static const uint8	gmac_AddrBcast[6] = {0xff,0xff,0xff,0xff,0xff,0xff};

static GmacDmaDesc *gmac_DmaDescRx;
static GmacDmaDesc *gmac_DmaDescTx;

volatile GmacDmaDesc *gmac_DmaDescRxIter;
volatile GmacDmaDesc *gmac_DmaDescTxIter;

static SQUEUE_t		gmac_FreeList;
static GmacBufDesc  gmac_DescBuf[GMAC_BUF_NUM];



/**********************************************************************\
							Static functions
\**********************************************************************/

#ifdef CPU_BE
static inline uint32 swap_bytes(uint32 value)
{
	uint8 b0, b1, b2, b3;

	b3 = (uint8)((value >>  0) & 0xFF);
	b2 = (uint8)((value >>  8) & 0xFF);
	b1 = (uint8)((value >> 16) & 0xFF);
	b0 = (uint8)((value >> 24) & 0xFF);
	return (b0 | (b1 << 8) | (b2 << 16) | (b3 << 24));
}
#endif


#ifdef DAN2400
void DAN_DSM_switch_dirty_config()
{
//    if (BOARD_HW_REV >= 1) return;

    *(UINT32 volatile *)0x20008714 = 0x8403;
    *(UINT32 volatile *)0x20008710 = 0xf843;
    while (*(UINT32 volatile *)0x20008710 & 1);

    *(UINT32 volatile *)0x20008714 = 0x9674;
    *(UINT32 volatile *)0x20008710 = 0xf803;
    while (*(UINT32 volatile *)0x20008710 & 1);

    do
    {
        *(UINT32 volatile *)0x20008710 = 0xf801;
        while (*(UINT32 volatile *)0x20008710 & 1);
    }
    while (*(UINT32 volatile *)0x20008714 & 0x8000);

    *(UINT32 volatile *)0x20008714 = 0x6E88;
    *(UINT32 volatile *)0x20008710 = 0xf843;
    while (*(UINT32 volatile *)0x20008710 & 1);

    *(UINT32 volatile *)0x20008714 = 0x9660;
    *(UINT32 volatile *)0x20008710 = 0xf803;
    while (*(UINT32 volatile *)0x20008710 & 1);

    do
    {
        *(UINT32 volatile *)0x20008710 = 0xf801;
        while (*(UINT32 volatile *)0x20008710 & 1);
    }
    while (*(UINT32 volatile *)0x20008714 & 0x8000);
}
#endif


static inline void InitDmaDescRx(volatile GmacDmaDesc *Desc, volatile GmacDmaDesc *base, volatile GmacDmaDesc *next, char EndOfRing)
{
	DESC_WRITE(Desc, status, 0);
#ifdef DAN2400
	DESC_WRITE(Desc, length, DescChain);
#else
	DESC_WRITE(Desc, length, RxDescChain);
#endif
	DESC_WRITE(Desc, buffer1, 0);
	DESC_WRITE(Desc, buffer2, EndOfRing ? base : next);
	DESC_WRITE(Desc, data1, 0);
	DESC_WRITE(Desc, data2, 0);
}


static inline void InitDmaDescTx(volatile GmacDmaDesc *Desc, volatile GmacDmaDesc *base, volatile GmacDmaDesc *next, char EndOfRing)
{
#ifdef DAN2400
    DESC_WRITE(Desc, status, 0);
	DESC_WRITE(Desc, length, DescChain);
#else
    DESC_WRITE(Desc, status, TxDescChain);
	DESC_WRITE(Desc, length, 0);
#endif
	DESC_WRITE(Desc, buffer1, 0);
	DESC_WRITE(Desc, buffer2, EndOfRing ? base : next);
	DESC_WRITE(Desc, data1, 0);
	DESC_WRITE(Desc, data2, 0);
}


static inline char IsDescDma(volatile GmacDmaDesc *Desc)
{
	return ((DESC_READ(Desc, status) & DescOwnByDma) != 0);   /* Owned by DMA */
}


static inline char IsDescEmpty(volatile GmacDmaDesc *Desc)
{
#ifdef DAN2400
	return ((DESC_READ(Desc,length) & ~ (DescEndOfRing | DescChain)) == 0);
#else
	return ((DESC_READ(Desc,length) & ~ (RxDescEndOfRing | RxDescChain)) == 0);
#endif
}


static inline int gmac_CalcMacAddrHash (const uint8 addr[6])
{
	int			i;
	unsigned	crc  = 0xFFFFFFFF;
	unsigned	poly = 0xEDB88320;

	for (i = 0; i < 6; i++)
	{
		int bit;
		uint8 data = addr[i];
		for (bit = 0; bit < 8; bit++)
		{
			int p = (crc^data) & 1;
			crc >>= 1;
			if( p != 0 ) crc ^= poly;
			data >>= 1;
		}
	}
	return (crc>>26) & 0x3F;  // return upper 6 bits
}

static int gmac_mdio_read_reg
(
	uint8							phy,			// PHY address
	uint8							reg,			// MDIO Register to read
	uint32							retries			// Maximum wait retries
)
{
	int data = -1, temp;

	GMAC_REG_WRITE(GmacGmiiAddr,
				((((uint32)reg) << GmiiRegShift) & GmiiRegMask) |
				((((uint32)phy) << GmiiDevShift) & GmiiDevMask) |
				GmiiAppClk2 | GmiiRead);
	do
	{
		temp = GMAC_REG_READ(GmacGmiiAddr);
	} while((temp & GmiiBusy) && retries--);

	if (retries)
	{
#pragma frequency_hint FREQUENT
		data = GMAC_REG_READ(GmacGmiiData);
	}

	return (data);
}

/*
 * -----------------------------------------------------------
 * Function:	GMAC_HW_mdio_write_reg
 * Description:	Write to the MDIO register data (16 bit)
 * Output:		0 if success, -1 if failure
 * -----------------------------------------------------------
 */
static int gmac_mdio_write_reg
(
	uint8							phy,			// PHY address
	uint8							reg,			// MDIO Register to write
	uint16							data,			// Data to write
	uint32							retries			// Maximum wait retries
)
{
	volatile uint32 temp;
	GMAC_REG_WRITE(GmacGmiiData, data);
	GMAC_REG_WRITE(GmacGmiiAddr,
				(((((uint32)reg) << GmiiRegShift) & GmiiRegMask) |
				((((uint32)phy) << GmiiDevShift) & GmiiDevMask) |
				GmiiWrite));
#pragma flush
	do
	{
		temp = 	GMAC_REG_READ(GmacGmiiAddr);
	} while( (temp & GmiiBusy) && retries--);
	return (retries ? 0 : -1);
}

static int gmac_phy_hw_init(uint8 phy)
{
	int rc;
	uint16 data;
	rc = gmac_mdio_read_reg(phy, GMAC_PHY_CRT_EXT_REG_OFFSET, PHY_RETRIES);
	if (rc >= 0)
	{
		data = rc;
		data |= PHY_CRT_EXT_REG_TX_DELAY | PHY_CRT_EXT_REG_RX_DELAY;
		rc = gmac_mdio_write_reg(phy, GMAC_PHY_CRT_EXT_REG_OFFSET,
				data, PHY_RETRIES);
	}
	return (rc);
}

/**********************************************************************\
							Public functions
\**********************************************************************/

void gmac_Init (uint8 gmacid, const uint8 macaddr[6])
{
	int			n, i;
	unsigned	buffer;

	gmac_DmaDescRx = (GmacDmaDesc*) GMAC_DESC_ADDR;
	gmac_DmaDescTx = gmac_DmaDescRx + GMAC_RX_DESC_NUM;

    SQUEUE_init(&gmac_FreeList);
	for (i = 0, buffer = GMAC_BUF_ADDR; i < GMAC_BUF_NUM; i++, buffer += GMAC_BUF_SIZE)
	{
		gmac_DescBuf[i].buf = (uint8*) buffer;
		SQUEUE_enqueue (&gmac_FreeList, &(gmac_DescBuf[i].node));
	}

	gmac_DmaDescRxIter = gmac_DmaDescRx;
	gmac_DmaDescTxIter = gmac_DmaDescTx;

	for (i = 0; i < GMAC_RX_DESC_NUM; i++)
	{
		uint8* buf_ptr = gmac_PacketAlloc();
		InitDmaDescRx(gmac_DmaDescRx + i, gmac_DmaDescRx, gmac_DmaDescRx + i + 1, i==GMAC_RX_DESC_NUM-1);
#ifdef DAN2400
		DESC_UPDATE_AND	(gmac_DmaDescRx + i, length, DescEndOfRing | DescChain);
#else
		DESC_UPDATE_AND	(gmac_DmaDescRx + i, length, RxDescEndOfRing | RxDescChain);
#endif
		DESC_UPDATE_OR	(gmac_DmaDescRx + i, length, (((GMAC_BUF_REP_SIZE) << DescSize1Shift) & DescSize1Mask));
		DESC_WRITE		(gmac_DmaDescRx + i, buffer1, buf_ptr);
		DESC_WRITE		(gmac_DmaDescRx + i, status, DescOwnByDma);
	}

	for (i = 0; i < GMAC_TX_DESC_NUM; i++)
	{
		InitDmaDescTx(gmac_DmaDescTx + i, gmac_DmaDescTx, gmac_DmaDescTx + i + 1, i==GMAC_TX_DESC_NUM-1);
	}

    PLATFORM_cache_control((void *)GMAC_BUF_ADDR, GMAC_BUF_NUM * GMAC_BUF_SIZE, 
        CACHE_OP_FLUSH | CACHE_OP_INVALIDATE);
    PLATFORM_cache_control((void *)GMAC_DESC_ADDR, 
        sizeof(GmacDmaDesc) * (GMAC_RX_DESC_NUM + GMAC_TX_DESC_NUM), 
        CACHE_OP_FLUSH  | CACHE_OP_INVALIDATE);

#ifdef DAN2400
    DAN_DSM_switch_dirty_config();
#endif

	GMAC_DMAREG_WRITE(DmaBusMode, DmaResetOn ); /* Reset DMA engine */
	/* wait for the reset to complete */
	for (i = 0; GMAC_DMAREG_READ(DmaBusMode) & DmaResetOn; i++)
		CPU_WaitCycles (400000);
 
	GMAC_DMAREG_WRITE (DmaBusMode,		DmaBusModeInit	   );
	GMAC_DMAREG_WRITE (DmaRxBaseAddr,	gmac_DmaDescRx	   );
	GMAC_DMAREG_WRITE (DmaTxBaseAddr,	gmac_DmaDescTx	   );
    GMAC_DMAREG_WRITE (DmaControl,		DmaControlInit1000 );
	GMAC_DMAREG_WRITE (DmaInterrupt,	DmaIntDisable	   );

	// ***********************************************************************
	// Set MAC addresses

	memcpy((char*)gmac_AddrMac, (char*)macaddr, sizeof(macaddr));

	GMAC_REG_WRITE (GmacAddr0High, macaddr[5]<<8  | macaddr[4]);
	GMAC_REG_WRITE (GmacAddr0Low,  macaddr[3]<<24 | macaddr[2]<<16 | macaddr[1]<<8 | macaddr[0]);

	/* set multicast  MAC address 01:00:5e:00:00:00 with mask  00:00:00:FF:FF:FF*/
	GMAC_REG_WRITE (GmacAddr1High, 0xb8000000);
	GMAC_REG_WRITE (GmacAddr1Low,  0x005e0001);

	/* clear hash table */
	GMAC_REG_WRITE (GmacHashHigh, 0);
	GMAC_REG_WRITE (GmacHashLow,  0);

	/* set the broadcast address */
	n = gmac_CalcMacAddrHash(gmac_AddrBcast);
	if (n>=32) GMAC_REG_WRITE (GmacHashHigh, 1<<(n-32) );
	else	   GMAC_REG_WRITE (GmacHashLow,  1<<n      );

    GMAC_REG_WRITE (GmacConfig,		 GmacConfigInitFdx1000 /*GmacConfigInitFdx110*/);
	GMAC_REG_WRITE (GmacFrameFilter, GmacFrameFilterInitFdx );
	GMAC_REG_WRITE (GmacFlowControl, GmacFlowControlInitFdx );
	GMAC_REG_WRITE (GmacGmiiAddr,	 GmacGmiiAddrInitFdx    );
	// ***********************************************************************

#ifndef DAN2400
    gmac_phy_hw_init(PHY_DEV_ADDR);
#endif

	GMAC_DMAREG_SET (DmaControl, DmaRxStart | DmaTxStart);
}


void gmac_Send(uint8 gmacid, uint8 *buf, int len)
{
	int		counter = 0;
	uint8  *buf2free = buf;
    
    do 
    {
        PLATFORM_cache_control((void *)gmac_DmaDescTxIter, sizeof(GmacDmaDesc), CACHE_OP_INVALIDATE);
    }
	while (IsDescDma(gmac_DmaDescTxIter) && counter++ < 100000);

	if (buf && len)
	{
        BOOL lasttx;
		buf2free = (uint8*) DESC_READ(gmac_DmaDescTxIter, buffer1);
		lasttx   = (gmac_DmaDescTxIter == (gmac_DmaDescTx + GMAC_TX_DESC_NUM - 1));

        PLATFORM_cache_control((void *)buf, len, CACHE_OP_FLUSH);

		// Packet OK, let us send it
#ifdef DAN2400
		DESC_WRITE (gmac_DmaDescTxIter, length, (((len << DescSize1Shift) & DescSize1Mask) | 
            DescChain |  DescTxLast | DescTxFirst));
        DESC_WRITE (gmac_DmaDescTxIter, buffer1, buf);
        DESC_WRITE (gmac_DmaDescTxIter, status, DescOwnByDma);
#else
		DESC_WRITE (gmac_DmaDescTxIter, length, ((len << DescSize1Shift) & DescSize1Mask));
        DESC_WRITE (gmac_DmaDescTxIter, buffer1, buf);
        DESC_WRITE (gmac_DmaDescTxIter, status, DescOwnByDma | TxDescLast | TxDescFirst | TxDescChain);
#endif
        PLATFORM_cache_control((void *)gmac_DmaDescTxIter, sizeof(GmacDmaDesc), CACHE_OP_FLUSH);
		GMAC_DMAREG_WRITE (DmaTxPollDemand, 0);

		// No need to wait sending, we have enough buffers 
		// while (IsDescDma(gmac_DmaDescTxIter));
		
		gmac_DmaDescTxIter = lasttx ? gmac_DmaDescTx : (gmac_DmaDescTxIter + 1);
	}
	gmac_PacketFree(buf2free);
}


uint8* gmac_Recv(uint8 gmacid, unsigned * retlen)
{
	uint8 *retbuf = 0;
    
    PLATFORM_cache_control((void *)gmac_DmaDescRxIter, sizeof(GmacDmaDesc), CACHE_OP_INVALIDATE);
    PLATFORM_cache_control((void *)gmac_DmaDescTxIter, sizeof(GmacDmaDesc), CACHE_OP_INVALIDATE);
	if (!IsDescDma(gmac_DmaDescRxIter) && !IsDescEmpty(gmac_DmaDescRxIter) && !IsDescDma(gmac_DmaDescTxIter))
	{
		BOOL	  lastrx = (gmac_DmaDescRxIter == (gmac_DmaDescRx + GMAC_RX_DESC_NUM - 1));
		unsigned  sts = DESC_READ(gmac_DmaDescRxIter, status);
		unsigned  buf = DESC_READ(gmac_DmaDescRxIter, buffer1);
		unsigned  len = (sts & DescFrameLengthMask) >> DescFrameLengthShift;

		ASSERT (!(sts & DescError));

        PLATFORM_cache_control((void *)buf, len, CACHE_OP_INVALIDATE);

		retbuf  = (uint8*) buf;
		*retlen = len;

		buf = (unsigned) gmac_PacketAlloc();
		ASSERT(buf);

		DESC_WRITE(gmac_DmaDescRxIter, buffer1, buf);
		DESC_WRITE(gmac_DmaDescRxIter, status, DescOwnByDma);

        PLATFORM_cache_control((void *)gmac_DmaDescRxIter, sizeof(GmacDmaDesc), 
            CACHE_OP_FLUSH | CACHE_OP_INVALIDATE);

        GMAC_DMAREG_WRITE(DmaRxPollDemand, 0);
        gmac_DmaDescRxIter = lastrx ? gmac_DmaDescRx : (gmac_DmaDescRxIter + 1);
	}

	return retbuf;
}


uint8 *gmac_PacketAlloc (void)
{
	GmacBufDesc *bufdesc = (GmacBufDesc*) SQUEUE_dequeue(&gmac_FreeList);
	return (bufdesc) ? bufdesc->buf : 0;
}


void gmac_PacketFree (uint8 *buf)
{
	if (buf) {
		GmacBufDesc *bufdesc = gmac_DescBuf + ((unsigned)buf - GMAC_BUF_ADDR) / GMAC_BUF_SIZE;
		SQUEUE_enqueue (&gmac_FreeList, &(bufdesc->node));
	}
}

