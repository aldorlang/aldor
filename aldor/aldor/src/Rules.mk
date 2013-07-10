THIS := $(dir $(lastword $(MAKEFILE_LIST)))

aldor_SOURCES =		\
	abcheck.c	\
	ablogic.c	\
	abnorm.c	\
	abpretty.c	\
	absub.c	\
	absyn.c	\
	abuse.c	\
	archive.c	\
	axl.c	\
	axlcomp.c	\
	axlobs.c	\
	bigint.c	\
	bigint_t.c	\
	bitv.c	\
	bitv_t.c	\
	bloop.c	\
	btree.c	\
	btree_t.c	\
	buffer.c	\
	buffer_t.c	\
	ccode.c	\
	ccode_t.c	\
	ccomp.c	\
	cfgfile.c	\
	cmdline.c	\
	compcfg.c	\
	compopt.c	\
	comsg.c	\
	comsgdb.c	\
	cport.c	\
	cport_t.c	\
	debug.c	\
	depdag.c	\
	dflow.c	\
	dnf.c	\
	dnf_t.c	\
	doc.c	\
	dword.c	\
	emit.c	\
	file.c	\
	file_t.c	\
	fint.c	\
	flatten.c	\
	float_t.c	\
	flog.c	\
	fluid.c	\
	fluid_t.c	\
	fname.c	\
	fname_t.c	\
	foam.c	\
	foamopt.c	\
	foam_c.c	\
	foam_cfp.c	\
	foam_i.c	\
	format.c	\
	format_t.c	\
	fortran.c	\
	freevar.c	\
	genc.c	\
	gencpp.c	\
	genfoam.c	\
	genlisp.c	\
	gf_add.c	\
	gf_excpt.c	\
	gf_fortran.c	\
	gf_gener.c	\
	gf_implicit.c	\
	gf_imps.c	\
	gf_prog.c	\
	gf_reference.c	\
	gf_rtime.c	\
	gf_seq.c	\
	include.c	\
	inlutil.c	\
	lib.c	\
	linear.c	\
	link_t.c	\
	list.c	\
	list_t.c	\
	loops.c	\
	macex.c	\
	main.c	\
	msg.c	\
	msg_t.c	\
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
	of_retyp.c	\
	of_rrfmt.c	\
	of_util.c	\
	opsys.c	\
	opsys_t.c	\
	optfoam.c	\
	opttools.c	\
	output.c	\
	parseby.c	\
	path.c	\
	phase.c	\
	priq.c	\
	priq_t.c	\
	scan.c	\
	scobind.c	\
	sefo.c	\
	sexpr.c	\
	simpl.c	\
	spesym.c	\
	srcline.c	\
	srcpos.c	\
	stab.c	\
	stdc.c	\
	store.c	\
	store1_t.c	\
	store2_t.c	\
	store3_t.c	\
	strops.c	\
	strops_t.c	\
	symbol.c	\
	symbol_t.c	\
	syme.c	\
	syscmd.c	\
	table.c	\
	table_t.c	\
	tconst.c	\
	termtype.c	\
	terror.c	\
	test.c	\
	textansi.c	\
	textcolour.c	\
	texthp.c	\
	tform.c	\
	tfsat.c	\
	timer.c	\
	tinfer.c	\
	ti_bup.c	\
	ti_decl.c	\
	ti_sef.c	\
	ti_tdn.c	\
	token.c	\
	tposs.c	\
	tqual.c	\
	usedef.c	\
	util.c	\
	util_t.c	\
	version.c	\
	xfloat.c	\
	xfloat_t.c

aldor_OBJECTS := $(addprefix $(THIS), $(aldor_SOURCES:.c=.o))

aldor_HEADERS =	\
	cconfig.h	\
	foam_c.h	\
	foamopt.h	\
	optcfg.h	\
	aldor.conf

aldor_HEADERS := $(addprefix build/include/, $(aldor_HEADERS))

build/aldor: $(aldor_OBJECTS)
	mkdir -p $(dir $@)
	$(LINK.c) $^ -lm -o $@

build/include/%: $(THIS)%
	mkdir -p $(dir $@)
	cp $< $@


libruntime_SOURCES =	\
	bigint.c	\
	btree.c	\
	buffer.c	\
	dword.c	\
	foam_c.c	\
	foam_cfp.c	\
	foam_i.c	\
	format.c	\
	list.c	\
	opsys.c	\
	output.c	\
	stdc.c	\
	store.c	\
	strops.c	\
	table.c	\
	timer.c	\
	util.c	\
	xfloat.c

libruntime_OBJECTS := $(addprefix $(THIS), $(libruntime_SOURCES:.c=.o))

build/libruntime.a: $(libruntime_OBJECTS)
	$(AR) cr $@ $^


clean: clean-$(THIS)
clean-$(THIS):
	$(RM) $(libruntime_OBJECTS)
	$(RM) $(aldor_OBJECTS)
	$(RM) build/aldor build/libruntime.a
