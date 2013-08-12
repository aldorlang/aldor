libraries = algebra aldor

AXLTESTS =	\
	Trandom	\
	bug1335	\
	bug1342	\
	issue56	\
	#

BROKEN =	\
	bug1341	\
	#

#XFAIL_TESTS =	\
#	issue56/issue56	\
#	#

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
