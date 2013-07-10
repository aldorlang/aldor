THIS := $(dir $(lastword $(MAKEFILE_LIST)))

libalgebra_ASOURCES :=	\
	algext/sit_algext.as	\
	algext/sit_sae.as	\
	algext/sit_saexcpt.as	\
	algext/sit_upmod.as	\
	basic/compbug/sit_interp.as	\
	basic/compbug/sit_shell.as	\
	basic/sit_complex.as	\
	basic/sit_indvar.as	\
	basic/sit_mkpring.as	\
	basic/sit_permut.as	\
	basic/sit_pring.as	\
	basic/sit_product.as	\
	categories/alg_cansimp.as	\
	categories/alg_ffield.as	\
	categories/alg_frering.as	\
	categories/alg_idxfrng.as	\
	categories/alg_modcmp.as	\
	categories/alg_primsrc.as	\
	categories/alg_rescls.as	\
	categories/alg_rring.as	\
	categories/sit_abgroup.as	\
	categories/sit_abmon.as	\
	categories/sit_algebra.as	\
	categories/sit_automor.as	\
	categories/sit_basic.as	\
	categories/sit_char0.as	\
	categories/sit_charp.as	\
	categories/sit_chrem.as	\
	categories/sit_comring.as	\
	categories/sit_dcmprng.as	\
	categories/sit_deriv.as	\
	categories/sit_diffext.as	\
	categories/sit_difring.as	\
	categories/sit_euclid.as	\
	categories/sit_field.as	\
	categories/sit_freealg.as	\
	categories/sit_freelar.as	\
	categories/sit_freelc.as	\
	categories/sit_freemod.as	\
	categories/sit_fset.as	\
	categories/sit_gcd.as	\
	categories/sit_gexpcat.as	\
	categories/sit_group.as	\
	categories/sit_idxfalg.as	\
	categories/sit_idxflar.as	\
	categories/sit_idxflc.as	\
	categories/sit_idxfmod.as	\
	categories/sit_intdom.as	\
	categories/sit_integer.as	\
	categories/sit_linarit.as	\
	categories/sit_module.as	\
	categories/sit_monoid.as	\
	categories/sit_ncid.as	\
	categories/sit_pable.as	\
	categories/sit_prfcat0.as	\
	categories/sit_ptools.as	\
	categories/sit_qring.as	\
	categories/sit_ring.as	\
	categories/sit_spf0.as	\
	categories/sit_spfcat0.as	\
	categories/sit_spzble.as	\
	extree/operators/sit_OPand.as	\
	extree/operators/sit_OPassgn.as	\
	extree/operators/sit_OPbigO.as	\
	extree/operators/sit_OPcase.as	\
	extree/operators/sit_OPcplex.as	\
	extree/operators/sit_OPequal.as	\
	extree/operators/sit_OPexpt.as	\
	extree/operators/sit_OPfact.as	\
	extree/operators/sit_OPif.as	\
	extree/operators/sit_OPless.as	\
	extree/operators/sit_OPlist.as	\
	extree/operators/sit_OPllist.as	\
	extree/operators/sit_OPmatrx.as	\
	extree/operators/sit_OPminus.as	\
	extree/operators/sit_OPmore.as	\
	extree/operators/sit_OPnoteq.as	\
	extree/operators/sit_OPplus.as	\
	extree/operators/sit_OPprefx.as	\
	extree/operators/sit_OPquot.as	\
	extree/operators/sit_OPsubsc.as	\
	extree/operators/sit_OPtimes.as	\
	extree/operators/sit_OPvect.as	\
	extree/parser/sit_infexpr.as	\
	extree/parser/sit_lspexpr.as	\
	extree/parser/sit_maple.as	\
	extree/parser/sit_parser.as	\
	extree/parser/sit_scanner.as	\
	extree/parser/sit_token.as	\
	extree/sit_extree.as	\
	extree/sit_optools.as	\
	ffield/alg_pf2.as	\
	ffield/sit_prfcat.as	\
	ffield/sit_spf.as	\
	ffield/sit_sprfcat.as	\
	ffield/sit_sprfgcd.as	\
	ffield/sit_sprfmat.as	\
	ffield/sit_zpf.as	\
	fraction/sit_lcqotct.as	\
	fraction/sit_matquot.as	\
	fraction/sit_qotbyc0.as	\
	fraction/sit_qotbyct.as	\
	fraction/sit_qotfcat.as	\
	fraction/sit_qotfct0.as	\
	fraction/sit_qotient.as	\
	fraction/sit_quotby.as	\
	fraction/sit_quotcat.as	\
	fraction/sit_uflgqot.as	\
	fraction/sit_vecquot.as	\
	mat/gauss/sit_dfge.as	\
	mat/gauss/sit_ff2ge.as	\
	mat/gauss/sit_ffge.as	\
	mat/gauss/sit_hermge.as	\
	mat/gauss/sit_linelim.as	\
	mat/gauss/sit_oge.as	\
	mat/linalg2/alg_ffupla.as	\
	mat/linalg2/sit_popov.as	\
	mat/linalg2/sit_upcrtla.as	\
	mat/linalg3/sit_hensela.as	\
	mat/linalg/sit_bsolve.as	\
	mat/linalg/sit_laring.as	\
	mat/linalg/sit_linalg.as	\
	mat/linalg/sit_overdet.as	\
	mat/modular/compbug/sit_speclin.as	\
	mat/modular/sit_modpoge.as	\
	mat/sit_dnsemat.as	\
	mat/sit_matcat.as	\
	mat/sit_vector.as	\
	multpoly/exponent/sm_dirprod.as	\
	multpoly/exponent/sm_dirprodc.as	\
	multpoly/exponent/sm_expocat.as	\
	multpoly/exponent/sm_fvt.as	\
	multpoly/exponent/sm_listovar.as	\
	multpoly/exponent/sm_midl.as	\
	multpoly/exponent/sm_midrl.as	\
	multpoly/exponent/sm_mievc.as	\
	multpoly/exponent/sm_milex.as	\
	multpoly/exponent/sm_osymbol.as	\
	multpoly/exponent/sm_tuplovar.as	\
	multpoly/exponent/sm_vt.as	\
	multpoly/exponent/sm_zevc.as	\
	multpoly/multpolycat/alg_defgcd.as	\
	multpoly/multpolycat/alg_poltype.as	\
	multpoly/multpolycat/alg_stdfrng.as	\
	multpoly/multpolycat/sm_famr0.as	\
	multpoly/multpolycat/sm_polring0.as	\
	multpoly/multpolydata/sm_delist.as	\
	multpoly/multpolydom/sm_dmp0.as	\
	multpoly/multpolydom/sm_dmp1.as	\
	multpoly/multpolypkg/alg_bivarpk.as	\
	multpoly/multpolypkg/alg_mresbiv.as	\
	multpoly/multpolypkg/alg_ZpUVres.as	\
	multpoly/multpolytest/alg_bivtst1.as	\
	multpoly/multpolytest/alg_bivtst2.as	\
	multpoly/multpolytest/alg_bivtst3.as	\
	multpoly/multpolytest/sm_dmp0pkgt.as	\
	multpoly/multpolytest/sm_exppkgt.as	\
	multpoly/multpolytest/sm_pr0pkgt.as	\
	numbers/sit_primes.as	\
	numbers/sit_primgen.as	\
	numbers/sit_prmroot.as	\
	numbers/sit_prmtabl.as	\
	polyfactor0/sit_fhensel.as	\
	polyfactor0/sit_zfactor.as	\
	polyfactor0/sit_zfring.as	\
	polyfactorp/sit_upfactp.as	\
	series/alg_serpoly.as	\
	series/compbug/sit_duts.as	\
	series/sit_seqence.as	\
	series/sit_sercat.as	\
	univpoly/alg_sup0.as	\
	univpoly/alg_sup1.as	\
	univpoly/alg_sup.as	\
	univpoly/alg_unitool.as	\
	univpoly/alg_uprcr.as	\
	univpoly/categories/alg_chrem2.as	\
	univpoly/categories/alg_modgcdp.as	\
	univpoly/categories/alg_polydio.as	\
	univpoly/categories/sit_fftring.as	\
	univpoly/categories/sit_fring.as	\
	univpoly/categories/sit_froot.as	\
	univpoly/categories/sit_resprs.as	\
	univpoly/categories/sit_sqfree.as	\
	univpoly/categories/sit_ufalg.as	\
	univpoly/categories/sit_uffalg.as	\
	univpoly/categories/sit_ugring.as	\
	univpoly/categories/sit_umonom.as	\
	univpoly/categories/sit_upolalg.as	\
	univpoly/categories/sit_upolc0.as	\
	univpoly/categories/sit_zring.as	\
	univpoly/gcd/sit_gcdint.as	\
	univpoly/gcd/sit_heugcd.as	\
	univpoly/gcd/sit_modgcd.as	\
	univpoly/gcd/sit_modpgcd.as	\
	univpoly/sit_dup.as	\
	univpoly/sit_polkara.as	\
	univpoly/sit_spread.as	\
	univpoly/sit_ufacpol.as	\
	univpoly/sit_upolc.as	\
	util/alg_version.as	\
	categories/sit_intgmp.as	\
	mat/modular/sit_zcrtla.as	\
	multpoly/multpolycat/sm_polring.as	\
	multpoly/multpolycat/sm_rmpcat0.as	\
	multpoly/multpolydom/alg_smp.as	\
	multpoly/multpolydom/sm_rmp.as	\
	multpoly/multpolydom/sm_rmpz.as	\
	multpoly/multpolydom/sm_rmpzx.as	\
	polyfactor0/sit_zfringg.as	\
	univpoly/gcd/sit_gcdintg.as

