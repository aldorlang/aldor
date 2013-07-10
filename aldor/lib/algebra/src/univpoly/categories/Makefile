#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

ThisDir = univpoly_categories

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

SRCS = sit_uffalg sit_ufalg sit_upolc0 sit_zring sit_fring sit_fftring\
	sit_sqfree sit_resprs sit_ugring sit_umonom sit_upolalg sit_froot\
	alg_polydio alg_chrem2 alg_modgcdp


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

${ALGEBRA}(sit_umonom.ao):	${ALGEBRA}(sit_integer.ao) ${ALGEBRA}(sit_charp.ao)
${ALGEBRA}(sit_ufalg.ao):		${ALGEBRA}(sit_idxflar.ao)
${ALGEBRA}(sit_uffalg.ao):${ALGEBRA}(sit_ufalg.ao) ${ALGEBRA}(sit_umonom.ao) ${ALGEBRA}(sit_idxfalg.ao) ${ALGEBRA}(sit_dnsemat.ao) ${ALGEBRA}(sit_automor.ao)
${ALGEBRA}(sit_upolalg.ao):	${ALGEBRA}(sit_uffalg.ao)
${ALGEBRA}(sit_upolc0.ao): ${ALGEBRA}(sit_upolalg.ao) ${ALGEBRA}(sit_product.ao) ${ALGEBRA}(sit_diffext.ao) ${ALGEBRA}(sit_linalg.ao) ${ALGEBRA}(alg_modcmp.ao)
${ALGEBRA}(sit_froot.ao):		${ALGEBRA}(sit_upolc0.ao)
${ALGEBRA}(sit_zring.ao):		${ALGEBRA}(sit_froot.ao)
${ALGEBRA}(sit_fftring.ao):	${ALGEBRA}(sit_upolc0.ao)
${ALGEBRA}(sit_sqfree.ao):	${ALGEBRA}(sit_upolc0.ao)
${ALGEBRA}(sit_resprs.ao):	${ALGEBRA}(sit_upolc0.ao)
${ALGEBRA}(sit_ugring.ao):	${ALGEBRA}(sit_upolc0.ao)
${ALGEBRA}(sit_fring.ao):		${ALGEBRA}(sit_zring.ao)
${ALGEBRA}(alg_polydio.ao):	${ALGEBRA}(sit_resprs.ao)
${ALGEBRA}(alg_chrem2.ao):	${ALGEBRA}(sit_upolc0.ao)
${ALGEBRA}(alg_modgcdp.ao):	${ALGEBRA}(sit_upolc0.ao)
