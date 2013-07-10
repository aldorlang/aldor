THIS := $(dir $(lastword $(MAKEFILE_LIST)))

zacc_SOURCES =	\
	zaccscan.c	\
	zaccgram.c	\
	zacc.c	\
	cenum.c

zacc_OBJECTS := $(addprefix $(THIS), $(zacc_SOURCES:.c=.o))
zacc_BUILT_SOURCES := $(addprefix $(THIS), zaccgram.c zaccgram.h zaccscan.c)

build/zacc: $(zacc_OBJECTS)
	mkdir -p $(dir $@)
	$(LINK.c) $+ -o $@

%.y: %.z build/zacc
	build/zacc -y $@ -p $<

%.c: %.y
	$(YACC) $< -o $@

%.c: %.l
	$(LEX) -o$@ $<


clean: clean-$(THIS)
clean-$(THIS):
	$(RM) $(zacc_OBJECTS)
	$(RM) $(zacc_BUILT_SOURCES)
	$(RM) build/zacc

# Depend
$(THIS)zaccgram.h: $(THIS)zaccgram.c
$(THIS)zaccgram.c: $(THIS)zaccgram.y
	$(YACC) -d -o $@ $<

$(THIS)zaccscan.o: $(THIS)zaccgram.h