libalgebra_ASOURCES := $(addprefix $(THIS), $(libalgebra_ASOURCES))

libalgebra_AOBJECTS := $(libalgebra_ASOURCES:.as=.ao)
libalgebra_COBJECTS := $(libalgebra_ASOURCES:.as=.o)


# C library
build/libalgebra.a: $(libalgebra_COBJECTS)
	$(AR) cr $@ $^

# Local aldor build rule
$(THIS)%.o: $(THIS)%.as build/aldor build/unicl $(aldor_HEADERS) build/include/algebra.as build/include/algebrauid.as
	build/aldor $(AFLAGS) -q1 $<
	$(AR) cr build/libalgebra.al $(@:.o=.ao)
	mv $(notdir $@) $@

# Clean
clean: clean-$(THIS)
clean-$(THIS):
	$(RM) $(libalgebra_AOBJECTS)
	$(RM) $(libalgebra_COBJECTS)
	$(RM) $(libalgebra_ASOURCES:.as=.c)
	$(RM) build/libalgebra.a build/libalgebra.al

# Depend
$(THIS)algext/sit_algext.o:	\
	$(THIS)series/compbug/sit_duts.o	\
	$(THIS)util/alg_version.o
$(THIS)algext/sit_sae.o:	\
	$(THIS)algext/sit_saexcpt.o	\
	$(THIS)algext/sit_upmod.o	\
	$(THIS)util/alg_version.o
