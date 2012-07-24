# clean:
# Both *.o and *.obj files are removed regardless
# of the E_EXT and O_EXT variables...
#
clean:
	rm -f *.o *.obj *.a *.lib *.exe
	rm -f *.a *.lib depends
ifdef TOOLS	
	for tool in $(TOOLS) ; \
	do \
		rm -f $$tool; \
	done
else
	rm -f $(TOOL) *.exe
endif

$(BIN):
	mkdir $(BIN)

# install:
# Copy the final executable tool to $(BIN)
#
install: $(addsuffix $(E_EXT),$(TOOL)) $(BIN)
ifdef TOOLS	
	for tool in $(TOOLS) ; \
	do \
		cp $$tool $(BIN); \
	done
else
	cp $(TOOL)$(E_EXT) $(BIN)
endif

# depend:
# Generate a 'depends' file...
#
depend:
ifeq ($(O_EXT),.obj)
	@if [ ! -f depends ] ;\
	then \
		for file in $(SRCLIST) ; \
		do \
			echo $$file dependencies...; \
			gcc -MM -I$(COMSRC) $$file | sed "s/\.o/\.obj/" >>depends; \
		done \
	fi
else
	@if [ ! -f depends ] ;\
	then \
		for file in $(SRCLIST) ; \
		do \
			echo $$file dependencies...; \
			gcc -MM -I$(COMSRC) $$file >>depends; \
		done \
	fi
endif
