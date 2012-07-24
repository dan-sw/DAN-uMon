#include "common.h"
#include <stdio.h>
#include <stdlib.h>

//#define SAVE_CRC32_LOOKUP_TABLE

#define PRINT_ERR			printf
#define PRINT_MSG			//printf
#define PRINT_OUT			printf
#define BYTES_IN_LINE		16
#define	MAX_BIN_SIZE_ARM	0x48000
#define	MAX_BIN_SIZE_DSP	0x48000
#define	MAX_BIN_SIZE_CPU	0x40000

#define	ARM_FLASH_START	0x10000
#define	DSP_FLASH_START	0x58000
#define	CPU_FLASH_START	0xA0000
#define	SEC_FLASH_START	0xA0000
#define	ALL_FLASH_END	0xE0000

#define	ARM_MEM_START	0x40400000
#define	DSP_MEM_START	0x60400000
#define	CPU_MEM_START	0x60400000
#define	SEC_MEM_START	0x60400000

#define SPI_BAUD_FAST	6

static inline void gen_crc32_lookup()
{
#ifdef SAVE_CRC32_LOOKUP_TABLE
	unsigned  i, polynom = CRC32_POLYNOMIAL;
	for (i = 0; i < 256; i++)
	{
		unsigned crc32 = ((unsigned)i << 24);
		unsigned j;
		for (j = 0; j < 8; j++)
		{
			unsigned crc32_prev = crc32;
			crc32 <<= 1;
			if (crc32_prev & 0x80000000)
			{
				crc32 ^= polynom;
			}
		}
		crc32_lookup[i] = __SWAPB(crc32);
	}
#endif
	return;
}

