#
# Relative include files
# 
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

SHELL=/bin/sh

OKFILES = Makefile \
	Z include0.as includeA.as includeB.as includeC.as

all: 
	@ echon

clean::
	@ rm -f *~

junk::
	@ ls -d $(OKFILES) * | sort | grep -v CVS | uniq -u


clean::	; -(cd Z ; $(MAKE) clean)
junk::	; -(cd Z ; $(MAKE) junk)
