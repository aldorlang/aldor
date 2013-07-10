#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

ThisDir = fraction

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

SRCS = sit_quotcat sit_qotfct0 sit_lcqotct sit_vecquot sit_matquot sit_uflgqot\
	sit_qotfcat sit_qotient sit_qotbyc0 sit_qotbyct sit_quotby

# directories that we depend on
DEPDIRS = univpoly

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

${ALGEBRA}(sit_quotcat.ao): ${ALGEBRA}(sit_laring.ao) ${ALGEBRA}(sit_diffext.ao) ${ALGEBRA}(sit_zring.ao) ${ALGEBRA}(sit_fring.ao) ${ALGEBRA}(sit_gcdint.ao)
${ALGEBRA}(sit_qotbyc0.ao):	${ALGEBRA}(sit_quotcat.ao)
${ALGEBRA}(sit_qotfct0.ao):	${ALGEBRA}(sit_quotcat.ao)
${ALGEBRA}(sit_lcqotct.ao):${ALGEBRA}(sit_qotfct0.ao) ${ALGEBRA}(sit_qotbyc0.ao)
${ALGEBRA}(sit_vecquot.ao):	${ALGEBRA}(sit_lcqotct.ao) ${ALGEBRA}(sit_vector.ao)
${ALGEBRA}(sit_matquot.ao):	${ALGEBRA}(sit_vecquot.ao) ${ALGEBRA}(sit_matcat.ao)
${ALGEBRA}(sit_uflgqot.ao):	${ALGEBRA}(sit_lcqotct.ao) ${ALGEBRA}(sit_uffalg.ao)
${ALGEBRA}(sit_qotfcat.ao): ${ALGEBRA}(sit_matquot.ao) ${ALGEBRA}(sit_uflgqot.ao)
${ALGEBRA}(sit_qotbyct.ao): ${ALGEBRA}(sit_matquot.ao) ${ALGEBRA}(sit_uflgqot.ao)
${ALGEBRA}(sit_qotient.ao):	${ALGEBRA}(sit_qotfcat.ao)
${ALGEBRA}(sit_quotby.ao):	${ALGEBRA}(sit_qotbyct.ao)
