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
$(THIS)%.c: $(THIS)%.as build/aldor build/include/aldor.conf build/include/algebra.as build/include/algebrauid.as
	build/aldor $(AFLAGS) -q1 $<
	$(AR) cr build/libalgebra.al $(@:.c=.ao)

# Clean
clean: clean-$(THIS)
clean-$(THIS):
	$(RM) $(libalgebra_AOBJECTS)
	$(RM) $(libalgebra_COBJECTS)
	$(RM) $(libalgebra_ASOURCES:.as=.c)
	$(RM) build/libalgebra.a build/libalgebra.al

# Depend
$(THIS)algext/sit_algext.c:	\
	$(THIS)series/compbug/sit_duts.c	\
	$(THIS)util/alg_version.c
$(THIS)algext/sit_sae.c:	\
	$(THIS)algext/sit_saexcpt.c	\
	$(THIS)algext/sit_upmod.c	\
	$(THIS)util/alg_version.c
$(THIS)algext/sit_saexcpt.c:	\
	$(THIS)categories/sit_comring.c	\
	$(THIS)util/alg_version.c
$(THIS)algext/sit_upmod.c:	\
	$(THIS)algext/sit_algext.c	\
	$(THIS)util/alg_version.c
$(THIS)basic/compbug/sit_interp.c:	\
	$(THIS)basic/sit_pring.c	\
	$(THIS)categories/sit_gexpcat.c	\
	$(THIS)util/alg_version.c
$(THIS)basic/compbug/sit_shell.c:	\
	$(THIS)basic/compbug/sit_interp.c	\
	$(THIS)util/alg_version.c
$(THIS)basic/sit_complex.c:	\
	$(THIS)categories/alg_ffield.c	\
	$(THIS)categories/sit_algebra.c	\
	$(THIS)categories/sit_qring.c	\
	$(THIS)extree/operators/sit_OPcplex.c	\
	$(THIS)util/alg_version.c
$(THIS)basic/sit_indvar.c:	\
	$(THIS)extree/operators/sit_OPsubsc.c	\
	$(THIS)util/alg_version.c
$(THIS)basic/sit_mkpring.c:	\
	$(THIS)basic/sit_pring.c	\
	$(THIS)categories/sit_field.c	\
	$(THIS)util/alg_version.c
$(THIS)basic/sit_permut.c:	\
	$(THIS)categories/sit_gexpcat.c	\
	$(THIS)categories/sit_group.c	\
	$(THIS)util/alg_version.c
$(THIS)basic/sit_pring.c:	\
	$(THIS)categories/sit_basic.c	\
	$(THIS)util/alg_version.c
