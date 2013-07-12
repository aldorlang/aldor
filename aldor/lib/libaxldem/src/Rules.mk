THIS := $(dir $(lastword $(MAKEFILE_LIST)))

libaxldem_ASOURCES :=	\
	dirprod.as	\
	gb.as	\
	ibits.as	\
	lmdict.as	\
	matopdom.as	\
	matrix.as	\
	nni.as	\
	poly.as	\
	poly3.as	\
	polycat.as	\
	prime.as	\
	quanc8.as	\
	random.as	\
	spf.as	\
	vector.as

libaxldem_ASOURCES := $(addprefix $(THIS), $(libaxldem_ASOURCES))

libaxldem_AOBJECTS := $(libaxldem_ASOURCES:.as=.ao)
libaxldem_COBJECTS := $(libaxldem_ASOURCES:.as=$(OBJEXT))


# C library
$(LIBDIR)/libaxldem$(LIBEXT): $(libaxldem_COBJECTS)
	$(AR) cr $@ $^

# Local aldor build rule
$(THIS)%$(OBJEXT): $(THIS)%.as $(BINDIR)/aldor$(EXEEXT) $(BINDIR)/unicl$(EXEEXT) $(aldor_HEADERS) $(INCDIR)/axldem.as
	$(BINDIR)/aldor$(EXEEXT) $(AFLAGS) $<
	$(AR) cr $(LIBDIR)/libaxldem.al $(@:$(OBJEXT)=.ao)

# Copy includes
$(INCDIR)/axldem.as: $(THIS)axldem.as
	mkdir -p $(dir $@)
	cp $< $@

# Clean
clean: clean-libaxldem
clean-libaxldem:
	$(RM) $(libaxldem_AOBJECTS)
	$(RM) $(libaxldem_COBJECTS)
	$(RM) $(libaxldem_ASOURCES:.as=.c)
	$(RM) $(LIBDIR)/libaxldem$(LIBEXT) $(LIBDIR)/libaxldem.al

# Depend
$(THIS)dirprod$(OBJEXT):	\
	$(THIS)polycat$(OBJEXT)
$(THIS)gb$(OBJEXT):	\
	$(THIS)polycat$(OBJEXT)
$(THIS)matopdom$(OBJEXT):	\
	$(THIS)matrix$(OBJEXT)
$(THIS)matrix$(OBJEXT):	\
	$(THIS)vector$(OBJEXT)
$(THIS)nni$(OBJEXT):	\
	$(THIS)polycat$(OBJEXT)
$(THIS)poly$(OBJEXT):	\
	$(THIS)polycat$(OBJEXT)
$(THIS)poly3$(OBJEXT):	\
	$(THIS)poly$(OBJEXT)
$(THIS)prime$(OBJEXT):	\
	$(THIS)nni$(OBJEXT)	\
	$(THIS)vector$(OBJEXT)
$(THIS)polycat$(OBJEXT):	\
	$(LIBDIR)/libaxllib$(LIBEXT)
