###############################################################################
#
# DAN3400 board makefile.
# Based on Generic uMon Port example (umon_ports\generic\makefile)
#
# This is a port that has nothing built into it that is specific to a target.  
# It is essentially a uMon application, but with all the basic functionality of uMon built in.
# It assumes to be launched by some other bootloader, and it built by a toolset that has 
# a library built for the target (i.e. putchar will put a character out on the console).
#
###############################################################################
# Usage:
# make TARGET=<0|1|2|3|4> DEBUG=<0|1> [all]
# Note:
# For details about UMON_TARGET, UMON_DEBUG see config.h
###############################################################################
#
# These are the only variables that should need to be touched for this generic build of uMon:
# UMONDIR, UMONAPPSDIR, PLATFORM, BIN, ELF, MAP, DEMOELF (3400.make)
# CPUTYPE (here)
#
CPUTYPE			= arm
# DAN2400			= 1

#########################################################################
#
# Set default TARGET (UMON_TARGET_ARM = 4) , DEBUG (0)
#

SHELL := /bin/dash
ifeq ($(TARGET),)
TARGET	= 4
endif

ifeq ($(DEBUG),)
DEBUG	= 0
endif

CFLAGS += -DARM

OSTYPE := $(shell uname)

###############################################################################
#
# ARM tools definition.
#
ifneq ($(DAN2400),)
ARM_INST_PATH 	= "/cygdrive/c/program files/gnuarm"
ARM_TOOL_SUFFIX	= arm-elf
ARM_TOOL_PATH 	= $(ARM_INST_PATH)/bin
ARM_INC_PATH	= $(ARM_INST_PATH)/include
else
ifeq ($(OSTYPE), Linux)
ARM_INST_PATH 	= "/home/lab/codesourcery/arm-2010.09-51"
ARM_TOOL_SUFFIX	= arm-none-eabi
else
ARM_INST_PATH 	= "/cygdrive/c/program files/CodeSourcery/Sourcery G++ Lite"
ARM_TOOL_SUFFIX	= arm-none-eabi
endif
ARM_TOOL_PATH 	= $(ARM_INST_PATH)/bin
ARM_INC_PATH	= $(ARM_INST_PATH)/$(ARM_TOOL_SUFFIX)/include
endif

TOOL_PREFIX = $(ARM_TOOL_PATH)/$(ARM_TOOL_SUFFIX)

#########################################################################
#
# Include DAN3400 uMon common part make
#
include ../3400/3400.make

###############################################################################
#
# Redefine some non-standard ARM tool names and other things from common.make.
#
NM	= $(ARM_TOOL_PATH)/$(ARM_TOOL_SUFFIX)-nm
AR	= $(ARM_TOOL_PATH)/$(ARM_TOOL_SUFFIX)-ar
LD 	= $(ARM_TOOL_PATH)/$(ARM_TOOL_SUFFIX)-ld
ASM	= $(ARM_TOOL_PATH)/$(ARM_TOOL_SUFFIX)-as
CC 	= $(ARM_TOOL_PATH)/$(ARM_TOOL_SUFFIX)-gcc
STRIP	= $(ARM_TOOL_PATH)/$(ARM_TOOL_SUFFIX)-strip
OBJCOPY	= $(ARM_TOOL_PATH)/$(ARM_TOOL_SUFFIX)-objcopy
OBJDUMP	= $(ARM_TOOL_PATH)/$(ARM_TOOL_SUFFIX)-objdump

# SECTIONS_CODE = -j .text 
# SECTIONS_DATA = -j .rodata -j .rodata.str1.1 -j .ARM.extab -j .data -j .ARM.exidx
# SECTIONS_DATA_PIGGY	= $(SECTIONS_DATA) -j .piggy.rodata

###############################################################################
#
# Define our c-flags. 
# The original CFLAGS passed to the $(CC) calls (see the rules.make file) is the 
# concatenation of COMMON_CFLAGS & CUSTOM_CFLAGS & COMMON_INCLUDE & CUSTOM_INCLUDE.  
# We are not using this concatanation, but taking COMMON_INCLUDE and adding it.  
#
CFLAGS += $(COMMON_INCLUDE) -I$(ARM_INC_PATH) -I$(COMMON3400DIR)
CFLAGS += -DUMON_TARGET=$(TARGET) -DUMON_DEBUG=$(DEBUG)
CFLAGS += -Wall -fno-builtin 
CFLAGS += -mcpu=arm926ej-s -std=gnu99 
CFLAGS += -fshort-enums -fverbose-asm -fasynchronous-unwind-tables
CFLAGS += -feliminate-unused-debug-types  -funsigned-bitfields 
CFLAGS += -msoft-float -mapcs-frame

