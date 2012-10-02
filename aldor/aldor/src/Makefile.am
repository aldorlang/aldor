#SUBDIRS = . java

bin_PROGRAMS =  javagen aldor

javagen_SOURCES = java/main.c java/genjava.c java/javacode.c		\
		java/javaobj.c axlcomp.c cmdline.c

javagen_LDADD = libphase.a libstruct.a libgen.a libport.a -lm
javagen_LDFLAGS =
javagen_CFLAGS = -g
include_HEADERS=foam_c.h cconfig.h platform.h optcfg.h foamopt.h basic.typ


aldor_SOURCES = main.c axlcomp.c cmdline.c
aldor_LDADD = libphase.a libstruct.a libgen.a libport.a -lm
aldor_LDFLAGS = 
aldor_CFLAGS  = -g

noinst_LIBRARIES = libphase.a libstruct.a libgen.a libport.a 

aldortest_CFLAGS=-DTEST_STAND_ALONE -DTEST_ALL --save-temps
aldortest_SOURCES = test.c
aldortest_LDADD = libtest.a libstruct.a libgen.a libport.a -lm
aldortest_LDFLAGS = 

libport_a_SOURCES = stdc.c cport.c opsys.c

libtest_a_CFLAGS  = -DTEST_ALL
libtest_a_SOURCES = bigint_t.c bitv_t.c btree_t.c buffer_t.c ccode_t.c	\
	            cport_t.c dnf_t.c file_t.c float_t.c fluid_t.c	\
	            fname_t.c format_t.c link_t.c list_t.c main_t.c	\
	            msg_t.c opsys_t.c priq_t.c store1_t.c store2_t.c	\
	            store3_t.c strops_t.c symbol_t.c table_t.c		\
	            util_t.c xfloat_t.c

# Foam stuff does not belong!
libgen_a_SOURCES = debug.c format.c fluid.c util.c store.c memclim.c	\
		test.c foam_c.c foam_i.c foam_cfp.c foamopt.c		\
		compopt.c dword.c bigint.c bitv.c btree.c buffer.c	\
		dnf.c file.c fname.c intset.c list.c msg.c path.c	\
		srcpos.c strops.c symbol.c table.c ccode.c sexpr.c	\
		xfloat.c priq.c timer.c cfgfile.c termtype.c		\
		textansi.c texthp.c textcolour.c ostream.c

#libgen_a_LIBADD = libport.a

gentest_SOURCES=gentest.c
gentest_LDADD=libgen.a libport.a

libstruct_a_SOURCES = gf_syme.c ti_top.c fbox.c \
		axlobs.c srcline.c token.c doc.c absyn.c absub.c ablogic.c \
		fint.c output.c simpl.c compcfg.c depdag.c \
		sefo.c syme.c forg.c freevar.c tform.c tfcond.c tfsat.c tposs.c tconst.c \
		tqual.c stab.c lib.c archive.c foam.c foamsig.c flog.c dflow.c spesym.c \
		abpretty.c version.c comsg.c loops.c comsgdb.c
#libstruct_a_LIBADD = libgen.a libport.a
structtest_SOURCES=structtest.c
structtest_LDADD=libstruct.a libgen.a libport.a -lm
structtest_LDFLAGS =

libphase_a_SOURCES = phase.c bloop.c fintphase.c \
		include.c scan.c syscmd.c linear.c parseby.c macex.c \
		abnorm.c abcheck.c abuse.c scobind.c \
		ti_decl.c \
		tinfer.c ti_bup.c ti_tdn.c ti_sef.c terror.c \
		genfoam.c gf_fortran.c gf_add.c gf_gener.c gf_imps.c \
		gf_excpt.c gf_reference.c gf_implicit.c \
		gf_prog.c gf_rtime.c gf_seq.c opttools.c \
		optfoam.c of_util.c  of_inlin.c of_cfold.c of_hfold.c \
		of_emerg.c of_env.c of_cprop.c of_jflow.c of_peep.c \
		of_deadv.c of_comex.c of_loops.c of_retyp.c of_deada.c \
		of_rrfmt.c of_killp.c \
		emit.c ccomp.c usedef.c flatten.c \
		inlutil.c genc.c genlisp.c fortran.c gencpp.c \
		axl_y.c 

#libphase_a_LIBADD = libstruct.a libgen.a libport.a

#
# TESTS
#
check_PROGRAMS=testall
TESTS=testall
testsuite = \
	test/test_ablogic.c 	\
	test/test_absyn.c 	\
	test/test_bitv.c 	\
	test/test_fname.c 	\
	test/test_foam.c 	\
	test/test_format.c	\
	test/test_flog.c	\
	test/test_jflow.c	\
	test/test_list.c	\
	test/test_ostream.c	\
	test/test_printf.c	\
	test/test_scobind.c	\
	test/test_srcpos.c	\
	test/test_stab.c	\
	test/test_syme.c	\
	test/test_tform.c	\
	test/test_tibup.c	\
	test/test_tinfer.c

testall_SOURCES = test/testall.c $(testsuite) test/testlib.c test/abquick.c cmdline.c axlcomp.c
testall_LDADD = libphase.a libstruct.a libgen.a libport.a -lm
testall_LDFLAGS = 
testall_CFLAGS = -save-temps
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

comsgdb.h comsgdb.c: $(srcdir)/comsgdb.msg $(tmpdir)
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

clean:
	rm -f *.i
	rm -f *.s

comsgpig.cat: comsgdb.msg	
	($(builddir)/../tools/unix/atinlay < $(srcdir)/comsgdb.msg > comsgpig.msg ; \
	 $(builddir)/../tools/unix/msgcat -cat -detab comsgpig ; \
	 rm comsgpig.msg )
