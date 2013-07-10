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
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

SHELL	= /bin/sh
AXL = aldor
ifeq ($(MACHINE), win32gcc)
	AXL=aldor.sh
endif

ifeq ($(MACHINE), win32msvc)
	AXL=aldor.sh
endif

#
# Environment
#
IncDir	= $(shell pwd)/include
LibDir	= $(shell pwd)/lib
BinDir	= $(shell pwd)/bin
SrcDir  = $(shell pwd)/src
TestDir = $(shell pwd)/test

LDEBUGLIB	= libdebuglib.al
LOBLIB	= libdebuglib.a
BASELIB = UseAldorlib

DOALDOR = $(BinDir)/makedebuglibal.sh
DORANLIB = $(BinDir)/doranlib

CP	= cp -p
RM	= rm -f


ifeq "X$(ALDORLIBROOT)" "X"
ALDORLIBROOT=$(GENERIC_DIR)/lib/aldor
export ALDORLIBROOT
endif

INCPATH=$(ALDORLIBROOT)/include
export INCPATH
LIBPATH=$(ALDORLIBROOT)/lib
export LIBPATH

# Aldor compilation flags.
# IMPORTANT: never compile this debugging library with -Wdebugger!
XFLAGS = -Fx
#OFLAGS = -Q2 -Qno-del-assert
PFLAGS = -Qno-cc -Zdb -Cargs=-Wopts=-pg 
DFLAGS = -Qno-cc -Qno-del-assert -Cargs=-Wopts=-g -Fc -Ffm 
LFLAGS = -Csmax=0 -I $(INCPATH) -Y $(LIBPATH) -R $(SrcDir) -D $(BASELIB)
MFLAGS = -Mno-mactext \
	 -Mno-ALDOR_W_CantUseArchive \
	 -Mno-ALDOR_W_OverRideLibraryFile \
	 -Mno-ALDOR_W_FunnyJuxta \
	 -Mno-ALDOR_W_GenDomFunNotConst
OFLAGS = -Qno-del-assert


AFLAGS = $(MFLAGS) $(OFLAGS) $(DFLAGS) $(LFLAGS)


# Define the names of the libraries and header file
LIB = debuglib
LIBHD = $(IncDir)/$(LIB).as
LIBAL = lib$(LIB).al
LIBA  = lib$(LIB).a


# Level X sources depend on sources in level X-1 and below. Level 0
# sources have no dependencies.
LEVEL00 = $(SrcDir)/dbg_empty.as


# Domains and other definitions designed to provide a uniform view
# of the base library (Axllib, Basicmath etc) for the rest of the
# debugger libraries.
LEVEL01 = 


# Utility domains which form the base of the library.
LEVEL02 = \
		$(SrcDir)/dbg_ltools.as	\
		$(SrcDir)/dbg_exts.as	\
		$(SrcDir)/dbg_gener.as	\
		$(SrcDir)/dbg_var.as	\
		$(SrcDir)/dbg_fmt.as	\
		$(SrcDir)/dbg_file_idx.as \
		$(SrcDir)/dbg_bpoint.as	\
		$(SrcDir)/dbg_help.as	\
		$(SrcDir)/dbg_utils.as	\


# Calling contexts need to use the format operations of dbg_fmt.as
# while the file table uses dbg_file_idx.as.
LEVEL03 = \
                $(SrcDir)/dbg_istack.as \
		$(SrcDir)/dbg_context.as	\
		$(SrcDir)/dbg_file_tbl.as	\
 
# The internal debugger state uses various fles from levels 1 and 2.
LEVEL04 = $(SrcDir)/dbg_state.as


# Breakpoint handling relies heavily on the debugger state.
LEVEL05 = $(SrcDir)/dbg_ui_break.as


# The debugger user interface (UI) uses all levels.
LEVEL06 = $(SrcDir)/dbg_ui.as


# This is the top-level domain that is exported to clients.
LEVEL07 = $(SrcDir)/dbg_core.as


# All the library source files
SRCS = \
	$(LEVEL00) \
	$(LEVEL01) \
	$(LEVEL02) \
	$(LEVEL03) \
	$(LEVEL04) \
	$(LEVEL05) \
	$(LEVEL06) \
	$(LEVEL07) \


# List of all the library FOAM and OBJ files
AOFILES = $(SRCS:%.as=%.ao)
OFILES = $(AOFILES:%.ao=%.o)
CFILES = $(AOFILES:%.ao=%.c)
FMFILES = $(AOFILES:%.ao=%.fm)


# Library targets
OBRACK = (
CBRACK = )
LIBAOS = $(AOFILES:%=$(LIBAL)$(OBRACK)%$(CBRACK))
LIBOS = $(OFILES:%=$(LIBA)$(OBRACK)%$(CBRACK))


#
# Target lists
#
#IncTargs = $(IncDir)/debuglib.as
#LibTargs = $(LDEBUGLIB) $(LOBLIB)

