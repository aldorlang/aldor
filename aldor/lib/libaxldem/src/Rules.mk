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
libaxldem_COBJECTS := $(libaxldem_ASOURCES:.as=.o)


# C library
$(LIBDIR)/libaxldem.a: $(libaxldem_COBJECTS)
	$(AR) cr $@ $^

# Local aldor build rule
$(THIS)%.o: $(THIS)%.as $(BINDIR)/aldor $(BINDIR)/unicl $(aldor_HEADERS) $(INCDIR)/axldem.as
	$(BINDIR)/aldor $(AFLAGS) $<
	$(AR) cr $(LIBDIR)/libaxldem.al $(@:.o=.ao)

# Copy includes
$(INCDIR)/axldem.as: $(THIS)axldem.as
	mkdir -p $(dir $@)
	cp $< $@

# Clean
clean: clean-$(THIS)
clean-$(THIS):
	$(RM) $(libaxldem_AOBJECTS)
	$(RM) $(libaxldem_COBJECTS)
	$(RM) $(libaxldem_ASOURCES:.as=.c)
	$(RM) $(LIBDIR)/libaxldem.a $(LIBDIR)/libaxldem.al

# Depend
$(THIS)dirprod.o:	\
	$(THIS)polycat.o
$(THIS)gb.o:	\
	$(THIS)polycat.o
$(THIS)matopdom.o:	\
	$(THIS)matrix.o
$(THIS)matrix.o:	\
	$(THIS)vector.o
$(THIS)nni.o:	\
	$(THIS)polycat.o
$(THIS)poly.o:	\
	$(THIS)polycat.o
$(THIS)poly3.o:	\
	$(THIS)poly.o
$(THIS)prime.o:	\
	$(THIS)nni.o	\
	$(THIS)vector.o
$(THIS)polycat.o:	\
	$(LIBDIR)/libaxllib.a
