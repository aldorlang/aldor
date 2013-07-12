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

libaxllib_ASOURCES := $(addprefix $(THIS), $(libaxllib_ASOURCES))

libaxllib_AOBJECTS := $(libaxllib_ASOURCES:.as=.ao)
libaxllib_COBJECTS := $(libaxllib_ASOURCES:.as=$(OBJEXT))


# C library
$(LIBDIR)/libaxllib$(LIBEXT): $(libaxllib_COBJECTS)
	$(AR) cr $@ $^

# Local aldor build rule
$(THIS)%$(OBJEXT): $(THIS)%.as $(BINDIR)/aldor$(EXEEXT) $(BINDIR)/unicl$(EXEEXT) $(aldor_HEADERS) $(INCDIR)/axllib.as
	$(BINDIR)/aldor$(EXEEXT) $(AFLAGS) $<
	$(AR) cr $(LIBDIR)/libaxllib.al $(@:$(OBJEXT)=.ao)

# Copy includes
$(INCDIR)/axllib.as: $(THIS)axllib.as
	mkdir -p $(dir $@)
	cp $< $@

# Clean
clean: clean-libaxllib
clean-libaxllib:
	$(RM) $(libaxllib_AOBJECTS)
	$(RM) $(libaxllib_COBJECTS)
	$(RM) $(libaxllib_ASOURCES:.as=.c)
	$(RM) $(LIBDIR)/libaxllib$(LIBEXT) $(LIBDIR)/libaxllib.al

# Depend
$(THIS)array$(OBJEXT):	\
	$(THIS)langx$(OBJEXT)	\
	$(THIS)parray$(OBJEXT)
$(THIS)axlcat$(OBJEXT):	\
	$(THIS)basic$(OBJEXT)
$(THIS)list2$(OBJEXT):	\
	$(THIS)list$(OBJEXT)
$(THIS)ieeectl$(OBJEXT):	\
	$(THIS)axlcat$(OBJEXT)\
	$(THIS)dfloat$(OBJEXT)
$(THIS)basic$(OBJEXT):	\
	$(THIS)machine$(OBJEXT)
$(THIS)axl_boolean$(OBJEXT):	\
	$(THIS)lang$(OBJEXT)	\
	$(THIS)axlcat$(OBJEXT)
$(THIS)bpower$(OBJEXT):	\
	$(THIS)sinteger$(OBJEXT)
$(THIS)axl_byte$(OBJEXT):	\
	$(THIS)sinteger$(OBJEXT)
$(THIS)axl_char$(OBJEXT):	\
	$(THIS)integer$(OBJEXT)	\
	$(THIS)segment$(OBJEXT)
$(THIS)complex$(OBJEXT):	\
	$(THIS)langx$(OBJEXT)	\
	$(THIS)bpower$(OBJEXT)	\
	$(THIS)integer$(OBJEXT)	\
	$(THIS)string$(OBJEXT)
$(THIS)debug$(OBJEXT):	\
	$(THIS)langx$(OBJEXT)	\
	$(THIS)textwrit$(OBJEXT)
$(THIS)dfloat$(OBJEXT):	\
	$(THIS)langx$(OBJEXT)	\
	$(THIS)sfloat$(OBJEXT)
$(THIS)efuns$(OBJEXT):	\
	$(THIS)complex$(OBJEXT)	\
	$(THIS)except$(OBJEXT)	\
	$(THIS)dfloat$(OBJEXT)	\
	$(THIS)textwrit$(OBJEXT)
$(THIS)except$(OBJEXT):	\
	$(THIS)axlcat$(OBJEXT)
$(THIS)file$(OBJEXT):	\
	$(THIS)pointer$(OBJEXT)	\
	$(THIS)array$(OBJEXT)	\
	$(THIS)fname$(OBJEXT)	\
	$(THIS)axl_char$(OBJEXT)
$(THIS)axl_float$(OBJEXT):	\
	$(THIS)format$(OBJEXT)	\
	$(THIS)except$(OBJEXT)	\
	$(THIS)integer$(OBJEXT)	\
	$(THIS)fprint$(OBJEXT)	\
	$(THIS)segment$(OBJEXT)
$(THIS)fmtout$(OBJEXT):	\
	$(THIS)textwrit$(OBJEXT)
$(THIS)fname$(OBJEXT):	\
	$(THIS)oslow$(OBJEXT)
$(THIS)format$(OBJEXT):	\
	$(THIS)axl_char$(OBJEXT)	\
	$(THIS)dfloat$(OBJEXT)	\
	$(THIS)textwrit$(OBJEXT)
