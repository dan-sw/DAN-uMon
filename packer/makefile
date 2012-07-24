OSTYPE := $(shell uname)

ifeq ($(OSTYPE),Linux)
EXESUFFIX = bin
else
EXESUFFIX = exe
endif

default:
	gcc packer.c -lgcc -I../../ROM_bootloader/common -o packer.$(EXESUFFIX)

