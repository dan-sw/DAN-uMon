#########################################################################
# 
#  uMon/LWIP demonstration application.
#  Simple application that demonstrates how a generic uMon application
#  can run an HTTP server (or other TCP/IP servers).
#  Run 'make' to build the target-resident application, then run
#  'make html' or 'make html1' to install the necessary html files.
#  Both of these html file sets are used to demonstrate issuing uMon
#  commands via HTTP; one uses frames, one does not.
#
#  Similar to other uMon demo applications, the user must provide
#  the parameters below.  The CLOCK_MS value is used to tell the
#  application how many clock ticks represent one millisecond.
#  If this is set to zero, then the code will assume that the underlying
#  monitor is release 1.16 (or newer) which contains the mon_timer()
#  API hook;  otherwise it will use CLOCK_MS along with a hardware-specific
#  clock_time() function (as of this writing, available for PPC &
#  BLACKFIN) that returns the content of a hardware timer.
#

PORT		= BF537
ARCH		=
MONCOMPTR	=
APPRAMBASE	=
TARGET_IP	=
CLOCK_MS	=
ENDIANNESS	=

USEAPPSTACK	= 1

# For convenience, some of the ports are preconfigured if you add
# "PORT=NNNN" to the command line...
#
ifeq ($(PORT),CSB472)
ARCH		= PPC
MONCOMPTR	= 0xfffffff0
APPRAMBASE	= 0x28000
TARGET_IP	= 135.222.138.20
CLOCK_MS	= 200000
ENDIANNESS	= -D BYTE_ORDER=BIG_ENDIAN
endif

ifeq ($(PORT),BF537)
ARCH		= BLACKFIN
MONCOMPTR	= 0x03f00004
APPRAMBASE	= 0x100000
TARGET_IP	= 135.222.138.21
CLOCK_MS	= 0
ENDIANNESS	= -D BYTE_ORDER=LITTLE_ENDIAN
endif

ifeq ($(PORT),CSB740)
ARCH		= ARM
MONCOMPTR	= 0x08000038
APPRAMBASE	= 0x80400000
TARGET_IP	= 135.222.138.21
CLOCK_MS	= 0
ENDIANNESS	= -D BYTE_ORDER=LITTLE_ENDIAN
endif

ifeq ($(PORT),CSB726)
ARCH		= ARM
MONCOMPTR	= 0x20
APPRAMBASE	= 0xa008d000
TARGET_IP	= 135.222.138.21
CLOCK_MS	= 0
ENDIANNESS	= -D BYTE_ORDER=LITTLE_ENDIAN
endif

ifeq ($(PORT),CSB732)
ARCH		= ARM
MONCOMPTR	= 0xa0000038
APPRAMBASE	= 0x80200000
TARGET_IP	= 135.222.138.21
CLOCK_MS	= 0
ENDIANNESS	= -D BYTE_ORDER=LITTLE_ENDIAN
endif



#########################################################################
#
#  Nothing below this point should have to be changed.
#  This uMon/LWIP application requires only that the
#  CLOCK_MS value be set based on the target-specific
#  function 'clock_time()'.  It should be set to the
#  number of ticks that represents 1 millisecond.
#

APPNAME		= lwipapp
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
INCPATHS	= -I . -I ./net -I ./lwip/src/include/ipv4 \
			  -I ./lwip/src/include -I ./lwip/src/include/netif \
			  -I ./lwip/src/netif/ppp

ifeq ($(ARCH),MIPS)
TOOL_PREFIX	:= mips-elf
CFLAGS		:= -fno-builtin -G 0 -march=r4600 -mips3 -mno-abicalls \
			  -fno-pic -c -g -O2 -Wall -EB $(INCPATHS) $(ENDIANNESS)
CRT0		:= crt0_mips.o
CPU			:= -D CPU_IS_MIPS=1
endif

ifeq ($(ARCH),PPC)
TOOL_PREFIX	:= ppc-elf
CFLAGS		:= -fno-builtin -mno-sdata -msoft-float \
			   -c -Wall -O -g $(INCPATHS) $(ENDIANNESS)
CRT0		:= crt0_ppc.o
CPU			:= -D CPU_IS_PPC=1
LIBGCC		= `$(CC) --print-file-name=nof/libgcc.a`
endif

ifeq ($(ARCH),ARM)
TOOL_PREFIX	:= arm-elf
CFLAGS		:= -fno-builtin -mcpu=arm9tdmi \
			   -c -Wall -O2 -g $(INCPATHS) $(ENDIANNESS)
CRT0		:= crt0_arm.o
CPU			:= -D CPU_IS_ARM=1
endif

ifeq ($(ARCH),BLACKFIN)
TOOL_PREFIX	:= bfin-elf
CFLAGS		:= -fno-builtin -mcsync-anomaly -c -O -Wall -g \
			   $(INCPATHS) $(ENDIANNESS)
CRT0		:= crt0_bfin.o
CPU			:= -D CPU_IS_BFIN=1
endif

ifeq ($(ARCH),MICROBLAZE)
TOOL_PREFIX	:= C:/EDK/gnu/microblaze/nt/bin/mb
LIBPATH		:= -L C:/xilinx/els_stuff/projects/avnet_spartan3_devkit/microblaze_0/lib
CFLAGS		:= -fno-builtin -mno-xl-soft-mul -c -Wall -O -g \
			   $(INCPATHS) $(ENDIANNESS)
CRT0		:= crt0_mb.o
CPU			:= -D CPU_IS_MICROBLAZE=1
endif

ifeq ($(ARCH),COLDFIRE)
TOOL_PREFIX	:= m68k-elf
CFLAGS		:= -Wall -fno-builtin -msoft-float -m5200 -g -c \
			   $(INCPATHS) $(ENDIANNESS)
CRT0		:= crt0_cf.o
CPU			:= -D CPU_IS_68K=1
#LIBGCC		= `$(CC) -m5200 --print-libgcc-file-name`
LIBGCC		= /usr/lib/gcc-lib/m68k-elf/3.2/m5200/libgcc.a -L /usr/m68k-elf/lib/m5200
endif

BASE_OBJS=$(CRT0) main.o console.o monlib.o

#
#########################################################################
#
NET_OBJS	= fs.o httpd.o httpget.o monsrvr.o telnetc.o umonlwip.o

API_OBJS	= api_lib.o api_msg.o err.o netbuf.o netdb.o netifapi.o

CORE_OBJS	= dhcp.o dhcp.o dns.o init.o mem.o memp.o netif.o pbuf.o \
			  raw.o stats.o sys.o tcp.o tcp_in.o tcp_out.o udp.o

IPV4_OBJS	= autoip.o icmp.o igmp.o inet.o inet_chksum.o ip.o \
			  ip_addr.o ip_frag.o

SNMP_OBJS	= asn1_dec.o asn1_enc.o mib2.o mib_structs.o msg_in.o \
			  msg_out.o

NETIF_OBJS	= etharp.o

PPP_OBJS	= auth.o chap.o chpms.o fsm.o ipcp.o lcp.o magic.o md5.o \
			  pap.o ppp.o ppp_oe.o randm.o vj.o

LWIP_OBJS	= $(API_OBJS) $(CORE_OBJS) $(IPV4_OBJS) $(NETIF_OBJS)
OBJS		= $(LWIP_OBJS) $(NET_OBJS) $(BASE_OBJS)

#####
#
# $(APPNAME):
# Top level target builds the application.
#
$(APPNAME): varcheck $(OBJS) makefile
	echo tools: $(TOOL_PREFIX)
	$(LD) -e start -o $(APPNAME) -Ttext $(APPRAMBASE) $(OBJS) $(LIBPATH) -lc $(LIBGCC) 
	$(OBJDUMP) --source --disassemble $(APPNAME) >$(APPNAME).dis
	$(NM) --numeric-sort $(APPNAME) >$(APPNAME).sym
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
crt0_68k.o: crt0_68k.S
	$(CC) $(CFLAGS) -D USE_APPSTACK=$(APPSTACK) -o $@ crt0_68k.S

crt0_arm.o: crt0_arm.S
	$(CC) $(CFLAGS) -D USE_APPSTACK=$(USEAPPSTACK) -o $@ crt0_arm.S

crt0_bfin.o: crt0_bfin.S
	$(CC) $(CFLAGS) -D USE_APPSTACK=$(USEAPPSTACK) -o $@ crt0_bfin.S

crt0_mips.o: crt0_mips.S
	$(CC) $(CFLAGS) -D USE_APPSTACK=$(USEAPPSTACK) -o $@ crt0_mips.S

crt0_mb.o: crt0_mb.S
	$(CC) $(CFLAGS) -D USE_APPSTACK=$(USEAPPSTACK) -o $@ crt0_mb.S

crt0_ppc.o: crt0_ppc.S
	$(CC) $(CFLAGS) -D USE_APPSTACK=$(USEAPPSTACK) -o $@ crt0_ppc.S

crt0_sh2.o: crt0_sh2.S
	$(CC) $(CFLAGS) -D USE_APPSTACK=$(USEAPPSTACK) -o $@ crt0_sh2.S

main.o: main.c 
	$(CC) $(CFLAGS) $(CPU) -o $@ \
		-D USE_APPSTACK=$(USEAPPSTACK) -D MONCOMPTR=$(MONCOMPTR) \
		main.c

console.o: console.c 
	$(CC) $(CFLAGS) $(CPU) -o $@ console.c

misc.o: misc.c 
	$(CC) $(CFLAGS) $(CPU) -o $@ misc.c

monlib.o: monlib.c
	$(CC) $(CFLAGS) -o $@ monlib.c


# To rebuild depends, add -MM to CFLAGS, then run make.
include depends

#########################################################################
#
# Dependencies:
#
fs.o:	 ./net/fs.c
		 $(CC) $(CFLAGS) ./net/fs.c

httpd.o:	 ./net/httpd.c
		 $(CC) $(CFLAGS) ./net/httpd.c

httpget.o:	 ./net/httpget.c
		 $(CC) $(CFLAGS) ./net/httpget.c

telnetc.o:	 ./net/telnetc.c
		 $(CC) $(CFLAGS) ./net/telnetc.c

umonlwip.o:	 ./net/umonlwip.c
		 $(CC) $(CFLAGS) -D CLOCK_MS=$(CLOCK_MS) ./net/umonlwip.c

api_lib.o:	 ./lwip/src/api/api_lib.c
		 $(CC) $(CFLAGS) ./lwip/src/api/api_lib.c

api_msg.o:	 ./lwip/src/api/api_msg.c
		 $(CC) $(CFLAGS) ./lwip/src/api/api_msg.c

err.o:	 ./lwip/src/api/err.c
		 $(CC) $(CFLAGS) ./lwip/src/api/err.c

netbuf.o:	 ./lwip/src/api/netbuf.c
		 $(CC) $(CFLAGS) ./lwip/src/api/netbuf.c

netdb.o:	 ./lwip/src/api/netdb.c
		 $(CC) $(CFLAGS) ./lwip/src/api/netdb.c

netifapi.o:	 ./lwip/src/api/netifapi.c
		 $(CC) $(CFLAGS) ./lwip/src/api/netifapi.c

sockets.o:	 ./lwip/src/api/sockets.c
		 $(CC) $(CFLAGS) ./lwip/src/api/sockets.c

tcpip.o:	 ./lwip/src/api/tcpip.c
		 $(CC) $(CFLAGS) ./lwip/src/api/tcpip.c

dhcp.o:	 ./lwip/src/core/dhcp.c
		 $(CC) $(CFLAGS) ./lwip/src/core/dhcp.c

dns.o:	 ./lwip/src/core/dns.c
		 $(CC) $(CFLAGS) ./lwip/src/core/dns.c

init.o:	 ./lwip/src/core/init.c
		 $(CC) $(CFLAGS) ./lwip/src/core/init.c

autoip.o:	 ./lwip/src/core/ipv4/autoip.c
		 $(CC) $(CFLAGS) ./lwip/src/core/ipv4/autoip.c

icmp.o:	 ./lwip/src/core/ipv4/icmp.c
		 $(CC) $(CFLAGS) ./lwip/src/core/ipv4/icmp.c

igmp.o:	 ./lwip/src/core/ipv4/igmp.c
		 $(CC) $(CFLAGS) ./lwip/src/core/ipv4/igmp.c

inet.o:	 ./lwip/src/core/ipv4/inet.c
		 $(CC) $(CFLAGS) ./lwip/src/core/ipv4/inet.c

inet_chksum.o:	 ./lwip/src/core/ipv4/inet_chksum.c
		 $(CC) $(CFLAGS) ./lwip/src/core/ipv4/inet_chksum.c

ip.o:	 ./lwip/src/core/ipv4/ip.c
		 $(CC) $(CFLAGS) ./lwip/src/core/ipv4/ip.c

ip_addr.o:	 ./lwip/src/core/ipv4/ip_addr.c
		 $(CC) $(CFLAGS) ./lwip/src/core/ipv4/ip_addr.c

ip_frag.o:	 ./lwip/src/core/ipv4/ip_frag.c
		 $(CC) $(CFLAGS) ./lwip/src/core/ipv4/ip_frag.c

mem.o:	 ./lwip/src/core/mem.c
		 $(CC) $(CFLAGS) ./lwip/src/core/mem.c

memp.o:	 ./lwip/src/core/memp.c
		 $(CC) $(CFLAGS) ./lwip/src/core/memp.c

netif.o:	 ./lwip/src/core/netif.c
		 $(CC) $(CFLAGS) ./lwip/src/core/netif.c

pbuf.o:	 ./lwip/src/core/pbuf.c
		 $(CC) $(CFLAGS) ./lwip/src/core/pbuf.c

raw.o:	 ./lwip/src/core/raw.c
		 $(CC) $(CFLAGS) ./lwip/src/core/raw.c

asn1_dec.o:	 ./lwip/src/core/snmp/asn1_dec.c
		 $(CC) $(CFLAGS) ./lwip/src/core/snmp/asn1_dec.c

asn1_enc.o:	 ./lwip/src/core/snmp/asn1_enc.c
		 $(CC) $(CFLAGS) ./lwip/src/core/snmp/asn1_enc.c

mib2.o:	 ./lwip/src/core/snmp/mib2.c
		 $(CC) $(CFLAGS) ./lwip/src/core/snmp/mib2.c

mib_structs.o:	 ./lwip/src/core/snmp/mib_structs.c
		 $(CC) $(CFLAGS) ./lwip/src/core/snmp/mib_structs.c

msg_in.o:	 ./lwip/src/core/snmp/msg_in.c
		 $(CC) $(CFLAGS) ./lwip/src/core/snmp/msg_in.c

msg_out.o:	 ./lwip/src/core/snmp/msg_out.c
		 $(CC) $(CFLAGS) ./lwip/src/core/snmp/msg_out.c

stats.o:	 ./lwip/src/core/stats.c
		 $(CC) $(CFLAGS) ./lwip/src/core/stats.c

sys.o:	 ./lwip/src/core/sys.c
		 $(CC) $(CFLAGS) ./lwip/src/core/sys.c

tcp.o:	 ./lwip/src/core/tcp.c
		 $(CC) $(CFLAGS) ./lwip/src/core/tcp.c

tcp_in.o:	 ./lwip/src/core/tcp_in.c
		 $(CC) $(CFLAGS) ./lwip/src/core/tcp_in.c

tcp_out.o:	 ./lwip/src/core/tcp_out.c
		 $(CC) $(CFLAGS) ./lwip/src/core/tcp_out.c

udp.o:	 ./lwip/src/core/udp.c
		 $(CC) $(CFLAGS) ./lwip/src/core/udp.c