$(THIS)basic/sit_product.c:	\
	$(THIS)categories/sit_gexpcat.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/alg_cansimp.c:	\
	$(THIS)categories/sit_euclid.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/alg_ffield.c:	\
	$(THIS)categories/sit_charp.c	\
	$(THIS)categories/sit_field.c	\
	$(THIS)categories/sit_fset.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/alg_frering.c:	\
	$(THIS)categories/alg_rring.c	\
	$(THIS)categories/sit_charp.c	\
	$(THIS)categories/sit_freelar.c	\
	$(THIS)categories/sit_freemod.c	\
	$(THIS)categories/sit_qring.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/alg_idxfrng.c:	\
	$(THIS)categories/alg_frering.c	\
	$(THIS)categories/sit_idxflar.c	\
	$(THIS)categories/sit_idxfmod.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/alg_modcmp.c:	\
	$(THIS)categories/alg_cansimp.c	\
	$(THIS)categories/alg_rescls.c	\
	$(THIS)categories/sit_chrem.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/alg_primsrc.c:	\
	$(THIS)categories/sit_euclid.c	\
	$(THIS)categories/sit_gexpcat.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/alg_rescls.c:	\
	$(THIS)categories/alg_primsrc.c	\
	$(THIS)categories/sit_field.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/alg_rring.c:	\
	$(THIS)categories/sit_linarit.c	\
	$(THIS)categories/sit_module.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_abgroup.c:	\
	$(THIS)categories/sit_abmon.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_abmon.c:	\
	$(THIS)categories/sit_basic.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_algebra.c:	\
	$(THIS)categories/alg_rring.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_automor.c:	\
	$(THIS)categories/sit_gexpcat.c	\
	$(THIS)categories/sit_group.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_basic.c:	\
	$(THIS)categories/sit_pable.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_char0.c:	\
	$(THIS)categories/sit_ring.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_charp.c:	\
	$(THIS)categories/sit_gexpcat.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_chrem.c:	\
	$(THIS)categories/sit_euclid.c	\
	$(THIS)categories/sit_gexpcat.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_comring.c:	\
	$(THIS)categories/sit_ring.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_dcmprng.c:	\
	$(THIS)categories/sit_comring.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_deriv.c:	\
	$(THIS)categories/sit_gexpcat.c	\
	$(THIS)categories/sit_module.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_diffext.c:	\
	$(THIS)categories/sit_difring.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_difring.c:	\
	$(THIS)categories/sit_deriv.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_euclid.c:	\
	$(THIS)categories/sit_gcd.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_field.c:	\
	$(THIS)categories/sit_euclid.c	\
	$(THIS)categories/sit_group.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_freealg.c:	\
	$(THIS)categories/alg_frering.c	\
	$(THIS)categories/sit_algebra.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_freelar.c:	\
	$(THIS)categories/sit_freelc.c	\
	$(THIS)categories/sit_linarit.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_freelc.c:	\
	$(THIS)categories/sit_ring.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_freemod.c:	\
	$(THIS)categories/sit_field.c	\
	$(THIS)categories/sit_freelc.c	\
	$(THIS)categories/sit_module.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_fset.c:	\
	$(THIS)extree/operators/sit_OPsubsc.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_gcd.c:	\
	$(THIS)categories/sit_intdom.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_gexpcat.c:	\
	$(THIS)categories/sit_ptools.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_group.c:	\
	$(THIS)categories/sit_monoid.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_idxfalg.c:	\
	$(THIS)categories/alg_idxfrng.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_idxflar.c:	\
	$(THIS)categories/sit_freelar.c	\
	$(THIS)categories/sit_idxflc.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_idxflc.c:	\
	$(THIS)categories/sit_freelc.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_idxfmod.c:	\
	$(THIS)categories/sit_freemod.c	\
	$(THIS)categories/sit_idxflc.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_intdom.c:	\
	$(THIS)categories/sit_comring.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_integer.c:	\
	$(THIS)categories/alg_modcmp.c	\
	$(THIS)categories/sit_char0.c	\
	$(THIS)categories/sit_spzble.c	\
	$(THIS)numbers/sit_primgen.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_intgmp.c:	\
	$(THIS)categories/sit_integer.c	\
	$(THIS)univpoly/gcd/sit_gcdint.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_linarit.c:	\
	$(THIS)categories/sit_ring.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_module.c:	\
	$(THIS)categories/sit_ring.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_monoid.c:	\
	$(THIS)categories/sit_basic.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_ncid.c:	\
	$(THIS)categories/sit_ring.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_pable.c:	\
	$(THIS)extree/parser/sit_infexpr.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_prfcat0.c:	\
	$(THIS)categories/alg_ffield.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_ptools.c:	\
	$(THIS)categories/sit_intdom.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_qring.c:	\
	$(THIS)categories/sit_char0.c	\
	$(THIS)categories/sit_field.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_ring.c:	\
	$(THIS)categories/sit_abgroup.c	\
	$(THIS)categories/sit_monoid.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_spf0.c:	\
	$(THIS)categories/sit_spfcat0.c	\
	$(THIS)numbers/sit_primgen.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_spfcat0.c:	\
	$(THIS)categories/sit_prfcat0.c	\
	$(THIS)util/alg_version.c
