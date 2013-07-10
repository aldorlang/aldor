#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

ThisDir = multpoly_exponent

COMPILER=aldor
LibDir = $(ALGEBRAROOT)/lib
IncDir = $(ALGEBRAROOT)/include
DocDir = $(ALGEBRAROOT)/doc/tex
TestDir = $(ALGEBRAROOT)/test
StampDir  = $(LibDir)
DocStampDir  = $(DocDir)
TestStampDir  = $(TestDir)
PSTARGS = $(SRCS:%=%.ps) 

include ../../Makefile.Rules

ALDOR=${COMPILER} ${FLAGS}

.PRECIOUS:

SRCS = sm_vt sm_fvt sm_osymbol sm_listovar sm_tuplovar\
	 sm_expocat sm_dirprodc sm_mievc sm_zevc\
	sm_dirprod sm_midrl sm_midl sm_milex  

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

ps : $(PSTARGS)

%.ps :
	a2ps -o `basename $*`.ps `basename $*`.as

clean:
	-@rm -f *.c *.h *.fm *.ao *.o *~ *.ps

$(TESTSTAMP): $(TESTTARGS)
	touch $(TESTSTAMP)

$(DOCSTAMP): $(DOCTARGS)
	touch $(DOCSTAMP)

$(STAMP): $(AOTARGS)
	touch $(STAMP)

# Explicit dependencies
OD = $(DEPLST)

${ALGEBRA}(sm_vt.ao):          ${ALGEBRA}(sit_pable.ao)
${ALGEBRA}(sm_fvt.ao):         ${ALGEBRA}(sm_vt.ao)
${ALGEBRA}(sm_osymbol.ao):     ${ALGEBRA}(sm_vt.ao)
${ALGEBRA}(sm_listovar.ao):    ${ALGEBRA}(sm_fvt.ao)
${ALGEBRA}(sm_tuplovar.ao):    ${ALGEBRA}(sm_listovar.ao)
${ALGEBRA}(sm_expocat.ao):     ${ALGEBRA}(sm_vt.ao) ${ALGEBRA}(sit_integer.ao)
${ALGEBRA}(sm_dirprodc.ao):    ${ALGEBRA}(sit_basic.ao)
${ALGEBRA}(sm_mievc.ao):       ${ALGEBRA}(sm_expocat.ao) ${ALGEBRA}(sm_dirprodc.ao)
${ALGEBRA}(sm_zevc.ao):        ${ALGEBRA}(sm_expocat.ao) ${ALGEBRA}(sm_dirprodc.ao)
${ALGEBRA}(sm_dirprod.ao):     ${ALGEBRA}(sm_dirprodc.ao)
${ALGEBRA}(sm_midrl.ao):       ${ALGEBRA}(sm_mievc.ao) ${ALGEBRA}(sm_dirprod.ao)
${ALGEBRA}(sm_midl.ao):        ${ALGEBRA}(sm_midrl.ao)
${ALGEBRA}(sm_milex.ao):       ${ALGEBRA}(sm_midrl.ao)


