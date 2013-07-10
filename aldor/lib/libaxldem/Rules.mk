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
build/libaxldem.a: $(libaxldem_COBJECTS)
	$(AR) cr $@ $^

# Local aldor build rule
$(THIS)%.c: $(THIS)%.as build/aldor build/include/aldor.conf build/include/axldem.as
	build/aldor $(AFLAGS) $<
	$(AR) cr build/libaxldem.al $(@:.c=.ao)

# Copy includes
build/include/axldem.as: $(THIS)axldem.as
	cp $< $@

# Clean
clean: clean-$(THIS)
clean-$(THIS):
	$(RM) $(libaxldem_AOBJECTS)
	$(RM) $(libaxldem_COBJECTS)
	$(RM) $(libaxldem_ASOURCES:.as=.c)
	$(RM) build/libaxldem.a build/libaxldem.al

# Depend
$(THIS)dirprod.c:	\
	$(THIS)polycat.c
$(THIS)gb.c:	\
	$(THIS)polycat.c
$(THIS)matopdom.c:	\
	$(THIS)matrix.c
$(THIS)matrix.c:	\
	$(THIS)vector.c
$(THIS)nni.c:	\
	$(THIS)polycat.c
$(THIS)poly.c:	\
	$(THIS)polycat.c
$(THIS)poly3.c:	\
	$(THIS)poly.c
$(THIS)prime.c:	\
	$(THIS)nni.c	\
	$(THIS)vector.c
$(THIS)polycat.c:	\
	build/libaxllib.a
