#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

ThisDir = util

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

SRCS = sal_timer sal_file sal_cmdline sal_agat sal_version rtexns ald_trace eio_rsto

# directories that we depend on
DEPDIRS = datastruc

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

objects: $(SALLIM)

clean:
	-@rm -f *.fm *.ao *.o *~

$(TESTSTAMP): $(TESTTARGS)
	touch $(TESTSTAMP)

$(DOCSTAMP): $(DOCTARGS)
	touch $(DOCSTAMP)

$(STAMP): $(AOTARGS)
	touch $(STAMP)

# Explicit dependencies
OD = $(DEPLST)

${ALDORLIB}(sal_timer.ao):		${ALDORLIB}(sal_mint.ao)
${ALDORLIB}(sal_file.ao):		${ALDORLIB}(sal_string.ao)
${ALDORLIB}(sal_version.ao):		${ALDORLIB}(sal_string.ao)
${ALDORLIB}(sal_cmdline.ao):		${ALDORLIB}(sal_string.ao)
${ALDORLIB}(sal_agat.ao):		${ALDORLIB}(sal_sfloat.ao) ${ALDORLIB}(sal_dfloat.ao)
${ALDORLIB}(ald_trace.ao):		${ALDORLIB}(sal_string.ao)
${ALDORLIB}(rtexns.ao):			${ALDORLIB}(ald_trace.ao)
${ALDORLIB}(eio_rsto.ao):		${OD}

