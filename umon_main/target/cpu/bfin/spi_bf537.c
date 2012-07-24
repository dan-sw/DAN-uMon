#include "config.h"
#include "cdefBF537.h"
#include "genlib.h"
#include "spi_bf537.h"

#define SPI_ENABLE()	*pSPI_CTL |= SPE
#define SPI_DISABLE()	*pSPI_CTL &= ~SPE

#define SPI_WRITE_MODE()	\
	*pSPI_CTL &= ~TDBR_CORE	// RDBR-read initiates transaction

#define SPI_READ_MODE()		\
	*pSPI_CTL |= TDBR_CORE	// TDBR-write initiates transaction

/* spi_finwait():
 * Wait for SPI finish flag.
 */
int
spi_finwait(void)
{
	volatile int i;
	volatile unsigned short stat;

	__builtin_bfin_ssync();
	for(i=0;i<200000;i++) {
		stat = *pSPI_STAT;
		__builtin_bfin_ssync();
		if (stat & SPIF)
			break;
	} 
	if (i == 200000) {
		printf("SPIF wait timeout\n");
		SPI_DISABLE();
		__builtin_bfin_ssync();
		return(-1);
	}

	__builtin_bfin_ssync();
	return(0);
}

/* spi_txswait():
 * Wait for transmit buffer empty.
 */
int
spi_txswait(void)
{
	volatile int i;
	volatile unsigned short stat;

	__builtin_bfin_ssync();
	for(i=0;i<200000;i++) {
		stat = *pSPI_STAT;
		__builtin_bfin_ssync();
		if ((stat & TXS) == 0)
			break;
	} 
	if (i == 200000) {
		printf("SPI_TXS wait timeout\n");
		SPI_DISABLE();
		__builtin_bfin_ssync();
		return(-1);
	}

	__builtin_bfin_ssync();
	return(0);
}

/* spi_rxswait():
 * Wait for receive buffer full.
 */
int
spi_rxswait(void)
{
	volatile int i;
	volatile unsigned short stat;

	__builtin_bfin_ssync();
	for(i=0;i<200000;i++) {
		stat = *pSPI_STAT;
		__builtin_bfin_ssync();
		if (stat & RXS)
			break;
	} 
	if (i == 200000) {
		printf("SPI_RXS wait timeout\n");
		SPI_DISABLE();
		__builtin_bfin_ssync();
		return(-1);
	}

	__builtin_bfin_ssync();
	return(0);
}
