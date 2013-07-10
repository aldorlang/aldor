#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

ThisDir = basic

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

# TEMPORARY: MOVED sit_interp&sit_shell TO SUBDIR BECAUSE OF BUG WHEN OPTIMIZED
SRCS = sit_product sit_pring sit_mkpring sit_permut sit_indvar sit_complex

# directories that we depend on
DEPDIRS = categories

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

${ALGEBRA}(sit_indvar.ao):	${ALGEBRA}(sit_basic.ao)
${ALGEBRA}(sit_product.ao):	${ALGEBRA}(sit_integer.ao)
${ALGEBRA}(sit_pring.ao):	${ALGEBRA}(sit_integer.ao)
${ALGEBRA}(sit_complex.ao):${ALGEBRA}(sit_field.ao) ${ALGEBRA}(sit_algebra.ao) ${ALGEBRA}(sit_OPcplex.ao)
${ALGEBRA}(sit_mkpring.ao):	${ALGEBRA}(sit_pring.ao)
${ALGEBRA}(sit_interp.ao):	${ALGEBRA}(sit_pring.ao)
${ALGEBRA}(sit_shell.ao):	${ALGEBRA}(sit_interp.ao)
${ALGEBRA}(sit_permut.ao):	${ALGEBRA}(sit_group.ao) ${ALGEBRA}(sit_integer.ao)
