##########################################################################
#
# Basic, target/architecture independent makefile for building an
# application that runs on its own stack. 
# This application is fairly thorough example of hooking up to uMon's
# packet (i.e. ethernet) interface; thus allowing a simple UDP based
# application to be built independent of the target's ethernet
# port (because it uses uMon's ethernet driver).
# 
# Site dependent information:
# Adjust these values based on your system configuration.  
# ARCH:
# 	Set ARCH to one of the accepted CPU architectures (i.e. MIPS
#	PPC, ARM, BLACKFIN, MICROBLAZE, COLDFIRE).
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
# There should be no need to change anything below this point if
# building for the csb350, csb472, csb337 or csb360...
# 
SRCDIR		= ../demo
APPNAME		= unet
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
LIBPATH		=

ifeq ($(ARCH),MIPS)
TOOL_PREFIX	:= mips-elf
CFLAGS		:= -fno-builtin -G 0 -march=r4600 -mips3 -mno-abicalls \
			  -fno-pic -c -g -O2 -Wall -EB -I .  -I $(SRCDIR)
CRT0		:= crt0_mips.o
CPU			:= -D CPU_IS_MIPS=1
endif

ifeq ($(ARCH),PPC)
TOOL_PREFIX	:= ppc-elf
CFLAGS		:= -fno-builtin -mno-sdata -msoft-float \
			   -c -Wall -O -g -I.  -I $(SRCDIR)
CRT0		:= crt0_ppc.o
CPU			:= -D CPU_IS_PPC=1
endif

ifeq ($(ARCH),ARM)
TOOL_PREFIX	:= arm-elf
CFLAGS		:= -fno-builtin -mcpu=arm9tdmi \
			   -D CPU_LE -c -Wall -O -g -I. -I $(SRCDIR)
CRT0		:= crt0_arm.o
CPU			:= -D CPU_IS_ARM=1
endif

ifeq ($(ARCH),BLACKFIN)
TOOL_PREFIX	:= bfin-elf
CFLAGS		:= -fno-builtin -mcsync-anomaly -c \
			   -Wall -O -g -I. -I $(SRCDIR) -D CPU_LE
CRT0		:= crt0_bfin.o
CPU			:= -D CPU_IS_BFIN=1
endif

ifeq ($(ARCH),MICROBLAZE)
TOOL_PREFIX	:= C:/EDK/gnu/microblaze/nt/bin/mb
LIBPATH		:= -L C:/xilinx/els_stuff/projects/avnet_spartan3_devkit/microblaze_0/lib
CFLAGS		:= -fno-builtin -mno-xl-soft-mul -c -Wall -O -g \
				-I. -I $(SRCDIR)
CRT0		:= crt0_mb.o
CPU			:= -D CPU_IS_MICROBLAZE=1
endif

ifeq ($(ARCH),COLDFIRE)
TOOL_PREFIX	:= m68k-elf
CFLAGS		:= -Wall -fno-builtin -msoft-float -m5200 \
			   -g -c -I. -I $(SRCDIR)
CRT0		:= crt0_cf.o
CPU			:= -D CPU_IS_68K=1
#LIBGCC		= `$(CC) -m5200 --print-libgcc-file-name`
LIBGCC		= /usr/lib/gcc-lib/m68k-elf/3.2/m5200/libgcc.a -L /usr/m68k-elf/lib/m5200
endif

OBJS=$(CRT0) main.o monlib.o arp.o icmp.o unet.o cmd.o udp.o

#####
#
# $(APPNAME):
# Top level target builds the application.
#
$(APPNAME): varcheck $(OBJS) makefile
	echo tools: $(TOOL_PREFIX)
	$(LD) -e start -o $(APPNAME) -Ttext $(APPRAMBASE) $(OBJS) $(LIBPATH) -lc $(LIBGCC) 
	$(NM) --numeric-sort $(APPNAME) >$(APPNAME).sym
	$(OBJDUMP) --source --disassemble $(APPNAME) > $(APPNAME).dis
	cp $(APPNAME)  $(APPNAME).withsyms
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
	@echo Invalid ARCH specification. Use PPC, ARM, MIPS, BLACKFIN or COLDFIRE.
	@exit 1
endif	
ifeq ($(TOOL_PREFIX),-)
	@echo Invalid ARCH specification. Use PPC, ARM, MIPS, BLACKFIN or COLDFIRE.
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
crt0_68k.o: $(SRCDIR)/crt0_68k.S
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/crt0_68k.S

crt0_bfin.o: $(SRCDIR)/crt0_bfin.S
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/crt0_bfin.S

crt0_arm.o: $(SRCDIR)/crt0_arm.S
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/crt0_arm.S

crt0_mips.o: $(SRCDIR)/crt0_mips.S
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/crt0_mips.S

crt0_mb.o: $(SRCDIR)/crt0_mb.S
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/crt0_mb.S

crt0_ppc.o: $(SRCDIR)/crt0_ppc.S
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/crt0_ppc.S

crt0_sh2.o: $(SRCDIR)/crt0_sh2.S
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/crt0_sh2.S

main.o: main.c unet.h
	$(CC) $(CFLAGS) -D MONCOMPTR=$(MONCOMPTR) -o $@ main.c

monlib.o: $(SRCDIR)/monlib.c
	$(CC) $(CFLAGS) -o $@ $(SRCDIR)/monlib.c

arp.o: arp.c unet.h
	$(CC) $(CFLAGS) -o $@ arp.c

cmd.o: cmd.c cmd.h unet.h
	$(CC) $(CFLAGS) -o $@ cmd.c

icmp.o: icmp.c unet.h
	$(CC) $(CFLAGS) -o $@ icmp.c

udp.o: udp.c unet.h
	$(CC) $(CFLAGS) -o $@ udp.c

unet.o: unet.c unet.h
	$(CC) $(CFLAGS) -o $@ unet.c

#####
#
# clean:
# Remove all files created by this make.
#
clean:
	rm -f *.o symtbl
	rm -f $(APPNAME) $(APPNAME).ezip $(APPNAME).sym 
	rm -f $(APPNAME).dis $(APPNAME).withsyms

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
