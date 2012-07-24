TOPDIR		= ..
TOOL		= newmon

include $(TOPDIR)/comsrc/common.make

ifneq ($(findstring Win,$(OS)),)
OBJS= newmon$(O_EXT) crc32$(O_EXT) moncmd$(O_EXT) ttftp$(O_EXT) getopt$(O_EXT)
else
OBJS= newmon$(O_EXT) crc32$(O_EXT) moncmd$(O_EXT) ttftp$(O_EXT)
endif

$(TOOL)$(E_EXT):	$(OBJS)
	$(CC) $(CFLAGS) -I$(COMSRC) $(COMSRC)/version.c
	$(LN) $(OBJS) version$(O_EXT) $(CONLIBS) $(SOCKLIBS) $(THREADLIBS)
	
SRCLIST=$(COMSRC)/getopt.c main.c

crc32$(O_EXT):	../comsrc/crc32.c
	$(CC) $(CFLAGS) -I$(COMSRC) ../comsrc/crc32.c

getopt$(O_EXT):	$(COMSRC)/getopt.c 
	$(CC) $(CFLAGS) -I$(COMSRC) $(COMSRC)/getopt.c

moncmd$(O_EXT):	../moncmd/moncmd.c
	$(CC) $(CFLAGS) -I$(COMSRC) -I../moncmd ../moncmd/moncmd.c

ttftp$(O_EXT):	../ttftp/ttftp.c
	$(CC) $(CFLAGS) -I$(COMSRC) -I../ttftp ../ttftp/ttftp.c

newmon$(O_EXT):	newmon.c
	$(CC) $(CFLAGS) -I$(COMSRC) -I ../ttftp -I ../moncmd newmon.c

include $(TOPDIR)/comsrc/rules.make
