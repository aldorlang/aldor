AFLAGS =			\
	-I $(INCDIR)		\
	-Y $(LIBDIR)		\
	-Y $(dir $@)		\
	-Q8 -Qinline-all	\
	-fao=$(basename $@).ao	\
	-ffm=$(basename $@).fm	\
	-fc=$(basename $@).c	\
	-fo=$(basename $@).o

%.c: %.ao
	@if test ! -f $@; then echo "missing $@"; exit 1; fi

%$(OBJEXT): %.ao
	@if test ! -f $@; then echo "missing $@"; exit 1; fi


define aldor-target
lib$1_ASOURCES := $(addprefix $(THIS), $(lib$1_ASOURCES))
lib$1_CSOURCES := $(addprefix $(THIS), $(lib$1_CSOURCES))

lib$1_AOBJECTS := $$(lib$1_ASOURCES:.as=.ao)
lib$1_COBJECTS := $$(lib$1_ASOURCES:.as=$(OBJEXT))

lib$1_HEADERS := $(addprefix $(INCDIR)/, $(lib$1_HEADERS))

# Generated C sources
lib$1-sources: $$(lib$1_ASOURCES:.as=.c)

# C library
$(LIBDIR)/lib$1$(LIBEXT): $$(lib$1_COBJECTS) $$(lib$1_CSOURCES:.c=$(OBJEXT))
	$(AR) cr $$@ $$^

# Local aldor build rule
$$(THIS)%.ao: $$(THIS)%.as $(BINDIR)/aldor$(EXEEXT) $(BINDIR)/unicl$(EXEEXT) $(aldor_HEADERS) $$(lib$1_HEADERS)
	$(BINDIR)/aldor$(EXEEXT) $$(AFLAGS) $(lib$1_AFLAGS) $$<
	$(AR) cr $(LIBDIR)/lib$1.al $$@

# Clean
clean: lib$1-clean
lib$1-clean:
	$(RM) $$(lib$1_AOBJECTS)
	$(RM) $$(lib$1_COBJECTS)
	$(RM) $$(lib$1_ASOURCES:.as=.c)
	$(RM) $$(lib$1_ASOURCES:.as=.fm)
	$(RM) $(LIBDIR)/lib$1$(LIBEXT) $(LIBDIR)/lib$1.al
endef
