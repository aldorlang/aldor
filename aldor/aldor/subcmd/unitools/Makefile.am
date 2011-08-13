
bin_PROGRAMS = unicl platform
unicl_SOURCES = unicl.c
unicl_LDADD = libport.a
unicl_CFLAGS = -I $(s) $(AM_CFLAGS)

platform_CFLAGS = -I $(s) $(AM_CFLAGS)

noinst_LIBRARIES = libport.a
s = $(srcdir)/../../src

libport_a_SOURCES=$(s)/file.c $(s)/fname.c $(s)/opsys.c $(s)/store.c $(s)/memclim.c \
	$(s)/fluid.c $(s)/btree.c $(s)/stdc.c $(s)/strops.c \
	$(s)/debug.c $(s)/xfloat.c $(s)/buffer.c $(s)/timer.c \
	$(s)/util.c $(s)/list.c $(s)/format.c $(s)/cfgfile.c