ifneq ($(DAN2400),)
# For DAN2400 - big endian
CFLAGS += -mfpu=fpa -mbig-endian -DBIG_ENDIAN -DDAN2400
endif

#CFLAGS += -Werror 
ifneq ($(DEBUG),1)
CFLAGS += -Os
else
CFLAGS += -g3 -gdwarf-2 -Os
endif

###############################################################################
#
# Define our linker flags. 
#
LFLAGS +=  -nostdlib -lgcc -Wl,-Map=$(MAP) 
LFLAGS_PIGGY +=  -lgcc -Wl,-Map=$(PIGGY_MAP) 

ifneq ($(DAN2400),)
LFLAGS += -Wl,-Tarm2400.ld
# For DAN2400 - big endian
LFLAGS += -mbig-endian -DBIG_ENDIAN
else
LINK_SCRIPT = arm3400.ld
LFLAGS += -Wl,-T$(LINK_SCRIPT)
LFLAGS_PIGGY += -Wl,-Tarm3400_piggy.ld
endif

###############################################################################
#
# Build each variable from a list of individual filenames...
# (COMCSRC is defined in 3400.make)
LOCSSRC		= startup_entry.S
CPUSSRC		= 
CPUCSRC		= dis_arm.c dis_arm_instr.c misc_arm.c regs_arm.c reg_cache.c

ifneq ($(DAN3400_ARM_CACHE),no)
CPUCSRC		+= cache_arm.c
CFLAGS		+= -DDAN3400_ARM_CACHE
endif

ifneq ($(findstring arm_cb_only, $(MAKECMDGOALS)),)
CFLAGS		+= -DUSE_NPU_CB_ONLY
endif

# Build final .elf, .dump, .map & .bin files
# monbuilt.c is always compiled here before building, in order to have the correct build date
$(BIN): $(OBJS) $(LINK_SCRIPT)
	@echo "Linking: 	" $@
	@$(CC) -c $(CFLAGS) -DBUILD_ID="\"$(BUILD_ID)\"" -o $(BUILDDIR)/version.o $(COMMON3400DIR)/version.c
	@$(CC) -c $(CFLAGS) -o $(BUILDDIR)/monbuilt.o $(COMDIR)/monbuilt.c
	@$(CC) $(OBJS) $(BUILDDIR)/monbuilt.o $(BUILDDIR)/version.o $(LFLAGS) -o $(ELF)
	@$(OBJDUMP) $(TEN_CFLAGS) -s        $(SECTIONS_CODE) $(SECTIONS_DATA) $(ELF) > $(ELF).dump
	@$(OBJDUMP) $(TEN_CFLAGS) -d        $(SECTIONS_CODE) $(SECTIONS_DATA) $(ELF) > $(ELF).dis
	@$(OBJCOPY) $(TEN_CFLAGS) -O binary $(SECTIONS_CODE) $(SECTIONS_DATA) $(ELF) $(BIN)
	@echo Build finished

$(GZBIN):	$(BIN)
	@gzip -c $(BIN) > $(GZBIN)

$(PIGGY_BIN): $(GZBIN) $(PIGGY_OBJS) $(TEN_LSP_PIGGY)
	@echo "Packing: 	" $@
	@mkdir -p $(BUILDDIR)
	@$(CC) -c $(CFLAGS) -DUMON_BIN_NAME="\"$(GZBIN)\"" -o $(BUILDDIR)/piggy.o $(COMMON3400DIR)/piggy.S
	@$(CC) $(PIGGY_OBJS) $(BUILDDIR)/piggy.o $(LFLAGS_PIGGY) -o $(PIGGY_ELF)
	@$(OBJDUMP)  -s        $(SECTIONS_CODE) $(SECTIONS_DATA_PIGGY) $(PIGGY_ELF) > $(PIGGY_ELF).dump
	@$(OBJDUMP)  -d        $(SECTIONS_CODE) $(SECTIONS_DATA_PIGGY) $(PIGGY_ELF) > $(PIGGY_ELF).dis
	@$(OBJCOPY)  -O binary $(SECTIONS_CODE) $(SECTIONS_DATA_PIGGY) $(PIGGY_ELF) $(PIGGY_BIN)
	@echo Packing finished

arm_cb_only:	$(PIGGY_BIN)

