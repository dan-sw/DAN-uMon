TOPDIR		= ..
TOOL		= ttftp

include $(TOPDIR)/comsrc/common.make

ifdef VCC
SRC		= main$(O_EXT) ttftp$(O_EXT) tftpsrvr$(O_EXT) tftptest$(O_EXT)
OBJS	= $(SRC) getopt$(O_EXT)
SRCLIST	= $(SRC:$(O_EXT)=.c) $(COMSRC)/getopt.c
else
OBJS	= main$(O_EXT) ttftp$(O_EXT) tftpsrvr$(O_EXT) tftptest$(O_EXT)
SRCLIST	= $(OBJS:$(O_EXT)=.c)
endif


$(TOOL)$(E_EXT):	$(OBJS)
	$(CC) $(CFLAGS) -I$(COMSRC) $(COMSRC)/version.c
	$(LN) $(OBJS) version$(O_EXT) $(SOCKLIBS) $(CONLIBS) $(THREADLIBS)

getopt$(O_EXT):	$(COMSRC)/getopt.c
	$(CC) $(CFLAGS) -I$(COMSRC) $(COMSRC)/getopt.c

main$(O_EXT):	main.c ttftp.h
	$(CC) $(CFLAGS) -I$(COMSRC) main.c

ttftp$(O_EXT):	ttftp.c ttftp.h
	$(CC) $(CFLAGS) -I$(COMSRC) ttftp.c

tftpsrvr$(O_EXT):	tftpsrvr.c ttftp.h
	$(CC) $(CFLAGS) -I$(COMSRC) tftpsrvr.c

tftptest$(O_EXT):	tftptest.c ttftp.h
	$(CC) $(CFLAGS) -I$(COMSRC) tftptest.c

include $(TOPDIR)/comsrc/rules.make
