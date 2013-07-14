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
	categories/sit_intgmp.as	\
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
	mat/modular/sit_zcrtla.as	\
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
	series/compbug/sit_duts.as	\
	series/alg_serpoly.as	\
	series/sit_seqence.as	\
	series/sit_sercat.as	\
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
	univpoly/alg_sup0.as	\
	univpoly/alg_sup1.as	\
	univpoly/alg_sup.as	\
	univpoly/alg_unitool.as	\
	univpoly/alg_uprcr.as	\
	univpoly/sit_dup.as	\
	univpoly/sit_polkara.as	\
	univpoly/sit_spread.as	\
	univpoly/sit_ufacpol.as	\
	univpoly/sit_upolc.as	\
	util/alg_version.as	\
	multpoly/multpolycat/sm_polring.as	\
	multpoly/multpolycat/sm_rmpcat0.as	\
	multpoly/multpolydom/alg_smp.as	\
	multpoly/multpolydom/sm_rmp.as	\
	multpoly/multpolydom/sm_rmpz.as	\
	multpoly/multpolydom/sm_rmpzx.as	\
	polyfactor0/sit_zfringg.as	\
	univpoly/gcd/sit_gcdintg.as

libalgebra_AFLAGS := -Q1

$(eval $(call aldor-target,algebra))

# Depend
$(THIS)algext/sit_algext.ao:	\
	$(THIS)series/compbug/sit_duts.ao	\
	$(THIS)util/alg_version.ao
$(THIS)algext/sit_sae.ao:	\
	$(THIS)algext/sit_saexcpt.ao	\
	$(THIS)algext/sit_upmod.ao	\
	$(THIS)util/alg_version.ao
$(THIS)algext/sit_saexcpt.ao:	\
	$(THIS)categories/sit_comring.ao	\
	$(THIS)util/alg_version.ao
$(THIS)algext/sit_upmod.ao:	\
	$(THIS)algext/sit_algext.ao	\
	$(THIS)util/alg_version.ao
$(THIS)basic/compbug/sit_interp.ao:	\
	$(THIS)basic/sit_pring.ao	\
	$(THIS)categories/sit_gexpcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)basic/compbug/sit_shell.ao:	\
	$(THIS)basic/compbug/sit_interp.ao	\
	$(THIS)util/alg_version.ao
$(THIS)basic/sit_complex.ao:	\
	$(THIS)categories/alg_ffield.ao	\
	$(THIS)categories/sit_algebra.ao	\
	$(THIS)categories/sit_qring.ao	\
	$(THIS)extree/operators/sit_OPcplex.ao	\
	$(THIS)util/alg_version.ao
$(THIS)basic/sit_indvar.ao:	\
	$(THIS)extree/operators/sit_OPsubsc.ao	\
	$(THIS)util/alg_version.ao
$(THIS)basic/sit_mkpring.ao:	\
	$(THIS)basic/sit_pring.ao	\
	$(THIS)categories/sit_field.ao	\
	$(THIS)util/alg_version.ao
$(THIS)basic/sit_permut.ao:	\
	$(THIS)categories/sit_gexpcat.ao	\
	$(THIS)categories/sit_group.ao	\
	$(THIS)util/alg_version.ao
$(THIS)basic/sit_pring.ao:	\
	$(THIS)categories/sit_basic.ao	\
	$(THIS)util/alg_version.ao
