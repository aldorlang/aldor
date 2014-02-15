libraries = aldor

AXLTESTS =	\
	bug1332	\
	bug1333	\
	bug1334	\
	bug1337	\
	bug1340	\
	bugExtend1	\
	bugFree	\
	bugReturn	\
	bugreport_1	\
	bugreport_5	\
	bugreport_7	\
	bugreport_13	\
	bugreport_14	\
	bugreport_21	\
	bugreport_25	\
	bugreport_1351	\
	bugreport_1356	\
	bugreport_1362	\
	expt	\
	intfact	\
	issue2	\
	issue38	\
	localcoerce	\
	removebug	\
	removebug2	\
	testargs	\
	type-constant	\
	cond-defaults	\
	cond	\
	cond2   \
	hang	\
	ret-exit \
	tst_integer \
	union-print \
	#

BROKEN =	\
	RepRecordError	\
	avl-set	\
	avl-set2	\
	bugCklst	\
	bugreport_1a	\
	bugreport_15	\
	bugreport_1347	\
	bugreport_1363	\
	has-bug1	\
	poly-type-expt	\
	testbug	\
	#

if HAVE_ALDOR_GC
GC_XFAIL_TESTS=issue2/issue2
endif

XFAIL_TESTS =		\
	bugExtend1/bugExtend1	\
	bugFree/bugFree	\
	bugreport_5/bugreport_5	\
	cond-defaults/cond-defaults \
	$(GC_XFAIL_TESTS) \
	#

check_PROGRAMS = # filled by Tests.am
CLEANFILES = # filled by Tests.am
TESTS = $(check_PROGRAMS)

include ../../testprog.am

cond/cond.c: cond/cond1.c
cond_cond_SOURCES += cond/cond1.c
