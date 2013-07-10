#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

# Targets:
#
# gmake release
#   makes the libaldor.al library with the .ao files (release)
#
# gmake debug
#   makes the libaldord.al library with the .ao files (debug)
#
# gmake doc
#   extracts all the .tex files from the sources
#
# gmake test
#   extracts all the test files from the sources

# gmake extract
#   makes the extract executable
#
# gmake aldoc2html
#   makes the aldoc2html executable
#
# External variables:
# ALDORLIBROOT: Root directory of libaldor
# ALDORROOT: Root directory of an aldor installation

ifeq ($(MACHINE), win32msvc)
	COMPILER=aldor.sh
else
	COMPILER=aldor
endif
ALDOR=$(COMPILER)

DocDir  = $(ALDORLIBROOT)/doc/tex
TestDir = $(ALDORLIBROOT)/test
LibDir  = $(ALDORLIBROOT)/lib
IncDir  = $(ALDORLIBROOT)/include

ALDORLIB=$(LibDir)/$(AldorLibName)

# Settings for variables
include Makefile.Rules

# Various default values
AldorLibName=libaldor

# All directories that require building	
DIRS =  lang base arith datastruc gmp util

TESTTARGS  = $(DIRS:%=%.test)
DOCTARGS   = $(DIRS:%=%.doc)
AOTARGS    = $(DIRS:%=%.aobj)
OBJTARGS   = $(DIRS:%=%.objects)
CLEANTARGS = $(DIRS:%=%.clean)

aldoc2html: flags.o aldoc2html.c
	$(CC) -o aldoc2html aldoc2html.c flags.o

extract: flags.o extract.c
	$(CC) -o extract extract.c flags.o

flags.o: flags.c flags.h
	$(CC) -c flags.c

release:
	$(MAKE) aobj0 AldorLibName=$(AldorLibName).al FLAGS="$(AOFLAGS)"

debug:
	$(MAKE) aobj0 AldorLibName=$(AldorLibName)d.al FLAGS="$(AODFLAGS)"

doc: $(DOCTARGS)

test: $(TESTTARGS)

clean: $(CLEANTARGS)

SUBFLAGS = FLAGS="$(FLAGS)" OFLAGS="$(OFLAGS)" \
           LibDir=$(LibDir) ALDORLIBROOT=$(ALDORLIBROOT) \
	   TestDir=$(TestDir) DocDir=$(DocDir) \
           ALDORLIB=$(ALDORLIB)

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

