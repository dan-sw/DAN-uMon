#
# To build this with a unique data table, issue a make with
# the following additional arguments...
#
# make DATATBL=-D UNIQUE_DATATBL -I datafile_directory_path
#
# where "datafile_directory_path" is the path to the directory that
# contains the file "unique_data_file.c" source file.
#
TOPDIR		= ..
TOOL		= maccrypt

include $(TOPDIR)/comsrc/common.make

ifneq ($(findstring Win,$(OS)),)
OBJS= main$(O_EXT) crypt$(O_EXT) getopt$(O_EXT)
else
OBJS= main$(O_EXT) crypt$(O_EXT)
endif

$(TOOL)$(E_EXT):	$(OBJS)
	$(CC) $(CFLAGS) -I$(COMSRC) $(COMSRC)/version.c
	$(LN) $(OBJS) version$(O_EXT) $(CONLIBS) $(SOCKLIBS)
	
SRCLIST=$(COMSRC)/getopt.c main.c

getopt$(O_EXT):	$(COMSRC)/getopt.c 
	$(CC) $(CFLAGS) -I$(COMSRC) $(COMSRC)/getopt.c

main$(O_EXT):	main.c
	$(CC) $(DATATBL) $(CFLAGS) -I$(COMSRC) main.c

crypt$(O_EXT):	crypt.c
	$(CC) $(CFLAGS) -I$(COMSRC) crypt.c

include $(TOPDIR)/comsrc/rules.make