int main(int argc, char *argv[])
{
	char *arm_bin_name, *dsp_bin_name, *cpu_bin_name;
	FILE *f_arm, *f_dsp, *f_cpu;
	int arm_bytes_read, dsp_bytes_read, cpu_bytes_read;
	unsigned char *arm_buf, *dsp_buf, *cpu_buf, *ptr;
	unsigned header_chksum;
	BOOT_header_t boot_header =
	{
		{
			{ARM_FLASH_START, ARM_MEM_START, 0, 0},
			{DSP_FLASH_START, DSP_MEM_START, 0, 0},
			{CPU_FLASH_START, CPU_MEM_START, 0, 0},
			{SEC_FLASH_START, SEC_MEM_START, 0, 0},
		},
		SPI_BAUD_FAST,
	};

	PRINT_MSG("Flash Boot Loader Packer v1.0\n");
	
	if (argc != 4)
	{
		PRINT_ERR("\tError: expected format:\n\tpacker.exe <arm_bin_name> <dsp_bin_name> <cpu_bin_name>\n");
		exit(1);
	}
	
	arm_bin_name = argv[1];
	dsp_bin_name = argv[2];
	cpu_bin_name = argv[3];
	
	f_arm = fopen(arm_bin_name, "r");
	if (f_arm == NULL)
	{
		PRINT_ERR("\tError: failed to open file %s\n", arm_bin_name);
		exit(1);
	}
	f_dsp = fopen(dsp_bin_name, "r");
	if (f_dsp == NULL)
	{
		PRINT_ERR("\tError: failed to open file %s\n", dsp_bin_name);
		fclose(f_arm);
		exit(1);
	}
	f_cpu = fopen(cpu_bin_name, "r");
	if (f_cpu == NULL)
	{
		PRINT_ERR("\tError: failed to open file %s\n", cpu_bin_name);
		fclose(f_arm);
		fclose(f_dsp);
		exit(1);
	}

	gen_crc32_lookup();
#ifdef SAVE_CRC32_LOOKUP_TABLE
	{
		FILE *f = fopen("crc32_lookup.h", "w");
		unsigned i;
		if (f)
		{
			fprintf(f, "static const unsigned crc32_lookup[256] = \n{");
			for (i = 0; i < 255; i++)
			{
				if (i % 8 == 0)
					fprintf(f, "\n\t");
				fprintf(f, "0x%08x, ", crc32_lookup[i]);
			}
			fprintf(f, "0x%08x \n};\n", crc32_lookup[255]);
			fclose(f);
		}
	}
#endif
	arm_buf = calloc(MAX_BIN_SIZE_ARM, 1);
	if (arm_buf == NULL)
	{
		PRINT_ERR("\tError: Out of memory\n");
		fclose(f_arm);
		fclose(f_dsp);
		fclose(f_cpu);
		exit(1);
	}
	dsp_buf = calloc(MAX_BIN_SIZE_DSP, 1);
	if (dsp_buf == NULL)
	{
		PRINT_ERR("\tError: Out of memory\n");
		free(arm_buf);
		fclose(f_arm);
		fclose(f_dsp);
		fclose(f_cpu);
		exit(1);
	}
	
	cpu_buf = calloc(MAX_BIN_SIZE_CPU, 1);
	if (cpu_buf == NULL)
	{
		PRINT_ERR("\tError: Out of memory\n");
		free(arm_buf);
		free(dsp_buf);
		fclose(f_arm);
		fclose(f_dsp);
		fclose(f_cpu);
		exit(1);
	}

	arm_bytes_read = fread(arm_buf, 1, MAX_BIN_SIZE_ARM, f_arm);
	boot_header.items[BOOT_SELECT_ARM].size = arm_bytes_read;
	boot_header.items[BOOT_SELECT_ARM].chksum =
			calc_chksum(arm_buf, arm_bytes_read);

	dsp_bytes_read = fread(dsp_buf, 1, MAX_BIN_SIZE_DSP, f_dsp);
	boot_header.items[BOOT_SELECT_DSP].size = dsp_bytes_read;
	boot_header.items[BOOT_SELECT_DSP].chksum =
			calc_chksum(dsp_buf, dsp_bytes_read);

	cpu_bytes_read = fread(cpu_buf, 1, MAX_BIN_SIZE_CPU, f_cpu);
	boot_header.items[BOOT_SELECT_CPU].size = cpu_bytes_read;
	boot_header.items[BOOT_SELECT_CPU].chksum =
			calc_chksum(cpu_buf, cpu_bytes_read);

	boot_header.items[BOOT_SELECT_SEC].size = cpu_bytes_read;
	boot_header.items[BOOT_SELECT_SEC].chksum =
			calc_chksum(cpu_buf, cpu_bytes_read);

	boot_header.hdr_chksum = 0;
	boot_header.hdr_chksum = calc_chksum((void *)&boot_header,
			sizeof(boot_header) - sizeof(unsigned));
	header_chksum = calc_chksum((void *)&boot_header, sizeof(boot_header));

	if (header_chksum)
	{
		PRINT_ERR("\tError: checksum is not 0: 0x%08x\n", header_chksum);
		free(arm_buf);
		free(dsp_buf);
		free(cpu_buf);
		fclose(f_arm);
		fclose(f_dsp);
		fclose(f_cpu);
		exit(1);
	}

	ptr = (unsigned char *)&boot_header;
	PRINT_OUT("@%08x\n", 0);
	while ((ptr - (unsigned char *)&boot_header) < sizeof(boot_header))
	{
		unsigned i;
		for (i = 0; (i < BYTES_IN_LINE) &&
			(ptr - (unsigned char *)&boot_header) < sizeof(boot_header); i++)
		{
			PRINT_OUT("%02x ", *ptr++);
		}
		if (i == BYTES_IN_LINE) PRINT_OUT("\n");
	}
	{
		unsigned i, pad;
		for (pad = sizeof(boot_header); pad < ARM_FLASH_START; pad += i)
		{
			for (i = 0; (i < BYTES_IN_LINE) && (pad < ARM_FLASH_START); i++)
			{
				PRINT_OUT("%02x ", 0);
			}
			PRINT_OUT("\n");
		}
	}

	ptr = arm_buf;
	PRINT_OUT("@%08x\n", ARM_FLASH_START);
	while ((ptr - arm_buf) < arm_bytes_read)
	{
		unsigned i;
		for (i = 0; (i < BYTES_IN_LINE) &&
			(ptr - arm_buf) < arm_bytes_read; i++)
		{
			PRINT_OUT("%02x ", *ptr++);
		}
		if (i == BYTES_IN_LINE) PRINT_OUT("\n");
	}
	{
		unsigned i, pad;
		for (pad = arm_bytes_read; 
				pad < DSP_FLASH_START - ARM_FLASH_START; pad += i)
		{
			for (i = 0; (pad % BYTES_IN_LINE + i < BYTES_IN_LINE) && 
					((pad + i) < (DSP_FLASH_START - ARM_FLASH_START)); i++)
			{
				PRINT_OUT("%02x ", 0);
			}
			PRINT_OUT("\n");
		}
	}

	ptr = dsp_buf;
	PRINT_OUT("@%08x\n", DSP_FLASH_START);
	while ((ptr - dsp_buf) < dsp_bytes_read)
	{
		unsigned i;
		for (i = 0; (i < BYTES_IN_LINE) &&
			(ptr - dsp_buf) < dsp_bytes_read; i++)
		{
			PRINT_OUT("%02x ", *ptr++);
		}
		if (i == BYTES_IN_LINE) PRINT_OUT("\n");
	}
	{
		unsigned i, pad;
		for (pad = dsp_bytes_read; 
				pad < (CPU_FLASH_START - DSP_FLASH_START); pad += i)
		{
			for (i = 0; (pad % BYTES_IN_LINE + i < BYTES_IN_LINE) && 
					((pad + i) < (CPU_FLASH_START - DSP_FLASH_START)); i++)
			{
				PRINT_OUT("%02x ", 0);
			}
			PRINT_OUT("\n");
		}
	}

	ptr = cpu_buf;
	PRINT_OUT("@%08x\n", CPU_FLASH_START);
	while ((ptr - cpu_buf) < cpu_bytes_read)
	{
		unsigned i;
		for (i = 0; (i < BYTES_IN_LINE) &&
			(ptr - cpu_buf) < cpu_bytes_read; i++)
		{
			PRINT_OUT("%02x ", *ptr++);
		}
		PRINT_OUT("\n");
	}
	{
		unsigned i, pad;
		for (pad = cpu_bytes_read; 
				pad < (ALL_FLASH_END - CPU_FLASH_START); pad += i)
		{
			for (i = 0; (pad % BYTES_IN_LINE + i < BYTES_IN_LINE) && 
					((pad + i) < (ALL_FLASH_END - CPU_FLASH_START)); i++)
			{
				PRINT_OUT("%02x ", 0);
			}
			PRINT_OUT("\n");
		}
	}

	free(arm_buf);
	free(dsp_buf);
	free(cpu_buf);
	fclose(f_arm);
	fclose(f_dsp);
	fclose(f_cpu);
}
