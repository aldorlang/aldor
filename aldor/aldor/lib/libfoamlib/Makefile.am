
SUBDIRS=al
libfoamlib_ALDOR=al/array.c al/basic.c al/bool.c al/character.c		\
	         al/file.c al/fname.c al/foamcat.c al/format.c		\
	         al/gener.c al/lang.c al/langx.c al/list.c		\
	         al/machine.c al/opsys.c al/oslow.c al/parray.c		\
	         al/partial.c al/pointer.c al/segment.c al/sfloat.c	\
	         al/sinteger.c al/string.c al/textwrit.c al/tuple.c

other_files=al/libfoamlib.al
aldorsrcdir = $(abs_top_srcdir)/aldor/src

libfoamlib_a_SOURCES=$(libfoamlib_ALDOR)
CFLAGS=-Wno-error
AM_CFLAGS=-I$(aldorsrcdir)

noinst_LIBRARIES=libfoamlib.a


