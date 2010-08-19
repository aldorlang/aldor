SUBDIRS=al

libfoam_ALDOR=al/runtime.c
libfoam_SRC=\
	$(aldorsrcdir)/bigint.c \
	$(aldorsrcdir)/btree.c  \
	$(aldorsrcdir)/dword.c \
	$(aldorsrcdir)/foamopt.c \
	$(aldorsrcdir)/output.c \
	$(aldorsrcdir)/opsys.c \
	$(aldorsrcdir)/store.c \
	$(aldorsrcdir)/table.c \
	$(aldorsrcdir)/foam_c.c \
	$(aldorsrcdir)/foam_i.c \
	$(aldorsrcdir)/foam_cfp.c \
	$(aldorsrcdir)/format.c  \
	$(aldorsrcdir)/stdc.c  \
	$(aldorsrcdir)/util.c	\
	$(aldorsrcdir)/timer.c	\
	$(aldorsrcdir)/compopt.c	\
	$(aldorsrcdir)/xfloat.c  

aldorsrcdir = $(abs_top_builddir)/aldor/src

AM_CFLAGS = -I$(aldorsrcdir)
libfoam_a_SOURCES=$(libfoam_SRC) $(libfoam_ALDOR)

noinst_LIBRARIES=libfoam.a
