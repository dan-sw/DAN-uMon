SHELL := /bin/dash

ifeq ($(BUILD_ID),)
	BUILD_ID = "0"
endif

OSTYPE := $(shell uname)

ifeq ($(OSTYPE),Linux)
EXESUFFIX = bin
J_FACTOR = -j32
else
EXESUFFIX = exe
J_FACTOR =
endif

ifeq ($(DDR),)
DDR = 800
endif

default:	all
all:		arm dsp cpu umon tfs


cb_only:	umon_cb_only tfs_cb_only	

tfs_win:
	@umon_main/host/src/utils/mktfsimg -s -b 0x3000000 -e 0x010000 0x020000 tfs.list tfs_win.bin

tfs: umon
	@umon_main/host/src/utils/mktfsimg.$(EXESUFFIX) -s -b 0x00060000 0x020000 tfs.list tfs.bin
	@echo Building final uMon image: boot_flash.hex.bin
	@cat umon.hex.bin tfs.bin > boot_flash.hex.bin
	@sha1sum -b umon_ports/dan3X00/cpu3400/Bin_tensilica/umon_piggy.bin > boot_secure_sha1.txt
	@echo "======== COMPLETE ========" 

tfs_cb_only:
	@umon_main/host/src/utils/mktfsimg.$(EXESUFFIX) -s -b 0x00060000 -e 0x010000 0x020000 tfs_cb_only.list tfs_cb_only.bin
	@cat umon.hex.bin tfs_cb_only.bin > boot_flash.hex.bin
	@sha1sum -b umon_ports/dan3X00/cpu3400/Bin_tensilica/umon_piggy.bin > boot_secure_sha1.txt

umon_cb_only:	arm_cb_only dsp cpu packer/packer.$(EXESUFFIX)
	@packer/packer.$(EXESUFFIX) umon_ports/dan3X00/arm3400/Bin_arm/umon_piggy.bin umon_ports/dan3X00/dsp3400/Bin_tensilica/umon_piggy.bin umon_ports/dan3X00/cpu3400/Bin_tensilica/umon_piggy.bin > umon.data
	@cat umon.data | grep -v @ | sed 's/ //g' > umon.hex
	@perl hex2bin.pl umon.hex

umon:	arm dsp cpu packer/packer.$(EXESUFFIX)
	@packer/packer.$(EXESUFFIX) umon_ports/dan3X00/arm3400/Bin_arm/umon_piggy.bin umon_ports/dan3X00/dsp3400/Bin_tensilica/umon_piggy.bin umon_ports/dan3X00/cpu3400/Bin_tensilica/umon_piggy.bin > umon.data
	@cat umon.data | grep -v @ | sed 's/ //g' > umon.hex
	@perl hex2bin.pl umon.hex

arm_cb_only:
	@cd umon_ports/dan3X00/arm3400; make $(J_FACTOR) BUILD_ID="$(BUILD_ID)" arm_cb_only; cd ../..

arm:
	@cd umon_ports/dan3X00/arm3400; make $(J_FACTOR) BUILD_ID="$(BUILD_ID)" DDR=$(DDR); cd ../..

dsp:
	@cd umon_ports/dan3X00/dsp3400; make $(J_FACTOR) BUILD_ID="$(BUILD_ID)" DDR=$(DDR); cd ../..

cpu:
	@cd umon_ports/dan3X00/cpu3400; make $(J_FACTOR) BUILD_ID="$(BUILD_ID)" DDR=$(DDR); cd ../..

arm_clean:
	@cd umon_ports/dan3X00/arm3400; make BUILD_ID="$(BUILD_ID)" clean; cd ../..

dsp_clean:
	@cd umon_ports/dan3X00/dsp3400; make BUILD_ID="$(BUILD_ID)" clean; cd ../..

cpu_clean:
	@cd umon_ports/dan3X00/cpu3400; make BUILD_ID="$(BUILD_ID)" clean; cd ../..

clean:	arm_clean dsp_clean cpu_clean
	@rm -f umon.data umon.hex umon.hex.bin tfs.bin boot_secure_sha1.txt

packer/packer.$(EXESUFFIX):
	@echo Compiling uMon image packer...
	@make -C packer
