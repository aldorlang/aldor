#
# External variables:
# ALDORLIBROOT: Root directory of libaldor
#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

COMPILER=$(ALDORROOT)/bin/aldor
ifeq ($(MACHINE), win32msvc)
	COMPILER=aldor.sh
endif
ifeq ($(MACHINE), win32gcc)
	COMPILER=aldor.sh
endif
ALDOR=$(COMPILER)

# Main directory for the sources of the library
ifeq "X$(ALDORLIBROOT)" "X"
ALDORLIBROOT=$(shell pwd)
export ALDORLIBROOT
endif

# Destination directory taken from top-level Makefile
# uncomment to modify.
# GENERIC_DIR=
ifeq "X$(GENERIC_DIR)" "X"
GENERIC_DIR=$(ALDORLIBROOT)
export GENERIC_DIR
endif

# Directories
LibDir   = $(ALDORLIBROOT)/lib
SrcDir   = $(ALDORLIBROOT)/src
BinDir   = $(ALDORLIBROOT)/bin
IncDir   = $(ALDORLIBROOT)/include
TestDir  = $(ALDORLIBROOT)/test

export BinDir
export LibDir
export IncDir
export SrcDir

DIRS = test src
CLEANTARGS = $(DIRS:%=%.clean)

DESTDIR=$(GENERIC_DIR)/lib/aldor

# ----------------------------------------------------------------
# MAIN TARGETS
# ----------------------------------------------------------------

all:
	@echo "Usage:"
	@echo "   make portable => make portable versions of the library (.al)"
	@echo "   make objects  => create binary version from portable version"
	@echo "   make library  => build both portable and binary version"

portable: cleanall init release debug gloop

objects:
	@echo "************** Make objects **************"
	@(LIBDIR=$(LibDir); export LIBDIR; $(MAKE) -f Makefile.generic -e COMPILER=$(COMPILER) objects)

library: portable objects
	cp include/* $(ALDORROOT)/include
	cp lib/* $(ALDORROOT)/lib

generic: checkgeneric msggeneric portable
	@(mkdir -p $(DESTDIR)/bin; \
	  mkdir -p $(DESTDIR)/lib; \
	  mkdir -p $(DESTDIR)/include; \
	  mkdir -p $(DESTDIR)/src/util; \
	  cp $(BinDir)/*                     $(DESTDIR)/bin; \
	  cp $(LibDir)/*.al                  $(DESTDIR)/lib; \
	  cp $(LibDir)/aldor_gloop*.ao       $(DESTDIR)/lib; \
	  cp $(IncDir)/*.as                  $(DESTDIR)/include; \
	  cp src/util/sal_util.c $(DESTDIR)/src/util; \
	  cp Makefile.generic $(DESTDIR)/Makefile; \
	  echo "*** Generic distribution for libaldor is done ***")

checkgeneric:
	-@ if [ "x$(GENERIC_DIR)" = "x" ] ; then echo "*** GENERIC_DIR unset"; fi
	@ [ "x$(GENERIC_DIR)" != "x" ]

msggeneric:
	@echo "**********************************"
	@echo "Making generic version of libaldor"
	@echo "**********************************"

# ----------------------------------------------------------------
# CLEAN
# ----------------------------------------------------------------

clean: $(CLEANTARGS)
	@(rm -f *~)
	-@(rm -f $(LibDir)/* )

cleanall: clean

%.clean: 
	@(cd $*; $(MAKE) clean)

# ----------------------------------------------------------------
# BUILD
# ----------------------------------------------------------------

gloop:
	@echo "************** Building gloop.ao and gloopd.ao **************";
	$(ALDOR) -R $(LibDir) -I $(IncDir) -Y $(LibDir) -Q5 $(SrcDir)/aldor_gloop.as
	$(ALDOR) -R $(LibDir) -I $(IncDir) -Y $(LibDir) -Q1 -dDEBUG $(SrcDir)/aldor_gloopd.as

release:
	@echo "************** Release build **************"
	@(cd $(SrcDir); $(MAKE) release)

debug:
	@echo "************** Debug build **************"
	@(cd $(SrcDir); $(MAKE) debug)

runtest:
	@(cd $(SrcDir); $(MAKE) test)
	@(cd $(TestDir); $(MAKE) all; $(MAKE) clean)

# ----------------------------------------------------------------
# EXTRAS
# ----------------------------------------------------------------

copydoc: buildtutorial # builddoc
	@(mkdir -p $(GENERIC_DIR)/doc/libaldor/tutorial; \
	  mkdir -p $(GENERIC_DIR)/doc/libaldor/doc; \
	  cp tutorial/*.as tutorial/tutorial.pdf $(GENERIC_DIR)/doc/libaldor/tutorial )

buildtutorial:
	@(cd tutorial; pdflatex tutorial.tex)

# builddoc:

copytests:
	@(mkdir -p $(GENERIC_DIR)/tests/libaldor; \
	  cp test/*.as test/testall test/Makefile $(GENERIC_DIR)/tests/libaldor )

init:
	@if [ ! -d $(LibDir) ]; then mkdir -p $(LibDir); fi
