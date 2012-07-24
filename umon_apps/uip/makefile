##########################################################################
#
# Basic, target/architecture independent makefile for building an
# application that runs on its own stack. 
# See notes in umon_apps/demo/makefile for generic details.
# This application demonstrates the hookup of the uIP TCP/IP stack from
# Adam Dunkels (http://www.sics.se/~adam/uip/index.php/Main_Page)
# with uMon's API to provide a simple, OS-less application envvironment
# that includes a file system and TCP/IP.  
# The intent is that this code be almost 100% independent of the target
# (except for the few variables that must be set below).
#
# As of this writing (Apr 2008), the code incorporates uMon 1.15 and
# uIP-1.0.  The complete UIP source tree is included here, and immediate
# builds for the hello-world, telnetd and webserver applications in uIP
# are supported.
#
# Site dependent information that must be supplied by the user:
# Adjust these values based on your system configuration.  
# ARCH:
# 	Set ARCH to one of the accepted CPU architectures (i.e. MIPS
#	PPC, ARM, BLACKFIN, COLDFIRE, MICROBLAZE).
# MONCOMPTR:
# 	Set MONCOMPTR to the output of 'echo $MONCOMPTR' on your target.
# APPRAMBASE:
# 	Set APPRAMBASE to the output of 'echo $APPRAMBASE' on your target.
# TARGET_IP:
#	Set TARGET_IP to the IP address of your target.  This is only needed
#	if you use make to download the application to your target.
#
# UIPAPP (used by uIP):
#   Set to 'hello', 'httpd' or 'telnetd' to build that uIP application.
# BYTEORDER (used by uIP):
#   Set to UIP_BIG_ENDIAN or UIP_LITTLE_ENDIAN
# CLOCK (used by uIP):
#   Set to the number of increments the clock_timer() granularity
#	equates to 1 second.
#
# The following settings apply to the Cogent CSB472 as an example:
#
# ARCH			= PPC
# MONCOMPTR		= 0xfffffff0
# APPRAMBASE	= 0x20000
# TARGET_IP		= 135.222.138.19
# BYTEORDER		= UIP_BIG_ENDIAN
# UIPAPP		= telnetd
# CLOCK			= 200000
#
#
##########################################################################
#
ARCH		=
MONCOMPTR	=
APPRAMBASE	=
TARGET_IP	=
BYTEORDER	=
UIPAPP		=
CLOCK		=

###########################################################################
#
# Assuming the directory structure used for uip-1.0 doesn't change,
# even if you use something newer than uip-1.0, you shouldn't have
# to touch anything below this point...
#
UIPTOP		= uip-1.0
UIPDIR		= $(UIPTOP)/uip
UIPLIBDIR	= $(UIPTOP)/lib

#####
#
# hello-world:
# Build and install, then telnet to the target at port 1000.
# The server will greet you.
#
ifeq ($(UIPAPP),hello)
UIPAPPFILES	= hello-world.o
UIPAPPDIR	= $(UIPTOP)/apps/hello-world
UIPAPPDEF	= -D HELLOWORLD_APP
endif

#####
#
# webserver:
# Build and install, then connect your browser this targets' IP address.
# The server will display some generic UIP html.
#
ifeq ($(UIPAPP),httpd)
UIPAPPFILES	= http-strings.o httpd-cgi.o httpd-fs.o httpd.o
UIPAPPDIR	= $(UIPTOP)/apps/webserver
UIPAPPDEF	= -D WEBSERVER_APP
endif

#####
#
# telnetd:
# Build and install, then telnet to the target to be connected to a
# simple shell.
#
ifeq ($(UIPAPP),telnetd)
UIPAPPFILES	= umonshell.o telnetd.o memb.o
UIPAPPDIR	= $(UIPTOP)/apps/telnetd
UIPAPPDEF	= -D TELNETD_APP
endif


##########################################################################
#
# There should be no need to change anything below this point if
# building for the csb350, csb472, csb337 or csb360...
# 
APPNAME		= $(UIPAPP)
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

