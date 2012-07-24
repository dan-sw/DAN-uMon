##########################################################################
#
# Target/architecture independent makefile for building a TFS-resident
# jffs2 command for use with MicroMonitor.
#
# Site dependent information:
# Adjust these values based on your system configuration.  
# ARCH:
# 	Set ARCH to one of the accepted CPU architectures (i.e. MIPS
#	PPC, ARM, COLDFIRE).
# MONCOMPTR:
# 	Set MONCOMPTR to the output of 'echo $MONCOMPTR' on your target.
# APPRAMBASE:
# 	Set APPRAMBASE to the output of 'echo $APPRAMBASE' on your target.
# TARGET_IP:
#	Set TARGET_IP to the IP address of your target.
#
ARCH		= 
MONCOMPTR	= 
APPRAMBASE	= 
TARGET_IP	= 

##########################################################################
#
# There should be no need to change anything below this point if building
# for csb350 (MIPS), csb472 (PPC), csb337 (ARM) or csb360 (ColdFire)...
# 
APPNAME		= jffs2
NM			= $(TOOL_PREFIX)-nm
AR			= $(TOOL_PREFIX)-ar
LD			= $(TOOL_PREFIX)-ld
ASM			= $(TOOL_PREFIX)-as
CC			= $(TOOL_PREFIX)-gcc
STRIP		= $(TOOL_PREFIX)-strip
OBJCOPY		= $(TOOL_PREFIX)-objcopy
OBJDUMP		= $(TOOL_PREFIX)-objdump
LIBGCC		= `$(CC) --print-libgcc-file-name`
LIBDIR		= $(LIBGCC:/libgcc.a=)
TGTDIR		= ../../umon_main/target
ZLIB		= $(TGTDIR)/fs/jffs2/zlib_inflate
GLIB		= $(TGTDIR)/glib
COMMON		= $(TGTDIR)/common
INCLUDES	= -I . -I $(COMMON) -I $(TGTDIR)/fs/jffs2/zlib_inflate 

ifeq ($(ARCH),MIPS)
TOOL_PREFIX	:= mips-elf
CFLAGS		:= -fno-builtin -G 0 -march=r4600 -mips3 -mno-abicalls \
			  -fno-pic -c -g -O2 -Wall -EB $(INCLUDES)
endif

ifeq ($(ARCH),PPC)
TOOL_PREFIX	:= ppc-elf
CFLAGS		:= -fno-builtin -mno-sdata -msoft-float -c -Wall -O -g $(INCLUDES)
endif

ifeq ($(ARCH),ARM)
TOOL_PREFIX	:= arm-elf
CFLAGS		:= -fno-builtin -mcpu=arm9tdmi -c -Wall -O -g $(INCLUDES)
endif

ifeq ($(ARCH),COLDFIRE)
TOOL_PREFIX	:= m68k-elf
CFLAGS		:= -Wall -fno-builtin -msoft-float -m5200 -g -c $(INCLUDES)
LIBGCC		= /usr/lib/gcc-lib/m68k-elf/3.2/m5200/libgcc.a -L /usr/m68k-elf/lib/m5200
endif

OBJS=main.o jz_infblock.o jz_infcodes.o jz_inffast.o jz_inflate.o \
	jz_inftrees.o jz_infutil.o ctypetbl.o getopt.o inrange.o monlib.o mprintf.o

xOBJS=main.o ctypetbl.o getopt.o monlib.o

#####
#
# $(APPNAME):
# Top level target builds the application.
#
$(APPNAME): varcheck $(OBJS) makefile
	$(LD) -e start -o $(APPNAME) -Ttext $(APPRAMBASE) $(OBJS) -lc $(LIBGCC)
	$(STRIP) $(APPNAME) 

#####
#
# Variable checks:
# Verify that the necessary variables have been set on the make
# command line.
#
varcheck:
ifndef ARCH
	@echo Must specify ARCH=XXX on command line.
	@exit 1
