##########################################################################
#
# Basic, target/architecture independent makefile for building the
# applications discussed in Chapter 9 of the MicroMonitor User's Manual.
# To deal with the target/cpu-architecture specific information needed
# by the application, the following site dependent data must be provided
# either on the make command line or in the variable initializations
# below...
#
# Adjust these values based on your system configuration.  
# ARCH:
# 	Set ARCH to one of the accepted CPU architectures (i.e. MIPS
#	PPC, ARM, BLACKFIN, COLDFIRE, MICROBLAZE).
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

# With the above values specified, walking through chapter 9 of the
# MicroMonitor User Manual simply requires a "make clean", "make appX"
# and "make appXdld" (where 'X' depends on the application example you
# are working on)

##########################################################################
#
# There should be no need to change anything below this point if
# building for MIPS, ColdFire, PPC or ARM...
# 
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

ifeq ($(ARCH),MIPS)
TOOL_PREFIX	:= mips-elf
CFLAGS		:= -fno-builtin -G 0 -march=r4600 -mips3 -mno-abicalls \
			  -fno-pic -c -g -O2 -Wall -EB -I . 
CRT0		:= crt0_mips.o
CPU			:= -D CPU_IS_MIPS=1
START		:= start
endif

ifeq ($(ARCH),PPC)
TOOL_PREFIX	:= ppc-elf
CFLAGS		:= -fno-builtin -mno-sdata -msoft-float \
			   -c -Wall -O -g -I. 
CRT0		:= crt0_ppc.o
CPU			:= -D CPU_IS_PPC=1
START		:= start
endif

ifeq ($(ARCH),ARM)
TOOL_PREFIX	:= arm-elf
CFLAGS		:= -fno-builtin -mcpu=arm9tdmi \
			   -c -Wall -O -g -I.
CRT0		:= crt0_arm.o
CPU			:= -D CPU_IS_ARM=1
START		:= start
endif

ifeq ($(ARCH),BLACKFIN)
TOOL_PREFIX	:= bfin-elf
CFLAGS		:= -fno-builtin -mcsync-anomaly -c -Wall -O -g -I.
CRT0		:= crt0_bfin.o
CPU			:= -D CPU_IS_BFIN=1
START		:= _start
endif

ifeq ($(ARCH),MICROBLAZE)
TOOL_PREFIX	:= C:/EDK/gnu/microblaze/nt/bin/mb
LIBPATH		:= -L C:/xilinx/els_stuff/projects/avnet_spartan3_devkit/microblaze_0/lib
CFLAGS		:= -fno-builtin -mno-xl-soft-mul -c -Wall -O -g -I.
CRT0		:= crt0_mb.o
CPU			:= -D CPU_IS_MICROBLAZE=1
START		:= start
endif

ifeq ($(ARCH),COLDFIRE)
TOOL_PREFIX	:= m68k-elf
CFLAGS		:= -Wall -fno-builtin -msoft-float -m5200 -g -c -I.
CRT0		:= crt0_cf.o
CPU			:= -D CPU_IS_68K=1
START		:= start
#LIBGCC		= `$(CC) -m5200 --print-libgcc-file-name`
LIBGCC		= /usr/lib/gcc-lib/m68k-elf/3.2/m5200/libgcc.a -L /usr/m68k-elf/lib/m5200
endif

OBJS1=main1.o monlib.o
OBJS2=$(CRT0) main2.o monlib.o
OBJS3=$(CRT0) main3.o monlib.o
OBJS4=$(CRT0) main4.o monlib.o
OBJS5=$(CRT0) main5.o monlib.o


#####
#
# all and alldld:
# Make and download all examples in one shot...
#
all:	app1 app2 app3 app4 app5
	@echo All applications built.

alldld: app1dld app2dld app3dld app4dld app5dld
	@echo All applications downloaded.

#####
#
# appX:
# Rules for each of the examples.
#
app1: varcheck $(OBJS1) makefile
	$(LD) -e $(START) -o app1 -Ttext $(APPRAMBASE) $(OBJS1) -lc $(LIBGCC) 
	$(NM) --numeric-sort app1 >app1.sym
	$(OBJDUMP) --source --disassemble app1 > app1.dis

app1dld: varcheck
	@ttftp $(TARGET_IP) put app1 app1,E

app2: varcheck $(OBJS2) makefile
	$(LD) -e $(START) -o app2 -Ttext $(APPRAMBASE) $(OBJS2) -lc $(LIBGCC) 
	$(NM) --numeric-sort app2 >app2.sym
	$(OBJDUMP) --source --disassemble app2 > app2.dis

app2dld: varcheck
	@ttftp $(TARGET_IP) put app2 app2,E

app3: varcheck $(OBJS3) makefile
	$(LD) -e $(START) -o app3 -Ttext $(APPRAMBASE) $(OBJS3) -lc $(LIBGCC) 
	$(NM) --numeric-sort app3 >app3.sym
	$(OBJDUMP) --source --disassemble app3 > app3.dis

app3dld: varcheck
	@ttftp $(TARGET_IP) put app3 app3,E

app4: varcheck $(OBJS4) makefile
	$(LD) -e $(START) -o app4 -Ttext $(APPRAMBASE) $(OBJS4) -lc $(LIBGCC) 
	$(NM) --numeric-sort app4 >app4.sym
	$(OBJDUMP) --source --disassemble app4 > app4.dis

app4dld: varcheck
	@ttftp $(TARGET_IP) put app4 app4,E

app5: varcheck $(OBJS5) makefile
	$(LD) -e $(START) -o app5 -Ttext $(APPRAMBASE) $(OBJS5) -lc $(LIBGCC) 
	$(NM) --numeric-sort app5 >app5.sym
	$(OBJDUMP) --source --disassemble app5 > app5.dis

app5dld: varcheck
	@ttftp $(TARGET_IP) put app5 app5,E



#####
#
# Variable checks:
# Verify that the necessary variables have been set on the make
# command line.
#
varcheck:
ifndef ARCH
	@echo Must specify ARCH=XXX on command line.
	@echo ARCH can be MIPS, PPC, ARM or COLDFIRE
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
# Miscellaneous rules:
#
clean:
	rm -f *.o app[1-5] app[1-5].dis app[1-5].sym symtbl


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

crt0_mb.o: crt0_mb.S
	$(CC) $(CFLAGS) -o $@ crt0_mb.S

crt0_mips.o: crt0_mips.S
	$(CC) $(CFLAGS) -o $@ crt0_mips.S

crt0_ppc.o: crt0_ppc.S
	$(CC) $(CFLAGS) -o $@ crt0_ppc.S

main1.o: main1.c 
	$(CC) $(CFLAGS) -D MONCOMPTR=$(MONCOMPTR) -o $@ main1.c

main2.o: main2.c 
	$(CC) $(CFLAGS) -D MONCOMPTR=$(MONCOMPTR) -o $@ main2.c

main3.o: main3.c 
	$(CC) $(CFLAGS) -D MONCOMPTR=$(MONCOMPTR) -o $@ main3.c

main4.o: main4.c 
	$(CC) $(CFLAGS) -D MONCOMPTR=$(MONCOMPTR) -o $@ main4.c

main5.o: main5.c 
	$(CC) $(CFLAGS) $(CPU) -D MONCOMPTR=$(MONCOMPTR) -o $@ main5.c

monlib.o: monlib.c
	$(CC) $(CFLAGS) -o $@ monlib.c
