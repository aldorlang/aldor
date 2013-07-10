#
# E.g. setenv MACHINE rs ; make -e
#
# See src/Makefile for the supported MACHINEs.
#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

SHELL	= /bin/sh

OKFILES	= Makefile frisco gmp

all:	friscodir gmpdir

clean::
	@ rm -f *~

junk::	
	@ ls -d $(OKFILES) * | sort | grep -v CVS | uniq -u


friscodir: 
	-(cd frisco ; $(MAKE) all)
clean::
	-(cd frisco ; $(MAKE) clean)
junk::
	-(cd frisco ; $(MAKE) junk)
samples::
	-(cd frisco ; $(MAKE) samples)


gmpdir: 
	-(cd gmp ; $(MAKE) all)
clean::
	-(cd gmp ; $(MAKE) clean)
junk::
	-(cd gmp ; $(MAKE) junk)
samples::
	-(cd gmp ; $(MAKE) samples)
