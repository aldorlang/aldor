# These need to be in dependency order.
SUBDIRS =	\
	util	\
	tform	


@BUILD_JAVA_TRUE@JAVA_SUBDIRS = $(SUBDIRS)
@BUILD_JAVA_TRUE@JAVA_TARGET = comp.jar

lib_LIBRARIES = libcomp.a

libcomp_a_SOURCES =	\
	tform/tform.c

datalibdir = $(datadir)/lib
datalib_DATA = libcomp.al $(JAVA_TARGET)

libraryname=comp
include ../../buildlib.am

