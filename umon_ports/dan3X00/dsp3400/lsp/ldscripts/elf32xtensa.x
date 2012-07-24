/* This linker script generated from xt-genldscripts.tpp for LSP ./lsp */
/* Linker Script for default link */
MEMORY
{
  sram_0_seg :                        	org = 0x00000000, len = 0x200
  sram_1_seg :                        	org = 0x00000200, len = 0x10E00
  sram_2_seg :                        	org = 0x00011000, len = 0x178
  sram_4_seg :                        	org = 0x0001117C, len = 0x1C
  sram_6_seg :                        	org = 0x0001119C, len = 0x1C
  sram_8_seg :                        	org = 0x000111BC, len = 0x1C
  sram_10_seg :                       	org = 0x000111DC, len = 0x1C
  sram_12_seg :                       	org = 0x000111FC, len = 0x1C
  sram_14_seg :                       	org = 0x0001121C, len = 0x1C
  sram_16_seg :                       	org = 0x0001123C, len = 0x1C
  sram_18_seg :                       	org = 0x0001125C, len = 0x1C
  sram_19_seg :                       	org = 0x00011278, len = 0x4ED88
  dram0_0_seg :                       	org = 0x5FFD0000, len = 0x10000
  iram0_0_seg :                       	org = 0x5FFE0000, len = 0x10
  iram0_1_seg :                       	org = 0x5FFE0010, len = 0xFFF0
}

PHDRS
{
  sram_0_phdr PT_LOAD;
  sram_1_phdr PT_LOAD;
  sram_2_phdr PT_LOAD;
  sram_3_phdr PT_LOAD;
  sram_4_phdr PT_LOAD;
  sram_5_phdr PT_LOAD;
  sram_6_phdr PT_LOAD;
  sram_7_phdr PT_LOAD;
  sram_8_phdr PT_LOAD;
  sram_9_phdr PT_LOAD;
  sram_10_phdr PT_LOAD;
  sram_11_phdr PT_LOAD;
  sram_12_phdr PT_LOAD;
  sram_13_phdr PT_LOAD;
  sram_14_phdr PT_LOAD;
  sram_15_phdr PT_LOAD;
  sram_16_phdr PT_LOAD;
  sram_17_phdr PT_LOAD;
  sram_18_phdr PT_LOAD;
  sram_19_phdr PT_LOAD;
  sram_19_bss_phdr PT_LOAD;
  dram0_0_phdr PT_LOAD;
  dram0_0_bss_phdr PT_LOAD;
  iram0_0_phdr PT_LOAD;
  iram0_1_phdr PT_LOAD;
}


/*  Default entry point:  */
ENTRY(_ResetVector)
_rom_store_table = 0;
PROVIDE(_memmap_vecbase_reset = 0x11000);
/* Various memory-map dependent cache attribute settings: */
_memmap_cacheattr_wb_base = 0x00000404;
_memmap_cacheattr_wt_base = 0x00000101;
_memmap_cacheattr_bp_base = 0x00000202;
_memmap_cacheattr_unused_mask = 0xFFFFF0F0;
_memmap_cacheattr_wb_trapnull = 0x22222424;
_memmap_cacheattr_wba_trapnull = 0x22222424;
_memmap_cacheattr_wbna_trapnull = 0x22222525;
_memmap_cacheattr_wt_trapnull = 0x22222121;
_memmap_cacheattr_bp_trapnull = 0x22222222;
_memmap_cacheattr_wb_strict = 0xFFFFF4F4;
_memmap_cacheattr_wt_strict = 0xFFFFF1F1;
_memmap_cacheattr_bp_strict = 0xFFFFF2F2;
_memmap_cacheattr_wb_allvalid = 0x22222424;
_memmap_cacheattr_wt_allvalid = 0x22222121;
_memmap_cacheattr_bp_allvalid = 0x22222222;
PROVIDE(_memmap_cacheattr_reset = _memmap_cacheattr_wb_trapnull);