$(THIS)basic/sit_product.ao:	\
	$(THIS)categories/sit_gexpcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/alg_cansimp.ao:	\
	$(THIS)categories/sit_euclid.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/alg_ffield.ao:	\
	$(THIS)categories/sit_charp.ao	\
	$(THIS)categories/sit_field.ao	\
	$(THIS)categories/sit_fset.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/alg_frering.ao:	\
	$(THIS)categories/alg_rring.ao	\
	$(THIS)categories/sit_charp.ao	\
	$(THIS)categories/sit_freelar.ao	\
	$(THIS)categories/sit_freemod.ao	\
	$(THIS)categories/sit_qring.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/alg_idxfrng.ao:	\
	$(THIS)categories/alg_frering.ao	\
	$(THIS)categories/sit_idxflar.ao	\
	$(THIS)categories/sit_idxfmod.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/alg_modcmp.ao:	\
	$(THIS)categories/alg_cansimp.ao	\
	$(THIS)categories/alg_rescls.ao	\
	$(THIS)categories/sit_chrem.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/alg_primsrc.ao:	\
	$(THIS)categories/sit_euclid.ao	\
	$(THIS)categories/sit_gexpcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/alg_rescls.ao:	\
	$(THIS)categories/alg_primsrc.ao	\
	$(THIS)categories/sit_field.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/alg_rring.ao:	\
	$(THIS)categories/sit_linarit.ao	\
	$(THIS)categories/sit_module.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_abgroup.ao:	\
	$(THIS)categories/sit_abmon.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_abmon.ao:	\
	$(THIS)categories/sit_basic.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_algebra.ao:	\
	$(THIS)categories/alg_rring.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_automor.ao:	\
	$(THIS)categories/sit_gexpcat.ao	\
	$(THIS)categories/sit_group.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_basic.ao:	\
	$(THIS)categories/sit_pable.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_char0.ao:	\
	$(THIS)categories/sit_ring.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_charp.ao:	\
	$(THIS)categories/sit_gexpcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_chrem.ao:	\
	$(THIS)categories/sit_euclid.ao	\
	$(THIS)categories/sit_gexpcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_comring.ao:	\
	$(THIS)categories/sit_ring.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_dcmprng.ao:	\
	$(THIS)categories/sit_comring.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_deriv.ao:	\
	$(THIS)categories/sit_gexpcat.ao	\
	$(THIS)categories/sit_module.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_diffext.ao:	\
	$(THIS)categories/sit_difring.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_difring.ao:	\
	$(THIS)categories/sit_deriv.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_euclid.ao:	\
	$(THIS)categories/sit_gcd.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_field.ao:	\
	$(THIS)categories/sit_euclid.ao	\
	$(THIS)categories/sit_group.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_freealg.ao:	\
	$(THIS)categories/alg_frering.ao	\
	$(THIS)categories/sit_algebra.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_freelar.ao:	\
	$(THIS)categories/sit_freelc.ao	\
	$(THIS)categories/sit_linarit.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_freelc.ao:	\
	$(THIS)categories/sit_ring.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_freemod.ao:	\
	$(THIS)categories/sit_field.ao	\
	$(THIS)categories/sit_freelc.ao	\
	$(THIS)categories/sit_module.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_fset.ao:	\
	$(THIS)extree/operators/sit_OPsubsc.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_gcd.ao:	\
	$(THIS)categories/sit_intdom.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_gexpcat.ao:	\
	$(THIS)categories/sit_ptools.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_group.ao:	\
	$(THIS)categories/sit_monoid.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_idxfalg.ao:	\
	$(THIS)categories/alg_idxfrng.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_idxflar.ao:	\
	$(THIS)categories/sit_freelar.ao	\
	$(THIS)categories/sit_idxflc.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_idxflc.ao:	\
	$(THIS)categories/sit_freelc.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_idxfmod.ao:	\
	$(THIS)categories/sit_freemod.ao	\
	$(THIS)categories/sit_idxflc.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_intdom.ao:	\
	$(THIS)categories/sit_comring.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_integer.ao:	\
	$(THIS)categories/alg_modcmp.ao	\
	$(THIS)categories/sit_char0.ao	\
	$(THIS)categories/sit_spzble.ao	\
	$(THIS)numbers/sit_primgen.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_intgmp.ao:	\
	$(THIS)categories/sit_integer.ao	\
	$(THIS)univpoly/gcd/sit_gcdint.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_linarit.ao:	\
	$(THIS)categories/sit_ring.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_module.ao:	\
	$(THIS)categories/sit_ring.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_monoid.ao:	\
	$(THIS)categories/sit_basic.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_ncid.ao:	\
	$(THIS)categories/sit_ring.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_pable.ao:	\
	$(THIS)extree/parser/sit_infexpr.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_prfcat0.ao:	\
	$(THIS)categories/alg_ffield.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_ptools.ao:	\
	$(THIS)categories/sit_intdom.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_qring.ao:	\
	$(THIS)categories/sit_char0.ao	\
	$(THIS)categories/sit_field.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_ring.ao:	\
	$(THIS)categories/sit_abgroup.ao	\
	$(THIS)categories/sit_monoid.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_spf0.ao:	\
	$(THIS)categories/sit_spfcat0.ao	\
	$(THIS)numbers/sit_primgen.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_spfcat0.ao:	\
	$(THIS)categories/sit_prfcat0.ao	\
	$(THIS)util/alg_version.ao