$(THIS)categories/sit_spzble.c:	\
	$(THIS)categories/sit_comring.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/operators/sit_OPand.c:	\
	$(THIS)categories/sit_gexpcat.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/operators/sit_OPassgn.c:	\
	$(THIS)extree/sit_optools.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/operators/sit_OPbigO.c:	\
	$(THIS)categories/sit_gexpcat.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/operators/sit_OPcase.c:	\
	$(THIS)extree/operators/sit_OPif.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/operators/sit_OPcplex.c:	\
	$(THIS)categories/sit_gexpcat.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/operators/sit_OPequal.c:	\
	$(THIS)categories/sit_gexpcat.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/operators/sit_OPexpt.c:	\
	$(THIS)extree/sit_optools.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/operators/sit_OPfact.c:	\
	$(THIS)categories/sit_gexpcat.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/operators/sit_OPif.c:	\
	$(THIS)categories/sit_gexpcat.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/operators/sit_OPless.c:	\
	$(THIS)extree/sit_extree.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/operators/sit_OPlist.c:	\
	$(THIS)extree/sit_optools.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/operators/sit_OPllist.c:	\
	$(THIS)categories/sit_gexpcat.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/operators/sit_OPmatrx.c:	\
	$(THIS)categories/sit_gexpcat.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/operators/sit_OPminus.c:	\
	$(THIS)extree/sit_optools.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/operators/sit_OPmore.c:	\
	$(THIS)extree/sit_extree.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/operators/sit_OPnoteq.c:	\
	$(THIS)categories/sit_gexpcat.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/operators/sit_OPplus.c:	\
	$(THIS)extree/operators/sit_OPminus.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/operators/sit_OPprefx.c:	\
	$(THIS)extree/sit_optools.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/operators/sit_OPquot.c:	\
	$(THIS)extree/sit_optools.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/operators/sit_OPsubsc.c:	\
	$(THIS)categories/sit_gexpcat.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/operators/sit_OPtimes.c:	\
	$(THIS)extree/sit_optools.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/operators/sit_OPvect.c:	\
	$(THIS)categories/sit_gexpcat.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/parser/sit_infexpr.c:	\
	$(THIS)extree/operators/sit_OPlist.c	\
	$(THIS)extree/parser/sit_parser.c	\
	$(THIS)extree/parser/sit_scanner.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/parser/sit_lspexpr.c:	\
	$(THIS)extree/operators/sit_OPllist.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/parser/sit_maple.c:	\
	$(THIS)categories/sit_gexpcat.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/parser/sit_parser.c:	\
	$(THIS)extree/sit_extree.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/parser/sit_scanner.c:	\
	$(THIS)extree/operators/sit_OPassgn.c	\
	$(THIS)extree/operators/sit_OPless.c	\
	$(THIS)extree/operators/sit_OPmore.c	\
	$(THIS)extree/parser/sit_token.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/parser/sit_token.c:	\
	$(THIS)extree/operators/sit_OPexpt.c	\
	$(THIS)extree/operators/sit_OPplus.c	\
	$(THIS)extree/operators/sit_OPprefx.c	\
	$(THIS)extree/operators/sit_OPquot.c	\
	$(THIS)extree/operators/sit_OPtimes.c	\
	$(THIS)util/alg_version.c
$(THIS)extree/sit_extree.c:	\
	$(THIS)util/alg_version.c
$(THIS)extree/sit_optools.c:	\
	$(THIS)extree/sit_extree.c	\
	$(THIS)util/alg_version.c
$(THIS)ffield/alg_pf2.c:	\
	$(THIS)ffield/sit_sprfcat.c	\
	$(THIS)util/alg_version.c
$(THIS)ffield/sit_prfcat.c:	\
	$(THIS)polyfactorp/sit_upfactp.c	\
	$(THIS)util/alg_version.c
$(THIS)ffield/sit_spf.c:	\
	$(THIS)ffield/sit_sprfcat.c	\
	$(THIS)util/alg_version.c
$(THIS)ffield/sit_sprfcat.c:	\
	$(THIS)ffield/sit_prfcat.c	\
	$(THIS)ffield/sit_sprfgcd.c	\
	$(THIS)ffield/sit_sprfmat.c	\
	$(THIS)util/alg_version.c
$(THIS)ffield/sit_sprfgcd.c:	\
	$(THIS)univpoly/categories/sit_upolc0.c	\
	$(THIS)univpoly/gcd/sit_modpgcd.c	\
	$(THIS)util/alg_version.c