$(THIS)algext/sit_saexcpt.o:	\
	$(THIS)categories/sit_comring.o	\
	$(THIS)util/alg_version.o
$(THIS)algext/sit_upmod.o:	\
	$(THIS)algext/sit_algext.o	\
	$(THIS)util/alg_version.o
$(THIS)basic/compbug/sit_interp.o:	\
	$(THIS)basic/sit_pring.o	\
	$(THIS)categories/sit_gexpcat.o	\
	$(THIS)util/alg_version.o
$(THIS)basic/compbug/sit_shell.o:	\
	$(THIS)basic/compbug/sit_interp.o	\
	$(THIS)util/alg_version.o
$(THIS)basic/sit_complex.o:	\
	$(THIS)categories/alg_ffield.o	\
	$(THIS)categories/sit_algebra.o	\
	$(THIS)categories/sit_qring.o	\
	$(THIS)extree/operators/sit_OPcplex.o	\
	$(THIS)util/alg_version.o
$(THIS)basic/sit_indvar.o:	\
	$(THIS)extree/operators/sit_OPsubsc.o	\
	$(THIS)util/alg_version.o
$(THIS)basic/sit_mkpring.o:	\
	$(THIS)basic/sit_pring.o	\
	$(THIS)categories/sit_field.o	\
	$(THIS)util/alg_version.o
$(THIS)basic/sit_permut.o:	\
	$(THIS)categories/sit_gexpcat.o	\
	$(THIS)categories/sit_group.o	\
	$(THIS)util/alg_version.o
$(THIS)basic/sit_pring.o:	\
	$(THIS)categories/sit_basic.o	\
	$(THIS)util/alg_version.o
$(THIS)basic/sit_product.o:	\
	$(THIS)categories/sit_gexpcat.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/alg_cansimp.o:	\
	$(THIS)categories/sit_euclid.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/alg_ffield.o:	\
	$(THIS)categories/sit_charp.o	\
	$(THIS)categories/sit_field.o	\
	$(THIS)categories/sit_fset.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/alg_frering.o:	\
	$(THIS)categories/alg_rring.o	\
	$(THIS)categories/sit_charp.o	\
	$(THIS)categories/sit_freelar.o	\
	$(THIS)categories/sit_freemod.o	\
	$(THIS)categories/sit_qring.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/alg_idxfrng.o:	\
	$(THIS)categories/alg_frering.o	\
	$(THIS)categories/sit_idxflar.o	\
	$(THIS)categories/sit_idxfmod.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/alg_modcmp.o:	\
	$(THIS)categories/alg_cansimp.o	\
	$(THIS)categories/alg_rescls.o	\
	$(THIS)categories/sit_chrem.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/alg_primsrc.o:	\
	$(THIS)categories/sit_euclid.o	\
	$(THIS)categories/sit_gexpcat.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/alg_rescls.o:	\
	$(THIS)categories/alg_primsrc.o	\
	$(THIS)categories/sit_field.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/alg_rring.o:	\
	$(THIS)categories/sit_linarit.o	\
	$(THIS)categories/sit_module.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_abgroup.o:	\
	$(THIS)categories/sit_abmon.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_abmon.o:	\
	$(THIS)categories/sit_basic.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_algebra.o:	\
	$(THIS)categories/alg_rring.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_automor.o:	\
	$(THIS)categories/sit_gexpcat.o	\
	$(THIS)categories/sit_group.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_basic.o:	\
	$(THIS)categories/sit_pable.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_char0.o:	\
	$(THIS)categories/sit_ring.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_charp.o:	\
	$(THIS)categories/sit_gexpcat.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_chrem.o:	\
	$(THIS)categories/sit_euclid.o	\
	$(THIS)categories/sit_gexpcat.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_comring.o:	\
	$(THIS)categories/sit_ring.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_dcmprng.o:	\
	$(THIS)categories/sit_comring.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_deriv.o:	\
	$(THIS)categories/sit_gexpcat.o	\
	$(THIS)categories/sit_module.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_diffext.o:	\
	$(THIS)categories/sit_difring.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_difring.o:	\
	$(THIS)categories/sit_deriv.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_euclid.o:	\
	$(THIS)categories/sit_gcd.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_field.o:	\
	$(THIS)categories/sit_euclid.o	\
	$(THIS)categories/sit_group.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_freealg.o:	\
	$(THIS)categories/alg_frering.o	\
	$(THIS)categories/sit_algebra.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_freelar.o:	\
	$(THIS)categories/sit_freelc.o	\
	$(THIS)categories/sit_linarit.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_freelc.o:	\
	$(THIS)categories/sit_ring.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_freemod.o:	\
	$(THIS)categories/sit_field.o	\
	$(THIS)categories/sit_freelc.o	\
	$(THIS)categories/sit_module.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_fset.o:	\
	$(THIS)extree/operators/sit_OPsubsc.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_gcd.o:	\
	$(THIS)categories/sit_intdom.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_gexpcat.o:	\
	$(THIS)categories/sit_ptools.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_group.o:	\
	$(THIS)categories/sit_monoid.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_idxfalg.o:	\
	$(THIS)categories/alg_idxfrng.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_idxflar.o:	\
	$(THIS)categories/sit_freelar.o	\
	$(THIS)categories/sit_idxflc.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_idxflc.o:	\
	$(THIS)categories/sit_freelc.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_idxfmod.o:	\
	$(THIS)categories/sit_freemod.o	\
	$(THIS)categories/sit_idxflc.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_intdom.o:	\
	$(THIS)categories/sit_comring.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_integer.o:	\
	$(THIS)categories/alg_modcmp.o	\
	$(THIS)categories/sit_char0.o	\
	$(THIS)categories/sit_spzble.o	\
	$(THIS)numbers/sit_primgen.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_intgmp.o:	\
	$(THIS)categories/sit_integer.o	\
	$(THIS)univpoly/gcd/sit_gcdint.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_linarit.o:	\
	$(THIS)categories/sit_ring.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_module.o:	\
	$(THIS)categories/sit_ring.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_monoid.o:	\
	$(THIS)categories/sit_basic.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_ncid.o:	\
	$(THIS)categories/sit_ring.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_pable.o:	\
	$(THIS)extree/parser/sit_infexpr.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_prfcat0.o:	\
	$(THIS)categories/alg_ffield.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_ptools.o:	\
	$(THIS)categories/sit_intdom.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_qring.o:	\
	$(THIS)categories/sit_char0.o	\
	$(THIS)categories/sit_field.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_ring.o:	\
	$(THIS)categories/sit_abgroup.o	\
	$(THIS)categories/sit_monoid.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_spf0.o:	\
	$(THIS)categories/sit_spfcat0.o	\
	$(THIS)numbers/sit_primgen.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_spfcat0.o:	\
	$(THIS)categories/sit_prfcat0.o	\
	$(THIS)util/alg_version.o