$(THIS)categories/sit_spzble.ao:	\
	$(THIS)categories/sit_comring.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/operators/sit_OPand.ao:	\
	$(THIS)categories/sit_gexpcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/operators/sit_OPassgn.ao:	\
	$(THIS)extree/sit_optools.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/operators/sit_OPbigO.ao:	\
	$(THIS)categories/sit_gexpcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/operators/sit_OPcase.ao:	\
	$(THIS)extree/operators/sit_OPif.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/operators/sit_OPcplex.ao:	\
	$(THIS)categories/sit_gexpcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/operators/sit_OPequal.ao:	\
	$(THIS)categories/sit_gexpcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/operators/sit_OPexpt.ao:	\
	$(THIS)extree/sit_optools.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/operators/sit_OPfact.ao:	\
	$(THIS)categories/sit_gexpcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/operators/sit_OPif.ao:	\
	$(THIS)categories/sit_gexpcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/operators/sit_OPless.ao:	\
	$(THIS)extree/sit_extree.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/operators/sit_OPlist.ao:	\
	$(THIS)extree/sit_optools.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/operators/sit_OPllist.ao:	\
	$(THIS)categories/sit_gexpcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/operators/sit_OPmatrx.ao:	\
	$(THIS)categories/sit_gexpcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/operators/sit_OPminus.ao:	\
	$(THIS)extree/sit_optools.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/operators/sit_OPmore.ao:	\
	$(THIS)extree/sit_extree.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/operators/sit_OPnoteq.ao:	\
	$(THIS)categories/sit_gexpcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/operators/sit_OPplus.ao:	\
	$(THIS)extree/operators/sit_OPminus.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/operators/sit_OPprefx.ao:	\
	$(THIS)extree/sit_optools.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/operators/sit_OPquot.ao:	\
	$(THIS)extree/sit_optools.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/operators/sit_OPsubsc.ao:	\
	$(THIS)categories/sit_gexpcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/operators/sit_OPtimes.ao:	\
	$(THIS)extree/sit_optools.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/operators/sit_OPvect.ao:	\
	$(THIS)categories/sit_gexpcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/parser/sit_infexpr.ao:	\
	$(THIS)extree/operators/sit_OPlist.ao	\
	$(THIS)extree/parser/sit_parser.ao	\
	$(THIS)extree/parser/sit_scanner.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/parser/sit_lspexpr.ao:	\
	$(THIS)extree/operators/sit_OPllist.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/parser/sit_maple.ao:	\
	$(THIS)categories/sit_gexpcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/parser/sit_parser.ao:	\
	$(THIS)extree/sit_extree.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/parser/sit_scanner.ao:	\
	$(THIS)extree/operators/sit_OPassgn.ao	\
	$(THIS)extree/operators/sit_OPless.ao	\
	$(THIS)extree/operators/sit_OPmore.ao	\
	$(THIS)extree/parser/sit_token.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/parser/sit_token.ao:	\
	$(THIS)extree/operators/sit_OPexpt.ao	\
	$(THIS)extree/operators/sit_OPplus.ao	\
	$(THIS)extree/operators/sit_OPprefx.ao	\
	$(THIS)extree/operators/sit_OPquot.ao	\
	$(THIS)extree/operators/sit_OPtimes.ao	\
	$(THIS)util/alg_version.ao
$(THIS)extree/sit_extree.ao:	\
	$(THIS)util/alg_version.ao
$(THIS)extree/sit_optools.ao:	\
	$(THIS)extree/sit_extree.ao	\
	$(THIS)util/alg_version.ao
$(THIS)ffield/alg_pf2.ao:	\
	$(THIS)ffield/sit_sprfcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)ffield/sit_prfcat.ao:	\
	$(THIS)polyfactorp/sit_upfactp.ao	\
	$(THIS)util/alg_version.ao