# Don't delete the libraries if a compilation fails.
#.PRECIOUS: $(LibTargs)

#
# Generic targets
#
OKFILES	 = Makefile $(Sources)

DESTDIR=$(GENERIC_DIR)/lib/debuglib

# ----------------------------------------------------------------
# MAIN TARGETS
# ----------------------------------------------------------------

all:
	@echo "Usage:"
	@echo "   make portable => make portable versions of the library (.al)"
	@echo "   make objects  => create binary version from portable version"
	@echo "   make library  => build both portable and binary version"

portable: clean libclean $(LDEBUGLIB)

objects: 
	@($(BinDir)/makedebugliba.sh $(LDEBUGLIB))

library: portable objects

generic: checkgeneric msggeneric portable
	@(mkdir -p $(DESTDIR)/bin; \
	  mkdir -p $(DESTDIR)/lib; \
	  mkdir -p $(DESTDIR)/src; \
	  mkdir -p $(DESTDIR)/include; \
	  mv $(LDEBUGLIB)            $(DESTDIR)/lib; \
	  cp $(IncDir)/debuglib.as   $(DESTDIR)/include; \
	  cp $(BinDir)/makedebugliba.sh $(DESTDIR)/bin; \
	  cp Makefile.generic      $(DESTDIR)/Makefile; \
	  echo "*** Generic distribution for libdebuglib is done ***")

checkgeneric:
	-@ if [ "x$(GENERIC_DIR)" = "x" ] ; then echo "*** GENERIC_DIR unset"; fi
	@ [ "x$(GENERIC_DIR)" != "x" ]

msggeneric:
	@echo "***********************************"
	@echo "Making generic version of libdebuglib"
	@echo "***********************************"

# ----------------------------------------------------------------
# CLEAN
# ----------------------------------------------------------------

clean:
	@ $(RM) *~ $(AOFILES) $(CFILES) $(FMFILES) $(OFILES)

libclean:
	@ $(RM) $(LDEBUGLIB) $(LOBLIB)

# ----------------------------------------------------------------
# MISC
# ----------------------------------------------------------------

# all:	$(IncTargs) $(LibTargs) 
#include:	$(IncTargs)
#libdebuglib:	$(LDEBUGLIB) $(LOBLIB)
junk:
	@ ls -d $(OKFILES) * | sort | grep -v CVS | uniq -u

# List of all the library FOAM and OBJ files
AOFILES = $(SRCS:%.as=%.ao)
OFILES = $(AOFILES:%.ao=%.o)
CFILES = $(AOFILES:%.ao=%.c)
FMFILES = $(AOFILES:%.ao=%.fm)

# Library targets
OBRACK = (
CBRACK = )
LIBAOS = $(AOFILES:%=$(LIBAL)$(OBRACK)%$(CBRACK))
LIBOS = $(OFILES:%=$(LIBA)$(OBRACK)%$(CBRACK))


# Rule for building FOAM objects from Aldor source
%.ao %.o : %.as
	$(AXL) -Fao -Csmax=0 $(AFLAGS) $<

# Rule for building executables
% : %.as
	$(AXL) $(XFLAGS) $(AFLAGS) $<

# Library dependencies
$(LDEBUGLIB): $(LIBAOS)
$(LOBLIB): $(LDEBUGLIB) $(LIBOS)

# Dependencies between levels for FOAM files. The first rule is a
# complete lie but it forces the library to be rebuilt if the main
# header file is modified.
 
$(LEVEL00:%.as=%.ao) : 
$(LEVEL01:%.as=%.ao) : $(LEVEL00:%.as=$(LIBAL)$(OBRACK)%.ao$(CBRACK))
$(LEVEL02:%.as=%.ao) : $(LEVEL01:%.as=$(LIBAL)$(OBRACK)%.ao$(CBRACK))
$(LEVEL03:%.as=%.ao) : $(LEVEL02:%.as=$(LIBAL)$(OBRACK)%.ao$(CBRACK))
$(LEVEL04:%.as=%.ao) : $(LEVEL03:%.as=$(LIBAL)$(OBRACK)%.ao$(CBRACK))
$(LEVEL05:%.as=%.ao) : $(LEVEL04:%.as=$(LIBAL)$(OBRACK)%.ao$(CBRACK))
$(LEVEL06:%.as=%.ao) : $(LEVEL05:%.as=$(LIBAL)$(OBRACK)%.ao$(CBRACK))
$(LEVEL07:%.as=%.ao) : $(LEVEL06:%.as=$(LIBAL)$(OBRACK)%.ao$(CBRACK))

#
# Specific dependencies and rules
#
#$(IncDir)/debuglib.as: $(SrcDir)/debuglib.as
#	$(CP) $(SrcDir)/debuglib.as $(IncDir)/debuglib.as
