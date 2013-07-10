#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

ThisDir = base

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

SRCS = sal_base sal_bstream sal_tstream sal_serial sal_partial sal_char\
	sal_manip sal_otype sal_itype sal_byte sal_htype sal_copy sal_gener\
	sal_syntax sal_order sal_torder ald_pfunc

# directories that we depend on
DEPDIRS = lang

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

${ALDORLIB}(sal_gener.ao):		${ALDORLIB}(sal_lang.ao)
${ALDORLIB}(sal_base.ao):		${ALDORLIB}(sal_gener.ao)
${ALDORLIB}(sal_order.ao):		${ALDORLIB}(sal_base.ao)
${ALDORLIB}(sal_torder.ao):	${ALDORLIB}(sal_order.ao)
${ALDORLIB}(sal_copy.ao):		${ALDORLIB}(sal_base.ao)
${ALDORLIB}(sal_bstream.ao):	${ALDORLIB}(sal_base.ao)
${ALDORLIB}(sal_tstream.ao):	${ALDORLIB}(sal_base.ao)
${ALDORLIB}(sal_htype.ao):		${ALDORLIB}(sal_base.ao)
${ALDORLIB}(sal_otype.ao):		${ALDORLIB}(sal_tstream.ao)
${ALDORLIB}(sal_itype.ao):		${ALDORLIB}(sal_tstream.ao)
${ALDORLIB}(sal_serial.ao): 	${ALDORLIB}(sal_bstream.ao)
${ALDORLIB}(sal_byte.ao): ${ALDORLIB}(sal_serial.ao) ${ALDORLIB}(sal_otype.ao) ${ALDORLIB}(sal_itype.ao) ${ALDORLIB}(sal_htype.ao)
${ALDORLIB}(sal_manip.ao):	${ALDORLIB}(sal_otype.ao) ${ALDORLIB}(sal_bstream.ao) ${ALDORLIB}(sal_byte.ao)
${ALDORLIB}(sal_char.ao):		${ALDORLIB}(sal_byte.ao) ${ALDORLIB}(sal_torder.ao)
${ALDORLIB}(sal_partial.ao):	${ALDORLIB}(sal_syntax.ao) ${ALDORLIB}(sal_char.ao)
${ALDORLIB}(ald_pfunc.ao):		${ALDORLIB}(sal_partial.ao)
