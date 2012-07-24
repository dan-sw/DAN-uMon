TOPDIR		= ..
TOOL		= zlib


include $(TOPDIR)/comsrc/common.make

OBJS	= adler32$(O_EXT) compress$(O_EXT) crc32$(O_EXT) gzio$(O_EXT) \
		  uncompr$(O_EXT) deflate$(O_EXT) trees$(O_EXT) zutil$(O_EXT) \
		  inflate$(O_EXT) infblock$(O_EXT) inftrees$(O_EXT) infcodes$(O_EXT) \
		  infutil$(O_EXT) inffast$(O_EXT)

SRCLIST	= $(OBJS:$(O_EXT)=.c)

all:	depend $(TOOL)$(L_EXT)

$(TOOL)$(L_EXT):	$(OBJS) 
	$(AR) $(OBJS)

$(TOOL)$(E_EXT):
	>$(TOOL)$(E_EXT)

example$(E_EXT): zlib$(L_EXT) example$(O_EXT) 
	$(LN) example$(O_EXT) -o example$(E_EXT) zlib$(L_EXT)

minigzip$(E_EXT): zlib$(L_EXT) minigzip$(O_EXT)
	$(LN) minigzip$(O_EXT) -o minigzip$(E_EXT) zlib$(L_EXT)

-include depends

$(OBJS): 
	$(CC) $(CFLAGS) -o $@ $(@:$(O_EXT)=.c)

include $(TOPDIR)/comsrc/rules.make
