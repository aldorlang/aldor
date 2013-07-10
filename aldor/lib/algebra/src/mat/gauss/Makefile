#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

ThisDir = mat_gauss

COMPILER=aldor
LibDir = $(ALGEBRAROOT)/lib
IncDir = $(ALGEBRAROOT)/include
DocDir = $(ALGEBRAROOT)/doc/tex
TestDir = $(ALGEBRAROOT)/test
StampDir  = $(LibDir)
DocStampDir  = $(DocDir)
TestStampDir  = $(TestDir)

include ../../Makefile.Rules

ALDOR=${COMPILER} ${FLAGS}

.PRECIOUS:

SRCS = sit_linelim sit_oge sit_dfge sit_hermge sit_ffge sit_ff2ge

# directories that we depend on
DEPDIRS = mat

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

${ALGEBRA}(sit_linelim.ao):	${ALGEBRA}(sit_matcat.ao)
${ALGEBRA}(sit_oge.ao):	${ALGEBRA}(sit_linelim.ao) ${ALGEBRA}(sit_permut.ao) ${ALGEBRA}(sit_field.ao)
${ALGEBRA}(sit_dfge.ao):	${ALGEBRA}(sit_linelim.ao) ${ALGEBRA}(sit_permut.ao)
${ALGEBRA}(sit_hermge.ao):${ALGEBRA}(sit_linelim.ao) ${ALGEBRA}(sit_permut.ao) ${ALGEBRA}(sit_euclid.ao)
${ALGEBRA}(sit_ffge.ao):	${ALGEBRA}(sit_linelim.ao) ${ALGEBRA}(sit_permut.ao) ${ALGEBRA}(sit_intdom.ao)
${ALGEBRA}(sit_ff2ge.ao):	${ALGEBRA}(sit_linelim.ao) ${ALGEBRA}(sit_permut.ao) ${ALGEBRA}(sit_intdom.ao)
