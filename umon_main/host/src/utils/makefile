TOPDIR		= ..

include $(TOPDIR)/comsrc/common.make

OBJS	= aout$(O_EXT) backtrace$(O_EXT) bedit$(O_EXT) bin2array$(O_EXT) \
		  bin2srec$(O_EXT) bswap$(O_EXT) chopline$(O_EXT) \
		  coff$(O_EXT) d2u$(O_EXT) \
		  defdate$(O_EXT) elf$(O_EXT) f2mem$(O_EXT) fcrc$(O_EXT) \
		  make2flist$(O_EXT) monsym$(O_EXT) mkupdate$(O_EXT) \
		  mkbin$(O_EXT) mkbmp$(O_EXT) mktfsimg$(O_EXT) fsize$(O_EXT) \
		  title$(O_EXT) tfs$(O_EXT) what$(O_EXT) vsub$(O_EXT) u2d$(O_EXT)

TOOLS	= $(OBJS:$(O_EXT)=$(E_EXT))
TOOL	= $(OBJS:$(O_EXT)=)
SRCLIST	= $(OBJS:$(O_EXT)=.c)

include $(TOPDIR)/comsrc/common.make

all:	depend $(TOOLS)

.c.o:
		$(CC) $(CFLAGS) -o $<

aout$(E_EXT):	aout$(O_EXT) utils$(O_EXT) 
		$(CC) $(CFLAGS) -o version$(O_EXT) ../comsrc/version.c
		$(LN) aout$(O_EXT) utils$(O_EXT) version$(O_EXT) 

backtrace$(E_EXT):	backtrace$(O_EXT) utils$(O_EXT)
		$(CC) $(CFLAGS) -o version$(O_EXT) ../comsrc/version.c
		$(LN) backtrace$(O_EXT) utils$(O_EXT) version$(O_EXT)
	
bedit$(E_EXT):	bedit$(O_EXT) utils$(O_EXT)
		$(CC) $(CFLAGS) -o version$(O_EXT) ../comsrc/version.c
		$(LN) bedit$(O_EXT) utils$(O_EXT) version$(O_EXT)
	
bin2array$(E_EXT):	bin2array$(O_EXT) utils$(O_EXT)
		$(CC) $(CFLAGS) -o version$(O_EXT) ../comsrc/version.c
		$(LN) bin2array$(O_EXT) utils$(O_EXT) version$(O_EXT)
	
bin2srec$(E_EXT):	bin2srec$(O_EXT) utils$(O_EXT)
		$(CC) $(CFLAGS) -o version$(O_EXT) ../comsrc/version.c
		$(LN) bin2srec$(O_EXT) utils$(O_EXT) version$(O_EXT)
	
bswap$(E_EXT):	bswap$(O_EXT) utils$(O_EXT)
		$(CC) $(CFLAGS) -o version$(O_EXT) ../comsrc/version.c
		$(LN) bswap$(O_EXT) utils$(O_EXT) version$(O_EXT)
	
chopline$(E_EXT):	chopline$(O_EXT) utils$(O_EXT)
		$(CC) $(CFLAGS) -o version$(O_EXT) ../comsrc/version.c
		$(LN) chopline$(O_EXT) utils$(O_EXT) version$(O_EXT)
	
coff$(E_EXT):	coff$(O_EXT) utils$(O_EXT) packdata$(O_EXT)
		$(CC) $(CFLAGS) -o version$(O_EXT) ../comsrc/version.c
		$(LN) coff$(O_EXT) utils$(O_EXT) packdata$(O_EXT) \
			version$(O_EXT) ../zlib/zlib$(L_EXT)

d2u$(E_EXT):	d2u$(O_EXT) utils$(O_EXT)
		$(CC) $(CFLAGS) -o version$(O_EXT) ../comsrc/version.c
		$(LN) d2u$(O_EXT) utils$(O_EXT) version$(O_EXT)
	
defdate$(E_EXT):	defdate$(O_EXT) utils$(O_EXT)
		$(CC) $(CFLAGS) -o version$(O_EXT) ../comsrc/version.c
		$(LN) defdate$(O_EXT) utils$(O_EXT) version$(O_EXT)
	
elf$(E_EXT):	elf$(O_EXT) utils$(O_EXT)
		$(CC) $(CFLAGS) -o version$(O_EXT) ../comsrc/version.c
		$(LN) elf$(O_EXT) utils$(O_EXT) version$(O_EXT) \
			../zlib/zlib$(L_EXT)
	
f2mem$(E_EXT):	f2mem$(O_EXT) utils$(O_EXT)
		$(CC) $(CFLAGS) -o version$(O_EXT) ../comsrc/version.c
		$(LN) f2mem$(O_EXT) utils$(O_EXT) version$(O_EXT)
	