CFLAGS		:= -c -g -Wall -fno-builtin \
			   -I. -I $(UIPDIR) -I $(UIPAPPDIR) -I $(UIPLIBDIR) \
			   $(UIPAPPDEF) -D UIP_CONF_BYTE_ORDER=$(BYTEORDER) \
			   -D CLOCK_CONF_SECOND=$(CLOCK)

ifeq ($(ARCH),MIPS)
TOOL_PREFIX	:= mips-elf
CFLAGS		+= -G 0 -march=r4600 -mips3 -mno-abicalls \
			  -fno-pic -O2 -EB 
CRT0		:= crt0_mips.o
CPU			:= -D CPU_IS_MIPS=1
endif

ifeq ($(ARCH),PPC)
TOOL_PREFIX	:= ppc-elf
CFLAGS		+= -mno-sdata -msoft-float -O2
CRT0		:= crt0_ppc.o
CPU			:= -D CPU_IS_PPC=1
LIBGCC		= `$(CC) --print-file-name=nof/libgcc.a`
endif

ifeq ($(ARCH),ARM)
TOOL_PREFIX	:= arm-elf
CFLAGS		+= -mcpu=arm9tdmi -O
CRT0		:= crt0_arm.o
CPU			:= -D CPU_IS_ARM=1
endif

ifeq ($(ARCH),BLACKFIN)
TOOL_PREFIX	:= bfin-elf
CFLAGS		+= -mcsync-anomaly -O 
CRT0		:= crt0_bfin.o
CPU			:= -D CPU_IS_BFIN=1
endif

ifeq ($(ARCH),MICROBLAZE)
TOOL_PREFIX	:= C:/EDK/gnu/microblaze/nt/bin/mb
LIBPATH		:= -L C:/xilinx/els_stuff/projects/avnet_spartan3_devkit/microblaze_0/lib
CFLAGS		+= -mno-xl-soft-mul -O 
CRT0		:= crt0_mb.o
CPU			:= -D CPU_IS_MICROBLAZE=1
endif

ifeq ($(ARCH),COLDFIRE)
TOOL_PREFIX	:= m68k-elf
CFLAGS		+= -msoft-float -m5200 
CRT0		:= crt0_cf.o
CPU			:= -D CPU_IS_68K=1
#LIBGCC		= `$(CC) -m5200 --print-libgcc-file-name`
LIBGCC		= /usr/lib/gcc-lib/m68k-elf/3.2/m5200/libgcc.a -L /usr/m68k-elf/lib/m5200
endif

OBJS=$(CRT0) cstart.o main.o psock.o timer.o uip-fw.o uip-neighbor.o \
			 uip-split.o uip.o uiplib.o uip_arp.o monlib.o \
			 $(UIPAPPFILES)

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
ifndef CLOCK
	@echo Must specify CLOCK=XXX on command line.
	@exit 1
endif	
ifndef BYTEORDER
	@echo "Must specify BYTEORDER=XXX"
	@echo "where XXX is UIP_BIG_ENDIAN or UIP_LITTLE_ENDIAN"
	@exit 1
endif
ifndef UIPAPP
	@echo "Must specify UIPAPP=XXX"
	@echo "   where XXX is hello, httpd or telnetd"
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
crt0_68k.o: crt0_68k.S
	$(CC) $(CFLAGS) -o $@ crt0_68k.S

crt0_arm.o: crt0_arm.S
	$(CC) $(CFLAGS) -o $@ crt0_arm.S

crt0_bfin.o: crt0_bfin.S
	$(CC) $(CFLAGS) -o $@ crt0_bfin.S

crt0_mips.o: crt0_mips.S
	$(CC) $(CFLAGS) -o $@ crt0_mips.S

crt0_mb.o: crt0_mb.S
	$(CC) $(CFLAGS) -o $@ crt0_mb.S

crt0_ppc.o: crt0_ppc.S
	$(CC) $(CFLAGS) -o $@ crt0_ppc.S

crt0_sh2.o: crt0_sh2.S
	$(CC) $(CFLAGS) -o $@ crt0_sh2.S

cstart.o: cstart.c 
	$(CC) $(CFLAGS) -D MONCOMPTR=$(MONCOMPTR) -o $@ cstart.c