$(THIS)fprint$(OBJEXT):	\
	$(THIS)langx$(OBJEXT)	\
	$(THIS)string$(OBJEXT)
$(THIS)fstring$(OBJEXT):	\
	$(THIS)textwrit$(OBJEXT)
$(THIS)gener$(OBJEXT):	\
	$(THIS)basic$(OBJEXT)
$(THIS)hinteger$(OBJEXT):	\
	$(THIS)sinteger$(OBJEXT)
$(THIS)imod$(OBJEXT):	\
	$(THIS)bpower$(OBJEXT)	\
	$(THIS)integer$(OBJEXT)	\
	$(THIS)axlcat$(OBJEXT)	\
	$(THIS)segment$(OBJEXT)
$(THIS)integer$(OBJEXT):	\
	$(THIS)sinteger$(OBJEXT)
$(THIS)langx$(OBJEXT):	\
	$(THIS)axlcat$(OBJEXT)	\
	$(THIS)segment$(OBJEXT)	\
	$(THIS)list$(OBJEXT)
$(THIS)list$(OBJEXT):	\
	$(THIS)pointer$(OBJEXT)	\
	$(THIS)sinteger$(OBJEXT)	\
	$(THIS)tuple$(OBJEXT)
$(THIS)machine$(OBJEXT):	\
	$(THIS)lang$(OBJEXT)
$(THIS)object$(OBJEXT):	\
	$(THIS)langx$(OBJEXT)
$(THIS)opsys$(OBJEXT):	\
	$(THIS)file$(OBJEXT)
$(THIS)oslow$(OBJEXT):	\
	$(THIS)langx$(OBJEXT)	\
	$(THIS)string$(OBJEXT)
$(THIS)parray$(OBJEXT):	\
	$(THIS)integer$(OBJEXT)
$(THIS)partial$(OBJEXT):	\
	$(THIS)pointer$(OBJEXT)	\
	$(THIS)machine$(OBJEXT)	\
	$(THIS)string$(OBJEXT)
$(THIS)pfloat$(OBJEXT):	\
	$(THIS)format$(OBJEXT)	\
	$(THIS)except$(OBJEXT)	\
	$(THIS)fprint$(OBJEXT)
$(THIS)pkarray$(OBJEXT):	\
	$(THIS)sinteger$(OBJEXT)	\
	$(THIS)axlcat$(OBJEXT)
$(THIS)pointer$(OBJEXT):	\
	$(THIS)axl_boolean$(OBJEXT)
$(THIS)ratio$(OBJEXT):	\
	$(THIS)langx$(OBJEXT)	\
	$(THIS)string$(OBJEXT)	\
	$(THIS)bpower$(OBJEXT)	\
	$(THIS)integer$(OBJEXT)
$(THIS)ref$(OBJEXT):	\
	$(THIS)basic$(OBJEXT)
$(THIS)segment$(OBJEXT):	\
	$(THIS)axl_boolean$(OBJEXT)	\
	$(THIS)gener$(OBJEXT)
$(THIS)sfloat$(OBJEXT):	\
	$(THIS)bpower$(OBJEXT)	\
	$(THIS)integer$(OBJEXT)	\
	$(THIS)string$(OBJEXT)
$(THIS)sinteger$(OBJEXT):	\
	$(THIS)segment$(OBJEXT)
$(THIS)sort$(OBJEXT):	\
	$(THIS)string$(OBJEXT)	\
	$(THIS)list$(OBJEXT)
$(THIS)string$(OBJEXT):	\
	$(THIS)array$(OBJEXT)	\
	$(THIS)axl_char$(OBJEXT)	\
	$(THIS)tuple$(OBJEXT)	\
	$(THIS)axlcat$(OBJEXT)
$(THIS)table$(OBJEXT):	\
	$(THIS)sfloat$(OBJEXT)	\
	$(THIS)list$(OBJEXT)	\
	$(THIS)textwrit$(OBJEXT)
$(THIS)textread$(OBJEXT):	\
	$(THIS)string$(OBJEXT)	\
	$(THIS)file$(OBJEXT)
$(THIS)textwrit$(OBJEXT):	\
	$(THIS)opsys$(OBJEXT)	\
	$(THIS)string$(OBJEXT)
$(THIS)tuple$(OBJEXT):	\
	$(THIS)basic$(OBJEXT)
$(THIS)uarray$(OBJEXT):	\
	$(THIS)sinteger$(OBJEXT)
$(THIS)rtexns$(OBJEXT):	\
	$(THIS)langx$(OBJEXT)	\
	$(THIS)textwrit$(OBJEXT)
