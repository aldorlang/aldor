##############################################################################
#
# Makefile
#
# Make program to build the basic library.
#
# Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#
##############################################################################
#
# Targets: all, junk, clean, quick
#
#    make all	-- make the basic library for the Aldor compiler
#    make junk	-- list files which are not part of the Aldor compiler
#    make clean -- remove generated files  (e.g. foo.c~)
#
# Parameters:  ALDORROOT
#
#    To override defaults do, e.g.,
#	 make -e ALDORROOT=/spad/local/aldor/rs all
#    or
#	 setenv ALDORROOT /spad/local/aldor/rs; setenv MAKEFLAGS e; make all
#
##############################################################################
#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

SHELL	= /bin/sh

#
# Environment
#
IncDir	= $(PWD)/include
LibDir	= $(PWD)/lib
BinDir	= $(PWD)/bin
SrcDir  = $(PWD)/src
TestDir = $(PWD)/test

LAXLLIB	= $(LibDir)/libaxllib.al
LOBLIB	= $(LibDir)/libaxllib.a

AXLCDB	 = -Wcheck -qno-del-assert # -Wdumb-import # -Wgc # -Qkillp # -Cargs=-g -Cargs='$$OPTS' -Fc -ffm #-Cargs=-g -Qno-cc -Ffm
AXLCDB	 = -Wcheck -qno-del-assert # -Wdebugger -Mno-mactext
AXLFLAGS = -Y$(LibDir) $(AXLCDB) # -Fc # -Wrtcache=-1 # -Qno-cc -Zprof

ECHON   = $(ALDORROOT)/bin/echon
DOALDOR = $(BinDir)/makeaxllibal.sh
DORANLIB = $(BinDir)/doranlib

CP	= cp -p
RM	= rm -f

#
# Source lists
#
# AXL<i>... depends on AXL<i-1>...

IncSrcs	= axllib.as
AXL0Srcs= lang.as		\
	  machine.as		\
	  basic.as		\
	  axlcat.as		\
	  tuple.as		\
	  gener.as		\
	  ref.as		\
	  except.as		\
	  boolean.as		\
	  segment.as		\
	  sinteger.as		\
	  byte.as		\
	  hinteger.as		\
	  bpower.as		\
	  integer.as		\
	  pointer.as		\
	  char.as		\
	  list.as		\
	  langx.as		\
	  uarray.as		\
	  pkarray.as		\
	  parray.as		\
	  array.as		\
	  string.as		\
	  sfloat.as		\
	  dfloat.as		\
	  imod.as		

AXL1Srcs= format.as		\
	  complex.as		\
	  ratio.as		\
	  efuns.as		\
	  partial.as		\
	  sort.as		\
	  oslow.as		\
	  fname.as		\
	  file.as		\
	  opsys.as		\
	  textwrit.as		\
	  textread.as		\
	  rtexns.as		\
	  fmtout.as		\
	  table.as		\
	  fprint.as		\
	  float.as		\
	  pfloat.as		\
	  ieeectl.as		\
	  object.as		\
	  fstring.as		\
	  debug.as		\


AXLSrcs	= $(AXL0Srcs) $(AXL1Srcs)

Sources	= $(IncSrcs) $(AXLSrcs)

#
# Object lists
#

AXL0Aos	= $(LAXLLIB)(lang.ao)		\
	  $(LAXLLIB)(machine.ao)	\
	  $(LAXLLIB)(basic.ao)		\
	  $(LAXLLIB)(axlcat.ao)		\
	  $(LAXLLIB)(tuple.ao)		\
	  $(LAXLLIB)(gener.ao)		\
	  $(LAXLLIB)(ref.ao)		\
	  $(LAXLLIB)(except.ao)		\
	  $(LAXLLIB)(boolean.ao)	\
	  $(LAXLLIB)(segment.ao)	\
	  $(LAXLLIB)(sinteger.ao)	\
	  $(LAXLLIB)(byte.ao)		\
	  $(LAXLLIB)(hinteger.ao)	\
	  $(LAXLLIB)(bpower.ao)		\
	  $(LAXLLIB)(integer.ao)	\
	  $(LAXLLIB)(pointer.ao)	\
	  $(LAXLLIB)(char.ao)		\
	  $(LAXLLIB)(list.ao)		\
	  $(LAXLLIB)(langx.ao)		\
	  $(LAXLLIB)(uarray.ao)		\
	  $(LAXLLIB)(pkarray.ao)	\
	  $(LAXLLIB)(parray.ao)		\
	  $(LAXLLIB)(array.ao)		\
	  $(LAXLLIB)(string.ao)		\
	  $(LAXLLIB)(sfloat.ao)		\
	  $(LAXLLIB)(dfloat.ao)		\
	  $(LAXLLIB)(imod.ao)		

