##############################################################################
#
# Makefile
#
# Make program to build the Demo library.
#
# Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#
##############################################################################
#
# Targets: all, junk, clean, quick
#
#    make all	-- make the demo library for the Aldor compiler
#    make junk	-- list files which are not part of the Aldor compiler
#    make clean -- remove generated files  (e.g. foo.c~)
#
# Parameters:  ALDORROOT
#
#    To override defaults do, e.g.,
#	 make -e ALDORROOT=/spad/local/axiomxl/rs all
#    or
#	 setenv ALDORROOT /spad/local/axiomxl/rs; setenv MAKEFLAGS e; make all
#
##############################################################################

SHELL	= /bin/sh

#
# Environment
#
# $(ALDORROOT) is retrieved from the environment
IncDir	= $(ALDORROOT)/include
LibDir	= $(ALDORROOT)/lib
BinDir	= $(ALDORROOT)/bin
SamDir	= $(ALDORROOT)/share/samples/libaxldem

LAXLLIB	= $(LibDir)/libaxllib.al
LAXLDEM	= $(LibDir)/libaxldem.al
LOBDEM	= $(LibDir)/libaxldem.a
BUILDARG = $(ALDORROOT)/toolbin/buildarg
AXLCDB	= -W check # -Wgc #-cargs=-g # -Fc -Cargs=-g
AXLFLAGS= $(AXLCDB)

RM	= rm -f
CP	= cp -p

#
# Source lists
#
Sources	= prime.as polycat.as poly.as random.as ibits.as lmdict.as \
	  spf.as dirprod.as gb.as nni.as vector.as quanc8.as \
	  matrix.as matopdom.as axldem.as
Lists	= axldem.lst

Includes = axldem.as

#
# Object lists
#
Aos   	= $(LAXLDEM)(prime.ao)		\
	  $(LAXLDEM)(polycat.ao)	\
	  $(LAXLDEM)(poly.ao)		\
	  $(LAXLDEM)(random.ao)		\
	  $(LAXLDEM)(ibits.ao)		\
	  $(LAXLDEM)(lmdict.ao)		\
	  $(LAXLDEM)(spf.ao)		\
	  $(LAXLDEM)(dirprod.ao)	\
	  $(LAXLDEM)(gb.ao)		\
	  $(LAXLDEM)(nni.ao)		\
	  $(LAXLDEM)(vector.ao)		\
	  $(LAXLDEM)(quanc8.ao)		\
	  $(LAXLDEM)(matrix.ao)		\
	  $(LAXLDEM)(matopdom.ao)

Objects	= $(LOBDEM)(prime.o)		\
	  $(LOBDEM)(polycat.o)		\
	  $(LOBDEM)(poly.o)		\
	  $(LOBDEM)(random.o)		\
	  $(LOBDEM)(ibits.o)		\
	  $(LOBDEM)(lmdict.o)		\
	  $(LOBDEM)(spf.o)		\
	  $(LOBDEM)(dirprod.o)		\
	  $(LOBDEM)(gb.o)		\
	  $(LOBDEM)(nni.o)		\
	  $(LOBDEM)(vector.o)		\
	  $(LOBDEM)(quanc8.o)		\
	  $(LOBDEM)(matrix.o)		\
	  $(LOBDEM)(matopdom.o)

#
# Target lists
#
LibTargs = $(LAXLDEM) $(LOBDEM)

IncTargs = $(IncDir)/axldem.as
.PRECIOUS: $(LibTargs)

#
# Generic targets
#
OKFILES	 = Makefile NMakefile $(Lists) $(Sources)

all:	$(Lists) $(LibTargs) $(IncTargs) 

ext: otherlibs

junk:
	@ ls -d $(OKFILES) * | sort | grep -v CVS | uniq -u

clean:
	@ $(RM) *~ core

libclean:
	@ $(RM) $(LAXLDEM) $(LOBDEM)

lists:		$(Lists)
libaxldem:	$(LAXLDEM) $(LOBDEM)