$(THIS)ffield/sit_sprfmat.c:	\
	$(THIS)categories/sit_spfcat0.c	\
	$(THIS)mat/modular/sit_modpoge.c	\
	$(THIS)mat/sit_matcat.c	\
	$(THIS)util/alg_version.c
$(THIS)ffield/sit_zpf.c:	\
	$(THIS)ffield/sit_sprfcat.c	\
	$(THIS)numbers/sit_prmroot.c	\
	$(THIS)util/alg_version.c
$(THIS)fraction/sit_lcqotct.c:	\
	$(THIS)fraction/sit_qotbyc0.c	\
	$(THIS)fraction/sit_qotfct0.c	\
	$(THIS)util/alg_version.c
$(THIS)fraction/sit_matquot.c:	\
	$(THIS)fraction/sit_vecquot.c	\
	$(THIS)util/alg_version.c
$(THIS)fraction/sit_qotbyc0.c:	\
	$(THIS)fraction/sit_quotcat.c	\
	$(THIS)util/alg_version.c
$(THIS)fraction/sit_qotbyct.c:	\
	$(THIS)fraction/sit_matquot.c	\
	$(THIS)fraction/sit_uflgqot.c	\
	$(THIS)univpoly/sit_dup.c	\
	$(THIS)util/alg_version.c
$(THIS)fraction/sit_qotfcat.c:	\
	$(THIS)fraction/sit_matquot.c	\
	$(THIS)fraction/sit_uflgqot.c	\
	$(THIS)univpoly/sit_dup.c	\
	$(THIS)util/alg_version.c
$(THIS)fraction/sit_qotfct0.c:	\
	$(THIS)fraction/sit_quotcat.c	\
	$(THIS)util/alg_version.c
$(THIS)fraction/sit_qotient.c:	\
	$(THIS)fraction/sit_qotfcat.c	\
	$(THIS)util/alg_version.c
$(THIS)fraction/sit_quotby.c:	\
	$(THIS)fraction/sit_qotbyct.c	\
	$(THIS)util/alg_version.c
$(THIS)fraction/sit_quotcat.c:	\
	$(THIS)categories/sit_algebra.c	\
	$(THIS)categories/sit_diffext.c	\
	$(THIS)univpoly/categories/sit_fring.c	\
	$(THIS)univpoly/categories/sit_ugring.c	\
	$(THIS)util/alg_version.c
$(THIS)fraction/sit_uflgqot.c:	\
	$(THIS)fraction/sit_lcqotct.c	\
	$(THIS)util/alg_version.c
$(THIS)fraction/sit_vecquot.c:	\
	$(THIS)fraction/sit_lcqotct.c	\
	$(THIS)util/alg_version.c
$(THIS)mat/gauss/sit_dfge.c:	\
	$(THIS)basic/sit_permut.c	\
	$(THIS)mat/gauss/sit_linelim.c	\
	$(THIS)util/alg_version.c
$(THIS)mat/gauss/sit_ff2ge.c:	\
	$(THIS)basic/sit_permut.c	\
	$(THIS)mat/gauss/sit_linelim.c	\
	$(THIS)util/alg_version.c
$(THIS)mat/gauss/sit_ffge.c:	\
	$(THIS)basic/sit_permut.c	\
	$(THIS)mat/gauss/sit_linelim.c	\
	$(THIS)util/alg_version.c
$(THIS)mat/gauss/sit_hermge.c:	\
	$(THIS)basic/sit_permut.c	\
	$(THIS)categories/sit_euclid.c	\
	$(THIS)mat/gauss/sit_linelim.c	\
	$(THIS)util/alg_version.c
$(THIS)mat/gauss/sit_linelim.c:	\
	$(THIS)mat/sit_matcat.c	\
	$(THIS)util/alg_version.c
$(THIS)mat/gauss/sit_oge.c:	\
	$(THIS)basic/sit_permut.c	\
	$(THIS)categories/sit_field.c	\
	$(THIS)mat/gauss/sit_linelim.c	\
	$(THIS)util/alg_version.c