AXL0Objs= $(LOBLIB)(lang.o)		\
	  $(LOBLIB)(machine.o)		\
	  $(LOBLIB)(basic.o)		\
	  $(LOBLIB)(axlcat.o)		\
	  $(LOBLIB)(tuple.o)		\
	  $(LOBLIB)(gener.o)		\
	  $(LOBLIB)(ref.o)		\
	  $(LOBLIB)(except.o)		\
	  $(LOBLIB)(boolean.o)		\
	  $(LOBLIB)(segment.o)		\
	  $(LOBLIB)(sinteger.o)		\
	  $(LOBLIB)(byte.o)		\
	  $(LOBLIB)(hinteger.o)		\
	  $(LOBLIB)(bpower.o)		\
	  $(LOBLIB)(integer.o)		\
	  $(LOBLIB)(pointer.o)		\
	  $(LOBLIB)(char.o)		\
	  $(LOBLIB)(list.o)		\
	  $(LOBLIB)(langx.o)		\
	  $(LOBLIB)(uarray.o)		\
	  $(LOBLIB)(pkarray.o)		\
	  $(LOBLIB)(parray.o)		\
	  $(LOBLIB)(array.o)		\
	  $(LOBLIB)(string.o)		\
	  $(LOBLIB)(sfloat.o)		\
	  $(LOBLIB)(dfloat.o)		\
	  $(LOBLIB)(imod.o)		\
	  $(LOBLIB)(mndWrite.o)		

AXL1Aos	= $(LAXLLIB)(format.ao)		\
	  $(LAXLLIB)(complex.ao)	\
	  $(LAXLLIB)(ratio.ao)		\
	  $(LAXLLIB)(efuns.ao)		\
	  $(LAXLLIB)(partial.ao)	\
	  $(LAXLLIB)(sort.ao)		\
	  $(LAXLLIB)(oslow.ao)		\
	  $(LAXLLIB)(fname.ao)		\
	  $(LAXLLIB)(file.ao)		\
	  $(LAXLLIB)(opsys.ao)		\
	  $(LAXLLIB)(textwrit.ao)	\
	  $(LAXLLIB)(textread.ao)	\
	  $(LAXLLIB)(rtexns.ao)		\
	  $(LAXLLIB)(fmtout.ao)		\
	  $(LAXLLIB)(table.ao)		\
	  $(LAXLLIB)(fprint.ao)		\
	  $(LAXLLIB)(pfloat.ao)		\
	  $(LAXLLIB)(float.ao)		\
	  $(LAXLLIB)(ieeectl.ao)	\
	  $(LAXLLIB)(object.ao)		\
	  $(LAXLLIB)(fstring.ao)	\
	  $(LAXLLIB)(debug.ao)		\

AXL1Objs= $(LOBLIB)(format.o)		\
	  $(LOBLIB)(complex.o)		\
	  $(LOBLIB)(ratio.o)		\
	  $(LOBLIB)(efuns.o)		\
	  $(LOBLIB)(partial.o)		\
	  $(LOBLIB)(sort.o)		\
	  $(LOBLIB)(oslow.o)		\
	  $(LOBLIB)(fname.o)		\
	  $(LOBLIB)(file.o)		\
	  $(LOBLIB)(opsys.o)		\
	  $(LOBLIB)(textwrit.o)		\
	  $(LOBLIB)(textread.o)		\
	  $(LOBLIB)(rtexns.o)		\
	  $(LOBLIB)(fmtout.o)		\
	  $(LOBLIB)(table.o)		\
	  $(LOBLIB)(fprint.o)		\
	  $(LOBLIB)(pfloat.o)		\
	  $(LOBLIB)(float.o)		\
	  $(LOBLIB)(ieeectl.o)		\
	  $(LOBLIB)(object.o)		\
	  $(LOBLIB)(fstring.o)		\
	  $(LOBLIB)(debug.o)		\


