SUBDIRS=al

aldorsrcdir = $(abs_top_builddir)/aldor/src

AM_CFLAGS = -I$(aldorsrcdir)

libaxldem_a_SOURCES= al/dirprod.c al/gb.c al/ibits.c al/lmdict.c	\
		     al/matopdom.c al/matrix.c al/nni.c al/poly3.c	\
		     al/poly.c al/polycat.c al/prime.c al/quanc8.c	\
		     al/random.c al/spf.c al/vector.c

lib_LIBRARIES=libaxldem.a
datalibdir = $(datadir)/lib
datalib_DATA=al/libaxldem.al
include_HEADERS=axldem.as
