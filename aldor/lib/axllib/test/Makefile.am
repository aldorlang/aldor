
ARGS=-Nfile=$(abs_top_builddir)/aldor/src/aldor \
	-Y$(abs_top_builddir)/lib/axllib/src/al \
	-Y$(abs_top_builddir)/lib/axllib/src \
	-I$(abs_top_builddir)/lib/axllib/include \
	-I$(abs_top_builddir)/aldor/src \
	-Y$(abs_top_builddir)/aldor/lib/libfoam \
	-Cargs="-Wconfig=$(abs_top_builddir)/aldor/src/aldor.conf" \
	-Cstandard

TESTS_ENVIRONMENT = ALDORARGS="$(ARGS)" \
			$(abs_top_builddir)/aldor/subcmd/testaldor/testaldor \
					-oldaldor $(abs_top_builddir)/aldor/src/aldor \
					-aldor $(abs_top_builddir)/aldor/src/aldor \
					-cc $(abs_top_builddir)/aldor/subcmd/unitools/unicl

TESTS = 1test.as test0.as test1.as srcpos.as \
	triv0.as triv1.as triv3.as triv4.as triv5.as triv6.as \
	hilbert.as intbug1.as \
	opt0.as opt1.as opt2.as opt3.as\
	genc0.sh genc1.as genc2.as \
	imod0.as table1.as format1.as format2.as \
	junk0.as \
	missing0.as missing1.as halt0.as halt1.as \
	dnames.as hash0.as \
	error0.as error1.as \
	hide0.as \
	domain0.as domain1.as domain2.as domain3.as \
	mandel.as bigmand.as \
	string-t.as ratio0.as doc0.as \
	oslowtst.as \
	cmdline0.sh \
	fopt.sh fopt-ax.sh fopt-ai.sh fopt-fm.sh fopt-mc.sh \
	multi.sh errorax.sh \
	include0.as include1.as include2.as include3.as \
	scan0.as scan1.as scan2.as scan3.as scan4.as scan5.as scan6.as \
	linear0.as linear1.as linear2.as linear3.as linear4.as linear5.as \
	parse0.as parse1.as parse2.as parse3.as parse4.as \
	macex0.as macex1.as macex2.as macex3.as macex4.as macex5.as \
	macex6.as macex7.as \
	abcheck0.as abcheck1.as abcheck2.as abcheck3.as \
	abnorm0.as abnorm1.as abnorm2.as \
	scobind0.as scobind1.as scobind2.as scobind3.as scobind4.as \
	scobind5.as scobind6.as \
	scope0.as scope1.as scope2.as scope3.as scope5.as \
	scope7.as scope8.as scope9.as scope10.as scope11.as scope12.as \
	tinfer0.as tinfer1.as tinfer2.as \
	fluid0.as enum0.as enum1.as union0.as union1.as union2.as \
	catdef0.as catdef1.as catdef3.as self0.as grok.as \
	ovload0.as ovload1.as ovload2.as \
	depend0.as depend1.as depend2.as depend3.as depend4.as depend5.as \
	depend6.as depend7.as depend8.as depend9.as \
	type0.as type1.as type2.as type3.as type4.as type5.as \
	type7.as type8.as \
	default0.as default1.as default2.as default3.as \
	default4.as default5.as default6.as default7.as \
	default9.as \
	cascade0.as cascade1.as cascade2.as cascade3.as cascade4.as \
	swap.as swap1.as ladder0.as cycle0.as \
	loop0.as loop1.as loop2.as loop3.as collect0.as collect1.as \
	genops.as \
	exit0.as exit1.as exit2.as \
	none0.as none1.as none2.as \
	if0.as if1.as if2.as if3.as if4.as if5.as \
	ifcross0.as \
	tuple0.as tuple1.as tuple2.as tuple3.as tuple4.as \
	embed.as embed1.as \
	impl.as \
	const0.as const1.as const2.as \
	const3.as const4.as const5.as \
	const6.as const7.as const8.as \
	defgroup0.as defgroup1.as defgroup2.as defgroup3.as \
	pack0.sh pack0.as pack1.as pack2.as \
	binadd.as qual0.as qual1.as qual2.as \
	try0.as	\
	libdup0.as libdup1.as \
	numeral0.as numeral1.as numeral2.as \
	ar0.sh ar1.sh ar2.sh ar3.sh ar4.sh ar5.sh ar6.sh \
	arrepl0.sh arrepl1.sh \
	session1.sh session2.sh session3.sh session4.sh session5.sh \
	session6.sh \
	flow0.as goto0.as \
	funvar.as	\
	qimport.as \
	eager0.sh \
	where0.as where1.as where3.as \
	builtin0.as builtin1.as \
	gen0.as gen1.as gen2.as gen3.as gen4.as \
	lit0.as lit1.as \
	forn0.as forn1.as forn2.as forn3.as forn4.as \
	forn5.as forn5o.as forn6.as forn6o.as forn7.as \
	forn7a.as forn8.sh forn9.sh forn10.sh\
	inline0.as inline1.as inline2.as inline3.as inline4.as inline5.as \
	basic.as complex.as fix0.as fix1.as \
	float0.as float1.as float3.as float4.as df1.as \
	big.as fact.as array0.as array1.as \
	f11.as \
	msg.sh limits.as \
	exn1.as exn2.as exn3.as exn4.as exn5.as exn6.as \
	emit0.sh emit1.sh respfile.sh \
	emerge0.as \
	t1009.as t1024.as t1053.as t1059.as t1106.as \
	t672.as t715.as t897.as t1142.as t1145.as t1149.as t1157.sh t1158.as\
	t934.as t936.as t944.as t947.as t950.as t958.as t966.as t967.as \
	t973.as t986.as t1099.as t1105.as ddata.as 


