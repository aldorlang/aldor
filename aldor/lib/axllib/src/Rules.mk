THIS := $(dir $(lastword $(MAKEFILE_LIST)))

libaxllib_ASOURCES :=	\
	array.as	\
	axlcat.as	\
	basic.as	\
	boolean.as	\
	bpower.as	\
	byte.as	\
	char.as	\
	complex.as	\
	debug.as	\
	dfloat.as	\
	efuns.as	\
	except.as	\
	file.as	\
	float.as	\
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

libaxllib_ASOURCES := $(addprefix $(THIS), $(libaxllib_ASOURCES))

libaxllib_AOBJECTS := $(libaxllib_ASOURCES:.as=.ao)
libaxllib_COBJECTS := $(libaxllib_ASOURCES:.as=.o)


# C library
build/libaxllib.a: $(libaxllib_COBJECTS)
	$(AR) cr $@ $^

# Local aldor build rule
$(THIS)%.c: $(THIS)%.as build/aldor build/include/aldor.conf build/include/axllib.as
	build/aldor $(AFLAGS) $<
	$(AR) cr build/libaxllib.al $(@:.c=.ao)

# Copy includes
build/include/axllib.as: $(THIS)axllib.as
	cp $< $@

# Clean
clean: clean-$(THIS)
clean-$(THIS):
	$(RM) $(libaxllib_AOBJECTS)
	$(RM) $(libaxllib_COBJECTS)
	$(RM) $(libaxllib_ASOURCES:.as=.c)
	$(RM) build/libaxllib.a build/libaxllib.al

# Depend
$(THIS)array.c:	\
	$(THIS)langx.c	\
	$(THIS)parray.c
$(THIS)axlcat.c:	\
	$(THIS)basic.c
$(THIS)list2.c:	\
	$(THIS)list.c
$(THIS)ieeectl.c:	\
	$(THIS)axlcat.c\
	$(THIS)dfloat.c
$(THIS)basic.c:	\
	$(THIS)machine.c
$(THIS)boolean.c:	\
	$(THIS)lang.c	\
	$(THIS)axlcat.c
$(THIS)bpower.c:	\
	$(THIS)sinteger.c
$(THIS)byte.c:	\
	$(THIS)sinteger.c
$(THIS)char.c:	\
	$(THIS)integer.c	\
	$(THIS)segment.c
$(THIS)complex.c:	\
	$(THIS)langx.c	\
	$(THIS)bpower.c	\
	$(THIS)integer.c	\
	$(THIS)string.c
$(THIS)debug.c:	\
	$(THIS)langx.c	\
	$(THIS)textwrit.c
$(THIS)dfloat.c:	\
	$(THIS)langx.c	\
	$(THIS)sfloat.c
$(THIS)efuns.c:	\
	$(THIS)complex.c	\
	$(THIS)except.c	\
	$(THIS)dfloat.c	\
	$(THIS)textwrit.c
$(THIS)except.c:	\
	$(THIS)axlcat.c
$(THIS)file.c:	\
	$(THIS)pointer.c	\
	$(THIS)array.c	\
	$(THIS)fname.c	\
	$(THIS)char.c
$(THIS)float.c:	\
	$(THIS)format.c	\
	$(THIS)except.c	\
	$(THIS)integer.c	\
	$(THIS)fprint.c	\
	$(THIS)segment.c
$(THIS)fmtout.c:	\
	$(THIS)textwrit.c
$(THIS)fname.c:	\
	$(THIS)oslow.c
$(THIS)format.c:	\
	$(THIS)char.c	\
	$(THIS)dfloat.c	\
	$(THIS)textwrit.c
$(THIS)fprint.c:	\
	$(THIS)langx.c	\
	$(THIS)string.c
$(THIS)fstring.c:	\
	$(THIS)textwrit.c
$(THIS)gener.c:	\
	$(THIS)basic.c
$(THIS)hinteger.c:	\
	$(THIS)sinteger.c
$(THIS)imod.c:	\
	$(THIS)bpower.c	\
	$(THIS)integer.c	\
	$(THIS)axlcat.c	\
	$(THIS)segment.c
$(THIS)integer.c:	\
	$(THIS)sinteger.c
$(THIS)langx.c:	\
	$(THIS)axlcat.c	\
	$(THIS)segment.c	\
	$(THIS)list.c
$(THIS)list.c:	\
	$(THIS)pointer.c	\
	$(THIS)sinteger.c	\
	$(THIS)tuple.c
$(THIS)machine.c:	\
	$(THIS)lang.c
$(THIS)object.c:	\
	$(THIS)langx.c
$(THIS)opsys.c:	\
	$(THIS)file.c
$(THIS)oslow.c:	\
	$(THIS)langx.c	\
	$(THIS)string.c
$(THIS)parray.c:	\
	$(THIS)integer.c
$(THIS)partial.c:	\
	$(THIS)pointer.c	\
	$(THIS)machine.c	\
	$(THIS)string.c
$(THIS)pfloat.c:	\
	$(THIS)format.c	\
	$(THIS)except.c	\
	$(THIS)fprint.c
$(THIS)pkarray.c:	\
	$(THIS)sinteger.c	\
	$(THIS)axlcat.c
$(THIS)pointer.c:	\
	$(THIS)boolean.c
$(THIS)ratio.c:	\
	$(THIS)langx.c	\
	$(THIS)string.c	\
	$(THIS)bpower.c	\
	$(THIS)integer.c
$(THIS)ref.c:	\
	$(THIS)basic.c
$(THIS)segment.c:	\
	$(THIS)boolean.c	\
	$(THIS)gener.c
$(THIS)sfloat.c:	\
	$(THIS)bpower.c	\
	$(THIS)integer.c	\
	$(THIS)string.c
$(THIS)sinteger.c:	\
	$(THIS)segment.c
$(THIS)sort.c:	\
	$(THIS)string.c	\
	$(THIS)list.c
$(THIS)string.c:	\
	$(THIS)array.c	\
	$(THIS)char.c	\
	$(THIS)tuple.c	\
	$(THIS)axlcat.c
$(THIS)table.c:	\
	$(THIS)sfloat.c	\
	$(THIS)list.c	\
	$(THIS)textwrit.c
$(THIS)textread.c:	\
	$(THIS)string.c	\
	$(THIS)file.c
$(THIS)textwrit.c:	\
	$(THIS)opsys.c	\
	$(THIS)string.c
$(THIS)tuple.c:	\
	$(THIS)basic.c
$(THIS)uarray.c:	\
	$(THIS)sinteger.c
$(THIS)rtexns.c:	\
	$(THIS)langx.c	\
	$(THIS)textwrit.c
