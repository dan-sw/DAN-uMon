#include "config.h"
#include "stddefs.h"
#include "genlib.h"
#include "common.h"
#include "cpu.h"
#include "uart16550.h" 
#include "CGEN.h"
#include "tmr.h" 
#include "ddr.h" 
  
#define TTB_ADDR_DEFAULT	(0x10004000)
#define	ROUND_UP_DIV(x,y)	(((x) + (y) - 1) / (y))
#define UMON_LOAD_ADDR 		0x00000000

#ifndef USE_NPU_CB_ONLY 
#ifndef DDR_CLOCK
#error No DDR clock is defined
#endif
#else
#define DDR_CLOCK 0 
#endif

extern unsigned char input_data; 
extern unsigned char input_data_end;
extern int decompress(char *src, int srclen, char *dest);
extern void CPU_cache_on(void);

unsigned uartdrv_CpuBusClock;

#ifndef DAN2400

// This function must be supplied for the uart16550 driver
// We will always use Baud rate = 115200, it simplifies this func implementation
// The CPU_BusClock variable is defined in CPU file.
int getUartDivisor (int baud, unsigned char *hi, unsigned char *lo) 
{
	int bus_clock = CPU_SystemClock / 2; 
	int divisor =
			(bus_clock + (8 * DEFAULT_BAUD_RATE)) / (16 * DEFAULT_BAUD_RATE);
	*lo = (unsigned char) (divisor);
	*hi = (unsigned char) (divisor >> 8); 
	return 0;
}

#endif

int flushDcache(char *address, int len)  
{
	PLATFORM_cache_control(address, len, CACHE_OP_FLUSH);
	PLATFORM_write_buffer_drain();
	return (0);
}

int invalidateIcache(char *address, int len)
{
	PLATFORM_cache_control(address, len, CACHE_OP_INVALIDATE);
	PLATFORM_write_buffer_drain();
	return (0);
}

unsigned long getAppRamStart(void)
{
	return (0);
}

void *tfsstat(char *tmp)
{
	return (NULL);
}

int shell_sprintf(char *varname, char *fmt, ...)
{
	return (0);
}

typedef char * caddr_t;

extern int _end;

/* just in case, most boards have at least some memory */
#ifndef RAMSIZE
#  define RAMSIZE             (caddr_t)0x100000
#endif

/*
 * sbrk -- changes heap size size. Get nbytes more
 *         RAM. We just increment a pointer in what's
 *         left of memory on the board.
 */
caddr_t _sbrk(int nbytes)
{
	static caddr_t heap_ptr = NULL;
	caddr_t        base;

	if (heap_ptr == NULL)
	{
		heap_ptr = (caddr_t)&_end;
		base = heap_ptr;
	}

	if ((RAMSIZE - (heap_ptr - base)) >= 0)
	{
		caddr_t retval = heap_ptr;
		heap_ptr += nbytes;
		return (retval);
	} else {
		return ((caddr_t)-1);
	}
}

void _write(void)
{
}

void _close(void)
{
}

void _fstat(void)
{
}

void _isatty(void)
{
}

void _lseek(void)
{
}

void _read(void)
{
}

extern char CPU_name_buf[];
int umon_main(int argc,char *argv[])
{
	int (*umon_entry)(int argc, char *argv[]) = (void *)UMON_LOAD_ADDR;  
	unsigned len = ROUND_UP_DIV(&input_data_end - &input_data, 32) * 32;
	
#ifdef USE_DBG_GPIO	
	uint32 test_counter, TEST_DBG_ADDR = 0xe000f000;				//DBG
#endif	
	
#ifdef USE_DBG_GPIO	
	RegWrite32(GPIO_SWPORTA_DR,0xFFFFFFFF);
	RegWrite32(GPIO_SWPORTA_DDR,0xFFFFFFFF);
	RegWrite32(GPIO_SWPORTA_CTL,0);
#endif	
	
#ifndef USE_NPU_CB_ONLY   
	
	DDR_init_all(); 

#else

	SOC_init(); 
	CPU_Init(); 
	InitUART(DEFAULT_BAUD_RATE);  
	
	DAN_puts("\r\nBooting...\r\n");
	
#endif 	 // USE_NPU_CB_ONLY 
	
	/* USE_NPU_CB_ONLY */
	//KS 26.09.11 See the comment inside CGEN_PLL0_init
	//CGEN_PLL0_init(0, FALSE);
	
	CPU_cache_on();
	
	WDT_init(WDT_TIME_OUT_UMON_600);
	
	len = decompress((void *)&input_data, len, (void *)UMON_LOAD_ADDR);
	flushDcache((void *)UMON_LOAD_ADDR, len); 
	
	invalidateIcache(UMON_LOAD_ADDR, len); 

	WDT_PAUSE(1);
	umon_entry(argc, argv);

	return (0);
}

 
int main(int argc, char *argv[])  
{
	return (umon_main(argc, argv)); 
}