$(THIS)mat/linalg/sit_bsolve.c:	\
	$(THIS)basic/sit_permut.c	\
	$(THIS)categories/sit_field.c	\
	$(THIS)mat/sit_matcat.c	\
	$(THIS)util/alg_version.c
$(THIS)mat/linalg/sit_laring.c:	\
	$(THIS)mat/sit_matcat.c	\
	$(THIS)util/alg_version.c
$(THIS)mat/linalg/sit_linalg.c:	\
	$(THIS)mat/gauss/sit_dfge.c	\
	$(THIS)mat/gauss/sit_ff2ge.c	\
	$(THIS)mat/gauss/sit_oge.c	\
	$(THIS)mat/linalg/sit_bsolve.c	\
	$(THIS)mat/linalg/sit_laring.c	\
	$(THIS)mat/linalg/sit_overdet.c	\
	$(THIS)mat/modular/compbug/sit_speclin.c	\
	$(THIS)util/alg_version.c
$(THIS)mat/linalg/sit_overdet.c:	\
	$(THIS)categories/sit_gcd.c	\
	$(THIS)mat/sit_matcat.c	\
	$(THIS)util/alg_version.c
$(THIS)mat/linalg2/alg_ffupla.c:	\
	$(THIS)mat/linalg2/sit_popov.c	\
	$(THIS)util/alg_version.c
$(THIS)mat/linalg2/sit_popov.c:	\
	$(THIS)univpoly/categories/sit_upolc0.c	\
	$(THIS)util/alg_version.c
$(THIS)mat/linalg2/sit_upcrtla.c:	\
	$(THIS)univpoly/categories/sit_upolc0.c	\
	$(THIS)util/alg_version.c
$(THIS)mat/linalg3/sit_hensela.c:	\
	$(THIS)fraction/sit_qotient.c	\
	$(THIS)mat/linalg2/sit_upcrtla.c	\
	$(THIS)util/alg_version.c
$(THIS)mat/modular/compbug/sit_speclin.c:	\
	$(THIS)categories/sit_spf0.c	\
	$(THIS)categories/sit_spzble.c	\
	$(THIS)mat/modular/sit_modpoge.c	\
	$(THIS)mat/sit_matcat.c	\
	$(THIS)util/alg_version.c
$(THIS)mat/modular/sit_modpoge.c:	\
	$(THIS)numbers/sit_primgen.c	\
	$(THIS)util/alg_version.c
$(THIS)mat/modular/sit_zcrtla.c:	\
	$(THIS)categories/sit_integer.c	\
	$(THIS)univpoly/gcd/sit_gcdintg.c	\
	$(THIS)util/alg_version.c
$(THIS)mat/sit_dnsemat.c:	\
	$(THIS)mat/sit_matcat.c	\
	$(THIS)util/alg_version.c
$(THIS)mat/sit_matcat.c:	\
	$(THIS)categories/sit_difring.c	\
	$(THIS)categories/sit_linarit.c	\
	$(THIS)extree/operators/sit_OPmatrx.c	\
	$(THIS)mat/sit_vector.c	\
	$(THIS)util/alg_version.c
