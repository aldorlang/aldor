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
libalgebra_COBJECTS := $(libalgebra_ASOURCES:.as=$(OBJEXT))


# C library
$(LIBDIR)/libalgebra$(LIBEXT): $(libalgebra_COBJECTS)
	$(AR) cr $@ $^

# Local aldor build rule
$(THIS)%$(OBJEXT): $(THIS)%.as $(BINDIR)/aldor$(EXEEXT) $(BINDIR)/unicl$(EXEEXT) $(aldor_HEADERS) $(INCDIR)/algebra.as $(INCDIR)/algebrauid.as
	$(BINDIR)/aldor$(EXEEXT) $(AFLAGS) -q1 $<
	$(AR) cr $(LIBDIR)/libalgebra.al $(@:$(OBJEXT)=.ao)

# Clean
clean: clean-libalgebra
clean-libalgebra:
	$(RM) $(libalgebra_AOBJECTS)
	$(RM) $(libalgebra_COBJECTS)
	$(RM) $(libalgebra_ASOURCES:.as=.c)
	$(RM) $(LIBDIR)/libalgebra$(LIBEXT) $(LIBDIR)/libalgebra.al

# Depend
$(THIS)algext/sit_algext$(OBJEXT):	\
	$(THIS)series/compbug/sit_duts$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)algext/sit_sae$(OBJEXT):	\
	$(THIS)algext/sit_saexcpt$(OBJEXT)	\
	$(THIS)algext/sit_upmod$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)algext/sit_saexcpt$(OBJEXT):	\
	$(THIS)categories/sit_comring$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)algext/sit_upmod$(OBJEXT):	\
	$(THIS)algext/sit_algext$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)basic/compbug/sit_interp$(OBJEXT):	\
	$(THIS)basic/sit_pring$(OBJEXT)	\
	$(THIS)categories/sit_gexpcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)basic/compbug/sit_shell$(OBJEXT):	\
	$(THIS)basic/compbug/sit_interp$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)basic/sit_complex$(OBJEXT):	\
	$(THIS)categories/alg_ffield$(OBJEXT)	\
	$(THIS)categories/sit_algebra$(OBJEXT)	\
	$(THIS)categories/sit_qring$(OBJEXT)	\
	$(THIS)extree/operators/sit_OPcplex$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)basic/sit_indvar$(OBJEXT):	\
	$(THIS)extree/operators/sit_OPsubsc$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)basic/sit_mkpring$(OBJEXT):	\
	$(THIS)basic/sit_pring$(OBJEXT)	\
	$(THIS)categories/sit_field$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)basic/sit_permut$(OBJEXT):	\
	$(THIS)categories/sit_gexpcat$(OBJEXT)	\
	$(THIS)categories/sit_group$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)basic/sit_pring$(OBJEXT):	\
	$(THIS)categories/sit_basic$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)basic/sit_product$(OBJEXT):	\
	$(THIS)categories/sit_gexpcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/alg_cansimp$(OBJEXT):	\
	$(THIS)categories/sit_euclid$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/alg_ffield$(OBJEXT):	\
	$(THIS)categories/sit_charp$(OBJEXT)	\
	$(THIS)categories/sit_field$(OBJEXT)	\
	$(THIS)categories/sit_fset$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/alg_frering$(OBJEXT):	\
	$(THIS)categories/alg_rring$(OBJEXT)	\
	$(THIS)categories/sit_charp$(OBJEXT)	\
	$(THIS)categories/sit_freelar$(OBJEXT)	\
	$(THIS)categories/sit_freemod$(OBJEXT)	\
	$(THIS)categories/sit_qring$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/alg_idxfrng$(OBJEXT):	\
	$(THIS)categories/alg_frering$(OBJEXT)	\
	$(THIS)categories/sit_idxflar$(OBJEXT)	\
	$(THIS)categories/sit_idxfmod$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/alg_modcmp$(OBJEXT):	\
	$(THIS)categories/alg_cansimp$(OBJEXT)	\
	$(THIS)categories/alg_rescls$(OBJEXT)	\
	$(THIS)categories/sit_chrem$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/alg_primsrc$(OBJEXT):	\
	$(THIS)categories/sit_euclid$(OBJEXT)	\
	$(THIS)categories/sit_gexpcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/alg_rescls$(OBJEXT):	\
	$(THIS)categories/alg_primsrc$(OBJEXT)	\
	$(THIS)categories/sit_field$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/alg_rring$(OBJEXT):	\
	$(THIS)categories/sit_linarit$(OBJEXT)	\
	$(THIS)categories/sit_module$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_abgroup$(OBJEXT):	\
	$(THIS)categories/sit_abmon$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_abmon$(OBJEXT):	\
	$(THIS)categories/sit_basic$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_algebra$(OBJEXT):	\
	$(THIS)categories/alg_rring$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_automor$(OBJEXT):	\
	$(THIS)categories/sit_gexpcat$(OBJEXT)	\
	$(THIS)categories/sit_group$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_basic$(OBJEXT):	\
	$(THIS)categories/sit_pable$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_char0$(OBJEXT):	\
	$(THIS)categories/sit_ring$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_charp$(OBJEXT):	\
	$(THIS)categories/sit_gexpcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_chrem$(OBJEXT):	\
	$(THIS)categories/sit_euclid$(OBJEXT)	\
	$(THIS)categories/sit_gexpcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_comring$(OBJEXT):	\
	$(THIS)categories/sit_ring$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_dcmprng$(OBJEXT):	\
	$(THIS)categories/sit_comring$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_deriv$(OBJEXT):	\
	$(THIS)categories/sit_gexpcat$(OBJEXT)	\
	$(THIS)categories/sit_module$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_diffext$(OBJEXT):	\
	$(THIS)categories/sit_difring$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_difring$(OBJEXT):	\
	$(THIS)categories/sit_deriv$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_euclid$(OBJEXT):	\
	$(THIS)categories/sit_gcd$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_field$(OBJEXT):	\
	$(THIS)categories/sit_euclid$(OBJEXT)	\
	$(THIS)categories/sit_group$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_freealg$(OBJEXT):	\
	$(THIS)categories/alg_frering$(OBJEXT)	\
	$(THIS)categories/sit_algebra$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_freelar$(OBJEXT):	\
	$(THIS)categories/sit_freelc$(OBJEXT)	\
	$(THIS)categories/sit_linarit$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_freelc$(OBJEXT):	\
	$(THIS)categories/sit_ring$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_freemod$(OBJEXT):	\
	$(THIS)categories/sit_field$(OBJEXT)	\
	$(THIS)categories/sit_freelc$(OBJEXT)	\
	$(THIS)categories/sit_module$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_fset$(OBJEXT):	\
	$(THIS)extree/operators/sit_OPsubsc$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_gcd$(OBJEXT):	\
	$(THIS)categories/sit_intdom$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_gexpcat$(OBJEXT):	\
	$(THIS)categories/sit_ptools$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_group$(OBJEXT):	\
	$(THIS)categories/sit_monoid$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_idxfalg$(OBJEXT):	\
	$(THIS)categories/alg_idxfrng$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_idxflar$(OBJEXT):	\
	$(THIS)categories/sit_freelar$(OBJEXT)	\
	$(THIS)categories/sit_idxflc$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_idxflc$(OBJEXT):	\
	$(THIS)categories/sit_freelc$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_idxfmod$(OBJEXT):	\
	$(THIS)categories/sit_freemod$(OBJEXT)	\
	$(THIS)categories/sit_idxflc$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_intdom$(OBJEXT):	\
	$(THIS)categories/sit_comring$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_integer$(OBJEXT):	\
	$(THIS)categories/alg_modcmp$(OBJEXT)	\
	$(THIS)categories/sit_char0$(OBJEXT)	\
	$(THIS)categories/sit_spzble$(OBJEXT)	\
	$(THIS)numbers/sit_primgen$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_intgmp$(OBJEXT):	\
	$(THIS)categories/sit_integer$(OBJEXT)	\
	$(THIS)univpoly/gcd/sit_gcdint$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_linarit$(OBJEXT):	\
	$(THIS)categories/sit_ring$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_module$(OBJEXT):	\
	$(THIS)categories/sit_ring$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_monoid$(OBJEXT):	\
	$(THIS)categories/sit_basic$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_ncid$(OBJEXT):	\
	$(THIS)categories/sit_ring$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_pable$(OBJEXT):	\
	$(THIS)extree/parser/sit_infexpr$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_prfcat0$(OBJEXT):	\
	$(THIS)categories/alg_ffield$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_ptools$(OBJEXT):	\
	$(THIS)categories/sit_intdom$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_qring$(OBJEXT):	\
	$(THIS)categories/sit_char0$(OBJEXT)	\
	$(THIS)categories/sit_field$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_ring$(OBJEXT):	\
	$(THIS)categories/sit_abgroup$(OBJEXT)	\
	$(THIS)categories/sit_monoid$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_spf0$(OBJEXT):	\
	$(THIS)categories/sit_spfcat0$(OBJEXT)	\
	$(THIS)numbers/sit_primgen$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_spfcat0$(OBJEXT):	\
	$(THIS)categories/sit_prfcat0$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)categories/sit_spzble$(OBJEXT):	\
	$(THIS)categories/sit_comring$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/operators/sit_OPand$(OBJEXT):	\
	$(THIS)categories/sit_gexpcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/operators/sit_OPassgn$(OBJEXT):	\
	$(THIS)extree/sit_optools$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/operators/sit_OPbigO$(OBJEXT):	\
	$(THIS)categories/sit_gexpcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/operators/sit_OPcase$(OBJEXT):	\
	$(THIS)extree/operators/sit_OPif$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/operators/sit_OPcplex$(OBJEXT):	\
	$(THIS)categories/sit_gexpcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/operators/sit_OPequal$(OBJEXT):	\
	$(THIS)categories/sit_gexpcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/operators/sit_OPexpt$(OBJEXT):	\
	$(THIS)extree/sit_optools$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/operators/sit_OPfact$(OBJEXT):	\
	$(THIS)categories/sit_gexpcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/operators/sit_OPif$(OBJEXT):	\
	$(THIS)categories/sit_gexpcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/operators/sit_OPless$(OBJEXT):	\
	$(THIS)extree/sit_extree$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/operators/sit_OPlist$(OBJEXT):	\
	$(THIS)extree/sit_optools$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/operators/sit_OPllist$(OBJEXT):	\
	$(THIS)categories/sit_gexpcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/operators/sit_OPmatrx$(OBJEXT):	\
	$(THIS)categories/sit_gexpcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/operators/sit_OPminus$(OBJEXT):	\
	$(THIS)extree/sit_optools$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/operators/sit_OPmore$(OBJEXT):	\
	$(THIS)extree/sit_extree$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/operators/sit_OPnoteq$(OBJEXT):	\
	$(THIS)categories/sit_gexpcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/operators/sit_OPplus$(OBJEXT):	\
	$(THIS)extree/operators/sit_OPminus$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/operators/sit_OPprefx$(OBJEXT):	\
	$(THIS)extree/sit_optools$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/operators/sit_OPquot$(OBJEXT):	\
	$(THIS)extree/sit_optools$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/operators/sit_OPsubsc$(OBJEXT):	\
	$(THIS)categories/sit_gexpcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/operators/sit_OPtimes$(OBJEXT):	\
	$(THIS)extree/sit_optools$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/operators/sit_OPvect$(OBJEXT):	\
	$(THIS)categories/sit_gexpcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/parser/sit_infexpr$(OBJEXT):	\
	$(THIS)extree/operators/sit_OPlist$(OBJEXT)	\
	$(THIS)extree/parser/sit_parser$(OBJEXT)	\
	$(THIS)extree/parser/sit_scanner$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/parser/sit_lspexpr$(OBJEXT):	\
	$(THIS)extree/operators/sit_OPllist$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/parser/sit_maple$(OBJEXT):	\
	$(THIS)categories/sit_gexpcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/parser/sit_parser$(OBJEXT):	\
	$(THIS)extree/sit_extree$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/parser/sit_scanner$(OBJEXT):	\
	$(THIS)extree/operators/sit_OPassgn$(OBJEXT)	\
	$(THIS)extree/operators/sit_OPless$(OBJEXT)	\
	$(THIS)extree/operators/sit_OPmore$(OBJEXT)	\
	$(THIS)extree/parser/sit_token$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/parser/sit_token$(OBJEXT):	\
	$(THIS)extree/operators/sit_OPexpt$(OBJEXT)	\
	$(THIS)extree/operators/sit_OPplus$(OBJEXT)	\
	$(THIS)extree/operators/sit_OPprefx$(OBJEXT)	\
	$(THIS)extree/operators/sit_OPquot$(OBJEXT)	\
	$(THIS)extree/operators/sit_OPtimes$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/sit_extree$(OBJEXT):	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)extree/sit_optools$(OBJEXT):	\
	$(THIS)extree/sit_extree$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)ffield/alg_pf2$(OBJEXT):	\
	$(THIS)ffield/sit_sprfcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)ffield/sit_prfcat$(OBJEXT):	\
	$(THIS)polyfactorp/sit_upfactp$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)ffield/sit_spf$(OBJEXT):	\
	$(THIS)ffield/sit_sprfcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)ffield/sit_sprfcat$(OBJEXT):	\
	$(THIS)ffield/sit_prfcat$(OBJEXT)	\
	$(THIS)ffield/sit_sprfgcd$(OBJEXT)	\
	$(THIS)ffield/sit_sprfmat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)ffield/sit_sprfgcd$(OBJEXT):	\
	$(THIS)univpoly/categories/sit_upolc0$(OBJEXT)	\
	$(THIS)univpoly/gcd/sit_modpgcd$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)ffield/sit_sprfmat$(OBJEXT):	\
	$(THIS)categories/sit_spfcat0$(OBJEXT)	\
	$(THIS)mat/modular/sit_modpoge$(OBJEXT)	\
	$(THIS)mat/sit_matcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)ffield/sit_zpf$(OBJEXT):	\
	$(THIS)ffield/sit_sprfcat$(OBJEXT)	\
	$(THIS)numbers/sit_prmroot$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)fraction/sit_lcqotct$(OBJEXT):	\
	$(THIS)fraction/sit_qotbyc0$(OBJEXT)	\
	$(THIS)fraction/sit_qotfct0$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)fraction/sit_matquot$(OBJEXT):	\
	$(THIS)fraction/sit_vecquot$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)fraction/sit_qotbyc0$(OBJEXT):	\
	$(THIS)fraction/sit_quotcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)fraction/sit_qotbyct$(OBJEXT):	\
	$(THIS)fraction/sit_matquot$(OBJEXT)	\
	$(THIS)fraction/sit_uflgqot$(OBJEXT)	\
	$(THIS)univpoly/sit_dup$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)fraction/sit_qotfcat$(OBJEXT):	\
	$(THIS)fraction/sit_matquot$(OBJEXT)	\
	$(THIS)fraction/sit_uflgqot$(OBJEXT)	\
	$(THIS)univpoly/sit_dup$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)fraction/sit_qotfct0$(OBJEXT):	\
	$(THIS)fraction/sit_quotcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)fraction/sit_qotient$(OBJEXT):	\
	$(THIS)fraction/sit_qotfcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)fraction/sit_quotby$(OBJEXT):	\
	$(THIS)fraction/sit_qotbyct$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)fraction/sit_quotcat$(OBJEXT):	\
	$(THIS)categories/sit_algebra$(OBJEXT)	\
	$(THIS)categories/sit_diffext$(OBJEXT)	\
	$(THIS)univpoly/categories/sit_fring$(OBJEXT)	\
	$(THIS)univpoly/categories/sit_ugring$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)fraction/sit_uflgqot$(OBJEXT):	\
	$(THIS)fraction/sit_lcqotct$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)fraction/sit_vecquot$(OBJEXT):	\
	$(THIS)fraction/sit_lcqotct$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)mat/gauss/sit_dfge$(OBJEXT):	\
	$(THIS)basic/sit_permut$(OBJEXT)	\
	$(THIS)mat/gauss/sit_linelim$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)mat/gauss/sit_ff2ge$(OBJEXT):	\
	$(THIS)basic/sit_permut$(OBJEXT)	\
	$(THIS)mat/gauss/sit_linelim$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)mat/gauss/sit_ffge$(OBJEXT):	\
	$(THIS)basic/sit_permut$(OBJEXT)	\
	$(THIS)mat/gauss/sit_linelim$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)mat/gauss/sit_hermge$(OBJEXT):	\
	$(THIS)basic/sit_permut$(OBJEXT)	\
	$(THIS)categories/sit_euclid$(OBJEXT)	\
	$(THIS)mat/gauss/sit_linelim$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)mat/gauss/sit_linelim$(OBJEXT):	\
	$(THIS)mat/sit_matcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)mat/gauss/sit_oge$(OBJEXT):	\
	$(THIS)basic/sit_permut$(OBJEXT)	\
	$(THIS)categories/sit_field$(OBJEXT)	\
	$(THIS)mat/gauss/sit_linelim$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)mat/linalg/sit_bsolve$(OBJEXT):	\
	$(THIS)basic/sit_permut$(OBJEXT)	\
	$(THIS)categories/sit_field$(OBJEXT)	\
	$(THIS)mat/sit_matcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)mat/linalg/sit_laring$(OBJEXT):	\
	$(THIS)mat/sit_matcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)mat/linalg/sit_linalg$(OBJEXT):	\
	$(THIS)mat/gauss/sit_dfge$(OBJEXT)	\
	$(THIS)mat/gauss/sit_ff2ge$(OBJEXT)	\
	$(THIS)mat/gauss/sit_oge$(OBJEXT)	\
	$(THIS)mat/linalg/sit_bsolve$(OBJEXT)	\
	$(THIS)mat/linalg/sit_laring$(OBJEXT)	\
	$(THIS)mat/linalg/sit_overdet$(OBJEXT)	\
	$(THIS)mat/modular/compbug/sit_speclin$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)mat/linalg/sit_overdet$(OBJEXT):	\
	$(THIS)categories/sit_gcd$(OBJEXT)	\
	$(THIS)mat/sit_matcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)mat/linalg2/alg_ffupla$(OBJEXT):	\
	$(THIS)mat/linalg2/sit_popov$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)mat/linalg2/sit_popov$(OBJEXT):	\
	$(THIS)univpoly/categories/sit_upolc0$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)mat/linalg2/sit_upcrtla$(OBJEXT):	\
	$(THIS)univpoly/categories/sit_upolc0$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)mat/linalg3/sit_hensela$(OBJEXT):	\
	$(THIS)fraction/sit_qotient$(OBJEXT)	\
	$(THIS)mat/linalg2/sit_upcrtla$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)mat/modular/compbug/sit_speclin$(OBJEXT):	\
	$(THIS)categories/sit_spf0$(OBJEXT)	\
	$(THIS)categories/sit_spzble$(OBJEXT)	\
	$(THIS)mat/modular/sit_modpoge$(OBJEXT)	\
	$(THIS)mat/sit_matcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)mat/modular/sit_modpoge$(OBJEXT):	\
	$(THIS)numbers/sit_primgen$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)mat/modular/sit_zcrtla$(OBJEXT):	\
	$(THIS)categories/sit_integer$(OBJEXT)	\
	$(THIS)univpoly/gcd/sit_gcdintg$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)mat/sit_dnsemat$(OBJEXT):	\
	$(THIS)mat/sit_matcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)mat/sit_matcat$(OBJEXT):	\
	$(THIS)categories/sit_difring$(OBJEXT)	\
	$(THIS)categories/sit_linarit$(OBJEXT)	\
	$(THIS)extree/operators/sit_OPmatrx$(OBJEXT)	\
	$(THIS)mat/sit_vector$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)mat/sit_vector$(OBJEXT):	\
	$(THIS)extree/operators/sit_OPvect$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/exponent/sm_dirprod$(OBJEXT):	\
	$(THIS)multpoly/exponent/sm_dirprodc$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/exponent/sm_dirprodc$(OBJEXT):	\
	$(THIS)categories/sit_gexpcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/exponent/sm_expocat$(OBJEXT):	\
	$(THIS)categories/sit_gexpcat$(OBJEXT)	\
	$(THIS)multpoly/exponent/sm_vt$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/exponent/sm_fvt$(OBJEXT):	\
	$(THIS)categories/sit_gexpcat$(OBJEXT)	\
	$(THIS)multpoly/exponent/sm_vt$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/exponent/sm_listovar$(OBJEXT):	\
	$(THIS)multpoly/exponent/sm_fvt$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/exponent/sm_midl$(OBJEXT):	\
	$(THIS)multpoly/exponent/sm_midrl$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/exponent/sm_midrl$(OBJEXT):	\
	$(THIS)multpoly/exponent/sm_dirprod$(OBJEXT)	\
	$(THIS)multpoly/exponent/sm_mievc$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/exponent/sm_mievc$(OBJEXT):	\
	$(THIS)multpoly/exponent/sm_expocat$(OBJEXT)	\
	$(THIS)multpoly/exponent/sm_fvt$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/exponent/sm_milex$(OBJEXT):	\
	$(THIS)multpoly/exponent/sm_midrl$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/exponent/sm_osymbol$(OBJEXT):	\
	$(THIS)multpoly/exponent/sm_vt$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/exponent/sm_tuplovar$(OBJEXT):	\
	$(THIS)multpoly/exponent/sm_listovar$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/exponent/sm_vt$(OBJEXT):	\
	$(THIS)categories/sit_basic$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/exponent/sm_zevc$(OBJEXT):	\
	$(THIS)multpoly/exponent/sm_dirprodc$(OBJEXT)	\
	$(THIS)multpoly/exponent/sm_expocat$(OBJEXT)	\
	$(THIS)multpoly/exponent/sm_fvt$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/multpolycat/alg_defgcd$(OBJEXT):	\
	$(THIS)multpoly/multpolycat/sm_polring0$(OBJEXT)	\
	$(THIS)univpoly/sit_dup$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/multpolycat/alg_poltype$(OBJEXT):	\
	$(THIS)multpoly/multpolycat/alg_stdfrng$(OBJEXT)	\
	$(THIS)univpoly/alg_sup$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/multpolycat/alg_stdfrng$(OBJEXT):	\
	$(THIS)categories/alg_frering$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/multpolycat/sm_famr0$(OBJEXT):	\
	$(THIS)multpoly/exponent/sm_expocat$(OBJEXT)	\
	$(THIS)multpoly/multpolycat/sm_polring0$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/multpolycat/sm_polring$(OBJEXT):	\
	$(THIS)multpoly/multpolycat/alg_defgcd$(OBJEXT)	\
	$(THIS)multpoly/multpolycat/sm_famr0$(OBJEXT)	\
	$(THIS)univpoly/gcd/sit_gcdintg$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/multpolycat/sm_polring0$(OBJEXT):	\
	$(THIS)categories/sit_freealg$(OBJEXT)	\
	$(THIS)multpoly/multpolycat/alg_poltype$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/multpolycat/sm_rmpcat0$(OBJEXT):	\
	$(THIS)multpoly/multpolycat/sm_polring$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/multpolydata/sm_delist$(OBJEXT):	\
	$(THIS)categories/sit_basic$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/multpolydom/alg_smp$(OBJEXT):	\
	$(THIS)multpoly/multpolycat/sm_rmpcat0$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/multpolydom/sm_dmp0$(OBJEXT):	\
	$(THIS)categories/sit_gexpcat$(OBJEXT)	\
	$(THIS)categories/sit_idxfmod$(OBJEXT)	\
	$(THIS)multpoly/multpolydata/sm_delist$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/multpolydom/sm_dmp1$(OBJEXT):	\
	$(THIS)multpoly/multpolycat/sm_famr0$(OBJEXT)	\
	$(THIS)multpoly/multpolydom/sm_dmp0$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/multpolydom/sm_rmp$(OBJEXT):	\
	$(THIS)multpoly/multpolycat/sm_rmpcat0$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/multpolydom/sm_rmpz$(OBJEXT):	\
	$(THIS)multpoly/exponent/sm_osymbol$(OBJEXT)	\
	$(THIS)multpoly/multpolydom/sm_rmp$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/multpolydom/sm_rmpzx$(OBJEXT):	\
	$(THIS)multpoly/multpolydom/sm_rmp$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/multpolypkg/alg_ZpUVres$(OBJEXT):	\
	$(THIS)ffield/sit_spf$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/multpolypkg/alg_bivarpk$(OBJEXT):	\
	$(THIS)ffield/sit_spf$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/multpolypkg/alg_mresbiv$(OBJEXT):	\
	$(THIS)multpoly/multpolypkg/alg_ZpUVres$(OBJEXT)	\
	$(THIS)multpoly/multpolypkg/alg_bivarpk$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/multpolytest/alg_bivtst1$(OBJEXT):	\
	$(THIS)multpoly/multpolypkg/alg_bivarpk$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/multpolytest/alg_bivtst2$(OBJEXT):	\
	$(THIS)multpoly/multpolypkg/alg_mresbiv$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/multpolytest/alg_bivtst3$(OBJEXT):	\
	$(THIS)ffield/sit_spf$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/multpolytest/sm_dmp0pkgt$(OBJEXT):	\
	$(THIS)categories/sit_gexpcat$(OBJEXT)	\
	$(THIS)categories/sit_idxfmod$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/multpolytest/sm_exppkgt$(OBJEXT):	\
	$(THIS)multpoly/exponent/sm_expocat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)multpoly/multpolytest/sm_pr0pkgt$(OBJEXT):	\
	$(THIS)multpoly/multpolycat/sm_polring0$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)numbers/sit_primes$(OBJEXT):	\
	$(THIS)numbers/sit_prmtabl$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)numbers/sit_primgen$(OBJEXT):	\
	$(THIS)numbers/sit_primes$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)numbers/sit_prmroot$(OBJEXT):	\
	$(THIS)numbers/sit_primgen$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)numbers/sit_prmtabl$(OBJEXT):	\
	$(THIS)categories/sit_gexpcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)polyfactor0/sit_fhensel$(OBJEXT):	\
	$(THIS)categories/sit_integer$(OBJEXT)	\
	$(THIS)univpoly/categories/sit_upolc0$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)polyfactor0/sit_zfactor$(OBJEXT):	\
	$(THIS)ffield/sit_zpf$(OBJEXT)	\
	$(THIS)polyfactor0/sit_fhensel$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)polyfactor0/sit_zfring$(OBJEXT):	\
	$(THIS)univpoly/categories/sit_fring$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)polyfactor0/sit_zfringg$(OBJEXT):	\
	$(THIS)univpoly/categories/sit_fring$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)polyfactorp/sit_upfactp$(OBJEXT):	\
	$(THIS)algext/sit_upmod$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)series/alg_serpoly$(OBJEXT):	\
	$(THIS)series/sit_sercat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)series/compbug/sit_duts$(OBJEXT):	\
	$(THIS)extree/operators/sit_OPbigO$(OBJEXT)	\
	$(THIS)series/sit_sercat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)series/sit_seqence$(OBJEXT):	\
	$(THIS)mat/sit_vector$(OBJEXT)	\
	$(THIS)univpoly/categories/sit_ufalg$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)series/sit_sercat$(OBJEXT):	\
	$(THIS)fraction/sit_qotient$(OBJEXT)	\
	$(THIS)series/sit_seqence$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/alg_sup$(OBJEXT):	\
	$(THIS)univpoly/alg_sup1$(OBJEXT)	\
	$(THIS)univpoly/sit_spread$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/alg_sup0$(OBJEXT):	\
	$(THIS)categories/sit_idxfmod$(OBJEXT)	\
	$(THIS)categories/sit_integer$(OBJEXT)	\
	$(THIS)categories/sit_prfcat0$(OBJEXT)	\
	$(THIS)multpoly/multpolydata/sm_delist$(OBJEXT)	\
	$(THIS)univpoly/categories/sit_umonom$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/alg_sup1$(OBJEXT):	\
	$(THIS)univpoly/alg_sup0$(OBJEXT)	\
	$(THIS)univpoly/categories/sit_upolalg$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/alg_unitool$(OBJEXT):	\
	$(THIS)univpoly/categories/sit_upolc0$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/alg_uprcr$(OBJEXT):	\
	$(THIS)univpoly/categories/sit_upolc0$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/categories/alg_chrem2$(OBJEXT):	\
	$(THIS)univpoly/categories/sit_upolc0$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/categories/alg_modgcdp$(OBJEXT):	\
	$(THIS)univpoly/categories/alg_chrem2$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/categories/alg_polydio$(OBJEXT):	\
	$(THIS)univpoly/categories/sit_resprs$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/categories/sit_fftring$(OBJEXT):	\
	$(THIS)univpoly/categories/sit_upolc0$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/categories/sit_fring$(OBJEXT):	\
	$(THIS)univpoly/categories/sit_zring$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/categories/sit_froot$(OBJEXT):	\
	$(THIS)categories/sit_field$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/categories/sit_resprs$(OBJEXT):	\
	$(THIS)univpoly/categories/sit_upolc0$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/categories/sit_sqfree$(OBJEXT):	\
	$(THIS)univpoly/categories/sit_upolc0$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/categories/sit_ufalg$(OBJEXT):	\
	$(THIS)categories/sit_idxflar$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/categories/sit_uffalg$(OBJEXT):	\
	$(THIS)categories/alg_idxfrng$(OBJEXT)	\
	$(THIS)mat/sit_dnsemat$(OBJEXT)	\
	$(THIS)univpoly/categories/sit_ufalg$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/categories/sit_ugring$(OBJEXT):	\
	$(THIS)univpoly/categories/sit_upolc0$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/categories/sit_umonom$(OBJEXT):	\
	$(THIS)categories/sit_charp$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/categories/sit_upolalg$(OBJEXT):	\
	$(THIS)univpoly/categories/sit_uffalg$(OBJEXT)	\
	$(THIS)univpoly/categories/sit_umonom$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/categories/sit_upolc0$(OBJEXT):	\
	$(THIS)basic/sit_product$(OBJEXT)	\
	$(THIS)categories/alg_modcmp$(OBJEXT)	\
	$(THIS)categories/sit_dcmprng$(OBJEXT)	\
	$(THIS)categories/sit_idxfalg$(OBJEXT)	\
	$(THIS)mat/linalg/sit_linalg$(OBJEXT)	\
	$(THIS)univpoly/categories/sit_upolalg$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/categories/sit_zring$(OBJEXT):	\
	$(THIS)univpoly/categories/sit_froot$(OBJEXT)	\
	$(THIS)univpoly/categories/sit_upolc0$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/gcd/sit_gcdint$(OBJEXT):	\
	$(THIS)univpoly/categories/sit_resprs$(OBJEXT)	\
	$(THIS)univpoly/categories/sit_ugring$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/gcd/sit_gcdintg$(OBJEXT):	\
	$(THIS)univpoly/categories/sit_ugring$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/gcd/sit_heugcd$(OBJEXT):	\
	$(THIS)categories/sit_integer$(OBJEXT)	\
	$(THIS)univpoly/categories/sit_upolc0$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/gcd/sit_modgcd$(OBJEXT):	\
	$(THIS)categories/sit_integer$(OBJEXT)	\
	$(THIS)univpoly/categories/sit_upolc0$(OBJEXT)	\
	$(THIS)univpoly/gcd/sit_modpgcd$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/gcd/sit_modpgcd$(OBJEXT):	\
	$(THIS)numbers/sit_primgen$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/sit_dup$(OBJEXT):	\
	$(THIS)univpoly/alg_sup$(OBJEXT)	\
	$(THIS)univpoly/categories/sit_fftring$(OBJEXT)	\
	$(THIS)univpoly/sit_polkara$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/sit_polkara$(OBJEXT):	\
	$(THIS)categories/sit_gexpcat$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/sit_spread$(OBJEXT):	\
	$(THIS)univpoly/sit_upolc$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/sit_ufacpol$(OBJEXT):	\
	$(THIS)extree/operators/sit_OPfact$(OBJEXT)	\
	$(THIS)univpoly/sit_upolc$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/sit_upolc$(OBJEXT):	\
	$(THIS)mat/linalg2/alg_ffupla$(OBJEXT)	\
	$(THIS)univpoly/alg_uprcr$(OBJEXT)	\
	$(THIS)univpoly/categories/alg_modgcdp$(OBJEXT)	\
	$(THIS)univpoly/categories/sit_fring$(OBJEXT)	\
	$(THIS)univpoly/categories/sit_resprs$(OBJEXT)	\
	$(THIS)univpoly/categories/sit_sqfree$(OBJEXT)	\
	$(THIS)univpoly/categories/sit_ugring$(OBJEXT)	\
	$(THIS)util/alg_version$(OBJEXT)
