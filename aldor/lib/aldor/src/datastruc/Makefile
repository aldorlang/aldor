#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

ThisDir = datastruc

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

SRCS = sal_lstruc sal_fstruc sal_bstruc sal_parray sal_list sal_array\
	sal_barray sal_ckarray sal_string sal_hash sal_set sal_stream\
	sal_memblk sal_ckmembk sal_pkarray sal_data sal_bdata sal_ddata\
	sal_table sal_kntry sal_slist sal_sset sal_sortas sal_cklist\
	ald_queue ald_flags ald_symbol ald_symtab

# directories that we depend on
DEPDIRS = arith

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

${ALDORLIB}(sal_data.ao):		${ALDORLIB}(sal_random.ao)
${ALDORLIB}(sal_bdata.ao):		${ALDORLIB}(sal_data.ao)
${ALDORLIB}(sal_ddata.ao):		${ALDORLIB}(sal_bdata.ao)
${ALDORLIB}(sal_lstruc.ao):	${ALDORLIB}(sal_data.ao)
${ALDORLIB}(sal_fstruc.ao):	${ALDORLIB}(sal_lstruc.ao)
${ALDORLIB}(sal_bstruc.ao):	${ALDORLIB}(sal_fstruc.ao) ${ALDORLIB}(sal_bdata.ao)
${ALDORLIB}(sal_list.ao):		${ALDORLIB}(sal_bstruc.ao) ${ALDORLIB}(sal_ddata.ao)
${ALDORLIB}(sal_cklist.ao):	${ALDORLIB}(sal_list.ao)
${ALDORLIB}(sal_set.ao):		${ALDORLIB}(sal_list.ao)
${ALDORLIB}(sal_sset.ao):		${ALDORLIB}(sal_list.ao)
${ALDORLIB}(sal_slist.ao):		${ALDORLIB}(sal_list.ao)
${ALDORLIB}(sal_parray.ao):	${ALDORLIB}(sal_list.ao)
${ALDORLIB}(sal_pkarray.ao):	${ALDORLIB}(sal_list.ao)
${ALDORLIB}(ald_queue.ao):	${ALDORLIB}(sal_list.ao)
${ALDORLIB}(sal_barray.ao):	${ALDORLIB}(sal_pkarray.ao)
${ALDORLIB}(ald_flags.ao):	${ALDORLIB}(sal_barray.ao)
${ALDORLIB}(sal_string.ao):	${ALDORLIB}(sal_array.ao)
${ALDORLIB}(sal_stream.ao):	${ALDORLIB}(sal_string.ao)
${ALDORLIB}(sal_array.ao):		${ALDORLIB}(sal_parray.ao)
${ALDORLIB}(sal_ckarray.ao):	${ALDORLIB}(sal_array.ao)
${ALDORLIB}(sal_memblk.ao):	${ALDORLIB}(sal_array.ao)
${ALDORLIB}(sal_ckmembk.ao):	${ALDORLIB}(sal_memblk.ao)
${ALDORLIB}(sal_table.ao):		${ALDORLIB}(sal_string.ao)
${ALDORLIB}(sal_kntry.ao):		${ALDORLIB}(sal_random.ao)
${ALDORLIB}(sal_hash.ao): ${ALDORLIB}(sal_table.ao) ${ALDORLIB}(sal_kntry.ao)
${ALDORLIB}(sal_sortas.ao):${ALDORLIB}(sal_table.ao) ${ALDORLIB}(sal_kntry.ao) ${ALDORLIB}(sal_sset.ao)
${ALDORLIB}(ald_symbol.ao):		${ALDORLIB}(sal_hash.ao)
${ALDORLIB}(ald_symtab.ao):		${ALDORLIB}(ald_symbol.ao)
