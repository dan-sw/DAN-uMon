BIN		= ../../bin
COMSRC		= ../comsrc
ZLIB		= ../zlib

ifdef VCC
CC		= cl
#LN		= cl -nologo -out:$@
LN		= link -nologo -out:$@
AR		= lib /OUT:$@ 
CONLIBS		= libc.lib oldnames.lib kernel32.lib ws2_32.lib \
			  mswsock.lib advapi32.lib
THREADLIBS	=
CFLAGS		= -c -W2 -DCRTAPI1=_cdecl -DCRTAPI2=_cdecl \
			  -nologo -D_X86_=1 -D_WINNT -D_WIN32_WINNT=0x0400 \
			  -D_WIN32_IE=0x0300 -DWINVER=0x0400 -DBUILD_WITH_VCC \
			  -I . -I $(COMSRC)
O_EXT		= .obj
E_EXT		= .exe
L_EXT		= .lib

else

CC			= gcc
ifeq (true,$(STATIC))
LN			= $(CC) -g -m32 --static -o $@
else
LN			= $(CC) -g -m32 -o $@
endif
AR			= ar ruv $(TOOL)$(L_EXT)
CONLIBS		=
THREADLIBS	= -lpthread
CFLAGS		= -O0 -g -m32 -fno-builtin -Wall -c -I $(COMSRC) -I .
O_EXT		= .o

ifneq ($(findstring Win,$(OS)),)
E_EXT		= .exe
else
E_EXT		= .bin
endif

L_EXT		= .a
endif

ifneq ($(findstring Win,$(OS)),)
SOCKLIBS	=
else
ifeq ($(OSTYPE),linux)
SOCKLIBS	= -lnsl
else
ifeq ($(OSTYPE),macos)
SOCKLIBS	= 
else
ifeq ($(OSTYPE),cygwin)
SOCKLIBS	= 
else
SOCKLIBS	= -lnsl
endif
endif
endif
endif