$(THIS)categories/sit_spzble.o:	\
	$(THIS)categories/sit_comring.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/operators/sit_OPand.o:	\
	$(THIS)categories/sit_gexpcat.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/operators/sit_OPassgn.o:	\
	$(THIS)extree/sit_optools.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/operators/sit_OPbigO.o:	\
	$(THIS)categories/sit_gexpcat.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/operators/sit_OPcase.o:	\
	$(THIS)extree/operators/sit_OPif.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/operators/sit_OPcplex.o:	\
	$(THIS)categories/sit_gexpcat.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/operators/sit_OPequal.o:	\
	$(THIS)categories/sit_gexpcat.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/operators/sit_OPexpt.o:	\
	$(THIS)extree/sit_optools.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/operators/sit_OPfact.o:	\
	$(THIS)categories/sit_gexpcat.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/operators/sit_OPif.o:	\
	$(THIS)categories/sit_gexpcat.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/operators/sit_OPless.o:	\
	$(THIS)extree/sit_extree.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/operators/sit_OPlist.o:	\
	$(THIS)extree/sit_optools.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/operators/sit_OPllist.o:	\
	$(THIS)categories/sit_gexpcat.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/operators/sit_OPmatrx.o:	\
	$(THIS)categories/sit_gexpcat.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/operators/sit_OPminus.o:	\
	$(THIS)extree/sit_optools.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/operators/sit_OPmore.o:	\
	$(THIS)extree/sit_extree.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/operators/sit_OPnoteq.o:	\
	$(THIS)categories/sit_gexpcat.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/operators/sit_OPplus.o:	\
	$(THIS)extree/operators/sit_OPminus.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/operators/sit_OPprefx.o:	\
	$(THIS)extree/sit_optools.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/operators/sit_OPquot.o:	\
	$(THIS)extree/sit_optools.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/operators/sit_OPsubsc.o:	\
	$(THIS)categories/sit_gexpcat.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/operators/sit_OPtimes.o:	\
	$(THIS)extree/sit_optools.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/operators/sit_OPvect.o:	\
	$(THIS)categories/sit_gexpcat.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/parser/sit_infexpr.o:	\
	$(THIS)extree/operators/sit_OPlist.o	\
	$(THIS)extree/parser/sit_parser.o	\
	$(THIS)extree/parser/sit_scanner.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/parser/sit_lspexpr.o:	\
	$(THIS)extree/operators/sit_OPllist.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/parser/sit_maple.o:	\
	$(THIS)categories/sit_gexpcat.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/parser/sit_parser.o:	\
	$(THIS)extree/sit_extree.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/parser/sit_scanner.o:	\
	$(THIS)extree/operators/sit_OPassgn.o	\
	$(THIS)extree/operators/sit_OPless.o	\
	$(THIS)extree/operators/sit_OPmore.o	\
	$(THIS)extree/parser/sit_token.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/parser/sit_token.o:	\
	$(THIS)extree/operators/sit_OPexpt.o	\
	$(THIS)extree/operators/sit_OPplus.o	\
	$(THIS)extree/operators/sit_OPprefx.o	\
	$(THIS)extree/operators/sit_OPquot.o	\
	$(THIS)extree/operators/sit_OPtimes.o	\
	$(THIS)util/alg_version.o
$(THIS)extree/sit_extree.o:	\
	$(THIS)util/alg_version.o
$(THIS)extree/sit_optools.o:	\
	$(THIS)extree/sit_extree.o	\
	$(THIS)util/alg_version.o
