aldorincdir	:= $(top_srcdir)/lib/aldor/include
aldorlibdir	:= $(top_builddir)/lib/aldor/src
abs_libdir	:= $(abs_top_builddir)/lib/algebra/src

libraryname	:= algebra
librarydeps     := aldor

#AXLCDB		:= -W check -Csmax=0 -Zdb -Qno-cc
AXLFLAGS	:= -Z db $(AXLCDB)
AXLFLAGS	+= -Y $(aldorlibdir) -I $(aldorincdir) -laldor -Q3

javalibrary := $(library)
withdocs := true
include $(top_srcdir)/lib/buildlib.mk
