##############################################################################
#
# Makefile for the UserGuide/pictures subdirectory
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

OKFILES= Makefile axlcat.ps axlcat.ps0

all:

clean:
	rm -f *~

junk:	
	@ ls -d $(OKFILES) * | sort | uniq -u
