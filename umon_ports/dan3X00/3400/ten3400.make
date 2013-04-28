OSTYPE := $(shell uname)

ifeq ($(OSTYPE), Linux)
TEN_ROOT_PATH	= ~
TEN_SYS_SUFFIX	= linux
else
TEN_ROOT_PATH	= c:/usr
TEN_SYS_SUFFIX	= win32
endif

###############################################################################
#
# Our Tensilica tools definition.
#
TOOLS_VERSION 		= RD-2011.1
TEN_INST_PATH 		= $(TEN_ROOT_PATH)/xtensa/XtDevTools/install
TEN_TOOL_PATH 		= $(TEN_INST_PATH)/tools/$(TOOLS_VERSION)-$(TEN_SYS_SUFFIX)/XtensaTools/bin
TEN_BUILD_PATH 		= $(TEN_INST_PATH)/builds/$(TOOLS_VERSION)-$(TEN_SYS_SUFFIX)
TEN_INC_ROOT		= $(TEN_INST_PATH)/tools/$(TOOLS_VERSION)-$(TEN_SYS_SUFFIX)/XtensaTools
TEN_INC_PATH		= $(TEN_INC_ROOT)/xtensa-elf/include/xtensa
TEN_CONFIG 		= $(TEN_BUILD_PATH)/$(TEN_CORE)/config
TEN_CFLAGS 		= --xtensa-system=$(TEN_CONFIG) --xtensa-core=$(TEN_CORE) 
TEN_LSP_DIR		= ./lsp
TEN_LSP_DIR_PIGGY 	= ./lsp_piggy
TEN_LSP			= $(TEN_LSP_DIR)/ldscripts/elf32xtensa.x
TEN_LSP_PIGGY		= $(TEN_LSP_DIR_PIGGY)/ldscripts/elf32xtensa.x

###############################################################################
#
# Redefine TEN_LSP_DIR in order to support several versions
#
ifeq ($(TARGET),3)     # UMON_TARGET_XT_SIM
	TEN_LSP_DIR	:= $(TEN_LSP_DIR)_sim
endif

ifeq ($(DEBUG),1)
	TEN_LSP_DIR	:= $(TEN_LSP_DIR)_deb
endif


###############################################################################
#
# Redefine some non-standard Tensilica tool names and other things from common.make.
#
NM		= $(TEN_TOOL_PATH)/xt-nm
AR		= $(TEN_TOOL_PATH)/xt-ar
LD 		= $(TEN_TOOL_PATH)/xt-xcc
ASM		= $(TEN_TOOL_PATH)/xt-as
CC 		= $(TEN_TOOL_PATH)/xt-xcc
STRIP	= $(TEN_TOOL_PATH)/xt-strip
OBJCOPY	= $(TEN_TOOL_PATH)/xt-objcopy
OBJDUMP	= $(TEN_TOOL_PATH)/xt-objdump
LDGEN	= $(TEN_TOOL_PATH)/xt-genldscripts

###############################################################################
#
# Define our c-flags. 
# The original CFLAGS passed to the $(CC) calls (see the rules.make file) is the 
# concatenation of COMMON_CFLAGS & CUSTOM_CFLAGS & COMMON_INCLUDE & CUSTOM_INCLUDE.  
# We are not using this concatanation, but taking COMMON_INCLUDE and adding it.  
#
CFLAGS += $(TEN_CFLAGS)
CFLAGS += -I$(COMMON3400DIR) $(COMMON_INCLUDE) -I$(TEN_INC_PATH)
CFLAGS += -DUMON_TARGET=$(TARGET) -DUMON_DEBUG=$(DEBUG)
CFLAGS += -mno-serialize-volatile -Wall -fno-builtin --longcalls
#CFLAGS += -Werror 
ifneq ($(DEBUG),1)
CFLAGS += -O2 -Os -ipa -mno-serialize-volatile
else
CFLAGS += -g3 -O1 -Os -mno-serialize-volatile
endif

LINK_SCRIPT = $(TEN_LSP)

###############################################################################
#
# Define our linker flags. 
#
LFLAGS			= $(CFLAGS)
LFLAGS			+= -mlsp=$(TEN_LSP_DIR)
LFLAGS			+= -Wl,-Map=$(MAP)
LFLAGS			+= -Wl,--defsym,_memmap_cacheattr_reset=0
LFLAGS			+= -Wl,--defsym,_memmap_cacheattr_reset=0x22224444
LFLAGS_PIGGY	= $(CFLAGS)
LFLAGS_PIGGY	+= -mlsp=$(TEN_LSP_DIR_PIGGY)
LFLAGS_PIGGY	+= -Wl,-Map=$(PIGGY_MAP)
LFLAGS_PIGGY	+= -Wl,--defsym,_memmap_cacheattr_reset=0
LFLAGS_PIGGY	+= -Wl,--defsym,_memmap_cacheattr_reset=0x22224444

SECTIONS_DATA_PIGGY	= $(SECTIONS_DATA) -j .piggy.rodata

###############################################################################
#
# LSP specific for xtensa processors. 
#
.PHONY:	lsp
lsp: $(TEN_LSP)

$(TEN_LSP): $(TEN_LSP_DIR)/memmap.xmm $(TEN_LSP_DIR)/specs
	$(LDGEN) --xtensa-core=$(TEN_CORE) -b $(TEN_LSP_DIR)

.PHONY:	lsp_piggy
lsp_piggy: $(TEN_LSP_PIGGY)

$(TEN_LSP_PIGGY): $(TEN_LSP_DIR_PIGGY)/memmap.xmm $(TEN_LSP_DIR_PIGGY)/specs
	@$(LDGEN) --xtensa-core=$(TEN_CORE) -b $(TEN_LSP_DIR_PIGGY)

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
	@$(OBJDUMP) $(TEN_CFLAGS) -s        $(SECTIONS_CODE) $(SECTIONS_DATA_PIGGY) $(PIGGY_ELF) > $(PIGGY_ELF).dump
	@$(OBJDUMP) $(TEN_CFLAGS) -d        $(SECTIONS_CODE) $(SECTIONS_DATA_PIGGY) $(PIGGY_ELF) > $(PIGGY_ELF).dis
	@$(OBJCOPY) $(TEN_CFLAGS) -O binary $(SECTIONS_CODE) $(SECTIONS_DATA_PIGGY) $(PIGGY_ELF) $(PIGGY_BIN)
	@echo Packing finished

