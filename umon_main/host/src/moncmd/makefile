TOPDIR		= ..
TOOL		= moncmd

include $(TOPDIR)/comsrc/common.make

ifneq ($(findstring Win,$(OS)),)
OBJS= main$(O_EXT) moncmd$(O_EXT) getopt$(O_EXT)
else
OBJS= main$(O_EXT) moncmd$(O_EXT)
endif

$(TOOL)$(E_EXT):	$(OBJS)
	$(CC) $(CFLAGS) -I$(COMSRC) $(COMSRC)/version.c
	$(LN) $(OBJS) version$(O_EXT) $(CONLIBS) $(SOCKLIBS) $(THREADLIBS)
	
SRCLIST=$(COMSRC)/getopt.c main.c

getopt$(O_EXT):	$(COMSRC)/getopt.c 
	$(CC) $(CFLAGS) -I$(COMSRC) $(COMSRC)/getopt.c

main$(O_EXT):	main.c  moncmd.h 
	$(CC) $(CFLAGS) -I$(COMSRC) main.c

moncmd$(O_EXT):	moncmd.c  moncmd.h
	$(CC) $(CFLAGS) -I$(COMSRC) moncmd.c

include $(TOPDIR)/comsrc/rules.make