$(THIS)ffield/alg_pf2.o:	\
	$(THIS)ffield/sit_sprfcat.o	\
	$(THIS)util/alg_version.o
$(THIS)ffield/sit_prfcat.o:	\
	$(THIS)polyfactorp/sit_upfactp.o	\
	$(THIS)util/alg_version.o
$(THIS)ffield/sit_spf.o:	\
	$(THIS)ffield/sit_sprfcat.o	\
	$(THIS)util/alg_version.o
$(THIS)ffield/sit_sprfcat.o:	\
	$(THIS)ffield/sit_prfcat.o	\
	$(THIS)ffield/sit_sprfgcd.o	\
	$(THIS)ffield/sit_sprfmat.o	\
	$(THIS)util/alg_version.o
$(THIS)ffield/sit_sprfgcd.o:	\
	$(THIS)univpoly/categories/sit_upolc0.o	\
	$(THIS)univpoly/gcd/sit_modpgcd.o	\
	$(THIS)util/alg_version.o
$(THIS)ffield/sit_sprfmat.o:	\
	$(THIS)categories/sit_spfcat0.o	\
	$(THIS)mat/modular/sit_modpoge.o	\
	$(THIS)mat/sit_matcat.o	\
	$(THIS)util/alg_version.o
$(THIS)ffield/sit_zpf.o:	\
	$(THIS)ffield/sit_sprfcat.o	\
	$(THIS)numbers/sit_prmroot.o	\
	$(THIS)util/alg_version.o
$(THIS)fraction/sit_lcqotct.o:	\
	$(THIS)fraction/sit_qotbyc0.o	\
	$(THIS)fraction/sit_qotfct0.o	\
	$(THIS)util/alg_version.o
$(THIS)fraction/sit_matquot.o:	\
	$(THIS)fraction/sit_vecquot.o	\
	$(THIS)util/alg_version.o
$(THIS)fraction/sit_qotbyc0.o:	\
	$(THIS)fraction/sit_quotcat.o	\
	$(THIS)util/alg_version.o
$(THIS)fraction/sit_qotbyct.o:	\
	$(THIS)fraction/sit_matquot.o	\
	$(THIS)fraction/sit_uflgqot.o	\
	$(THIS)univpoly/sit_dup.o	\
	$(THIS)util/alg_version.o
$(THIS)fraction/sit_qotfcat.o:	\
	$(THIS)fraction/sit_matquot.o	\
	$(THIS)fraction/sit_uflgqot.o	\
	$(THIS)univpoly/sit_dup.o	\
	$(THIS)util/alg_version.o
$(THIS)fraction/sit_qotfct0.o:	\
	$(THIS)fraction/sit_quotcat.o	\
	$(THIS)util/alg_version.o
$(THIS)fraction/sit_qotient.o:	\
	$(THIS)fraction/sit_qotfcat.o	\
	$(THIS)util/alg_version.o
$(THIS)fraction/sit_quotby.o:	\
	$(THIS)fraction/sit_qotbyct.o	\
	$(THIS)util/alg_version.o
$(THIS)fraction/sit_quotcat.o:	\
	$(THIS)categories/sit_algebra.o	\
	$(THIS)categories/sit_diffext.o	\
	$(THIS)univpoly/categories/sit_fring.o	\
	$(THIS)univpoly/categories/sit_ugring.o	\
	$(THIS)util/alg_version.o
$(THIS)fraction/sit_uflgqot.o:	\
	$(THIS)fraction/sit_lcqotct.o	\
	$(THIS)util/alg_version.o
$(THIS)fraction/sit_vecquot.o:	\
	$(THIS)fraction/sit_lcqotct.o	\
	$(THIS)util/alg_version.o
$(THIS)mat/gauss/sit_dfge.o:	\
	$(THIS)basic/sit_permut.o	\
	$(THIS)mat/gauss/sit_linelim.o	\
	$(THIS)util/alg_version.o
$(THIS)mat/gauss/sit_ff2ge.o:	\
	$(THIS)basic/sit_permut.o	\
	$(THIS)mat/gauss/sit_linelim.o	\
	$(THIS)util/alg_version.o
$(THIS)mat/gauss/sit_ffge.o:	\
	$(THIS)basic/sit_permut.o	\
	$(THIS)mat/gauss/sit_linelim.o	\
	$(THIS)util/alg_version.o
$(THIS)mat/gauss/sit_hermge.o:	\
	$(THIS)basic/sit_permut.o	\
	$(THIS)categories/sit_euclid.o	\
	$(THIS)mat/gauss/sit_linelim.o	\
	$(THIS)util/alg_version.o
$(THIS)mat/gauss/sit_linelim.o:	\
	$(THIS)mat/sit_matcat.o	\
	$(THIS)util/alg_version.o
$(THIS)mat/gauss/sit_oge.o:	\
	$(THIS)basic/sit_permut.o	\
	$(THIS)categories/sit_field.o	\
	$(THIS)mat/gauss/sit_linelim.o	\
	$(THIS)util/alg_version.o
$(THIS)mat/linalg/sit_bsolve.o:	\
	$(THIS)basic/sit_permut.o	\
	$(THIS)categories/sit_field.o	\
	$(THIS)mat/sit_matcat.o	\
	$(THIS)util/alg_version.o
$(THIS)mat/linalg/sit_laring.o:	\
	$(THIS)mat/sit_matcat.o	\
	$(THIS)util/alg_version.o
