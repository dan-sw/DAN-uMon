	.section .piggy.rodata
	.globl	input_data
input_data:
	.incbin	UMON_BIN_NAME
	.globl	input_data_end
input_data_end:
