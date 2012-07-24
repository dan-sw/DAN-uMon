APPRAMBASE	= 0x100000

APPNAME		= uumon
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
TARGET_IP	= 135.222.138.22

TOOL_PREFIX	:= bfin-elf
CFLAGS		:= -fno-builtin -mcsync-anomaly -c -Wall -O \
			   -I . -I port -I ../../umon_main/target/cpu/bfin

OBJS=entry.o main.o chario.o cmdtbl.o cmds.o cpuio.o docmd.o \
	genlib.o mprintf.o xmodem.o flashdev.o flash.o \
	arp.o bootp.o tftp.o phy.o enetcore.o etherdev.o  timestuff.o

#####
#
# $(APPNAME):
# Top level target builds the application.
#
$(APPNAME): $(OBJS) makefile
	$(LD) -e start -o $(APPNAME) -Ttext $(APPRAMBASE) $(OBJS) $(LIBGCC) 
	$(NM) --numeric-sort $(APPNAME) >$(APPNAME).sym
	$(OBJDUMP) --source --disassemble $(APPNAME) > $(APPNAME).dis
	elf -B uumon.bin uumon
	elf -m uumon

##	$(STRIP) $(APPNAME) 

#####
#
# Objects:
#
entry.o: entry.S
	$(CC) -c -o $@ entry.S

arp.o: arp.c 
	$(CC) $(CFLAGS) -o $@ arp.c

bootp.o: bootp.c 
	$(CC) $(CFLAGS) -o $@ bootp.c

chario.o: chario.c 
	$(CC) $(CFLAGS) -o $@ chario.c

cpuio.o: port/cpuio.c 
	$(CC) $(CFLAGS) -o $@ port/cpuio.c

cmdtbl.o: cmdtbl.c 
	$(CC) $(CFLAGS) -o $@ cmdtbl.c

docmd.o: docmd.c 
	$(CC) $(CFLAGS) -o $@ docmd.c

etherdev.o: port/etherdev.c 
	$(CC) $(CFLAGS) -o $@ port/etherdev.c

enetcore.o: enetcore.c 
	$(CC) $(CFLAGS) -o $@ enetcore.c

flash.o: flash.c
	$(CC) $(CFLAGS) -o $@ flash.c

flashdev.o: port/flashdev.c
	$(CC) $(CFLAGS) -o $@ port/flashdev.c

genlib.o: genlib.c 
	$(CC) $(CFLAGS) -o $@ genlib.c

mprintf.o: mprintf.c 
	$(CC) $(CFLAGS) -o $@ mprintf.c

cmds.o: cmds.c 
	$(CC) $(CFLAGS) -o $@ cmds.c

main.o: main.c 
	$(CC) $(CFLAGS) -o $@ main.c

phy.o: phy.c 
	$(CC) $(CFLAGS) -o $@ phy.c

tftp.o: tftp.c 
	$(CC) $(CFLAGS) -o $@ tftp.c

timestuff.o: timestuff.c 
	$(CC) $(CFLAGS) -o $@ timestuff.c

xmodem.o: xmodem.c 
	$(CC) $(CFLAGS) -o $@ xmodem.c

#####
#
# clean/clobber:
# Remove all files created by this make.
#
clean:
	rm -f *.o $(APPNAME) $(APPNAME).ezip $(APPNAME).sym
	rm -f $(APPNAME).dis $(APPNAME).bin symtbl

clobber: clean
	rm -f tags cscope.files cscope.out

#####
#
# sym:
# Create and download the symbol table file that can be used by uMon
# with this application...
#
sym: 
	@if ! test -f $(APPNAME).sym; then echo Must build $(APPNAME) first; exit 1; fi
	monsym -p0x $(APPNAME).sym >symtbl
	ttftp $(TARGET_IP) put symtbl


	
#####
#
# dld:
# Use the ttftp tool (supplied with MicroMonitor) to download the
# application to the target.
#
dld: 
	ttftp $(TARGET_IP) put $(APPNAME).bin $(APPRAMBASE)

#####
#
# cscope:
# Build the cscope file list and create a tags file...
#
cscope:
	find . -name '*.[chS]' >cscope.files
	ctags --file-tags=yes -n -L cscope.files
