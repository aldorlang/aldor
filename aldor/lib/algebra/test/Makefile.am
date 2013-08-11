libraries = algebra aldor

AXLTESTS =	\
	Trandom	\
	bug1335	\
	bug1342	\
	#

BROKEN =	\
	bug1341	\
	#

check_PROGRAMS = # filled by Tests.am
CLEANFILES = # filled by Tests.am
TESTS = $(check_PROGRAMS)

include Tests.am
$(srcdir)/Tests.am: Makefile.am
	truncate -s0 $@
	for test in $(AXLTESTS); do			\
	  echo "check_PROGRAMS += $$test/$$test" >> $@;	\
	  echo "$${test}_$${test}_SOURCES = $$test/$${test}main.c $$test/$${test}.c" >> $@;	\
	  echo "CLEANFILES += $$test/$${test}main.c $$test/$${test}.c $$test/$${test}.ao" >> $@;	\
	done

check_PROGRAMS += tst_complex
tst_complex_SOURCES =		\
	tst_complexmain.c	\
	tst_complex.c

check_PROGRAMS += tst_mint
tst_mint_SOURCES =		\
	tst_mintmain.c		\
	tst_mint.c

check_PROGRAMS += tst_random
tst_random_SOURCES =		\
	tst_randommain.c	\
	tst_random.c

include ../../testprog.am
