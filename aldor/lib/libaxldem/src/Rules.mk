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

libaxldem_HEADERS := axldem.as

# Copy includes
$(INCDIR)/axldem.as: $(THIS)axldem.as
	cp $< $@

$(eval $(call aldor-target,axldem))

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
