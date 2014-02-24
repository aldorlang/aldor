# Aldor portability layer
noinst_LIBRARIES = libport.a
s = $(srcdir)/../../src

libport_a_SOURCES =	\
	$s/bigint.c	\
	$s/btree.c	\
	$s/buffer.c	\
	$s/cfgfile.c	\
	$s/compopt.c	\
	$s/debug.c	\
	$s/dword.c	\
	$s/file.c	\
	$s/fluid.c	\
	$s/fname.c	\
	$s/format.c	\
	$s/int.c	\
	$s/list.c	\
	$s/memclim.c	\
	$s/opsys.c	\
	$s/ostream.c	\
	$s/stdc.c	\
	$s/store.c	\
	$s/strops.c	\
	$s/timer.c	\
	$s/util.c	\
	$s/xfloat.c


# C compiler driver
bin_PROGRAMS = unicl

unicl_SOURCES = unicl.c
unicl_LDADD = libport.a -lm
unicl_CFLAGS = -I $s $(AM_CFLAGS)


# Print platform information
noinst_PROGRAMS = platform

platform_CFLAGS = -I $s $(AM_CFLAGS)
