#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

ThisDir = multpoly_multpolytest

COMPILER=aldor
LibDir = $(ALGEBRAROOT)/lib
IncDir = $(ALGEBRAROOT)/include
DocDir = $(ALGEBRAROOT)/doc/tex
TestDir = $(ALGEBRAROOT)/test
StampDir  = $(LibDir)
DocStampDir  = $(DocDir)
TestStampDir  = $(TestDir)
PSTARGS = $(SRCS:%=%.ps) 

include ../../Makefile.Rules

ALDOR=${COMPILER} ${FLAGS}

.PRECIOUS:

SRCS = sm_exppkgt sm_dmp0pkgt sm_pr0pkgt alg_bivtst1 alg_bivtst2 alg_bivtst3

# directories that we depend on
DEPDIRS = multpoly_multpolydom

NOTYET =

OBRACK = (
CBRACK = )

SRCAS   = $(SRCS:%=%.as)
AOTARGS = $(SRCS:%=$(ALGEBRA)$(OBRACK)%.ao$(CBRACK))
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

ps : $(PSTARGS)

%.ps :
	a2ps -o `basename $*`.ps `basename $*`.as

clean:
	-@rm -f *.c *.h *.fm *.ao *.o *~ *.ps

$(TESTSTAMP): $(TESTTARGS)
	touch $(TESTSTAMP)

$(DOCSTAMP): $(DOCTARGS)
	touch $(DOCSTAMP)

$(STAMP): $(AOTARGS)
	touch $(STAMP)

# Explicit dependencies
OD = $(DEPLST)

${ALGEBRA}(sm_exppkgt.ao):	        ${ALGEBRA}(sm_expocat.ao)
${ALGEBRA}(sm_dmp0pkgt.ao):	        ${ALGEBRA}(sm_dmp0.ao)
${ALGEBRA}(sm_pr0pkgt.ao):	        ${ALGEBRA}(sm_polring0.ao)
${ALGEBRA}(alg_bivtst1.ao):             ${ALGEBRA}(alg_mresbiv.ao)
${ALGEBRA}(alg_bivtst2.ao):             ${ALGEBRA}(alg_mresbiv.ao)
${ALGEBRA}(alg_bivtst3.ao):             ${ALGEBRA}(alg_mresbiv.ao)