$(THIS)mat/linalg/sit_linalg.o:	\
	$(THIS)mat/gauss/sit_dfge.o	\
	$(THIS)mat/gauss/sit_ff2ge.o	\
	$(THIS)mat/gauss/sit_oge.o	\
	$(THIS)mat/linalg/sit_bsolve.o	\
	$(THIS)mat/linalg/sit_laring.o	\
	$(THIS)mat/linalg/sit_overdet.o	\
	$(THIS)mat/modular/compbug/sit_speclin.o	\
	$(THIS)util/alg_version.o
$(THIS)mat/linalg/sit_overdet.o:	\
	$(THIS)categories/sit_gcd.o	\
	$(THIS)mat/sit_matcat.o	\
	$(THIS)util/alg_version.o
$(THIS)mat/linalg2/alg_ffupla.o:	\
	$(THIS)mat/linalg2/sit_popov.o	\
	$(THIS)util/alg_version.o
$(THIS)mat/linalg2/sit_popov.o:	\
	$(THIS)univpoly/categories/sit_upolc0.o	\
	$(THIS)util/alg_version.o
$(THIS)mat/linalg2/sit_upcrtla.o:	\
	$(THIS)univpoly/categories/sit_upolc0.o	\
	$(THIS)util/alg_version.o
$(THIS)mat/linalg3/sit_hensela.o:	\
	$(THIS)fraction/sit_qotient.o	\
	$(THIS)mat/linalg2/sit_upcrtla.o	\
	$(THIS)util/alg_version.o
$(THIS)mat/modular/compbug/sit_speclin.o:	\
	$(THIS)categories/sit_spf0.o	\
	$(THIS)categories/sit_spzble.o	\
	$(THIS)mat/modular/sit_modpoge.o	\
	$(THIS)mat/sit_matcat.o	\
	$(THIS)util/alg_version.o
$(THIS)mat/modular/sit_modpoge.o:	\
	$(THIS)numbers/sit_primgen.o	\
	$(THIS)util/alg_version.o
$(THIS)mat/modular/sit_zcrtla.o:	\
	$(THIS)categories/sit_integer.o	\
	$(THIS)univpoly/gcd/sit_gcdintg.o	\
	$(THIS)util/alg_version.o
$(THIS)mat/sit_dnsemat.o:	\
	$(THIS)mat/sit_matcat.o	\
	$(THIS)util/alg_version.o
$(THIS)mat/sit_matcat.o:	\
	$(THIS)categories/sit_difring.o	\
	$(THIS)categories/sit_linarit.o	\
	$(THIS)extree/operators/sit_OPmatrx.o	\
	$(THIS)mat/sit_vector.o	\
	$(THIS)util/alg_version.o
$(THIS)mat/sit_vector.o:	\
	$(THIS)extree/operators/sit_OPvect.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/exponent/sm_dirprod.o:	\
	$(THIS)multpoly/exponent/sm_dirprodc.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/exponent/sm_dirprodc.o:	\
	$(THIS)categories/sit_gexpcat.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/exponent/sm_expocat.o:	\
	$(THIS)categories/sit_gexpcat.o	\
	$(THIS)multpoly/exponent/sm_vt.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/exponent/sm_fvt.o:	\
	$(THIS)categories/sit_gexpcat.o	\
	$(THIS)multpoly/exponent/sm_vt.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/exponent/sm_listovar.o:	\
	$(THIS)multpoly/exponent/sm_fvt.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/exponent/sm_midl.o:	\
	$(THIS)multpoly/exponent/sm_midrl.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/exponent/sm_midrl.o:	\
	$(THIS)multpoly/exponent/sm_dirprod.o	\
	$(THIS)multpoly/exponent/sm_mievc.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/exponent/sm_mievc.o:	\
	$(THIS)multpoly/exponent/sm_expocat.o	\
	$(THIS)multpoly/exponent/sm_fvt.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/exponent/sm_milex.o:	\
	$(THIS)multpoly/exponent/sm_midrl.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/exponent/sm_osymbol.o:	\
	$(THIS)multpoly/exponent/sm_vt.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/exponent/sm_tuplovar.o:	\
	$(THIS)multpoly/exponent/sm_listovar.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/exponent/sm_vt.o:	\
	$(THIS)categories/sit_basic.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/exponent/sm_zevc.o:	\
	$(THIS)multpoly/exponent/sm_dirprodc.o	\
	$(THIS)multpoly/exponent/sm_expocat.o	\
	$(THIS)multpoly/exponent/sm_fvt.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/multpolycat/alg_defgcd.o:	\
	$(THIS)multpoly/multpolycat/sm_polring0.o	\
	$(THIS)univpoly/sit_dup.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/multpolycat/alg_poltype.o:	\
	$(THIS)multpoly/multpolycat/alg_stdfrng.o	\
	$(THIS)univpoly/alg_sup.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/multpolycat/alg_stdfrng.o:	\
	$(THIS)categories/alg_frering.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/multpolycat/sm_famr0.o:	\
	$(THIS)multpoly/exponent/sm_expocat.o	\
	$(THIS)multpoly/multpolycat/sm_polring0.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/multpolycat/sm_polring.o:	\
	$(THIS)multpoly/multpolycat/alg_defgcd.o	\
	$(THIS)multpoly/multpolycat/sm_famr0.o	\
	$(THIS)univpoly/gcd/sit_gcdintg.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/multpolycat/sm_polring0.o:	\
	$(THIS)categories/sit_freealg.o	\
	$(THIS)multpoly/multpolycat/alg_poltype.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/multpolycat/sm_rmpcat0.o:	\
	$(THIS)multpoly/multpolycat/sm_polring.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/multpolydata/sm_delist.o:	\
	$(THIS)categories/sit_basic.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/multpolydom/alg_smp.o:	\
	$(THIS)multpoly/multpolycat/sm_rmpcat0.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/multpolydom/sm_dmp0.o:	\
	$(THIS)categories/sit_gexpcat.o	\
	$(THIS)categories/sit_idxfmod.o	\
	$(THIS)multpoly/multpolydata/sm_delist.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/multpolydom/sm_dmp1.o:	\
	$(THIS)multpoly/multpolycat/sm_famr0.o	\
	$(THIS)multpoly/multpolydom/sm_dmp0.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/multpolydom/sm_rmp.o:	\
	$(THIS)multpoly/multpolycat/sm_rmpcat0.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/multpolydom/sm_rmpz.o:	\
	$(THIS)multpoly/exponent/sm_osymbol.o	\
	$(THIS)multpoly/multpolydom/sm_rmp.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/multpolydom/sm_rmpzx.o:	\
	$(THIS)multpoly/multpolydom/sm_rmp.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/multpolypkg/alg_ZpUVres.o:	\
	$(THIS)ffield/sit_spf.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/multpolypkg/alg_bivarpk.o:	\
	$(THIS)ffield/sit_spf.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/multpolypkg/alg_mresbiv.o:	\
	$(THIS)multpoly/multpolypkg/alg_ZpUVres.o	\
	$(THIS)multpoly/multpolypkg/alg_bivarpk.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/multpolytest/alg_bivtst1.o:	\
	$(THIS)multpoly/multpolypkg/alg_bivarpk.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/multpolytest/alg_bivtst2.o:	\
	$(THIS)multpoly/multpolypkg/alg_mresbiv.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/multpolytest/alg_bivtst3.o:	\
	$(THIS)ffield/sit_spf.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/multpolytest/sm_dmp0pkgt.o:	\
	$(THIS)categories/sit_gexpcat.o	\
	$(THIS)categories/sit_idxfmod.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/multpolytest/sm_exppkgt.o:	\
	$(THIS)multpoly/exponent/sm_expocat.o	\
	$(THIS)util/alg_version.o
