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

libfoamlib_HEADERS := foamlib.as

$(eval $(call aldor-target,foamlib))

# Copy includes
$(INCDIR)/foamlib.as: $(THIS)foamlib.as
	cp $< $@

# Depend
$(THIS)array.ao:	$(THIS)foamcat.ao $(THIS)tuple.ao $(THIS)sinteger.ao $(THIS)parray.ao $(THIS)
$(THIS)basic.ao:	$(THIS)lang.ao $(THIS)machine.ao
$(THIS)bool.ao:		$(THIS)foamcat.ao
$(THIS)character.ao:	$(THIS)foamcat.ao
$(THIS)file.ao:		$(THIS)foamcat.ao $(THIS)fname.ao
$(THIS)fname.ao:	$(THIS)foamcat.ao $(THIS)parray.ao $(THIS)string.ao $(THIS)oslow.ao
$(THIS)foamcat.ao:	$(THIS)basic.ao
$(THIS)format.ao:	$(THIS)foamcat.ao $(THIS)sinteger.ao
$(THIS)gener.ao:	$(THIS)foamcat.ao
$(THIS)langx.ao:	$(THIS)lv.ao $(THIS)foamcat.ao $(THIS)tuple.ao $(THIS)sinteger.ao $(THIS)list.ao
$(THIS)list.ao:		$(THIS)foamcat.ao $(THIS)tuple.ao $(THIS)segment.ao $(THIS)sinteger.ao $(THIS)pointer.ao
$(THIS)lv.ao:		$(THIS)basic.ao
$(THIS)machine.ao:	$(THIS)lang.ao
$(THIS)opsys.ao:	$(THIS)foamcat.ao $(THIS)oslow.ao $(THIS)file.ao
$(THIS)oslow.ao:	$(THIS)foamcat.ao $(THIS)parray.ao $(THIS)pointer.ao $(THIS)string.ao
$(THIS)parray.ao:	$(THIS)foamcat.ao $(THIS)sinteger.ao
$(THIS)partial.ao:	$(THIS)foamcat.ao $(THIS)pointer.ao
$(THIS)pointer.ao:	$(THIS)foamcat.ao
$(THIS)segment.ao:	$(THIS)foamcat.ao $(THIS)bool.ao
$(THIS)sfloat.ao:	$(THIS)foamcat.ao $(THIS)sinteger.ao
$(THIS)sinteger.ao:	$(THIS)foamcat.ao $(THIS)segment.ao
$(THIS)string.ao:	$(THIS)foamcat.ao $(THIS)character.ao $(THIS)array.ao
$(THIS)textwrit.ao:	$(THIS)foamcat.ao $(THIS)fname.ao $(THIS)file.ao $(THIS)array.ao $(THIS)opsys.ao
$(THIS)tuple.ao:	$(THIS)foamcat.ao
