#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

ThisDir = extree_operators

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

SRCS = sit_OPplus sit_OPtimes sit_OPexpt sit_OPquot sit_OPminus\
 sit_OPmatrx sit_OPand sit_OPequal sit_OPcase sit_OPif sit_OPnoteq\
 sit_OPprefx sit_OPassgn sit_OPfact sit_OPlist sit_OPllist sit_OPless\
 sit_OPmore sit_OPvect sit_OPsubsc sit_OPbigO sit_OPcplex

# directories that we depend on
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

${ALGEBRA}(sit_OPtimes.ao):	${ALGEBRA}(sit_optools.ao)
${ALGEBRA}(sit_OPexpt.ao):	${ALGEBRA}(sit_optools.ao)
${ALGEBRA}(sit_OPfact.ao):	${ALGEBRA}(sit_optools.ao)
${ALGEBRA}(sit_OPquot.ao):	${ALGEBRA}(sit_optools.ao)
${ALGEBRA}(sit_OPminus.ao):	${ALGEBRA}(sit_optools.ao)
${ALGEBRA}(sit_OPmatrx.ao):	${ALGEBRA}(sit_optools.ao)
${ALGEBRA}(sit_OPvect.ao):	${ALGEBRA}(sit_optools.ao)
${ALGEBRA}(sit_OPlist.ao):	${ALGEBRA}(sit_optools.ao)
${ALGEBRA}(sit_OPand.ao):		${ALGEBRA}(sit_optools.ao)
${ALGEBRA}(sit_OPmore.ao):	${ALGEBRA}(sit_optools.ao)
${ALGEBRA}(sit_OPless.ao):	${ALGEBRA}(sit_optools.ao)
${ALGEBRA}(sit_OPsubsc.ao):	${ALGEBRA}(sit_optools.ao)
${ALGEBRA}(sit_OPequal.ao):	${ALGEBRA}(sit_optools.ao)
${ALGEBRA}(sit_OPif.ao):		${ALGEBRA}(sit_optools.ao)
${ALGEBRA}(sit_OPnoteq.ao):	${ALGEBRA}(sit_optools.ao)
${ALGEBRA}(sit_OPprefx.ao):	${ALGEBRA}(sit_optools.ao)
${ALGEBRA}(sit_OPassgn.ao):	${ALGEBRA}(sit_optools.ao)
${ALGEBRA}(sit_OPbigO.ao):	${ALGEBRA}(sit_optools.ao)
${ALGEBRA}(sit_OPcplex.ao):	${ALGEBRA}(sit_optools.ao)
${ALGEBRA}(sit_OPcase.ao):	${ALGEBRA}(sit_OPif.ao)
${ALGEBRA}(sit_OPllist.ao):	${ALGEBRA}(sit_OPlist.ao)
${ALGEBRA}(sit_OPplus.ao):	${ALGEBRA}(sit_OPminus.ao)