$(THIS)ffield/sit_spf.ao:	\
	$(THIS)ffield/sit_sprfcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)ffield/sit_sprfcat.ao:	\
	$(THIS)ffield/sit_prfcat.ao	\
	$(THIS)ffield/sit_sprfgcd.ao	\
	$(THIS)ffield/sit_sprfmat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)ffield/sit_sprfgcd.ao:	\
	$(THIS)univpoly/categories/sit_upolc0.ao	\
	$(THIS)univpoly/gcd/sit_modpgcd.ao	\
	$(THIS)util/alg_version.ao
$(THIS)ffield/sit_sprfmat.ao:	\
	$(THIS)categories/sit_spfcat0.ao	\
	$(THIS)mat/modular/sit_modpoge.ao	\
	$(THIS)mat/sit_matcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)ffield/sit_zpf.ao:	\
	$(THIS)ffield/sit_sprfcat.ao	\
	$(THIS)numbers/sit_prmroot.ao	\
	$(THIS)util/alg_version.ao
$(THIS)fraction/sit_lcqotct.ao:	\
	$(THIS)fraction/sit_qotbyc0.ao	\
	$(THIS)fraction/sit_qotfct0.ao	\
	$(THIS)util/alg_version.ao
$(THIS)fraction/sit_matquot.ao:	\
	$(THIS)fraction/sit_vecquot.ao	\
	$(THIS)util/alg_version.ao
$(THIS)fraction/sit_qotbyc0.ao:	\
	$(THIS)fraction/sit_quotcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)fraction/sit_qotbyct.ao:	\
	$(THIS)fraction/sit_matquot.ao	\
	$(THIS)fraction/sit_uflgqot.ao	\
	$(THIS)univpoly/sit_dup.ao	\
	$(THIS)util/alg_version.ao
$(THIS)fraction/sit_qotfcat.ao:	\
	$(THIS)fraction/sit_matquot.ao	\
	$(THIS)fraction/sit_uflgqot.ao	\
	$(THIS)univpoly/sit_dup.ao	\
	$(THIS)util/alg_version.ao
$(THIS)fraction/sit_qotfct0.ao:	\
	$(THIS)fraction/sit_quotcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)fraction/sit_qotient.ao:	\
	$(THIS)fraction/sit_qotfcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)fraction/sit_quotby.ao:	\
	$(THIS)fraction/sit_qotbyct.ao	\
	$(THIS)util/alg_version.ao
$(THIS)fraction/sit_quotcat.ao:	\
	$(THIS)categories/sit_algebra.ao	\
	$(THIS)categories/sit_diffext.ao	\
	$(THIS)univpoly/categories/sit_fring.ao	\
	$(THIS)univpoly/categories/sit_ugring.ao	\
	$(THIS)util/alg_version.ao
$(THIS)fraction/sit_uflgqot.ao:	\
	$(THIS)fraction/sit_lcqotct.ao	\
	$(THIS)util/alg_version.ao
$(THIS)fraction/sit_vecquot.ao:	\
	$(THIS)fraction/sit_lcqotct.ao	\
	$(THIS)util/alg_version.ao
$(THIS)mat/gauss/sit_dfge.ao:	\
	$(THIS)basic/sit_permut.ao	\
	$(THIS)mat/gauss/sit_linelim.ao	\
	$(THIS)util/alg_version.ao
$(THIS)mat/gauss/sit_ff2ge.ao:	\
	$(THIS)basic/sit_permut.ao	\
	$(THIS)mat/gauss/sit_linelim.ao	\
	$(THIS)util/alg_version.ao
$(THIS)mat/gauss/sit_ffge.ao:	\
	$(THIS)basic/sit_permut.ao	\
	$(THIS)mat/gauss/sit_linelim.ao	\
	$(THIS)util/alg_version.ao
$(THIS)mat/gauss/sit_hermge.ao:	\
	$(THIS)basic/sit_permut.ao	\
	$(THIS)categories/sit_euclid.ao	\
	$(THIS)mat/gauss/sit_linelim.ao	\
	$(THIS)util/alg_version.ao
