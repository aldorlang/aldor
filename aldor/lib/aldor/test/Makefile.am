libraries = aldor

AXLTESTS =	\
	bug1332	\
	bug1333	\
	bug1334	\
	bug1337	\
	bug1340	\
	intfact	\
	issue2	\
	issue38	\
	#

XFAIL_TESTS =		\
	issue2/issue2	\
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

include ../../testprog.am
