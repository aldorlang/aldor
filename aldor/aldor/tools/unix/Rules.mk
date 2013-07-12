THIS := $(dir $(lastword $(MAKEFILE_LIST)))

#############################################################################
# :: YACC preprocessor
#############################################################################

zacc_SOURCES =	\
	zaccscan.c	\
	zaccgram.c	\
	zacc.c	\
	cenum.c

zacc_OBJECTS := $(addprefix $(THIS), $(zacc_SOURCES:.c=.o))
zacc_BUILT_SOURCES := $(addprefix $(THIS), zaccgram.c zaccgram.h zaccscan.c)

$(BINDIR)/zacc: $(zacc_OBJECTS)
	mkdir -p $(dir $@)
	$(LINK.c) $+ -o $@

%.y: %.z $(BINDIR)/zacc
	$(BINDIR)/zacc -y $@ -p $<

%.c: %.y
	$(YACC) $< -o $@

%.c: %.l
	$(LEX) -o$@ $<


clean: clean-zacc
clean-zacc:
	$(RM) $(zacc_OBJECTS)
	$(RM) $(zacc_BUILT_SOURCES)
	$(RM) $(BINDIR)/zacc

# Depend
$(THIS)zaccgram.h: $(THIS)zaccgram.c
$(THIS)zaccgram.c: $(THIS)zaccgram.y
	$(YACC) -d -o $@ $<

$(THIS)zaccscan.o: $(THIS)zaccgram.h



#############################################################################
# :: Message catalog generator
#############################################################################

msgcat_SOURCES =	\
	msgcat.c

msgcat_OBJECTS := $(addprefix $(THIS), $(msgcat_SOURCES:.c=.o))

$(BINDIR)/msgcat: $(msgcat_OBJECTS)
	mkdir -p $(dir $@)
	$(LINK.c) $^ -o $@

%.c %.h: %.msg $(BINDIR)/msgcat
	cd $(dir $@); $(CURDIR)/$(BINDIR)/msgcat -h -c $(notdir $*)

clean: clean-msgcat
clean-msgcat:
	$(RM) $(msgcat_OBJECTS)
	$(RM) $(msgcat_BUILT_SOURCES)
	$(RM) $(BINDIR)/msgcat