otherlibs:
	for x in "" `$(BUILDARG) extra-libdirs` ; do 		\
		lib=$(ALDORROOT)/lib-$(MACHINE)$$x;		\
		if [ ! -d $$lib ] ; then  mkdir $$lib ; fi;	\
		sys=$(MACHINE)$$x;				\
		echo "mklib $$sys libaxldem" >> $(ALDORROOT)/lib-$$sys/runme.sh; \
		chmod 755 $(ALDORROOT)/lib-$$sys/runme.sh;		\
	done

#
# Samples
#
samples:	$(SamDir) $(Sources)
	@echon "Copying samples..."
	@for f in $(Sources) ; do \
		$(CP) $$f $(SamDir) ; done
	@echo "done"

include:	$(IncTargs)

$(SamDir):
	@echo "Making directory $(SamDir)."
	@mkdir $(SamDir)

#
# General dependencies
#
$(Objects) $(Aos):	$(LAXLLIB)

incaxllib    $(IncDir)/axldem.as:	   axldem.as
	$(CP) axldem.as	$(IncDir)/axldem.as

# The doranlib dependency ensures that doranlib gets called.
$(LAXLDEM):  $(Aos)    doranlib		; @ echo doranlib $@
$(LOBDEM):  $(Objects) doranlib		; doranlib $@

doranlib:				; @ echon

#
# Actually make stuff
#
axldem.lst: Makefile
	@echon "Making axldem.lst..."
	@(for f in $(Sources) ; do echo $$f ; done) | \
		sed -e 's/\.as//' > axldem.lst
	@echo "done"

prime $(LOBDEM)(prime.o) $(LAXLDEM)(prime.ao): prime.as
	@doaldor prime axldem $(AXLFLAGS) -Q inline-limit=2

polycat $(LOBDEM)(polycat.o) $(LAXLDEM)(polycat.ao): polycat.as
	@doaldor polycat axldem $(AXLFLAGS)

poly $(LOBDEM)(poly.o) $(LAXLDEM)(poly.ao): poly.as
	@doaldor poly axldem $(AXLFLAGS) -laxldem

random $(LOBDEM)(random.o) $(LAXLDEM)(random.ao): random.as
	@doaldor random axldem $(AXLFLAGS)

ibits $(LOBDEM)(ibits.o) $(LAXLDEM)(ibits.ao): ibits.as
	@doaldor ibits axldem $(AXLFLAGS)

lmdict $(LOBDEM)(lmdict.o) $(LAXLDEM)(lmdict.ao): lmdict.as
	@doaldor lmdict axldem $(AXLFLAGS)

spf $(LOBDEM)(spf.o) $(LAXLDEM)(spf.ao): spf.as
	@doaldor spf axldem $(AXLFLAGS)

dirprod $(LOBDEM)(dirprod.o) $(LAXLDEM)(dirprod.ao): dirprod.as
	@doaldor dirprod axldem $(AXLFLAGS) -laxldem

gb $(LOBDEM)(gb.o) $(LAXLDEM)(gb.ao): gb.as
	@doaldor gb axldem $(AXLFLAGS) -laxldem -Q inline-limit=5

nni $(LOBDEM)(nni.o) $(LAXLDEM)(nni.ao): nni.as
	@doaldor nni axldem $(AXLFLAGS)

vector $(LOBDEM)(vector.o) $(LAXLDEM)(vector.ao): vector.as
	@doaldor vector axldem $(AXLFLAGS)

quanc8 $(LOBDEM)(quanc8.o) $(LAXLDEM)(quanc8.ao): quanc8.as
	@doaldor quanc8 axldem $(AXLFLAGS)

matrix $(LOBDEM)(matrix.o) $(LAXLDEM)(matrix.ao): matrix.as
	@doaldor matrix axldem $(AXLFLAGS)

matopfld $(LOBDEM)(matopfld.o) $(LAXLDEM)(matopfld.ao): matopfld.as
	@doaldor matopfld axldem $(AXLFLAGS)

matopdom $(LOBDEM)(matopdom.o) $(LAXLDEM)(matopdom.ao): matopdom.as
	@doaldor matopdom axldem $(AXLFLAGS)