$(THIS)mat/gauss/sit_linelim.ao:	\
	$(THIS)mat/sit_matcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)mat/gauss/sit_oge.ao:	\
	$(THIS)basic/sit_permut.ao	\
	$(THIS)categories/sit_field.ao	\
	$(THIS)mat/gauss/sit_linelim.ao	\
	$(THIS)util/alg_version.ao
$(THIS)mat/linalg/sit_bsolve.ao:	\
	$(THIS)basic/sit_permut.ao	\
	$(THIS)categories/sit_field.ao	\
	$(THIS)mat/sit_matcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)mat/linalg/sit_laring.ao:	\
	$(THIS)mat/sit_matcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)mat/linalg/sit_linalg.ao:	\
	$(THIS)mat/gauss/sit_dfge.ao	\
	$(THIS)mat/gauss/sit_ff2ge.ao	\
	$(THIS)mat/gauss/sit_oge.ao	\
	$(THIS)mat/linalg/sit_bsolve.ao	\
	$(THIS)mat/linalg/sit_laring.ao	\
	$(THIS)mat/linalg/sit_overdet.ao	\
	$(THIS)mat/modular/compbug/sit_speclin.ao	\
	$(THIS)util/alg_version.ao
$(THIS)mat/linalg/sit_overdet.ao:	\
	$(THIS)categories/sit_gcd.ao	\
	$(THIS)mat/sit_matcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)mat/linalg2/alg_ffupla.ao:	\
	$(THIS)mat/linalg2/sit_popov.ao	\
	$(THIS)util/alg_version.ao
$(THIS)mat/linalg2/sit_popov.ao:	\
	$(THIS)univpoly/categories/sit_upolc0.ao	\
	$(THIS)util/alg_version.ao
$(THIS)mat/linalg2/sit_upcrtla.ao:	\
	$(THIS)univpoly/categories/sit_upolc0.ao	\
	$(THIS)util/alg_version.ao
$(THIS)mat/linalg3/sit_hensela.ao:	\
	$(THIS)fraction/sit_qotient.ao	\
	$(THIS)mat/linalg2/sit_upcrtla.ao	\
	$(THIS)util/alg_version.ao
$(THIS)mat/modular/compbug/sit_speclin.ao:	\
	$(THIS)categories/sit_spf0.ao	\
	$(THIS)categories/sit_spzble.ao	\
	$(THIS)mat/modular/sit_modpoge.ao	\
	$(THIS)mat/sit_matcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)mat/modular/sit_modpoge.ao:	\
	$(THIS)numbers/sit_primgen.ao	\
	$(THIS)util/alg_version.ao
$(THIS)mat/modular/sit_zcrtla.ao:	\
	$(THIS)categories/sit_integer.ao	\
	$(THIS)univpoly/gcd/sit_gcdintg.ao	\
	$(THIS)util/alg_version.ao
$(THIS)mat/sit_dnsemat.ao:	\
	$(THIS)mat/sit_matcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)mat/sit_matcat.ao:	\
	$(THIS)categories/sit_difring.ao	\
	$(THIS)categories/sit_linarit.ao	\
	$(THIS)extree/operators/sit_OPmatrx.ao	\
	$(THIS)mat/sit_vector.ao	\
	$(THIS)util/alg_version.ao