SECTIONS
{

  .ResetVector.text : ALIGN(4)
  {
    _ResetVector_text_start = ABSOLUTE(.);
    KEEP (*(.ResetVector.text))
    _ResetVector_text_end = ABSOLUTE(.);
  } >sram_0_seg :sram_0_phdr

  .lit4 : ALIGN(4)
  {
    _lit4_start = ABSOLUTE(.);
    *(*.lit4)
    *(.lit4.*)
    *(.gnu.linkonce.lit4.*)
    _lit4_end = ABSOLUTE(.);
  } >sram_1_seg :sram_1_phdr

  .literal : ALIGN(4)
  {
    _literal_start = ABSOLUTE(.);
    *(.init.literal)
    *(.literal)
    *(.literal.*)
    *(.gnu.linkonce.literal.*)
    *(.gnu.linkonce.t.*.literal)
    *(.fini.literal)
    _literal_end = ABSOLUTE(.);
  } >sram_1_seg :sram_1_phdr

  .Level2InterruptVector.literal : ALIGN(4)
  {
    _Level2InterruptVector_literal_start = ABSOLUTE(.);
    *(.Level2InterruptVector.literal)
    _Level2InterruptVector_literal_end = ABSOLUTE(.);
  } >sram_1_seg :sram_1_phdr

  .Level3InterruptVector.literal : ALIGN(4)
  {
    _Level3InterruptVector_literal_start = ABSOLUTE(.);
    *(.Level3InterruptVector.literal)
    _Level3InterruptVector_literal_end = ABSOLUTE(.);
  } >sram_1_seg :sram_1_phdr

  .Level4InterruptVector.literal : ALIGN(4)
  {
    _Level4InterruptVector_literal_start = ABSOLUTE(.);
    *(.Level4InterruptVector.literal)
    _Level4InterruptVector_literal_end = ABSOLUTE(.);
  } >sram_1_seg :sram_1_phdr

  .Level5InterruptVector.literal : ALIGN(4)
  {
    _Level5InterruptVector_literal_start = ABSOLUTE(.);
    *(.Level5InterruptVector.literal)
    _Level5InterruptVector_literal_end = ABSOLUTE(.);
  } >sram_1_seg :sram_1_phdr

  .DebugExceptionVector.literal : ALIGN(4)
  {
    _DebugExceptionVector_literal_start = ABSOLUTE(.);
    *(.DebugExceptionVector.literal)
    _DebugExceptionVector_literal_end = ABSOLUTE(.);
  } >sram_1_seg :sram_1_phdr

  .KernelExceptionVector.literal : ALIGN(4)
  {
    _KernelExceptionVector_literal_start = ABSOLUTE(.);
    *(.KernelExceptionVector.literal)
    _KernelExceptionVector_literal_end = ABSOLUTE(.);
  } >sram_1_seg :sram_1_phdr

  .UserExceptionVector.literal : ALIGN(4)
  {
    _UserExceptionVector_literal_start = ABSOLUTE(.);
    *(.UserExceptionVector.literal)
    _UserExceptionVector_literal_end = ABSOLUTE(.);
  } >sram_1_seg :sram_1_phdr

  .DoubleExceptionVector.literal : ALIGN(4)
  {
    _DoubleExceptionVector_literal_start = ABSOLUTE(.);
    *(.DoubleExceptionVector.literal)
    _DoubleExceptionVector_literal_end = ABSOLUTE(.);
  } >sram_1_seg :sram_1_phdr

  .WindowVectors.text : ALIGN(4)
  {
    _WindowVectors_text_start = ABSOLUTE(.);
    KEEP (*(.WindowVectors.text))
    _WindowVectors_text_end = ABSOLUTE(.);
  } >sram_2_seg :sram_2_phdr

  .Level2InterruptVector.text : ALIGN(4)
  {
    _Level2InterruptVector_text_start = ABSOLUTE(.);
    KEEP (*(.Level2InterruptVector.text))
    _Level2InterruptVector_text_end = ABSOLUTE(.);
  } >sram_4_seg :sram_4_phdr

  .Level3InterruptVector.text : ALIGN(4)
  {
    _Level3InterruptVector_text_start = ABSOLUTE(.);
    KEEP (*(.Level3InterruptVector.text))
    _Level3InterruptVector_text_end = ABSOLUTE(.);
  } >sram_6_seg :sram_6_phdr

  .Level4InterruptVector.text : ALIGN(4)
  {
    _Level4InterruptVector_text_start = ABSOLUTE(.);
    KEEP (*(.Level4InterruptVector.text))
    _Level4InterruptVector_text_end = ABSOLUTE(.);
  } >sram_8_seg :sram_8_phdr

  .Level5InterruptVector.text : ALIGN(4)
  {
    _Level5InterruptVector_text_start = ABSOLUTE(.);
    KEEP (*(.Level5InterruptVector.text))
    _Level5InterruptVector_text_end = ABSOLUTE(.);
  } >sram_10_seg :sram_10_phdr

  .DebugExceptionVector.text : ALIGN(4)
  {
    _DebugExceptionVector_text_start = ABSOLUTE(.);
    KEEP (*(.DebugExceptionVector.text))
    _DebugExceptionVector_text_end = ABSOLUTE(.);
  } >sram_12_seg :sram_12_phdr

  .KernelExceptionVector.text : ALIGN(4)
  {
    _KernelExceptionVector_text_start = ABSOLUTE(.);
    KEEP (*(.KernelExceptionVector.text))
    _KernelExceptionVector_text_end = ABSOLUTE(.);
  } >sram_14_seg :sram_14_phdr

  .UserExceptionVector.text : ALIGN(4)
  {
    _UserExceptionVector_text_start = ABSOLUTE(.);
    KEEP (*(.UserExceptionVector.text))
    _UserExceptionVector_text_end = ABSOLUTE(.);
  } >sram_16_seg :sram_16_phdr

  .DoubleExceptionVector.text : ALIGN(4)
  {
    _DoubleExceptionVector_text_start = ABSOLUTE(.);
    KEEP (*(.DoubleExceptionVector.text))
    _DoubleExceptionVector_text_end = ABSOLUTE(.);
  } >sram_18_seg :sram_18_phdr

  .text : ALIGN(4)
  {
    _stext = .;
    _text_start = ABSOLUTE(.);
    *(.entry.text)
    KEEP (*(.init))
    *(.text)
    *(.text.*)
    *(.gnu.linkonce.t.*)
    KEEP (*(.text.*personality*))
    *(.stub)
    *(.gnu.warning)
    KEEP (*(.fini))
    *(.gnu.version)
    _text_end = ABSOLUTE(.);
    _etext = .;
  } >sram_19_seg :sram_19_phdr

  .rodata : ALIGN(4)
  {
    _rodata_start = ABSOLUTE(.);
    *(.rodata)
    *(.rodata.*)
    *(.gnu.linkonce.r.*)
    *(.rodata1)
    __XT_EXCEPTION_TABLE__ = ABSOLUTE(.);
    KEEP (*(.xt_except_table))
    KEEP (*(.gcc_except_table))
    *(.gnu.linkonce.e.*)
    *(.gnu.version_r)
    KEEP (*(.eh_frame))
    /*  C++ constructor and destructor tables, properly ordered:  */
    KEEP (*crtbegin.o(.ctors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    KEEP (*crtbegin.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    /*  C++ exception handlers table:  */
    __XT_EXCEPTION_DESCS__ = ABSOLUTE(.);
    *(.xt_except_desc)
    *(.gnu.linkonce.h.*)
    __XT_EXCEPTION_DESCS_END__ = ABSOLUTE(.);
    *(.xt_except_desc_end)
    *(.dynamic)
    *(.gnu.version_d)
    . = ALIGN(4);		/* this table MUST be 4-byte aligned */
    _bss_table_start = ABSOLUTE(.);
    LONG(_bss_start)
    LONG(_bss_end)
    LONG(_dram0_bss_start)
    LONG(_dram0_bss_end)
    _bss_table_end = ABSOLUTE(.);
    _rodata_end = ABSOLUTE(.);
  } >sram_19_seg :sram_19_phdr

  .data : ALIGN(4)
  {
    _data_start = ABSOLUTE(.);
    *(.data)
    *(.data.*)
    *(.gnu.linkonce.d.*)
    KEEP(*(.gnu.linkonce.d.*personality*))
    *(.data1)
    *(.sdata)
    *(.sdata.*)
    *(.gnu.linkonce.s.*)
    *(.sdata2)
    *(.sdata2.*)
    *(.gnu.linkonce.s2.*)
    KEEP(*(.jcr))
    _data_end = ABSOLUTE(.);
  } >sram_19_seg :sram_19_phdr

  .bss (NOLOAD) : ALIGN(8)
  {
    . = ALIGN (8);
    _bss_start = ABSOLUTE(.);
    *(.dynsbss)
    *(.sbss)
    *(.sbss.*)
    *(.gnu.linkonce.sb.*)
    *(.scommon)
    *(.sbss2)
    *(.sbss2.*)
    *(.gnu.linkonce.sb2.*)
    *(.dynbss)
    *(.bss)
    *(.bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN (8);
    _bss_end = ABSOLUTE(.);
    _end = ALIGN(0x8);
    PROVIDE(end = ALIGN(0x8));
    _stack_sentry = ALIGN(0x8);
  } >sram_19_seg :sram_19_bss_phdr
 __stack = 0x60000;
  _heap_sentry = 0x60000;

  .SharedResetVector.literal : ALIGN(4)
  {
    _SharedResetVector_literal_start = ABSOLUTE(.);
    *(.SharedResetVector.literal)
    _SharedResetVector_literal_end = ABSOLUTE(.);
  } >dram0_0_seg :dram0_0_phdr

  .iram0.literal : ALIGN(4)
  {
    _iram0_literal_start = ABSOLUTE(.);
    *(.iram0.literal)
    *(.iram.literal)
    *(.iram.text.literal)
    _iram0_literal_end = ABSOLUTE(.);
  } >dram0_0_seg :dram0_0_phdr

  .dram0.rodata : ALIGN(4)
  {
    _dram0_rodata_start = ABSOLUTE(.);
    *(.dram0.rodata)
    *(.dram.rodata)
    _dram0_rodata_end = ABSOLUTE(.);
  } >dram0_0_seg :dram0_0_phdr

  .dram0.literal : ALIGN(4)
  {
    _dram0_literal_start = ABSOLUTE(.);
    *(.dram0.literal)
    *(.dram.literal)
    _dram0_literal_end = ABSOLUTE(.);
  } >dram0_0_seg :dram0_0_phdr

  .dram0.data : ALIGN(4)
  {
    _dram0_data_start = ABSOLUTE(.);
    *(.dram0.data)
    *(.dram.data)
    _dram0_data_end = ABSOLUTE(.);
  } >dram0_0_seg :dram0_0_phdr

  .dram0.bss (NOLOAD) : ALIGN(8)
  {
    . = ALIGN (8);
    _dram0_bss_start = ABSOLUTE(.);
    *(.dram0.bss)
    . = ALIGN (8);
    _dram0_bss_end = ABSOLUTE(.);
  } >dram0_0_seg :dram0_0_bss_phdr

  .SharedResetVector.text : ALIGN(4)
  {
    _SharedResetVector_text_start = ABSOLUTE(.);
    KEEP (*(.SharedResetVector.text))
    _SharedResetVector_text_end = ABSOLUTE(.);
  } >iram0_0_seg :iram0_0_phdr

  .iram0.text : ALIGN(4)
  {
    _iram0_text_start = ABSOLUTE(.);
    *(.iram0.text)
    *(.iram.text)
    _iram0_text_end = ABSOLUTE(.);
  } >iram0_1_seg :iram0_1_phdr
  .debug  0 :  { *(.debug) }
  .line  0 :  { *(.line) }
  .debug_srcinfo  0 :  { *(.debug_srcinfo) }
  .debug_sfnames  0 :  { *(.debug_sfnames) }
  .debug_aranges  0 :  { *(.debug_aranges) }
  .debug_pubnames  0 :  { *(.debug_pubnames) }
  .debug_info  0 :  { *(.debug_info) }
  .debug_abbrev  0 :  { *(.debug_abbrev) }
  .debug_line  0 :  { *(.debug_line) }
  .debug_frame  0 :  { *(.debug_frame) }
  .debug_str  0 :  { *(.debug_str) }
  .debug_loc  0 :  { *(.debug_loc) }
  .debug_macinfo  0 :  { *(.debug_macinfo) }
  .debug_weaknames  0 :  { *(.debug_weaknames) }
  .debug_funcnames  0 :  { *(.debug_funcnames) }
  .debug_typenames  0 :  { *(.debug_typenames) }
  .debug_varnames  0 :  { *(.debug_varnames) }
  .xt.insn 0 :
  {
    KEEP (*(.xt.insn))
    KEEP (*(.gnu.linkonce.x.*))
  }
  .xt.prop 0 :
  {
    KEEP (*(.xt.prop))
    KEEP (*(.xt.prop.*))
    KEEP (*(.gnu.linkonce.prop.*))
  }
  .xt.lit 0 :
  {
    KEEP (*(.xt.lit))
    KEEP (*(.xt.lit.*))
    KEEP (*(.gnu.linkonce.p.*))
  }
  .xt.profile_range 0 :
  {
    KEEP (*(.xt.profile_range))
    KEEP (*(.gnu.linkonce.profile_range.*))
  }
  .xt.profile_ranges 0 :
  {
    KEEP (*(.xt.profile_ranges))
    KEEP (*(.gnu.linkonce.xt.profile_ranges.*))
  }
  .xt.profile_files 0 :
  {
    KEEP (*(.xt.profile_files))
    KEEP (*(.gnu.linkonce.xt.profile_files.*))
  }
}

