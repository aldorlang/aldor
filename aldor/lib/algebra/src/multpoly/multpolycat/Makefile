#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

ThisDir = multpoly_multpolycat

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

SRCS = sm_polring0 sm_famr0 sm_polring sm_rmpcat0 alg_defgcd\
	alg_stdfrng alg_poltype

# directories that we depend on
DEPDIRS = multpoly_exponent

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
 
${ALGEBRA}(sm_polring0.ao): ${ALGEBRA}(sit_freealg.ao) ${ALGEBRA}(alg_poltype.ao)
${ALGEBRA}(alg_defgcd.ao):      ${ALGEBRA}(sm_polring0.ao)
${ALGEBRA}(sm_famr0.ao):        ${ALGEBRA}(sm_polring0.ao) ${ALGEBRA}(sm_expocat.ao)
${ALGEBRA}(sm_polring.ao):      ${ALGEBRA}(alg_defgcd.ao) ${ALGEBRA}(sm_expocat.ao) ${ALGEBRA}(sm_polring0.ao)
${ALGEBRA}(sm_rmpcat0.ao):      ${ALGEBRA}(sm_polring.ao)
${ALGEBRA}(alg_stdfrng.ao):	${ALGEBRA}(alg_frering.ao)
${ALGEBRA}(alg_poltype.ao):	${ALGEBRA}(alg_stdfrng.ao)
