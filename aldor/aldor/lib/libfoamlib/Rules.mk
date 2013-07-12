THIS := $(dir $(lastword $(MAKEFILE_LIST)))

libfoamlib_ASOURCES :=	\
	array.as	\
	basic.as	\
	bool.as	\
	character.as	\
	file.as	\
	fname.as	\
	foamcat.as	\
	foamlib.as	\
	format.as	\
	gener.as	\
	lang.as	\
	langx.as	\
	list.as	\
	machine.as	\
	opsys.as	\
	oslow.as	\
	parray.as	\
	partial.as	\
	pointer.as	\
	segment.as	\
	sfloat.as	\
	sinteger.as	\
	string.as	\
	textwrit.as	\
	tuple.as

libfoamlib_ASOURCES := $(addprefix $(THIS), $(libfoamlib_ASOURCES))

libfoamlib_AOBJECTS := $(libfoamlib_ASOURCES:.as=.ao)
libfoamlib_COBJECTS := $(libfoamlib_ASOURCES:.as=.o)


# C library
$(LIBDIR)/libfoamlib.a: $(libfoamlib_COBJECTS)
	mkdir -p $(dir $@)
	$(AR) cr $@ $^

# Local aldor build rule
$(THIS)%.o: $(THIS)%.as $(BINDIR)/aldor $(BINDIR)/unicl $(aldor_HEADERS) $(INCDIR)/foamlib.as
	$(BINDIR)/aldor $(AFLAGS) $<
	$(AR) cr $(LIBDIR)/libfoamlib.al $(@:.o=.ao)

# Copy includes
$(INCDIR)/foamlib.as: $(THIS)foamlib.as
	mkdir -p $(dir $@)
	cp $< $@

# Clean
clean: clean-libfoamlib
clean-libfoamlib:
	$(RM) $(libfoamlib_AOBJECTS)
	$(RM) $(libfoamlib_COBJECTS)
	$(RM) $(libfoamlib_ASOURCES:.as=.c)
	$(RM) $(LIBDIR)/libfoamlib.a $(LIBDIR)/libfoamlib.al

# Depend
$(THIS)array.o:		$(THIS)foamcat.o $(THIS)tuple.o $(THIS)sinteger.o $(THIS)parray.o $(THIS)
$(THIS)basic.o:		$(THIS)lang.o $(THIS)machine.o
$(THIS)bool.o:		$(THIS)foamcat.o
$(THIS)character.o:	$(THIS)foamcat.o
$(THIS)file.o:		$(THIS)foamcat.o $(THIS)fname.o
$(THIS)fname.o:		$(THIS)foamcat.o $(THIS)parray.o $(THIS)string.o $(THIS)oslow.o
$(THIS)foamcat.o:	$(THIS)basic.o
$(THIS)format.o:	$(THIS)foamcat.o $(THIS)sinteger.o
$(THIS)gener.o:		$(THIS)foamcat.o
$(THIS)langx.o:		$(THIS)lv.o $(THIS)foamcat.o $(THIS)tuple.o $(THIS)sinteger.o $(THIS)list.o
$(THIS)list.o:		$(THIS)foamcat.o $(THIS)tuple.o $(THIS)segment.o $(THIS)sinteger.o $(THIS)pointer.o
$(THIS)lv.o:		$(THIS)basic.o
$(THIS)machine.o:	$(THIS)lang.o
$(THIS)opsys.o:		$(THIS)foamcat.o $(THIS)oslow.o $(THIS)file.o
$(THIS)oslow.o:		$(THIS)foamcat.o $(THIS)parray.o $(THIS)pointer.o $(THIS)string.o
$(THIS)parray.o:	$(THIS)foamcat.o $(THIS)sinteger.o
$(THIS)partial.o:	$(THIS)foamcat.o $(THIS)pointer.o
$(THIS)pointer.o:	$(THIS)foamcat.o
$(THIS)segment.o:	$(THIS)foamcat.o $(THIS)bool.o
$(THIS)sfloat.o:	$(THIS)foamcat.o $(THIS)sinteger.o
$(THIS)sinteger.o:	$(THIS)foamcat.o $(THIS)segment.o
$(THIS)string.o:	$(THIS)foamcat.o $(THIS)character.o $(THIS)array.o
$(THIS)textwrit.o:	$(THIS)foamcat.o $(THIS)fname.o $(THIS)file.o $(THIS)array.o $(THIS)opsys.o
$(THIS)tuple.o:		$(THIS)foamcat.o
