##############################################################################
#
# Makefile for the UserGuide/examples subdirectory
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

FILES	= simple1.as simple2.as simple3.as simple4a.as simple4b.as \
	  arigato.as aside.as choose.as compact.as comtest.as cside.c \
	  embed.as error0.as error0.out error1.as error1.out error2.as \
	  error3.as forn1.out lambertw.as matops.as moderr1 moderr2 \
	  moderr1.out moderr2.out \
	  modfact.as nputs.c poker.as rratio.as sieve.as testroun.as \
	  sizextd.as ax1.rec ax2.rec

OKFILES = Makefile $(FILES)

all:	

junk:
	@ ls -d $(OKFILES) * | sort | uniq -u