$(THIS)univpoly/categories/sit_ufalg$(OBJEXT):	\
	$(THIS)categories/sit_integer$(OBJEXT)
$(THIS)univpoly/categories/sit_froot$(OBJEXT):	\
	$(THIS)categories/sit_integer$(OBJEXT)
$(THIS)multpoly/exponent/sm_expocat$(OBJEXT):	\
	$(THIS)categories/sit_integer$(OBJEXT)
$(THIS)basic/sit_product$(OBJEXT):	\
	$(THIS)categories/sit_integer$(OBJEXT)
$(THIS)categories/sit_fset$(OBJEXT):	\
	$(THIS)categories/sit_integer$(OBJEXT)
$(THIS)univpoly/categories/sit_umonom$(OBJEXT):	\
	$(THIS)categories/sit_integer$(OBJEXT)
$(THIS)univpoly/gcd/sit_gcdintg$(OBJEXT):	\
	$(THIS)categories/sit_intgmp$(OBJEXT)
$(THIS)univpoly/gcd/sit_gcdint$(OBJEXT):	\
	$(THIS)univpoly/gcd/sit_heugcd$(OBJEXT)	\
	$(THIS)univpoly/gcd/sit_modgcd$(OBJEXT)
$(THIS)polyfactor0/sit_zfring$(OBJEXT):	\
	$(THIS)polyfactor0/sit_zfactor$(OBJEXT)
$(THIS)polyfactor0/sit_zfringg$(OBJEXT):	\
	$(THIS)categories/sit_intgmp$(OBJEXT)	\
	$(THIS)polyfactor0/sit_zfactor$(OBJEXT)
$(THIS)util/alg_version$(OBJEXT):	\
	$(LIBDIR)/libaldor$(LIBEXT)
