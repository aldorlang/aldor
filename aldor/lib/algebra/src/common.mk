aldorincdir	:= $(top_srcdir)/lib/aldor/include
aldorlibdir	:= $(top_builddir)/lib/aldor/src

libraryname	:= algebra

#AXLCDB		:= -W check -Csmax=0 -Fc -Zdb -Qno-cc
AXLFLAGS	:= -Z db -Fc -Q2 $(AXLCDB)
AXLFLAGS	+= -Y $(aldorlibdir) -I $(aldorincdir) -laldor

include $(srcdir)/Makefile.deps
include $(top_srcdir)/lib/buildlib.mk