$(THIS)mat/sit_vector.c:	\
	$(THIS)extree/operators/sit_OPvect.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/exponent/sm_dirprod.c:	\
	$(THIS)multpoly/exponent/sm_dirprodc.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/exponent/sm_dirprodc.c:	\
	$(THIS)categories/sit_gexpcat.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/exponent/sm_expocat.c:	\
	$(THIS)categories/sit_gexpcat.c	\
	$(THIS)multpoly/exponent/sm_vt.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/exponent/sm_fvt.c:	\
	$(THIS)categories/sit_gexpcat.c	\
	$(THIS)multpoly/exponent/sm_vt.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/exponent/sm_listovar.c:	\
	$(THIS)multpoly/exponent/sm_fvt.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/exponent/sm_midl.c:	\
	$(THIS)multpoly/exponent/sm_midrl.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/exponent/sm_midrl.c:	\
	$(THIS)multpoly/exponent/sm_dirprod.c	\
	$(THIS)multpoly/exponent/sm_mievc.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/exponent/sm_mievc.c:	\
	$(THIS)multpoly/exponent/sm_expocat.c	\
	$(THIS)multpoly/exponent/sm_fvt.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/exponent/sm_milex.c:	\
	$(THIS)multpoly/exponent/sm_midrl.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/exponent/sm_osymbol.c:	\
	$(THIS)multpoly/exponent/sm_vt.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/exponent/sm_tuplovar.c:	\
	$(THIS)multpoly/exponent/sm_listovar.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/exponent/sm_vt.c:	\
	$(THIS)categories/sit_basic.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/exponent/sm_zevc.c:	\
	$(THIS)multpoly/exponent/sm_dirprodc.c	\
	$(THIS)multpoly/exponent/sm_expocat.c	\
	$(THIS)multpoly/exponent/sm_fvt.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/multpolycat/alg_defgcd.c:	\
	$(THIS)multpoly/multpolycat/sm_polring0.c	\
	$(THIS)univpoly/sit_dup.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/multpolycat/alg_poltype.c:	\
	$(THIS)multpoly/multpolycat/alg_stdfrng.c	\
	$(THIS)univpoly/alg_sup.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/multpolycat/alg_stdfrng.c:	\
	$(THIS)categories/alg_frering.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/multpolycat/sm_famr0.c:	\
	$(THIS)multpoly/exponent/sm_expocat.c	\
	$(THIS)multpoly/multpolycat/sm_polring0.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/multpolycat/sm_polring.c:	\
	$(THIS)multpoly/multpolycat/alg_defgcd.c	\
	$(THIS)multpoly/multpolycat/sm_famr0.c	\
	$(THIS)univpoly/gcd/sit_gcdintg.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/multpolycat/sm_polring0.c:	\
	$(THIS)categories/sit_freealg.c	\
	$(THIS)multpoly/multpolycat/alg_poltype.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/multpolycat/sm_rmpcat0.c:	\
	$(THIS)multpoly/multpolycat/sm_polring.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/multpolydata/sm_delist.c:	\
	$(THIS)categories/sit_basic.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/multpolydom/alg_smp.c:	\
	$(THIS)multpoly/multpolycat/sm_rmpcat0.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/multpolydom/sm_dmp0.c:	\
	$(THIS)categories/sit_gexpcat.c	\
	$(THIS)categories/sit_idxfmod.c	\
	$(THIS)multpoly/multpolydata/sm_delist.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/multpolydom/sm_dmp1.c:	\
	$(THIS)multpoly/multpolycat/sm_famr0.c	\
	$(THIS)multpoly/multpolydom/sm_dmp0.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/multpolydom/sm_rmp.c:	\
	$(THIS)multpoly/multpolycat/sm_rmpcat0.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/multpolydom/sm_rmpz.c:	\
	$(THIS)multpoly/exponent/sm_osymbol.c	\
	$(THIS)multpoly/multpolydom/sm_rmp.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/multpolydom/sm_rmpzx.c:	\
	$(THIS)multpoly/multpolydom/sm_rmp.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/multpolypkg/alg_ZpUVres.c:	\
	$(THIS)ffield/sit_spf.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/multpolypkg/alg_bivarpk.c:	\
	$(THIS)ffield/sit_spf.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/multpolypkg/alg_mresbiv.c:	\
	$(THIS)multpoly/multpolypkg/alg_ZpUVres.c	\
	$(THIS)multpoly/multpolypkg/alg_bivarpk.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/multpolytest/alg_bivtst1.c:	\
	$(THIS)multpoly/multpolypkg/alg_bivarpk.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/multpolytest/alg_bivtst2.c:	\
	$(THIS)multpoly/multpolypkg/alg_mresbiv.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/multpolytest/alg_bivtst3.c:	\
	$(THIS)ffield/sit_spf.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/multpolytest/sm_dmp0pkgt.c:	\
	$(THIS)categories/sit_gexpcat.c	\
	$(THIS)categories/sit_idxfmod.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/multpolytest/sm_exppkgt.c:	\
	$(THIS)multpoly/exponent/sm_expocat.c	\
	$(THIS)util/alg_version.c
$(THIS)multpoly/multpolytest/sm_pr0pkgt.c:	\
	$(THIS)multpoly/multpolycat/sm_polring0.c	\
	$(THIS)util/alg_version.c
