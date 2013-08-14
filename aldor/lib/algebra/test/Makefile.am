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
	primetest/primetest	\
	#

check_PROGRAMS = # filled by Tests.am
CLEANFILES = # filled by Tests.am
TESTS = $(check_PROGRAMS)

check_PROGRAMS += tst_complex
tst_complex_SOURCES =		\
	tst_complexmain.c	\
	tst_complex.c
CLEANFILES += $(tst_complex_SOURCES) tst_complex.ao

check_PROGRAMS += tst_mint
tst_mint_SOURCES =		\
	tst_mintmain.c		\
	tst_mint.c
CLEANFILES += $(tst_mint_SOURCES) tst_mint.ao

check_PROGRAMS += tst_random
tst_random_SOURCES =		\
	tst_randommain.c	\
	tst_random.c
CLEANFILES += $(tst_random_SOURCES) tst_random.ao

include ../../testprog.am

tstSprsUniPol_tstSprsUniPol_SOURCES += tstSprsUniPol/support.c
tstSprsUniPol/tstSprsUniPol.c: tstSprsUniPol/support.c
CLEANFILES += tstSprsUniPol/support.c tstSprsUniPol/support.ao

progs: Makefile.am $(check_PROGRAMS)
	touch $<
