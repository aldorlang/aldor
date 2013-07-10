#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

ThisDir = arith

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

SRCS = sal_arith sal_intcat sal_bool sal_bsearch sal_itools\
	sal_mint sal_int sal_pointer sal_fltcat sal_ftools sal_sfloat\
	sal_dfloat sal_segment sal_random sal_binpow sal_oarith\
	sal_lincomb sal_complex

# directories that we depend on
DEPDIRS = base

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

${ALDORLIB}(sal_bool.ao):		${ALDORLIB}(sal_torder.ao)
${ALDORLIB}(sal_arith.ao):		${ALDORLIB}(sal_bool.ao)
${ALDORLIB}(sal_oarith.ao):	${ALDORLIB}(sal_arith.ao)
${ALDORLIB}(sal_intcat.ao):	${ALDORLIB}(sal_oarith.ao)
${ALDORLIB}(sal_bsearch.ao):	${ALDORLIB}(sal_intcat.ao)
${ALDORLIB}(sal_segment.ao):	${ALDORLIB}(sal_intcat.ao)
${ALDORLIB}(sal_binpow.ao):	${ALDORLIB}(sal_intcat.ao)
${ALDORLIB}(sal_itools.ao):${ALDORLIB}(sal_bsearch.ao) ${ALDORLIB}(sal_segment.ao)
${ALDORLIB}(sal_mint.ao):	${ALDORLIB}(sal_itools.ao) ${ALDORLIB}(sal_binpow.ao)
${ALDORLIB}(sal_random.ao):	${ALDORLIB}(sal_mint.ao)
${ALDORLIB}(sal_int.ao):		${ALDORLIB}(sal_random.ao)
${ALDORLIB}(sal_pointer.ao):	${ALDORLIB}(sal_random.ao)
${ALDORLIB}(sal_fltcat.ao):	${ALDORLIB}(sal_int.ao)
${ALDORLIB}(sal_ftools.ao):	${ALDORLIB}(sal_fltcat.ao)
${ALDORLIB}(sal_sfloat.ao):	${ALDORLIB}(sal_ftools.ao)
${ALDORLIB}(sal_dfloat.ao):	${ALDORLIB}(sal_sfloat.ao)
${ALDORLIB}(sal_lincomb.ao):	${ALDORLIB}(sal_arith.ao)
${ALDORLIB}(sal_complex.ao):	${ALDORLIB}(sal_lincomb.ao) ${ALDORLIB}(sal_fltcat.ao)
