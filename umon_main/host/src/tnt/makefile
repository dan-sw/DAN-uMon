TOPDIR		= ..
TOOL		= tnt

include $(TOPDIR)/comsrc/common.make

# tnt makefile:
# This tool has only been tested on Linux.
# If you want something similar to this but for Windows, use uCon.
#
# Although it is untested on Solaris, it can be built if -lsockets is
# added to the ld line.
#
CFLAGS	= -Wall -c
OBJS	= tnt.o telnetd.o utils.o

tnt.bin:	$(OBJS)
	gcc --static -o tnt $(OBJS) -lpthread

tnt.o: tnt.c tnt.h
	gcc $(CFLAGS) -o tnt.o tnt.c

telnetd.o: telnetd.c telnetd.h
	gcc $(CFLAGS) -o telnetd.o telnetd.c

utils.o: utils.c utils.h
	gcc $(CFLAGS) -o utils.o utils.c

include $(TOPDIR)/comsrc/rules.make
