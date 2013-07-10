##############################################################################
#
# Makefile
#
# Make program to compile the Frisco runtime 
#
# Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#
#
##############################################################################
#
# Targets: all, junk, clean, quick
#
#    make all	-- make the runtime files for the Aldor compiler
#    make junk	-- list files which are not part of the Aldor compiler
#    make clean -- remove generated files  (e.g. foo.c~)
#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

SHELL	= /bin/sh

OKFILES	= Makefile foam

all: foamdir

clean::
	@ rm -f *~

junk::	
	@ ls -d $(OKFILES) * | sort | grep -v CVS | uniq -u

foamdir: 
	-(cd foam ; $(MAKE) all)
clean::
	-(cd foam ; $(MAKE) clean)
junk::
	-(cd foam ; $(MAKE) junk)
