#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

ThisDir = ffield

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

SRCS = sit_prfcat sit_sprfcat sit_spf sit_zpf sit_sprfgcd sit_sprfmat alg_pf2

# directories that we depend on
DEPDIRS = polyfactorp

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

${ALGEBRA}(sit_prfcat.ao):	${ALGEBRA}(sit_upfactp.ao) ${ALGEBRA}(sit_fring.ao)
${ALGEBRA}(sit_sprfgcd.ao):     ${ALGEBRA}(sit_gcdint.ao) ${ALGEBRA}(sit_spfcat0.ao)
${ALGEBRA}(sit_sprfmat.ao):     ${ALGEBRA}(sit_modpoge.ao) ${ALGEBRA}(sit_sprfgcd.ao)
${ALGEBRA}(sit_sprfcat.ao):${ALGEBRA}(sit_prfcat.ao) ${ALGEBRA}(sit_sprfgcd.ao) ${ALGEBRA}(sit_ugring.ao) ${ALGEBRA}(sit_sprfmat.ao) ${ALGEBRA}(sit_laring.ao)
${ALGEBRA}(sit_spf.ao):		${ALGEBRA}(sit_sprfcat.ao) ${ALGEBRA}(sit_spf0.ao)
${ALGEBRA}(sit_zpf.ao):	${ALGEBRA}(sit_sprfcat.ao) ${ALGEBRA}(sit_prmroot.ao)
${ALGEBRA}(alg_pf2.ao):		${ALGEBRA}(sit_sprfcat.ao) ${ALGEBRA}(sit_spf0.ao)
