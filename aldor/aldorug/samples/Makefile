##############################################################################
#
# Makefile for the UserGuide/samples subdirectory
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

.SUFFIXES: .as .tnb .out .msg

COMPILER=	aldor
TOOLDIR=	$(ALDORROOT)/toolbin
TMPDIR=		/tmp

NUMBER=		$(TOOLDIR)/number

CP=		cp -p
SamDir= 	$(ALDORROOT)/share/samples/UserGuide

CAT=		untab
#CAT=		cat

ALDOREGS=	cycle.as gens.as extend.as greet.as hello.as \
                imod.as mandel.as myfact.as objectb.as objectr.as \
		quanc8.as stack.as swap.as symbol.as \
                textin.as tree.as f77sort.as
ETCEGS=		badsieve.as number.as

TEXEGS=		$(ALDOREGS:.as=.tnb)
MSGS=		$(ALDOREGS:.as=.msg)
OUTPUTS=	$(ALDOREGS:.as=.out)

OKFILES=	$(ALDOREGS) $(TEXEGS) $(OUTPUTS) $(ETCEGS) Makefile

# standard rules

all:	$(TEXEGS) $(OUTPUTS)

clean:
	rm -f *~ *.ao 

genclean: clean
	rm -f *.tnb

reallyclean: genclean
	rm -f *.out *.msg

check:	$(MSGS)
	ls -s $(MSGS) | grep -v '^0'

junk:	
	@ ls -d $(OKFILES) * | sort | uniq -u

samples: $(SamDir)
	@echon "Copying samples"
	@for f in $(ALDOREGS) ; do \
		$(CP) $$f $(SamDir) ; done
	@echon "done"

$(SamDir):
	@echon "Making directory $(SamDir)."
	@mkdir $(SamDir)

# dependencies

$(NUMBER): number.as
	aldor -Fx -R$(TOOLDIR) -laxllib number.as
#	aldor -Fx -R$(TOOLDIR) -laldor number.as


$(TEXEGS): $(NUMBER)

# specific rules

# general rules

.as.tnb: 
	$(ALDORROOT)/toolbin/number < $*.as > $@

f77sort.out: f77sort.as
	@ echo "f77sort not compiled yet"

textin.out: textin.as
	aldor -laldor -G interp textin.as > textin.out

.as.out:
	aldor -laldor -G run $*.as > $@

$(MSGS): $(ALDORROOT)/bin/$(COMPILER)

.as.msg:
#	aldor -laldordem $*..as > $@
	aldor -laldor $*.as > $@
	rm $*.ao
