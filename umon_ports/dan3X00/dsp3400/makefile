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
SHELL := /bin/dash
CPUTYPE	 = tensilica
TEN_CORE = dsp3400_newlib_RD1

#########################################################################
#
# Set default TARGET (UMON_TARGET_XT = 1) , DEBUG (0)
#
ifeq ($(TARGET),)
TARGET	= 1
endif

ifeq ($(DEBUG),)
DEBUG	= 0
endif



#########################################################################
#
# Include DAN3400 uMon common part make
#
include ../3400/3400.make


###############################################################################
#
# Build each variable from a list of individual filenames...
# (COMCSRC is defined in 3400.make)
LOCSSRC		= startup_entry.S
CPUSSRC		= 
CPUCSRC		= dis_ten.c cache_ten.c

#########################################################################
#
# Tensilica dependent rules
#

SECTIONS_CODE = -j .ResetVector.text -j .ResetHandler.text -j .WindowVectors.text -j .KernelExceptionVector.text -j .UserExceptionVector.text -j .DoubleExceptionVector.text -j .DebugExceptionVector.text -j .text 
SECTIONS_DATA = -j .rodata -j .lit4 -j .UserExceptionVector.literal -j .literal -j .data

#########################################################################
#
# Include Tensilica DAN3400 uMon common part make
#
include ../3400/ten3400.make



