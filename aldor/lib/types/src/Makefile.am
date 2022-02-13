SUBDIRS=types

@BUILD_JAVA_TRUE@JAVA_SUBDIRS = $(SUBDIRS)
@BUILD_JAVA_TRUE@JAVA_TARGET = types.jar types-sources.jar

lib_LIBRARIES = libtypes.a

libtypes_a_SOURCES = \
	types/tp_annotated.c	\
	types/tp_counter.c	\
	types/tp_env.c		\
	types/tp_expr.c		\
	types/tp_infer.c	\
	types/tp_listset.c	\
	types/tp_subst.c	\
	types/tp_sx.c		\
	types/tp_test.c		\
	types/tp_tposs.c	\
	types/tp_typeterm.c	\
	types/tp_unify.c

datalibdir = $(datadir)/lib
datalib_DATA = libtypes.al $(JAVA_TARGET)

libraryname = types
include ../../buildlib.am