Objects	= $(AXL0Objs) $(AXL1Objs) 
Aos	= $(AXL0Aos) $(AXL1Aos)

#
# Target lists
#
IncTargs = $(IncDir)/axllib.as
LibTargs = $(LAXLLIB) $(LOBLIB)

# Don't delete the libraries if a compilation fails.
.PRECIOUS: $(LibTargs)

#
# Generic targets
#
OKFILES	 = Makefile Makefile.win32 $(Sources)

DESTDIR=$(GENERIC_DIR)/lib/axllib

# ----------------------------------------------------------------
# MAIN TARGETS
# ----------------------------------------------------------------

all:
	@echo "Usage:"
	@echo "   make portable => make portable versions of the library (.al)"
	@echo "   make objects  => create binary version from portable version"
	@echo "   make library  => build both portable and binary version"

portable: clean libclean include $(LAXLLIB)

objects: 
	@($(BinDir)/makeaxlliba.sh $(LAXLLIB))

library: portable objects
	cp lib/* $(ALDORROOT)/lib
	cp include/* $(ALDORROOT)/include

generic: checkgeneric msggeneric portable
	@(mkdir -p $(DESTDIR)/bin; \
	  mkdir -p $(DESTDIR)/lib; \
	  mkdir -p $(DESTDIR)/src; \
	  mkdir -p $(DESTDIR)/include; \
	  mv $(LAXLLIB)            $(DESTDIR)/lib; \
	  cp $(IncDir)/axllib.as   $(DESTDIR)/include; \
	  cp $(BinDir)/makeaxlliba.sh $(DESTDIR)/bin; \
	  cp src/mndWrite.c        $(DESTDIR)/src; \
	  cp Makefile.generic      $(DESTDIR)/Makefile; \
	  echo "*** Generic distribution for libaxllib is done ***")

checkgeneric:
	-@ if [ "x$(GENERIC_DIR)" = "x" ] ; then echo "*** GENERIC_DIR unset"; fi
	@ [ "x$(GENERIC_DIR)" != "x" ]
	if [ ! -d $(LibDir) ]; then  mkdir -p $(LibDir); fi

msggeneric:
	@echo "***********************************"
	@echo "Making generic version of libaxllib"
	@echo "***********************************"

# ----------------------------------------------------------------
# CLEAN
# ----------------------------------------------------------------

clean:
	@ $(RM) *~

libclean:
	@ $(RM) $(IncTargs) $(LAXLLIB) $(LOBLIB)

# ----------------------------------------------------------------
# MISC
# ----------------------------------------------------------------

# all:	$(IncTargs) $(LibTargs) 
include:	$(IncTargs)
libaxllib:	$(LAXLLIB) $(LOBLIB)
junk:
	@ ls -d $(OKFILES) * | sort | grep -v CVS | uniq -u

# ----------------------------------------------------------------

#
# General dependencies
#
$(Objects) $(Aos):	$(ALDOR) $(IncTargs)
$(AXL1Objs) $(AXL1Aos):	$(AXL0Aos)

#
# Specific dependencies and rules
#
$(IncDir)/axllib.as: $(SrcDir)/axllib.as
	$(CP) $(SrcDir)/axllib.as $(IncDir)/axllib.as


# The doranlib dependency ensures that doranlib gets called.
$(LAXLLIB):  $(AXL0Aos) $(AXL1Aos) doranlib	; @ echo doranlib $@
$(LOBLIB):  $(AXL0Objs) $(AXL1Objs) doranlib	; doranlib $@

doranlib:
	@$(ECHON)

#
# AXL0Srcs
#
$(LAXLLIB)(lang.ao): $(SrcDir)/lang.as
	@$(DOALDOR) $(SrcDir)/lang.as $(AXLFLAGS) -M no-ALDOR_W_WillObsolete

$(LAXLLIB)(machine.ao): $(SrcDir)/machine.as
	@$(DOALDOR) $(SrcDir)/machine.as $(AXLFLAGS) -laxllib

$(LAXLLIB)(basic.ao): $(SrcDir)/basic.as
	@$(DOALDOR) $(SrcDir)/basic.as $(AXLFLAGS) -laxllib -Q inline-limit:18

$(LAXLLIB)(axlcat.ao): $(SrcDir)/axlcat.as
	@$(DOALDOR) $(SrcDir)/axlcat.as $(AXLFLAGS) -laxllib

$(LAXLLIB)(langx.ao): $(SrcDir)/langx.as
	@$(DOALDOR) $(SrcDir)/langx.as $(AXLFLAGS)

$(LAXLLIB)(rtexns.ao): $(SrcDir)/rtexns.as
	@$(DOALDOR) $(SrcDir)/rtexns.as $(AXLFLAGS)

$(LAXLLIB)(tuple.ao): $(SrcDir)/tuple.as
	@$(DOALDOR) $(SrcDir)/tuple.as $(AXLFLAGS)

$(LAXLLIB)(gener.ao): $(SrcDir)/gener.as
	@$(DOALDOR) $(SrcDir)/gener.as $(AXLFLAGS)

$(LAXLLIB)(ref.ao): $(SrcDir)/ref.as
	@$(DOALDOR) $(SrcDir)/ref.as $(AXLFLAGS)

$(LAXLLIB)(except.ao): $(SrcDir)/except.as
	@$(DOALDOR) $(SrcDir)/except.as $(AXLFLAGS)

$(LAXLLIB)(boolean.ao): $(SrcDir)/boolean.as
	@$(DOALDOR) $(SrcDir)/boolean.as $(AXLFLAGS)

$(LAXLLIB)(segment.ao): $(SrcDir)/segment.as
	@$(DOALDOR) $(SrcDir)/segment.as $(AXLFLAGS)

$(LAXLLIB)(byte.ao): $(SrcDir)/byte.as
	@$(DOALDOR) $(SrcDir)/byte.as $(AXLFLAGS)

$(LAXLLIB)(hinteger.ao): $(SrcDir)/hinteger.as
	@$(DOALDOR) $(SrcDir)/hinteger.as $(AXLFLAGS)

$(LAXLLIB)(sinteger.ao): $(SrcDir)/sinteger.as
	@$(DOALDOR) $(SrcDir)/sinteger.as $(AXLFLAGS)

$(LAXLLIB)(bpower.ao): $(SrcDir)/bpower.as
	@$(DOALDOR) $(SrcDir)/bpower.as $(AXLFLAGS)

$(LAXLLIB)(integer.ao): $(SrcDir)/integer.as
	@$(DOALDOR) $(SrcDir)/integer.as $(AXLFLAGS)

$(LAXLLIB)(pointer.ao): $(SrcDir)/pointer.as
	@$(DOALDOR) $(SrcDir)/pointer.as $(AXLFLAGS)

$(LAXLLIB)(char.ao): $(SrcDir)/char.as
	@$(DOALDOR) $(SrcDir)/char.as $(AXLFLAGS)

$(LAXLLIB)(string.ao): $(SrcDir)/string.as
	@$(DOALDOR) $(SrcDir)/string.as $(AXLFLAGS)

$(LAXLLIB)(fstring.ao): $(SrcDir)/fstring.as
	@$(DOALDOR) $(SrcDir)/fstring.as $(AXLFLAGS)

$(LAXLLIB)(sfloat.ao): $(SrcDir)/sfloat.as
	@$(DOALDOR) $(SrcDir)/sfloat.as $(AXLFLAGS)

$(LAXLLIB)(dfloat.ao): $(SrcDir)/dfloat.as
	@$(DOALDOR) $(SrcDir)/dfloat.as $(AXLFLAGS)

$(LAXLLIB)(imod.ao): $(SrcDir)/imod.as
	@$(DOALDOR) $(SrcDir)/imod.as $(AXLFLAGS)

$(LAXLLIB)(fmdarray.ao): $(SrcDir)/fmdarray.as
	@$(DOALDOR) $(SrcDir)/fmdarray.as $(AXLFLAGS)

#
# AXL1Srcs
#
$(LAXLLIB)(format.ao): $(SrcDir)/format.as
	@$(DOALDOR) $(SrcDir)/format.as $(AXLFLAGS)

$(LAXLLIB)(complex.ao): $(SrcDir)/complex.as
	@$(DOALDOR) $(SrcDir)/complex.as $(AXLFLAGS)

$(LAXLLIB)(ratio.ao): $(SrcDir)/ratio.as
	@$(DOALDOR) $(SrcDir)/ratio.as $(AXLFLAGS)

$(LAXLLIB)(list.ao): $(SrcDir)/list.as
	@$(DOALDOR) $(SrcDir)/list.as $(AXLFLAGS)

$(LAXLLIB)(list2.ao): $(SrcDir)/list2.as
	@$(DOALDOR) $(SrcDir)/list2.as $(AXLFLAGS)

$(LAXLLIB)(uarray.ao): $(SrcDir)/uarray.as
	@$(DOALDOR) $(SrcDir)/uarray.as $(AXLFLAGS)

$(LAXLLIB)(pkarray.ao): $(SrcDir)/pkarray.as
	@$(DOALDOR) $(SrcDir)/pkarray.as $(AXLFLAGS)

$(LAXLLIB)(parray.ao): $(SrcDir)/parray.as
	@$(DOALDOR) $(SrcDir)/parray.as $(AXLFLAGS)

$(LAXLLIB)(array.ao): $(SrcDir)/array.as
	@$(DOALDOR) $(SrcDir)/array.as $(AXLFLAGS)

$(LAXLLIB)(efuns.ao): $(SrcDir)/efuns.as
	@$(DOALDOR) $(SrcDir)/efuns.as $(AXLFLAGS)

$(LAXLLIB)(partial.ao): $(SrcDir)/partial.as
	@$(DOALDOR) $(SrcDir)/partial.as $(AXLFLAGS)

$(LAXLLIB)(sort.ao): $(SrcDir)/sort.as
	@$(DOALDOR) $(SrcDir)/sort.as $(AXLFLAGS)

$(LAXLLIB)(fname.ao): $(SrcDir)/fname.as
	@$(DOALDOR) $(SrcDir)/fname.as $(AXLFLAGS)

$(LAXLLIB)(file.ao): $(SrcDir)/file.as
	@$(DOALDOR) $(SrcDir)/file.as $(AXLFLAGS)

$(LAXLLIB)(opsys.ao): $(SrcDir)/opsys.as
	@$(DOALDOR) $(SrcDir)/opsys.as  $(AXLFLAGS)

$(LAXLLIB)(oslow.ao): $(SrcDir)/oslow.as
	@$(DOALDOR) $(SrcDir)/oslow.as $(AXLFLAGS)

$(LAXLLIB)(textwrit.ao): $(SrcDir)/textwrit.as 
	@$(DOALDOR) $(SrcDir)/textwrit.as $(AXLFLAGS)

$(LAXLLIB)(textread.ao): $(SrcDir)/textread.as 
	@$(DOALDOR) $(SrcDir)/textread.as  $(AXLFLAGS)

$(LAXLLIB)(fmtout.ao): $(SrcDir)/fmtout.as
	@$(DOALDOR) $(SrcDir)/fmtout.as  $(AXLFLAGS)

$(LAXLLIB)(table.ao): $(SrcDir)/table.as
	@$(DOALDOR) $(SrcDir)/table.as  $(AXLFLAGS)

$(LAXLLIB)(fprint.ao): $(SrcDir)/fprint.as
	@$(DOALDOR) $(SrcDir)/fprint.as $(AXLFLAGS) -Q inline-limit:2.4

$(LAXLLIB)(float.ao): $(SrcDir)/float.as
	@$(DOALDOR) $(SrcDir)/float.as $(AXLFLAGS) -Q inline-limit:2.4

$(LAXLLIB)(pfloat.ao): $(SrcDir)/pfloat.as
	@$(DOALDOR) $(SrcDir)/pfloat.as $(AXLFLAGS) -Q inline-limit:2.4

$(LAXLLIB)(ieeectl.ao): $(SrcDir)/ieeectl.as
	@$(DOALDOR) $(SrcDir)/ieeectl.as $(AXLFLAGS)

$(LAXLLIB)(object.ao): $(SrcDir)/object.as
	@$(DOALDOR) $(SrcDir)/object.as $(AXLFLAGS)

$(LAXLLIB)(debug.ao): $(SrcDir)/debug.as
	@$(DOALDOR) $(SrcDir)/debug.as $(AXLFLAGS)
