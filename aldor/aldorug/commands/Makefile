##############################################################################
#
# Makefile for the UserGuide/commands subdirectory
#
##############################################################################
#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

SRCFILES = \
	aldorbug.rec \
	verbose.rec \
	bloop.rec \
	arigato.sh cside.sh  sieve.sh  error0.sh separate.sh 

MADEFILES = \
	arigato.in arigato.out \
	cside.in   cside.out \
	sieve.in   sieve.out \
	error0.in \
        separate.in

OKFILES = Makefile $(SRCFILES) $(MADEFILES)

# ALDORARGS="-Y /usr/local/BasicMath/lib -I /usr/local/BasicMath/include -laldor"
ALDORARGS="-Y$(BMROOT)/lib -I$(BMROOT)/include -laldor"

XMPDIR=../examples

all:	arigato.in $(MADEFILES)

junk:
	@@ ls -d $(OKFILES) * | sort | uniq -u

arigato.in: arigato.sh
	sed -e 's/^/% /' < arigato.sh > $@@

cside.in: cside.sh
	sed -e 's/^/% /' < cside.sh > $@@

sieve.in: sieve.sh
	sed -e 's/^/% /' < sieve.sh > $@@

error0.in: error0.sh
	sed -e 's/^/% /' < error0.sh > $@@

separate.in: separate.sh
	sed -e 's/^/% /' < separate.sh > $@@

arigato.out: arigato.sh $(XMPDIR)/arigato.as $(XMPDIR)/nputs.c
	cp $(XMPDIR)/arigato.as .
	cp $(XMPDIR)/nputs.c .
	env ALDORARGS=$(ALDORARGS) sh arigato.sh | grep -v "arigato.as:" > arigato.out
	rm -f arigato.as nputs.c nputs.o arigato

cside.out: cside.sh $(XMPDIR)/aside.as $(XMPDIR)/cside.c
	cp $(XMPDIR)/aside.as .
	cp $(XMPDIR)/cside.c .
	env ALDORARGS=$(ALDORARGS) sh cside.sh >cside.out
	rm -f aside.as  cside.c aside.o cside.o lcm64

sieve.out: sieve.sh $(XMPDIR)/sieve.as
	cp $(XMPDIR)/sieve.as .
	env ALDORARGS=$(ALDORARGS) sh sieve.sh >sieve.out
	rm -f sieve.as sieve
