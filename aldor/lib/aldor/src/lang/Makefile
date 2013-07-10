#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

ThisDir = lang

COMPILER=$(ALDORROOT)/bin/aldor
LibDir = $(ALDORLIBROOT)/lib
IncDir = $(ALDORLIBROOT)/include
StampDir  = $(LibDir)

include ../Makefile.Rules

ALDOR=${COMPILER} ${FLAGS}

.PRECIOUS:

SRCS = sal_lang

NOTYET = 

OBRACK = (
CBRACK = )

SRCAS   = $(SRCS:%=%.as)
AOTARGS = $(SRCS:%=$(ALDORLIB)$(OBRACK)%.ao$(CBRACK))

STAMP = $(StampDir)/stamp-$(ThisDir)

OKFILES = Makefile $(SRCAS) $(NOTYET)

test: $(SRCAS)

doc: $(SRCAS)

aobj: $(STAMP)

clean:
	-@rm -f *.c *.h *.fm *.ao *.o *~

$(STAMP): $(AOTARGS)
	touch $(STAMP)

${ALDORLIB}(sal_lang.ao):

