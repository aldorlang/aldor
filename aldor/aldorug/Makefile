##############################################################################
#
# Makefile for the UserGuide subdirectory
#
##############################################################################
#
# This file contains the source and tools necessary to create
# an Aldor Compiler User's Guide.
#
# Makefile
#	  Creates the proper auxilliary files for the User's Guide.
#	  You must have the compiler in your path.
#
#	  make all     creates the extra files needed.
#	  make latex   creates the User's Guide .dvi file
#	  make view    invokes xdvi on the generated .dvi file
#         make distrib creates .ps and .pdf files for distribution
#	  make clean   cleans up generated files
#
# 	  make PRT=algebra print
#		      prints the generated .dvi file on the printer 'algebra'
#
# aldorug.tex
#	  This is the main document source for the User's Guide.
#
# ../TexMacs/axiom.sty
# ../TexMacs/axiomps.sty
# ../TexMacs/verbatim.sty
#	  These are LaTeX style files used by aldorug.tex.
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

TEXMACS=	TexMacs/verbatim.sty \
		TexMacs/axiom.sty TexMacs/axiomps.sty

#SUBDIRS=	commands examples screens samples pictures
SUBDIRS=	commands examples screens pictures
TEXSRC=		aldorug.tex aldorugmacs.tex \
		\
		version.tex title.tex notices.tex preface.tex \
		\
		intro.tex \
		    ports.tbl \
		progsimp.tex \
		langsumm.tex \
		langsyn.tex \
		    precedence.tbl \
                langexpr.tex \
		    langloop.tex \
		langfuns.tex langtype.tex \
                langenvs.tex \
		langgenr.tex langextd.tex \
                langties.tex \
		langmacs.tex \
		langtdef.tex \
		langmach.tex langlibs.tex \
		errors.tex \
		separate.tex interact.tex \
		c.tex \
		samples.tex \
		\
		formal.tex \
		    grammar.tex \
		options.tex \
                unicl.tex \
		messages.tex \
		\
		\
		foreign.tex install.tex develop.tex fornlisp.tex

COMPILER=	aldor
SRCDIR=		Src
TESTDIR=	../../test
CONTRIBDIR=	../../contrib
TOOLDIR=	$(ALDORROOT)/toolbin
NUMBER=		$(TOOLDIR)/number

CAT=		untab
#CAT=		cat

TEXJUNK=	aldorug.toc aldorug.toc.keep aldorug.aux aldorug.lof \
		aldorug.log aldorug.idx aldorug.ilg
GENTEX=		msghelp.tex $(TEXEGS)
GENFILES=	$(UGEGS) $(TESTEGS) $(ALDORLIBEGS) $(CONTRIBEGS) $(OUTPUTS) \
		$(GENTEX)
ERASABLE=	$(TESTEGS) $(ALDORLIBEGS) $(CONTRIBEGS) $(OUTPUTS) $(TEXJUNK) \
		$(GENTEX) 

OKFILES=	Makefile $(SUBDIRS) $(TEXSRC) $(TEXJUNK) msghelp.tex \
		aldorug.dvi aldorug.ind aldorug.ps comsgdb.awk \
		aldorughtex.awk  TsCheck 

ifeq "$(ALDOR_DISTRIB)" ""
  ALDORUG_DEST = .
else
  ALDORUG_DEST = $(ALDOR_DISTRIB)/generic/1.0/doc
endif

#
# Generic rules
#
all:	aldorug.dvi

distrib: distribdir aldorug.dvi ps aldorug.pdf
	@ echo "Create aldorug.ps in $(ALDORUG_DEST)"
	mv aldorug.ps $(ALDORUG_DEST)
	@ echo "Create aldorug.pdf in $(ALDORUG_DEST)"
	mv aldorug.pdf $(ALDORUG_DEST)
#	dvipdf aldorug.dvi $(ALDORUG_DEST)/aldorug.pdf

distribdir:
	mkdir -p $(ALDORUG_DEST)

clean::
	@ rm -f *~ *.aux *.pdf *.idx *.ind *.lof *.log *.out *.toc

junk:
	@ for i in $(SUBDIRS) ; do  (cd $$i ; make junk) ; done
	@ ls -d $(OKFILES) * | sort | grep -v CVS | uniq -u

#
# Specific rules
#
genclean:
	@ rm -rf $(ERASABLE) *.ao

texclean:
	rm -f $(TEXJUNK)

newer:
	@ find . -newer TsCheck -type f -print

#samples::
#	-(cd samples ; $(MAKE) samples)
#
# The .dvi file for the User's Guide
#
aldorug.dvi: subdirs Makefile $(TEXSRC) $(TEXMACS) $(TEXEGS) $(OUTPUTS) $(GENTEX)
	echo '%' > aldorug.ind
	dolatex aldorug.tex
	makeindex aldorug
	dolatex aldorug.tex
	makeindex aldorug
	dolatex aldorug.tex
	#rm -f $(TEXJUNK)

aldorug.pdf: subdirs Makefile $(TEXSRC) $(TEXMACS) $(TEXEGS) $(OUTPUTS) $(GENTEX)
	echo '%' > aldorug.ind
	dopdflatex aldorug.tex
	makeindex aldorug
	dopdflatex aldorug.tex
	makeindex aldorug
	dopdflatex aldorug.tex
#	rm -f $(TEXJUNK)

aldorug0.dvi: subdirs Makefile $(TEXSRC) $(TEXMACS) $(TEXEGS) $(OUTPUTS) $(GENTEX)
	echo '%' > aldorug0.ind
	dolatex aldorug0.tex
	makeindex aldorug0

latex:	
	dolatex aldorug.tex

latex0:	
	dolatex aldorug0.tex

view:	aldorug.dvi
	xdvi aldorug.dvi

# PRT is taken from the environment.


ps:	aldorug.dvi
	dvips -o aldorug.ps aldorug.dvi 

aldorug.ind:
	echo '%' > aldorug.ind

subdirs: 
	for i in $(SUBDIRS) ; do (cd $$i; make) ; done


#arigato.out: arigato.as nputs.c
#	cc -c nputs.c
#	-axiomxl -Fx arigato.as nputs.o
#	-arigato >arigato.out
#	-rm arigato nputs.o
#
#cside.out:  cside.c aside.as
#	-axiomxl -Fo aside.as
#	-cc -I$$ALDORROOT/include -c cside.c
#	-cc cside.o aside.o -L$$ALDORROOT/lib -laldor -lfoam -lm
#	-a.out > cside.out
#	-rm cside.o aside.o a.out

msghelp.tex: $(SRCDIR)/comsgdb.msg comsgdb.awk
	awk -f comsgdb.awk $(SRCDIR)/comsgdb.msg


