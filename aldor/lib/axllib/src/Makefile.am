SUBDIRS=al

aldorsrcdir = $(abs_top_srcdir)/aldor/src

AM_CFLAGS = -I$(aldorsrcdir)
libaxllib_a_SOURCES= al/array.c al/axlcat.c al/basic.c al/boolean.c	\
	             al/bpower.c al/char.c al/complex.c al/dfloat.c	\
	             al/file.c al/fname.c al/format.c al/gener.c	\
	             al/imod.c al/integer.c al/lang.c al/langx.c	\
	             al/list.c al/machine.c al/opsys.c al/oslow.c	\
	             al/parray.c al/partial.c al/pointer.c al/ratio.c	\
	             al/segment.c al/sfloat.c al/sinteger.c		\
	             al/string.c al/textwrit.c al/tuple.c al/rtexns.c	\
	             al/float.c al/table.c al/except.c al/fprint.c	\
	             al/fmtout.c al/object.c al/complex.as al/efuns.c	\
	             al/ieeectl.c al/except.c al/pfloat.c al/sort.c


lib_LIBRARIES=libaxllib.a
datalibdir = $(datadir)/lib
datalib_DATA=al/libaxllib.al
include_HEADERS=axllib.as