$(THIS)mat/sit_vector.ao:	\
	$(THIS)extree/operators/sit_OPvect.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/exponent/sm_dirprod.ao:	\
	$(THIS)multpoly/exponent/sm_dirprodc.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/exponent/sm_dirprodc.ao:	\
	$(THIS)categories/sit_gexpcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/exponent/sm_expocat.ao:	\
	$(THIS)categories/sit_gexpcat.ao	\
	$(THIS)multpoly/exponent/sm_vt.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/exponent/sm_fvt.ao:	\
	$(THIS)categories/sit_gexpcat.ao	\
	$(THIS)multpoly/exponent/sm_vt.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/exponent/sm_listovar.ao:	\
	$(THIS)multpoly/exponent/sm_fvt.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/exponent/sm_midl.ao:	\
	$(THIS)multpoly/exponent/sm_midrl.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/exponent/sm_midrl.ao:	\
	$(THIS)multpoly/exponent/sm_dirprod.ao	\
	$(THIS)multpoly/exponent/sm_mievc.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/exponent/sm_mievc.ao:	\
	$(THIS)multpoly/exponent/sm_expocat.ao	\
	$(THIS)multpoly/exponent/sm_fvt.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/exponent/sm_milex.ao:	\
	$(THIS)multpoly/exponent/sm_midrl.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/exponent/sm_osymbol.ao:	\
	$(THIS)multpoly/exponent/sm_vt.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/exponent/sm_tuplovar.ao:	\
	$(THIS)multpoly/exponent/sm_listovar.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/exponent/sm_vt.ao:	\
	$(THIS)categories/sit_basic.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/exponent/sm_zevc.ao:	\
	$(THIS)multpoly/exponent/sm_dirprodc.ao	\
	$(THIS)multpoly/exponent/sm_expocat.ao	\
	$(THIS)multpoly/exponent/sm_fvt.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/multpolycat/alg_defgcd.ao:	\
	$(THIS)multpoly/multpolycat/sm_polring0.ao	\
	$(THIS)univpoly/sit_dup.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/multpolycat/alg_poltype.ao:	\
	$(THIS)multpoly/multpolycat/alg_stdfrng.ao	\
	$(THIS)univpoly/alg_sup.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/multpolycat/alg_stdfrng.ao:	\
	$(THIS)categories/alg_frering.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/multpolycat/sm_famr0.ao:	\
	$(THIS)multpoly/exponent/sm_expocat.ao	\
	$(THIS)multpoly/multpolycat/sm_polring0.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/multpolycat/sm_polring.ao:	\
	$(THIS)multpoly/multpolycat/alg_defgcd.ao	\
	$(THIS)multpoly/multpolycat/sm_famr0.ao	\
	$(THIS)univpoly/gcd/sit_gcdintg.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/multpolycat/sm_polring0.ao:	\
	$(THIS)categories/sit_freealg.ao	\
	$(THIS)multpoly/multpolycat/alg_poltype.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/multpolycat/sm_rmpcat0.ao:	\
	$(THIS)multpoly/multpolycat/sm_polring.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/multpolydata/sm_delist.ao:	\
	$(THIS)categories/sit_basic.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/multpolydom/alg_smp.ao:	\
	$(THIS)multpoly/multpolycat/sm_rmpcat0.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/multpolydom/sm_dmp0.ao:	\
	$(THIS)categories/sit_gexpcat.ao	\
	$(THIS)categories/sit_idxfmod.ao	\
	$(THIS)multpoly/multpolydata/sm_delist.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/multpolydom/sm_dmp1.ao:	\
	$(THIS)multpoly/multpolycat/sm_famr0.ao	\
	$(THIS)multpoly/multpolydom/sm_dmp0.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/multpolydom/sm_rmp.ao:	\
	$(THIS)multpoly/multpolycat/sm_rmpcat0.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/multpolydom/sm_rmpz.ao:	\
	$(THIS)multpoly/exponent/sm_osymbol.ao	\
	$(THIS)multpoly/multpolydom/sm_rmp.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/multpolydom/sm_rmpzx.ao:	\
	$(THIS)multpoly/multpolydom/sm_rmp.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/multpolypkg/alg_ZpUVres.ao:	\
	$(THIS)ffield/sit_spf.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/multpolypkg/alg_bivarpk.ao:	\
	$(THIS)ffield/sit_spf.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/multpolypkg/alg_mresbiv.ao:	\
	$(THIS)multpoly/multpolypkg/alg_ZpUVres.ao	\
	$(THIS)multpoly/multpolypkg/alg_bivarpk.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/multpolytest/alg_bivtst1.ao:	\
	$(THIS)multpoly/multpolypkg/alg_bivarpk.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/multpolytest/alg_bivtst2.ao:	\
	$(THIS)multpoly/multpolypkg/alg_mresbiv.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/multpolytest/alg_bivtst3.ao:	\
	$(THIS)ffield/sit_spf.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/multpolytest/sm_dmp0pkgt.ao:	\
	$(THIS)categories/sit_gexpcat.ao	\
	$(THIS)categories/sit_idxfmod.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/multpolytest/sm_exppkgt.ao:	\
	$(THIS)multpoly/exponent/sm_expocat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)multpoly/multpolytest/sm_pr0pkgt.ao:	\
	$(THIS)multpoly/multpolycat/sm_polring0.ao	\
	$(THIS)util/alg_version.ao
