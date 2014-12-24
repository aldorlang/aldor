#SUBDIRS = . java

AM_CFLAGS=$(STRICTCFLAGS)

libexec_PROGRAMS = aldor
bin_PROGRAMS = javagen
noinst_PROGRAMS = aldortest structtest gentest showexp

dataincludedir = $(datadir)/include
datainclude_DATA = aldor.conf 

datalib_DATA = comsgpig.cat
datalibdir = $(datadir)/lib

include_HEADERS =	\
	basic.typ	\
	cconfig.h	\
	foam_c.h	\
	foamopt.h	\
	optcfg.h	\
	platform.h

noinst_LIBRARIES =	\
	libgen.a	\
	libphase.a	\
	libport.a	\
	libstruct.a	\
	libtest.a

AM_CPPFLAGS = -DVCSVERSION='"$(VCSVERSION)"'

javagen_SOURCES =	\
	java/genjava.c	\
	java/javacode.c	\
	java/javaobj.c	\
	java/main.c	\
	axlcomp.c	\
	cmdline.c

javagen_LDADD = libphase.a libstruct.a libgen.a libport.a -lm
javagen_LDFLAGS =
javagen_CFLAGS = -g $(STRICTCFLAGS)

showexp_SOURCES = showexports.c
showexp_LDADD = libphase.a libstruct.a libgen.a libport.a -lm
showexp_LDFLAGS =
showexp_CFLAGS = -g $(STRICTCFLAGS)

aldor_SOURCES =		\
	axlcomp.c	\
	cmdline.c	\
	main.c
aldor_LDADD = libphase.a libstruct.a libgen.a libport.a -lm
aldor_LDFLAGS = $(build_id)
aldor_CFLAGS  = -g $(STRICTCFLAGS)

aldortest_CFLAGS = -DTEST_STAND_ALONE -DTEST_ALL -save-temps $(STRICTCFLAGS)
aldortest_SOURCES = test.c
aldortest_LDADD = libtest.a libstruct.a libgen.a libport.a -lm
aldortest_LDFLAGS = 

libport_a_SOURCES =	\
	cport.c		\
	opsys.c		\
	stdc.c

libtest_a_CFLAGS  = -DTEST_ALL $(STRICTCFLAGS)
libtest_a_SOURCES =	\
	bigint_t.c	\
	bitv_t.c	\
	btree_t.c	\
	buffer_t.c	\
	ccode_t.c	\
	cport_t.c	\
	dnf_t.c		\
	file_t.c	\
	float_t.c	\
	fluid_t.c	\
	fname_t.c	\
	format_t.c	\
	link_t.c	\
	list_t.c	\
	main_t.c	\
	msg_t.c		\
	opsys_t.c	\
	priq_t.c	\
	store1_t.c	\
	store2_t.c	\
	store3_t.c	\
	strops_t.c	\
	symbol_t.c	\
	table_t.c	\
	util_t.c	\
	xfloat_t.c

# Foam stuff does not belong!
libgen_a_SOURCES =	\
	bigint.c	\
	bitv.c		\
	btree.c		\
	buffer.c	\
	ccode.c		\
	cfgfile.c	\
	compopt.c	\
	debug.c		\
	dnf.c		\
	dword.c		\
	errorset.c	\
	file.c		\
	fluid.c		\
	fname.c		\
	foam_c.c	\
	foam_cfp.c	\
	foam_i.c	\
	foamopt.c	\
	format.c	\
	int.c		\
	intset.c	\
	java/javacode.c	\
	java/javaobj.c	\
	list.c		\
	memclim.c	\
	msg.c		\
	ostream.c	\
	path.c		\
	priq.c		\
	sexpr.c		\
	srcpos.c	\
	store.c		\
	strops.c	\
	symbol.c	\
	table.c		\
	termtype.c	\
	test.c		\
	textansi.c	\
	textcolour.c	\
	texthp.c	\
	timer.c		\
	util.c		\
	xfloat.c

#libgen_a_LIBADD = libport.a

gentest_SOURCES = gentest.c
gentest_LDADD = libgen.a libport.a -lm

libstruct_a_SOURCES =	\
	ablogic.c	\
	abpretty.c	\
	absub.c		\
	absyn.c		\
	archive.c	\
	axlobs.c	\
	compcfg.c	\
	comsg.c		\
	comsgdb.c	\
	depdag.c	\
	dflow.c		\
	doc.c		\
	fbox.c		\
	fint.c		\
	flog.c		\
	foam.c		\
	foamsig.c	\
	forg.c		\
	freevar.c	\
	formatters.c	\
	ftype.c		\
	gf_syme.c	\
	lib.c		\
	loops.c		\
	output.c	\
	sefo.c		\
	simpl.c		\
	spesym.c	\
	srcline.c	\
	stab.c		\
	syme.c		\
	symcoinfo.c	\
	tconst.c	\
	tfcond.c	\
	tform.c		\
	tfsat.c		\
	ti_top.c	\
	token.c		\
	tposs.c		\
	tqual.c		\
	utype.c		\
	version.c

#libstruct_a_LIBADD = libgen.a libport.a
structtest_SOURCES = structtest.c
structtest_LDADD = libstruct.a libgen.a libport.a -lm
structtest_LDFLAGS =

