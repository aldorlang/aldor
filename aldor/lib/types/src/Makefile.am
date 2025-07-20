SUBDIRS=base types xsubsts

@BUILD_JAVA_TRUE@JAVA_SUBDIRS = $(SUBDIRS)
@BUILD_JAVA_TRUE@JAVA_TARGET = types.jar types-sources.jar

lib_LIBRARIES = libtypes.a

libtypes_a_SOURCES = \
	base/tp_annotated.c	\
	base/tp_expr.c		\
	base/tp_listset.c	\
	base/tp_sx.c		\
	types/tp_counter.c	\
	types/tp_env.c		\
	types/tp_infer2.c	\
	types/tp_scopes.c	\
	types/tp_subst.c	\
	types/tp_test.c		\
	types/tp_tposs.c	\
	types/tp_typeterm.c	\
	types/tp_unify2.c	\
	hm/tp_hm.as		\
	xsubsts/tp_xsubst.c

datalibdir = $(datadir)/lib
datalib_DATA = libtypes.al $(JAVA_TARGET)

libraryname = types
include ../../buildlib.am

