#define SPI_ENABLE()	*pSPI_CTL |= SPE
#define SPI_DISABLE()	*pSPI_CTL &= ~SPE

#define SPI_WRITE_MODE()	\
	*pSPI_CTL &= ~TDBR_CORE	// RDBR-read initiates transaction

#define SPI_READ_MODE()		\
	*pSPI_CTL |= TDBR_CORE	// TDBR-write initiates transaction

extern int spi_finwait(void);
extern int spi_txswait(void);
extern int spi_rxswait(void);

#define SPI_FINWAIT()	{ if (spi_finwait() == -1) return(-1); }
#define SPI_RXSWAIT()	{ if (spi_rxswait() == -1) return(-1); }
#define SPI_TXSWAIT()	{ if (spi_txswait() == -1) return(-1); }
