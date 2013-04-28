/* Copyright 2013, Qualcomm Atheros, Inc. */
/*
All files except if stated otherwise in the begining of the file are under the GPLv2 license:
-----------------------------------------------------------------------------------

Copyright (c) 2010-2012 Design Art Networks Ltd.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

-----------------------------------------------------------------------------------
*/
/**********************************************************************\
 Copyright	:  Design Art Networks (c) 2010
 Library    :  uMON (Micro Monitor)
 Filename   :  spi.c
 Purpose    :  Simple SPI driver for uMON only.
 Owner		:  Albert Yosher
 Created    :  18.08.2010
\**********************************************************************/

#define TRACE_ENABLED_PRINTF

#include "config.h"
#include "genlib.h"
#include "CGEN.h"
#include "common.h"
#include "spi.h"
#include "trc.h"

unsigned SPI_divider;



/*
 *  Wait on Tx FIFO level till it will be empty, waiting end of transaction.
 *  KS 5.7.12: We do not need to call it at the end of SPI_process because it works always in 
 *  duplex mode, so we always do Rx after Tx for the same number of bytes, so when SPI_process 
 *  ends the FIFO must be always empty.
 */
#define SPI_CTRLR_WaitTxFifoEmpty_						\
	{													\
	UINT32 val;                                     	\
	do {												\
		SPI_READ_REG(Ssi_Sr_Offset,val);				\
	}													\
    while ((val & 0x5) != 4 );							\
    }


#define SPI_LOCAL_BUF_SIZE		128		// Should be complained with flash FIFO size FLASH_FIFO_SIZE
#define SPI_MAX_CMD_BUF_SIZE	  5		// Should be complained with flash FIFO size FLASH_FIFO_SIZE


void SPI_process 
	(
		uint8		bank,
		unsigned	len_in, 
		unsigned	len_out1,
		unsigned	len_out2, 
		uint8*		inbuf, 
		uint8*		outbuf1,
		uint8*		outbuf2,
		unsigned	in_skip_start, 
		unsigned	in_skip_end
	)
{
	uint8	out_array1[SPI_MAX_CMD_BUF_SIZE];
	uint8	out_array2[SPI_LOCAL_BUF_SIZE];
	uint8*	out_buff1 = out_array1;
	uint8*	out_buff2 = out_array2;

	unsigned i, data_read = 0;
	uint8    dummy = 0xff;
	int      len_dif;

	TRCFUNS();
	TRCTRC ("len_in = 0x%x", len_in);
	TRCTRC ("inbuf  = 0x%x", inbuf);

	// Copy outbuf1 & outbuf2 to the local DDR buffer.
	// It solves the problem of slow DDR in some circumstances. Done by IgorL.
	// SergeyK. 29.6.12
	if (outbuf1 && (len_out1 <= SPI_MAX_CMD_BUF_SIZE))
	{
		memcpy((void*)out_buff1, (void*)outbuf1, len_out1);
	}

	if (outbuf2 && (len_out1 <= SPI_LOCAL_BUF_SIZE))
	{
		memcpy((void*)out_buff2, (void*)outbuf2, len_out2);
	}
	

    // Enable SPI
	SPI_enable(bank);

	// Write data from the out_buff1
	TRCTRC ("write from out_buff1, len_out1=%X", len_out1);
	for (i = 0; i < len_out1; i++) 
	{
		SPI_WRITE_REG (Ssi_Dr_Low_Offset, *out_buff1++);
	}

	// Write data from the out_buff2
	TRCTRC ("write from out_buff2, len_out2=%X", len_out2);
	for (i = 0; i < len_out2; i++) 
	{
		SPI_WRITE_REG (Ssi_Dr_Low_Offset, *out_buff2++);
	}

	len_dif = (int)len_in - (int)(len_out1 + len_out2);
	// ASSERT (len_dif >=0)

	// Write dummy data to the FIFO
	TRCTRC ("write dummy, len_dif=%X", len_dif);
	for (i = 0; i < (unsigned)len_dif; i++) 
	{
		SPI_WRITE_REG (Ssi_Dr_Low_Offset, dummy);
	}

	// Poll status (level) of the input FIFO till it has all data
	do
	{
		unsigned ready_data;
		SPI_READ_REG (Ssi_Rxflr_Offset, ready_data);

		if (ready_data && (data_read < in_skip_start))
		{
			TRCTRC ("*1* ready_data=%X", ready_data);
			unsigned limit = MIN(data_read + ready_data, in_skip_start);
			ready_data -= limit - data_read;
			// Skip start bytes if needed (dummy bytes)
			for (; (data_read < limit); data_read++)
			{
				SPI_READ_REG (Ssi_Dr_Low_Offset, dummy);
			}
		}
		if (ready_data && (data_read < (len_in - in_skip_end)))
		{
			//TRCTRC ("*2* ready_data=%X", ready_data);
			unsigned limit = MIN(data_read + ready_data, (len_in - in_skip_end));
			ready_data -= limit - data_read;
			// Read input data
			for (; data_read < limit; data_read++)
			{
				uint8  data;
				SPI_READ_REG(Ssi_Dr_Low_Offset, data);
				*inbuf++ = data;
			}
		}
		if (ready_data && (data_read < len_in))
		{
			TRCTRC ("*3* ready_data=%X", ready_data);
			unsigned limit = MIN(data_read + ready_data, len_in);
			ready_data -= limit - data_read;
			// Skip end bytes if needed (dummy bytes)
			for (; data_read < limit; data_read++)
			{
				SPI_READ_REG(Ssi_Dr_Low_Offset, dummy);
			}
		}
	}
	while (data_read < len_in);

	TRCSTR ("before SPI_CTRLR_WaitTxFifoEmpty_");
	SPI_CTRLR_WaitTxFifoEmpty_;
	
	// Disable SPI
	SPI_disable();

	TRCFUNE();
}

