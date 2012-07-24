OSTYPE	:= $(shell uname)

ifeq ($(OSTYPE),linux)
TNT=tnt
else
TNT=
endif

dirs=moncmd ttftp zlib utils newmon scripts maccrypt $(TNT)

ifdef VCC
MAKEARGS	= VCC=TRUE
else
MAKEARGS	=
endif

ifndef BIN
BIN			= bin
endif

all: oscheck
	@for dir in $(dirs) ; \
	do \
		cd src/$$dir; make $(MAKEARGS); cd ../..; \
	done

depend: oscheck
	@for dir in $(dirs) ; \
	do \
		cd src/$$dir; make $(MAKEARGS) depend; cd ../..; \
	done

clean: 
	@if ! test -d $(BIN) ; then mkdir $(BIN); fi
	@for dir in $(dirs) ; \
	do \
		cd src/$$dir; make $(MAKEARGS) clean; cd ../..; \
	done

oscheck:
	@if ! test -d $(BIN) ; then mkdir $(BIN); fi
ifeq ($(OSTYPE),)
	@printf "Set OSTYPE variable to cygwin, linux, macos or solaris\n"
	@printf "(depending on your environment).\n"
	exit 1
endif

# Note that 'clobber' will not remove $(BIN) simply because it
# may be pointing to some generic bin directory.
clobber:
	@if ! test -d $(BIN) ; then mkdir $(BIN); fi
	@for dir in $(dirs) ; \
	do \
		cd src/$$dir; make $(MAKEARGS) clean; cd ../..; \
	done
	@find bin -type f -exec rm {} \;

install: all
	@for dir in $(dirs) ; \
	do \
		cd src/$$dir; make $(MAKEARGS) install; cd ../..; \
	done

rebuild:	clobber all install 
	@echo Tools rebuilt for $(OSTYPE)
