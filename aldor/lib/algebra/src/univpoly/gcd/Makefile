#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

ThisDir = univpoly_gcd

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

SRCS = sit_heugcd sit_modpgcd sit_modgcd sit_gcdint sit_gcdintg

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

${ALGEBRA}(sit_heugcd.ao):	${ALGEBRA}(sit_upolc0.ao) ${ALGEBRA}(sit_integer.ao)
${ALGEBRA}(sit_modpgcd.ao): ${ALGEBRA}(sit_primgen.ao) sit_fullgcd.as sit_halfgcd.as sit_loggcd.as
${ALGEBRA}(sit_modgcd.ao):${ALGEBRA}(sit_dup.ao) ${ALGEBRA}(sit_modpgcd.ao) ${ALGEBRA}(sit_chrem.ao)
${ALGEBRA}(sit_gcdint.ao): ${ALGEBRA}(sit_modgcd.ao) ${ALGEBRA}(sit_heugcd.ao) ${ALGEBRA}(sit_resprs.ao) ${ALGEBRA}(sit_intgmp.ao) sit_gcdint0.as
${ALGEBRA}(sit_gcdintg.ao): ${ALGEBRA}(sit_modgcd.ao) ${ALGEBRA}(sit_heugcd.ao) ${ALGEBRA}(sit_resprs.ao) ${ALGEBRA}(sit_intgmp.ao) sit_gcdint0.as