libphase_a_SOURCES =	\
	abcheck.c	\
	abnorm.c	\
	abuse.c		\
	axl_y.c		\
	bloop.c		\
	ccomp.c		\
	emit.c		\
	fintphase.c	\
	flatten.c	\
	fortran.c	\
	genc.c		\
	gencpp.c	\
	genfoam.c	\
	genlisp.c	\
	java/genjava.c	\
	gf_add.c	\
	gf_excpt.c	\
	gf_fortran.c	\
	gf_gener.c	\
	gf_implicit.c	\
	gf_imps.c	\
	gf_java.c	\
	gf_prog.c	\
	gf_reference.c	\
	gf_rtime.c	\
	gf_seq.c	\
	include.c	\
	inlutil.c	\
	linear.c	\
	macex.c		\
	of_argsub.c	\
	of_cfold.c	\
	of_comex.c	\
	of_cprop.c	\
	of_deada.c	\
	of_deadv.c	\
	of_emerg.c	\
	of_env.c	\
	of_hfold.c	\
	of_inlin.c	\
	of_jflow.c	\
	of_killp.c	\
	of_loops.c	\
	of_peep.c	\
	of_retyp2.c	\
	of_rrfmt.c	\
	of_util.c	\
	optfoam.c	\
	opttools.c	\
	parseby.c	\
	phase.c		\
	scan.c		\
	scobind.c	\
	syscmd.c	\
	terror.c	\
	ti_bup.c	\
	ti_decl.c	\
	ti_sef.c	\
	ti_tdn.c	\
	tinfer.c	\
	usedef.c

#libphase_a_LIBADD = libstruct.a libgen.a libport.a

#
# TESTS
#
check_PROGRAMS = testall
TESTS = testall
testsuite =			\
	test/test_abcheck.c	\
	test/test_ablogic.c 	\
	test/test_abnorm.c	\
	test/test_absyn.c 	\
	test/test_archive.c	\
	test/test_bigint.c 	\
	test/test_bitv.c 	\
	test/test_errorset.c	\
	test/test_float.c	\
	test/test_flog.c	\
	test/test_fname.c 	\
	test/test_foam.c 	\
	test/test_format.c	\
	test/test_genfoam.c	\
	test/test_jflow.c	\
	test/test_java.c	\
	test/test_int.c		\
	test/test_list.c	\
	test/test_ostream.c	\
	test/test_printf.c	\
	test/test_retyp.c	\
	test/test_scobind.c	\
	test/test_srcpos.c	\
	test/test_stab.c	\
	test/test_syme.c	\
	test/test_tform.c	\
	test/test_tibup.c	\
	test/test_tfsat.c	\
	test/test_tinfer.c	\
	test/test_utype.c

testall_SOURCES =	\
	$(testsuite)	\
	test/abquick.c	\
	test/testall.c	\
	test/testlib.c	\
	cmdline.c	\
	axlcomp.c
testall_LDADD = libphase.a libstruct.a libgen.a libport.a -lm
testall_LDFLAGS = 
testall_CFLAGS = -save-temps $(STRICTCFLAGS)
#
# Special rules
#


grammar axl_y.c axl_y.yt: axl.z token.h axl_y.sed
	$(abs_top_builddir)/aldor/tools/unix/zacc -p -y axl_y.yt -c axl_y.c $(srcdir)/axl.z
	@sed -f $(srcdir)/axl_y.sed axl_y.c > axl_y.c.tmp
	@if diff axl_y.c axl_y.c.tmp > /dev/null ; then \
		rm $axl_y.c.tmp ; \
	else \
		echo "Applied fixups to axl_y.c" ; \
		mv axl_y.c.tmp axl_y.c  ; \
	fi

tmpdir=$(abs_top_builddir)/build/tmp
toolexedir=$(abs_top_builddir)/aldor/tools/unix

DUMMY:
$(tmpdir): DUMMY
	mkdir -p $(tmpdir)

# make sure these are built before things are compiled
BUILT_SOURCES = comsgdb.h comsgdb.c
CLEANFILES = $(BUILT_SOURCES)

comsgdb.h: comsgdb.c
comsgdb.c: $(srcdir)/comsgdb.msg $(tmpdir)
	@if [ ! -f comsgdb.h ] ; then touch comsgdb.h ; fi
	@if [ ! -f comsgdb.c ] ; then touch comsgdb.c ; fi
	@cp $(srcdir)/comsgdb.msg $(tmpdir)/comsgdb.msg
	@(cd $(tmpdir); $(toolexedir)/msgcat -h -c -detab comsgdb)
	@if diff comsgdb.h $(tmpdir)/comsgdb.h > /dev/null ; then \
		rm $(tmpdir)/comsgdb.h ; \
	else \
		echo "Created new version of comsgdb.h" ; \
		mv $(tmpdir)/comsgdb.h . ; \
	fi
	@if diff comsgdb.c $(tmpdir)/comsgdb.c > /dev/null ; then \
		rm $(tmpdir)/comsgdb.c ; \
	else \
		echo "Created new version of comsgdb.c" ; \
		mv $(tmpdir)/comsgdb.c . ; \
	fi

clean-local:
	rm -f $(addsuffix .i,$(notdir $(testall_SOURCES))) $(addsuffix .s,$(notdir $(testall_SOURCES)))
	rm -f *.i
	rm -f *.o

comsgpig.cat: $(srcdir)/comsgdb.msg
	($(builddir)/../tools/unix/atinlay < $(srcdir)/comsgdb.msg > comsgpig.msg ; \
	 $(builddir)/../tools/unix/msgcat -cat -detab comsgpig ; \
	 rm comsgpig.msg )