endif	
ifndef TOOL_PREFIX
	@echo Invalid ARCH specification, use PPC, ARM, MIPS or COLDFIRE.
	@exit 1
endif	
ifeq ($(TOOL_PREFIX),-)
	@echo Invalid ARCH specification, use PPC, ARM, MIPS or COLDFIRE.
	@exit 1
endif	
ifndef MONCOMPTR
	@echo Must specify MONCOMPTR=XXX on command line.
	@exit 1
endif	
ifndef APPRAMBASE
	@echo Must specify APPRAMBASE=XXX on command line.
	@exit 1
endif	

targetipcheck:
ifndef TARGET_IP
	@echo Must specify TARGET_IP=IPADDRESS on command line.
	@exit 1
endif	

#####
#
# Objects:
#
main.o: main.c 
	$(CC) $(CFLAGS) -D MONCOMPTR=$(MONCOMPTR) -o $@ main.c

jz_infblock.o: $(ZLIB)/jz_infblock.c
	$(CC) $(CFLAGS) -o $@ $(ZLIB)/jz_infblock.c

jz_infcodes.o: $(ZLIB)/jz_infcodes.c
	$(CC) $(CFLAGS) -o $@ $(ZLIB)/jz_infcodes.c

jz_inffast.o: $(ZLIB)/jz_inffast.c
	$(CC) $(CFLAGS) -o $@ $(ZLIB)/jz_inffast.c

jz_inflate.o: $(ZLIB)/jz_inflate.c
	$(CC) $(CFLAGS) -o $@ $(ZLIB)/jz_inflate.c

jz_inftrees.o: $(ZLIB)/jz_inftrees.c
	$(CC) $(CFLAGS) -o $@ $(ZLIB)/jz_inftrees.c

jz_infutil.o: $(ZLIB)/jz_infutil.c
	$(CC) $(CFLAGS) -o $@ $(ZLIB)/jz_infutil.c

ctypetbl.o: $(GLIB)/ctypetbl.c
	$(CC) $(CFLAGS) -o $@ $(GLIB)/ctypetbl.c

mprintf.o: $(COMMON)/mprintf.c
	$(CC) $(CFLAGS) -o $@ $(COMMON)/mprintf.c

getopt.o: $(GLIB)/getopt.c
	$(CC) $(CFLAGS) -o $@ $(GLIB)/getopt.c

inrange.o: $(GLIB)/inrange.c
	$(CC) $(CFLAGS) -o $@ $(GLIB)/inrange.c

monlib.o: $(COMMON)/monlib.c
	$(CC) $(CFLAGS) -o $@ $(COMMON)/monlib.c

#####
#
# clean:
# Remove all files created by this make.
#
clean:
	rm -f *.o $(APPNAME) $(APPNAME).ezip $(APPNAME).sym $(APPNAME).dis symtbl

#####
#
# sym:
# Create and download the symbol table file that can be used by uMon
# with this application...
#
sym: targetipcheck
	@if ! test -f $(APPNAME).sym; then echo Must build $(APPNAME) first; exit 1; fi
	monsym -p0x $(APPNAME).sym >symtbl
	ttftp $(TARGET_IP) put symtbl


	
#####
#
# dld:
# Use the ttftp tool (supplied with MicroMonitor) to download the
# application to the target.
#
dld: targetipcheck
	@if ! test -f $(APPNAME); then echo Must build $(APPNAME) first; exit 1; fi
	ttftp $(TARGET_IP) put $(APPNAME) $(APPNAME),E

#####
#
# zdld:
# Compress the elf file using the 'elf' tool (supplied with MicroMonitor)
# The output of this is "$(APPNAME).ezip", then download that compressed file.
#
zdld: targetipcheck 
	@if ! test -f $(APPNAME); then echo Must build $(APPNAME) first; exit 1; fi
	elf -z6 $(APPNAME)
	ttftp $(TARGET_IP) put $(APPNAME).ezip $(APPNAME),Ec
