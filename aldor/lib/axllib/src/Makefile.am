SUBDIRS = al

lib_LIBRARIES = libaxllib.a

libaxllib_a_SOURCES =	\
	al/array.c	\
	al/axl_boolean.c	\
	al/axl_byte.c	\
	al/axlcat.c	\
	al/axl_char.c	\
	al/axl_float.c	\
	al/basic.c	\
	al/bpower.c	\
	al/buffer.c	\
	al/complex.as	\
	al/complex.c	\
	al/dfloat.c	\
	al/efuns.c	\
	al/except.c	\
	al/except.c	\
	al/file.c	\
	al/fmtout.c	\
	al/fname.c	\
	al/format.c	\
	al/fprint.c	\
	al/gener.c	\
	al/ieeectl.c	\
	al/imod.c	\
	al/integer.c	\
	al/lang.c	\
	al/langx.c	\
	al/list.c	\
	al/machine.c	\
	al/object.c	\
	al/opsys.c	\
	al/oslow.c	\
	al/parray.c	\
	al/partial.c	\
	al/pfloat.c	\
	al/pointer.c	\
	al/ratio.c	\
	al/rtexns.c	\
	al/segment.c	\
	al/sfloat.c	\
	al/sinteger.c	\
	al/sort.c	\
	al/string.c	\
	al/table.c	\
	al/textread.c	\
	al/textwrit.c	\
	al/tuple.c	\
	al/xassert.c	\
	#

datalibdir = $(datadir)/lib
datalib_DATA = libaxllib.al

libraryname = axllib
include ../../buildlib.am

AM_CFLAGS += -ffloat-store $(LIB_CC_FLAGS)
