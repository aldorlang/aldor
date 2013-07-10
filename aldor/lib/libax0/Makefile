##############################################################################
#
# Makefile
#
# Make program to build the Axiom interface.
#
# Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#
##############################################################################
#
# Targets: all, junk, clean, quick
#
#    make all	-- make the Axiom interface for the AXIOM-XL compiler
#    make junk	-- list files which are not part of the AXIOM-XL compiler
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
SamDir	= $(ALDORROOT)/share/samples/libax0

AXIOMXL	= $(BinDir)/axiomxl
LAXLLIB	= $(ALDORROOT)/lib/libaxllib.al
LAXIOM	= $(LibDir)/libax0.al
LAXIOMOBJ	= $(LibDir)/libax0.a
LAXKEY	= ax0
doaxiomxl = ../../aldor/tools/unix/doaldor

AXLCDB	= -W check -M no-AXL_W_WillObsolete
AXLFLAGS= -lAxiomLib=$(LAXKEY) $(AXLCDB) -Y $(LibDir)
AXFLAGS	= -Wname=axiom -Wnhash -M no-AXL_W_OverRideLibraryFile -R $(LibDir) -Y $(LibDir) $(AXLFLAGS)

# $(AXIOM) is retrieved from the environment
OBJ		= $(AXIOM)/../../obj
MACH		= `basename $(AXIOM)`
INTERPSYS	= $(OBJ)/$(MACH)/bin/interpsys
INTERPSYS	= $(AXIOM)/bin/AXIOMsys

CFLAGS	= -g
RM	= rm -f
CP	= cp

AR	= ar
ARFLAGS	= rv
ARXFLAGS= x
ARR	= $(AR) $(ARFLAGS)
ARX	= $(AR) $(ARXFLAGS)

#
# Source lists
#
Samples	= axlit.as axiom.as
Sources	= attrib.as stub.as axlit.as minimach.as axextend.as \
	  axext_l.lsp axiom.as ax0.as mkax.lsp 
Lists	= axiom.lst ax.lst

#
# Object lists
#
Aos	= $(LAXIOM)(lang.ao)		\
	  $(LAXIOM)(attrib.ao)		\
	  $(LAXIOM)(stub.ao)		\
	  $(LAXIOM)(axlit.ao)		\
	  $(LAXIOM)(minimach.ao)	\
	  $(LAXIOM)(axextend.ao)

#
# Target lists
#
IncTargs = $(IncDir)/axiom.as $(IncDir)/ax0.as
LibTargs = $(LAXIOM)
AoTargs = lang attrib stub mkao axlit minimach axextend axext
AuxTargs = $(LibDir)/lang.lsp		\
	   $(LibDir)/attrib.asy		\
	   $(LibDir)/attrib.lsp		\
	   $(LibDir)/axlit.asy		\
	   $(LibDir)/axlit.lsp		\
	   $(LibDir)/minimach.asy	\
	   $(LibDir)/minimach.lsp	\
	   $(LibDir)/axextend.asy	\
	   $(LibDir)/axext_l.lsp

# Don't delete the libraries if a compilation fails.
.PRECIOUS:	$(LibTargs)

#
# Generic targets
#
OKFILES	 = Makefile NMakefile $(Lists) $(Sources) ax nax

all:	$(Lists) $(IncTargs) $(AoTargs)

junk:
	@ ls -d $(OKFILES) * | sort | grep -v CVS | uniq -u

clean:
	@ $(RM) *~

libclean:
	@ $(RM) $(IncTargs) $(LibTargs) $(AuxTargs)

axiom.lst:	Makefile
	@echon "Making axiom.lst..."
	@(for f in $(Sources) ; do echo $$f ; done) | \
		sed -e 's/\.ax//' -e 's/\.as//' > axiom.lst
	@echo "done"

lists:		$(Lists)
include:	$(IncTargs)
lib:		$(LibTargs)

#
# Samples
#
samples:	$(SamDir) $(Samples)
	@echon "Copying samples..."
	@for f in $(Samples) ; do \
		$(CP) $$f $(SamDir) ; done
	@echo "done"

$(SamDir):
	@echo "Making directory $(SamDir)."
	@mkdir $(SamDir)

#
# Specific dependencies and rules
#

# The doranlib dependency ensures that doranlib gets called.
$(LAXIOM):  $(Aos)     doranlib	; doranlib $@

doranlib:				; @ echon

#
# Ax0
#
lang $(LAXIOM)(lang.ao): $(LAXLLIB)(lang.ao)
	(cd $(LibDir); \
	  $(ARX) $(LAXLLIB) lang.ao; \
	  $(ARR) $(LAXIOM)  lang.ao; \
	  echo Generating $(LibDir)/lang.lsp; \
	  $(AXIOMXL) -F lsp $(AXLFLAGS) lang.ao; \
	  $(RM) lang.ao)

attrib $(LAXIOM)(attrib.ao): attrib.as
	$(doaxiomxl) attrib $(LAXKEY) as $(AXFLAGS) -Fasy -Flsp

stub $(LAXIOM)(stub.ao): stub.as
	$(doaxiomxl) stub $(LAXKEY) as $(AXLFLAGS)

axlit $(LAXIOM)(axlit.ao): axlit.as
	$(doaxiomxl) axlit $(LAXKEY) as $(AXLFLAGS) -Fasy -Flsp

minimach $(LAXIOM)(minimach.ao): minimach.as
	$(doaxiomxl) minimach $(LAXKEY) as $(AXLFLAGS) -Fasy -Flsp

axextend $(LAXIOM)(axextend.ao): axextend.as
	$(doaxiomxl) axextend $(LAXKEY) as $(AXLFLAGS) -Fasy -Flsp -X

axext $(LibDir)/axext_l.lsp:: axext_l.lsp
	echo Copying axext_l.lsp into $(LibDir)
	$(CP) axext_l.lsp $(LibDir)/axext_l.lsp

$(IncDir)/axiom.as: axiom.as
	echo Copying axiom.as into $(IncDir)
	$(CP) axiom.as $(IncDir)/axiom.as

$(IncDir)/ax0.as: ax0.as
	echo Copying ax0.as into $(IncDir)
	$(CP) ax0.as $(IncDir)/ax0.as

#
# basecliq
#

mkax:
	@if [ ! -x ax ] ; then mkdir ax ; fi
	@ $(INTERPSYS) < mkax.lsp
#	@if [ -x nax ] ; then cp nax/*.ax ax; fi

mkao:
	(for i in `cat ax.lst` ; do \
	    echo $$i ; \
	    (cd ax ; $(AXIOMXL) $(AXFLAGS) $$i.ax) ; \
	    (cd $(LibDir); \
	     $(ARR) $(LAXIOM) $$i.ao ; \
	     $(RM) $$TMPDIR/vA*; \
	     $(RM) $$i.ao) ; \
	done)

mko:
	@echo "Building $(LAXIOMOBJ)"
	(for i in `cat ax.lst` ; do \
	  echo $$i ; \
	  (cd ax ; ar rv $(LAXIOMOBJ) $$i.o) ; \
	done)
	ranlib $(LAXIONOBJ)
	  #(cd ax ; $(AXIOMXL) -Fo $(AXFLAGS) $$i.ax) ; \
	  #(cd ax ; rm -f $$i.o) ; \
