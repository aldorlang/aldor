THIS := $(dir $(lastword $(MAKEFILE_LIST)))

libfoamlib_ASOURCES :=	\
	array.as	\
	basic.as	\
	boolean.as	\
	char.as	\
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
build/libfoamlib.a: $(libfoamlib_COBJECTS)
	$(AR) cr $@ $^

# Local aldor build rule
$(THIS)%.o: $(THIS)%.as build/aldor build/unicl $(aldor_HEADERS) build/include/foamlib.as
	build/aldor $(AFLAGS) $<
	$(AR) cr build/libfoamlib.al $(@:.o=.ao)
	mv $(notdir $@) $@

# Copy includes
build/include/foamlib.as: $(THIS)foamlib.as
	mkdir -p $(dir $@)
	cp $< $@

# Clean
clean: clean-$(THIS)
clean-$(THIS):
	$(RM) $(libfoamlib_AOBJECTS)
	$(RM) $(libfoamlib_COBJECTS)
	$(RM) $(libfoamlib_ASOURCES:.as=.c)
	$(RM) build/libfoamlib.a build/libfoamlib.al

# Depend
$(THIS)array.o:		$(THIS)foamcat.o $(THIS)tuple.o $(THIS)sinteger.o $(THIS)parray.o $(THIS)
$(THIS)basic.o:		$(THIS)lang.o $(THIS)machine.o
$(THIS)boolean.o:	$(THIS)foamcat.o
$(THIS)char.o:		$(THIS)foamcat.o
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
$(THIS)segment.o:	$(THIS)foamcat.o $(THIS)boolean.o
$(THIS)sfloat.o:	$(THIS)foamcat.o $(THIS)sinteger.o
$(THIS)sinteger.o:	$(THIS)foamcat.o $(THIS)segment.o
$(THIS)string.o:	$(THIS)foamcat.o $(THIS)char.o $(THIS)array.o
$(THIS)textwrit.o:	$(THIS)foamcat.o $(THIS)fname.o $(THIS)file.o $(THIS)array.o $(THIS)opsys.o
$(THIS)tuple.o:		$(THIS)foamcat.o
