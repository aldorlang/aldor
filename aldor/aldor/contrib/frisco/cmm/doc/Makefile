#
# Reckless makefiles 'R' us
#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

DocDir   = $(ALDORROOT)/share/doc/cmm

Files    = CMMguide.tex SPE.ps
samples:
	mkdir $(DocDir)
	cp $(Files) $(DocDir)

