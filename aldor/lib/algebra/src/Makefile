# Targets:
#
# gmake gmp
#   makes the libalgebra-gmp.al library with the .ao files
#
# gmake release
#   makes the libalgebra.al library with the .ao files
#
# gmake debug
#   makes the libalgebrad.al library with the .ao files
#
# gmake doc
#   extracts all the .tex files from the sources
#
# gmake test
#   extracts all the test files from the sources
#
# External variables
#
# ALGEBRAROOT: Root directory of libalgebra
#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

COMPILER=$(ALDORROOT)/bin/aldor

DocDir  = $(ALGEBRAROOT)/doc/tex
TestDir = $(ALGEBRAROOT)/test
LibDir  = $(ALGEBRAROOT)/lib
IncDir  = $(ALGEBRAROOT)/include
BinDir  = $(ALGEBRAROOT)/bin

ALGEBRA=$(LibDir)/$(AlgLibName)

# Settings for variables
include Makefile.Rules

# Various default values
AlgLibName=libalgebra

# All directories that require building
DIRS = util numbers extree extree/operators extree/parser categories basic\
	basic/compbug\
	mat mat/gauss mat/modular\
	mat/modular/compbug\
	mat/linalg\
	multpoly/multpolydata multpoly/exponent\
	univpoly/categories mat/linalg2 univpoly univpoly/gcd fraction series\
	series/compbug\
	mat/linalg3 algext polyfactorp ffield polyfactor0\
	multpoly/multpolycat multpoly/multpolydom multpoly/multpolypkg\
	multpoly/multpolytest

TESTTARGS  = $(DIRS:%=%.test)
DOCTARGS   = $(DIRS:%=%.doc)
AOTARGS    = $(DIRS:%=%.aobj)
OBJTARGS   = $(DIRS:%=%.objects)
CLEANTARGS = $(DIRS:%=%.clean)

gmp:
	$(MAKE) aobj0 AlgLibName=$(AlgLibName)-gmp.al FLAGS="$(AOGFLAGS)"

release:
	$(MAKE) aobj0 AlgLibName=$(AlgLibName).al FLAGS="$(AOFLAGS)"

debug:
	$(MAKE) aobj0 AlgLibName=$(AlgLibName)d.al FLAGS="$(AODFLAGS)"

debugger:
	$(MAKE) aobj0 AlgLibName=$(AlgLibName)dbg.al FLAGS="$(AODEBUGGERFLAGS)"

aofiles : release gmp debug debugger

objects :
	-@ ( cd $(BinDir); sh ./makealgebra.sh $(ALGEBRAROOT) libalgebra.al '-q5 -qinline-all' ; sh ./makealgebra.sh $(ALGEBRAROOT) libalgebra-gmp.al '-q5 -qinline-all -dGMP'; sh ./makealgebra.sh $(ALGEBRAROOT) libalgebrad.al '-q1 -dDEBUG'; )

doc: $(DOCTARGS)

test: $(TESTTARGS)

clean: $(CLEANTARGS)

SUBFLAGS = FLAGS="$(FLAGS)" OFLAGS="$(OFLAGS)" \
           LibDir=$(LibDir) ALGEBRAROOT=$(ALGEBRAROOT) \
	   TestDir=$(TestDir) DocDir=$(DocDir) \
           ALGEBRA=$(ALGEBRA)

SUBMAKE = $(MAKE) $(SUBFLAGS)

aobj0: $(AOTARGS)

%.include: $(IncDir)
	@(cd $*; $(SUBMAKE) include)

%.aobj:
	@(cd $*; $(SUBMAKE) aobj)

%.doc: $(DocDir)
	@(cd $*; $(SUBMAKE) doc)

%.test: $(TestDir)
	@(cd $*; $(SUBMAKE) test)

%.clean: 
	@(cd $*; $(SUBMAKE) clean)

