aldorincdir	:= $(top_srcdir)/lib/aldor/include
aldorlibdir	:= $(top_builddir)/lib/aldor/src
algebraincdir	:= $(top_srcdir)/lib/algebra/include
algebralibdir	:= $(top_builddir)/lib/algebra/src
typesincdir	:= $(top_srcdir)/lib/types/include
typeslibdir	:= $(top_builddir)/lib/types/src
abs_libdir	:= $(abs_top_builddir)/lib/types/src

libraryname	:= types
librarydeps     := aldor algebra

#AXLCDB		:= -W check -Csmax=0 -Zdb -Qno-cc
AXLFLAGS	:= $(AXLCDB)
AXLFLAGS	+= 
AXLFLAGS	+= -Y $(aldorlibdir) -I $(aldorincdir) -laldor
AXLFLAGS	+= -Y $(algebralibdir) -I $(algebraincdir) -lalgebra

javalibrary := $(library)
withdocs := false
include $(top_srcdir)/lib/buildlib.mk