$(THIS)multpoly/multpolytest/sm_pr0pkgt.o:	\
	$(THIS)multpoly/multpolycat/sm_polring0.o	\
	$(THIS)util/alg_version.o
$(THIS)numbers/sit_primes.o:	\
	$(THIS)numbers/sit_prmtabl.o	\
	$(THIS)util/alg_version.o
$(THIS)numbers/sit_primgen.o:	\
	$(THIS)numbers/sit_primes.o	\
	$(THIS)util/alg_version.o
$(THIS)numbers/sit_prmroot.o:	\
	$(THIS)numbers/sit_primgen.o	\
	$(THIS)util/alg_version.o
$(THIS)numbers/sit_prmtabl.o:	\
	$(THIS)categories/sit_gexpcat.o	\
	$(THIS)util/alg_version.o
$(THIS)polyfactor0/sit_fhensel.o:	\
	$(THIS)categories/sit_integer.o	\
	$(THIS)univpoly/categories/sit_upolc0.o	\
	$(THIS)util/alg_version.o
$(THIS)polyfactor0/sit_zfactor.o:	\
	$(THIS)ffield/sit_zpf.o	\
	$(THIS)polyfactor0/sit_fhensel.o	\
	$(THIS)util/alg_version.o
$(THIS)polyfactor0/sit_zfring.o:	\
	$(THIS)univpoly/categories/sit_fring.o	\
	$(THIS)util/alg_version.o
$(THIS)polyfactor0/sit_zfringg.o:	\
	$(THIS)univpoly/categories/sit_fring.o	\
	$(THIS)util/alg_version.o
$(THIS)polyfactorp/sit_upfactp.o:	\
	$(THIS)algext/sit_upmod.o	\
	$(THIS)util/alg_version.o
$(THIS)series/alg_serpoly.o:	\
	$(THIS)series/sit_sercat.o	\
	$(THIS)util/alg_version.o
$(THIS)series/compbug/sit_duts.o:	\
	$(THIS)extree/operators/sit_OPbigO.o	\
	$(THIS)series/sit_sercat.o	\
	$(THIS)util/alg_version.o
$(THIS)series/sit_seqence.o:	\
	$(THIS)mat/sit_vector.o	\
	$(THIS)univpoly/categories/sit_ufalg.o	\
	$(THIS)util/alg_version.o
