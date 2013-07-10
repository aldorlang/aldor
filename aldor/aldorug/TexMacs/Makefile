#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

OKFILES = Makefile axiomxl.sty axiom.sty axiomps.sty scrunch.sty \
	  treedef.tex verbatim.sty acmconf.sty

all: 
	@ echon

clean::
	@ rm -f *~

junk::	
	@ ls -d $(OKFILES) * | sort | grep -v CVS | uniq -u

print:
	@ echon