$(THIS)numbers/sit_primes.c:	\
	$(THIS)numbers/sit_prmtabl.c	\
	$(THIS)util/alg_version.c
$(THIS)numbers/sit_primgen.c:	\
	$(THIS)numbers/sit_primes.c	\
	$(THIS)util/alg_version.c
$(THIS)numbers/sit_prmroot.c:	\
	$(THIS)numbers/sit_primgen.c	\
	$(THIS)util/alg_version.c
$(THIS)numbers/sit_prmtabl.c:	\
	$(THIS)categories/sit_gexpcat.c	\
	$(THIS)util/alg_version.c
$(THIS)polyfactor0/sit_fhensel.c:	\
	$(THIS)categories/sit_integer.c	\
	$(THIS)univpoly/categories/sit_upolc0.c	\
	$(THIS)util/alg_version.c
$(THIS)polyfactor0/sit_zfactor.c:	\
	$(THIS)ffield/sit_zpf.c	\
	$(THIS)polyfactor0/sit_fhensel.c	\
	$(THIS)util/alg_version.c
$(THIS)polyfactor0/sit_zfring.c:	\
	$(THIS)univpoly/categories/sit_fring.c	\
	$(THIS)util/alg_version.c
$(THIS)polyfactor0/sit_zfringg.c:	\
	$(THIS)univpoly/categories/sit_fring.c	\
	$(THIS)util/alg_version.c
$(THIS)polyfactorp/sit_upfactp.c:	\
	$(THIS)algext/sit_upmod.c	\
	$(THIS)util/alg_version.c
$(THIS)series/alg_serpoly.c:	\
	$(THIS)series/sit_sercat.c	\
	$(THIS)util/alg_version.c
$(THIS)series/compbug/sit_duts.c:	\
	$(THIS)extree/operators/sit_OPbigO.c	\
	$(THIS)series/sit_sercat.c	\
	$(THIS)util/alg_version.c
$(THIS)series/sit_seqence.c:	\
	$(THIS)mat/sit_vector.c	\
	$(THIS)univpoly/categories/sit_ufalg.c	\
	$(THIS)util/alg_version.c
