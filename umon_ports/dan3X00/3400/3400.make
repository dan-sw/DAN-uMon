###############################################################################
#
# DAN3400 board makefile (common part for all CPUs).
# It is included from ten3400\makefile, dsp3400\makefile or arm3400\makefile
#
###############################################################################

PLATFORM		= DAN3400
UMONDIR			= ../../../umon_main
UMONAPPSDIR		= ../../../umon_apps
BIN				= $(BUILDDIR)/umon.bin
PIGGY_BIN		= $(BUILDDIR)/umon_piggy.bin
ELF				= $(BUILDDIR)/umon.elf
PIGGY_ELF		= $(BUILDDIR)/umon_piggy.elf
MAP				= $(BUILDDIR)/umon.map
PIGGY_MAP		= $(BUILDDIR)/umon_piggy.map
GZBIN			= $(BUILDDIR)/umon.gz
COMMON3400DIR   = ../3400


###############################################################################
#
# Include uMon common.make; firstly define TOPDIR (it's uMon's required var)
#
TOPDIR = $(UMONDIR)
include	$(UMONDIR)/target/make/common.make


###############################################################################
#
# Redefine BUILDDIR (defined in common.make) in order to support several versions
#
BUILDDIR = Bin_$(CPUTYPE)

###############################################################################
#
# Build each variable from a list of individual filenames...
# COMCSRC - common sources
# Use tfsclean1.c for powersafe defrag or tfsclean2 for non-powersafe defrag
COMCSRC		= arp.c cast.c cache.c chario.c cmdtbl.c \
			  docmd.c dhcp_00.c dhcpboot.c dns.c ee.c env.c \
			  ethernet.c flash.c genlib.c icmp.c if.c \
			  jffs2.c ledit_vt100.c monprof.c \
			  mprintf.c memcmds.c malloc.c moncom.c memtrace.c misccmds.c \
			  misc.c redirect.c sbrk.c start.c \
			  struct.c symtbl.c syslog.c tcpstuff.c timestuff.c tfs.c \
			  tfsapi.c tfsclean2.c tfscli.c tfsloader.c tfslog.c tftp.c \
			  xmodem.c spif.c
IODEVSRC	= spi.c flash3400.c gmac_mon.c etherdev.c CGEN.c tmr.c ddr.c
# The modules below added for compilation not for SIMULATION or JTAG (real target)
IODEVSRC 	+= uart16550.c

LOCCSRC		= except.c cpu.c cpuio.c startup.c common.c dancli.c

PIGGY_C_SRC	= umon_loader.c CGEN.c tmr.c cpu.c crc32.c uart16550.c ddr.c  
PIGGY_A_SRC	= startup_entry.S

PIGGY_OBJS	 = $(addprefix $(BUILDDIR)/, $(PIGGY_C_SRC:%.c=%.o))
PIGGY_OBJS	+= $(addprefix $(BUILDDIR)/, $(PIGGY_A_SRC:%.S=%.o))
PIGGY_OBJS	+= $(addprefix $(BUILDDIR)/, $(ZLIBOBJ))
			  
include $(UMONDIR)/target/make/objects.make

OBJS = $(addprefix $(BUILDDIR)/,\
	$(LOCSOBJ)  \
	$(CPUSOBJ)  \
	$(LOCCOBJ)  \
	$(CPUCOBJ)  \
	$(COMCOBJ)  \
	$(IODEVOBJ) \
	$(JZLIBOBJ) \
	$(ZLIBOBJ)  \
	$(GLIBOBJ) )
#	$(FATFSOBJ)

DEPS = $(OBJS:%.o=%.d) $(addprefix $(BUILDDIR)/, $(PIGGY_OBJS:%.o=%.d))

VPATH = $(COMMON3400DIR):$(CPUDIR):$(COMDIR):$(DEVDIR):$(FATFSDIR):$(JZLIBDIR):$(ZLIBDIR):$(GLIBDIR)

CFLAGS += -DBUILD_ID="\"$(BUILD_ID)\""
ifneq ($(DDR),)
CFLAGS += -D"DDR_CLOCK=$(DDR)"
endif

#########################################################################
#
# Targets...
#

all: $(PIGGY_BIN)

###########################################################################
#
# clean (overrides the clean from rules.make):
#
clean:
	rm -f $(BUILDDIR)/* 

-include $(DEPS)

# Compile: create object files from C source files.
$(BUILDDIR)/%.o : %.c
	@echo "Dependency:	" $<
	@mkdir -p $(BUILDDIR)
	@$(CC) $(CFLAGS) -MM $< -MT $@ >$@.tmp
	@sed -e 's/c:/\/cygdrive\/c/' -e 's/C:/\/cygdrive\/c/' <$@.tmp >$(basename $@).d
	@rm -f $@.tmp
	@echo "Compiling:	" $<
	@mkdir -p $(BUILDDIR)
	@$(CC) -c $(CFLAGS) -o $@ $<

# Compile: create object files from S source files.
$(BUILDDIR)/%.o : %.S
	@echo "Dependency:	" $<
	@mkdir -p $(BUILDDIR)
	@$(CC) $(CFLAGS) -MM $< -MT $@ >$@.tmp
	@sed -e 's/c:/\/cygdrive\/c/' -e 's/C:/\/cygdrive\/c/' <$@.tmp >$(basename $@).d
	@rm -f $@.tmp
	@echo "Assembling:	" $<
	@mkdir -p $(BUILDDIR)
	@$(CC) -c $(CFLAGS) -o $@ $<