$(THIS)numbers/sit_primes.ao:	\
	$(THIS)numbers/sit_prmtabl.ao	\
	$(THIS)util/alg_version.ao
$(THIS)numbers/sit_primgen.ao:	\
	$(THIS)numbers/sit_primes.ao	\
	$(THIS)util/alg_version.ao
$(THIS)numbers/sit_prmroot.ao:	\
	$(THIS)numbers/sit_primgen.ao	\
	$(THIS)util/alg_version.ao
$(THIS)numbers/sit_prmtabl.ao:	\
	$(THIS)categories/sit_gexpcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)polyfactor0/sit_fhensel.ao:	\
	$(THIS)categories/sit_integer.ao	\
	$(THIS)univpoly/categories/sit_upolc0.ao	\
	$(THIS)util/alg_version.ao
$(THIS)polyfactor0/sit_zfactor.ao:	\
	$(THIS)ffield/sit_zpf.ao	\
	$(THIS)polyfactor0/sit_fhensel.ao	\
	$(THIS)util/alg_version.ao
$(THIS)polyfactor0/sit_zfring.ao:	\
	$(THIS)univpoly/categories/sit_fring.ao	\
	$(THIS)util/alg_version.ao
$(THIS)polyfactor0/sit_zfringg.ao:	\
	$(THIS)univpoly/categories/sit_fring.ao	\
	$(THIS)util/alg_version.ao
$(THIS)polyfactorp/sit_upfactp.ao:	\
	$(THIS)algext/sit_upmod.ao	\
	$(THIS)util/alg_version.ao
$(THIS)series/alg_serpoly.ao:	\
	$(THIS)series/sit_sercat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)series/compbug/sit_duts.ao:	\
	$(THIS)extree/operators/sit_OPbigO.ao	\
	$(THIS)series/sit_sercat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)series/sit_seqence.ao:	\
	$(THIS)mat/sit_vector.ao	\
	$(THIS)univpoly/categories/sit_ufalg.ao	\
	$(THIS)util/alg_version.ao
