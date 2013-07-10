#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

ThisDir = multpoly_multpolydata

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

SRCS = alg_bivarpk alg_ZpUVres alg_mresbiv 

# directories that we depend on
DEPDIRS = multpoly_multpolydom

NOTYET = alg_disolve alg_mhensel alg_ezgcd

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

${ALGEBRA}(alg_bivarpk.ao):	        ${ALGEBRA}(sit_upolc.ao)
${ALGEBRA}(alg_ZpUVres.ao):             ${ALGEBRA}(alg_bivarpk.ao)
${ALGEBRA}(alg_mresbiv.ao):             ${ALGEBRA}(alg_ZpUVres.ao)
# ${ALGEBRA}(alg_disolve.ao):             ${ALGEBRA}(alg_sup.ao) ${ALGEBRA}(alg_unitool.ao)
# ${ALGEBRA}(alg_mhensel.ao):             ${ALGEBRA}(alg_disolve.ao)
# ${ALGEBRA}(alg_ezgcd.ao):               ${ALGEBRA}(alg_mhensel.ao)