$(THIS)series/sit_sercat.c:	\
	$(THIS)fraction/sit_qotient.c	\
	$(THIS)series/sit_seqence.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/alg_sup.c:	\
	$(THIS)univpoly/alg_sup1.c	\
	$(THIS)univpoly/sit_spread.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/alg_sup0.c:	\
	$(THIS)categories/sit_idxfmod.c	\
	$(THIS)categories/sit_integer.c	\
	$(THIS)categories/sit_prfcat0.c	\
	$(THIS)multpoly/multpolydata/sm_delist.c	\
	$(THIS)univpoly/categories/sit_umonom.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/alg_sup1.c:	\
	$(THIS)univpoly/alg_sup0.c	\
	$(THIS)univpoly/categories/sit_upolalg.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/alg_unitool.c:	\
	$(THIS)univpoly/categories/sit_upolc0.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/alg_uprcr.c:	\
	$(THIS)univpoly/categories/sit_upolc0.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/categories/alg_chrem2.c:	\
	$(THIS)univpoly/categories/sit_upolc0.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/categories/alg_modgcdp.c:	\
	$(THIS)univpoly/categories/alg_chrem2.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/categories/alg_polydio.c:	\
	$(THIS)univpoly/categories/sit_resprs.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/categories/sit_fftring.c:	\
	$(THIS)univpoly/categories/sit_upolc0.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/categories/sit_fring.c:	\
	$(THIS)univpoly/categories/sit_zring.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/categories/sit_froot.c:	\
	$(THIS)categories/sit_field.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/categories/sit_resprs.c:	\
	$(THIS)univpoly/categories/sit_upolc0.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/categories/sit_sqfree.c:	\
	$(THIS)univpoly/categories/sit_upolc0.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/categories/sit_ufalg.c:	\
	$(THIS)categories/sit_idxflar.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/categories/sit_uffalg.c:	\
	$(THIS)categories/alg_idxfrng.c	\
	$(THIS)mat/sit_dnsemat.c	\
	$(THIS)univpoly/categories/sit_ufalg.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/categories/sit_ugring.c:	\
	$(THIS)univpoly/categories/sit_upolc0.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/categories/sit_umonom.c:	\
	$(THIS)categories/sit_charp.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/categories/sit_upolalg.c:	\
	$(THIS)univpoly/categories/sit_uffalg.c	\
	$(THIS)univpoly/categories/sit_umonom.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/categories/sit_upolc0.c:	\
	$(THIS)basic/sit_product.c	\
	$(THIS)categories/alg_modcmp.c	\
	$(THIS)categories/sit_dcmprng.c	\
	$(THIS)categories/sit_idxfalg.c	\
	$(THIS)mat/linalg/sit_linalg.c	\
	$(THIS)univpoly/categories/sit_upolalg.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/categories/sit_zring.c:	\
	$(THIS)univpoly/categories/sit_froot.c	\
	$(THIS)univpoly/categories/sit_upolc0.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/gcd/sit_gcdint.c:	\
	$(THIS)univpoly/categories/sit_resprs.c	\
	$(THIS)univpoly/categories/sit_ugring.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/gcd/sit_gcdintg.c:	\
	$(THIS)univpoly/categories/sit_ugring.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/gcd/sit_heugcd.c:	\
	$(THIS)categories/sit_integer.c	\
	$(THIS)univpoly/categories/sit_upolc0.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/gcd/sit_modgcd.c:	\
	$(THIS)categories/sit_integer.c	\
	$(THIS)univpoly/categories/sit_upolc0.c	\
	$(THIS)univpoly/gcd/sit_modpgcd.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/gcd/sit_modpgcd.c:	\
	$(THIS)numbers/sit_primgen.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/sit_dup.c:	\
	$(THIS)univpoly/alg_sup.c	\
	$(THIS)univpoly/categories/sit_fftring.c	\
	$(THIS)univpoly/sit_polkara.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/sit_polkara.c:	\
	$(THIS)categories/sit_gexpcat.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/sit_spread.c:	\
	$(THIS)univpoly/sit_upolc.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/sit_ufacpol.c:	\
	$(THIS)extree/operators/sit_OPfact.c	\
	$(THIS)univpoly/sit_upolc.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/sit_upolc.c:	\
	$(THIS)mat/linalg2/alg_ffupla.c	\
	$(THIS)univpoly/alg_uprcr.c	\
	$(THIS)univpoly/categories/alg_modgcdp.c	\
	$(THIS)univpoly/categories/sit_fring.c	\
	$(THIS)univpoly/categories/sit_resprs.c	\
	$(THIS)univpoly/categories/sit_sqfree.c	\
	$(THIS)univpoly/categories/sit_ugring.c	\
	$(THIS)util/alg_version.c
$(THIS)univpoly/categories/sit_ufalg.c:	\
	$(THIS)categories/sit_integer.c
$(THIS)univpoly/categories/sit_froot.c:	\
	$(THIS)categories/sit_integer.c
$(THIS)multpoly/exponent/sm_expocat.c:	\
	$(THIS)categories/sit_integer.c
$(THIS)basic/sit_product.c:	\
	$(THIS)categories/sit_integer.c
$(THIS)categories/sit_fset.c:	\
	$(THIS)categories/sit_integer.c
$(THIS)univpoly/categories/sit_umonom.c:	\
	$(THIS)categories/sit_integer.c
$(THIS)univpoly/gcd/sit_gcdintg.c:	\
	$(THIS)categories/sit_intgmp.c
$(THIS)univpoly/gcd/sit_gcdint.c:	\
	$(THIS)univpoly/gcd/sit_heugcd.c	\
	$(THIS)univpoly/gcd/sit_modgcd.c
$(THIS)polyfactor0/sit_zfring.c:	\
	$(THIS)polyfactor0/sit_zfactor.c
$(THIS)polyfactor0/sit_zfringg.c:	\
	$(THIS)categories/sit_intgmp.c	\
	$(THIS)polyfactor0/sit_zfactor.c
$(THIS)util/alg_version.c:	\
	build/libaldor.a
