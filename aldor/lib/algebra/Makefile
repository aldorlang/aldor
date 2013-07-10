##############################################################################
#
# Makefile
#
# ASSUME there is an ALGEBRAROOT variable sets
# to the directory containing bin include/ lib/ src/ test/
# from the libalgebra distribution.
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

SHELL=/bin/sh
# SHELL=/usr/bin/bash

# Files location
OKFILES = Makefile bin include lib src test

ifeq "X$(ALGEBRAROOT)" "X"
ALGEBRAROOT=$(PWD)
export ALGEBRAROOT
endif

# Destination directory taken from top-level Makefile
# uncomment to modify.
ifeq "X$(GENERIC_DIR)" "X"
GENERIC_DIR=$(ALGEBRAROOT)
export GENERIC_DIR
endif

ifeq "X$(ALDORLIBROOT)" "X"
ALDORLIBROOT=$(GENERIC_DIR)/lib/aldor
export ALDORLIBROOT
endif

ifeq "X$(DEBUGLIBROOT)" "X"
DEBUGLIBROOT=$(GENERIC_DIR)/lib/debuglib
export DEBUGLIBROOT
endif

INCPATH=$(ALDORLIBROOT)/include:$(ALGEBRAROOT)/include:$(DEBUGLIBROOT)/include
export INCPATH
LIBPATH=$(ALDORLIBROOT)/lib:$(ALGEBRAROOT)/lib:$(DEBUGLIBROOT)/lib
export LIBPATH

# Directories
SRCDIR = $(ALGEBRAROOT)/src
LIBDIR = $(ALGEBRAROOT)/lib
INCDIR = $(ALGEBRAROOT)/include
BINDIR = $(ALGEBRAROOT)/bin
TESTDIR = $(ALGEBRAROOT)/test

# ----------------------------------------------------------------
# MAIN TARGETS
# ----------------------------------------------------------------

all:
	@echo "Usage:"
	@echo "   make portable => make portable versions of the library (.al)"
	@echo "   make objects  => create binary version from portable version"
	@echo "   make library  => build both portable and binary version"

portable: cleanall init checksrc checklibaldor aofiles

objects:
	@$(MAKE) -f Makefile.generic objects

library: portable objects

generic: checkgeneric msggeneric portable
	@(mkdir -p $(GENERIC_DIR)/lib/algebra/bin; \
	  mkdir -p $(GENERIC_DIR)/lib/algebra/lib; \
	  mkdir -p $(GENERIC_DIR)/lib/algebra/include; \
	  mkdir -p $(GENERIC_DIR)/doc/algebra; \
	  cp $(BINDIR)/*              $(GENERIC_DIR)/lib/algebra/bin; \
	  cp $(LIBDIR)/*.al           $(GENERIC_DIR)/lib/algebra/lib; \
	  cp $(INCDIR)/*.as           $(GENERIC_DIR)/lib/algebra/include; \
	  cp Makefile.generic         $(GENERIC_DIR)/lib/algebra/Makefile; \
	  echo "*** Generic distribution for algebra is done ***")

msggeneric:
	@echo "*********************************"
	@echo "Making generic version of algebra"
	@echo "*********************************"

# ----------------------------------------------------------------
# CLEAN
# ----------------------------------------------------------------

clean: cleanall

cleanall: cleanlib cleansrc

cleanlib:
	-@ ( cd $(LIBDIR); rm -f libalgebra*.al stamp*; )

cleansrc:
	-@ ( cd $(BINDIR); $(SHELL) ./cleansrc.sh; )

# ----------------------------------------------------------------
# BUILD
# ----------------------------------------------------------------

checkgeneric:
	-@ if [ "x$(GENERIC_DIR)" = "x" ] ; then echo "*** GENERIC_DIR unset"; fi
	@ [ "x$(GENERIC_DIR)" != "x" ]

checklibaldor:
	-@ if [ ! -f $(ALDORLIBROOT)/lib/libaldor.al ] ; \
	   then echo "libaldor should be compiled before algebra can be compiled"; fi
	@ [ -f $(ALDORLIBROOT)/lib/libaldor.al ]

checksrc :
	-@ ( cd $(BINDIR); $(SHELL) ./checksrc.sh $(ALGEBRAROOT); )

aofiles : releaseao debugao gmpao

releaseao :
	@(cd $(SRCDIR); echo "* Making release version *"; $(MAKE) release)
debugao :
	@(cd $(SRCDIR); echo "* Making debug version *"; $(MAKE) debug)
gmpao :
	@(cd $(SRCDIR); echo "* Making gmp version *"; $(MAKE) gmp)

# ----------------------------------------------------------------
# Tests
# ----------------------------------------------------------------

testfiles :
	-@ ( cd $(BINDIR); sh ./maketestsuite.sh $(ALGEBRAROOT); )

runtest :
	-@ ( cd $(BINDIR); sh ./runtest.sh ; )


# ----------------------------------------------------------------
# Developer special targets.
# ----------------------------------------------------------------

install: library
	-@ ( cd $(BINDIR); sh ./makeinstall.sh ; )

uninstall:
	-@ ( cd $(BINDIR); $(SHELL) ./makeuninstall.sh ; )

junk:

samples:

init:
	@if [ ! -d $(LIBDIR) ]; then mkdir -p $(LIBDIR); fi



