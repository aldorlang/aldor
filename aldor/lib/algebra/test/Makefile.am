libraries = algebra aldor

AXLTESTS =	\
	Trandom	\
	bug1335	\
	bug1342	\
	bugreport_1350	\
	issue56	\
	max	\
	modpbug	\
	optbug	\
	optbug2	\
	opteqbug	\
	primetest	\
	tstSprsUniPol	\
	#

BROKEN =	\
	bug1341	\
	bugExtend2	\
	bugreport_1369	\
	bugreport_1370	\
	pftest	\
	#

XFAIL_TESTS =	\
	#

include ../../testprog.am

tstSprsUniPol_tstSprsUniPol_SOURCES += tstSprsUniPol/support.c
tstSprsUniPol/tstSprsUniPol.c: tstSprsUniPol/support.c
CLEANFILES += tstSprsUniPol/support.c tstSprsUniPol/support.ao

progs: Makefile.am $(check_PROGRAMS)
	touch $<
