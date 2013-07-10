#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

ThisDir = categories

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

SRCS = sit_abgroup sit_basic sit_monoid sit_abmon sit_fset sit_ring\
 sit_group sit_ncid sit_comring sit_intdom sit_gcd sit_euclid sit_field\
 sit_char0 sit_spzble sit_integer sit_deriv sit_difring sit_diffext sit_qring\
 sit_charp sit_pable sit_chrem sit_module sit_prfcat0 sit_spfcat0\
 sit_linarit sit_algebra sit_dcmprng sit_intgmp sit_automor sit_gexpcat\
 sit_freemod sit_freealg sit_freelc sit_freelar sit_ptools\
 sit_idxflc sit_idxflar sit_idxfmod sit_idxfalg alg_idxfrng alg_cansimp \
 alg_primsrc alg_rescls alg_modcmp alg_ffield sit_spf0 alg_rring alg_frering

DEPDIRS = extree

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

doc: $(DOCSTAMP) $(DocDir)/sit_power.tex $(DocDir)/sit_ptools.tex

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

${ALGEBRA}(sit_pable.ao):	${ALGEBRA}(sit_infexpr.ao)
${ALGEBRA}(sit_basic.ao):	${ALGEBRA}(sit_pable.ao)
${ALGEBRA}(sit_monoid.ao):	${ALGEBRA}(sit_basic.ao)
${ALGEBRA}(sit_abmon.ao):	${ALGEBRA}(sit_basic.ao)
${ALGEBRA}(sit_abgroup.ao):	${ALGEBRA}(sit_abmon.ao)
${ALGEBRA}(sit_group.ao):	${ALGEBRA}(sit_monoid.ao)
${ALGEBRA}(sit_ring.ao):	${ALGEBRA}(sit_abgroup.ao) ${ALGEBRA}(sit_monoid.ao)
${ALGEBRA}(sit_ncid.ao):	${ALGEBRA}(sit_ring.ao)
${ALGEBRA}(sit_comring.ao):	${ALGEBRA}(sit_ring.ao)
${ALGEBRA}(sit_char0.ao):	${ALGEBRA}(sit_ring.ao)
${ALGEBRA}(sit_charp.ao): ${ALGEBRA}(sit_integer.ao) ${ALGEBRA}(sit_intgmp.ao)
${ALGEBRA}(sit_spzble.ao):	${ALGEBRA}(sit_comring.ao)
${ALGEBRA}(sit_intdom.ao):	${ALGEBRA}(sit_comring.ao)
${ALGEBRA}(sit_dcmprng.ao):	${ALGEBRA}(sit_comring.ao)
${ALGEBRA}(sit_ptools.ao):	${ALGEBRA}(sit_comring.ao)
${ALGEBRA}(sit_gcd.ao):		${ALGEBRA}(sit_intdom.ao)
${ALGEBRA}(sit_euclid.ao):	${ALGEBRA}(sit_gcd.ao)
${ALGEBRA}(sit_gexpcat.ao):	${ALGEBRA}(sit_ptools.ao)
# ${ALGEBRA}(sit_chrem.ao):	${ALGEBRA}(sit_euclid.ao) ${ALGEBRA}(sit_integer.ao) ${ALGEBRA}(sit_intgmp.ao)
${ALGEBRA}(sit_chrem.ao):	${ALGEBRA}(sit_euclid.ao)
${ALGEBRA}(sit_integer.ao): ${ALGEBRA}(alg_modcmp.ao) ${ALGEBRA}(sit_euclid.ao) ${ALGEBRA}(sit_char0.ao) ${ALGEBRA}(sit_spzble.ao) ${ALGEBRA}(sit_gexpcat.ao)
${ALGEBRA}(sit_intgmp.ao):	${ALGEBRA}(sit_integer.ao)
${ALGEBRA}(sit_automor.ao): 	${ALGEBRA}(sit_group.ao) ${ALGEBRA}(sit_integer.ao) ${ALGEBRA}(sit_intgmp.ao)
${ALGEBRA}(sit_linarit.ao): 	${ALGEBRA}(sit_integer.ao) ${ALGEBRA}(sit_intgmp.ao)
${ALGEBRA}(sit_module.ao): 	${ALGEBRA}(sit_integer.ao) ${ALGEBRA}(sit_intgmp.ao)
${ALGEBRA}(alg_rring.ao): ${ALGEBRA}(sit_module.ao) ${ALGEBRA}(sit_linarit.ao)
${ALGEBRA}(sit_algebra.ao):	${ALGEBRA}(alg_rring.ao)
${ALGEBRA}(sit_fset.ao): ${ALGEBRA}(sit_integer.ao) ${ALGEBRA}(sit_intgmp.ao)
${ALGEBRA}(sit_deriv.ao):	${ALGEBRA}(sit_module.ao)
${ALGEBRA}(sit_difring.ao):	${ALGEBRA}(sit_deriv.ao)
${ALGEBRA}(sit_diffext.ao):	${ALGEBRA}(sit_difring.ao)
${ALGEBRA}(sit_field.ao): ${ALGEBRA}(sit_euclid.ao) ${ALGEBRA}(sit_group.ao)
${ALGEBRA}(alg_ffield.ao): ${ALGEBRA}(sit_field.ao) ${ALGEBRA}(sit_fset.ao) ${ALGEBRA}(sit_charp.ao)
${ALGEBRA}(sit_prfcat0.ao):	${ALGEBRA}(alg_ffield.ao)
${ALGEBRA}(sit_spfcat0.ao):	${ALGEBRA}(sit_prfcat0.ao)
${ALGEBRA}(sit_qring.ao): ${ALGEBRA}(sit_field.ao) ${ALGEBRA}(sit_integer.ao)
${ALGEBRA}(sit_freelc.ao): 	${ALGEBRA}(sit_integer.ao) ${ALGEBRA}(sit_intgmp.ao)
${ALGEBRA}(sit_idxflc.ao): 	${ALGEBRA}(sit_freelc.ao)
${ALGEBRA}(sit_freelar.ao): 	${ALGEBRA}(sit_integer.ao) ${ALGEBRA}(sit_linarit.ao) ${ALGEBRA}(sit_freelc.ao)
${ALGEBRA}(sit_idxflar.ao):     ${ALGEBRA}(sit_freealg.ao) ${ALGEBRA}(sit_idxflc.ao)
${ALGEBRA}(sit_freemod.ao):	${ALGEBRA}(sit_freelc.ao) ${ALGEBRA}(sit_module.ao)
${ALGEBRA}(sit_idxfmod.ao):     ${ALGEBRA}(sit_freemod.ao) ${ALGEBRA}(sit_idxflc.ao)
${ALGEBRA}(alg_frering.ao): ${ALGEBRA}(sit_freemod.ao) ${ALGEBRA}(sit_freelar.ao) ${ALGEBRA}(alg_rring.ao)
${ALGEBRA}(sit_freealg.ao): ${ALGEBRA}(alg_frering.ao) ${ALGEBRA}(sit_algebra.ao)
${ALGEBRA}(alg_idxfrng.ao): ${ALGEBRA}(alg_frering.ao) ${ALGEBRA}(sit_idxflar.ao)
${ALGEBRA}(sit_idxfalg.ao): ${ALGEBRA}(sit_freealg.ao) ${ALGEBRA}(alg_idxfrng.ao)
${ALGEBRA}(alg_cansimp.ao): 	${ALGEBRA}(sit_euclid.ao)
${ALGEBRA}(alg_primsrc.ao): 	${ALGEBRA}(sit_euclid.ao)
${ALGEBRA}(alg_modcmp.ao):  ${ALGEBRA}(alg_cansimp.ao) ${ALGEBRA}(alg_rescls.ao) ${ALGEBRA}(sit_chrem.ao)
${ALGEBRA}(alg_rescls.ao): 	${ALGEBRA}(sit_field.ao) ${ALGEBRA}(alg_primsrc.ao)
${ALGEBRA}(sit_spf0.ao): ${ALGEBRA}(sit_spfcat0.ao) ${ALGEBRA}(sit_primgen.ao)

