aldorsrcdir	:= $(top_srcdir)/aldor/src
aldorexedir	:= $(top_builddir)/aldor/src
aldorincdir	:= $(top_srcdir)/lib/aldor/include
aldorlibdir	:= $(top_builddir)/lib/aldor/src

libraryincdir	:= $(top_srcdir)/lib/algebra/include
librarylibdir	:= $(top_builddir)/lib/algebra/src

libraryname	:= algebra

#AXLCDB		:= -W check -Csmax=0 -Fc -Zdb -Qno-cc
AXLFLAGS	:= -Z db -Fc -Q2 $(AXLCDB)
AXLFLAGS	+= -Y $(aldorlibdir) -I $(aldorincdir) -laldor

include $(srcdir)/Makefile.deps
include $(top_srcdir)/lib/buildlib.mk