fcrc$(E_EXT):	fcrc$(O_EXT) utils$(O_EXT)
		$(CC) $(CFLAGS) -o crc32$(O_EXT) ../comsrc/crc32.c
		$(CC) $(CFLAGS) -o version$(O_EXT) ../comsrc/version.c
		$(LN) fcrc$(O_EXT) crc32$(O_EXT) utils$(O_EXT) version$(O_EXT)
	
fsize$(E_EXT):	fsize$(O_EXT) utils$(O_EXT)
		$(CC) $(CFLAGS) -o version$(O_EXT) ../comsrc/version.c
		$(LN) fsize$(O_EXT) utils$(O_EXT) version$(O_EXT)
	
make2flist$(E_EXT):	make2flist$(O_EXT) utils$(O_EXT)
		$(CC) $(CFLAGS) -o version$(O_EXT) ../comsrc/version.c
		$(LN) make2flist$(O_EXT) utils$(O_EXT) version$(O_EXT)
	
mkbin$(E_EXT):	mkbin$(O_EXT) utils$(O_EXT)
		$(CC) $(CFLAGS) -o version$(O_EXT) ../comsrc/version.c
		$(LN) mkbin$(O_EXT) utils$(O_EXT) version$(O_EXT)
	
mkbmp$(E_EXT):	mkbmp$(O_EXT) utils$(O_EXT)
		$(CC) $(CFLAGS) -o version$(O_EXT) ../comsrc/version.c
		$(LN) mkbmp$(O_EXT) utils$(O_EXT) version$(O_EXT)
	
mkupdate$(E_EXT):	mkupdate$(O_EXT) utils$(O_EXT) ../ttftp/ttftp.c
		$(CC) $(CFLAGS) -o crc32$(O_EXT) ../comsrc/crc32.c
		$(CC) $(CFLAGS) -o version$(O_EXT) ../comsrc/version.c
		$(CC) $(CFLAGS) -I$(COMSRC) -I../ttftp ../ttftp/ttftp.c
		$(LN) mkupdate$(O_EXT) crc32$(O_EXT) utils$(O_EXT) \
			version$(O_EXT) ttftp$(O_EXT) $(CONLIBS) $(SOCKLIBS) \
			$(THREADLIBS)
	
mktfsimg$(E_EXT):	mktfsimg$(O_EXT) utils$(O_EXT)
		$(CC) $(CFLAGS) -o version$(O_EXT) ../comsrc/version.c
		$(LN) mktfsimg$(O_EXT) utils$(O_EXT) version$(O_EXT)
	
monsym$(E_EXT):	monsym$(O_EXT) utils$(O_EXT)
		$(CC) $(CFLAGS) -o version$(O_EXT) ../comsrc/version.c
		$(LN) monsym$(O_EXT) utils$(O_EXT) version$(O_EXT)
	
tfs$(E_EXT):	tfs$(O_EXT) utils$(O_EXT) 
		$(CC) $(CFLAGS) -o version$(O_EXT) ../comsrc/version.c
		$(LN) tfs$(O_EXT) utils$(O_EXT) version$(O_EXT) 

title$(E_EXT):	title$(O_EXT) utils$(O_EXT)
		$(LN) title$(O_EXT) 
	
vsub$(E_EXT):	vsub$(O_EXT) utils$(O_EXT)
		$(CC) $(CFLAGS) -o version$(O_EXT) ../comsrc/version.c
		$(LN) vsub$(O_EXT) utils$(O_EXT) version$(O_EXT)

what$(E_EXT):	what$(O_EXT) utils$(O_EXT)
		$(CC) $(CFLAGS) -o version$(O_EXT) ../comsrc/version.c
		$(LN) what$(O_EXT) utils$(O_EXT) version$(O_EXT)

u2d$(E_EXT):	u2d$(O_EXT) utils$(O_EXT)
		$(CC) $(CFLAGS) -o version$(O_EXT) ../comsrc/version.c
		$(LN) u2d$(O_EXT) utils$(O_EXT) version$(O_EXT)

-include depends

$(OBJS):
	$(CC) $(CFLAGS) -I$(COMSRC) -o $@ $(@:$(O_EXT)=.c)

utils$(O_EXT):
	$(CC) $(CFLAGS) -I$(COMSRC) -o $@ $(@:$(O_EXT)=.c)

packdata$(O_EXT):
	$(CC) $(CFLAGS) -I$(COMSRC) -o $@ $(@:$(O_EXT)=.c)

include $(TOPDIR)/comsrc/rules.make
