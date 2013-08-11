libraries = axldem axllib

AXLTESTS =	\
	bug1089	\
	bug1168	\
	catdef2	\
	f21	\
	float2	\
	gbtest1	\
	hilbert1	\
	intfact	\
	iroots	\
	scobind1	\
	type6	\
	#

BROKEN =	\
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
