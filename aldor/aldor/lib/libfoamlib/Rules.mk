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
libfoamlib_COBJECTS := $(libfoamlib_ASOURCES:.as=$(OBJEXT))


# C library
$(LIBDIR)/libfoamlib$(LIBEXT): $(libfoamlib_COBJECTS)
	$(AR) cr $@ $^

# Local aldor build rule
$(THIS)%$(OBJEXT): $(THIS)%.as $(BINDIR)/aldor$(EXEEXT) $(BINDIR)/unicl$(EXEEXT) $(aldor_HEADERS) $(INCDIR)/foamlib.as
	$(BINDIR)/aldor$(EXEEXT) $(AFLAGS) $<
	$(AR) cr $(LIBDIR)/libfoamlib.al $(@:$(OBJEXT)=.ao)

# Copy includes
$(INCDIR)/foamlib.as: $(THIS)foamlib.as
	cp $< $@

# Clean
clean: clean-libfoamlib
clean-libfoamlib:
	$(RM) $(libfoamlib_AOBJECTS)
	$(RM) $(libfoamlib_COBJECTS)
	$(RM) $(libfoamlib_ASOURCES:.as=.c)
	$(RM) $(LIBDIR)/libfoamlib$(LIBEXT) $(LIBDIR)/libfoamlib.al

# Depend
$(THIS)array$(OBJEXT):		$(THIS)foamcat$(OBJEXT) $(THIS)tuple$(OBJEXT) $(THIS)sinteger$(OBJEXT) $(THIS)parray$(OBJEXT) $(THIS)
$(THIS)basic$(OBJEXT):		$(THIS)lang$(OBJEXT) $(THIS)machine$(OBJEXT)
$(THIS)bool$(OBJEXT):		$(THIS)foamcat$(OBJEXT)
$(THIS)character$(OBJEXT):	$(THIS)foamcat$(OBJEXT)
$(THIS)file$(OBJEXT):		$(THIS)foamcat$(OBJEXT) $(THIS)fname$(OBJEXT)
$(THIS)fname$(OBJEXT):		$(THIS)foamcat$(OBJEXT) $(THIS)parray$(OBJEXT) $(THIS)string$(OBJEXT) $(THIS)oslow$(OBJEXT)
$(THIS)foamcat$(OBJEXT):	$(THIS)basic$(OBJEXT)
$(THIS)format$(OBJEXT):	$(THIS)foamcat$(OBJEXT) $(THIS)sinteger$(OBJEXT)
$(THIS)gener$(OBJEXT):		$(THIS)foamcat$(OBJEXT)
$(THIS)langx$(OBJEXT):		$(THIS)lv$(OBJEXT) $(THIS)foamcat$(OBJEXT) $(THIS)tuple$(OBJEXT) $(THIS)sinteger$(OBJEXT) $(THIS)list$(OBJEXT)
$(THIS)list$(OBJEXT):		$(THIS)foamcat$(OBJEXT) $(THIS)tuple$(OBJEXT) $(THIS)segment$(OBJEXT) $(THIS)sinteger$(OBJEXT) $(THIS)pointer$(OBJEXT)
$(THIS)lv$(OBJEXT):		$(THIS)basic$(OBJEXT)
$(THIS)machine$(OBJEXT):	$(THIS)lang$(OBJEXT)
$(THIS)opsys$(OBJEXT):		$(THIS)foamcat$(OBJEXT) $(THIS)oslow$(OBJEXT) $(THIS)file$(OBJEXT)
$(THIS)oslow$(OBJEXT):		$(THIS)foamcat$(OBJEXT) $(THIS)parray$(OBJEXT) $(THIS)pointer$(OBJEXT) $(THIS)string$(OBJEXT)
$(THIS)parray$(OBJEXT):	$(THIS)foamcat$(OBJEXT) $(THIS)sinteger$(OBJEXT)
$(THIS)partial$(OBJEXT):	$(THIS)foamcat$(OBJEXT) $(THIS)pointer$(OBJEXT)
$(THIS)pointer$(OBJEXT):	$(THIS)foamcat$(OBJEXT)
$(THIS)segment$(OBJEXT):	$(THIS)foamcat$(OBJEXT) $(THIS)bool$(OBJEXT)
$(THIS)sfloat$(OBJEXT):	$(THIS)foamcat$(OBJEXT) $(THIS)sinteger$(OBJEXT)
$(THIS)sinteger$(OBJEXT):	$(THIS)foamcat$(OBJEXT) $(THIS)segment$(OBJEXT)
$(THIS)string$(OBJEXT):	$(THIS)foamcat$(OBJEXT) $(THIS)character$(OBJEXT) $(THIS)array$(OBJEXT)
$(THIS)textwrit$(OBJEXT):	$(THIS)foamcat$(OBJEXT) $(THIS)fname$(OBJEXT) $(THIS)file$(OBJEXT) $(THIS)array$(OBJEXT) $(THIS)opsys$(OBJEXT)
$(THIS)tuple$(OBJEXT):		$(THIS)foamcat$(OBJEXT)
