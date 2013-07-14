THIS := $(dir $(lastword $(MAKEFILE_LIST)))

libaxllib_ASOURCES :=	\
	array.as	\
	axlcat.as	\
	basic.as	\
	axl_boolean.as	\
	bpower.as	\
	axl_byte.as	\
	axl_char.as	\
	complex.as	\
	debug.as	\
	dfloat.as	\
	efuns.as	\
	except.as	\
	file.as	\
	axl_float.as	\
	fmtout.as	\
	fname.as	\
	format.as	\
	fprint.as	\
	fstring.as	\
	gener.as	\
	hinteger.as	\
	ieeectl.as	\
	imod.as	\
	integer.as	\
	lang.as	\
	langx.as	\
	list.as	\
	list2.as	\
	machine.as	\
	object.as	\
	opsys.as	\
	oslow.as	\
	parray.as	\
	partial.as	\
	pfloat.as	\
	pkarray.as	\
	pointer.as	\
	ratio.as	\
	ref.as	\
	rtexns.as	\
	segment.as	\
	sfloat.as	\
	sinteger.as	\
	sort.as	\
	string.as	\
	table.as	\
	textread.as	\
	textwrit.as	\
	tuple.as	\
	uarray.as

libaxllib_INCLUDES := $(INCDIR)/axllib.as

# Copy includes
$(INCDIR)/axllib.as: $(THIS)axllib.as
	cp $$< $$@

$(eval $(call aldor-target,axllib))

# Depend
$(THIS)array.ao:	\
	$(THIS)langx.ao	\
	$(THIS)parray.ao
$(THIS)axlcat.ao:	\
	$(THIS)basic.ao
$(THIS)list2.ao:	\
	$(THIS)list.ao
$(THIS)ieeectl.ao:	\
	$(THIS)axlcat.ao\
	$(THIS)dfloat.ao
$(THIS)basic.ao:	\
	$(THIS)machine.ao
$(THIS)axl_boolean.ao:	\
	$(THIS)lang.ao	\
	$(THIS)axlcat.ao
$(THIS)bpower.ao:	\
	$(THIS)sinteger.ao
$(THIS)axl_byte.ao:	\
	$(THIS)sinteger.ao
$(THIS)axl_char.ao:	\
	$(THIS)integer.ao	\
	$(THIS)segment.ao
$(THIS)complex.ao:	\
	$(THIS)langx.ao	\
	$(THIS)bpower.ao	\
	$(THIS)integer.ao	\
	$(THIS)string.ao
$(THIS)debug.ao:	\
	$(THIS)langx.ao	\
	$(THIS)textwrit.ao
$(THIS)dfloat.ao:	\
	$(THIS)langx.ao	\
	$(THIS)sfloat.ao
$(THIS)efuns.ao:	\
	$(THIS)complex.ao	\
	$(THIS)except.ao	\
	$(THIS)dfloat.ao	\
	$(THIS)textwrit.ao
$(THIS)except.ao:	\
	$(THIS)axlcat.ao
$(THIS)file.ao:	\
	$(THIS)pointer.ao	\
	$(THIS)array.ao	\
	$(THIS)fname.ao	\
	$(THIS)axl_char.ao
$(THIS)axl_float.ao:	\
	$(THIS)format.ao	\
	$(THIS)except.ao	\
	$(THIS)integer.ao	\
	$(THIS)fprint.ao	\
	$(THIS)segment.ao
$(THIS)fmtout.ao:	\
	$(THIS)textwrit.ao
$(THIS)fname.ao:	\
	$(THIS)oslow.ao
$(THIS)format.ao:	\
	$(THIS)axl_char.ao	\
	$(THIS)dfloat.ao	\
	$(THIS)textwrit.ao
$(THIS)fprint.ao:	\
	$(THIS)langx.ao	\
	$(THIS)string.ao
$(THIS)fstring.ao:	\
	$(THIS)textwrit.ao
$(THIS)gener.ao:	\
	$(THIS)basic.ao
$(THIS)hinteger.ao:	\
	$(THIS)sinteger.ao
$(THIS)imod.ao:	\
	$(THIS)bpower.ao	\
	$(THIS)integer.ao	\
	$(THIS)axlcat.ao	\
	$(THIS)segment.ao
$(THIS)integer.ao:	\
	$(THIS)sinteger.ao
$(THIS)langx.ao:	\
	$(THIS)axlcat.ao	\
	$(THIS)segment.ao	\
	$(THIS)list.ao
$(THIS)list.ao:	\
	$(THIS)pointer.ao	\
	$(THIS)sinteger.ao	\
	$(THIS)tuple.ao
$(THIS)machine.ao:	\
	$(THIS)lang.ao
$(THIS)object.ao:	\
	$(THIS)langx.ao
$(THIS)opsys.ao:	\
	$(THIS)file.ao
$(THIS)oslow.ao:	\
	$(THIS)langx.ao	\
	$(THIS)string.ao
$(THIS)parray.ao:	\
	$(THIS)integer.ao
$(THIS)partial.ao:	\
	$(THIS)pointer.ao	\
	$(THIS)machine.ao	\
	$(THIS)string.ao
$(THIS)pfloat.ao:	\
	$(THIS)format.ao	\
	$(THIS)except.ao	\
	$(THIS)fprint.ao
$(THIS)pkarray.ao:	\
	$(THIS)sinteger.ao	\
	$(THIS)axlcat.ao
$(THIS)pointer.ao:	\
	$(THIS)axl_boolean.ao
$(THIS)ratio.ao:	\
	$(THIS)langx.ao	\
	$(THIS)string.ao	\
	$(THIS)bpower.ao	\
	$(THIS)integer.ao
$(THIS)ref.ao:	\
	$(THIS)basic.ao
$(THIS)segment.ao:	\
	$(THIS)axl_boolean.ao	\
	$(THIS)gener.ao
$(THIS)sfloat.ao:	\
	$(THIS)bpower.ao	\
	$(THIS)integer.ao	\
	$(THIS)string.ao
$(THIS)sinteger.ao:	\
	$(THIS)segment.ao
$(THIS)sort.ao:	\
	$(THIS)string.ao	\
	$(THIS)list.ao
$(THIS)string.ao:	\
	$(THIS)array.ao	\
	$(THIS)axl_char.ao	\
	$(THIS)tuple.ao	\
	$(THIS)axlcat.ao
$(THIS)table.ao:	\
	$(THIS)sfloat.ao	\
	$(THIS)list.ao	\
	$(THIS)textwrit.ao
$(THIS)textread.ao:	\
	$(THIS)string.ao	\
	$(THIS)file.ao
$(THIS)textwrit.ao:	\
	$(THIS)opsys.ao	\
	$(THIS)string.ao
$(THIS)tuple.ao:	\
	$(THIS)basic.ao
$(THIS)uarray.ao:	\
	$(THIS)sinteger.ao
$(THIS)rtexns.ao:	\
	$(THIS)langx.ao	\
	$(THIS)textwrit.ao
