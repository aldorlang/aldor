#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

ThisDir = univpoly

COMPILER=aldor
LibDir = $(ALGEBRAROOT)/lib
IncDir = $(ALGEBRAROOT)/include
DocDir = $(ALGEBRAROOT)/doc/tex
TestDir = $(ALGEBRAROOT)/test
StampDir  = $(LibDir)
DocStampDir  = $(DocDir)
TestStampDir  = $(TestDir)

include ../Makefile.Rules

ALDOR=${COMPILER} ${FLAGS}

.PRECIOUS:

SRCS = sit_polkara alg_sup0 alg_sup1 alg_sup sit_dup sit_ufacpol alg_uprcr sit_upolc\
	 sit_spread alg_unitool

# directories that we depend on
DEPDIRS = univpoly/categories

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

${ALGEBRA}(sit_polkara.ao):	${ALGEBRA}(sit_comring.ao)
${ALGEBRA}(alg_sup0.ao):        ${ALGEBRA}(sit_idxfmod.ao)
${ALGEBRA}(alg_sup1.ao):        ${ALGEBRA}(alg_sup0.ao)
${ALGEBRA}(alg_sup.ao):         ${ALGEBRA}(alg_sup1.ao) ${ALGEBRA}(sit_spread.ao)
${ALGEBRA}(sit_dup.ao):		${ALGEBRA}(sit_polkara.ao) ${ALGEBRA}(sit_spread.ao)
${ALGEBRA}(sit_ufacpol.ao):	${ALGEBRA}(sit_dup.ao)
${ALGEBRA}(sit_upolc.ao):       ${ALGEBRA}(sit_fring.ao) ${ALGEBRA}(sit_ugring.ao) ${ALGEBRA}(sit_sqfree.ao) ${ALGEBRA}(sit_resprs.ao) ${ALGEBRA}(alg_ffupla.ao) ${ALGEBRA}(alg_uprcr.ao)
${ALGEBRA}(sit_spread.ao):	${ALGEBRA}(sit_upolc.ao)
${ALGEBRA}(alg_uprcr.ao):       ${ALGEBRA}(alg_rescls.ao) ${ALGEBRA}(sit_upolc0.ao)
${ALGEBRA}(alg_unitool.ao):     ${ALGEBRA}(sit_upolc0.ao) 
