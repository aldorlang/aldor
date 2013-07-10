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
$(THIS)%.c: $(THIS)%.as build/aldor build/include/aldor.conf build/include/foamlib.as
	build/aldor $(AFLAGS) $<
	$(AR) cr build/libfoamlib.al $(@:.c=.ao)

# Copy includes
build/include/foamlib.as: $(THIS)foamlib.as
	cp $< $@

# Clean
clean: clean-$(THIS)
clean-$(THIS):
	$(RM) $(libfoamlib_AOBJECTS)
	$(RM) $(libfoamlib_COBJECTS)
	$(RM) $(libfoamlib_ASOURCES:.as=.c)
	$(RM) build/libfoamlib.a build/libfoamlib.al

# Depend
$(THIS)array.c:		$(THIS)foamcat.c $(THIS)tuple.c $(THIS)sinteger.c $(THIS)parray.c $(THIS)
$(THIS)basic.c:		$(THIS)lang.c $(THIS)machine.c
$(THIS)boolean.c:	$(THIS)foamcat.c
$(THIS)char.c:		$(THIS)foamcat.c
$(THIS)file.c:		$(THIS)foamcat.c $(THIS)fname.c
$(THIS)fname.c:		$(THIS)foamcat.c $(THIS)parray.c $(THIS)string.c $(THIS)oslow.c
$(THIS)foamcat.c:	$(THIS)basic.c
$(THIS)format.c:	$(THIS)foamcat.c $(THIS)sinteger.c
$(THIS)gener.c:		$(THIS)foamcat.c
$(THIS)langx.c:		$(THIS)lv.c $(THIS)foamcat.c $(THIS)tuple.c $(THIS)sinteger.c $(THIS)list.c
$(THIS)list.c:		$(THIS)foamcat.c $(THIS)tuple.c $(THIS)segment.c $(THIS)sinteger.c $(THIS)pointer.c
$(THIS)lv.c:		$(THIS)basic.c
$(THIS)machine.c:	$(THIS)lang.c
$(THIS)opsys.c:		$(THIS)foamcat.c $(THIS)oslow.c $(THIS)file.c
$(THIS)oslow.c:		$(THIS)foamcat.c $(THIS)parray.c $(THIS)pointer.c $(THIS)string.c
$(THIS)parray.c:	$(THIS)foamcat.c $(THIS)sinteger.c
$(THIS)partial.c:	$(THIS)foamcat.c $(THIS)pointer.c
$(THIS)pointer.c:	$(THIS)foamcat.c
$(THIS)segment.c:	$(THIS)foamcat.c $(THIS)boolean.c
$(THIS)sfloat.c:	$(THIS)foamcat.c $(THIS)sinteger.c
$(THIS)sinteger.c:	$(THIS)foamcat.c $(THIS)segment.c
$(THIS)string.c:	$(THIS)foamcat.c $(THIS)char.c $(THIS)array.c
$(THIS)textwrit.c:	$(THIS)foamcat.c $(THIS)fname.c $(THIS)file.c $(THIS)array.c $(THIS)opsys.c
$(THIS)tuple.c:		$(THIS)foamcat.c
