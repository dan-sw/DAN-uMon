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

#include "config.h"
#include "CGEN.h"
#include "common.h"
#include "spi.h"


unsigned SPI_divider;



/*
 *  Wait on Tx FIFO level till it will be empty, waiting end of tranzaction
 */
#define SPI_CTRLR_WaitTxFifoEmpty_						\
	{													\
	UINT32 val;                                     	\
	do {												\
		SPI_READ_REG(Ssi_Sr_Offset,val);				\
	}													\
    while ((val & 0x5) != 4 );							\
    }


#define SPI_LOCAL_BUF_SIZE		128	/* Should be complined with flash FIFO size FLASH_FIFO_SIZE */
#define SPI_MAX_CMD_BUF_SIZE	5	/* Should be complined with flash FIFO size FLASH_FIFO_SIZE */

void SPI_process 
	(
		uint8		bank,
		unsigned	len_in, 
		unsigned	len_out1,
		unsigned	len_out2, 
		uint8		*inbuf, 
		uint8		*outbuf1,
		uint8		*outbuf2,
		unsigned	in_skip_start, 
		unsigned	in_skip_end
	)
{
	uint8 out_array1[SPI_MAX_CMD_BUF_SIZE];
	uint8 out_array2[SPI_LOCAL_BUF_SIZE];
	uint8 *out_buff1 = out_array1;
	uint8 *out_buff2 = out_array2;
	unsigned i, data_read = 0;
	uint8    dummy = 0xff;
	int      len_dif;

	if(outbuf1 && (len_out1 <= SPI_MAX_CMD_BUF_SIZE))
	{
		memcpy(out_buff1, (void *)outbuf1, len_out1);
	}
	
	if(outbuf2 && (len_out1 <= SPI_LOCAL_BUF_SIZE))
	{
		memcpy(out_buff2, (void *)outbuf2, len_out2);
	}
	
#if 0 /* RAN, temp solution */
	volatile int zz = (int)(*outbuf1 + *outbuf2);
	if(zz) {
		zz=0;
	}
#endif 
    // Enable SPI
	SPI_enable(bank);

	// Write data from the out_buff1
	for (i = 0; i < len_out1; i++) 
	{
		SPI_WRITE_REG (Ssi_Dr_Low_Offset, *out_buff1++);
	}

	// Write data from the out_buff2
	for (i = 0; i < len_out2; i++) 
	{
		SPI_WRITE_REG (Ssi_Dr_Low_Offset, *out_buff2++);
	}

	len_dif = (int)len_in - (int)(len_out1 + len_out2);
	// ASSERT (len_dif >=0)

	// Write dummy data to the FIFO
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

	
	SPI_CTRLR_WaitTxFifoEmpty_;
	
	// Disable SPI
	SPI_disable();
}

