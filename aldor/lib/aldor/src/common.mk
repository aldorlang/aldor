aldorincdir	:= $(top_srcdir)/lib/aldor/include
aldorlibdir	:= $(top_builddir)/lib/aldor/src

libraryname	:= aldor
librarydeps     :=

#AXLCDB		:= -W check -Csmax=0 -Zdb -Qno-cc
AXLFLAGS	:= -Q3 $(AXLCDB)

include $(top_srcdir)/lib/buildlib.mk