$(THIS)series/sit_sercat.ao:	\
	$(THIS)fraction/sit_qotient.ao	\
	$(THIS)series/sit_seqence.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/alg_sup.ao:	\
	$(THIS)univpoly/alg_sup1.ao	\
	$(THIS)univpoly/sit_spread.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/alg_sup0.ao:	\
	$(THIS)categories/sit_idxfmod.ao	\
	$(THIS)categories/sit_integer.ao	\
	$(THIS)categories/sit_prfcat0.ao	\
	$(THIS)multpoly/multpolydata/sm_delist.ao	\
	$(THIS)univpoly/categories/sit_umonom.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/alg_sup1.ao:	\
	$(THIS)univpoly/alg_sup0.ao	\
	$(THIS)univpoly/categories/sit_upolalg.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/alg_unitool.ao:	\
	$(THIS)univpoly/categories/sit_upolc0.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/alg_uprcr.ao:	\
	$(THIS)univpoly/categories/sit_upolc0.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/categories/alg_chrem2.ao:	\
	$(THIS)univpoly/categories/sit_upolc0.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/categories/alg_modgcdp.ao:	\
	$(THIS)univpoly/categories/alg_chrem2.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/categories/alg_polydio.ao:	\
	$(THIS)univpoly/categories/sit_resprs.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/categories/sit_fftring.ao:	\
	$(THIS)univpoly/categories/sit_upolc0.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/categories/sit_fring.ao:	\
	$(THIS)univpoly/categories/sit_zring.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/categories/sit_froot.ao:	\
	$(THIS)categories/sit_field.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/categories/sit_resprs.ao:	\
	$(THIS)univpoly/categories/sit_upolc0.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/categories/sit_sqfree.ao:	\
	$(THIS)univpoly/categories/sit_upolc0.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/categories/sit_ufalg.ao:	\
	$(THIS)categories/sit_idxflar.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/categories/sit_uffalg.ao:	\
	$(THIS)categories/alg_idxfrng.ao	\
	$(THIS)mat/sit_dnsemat.ao	\
	$(THIS)univpoly/categories/sit_ufalg.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/categories/sit_ugring.ao:	\
	$(THIS)univpoly/categories/sit_upolc0.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/categories/sit_umonom.ao:	\
	$(THIS)categories/sit_charp.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/categories/sit_upolalg.ao:	\
	$(THIS)univpoly/categories/sit_uffalg.ao	\
	$(THIS)univpoly/categories/sit_umonom.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/categories/sit_upolc0.ao:	\
	$(THIS)basic/sit_product.ao	\
	$(THIS)categories/alg_modcmp.ao	\
	$(THIS)categories/sit_dcmprng.ao	\
	$(THIS)categories/sit_idxfalg.ao	\
	$(THIS)mat/linalg/sit_linalg.ao	\
	$(THIS)univpoly/categories/sit_upolalg.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/categories/sit_zring.ao:	\
	$(THIS)univpoly/categories/sit_froot.ao	\
	$(THIS)univpoly/categories/sit_upolc0.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/gcd/sit_gcdint.ao:	\
	$(THIS)univpoly/categories/sit_resprs.ao	\
	$(THIS)univpoly/categories/sit_ugring.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/gcd/sit_gcdintg.ao:	\
	$(THIS)univpoly/categories/sit_ugring.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/gcd/sit_heugcd.ao:	\
	$(THIS)categories/sit_integer.ao	\
	$(THIS)univpoly/categories/sit_upolc0.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/gcd/sit_modgcd.ao:	\
	$(THIS)categories/sit_integer.ao	\
	$(THIS)univpoly/categories/sit_upolc0.ao	\
	$(THIS)univpoly/gcd/sit_modpgcd.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/gcd/sit_modpgcd.ao:	\
	$(THIS)numbers/sit_primgen.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/sit_dup.ao:	\
	$(THIS)univpoly/alg_sup.ao	\
	$(THIS)univpoly/categories/sit_fftring.ao	\
	$(THIS)univpoly/sit_polkara.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/sit_polkara.ao:	\
	$(THIS)categories/sit_gexpcat.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/sit_spread.ao:	\
	$(THIS)univpoly/sit_upolc.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/sit_ufacpol.ao:	\
	$(THIS)extree/operators/sit_OPfact.ao	\
	$(THIS)univpoly/sit_upolc.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/sit_upolc.ao:	\
	$(THIS)mat/linalg2/alg_ffupla.ao	\
	$(THIS)univpoly/alg_uprcr.ao	\
	$(THIS)univpoly/categories/alg_modgcdp.ao	\
	$(THIS)univpoly/categories/sit_fring.ao	\
	$(THIS)univpoly/categories/sit_resprs.ao	\
	$(THIS)univpoly/categories/sit_sqfree.ao	\
	$(THIS)univpoly/categories/sit_ugring.ao	\
	$(THIS)util/alg_version.ao
$(THIS)univpoly/categories/sit_ufalg.ao:	\
	$(THIS)categories/sit_integer.ao
$(THIS)univpoly/categories/sit_froot.ao:	\
	$(THIS)categories/sit_integer.ao
$(THIS)multpoly/exponent/sm_expocat.ao:	\
	$(THIS)categories/sit_integer.ao
$(THIS)basic/sit_product.ao:	\
	$(THIS)categories/sit_integer.ao
$(THIS)categories/sit_fset.ao:	\
	$(THIS)categories/sit_integer.ao
$(THIS)univpoly/categories/sit_umonom.ao:	\
	$(THIS)categories/sit_integer.ao
$(THIS)univpoly/gcd/sit_gcdintg.ao:	\
	$(THIS)categories/sit_intgmp.ao
$(THIS)univpoly/gcd/sit_gcdint.ao:	\
	$(THIS)univpoly/gcd/sit_heugcd.ao	\
	$(THIS)univpoly/gcd/sit_modgcd.ao
$(THIS)polyfactor0/sit_zfring.ao:	\
	$(THIS)polyfactor0/sit_zfactor.ao
$(THIS)polyfactor0/sit_zfringg.ao:	\
	$(THIS)categories/sit_intgmp.ao	\
	$(THIS)polyfactor0/sit_zfactor.ao
$(THIS)util/alg_version.ao:	\
	$(LIBDIR)/libaldor$(LIBEXT)
