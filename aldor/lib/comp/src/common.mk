aldorincdir	:= $(top_srcdir)/lib/aldor/include
aldorlibdir	:= $(top_builddir)/lib/aldor/src

libraryname	:= comp
librarydeps     := aldor

#AXLCDB		:= -W check -Csmax=0 -Zdb -Qno-cc
AXLFLAGS	:= -Z db $(AXLCDB)
AXLFLAGS	+= -Y $(aldorlibdir) -I $(aldorincdir) -laldor

javalibrary := $(library)
include $(top_srcdir)/lib/buildlib.mk