main.o: main.c 
	$(CC) $(CFLAGS) $(CPU) -o $@ main.c

monlib.o: monlib.c
	$(CC) $(CFLAGS) -o $@ monlib.c

strace.o: strace.c 
	$(CC) $(CFLAGS) $(CPU) -o $@ strace.c

psock.o:	$(UIPDIR)/psock.c
	$(CC) $(CFLAGS) $(CPU) -o $@ $(UIPDIR)/psock.c

timer.o:	$(UIPDIR)/timer.c
	$(CC) $(CFLAGS) $(CPU) -o $@ $(UIPDIR)/timer.c

uip-fw.o:	$(UIPDIR)/uip-fw.c
	$(CC) $(CFLAGS) $(CPU) -o $@ $(UIPDIR)/uip-fw.c

uip-neighbor.o:	$(UIPDIR)/uip-neighbor.c
	$(CC) $(CFLAGS) $(CPU) -o $@ $(UIPDIR)/uip-neighbor.c

uip-split.o:	#(UIPDIR)/uip-split.c
	$(CC) $(CFLAGS) $(CPU) -o $@ $(UIPDIR)/uip-split.c

uip.o:	$(UIPDIR)/uip.c
	$(CC) $(CFLAGS) $(CPU) -o $@ $(UIPDIR)/uip.c

uiplib.o:	$(UIPDIR)/uiplib.c
	$(CC) $(CFLAGS) $(CPU) -o $@ $(UIPDIR)/uiplib.c

uip_arp.o:	$(UIPDIR)/uip_arp.c
	$(CC) $(CFLAGS) $(CPU) -o $@ $(UIPDIR)/uip_arp.c

hello-world.o:	$(UIPAPPDIR)/hello-world.c
	$(CC) $(CFLAGS) $(CPU) -o $@ $(UIPAPPDIR)/hello-world.c

http-strings.o:	$(UIPAPPDIR)/http-strings.c
	$(CC) $(CFLAGS) $(CPU) -o $@ $(UIPAPPDIR)/http-strings.c

httpd-cgi.o:	$(UIPAPPDIR)/httpd-cgi.c
	$(CC) $(CFLAGS) $(CPU) -o $@ $(UIPAPPDIR)/httpd-cgi.c

httpd-fs.o:	$(UIPAPPDIR)/httpd-fs.c
	$(CC) $(CFLAGS) $(CPU) -o $@ $(UIPAPPDIR)/httpd-fs.c

httpd.o:	$(UIPAPPDIR)/httpd.c
	$(CC) $(CFLAGS) $(CPU) -o $@ $(UIPAPPDIR)/httpd.c

umonshell.o:	$(UIPAPPDIR)/umonshell.c
	$(CC) $(CFLAGS) $(CPU) -D USE_UMON_CMDTBL -o $@ $(UIPAPPDIR)/umonshell.c

telnetd.o:	$(UIPAPPDIR)/telnetd.c
	$(CC) $(CFLAGS) $(CPU) -o $@ $(UIPAPPDIR)/telnetd.c

memb.o:	$(UIPLIBDIR)/memb.c
	$(CC) $(CFLAGS) $(CPU) -o $@ $(UIPLIBDIR)/memb.c


#####
#
# clean:
# Remove all files created by this make.
#
clean:
	rm -f *.o $(APPNAME) $(APPNAME).ezip $(APPNAME).sym $(APPNAME).dis symtbl
	rm -f cscope.out tags cscope.files

#####
#
# cscope:
# Build a list of files that can then be used by cscope and ctags for
# source code browsing.  The 'ctags' command below, assumes you have
# Exuberant Ctags installed; however, others may work as well.
cscope:
	ls *.[ch] >cscope.files
	ls $(CRT0:.o=.S) >>cscope.files
	ls $(UIPDIR)/*.[ch] >>cscope.files
	ls $(UIPLIBDIR)/*.[ch] >>cscope.files
	ls $(UIPAPPDIR)/*.[ch] >>cscope.files
	ctags --file-tags=yes -n -L cscope.files

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
