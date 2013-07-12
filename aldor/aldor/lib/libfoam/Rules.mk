THIS := $(dir $(lastword $(MAKEFILE_LIST)))

libfoam_ASOURCES :=	\
	runtime.as

libfoam_ASOURCES := $(addprefix $(THIS), $(libfoam_ASOURCES))

libfoam_AOBJECTS := $(libfoam_ASOURCES:.as=.ao)
libfoam_COBJECTS := $(libfoam_ASOURCES:.as=.o)


# C library
$(LIBDIR)/libfoam.a: $(libfoam_COBJECTS) $(libruntime_OBJECTS)
	$(AR) cr $@ $^

# Local aldor build rule
$(THIS)%.o: $(THIS)%.as $(BINDIR)/aldor $(BINDIR)/unicl $(aldor_HEADERS)
	$(BINDIR)/aldor $(AFLAGS) -Q3 -Wruntime $<
	$(AR) cr $(LIBDIR)/libfoam.al $(@:.o=.ao)
	mv $(notdir $@) $@

# Clean
clean: clean-$(THIS)
clean-$(THIS):
	$(RM) $(libfoam_AOBJECTS)
	$(RM) $(libfoam_COBJECTS)
	$(RM) $(libfoam_ASOURCES:.as=.c)
	$(RM) $(LIBDIR)/libfoam.a $(LIBDIR)/libfoam.al

# Depend
$(THIS)runtime.o:	$(LIBDIR)/libfoamlib.a
