#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

ThisDir = gmp

COMPILER=$(ALDORROOT)/bin/aldor
LibDir = $(ALDORLIBROOT)/lib
IncDir = $(ALDORLIBROOT)/include
DocDir = $(ALDORLIBROOT)/doc/tex
TestDir = $(ALDORLIBROOT)/test
StampDir  = $(LibDir)
DocStampDir  = $(DocDir)
TestStampDir  = $(TestDir)

include ../Makefile.Rules

ALDOR=${COMPILER} ${FLAGS}

.PRECIOUS:

SRCS = sal_intgmp sal_fltgmp sal_gmptls

# directories that we depend on
DEPDIRS = datastruc

NOTYET =

OBRACK = (
CBRACK = )

SRCAS   = $(SRCS:%=%.as)
AOTARGS = $(SRCS:%=$(ALDORLIB)$(OBRACK)%.ao$(CBRACK))
DOCTARGS = $(SRCS:%=$(DocDir)/%.tex)
TESTTARGS = $(SRCS:%=$(TestDir)/%.test.as)
DEPLST  = $(DEPDIRS:%=$(StampDir)/stamp-%)
STAMP = $(StampDir)/stamp-$(ThisDir)
DOCSTAMP = $(DocStampDir)/stamp-$(ThisDir)
TESTSTAMP = $(TestStampDir)/stamp-$(ThisDir)

OKFILES = Makefile $(SRCAS) $(NOTYET)

test: $(TESTSTAMP)

doc: $(DOCSTAMP)

aobj: $(STAMP)

clean:
	-@rm -f *.c *.h *.fm *.ao *.o *~

$(TESTSTAMP): $(TESTTARGS)
	touch $(TESTSTAMP)

$(DOCSTAMP): $(DOCTARGS)
	touch $(DOCSTAMP)

$(STAMP): $(AOTARGS)
	touch $(STAMP)

# Explicit dependencies
OD = $(DEPLST)

${ALDORLIB}(sal_gmptls.ao):	${ALDORLIB}(sal_parray.ao)
${ALDORLIB}(sal_intgmp.ao):	${ALDORLIB}(sal_int.ao) ${ALDORLIB}(sal_gmptls.ao) ${ALDORLIB}(sal_string.ao)
${ALDORLIB}(sal_fltgmp.ao): ${ALDORLIB}(sal_intgmp.ao) ${ALDORLIB}(sal_dfloat.ao)