etharp.o:	 ./lwip/src/netif/etharp.c
		 $(CC) $(CFLAGS) ./lwip/src/netif/etharp.c

ethernetif.o:	 ./lwip/src/netif/ethernetif.c
		 $(CC) $(CFLAGS) ./lwip/src/netif/ethernetif.c

loopif.o:	 ./lwip/src/netif/loopif.c
		 $(CC) $(CFLAGS) ./lwip/src/netif/loopif.c

auth.o:	 ./lwip/src/netif/ppp/auth.c
		 $(CC) $(CFLAGS) ./lwip/src/netif/ppp/auth.c

chap.o:	 ./lwip/src/netif/ppp/chap.c
		 $(CC) $(CFLAGS) ./lwip/src/netif/ppp/chap.c

chpms.o:	 ./lwip/src/netif/ppp/chpms.c
		 $(CC) $(CFLAGS) ./lwip/src/netif/ppp/chpms.c

fsm.o:	 ./lwip/src/netif/ppp/fsm.c
		 $(CC) $(CFLAGS) ./lwip/src/netif/ppp/fsm.c

ipcp.o:	 ./lwip/src/netif/ppp/ipcp.c
		 $(CC) $(CFLAGS) ./lwip/src/netif/ppp/ipcp.c

lcp.o:	 ./lwip/src/netif/ppp/lcp.c
		 $(CC) $(CFLAGS) ./lwip/src/netif/ppp/lcp.c

magic.o:	 ./lwip/src/netif/ppp/magic.c
		 $(CC) $(CFLAGS) ./lwip/src/netif/ppp/magic.c

md5.o:	 ./lwip/src/netif/ppp/md5.c
		 $(CC) $(CFLAGS) ./lwip/src/netif/ppp/md5.c

pap.o:	 ./lwip/src/netif/ppp/pap.c
		 $(CC) $(CFLAGS) ./lwip/src/netif/ppp/pap.c

ppp.o:	 ./lwip/src/netif/ppp/ppp.c
		 $(CC) $(CFLAGS) ./lwip/src/netif/ppp/ppp.c

ppp_oe.o:	 ./lwip/src/netif/ppp/ppp_oe.c
		 $(CC) $(CFLAGS) ./lwip/src/netif/ppp/ppp_oe.c

randm.o:	 ./lwip/src/netif/ppp/randm.c
		 $(CC) $(CFLAGS) ./lwip/src/netif/ppp/randm.c

vj.o:	 ./lwip/src/netif/ppp/vj.c
		 $(CC) $(CFLAGS) ./lwip/src/netif/ppp/vj.c

slipif.o:	 ./lwip/src/netif/slipif.c
		 $(CC) $(CFLAGS) ./lwip/src/netif/slipif.c

monsrvr.o:	 ./net/monsrvr.c
		 $(CC) $(CFLAGS) ./net/monsrvr.c


##########################################################################
#
# UTILITIES:
#
#####
#
# clean:
# Remove all files created by this make.
#
clean:
	find . -name '*.o' -exec rm {} \;
	rm -f $(APPNAME) $(APPNAME).ezip $(APPNAME).sym $(APPNAME).dis symtbl
	rm -f cscope.out tags cscope.files

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
	moncmd -q -w2 $(TARGET_IP) reset
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

#####
#
# dis:
# Build a disassembly of the image...
#
dis:
	$(OBJDUMP) --source --disassemble $(APPNAME) >$(APPNAME).dis

#####
#
# html/html1:
# Two slightly different versions of html file sets.
# Both are used to provide an HTTP interface to uMon's command set.
# One (htmldir) uses frames, the other does not.
#
html:
	(cd htmldir; ./loadhtml $(TARGET_IP); cd -)

html1:
	(cd htmldir1; ./loadhtml $(TARGET_IP); cd -)

#####
#
# cscope:
# Remove all files created by this make.
#
cscope:
	find . -name '*.[chS]' | grep -v ipv6 | grep -v ppp | grep -v snmp >cscope.files
	ctags --file-tags=yes -n -L cscope.files