$(THIS)series/sit_sercat.o:	\
	$(THIS)fraction/sit_qotient.o	\
	$(THIS)series/sit_seqence.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/alg_sup.o:	\
	$(THIS)univpoly/alg_sup1.o	\
	$(THIS)univpoly/sit_spread.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/alg_sup0.o:	\
	$(THIS)categories/sit_idxfmod.o	\
	$(THIS)categories/sit_integer.o	\
	$(THIS)categories/sit_prfcat0.o	\
	$(THIS)multpoly/multpolydata/sm_delist.o	\
	$(THIS)univpoly/categories/sit_umonom.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/alg_sup1.o:	\
	$(THIS)univpoly/alg_sup0.o	\
	$(THIS)univpoly/categories/sit_upolalg.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/alg_unitool.o:	\
	$(THIS)univpoly/categories/sit_upolc0.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/alg_uprcr.o:	\
	$(THIS)univpoly/categories/sit_upolc0.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/categories/alg_chrem2.o:	\
	$(THIS)univpoly/categories/sit_upolc0.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/categories/alg_modgcdp.o:	\
	$(THIS)univpoly/categories/alg_chrem2.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/categories/alg_polydio.o:	\
	$(THIS)univpoly/categories/sit_resprs.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/categories/sit_fftring.o:	\
	$(THIS)univpoly/categories/sit_upolc0.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/categories/sit_fring.o:	\
	$(THIS)univpoly/categories/sit_zring.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/categories/sit_froot.o:	\
	$(THIS)categories/sit_field.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/categories/sit_resprs.o:	\
	$(THIS)univpoly/categories/sit_upolc0.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/categories/sit_sqfree.o:	\
	$(THIS)univpoly/categories/sit_upolc0.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/categories/sit_ufalg.o:	\
	$(THIS)categories/sit_idxflar.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/categories/sit_uffalg.o:	\
	$(THIS)categories/alg_idxfrng.o	\
	$(THIS)mat/sit_dnsemat.o	\
	$(THIS)univpoly/categories/sit_ufalg.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/categories/sit_ugring.o:	\
	$(THIS)univpoly/categories/sit_upolc0.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/categories/sit_umonom.o:	\
	$(THIS)categories/sit_charp.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/categories/sit_upolalg.o:	\
	$(THIS)univpoly/categories/sit_uffalg.o	\
	$(THIS)univpoly/categories/sit_umonom.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/categories/sit_upolc0.o:	\
	$(THIS)basic/sit_product.o	\
	$(THIS)categories/alg_modcmp.o	\
	$(THIS)categories/sit_dcmprng.o	\
	$(THIS)categories/sit_idxfalg.o	\
	$(THIS)mat/linalg/sit_linalg.o	\
	$(THIS)univpoly/categories/sit_upolalg.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/categories/sit_zring.o:	\
	$(THIS)univpoly/categories/sit_froot.o	\
	$(THIS)univpoly/categories/sit_upolc0.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/gcd/sit_gcdint.o:	\
	$(THIS)univpoly/categories/sit_resprs.o	\
	$(THIS)univpoly/categories/sit_ugring.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/gcd/sit_gcdintg.o:	\
	$(THIS)univpoly/categories/sit_ugring.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/gcd/sit_heugcd.o:	\
	$(THIS)categories/sit_integer.o	\
	$(THIS)univpoly/categories/sit_upolc0.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/gcd/sit_modgcd.o:	\
	$(THIS)categories/sit_integer.o	\
	$(THIS)univpoly/categories/sit_upolc0.o	\
	$(THIS)univpoly/gcd/sit_modpgcd.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/gcd/sit_modpgcd.o:	\
	$(THIS)numbers/sit_primgen.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/sit_dup.o:	\
	$(THIS)univpoly/alg_sup.o	\
	$(THIS)univpoly/categories/sit_fftring.o	\
	$(THIS)univpoly/sit_polkara.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/sit_polkara.o:	\
	$(THIS)categories/sit_gexpcat.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/sit_spread.o:	\
	$(THIS)univpoly/sit_upolc.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/sit_ufacpol.o:	\
	$(THIS)extree/operators/sit_OPfact.o	\
	$(THIS)univpoly/sit_upolc.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/sit_upolc.o:	\
	$(THIS)mat/linalg2/alg_ffupla.o	\
	$(THIS)univpoly/alg_uprcr.o	\
	$(THIS)univpoly/categories/alg_modgcdp.o	\
	$(THIS)univpoly/categories/sit_fring.o	\
	$(THIS)univpoly/categories/sit_resprs.o	\
	$(THIS)univpoly/categories/sit_sqfree.o	\
	$(THIS)univpoly/categories/sit_ugring.o	\
	$(THIS)util/alg_version.o
$(THIS)univpoly/categories/sit_ufalg.o:	\
	$(THIS)categories/sit_integer.o
$(THIS)univpoly/categories/sit_froot.o:	\
	$(THIS)categories/sit_integer.o
$(THIS)multpoly/exponent/sm_expocat.o:	\
	$(THIS)categories/sit_integer.o
$(THIS)basic/sit_product.o:	\
	$(THIS)categories/sit_integer.o
$(THIS)categories/sit_fset.o:	\
	$(THIS)categories/sit_integer.o
$(THIS)univpoly/categories/sit_umonom.o:	\
	$(THIS)categories/sit_integer.o
$(THIS)univpoly/gcd/sit_gcdintg.o:	\
	$(THIS)categories/sit_intgmp.o
$(THIS)univpoly/gcd/sit_gcdint.o:	\
	$(THIS)univpoly/gcd/sit_heugcd.o	\
	$(THIS)univpoly/gcd/sit_modgcd.o
$(THIS)polyfactor0/sit_zfring.o:	\
	$(THIS)polyfactor0/sit_zfactor.o
$(THIS)polyfactor0/sit_zfringg.o:	\
	$(THIS)categories/sit_intgmp.o	\
	$(THIS)polyfactor0/sit_zfactor.o
$(THIS)util/alg_version.o:	\
	build/libaldor.a
