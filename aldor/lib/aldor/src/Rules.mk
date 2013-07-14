THIS := $(dir $(lastword $(MAKEFILE_LIST)))

libaldor_ASOURCES :=	\
	arith/sal_arith.as	\
	arith/sal_binpow.as	\
	arith/sal_bool.as	\
	arith/sal_bsearch.as	\
	arith/sal_complex.as	\
	arith/sal_dfloat.as	\
	arith/sal_fltcat.as	\
	arith/sal_ftools.as	\
	arith/sal_int.as	\
	arith/sal_intcat.as	\
	arith/sal_itools.as	\
	arith/sal_lincomb.as	\
	arith/sal_mint.as	\
	arith/sal_oarith.as	\
	arith/sal_pointer.as	\
	arith/sal_random.as	\
	arith/sal_segment.as	\
	arith/sal_sfloat.as	\
	base/ald_pfunc.as	\
	base/sal_base.as	\
	base/sal_bstream.as	\
	base/sal_byte.as	\
	base/sal_char.as	\
	base/sal_copy.as	\
	base/sal_gener.as	\
	base/sal_htype.as	\
	base/sal_itype.as	\
	base/sal_manip.as	\
	base/sal_order.as	\
	base/sal_otype.as	\
	base/sal_partial.as	\
	base/sal_serial.as	\
	base/sal_syntax.as	\
	base/sal_torder.as	\
	base/sal_tstream.as	\
	datastruc/ald_flags.as	\
	datastruc/ald_queue.as	\
	datastruc/ald_symbol.as	\
	datastruc/ald_symtab.as	\
	datastruc/sal_array.as	\
	datastruc/sal_barray.as	\
	datastruc/sal_bdata.as	\
	datastruc/sal_bstruc.as	\
	datastruc/sal_ckarray.as	\
	datastruc/sal_cklist.as	\
	datastruc/sal_ckmembk.as	\
	datastruc/sal_data.as	\
	datastruc/sal_ddata.as	\
	datastruc/sal_fstruc.as	\
	datastruc/sal_hash.as	\
	datastruc/sal_kntry.as	\
	datastruc/sal_list.as	\
	datastruc/sal_lstruc.as	\
	datastruc/sal_memblk.as	\
	datastruc/sal_parray.as	\
	datastruc/sal_pkarray.as	\
	datastruc/sal_set.as	\
	datastruc/sal_slist.as	\
	datastruc/sal_sortas.as	\
	datastruc/sal_sset.as	\
	datastruc/sal_stream.as	\
	datastruc/sal_string.as	\
	datastruc/sal_table.as	\
	gmp/sal_fltgmp.as	\
	gmp/sal_gmptls.as	\
	gmp/sal_intgmp.as	\
	lang/sal_lang.as	\
	util/ald_trace.as	\
	util/eio_rsto.as	\
	util/rtexns.as	\
	util/sal_agat.as	\
	util/sal_cmdline.as	\
	util/sal_file.as	\
	util/sal_timer.as	\
	util/sal_version.as

libaldor_CSOURCES =		\
	util/sal_util.c

$(eval $(call aldor-target,aldor))

$(LIBDIR)/aldor_%.ao: $(THIS)/aldor_%.as $(BINDIR)/aldor$(EXEEXT) $(LIBDIR)/libaldor$(LIBEXT)
	$(BINDIR)/aldor$(EXEEXT) -Y $(LIBDIR) -I $(INCDIR) -Fao=$@ $<


# Depend
$(THIS)arith/sal_arith$(OBJEXT):	\
	$(THIS)base/sal_copy$(OBJEXT)
$(THIS)arith/sal_binpow$(OBJEXT):	\
	$(THIS)arith/sal_arith$(OBJEXT)	\
	$(THIS)arith/sal_intcat$(OBJEXT)
$(THIS)arith/sal_bool$(OBJEXT):	\
	$(THIS)base/sal_htype$(OBJEXT)
$(THIS)arith/sal_bsearch$(OBJEXT):	\
	$(THIS)arith/sal_intcat$(OBJEXT)
$(THIS)arith/sal_complex$(OBJEXT):	\
	$(THIS)arith/sal_fltcat$(OBJEXT)	\
	$(THIS)arith/sal_lincomb$(OBJEXT)
$(THIS)arith/sal_dfloat$(OBJEXT):	\
	$(THIS)arith/sal_sfloat$(OBJEXT)
$(THIS)arith/sal_fltcat$(OBJEXT):	\
	$(THIS)arith/sal_int$(OBJEXT)
$(THIS)arith/sal_ftools$(OBJEXT):	\
	$(THIS)arith/sal_fltcat$(OBJEXT)
$(THIS)arith/sal_int$(OBJEXT):	\
	$(THIS)arith/sal_random$(OBJEXT)	\
	$(THIS)arith/sal_binpow$(OBJEXT)	\
	$(THIS)arith/sal_segment$(OBJEXT)	\
	$(THIS)arith/sal_itools$(OBJEXT)	\
	$(THIS)base/sal_byte$(OBJEXT)
$(THIS)arith/sal_intcat$(OBJEXT):	\
	$(THIS)arith/sal_bool$(OBJEXT)	\
	$(THIS)arith/sal_oarith$(OBJEXT)	\
	$(THIS)base/sal_serial$(OBJEXT)	\
	$(THIS)base/sal_itype$(OBJEXT)	\
	$(THIS)base/sal_otype$(OBJEXT)
$(THIS)arith/sal_itools$(OBJEXT):	\
	$(THIS)arith/sal_bsearch$(OBJEXT)	\
	$(THIS)arith/sal_intcat$(OBJEXT)	\
	$(THIS)base/sal_partial$(OBJEXT)
$(THIS)arith/sal_lincomb$(OBJEXT):	\
	$(THIS)arith/sal_arith$(OBJEXT)
$(THIS)arith/sal_mint$(OBJEXT):	\
	$(THIS)arith/sal_itools$(OBJEXT)	\
	$(THIS)arith/sal_segment$(OBJEXT)	\
	$(THIS)base/sal_byte$(OBJEXT)
$(THIS)arith/sal_oarith$(OBJEXT):	\
	$(THIS)arith/sal_arith$(OBJEXT)	\
	$(THIS)base/sal_torder$(OBJEXT)
$(THIS)arith/sal_pointer$(OBJEXT):	\
	$(THIS)arith/sal_mint$(OBJEXT)
$(THIS)arith/sal_random$(OBJEXT):	\
	$(THIS)arith/sal_mint$(OBJEXT)	\
	$(THIS)base/sal_gener$(OBJEXT)
$(THIS)arith/sal_segment$(OBJEXT):	\
	$(THIS)arith/sal_intcat$(OBJEXT)	\
	$(THIS)base/sal_partial$(OBJEXT)
$(THIS)arith/sal_sfloat$(OBJEXT):	\
	$(THIS)arith/sal_ftools$(OBJEXT)
$(THIS)base/ald_pfunc$(OBJEXT):	\
	$(THIS)arith/sal_pointer$(OBJEXT)
$(THIS)base/sal_base$(OBJEXT):	\
	$(THIS)lang/sal_lang$(OBJEXT)
$(THIS)base/sal_bstream$(OBJEXT):	\
	$(THIS)base/sal_base$(OBJEXT)
$(THIS)base/sal_byte$(OBJEXT):	\
	$(THIS)base/sal_htype$(OBJEXT)	\
	$(THIS)base/sal_itype$(OBJEXT)	\
	$(THIS)base/sal_otype$(OBJEXT)	\
	$(THIS)base/sal_bstream$(OBJEXT)	\
	$(THIS)base/sal_serial$(OBJEXT)
$(THIS)base/sal_char$(OBJEXT):	\
	$(THIS)base/sal_htype$(OBJEXT)	\
	$(THIS)base/sal_itype$(OBJEXT)	\
	$(THIS)base/sal_otype$(OBJEXT)	\
	$(THIS)base/sal_bstream$(OBJEXT)	\
	$(THIS)base/sal_serial$(OBJEXT)	\
	$(THIS)base/sal_byte$(OBJEXT)	\
	$(THIS)base/sal_torder$(OBJEXT)
$(THIS)base/sal_copy$(OBJEXT):	\
	$(THIS)base/sal_base$(OBJEXT)
$(THIS)base/sal_gener$(OBJEXT):	\
	$(THIS)lang/sal_lang$(OBJEXT)
$(THIS)base/sal_htype$(OBJEXT):	\
	$(THIS)base/sal_base$(OBJEXT)
$(THIS)base/sal_itype$(OBJEXT):	\
	$(THIS)base/sal_tstream$(OBJEXT)
$(THIS)base/sal_manip$(OBJEXT):	\
	$(THIS)base/sal_byte$(OBJEXT)
$(THIS)base/sal_order$(OBJEXT):	\
	$(THIS)base/sal_base$(OBJEXT)
$(THIS)base/sal_otype$(OBJEXT):	\
	$(THIS)base/sal_tstream$(OBJEXT)
$(THIS)base/sal_partial$(OBJEXT):	\
	$(THIS)base/sal_char$(OBJEXT)	\
	$(THIS)base/sal_gener$(OBJEXT)	\
	$(THIS)base/sal_syntax$(OBJEXT)
$(THIS)base/sal_serial$(OBJEXT):	\
	$(THIS)base/sal_bstream$(OBJEXT)
$(THIS)base/sal_syntax$(OBJEXT):	\
	$(THIS)base/sal_base$(OBJEXT)
$(THIS)base/sal_torder$(OBJEXT):	\
	$(THIS)base/sal_order$(OBJEXT)
$(THIS)base/sal_tstream$(OBJEXT):	\
	$(THIS)base/sal_base$(OBJEXT)
$(THIS)datastruc/ald_flags$(OBJEXT):	\
	$(THIS)datastruc/sal_barray$(OBJEXT)
$(THIS)datastruc/ald_queue$(OBJEXT):	\
	$(THIS)datastruc/ald_symtab$(OBJEXT)
$(THIS)datastruc/ald_symbol$(OBJEXT):	\
	$(THIS)datastruc/sal_hash$(OBJEXT)
$(THIS)datastruc/ald_symtab$(OBJEXT):	\
	$(THIS)datastruc/ald_symbol$(OBJEXT)
$(THIS)datastruc/sal_array$(OBJEXT):	\
	$(THIS)arith/sal_bsearch$(OBJEXT)	\
	$(THIS)datastruc/sal_parray$(OBJEXT)
$(THIS)datastruc/sal_barray$(OBJEXT):	\
	$(THIS)datastruc/sal_pkarray$(OBJEXT)
$(THIS)datastruc/sal_bdata$(OBJEXT):	\
	$(THIS)arith/sal_mint$(OBJEXT)	\
	$(THIS)datastruc/sal_data$(OBJEXT)
$(THIS)datastruc/sal_bstruc$(OBJEXT):	\
	$(THIS)datastruc/sal_bdata$(OBJEXT)	\
	$(THIS)datastruc/sal_fstruc$(OBJEXT)
$(THIS)datastruc/sal_ckarray$(OBJEXT):	\
	$(THIS)datastruc/sal_array$(OBJEXT)
$(THIS)datastruc/sal_cklist$(OBJEXT):	\
	$(THIS)datastruc/sal_list$(OBJEXT)	\
	$(THIS)arith/sal_random$(OBJEXT)
$(THIS)datastruc/sal_ckmembk$(OBJEXT):	\
	$(THIS)datastruc/sal_memblk$(OBJEXT)
$(THIS)datastruc/sal_data$(OBJEXT):	\
	$(THIS)base/sal_char$(OBJEXT)
$(THIS)datastruc/sal_ddata$(OBJEXT):	\
	$(THIS)datastruc/sal_bdata$(OBJEXT)
$(THIS)datastruc/sal_fstruc$(OBJEXT):	\
	$(THIS)datastruc/sal_lstruc$(OBJEXT)
$(THIS)datastruc/sal_hash$(OBJEXT):	\
	$(THIS)datastruc/sal_kntry$(OBJEXT)	\
	$(THIS)datastruc/sal_table$(OBJEXT)
$(THIS)datastruc/sal_kntry$(OBJEXT):	\
	$(THIS)base/sal_char$(OBJEXT)	\
	$(THIS)base/sal_copy$(OBJEXT)
$(THIS)datastruc/sal_list$(OBJEXT):	\
	$(THIS)base/sal_partial$(OBJEXT)	\
	$(THIS)datastruc/sal_bstruc$(OBJEXT)	\
	$(THIS)datastruc/sal_ddata$(OBJEXT)
$(THIS)datastruc/sal_lstruc$(OBJEXT):	\
	$(THIS)base/sal_gener$(OBJEXT)	\
	$(THIS)datastruc/sal_data$(OBJEXT)
$(THIS)datastruc/sal_memblk$(OBJEXT):	\
	$(THIS)datastruc/sal_array$(OBJEXT)	\
	$(THIS)datastruc/sal_barray$(OBJEXT)
$(THIS)datastruc/sal_parray$(OBJEXT):	\
	$(THIS)arith/sal_random$(OBJEXT)	\
	$(THIS)datastruc/sal_fstruc$(OBJEXT)	\
	$(THIS)datastruc/sal_list$(OBJEXT)
$(THIS)datastruc/sal_pkarray$(OBJEXT):	\
	$(THIS)datastruc/sal_parray$(OBJEXT)
$(THIS)datastruc/sal_set$(OBJEXT):	\
	$(THIS)datastruc/ald_symtab$(OBJEXT)
$(THIS)datastruc/sal_slist$(OBJEXT):	\
	$(THIS)arith/sal_random$(OBJEXT)	\
	$(THIS)datastruc/sal_bstruc$(OBJEXT)	\
	$(THIS)datastruc/sal_list$(OBJEXT)
$(THIS)datastruc/sal_sortas$(OBJEXT):	\
	$(THIS)datastruc/sal_kntry$(OBJEXT)	\
	$(THIS)datastruc/sal_sset$(OBJEXT)	\
	$(THIS)datastruc/sal_table$(OBJEXT)
$(THIS)datastruc/sal_sset$(OBJEXT):	\
	$(THIS)datastruc/sal_list$(OBJEXT)	\
	$(THIS)datastruc/sal_ddata$(OBJEXT)	\
	$(THIS)datastruc/sal_bstruc$(OBJEXT)	\
	$(THIS)arith/sal_random$(OBJEXT)
$(THIS)datastruc/sal_stream$(OBJEXT):	\
	$(THIS)arith/sal_pointer$(OBJEXT)	\
	$(THIS)datastruc/sal_list$(OBJEXT)	\
	$(THIS)datastruc/sal_lstruc$(OBJEXT)	\
	$(THIS)datastruc/sal_parray$(OBJEXT)	\
	$(THIS)datastruc/sal_string$(OBJEXT)
$(THIS)datastruc/sal_string$(OBJEXT):	\
	$(THIS)datastruc/sal_array$(OBJEXT)	\
	$(THIS)datastruc/sal_pkarray$(OBJEXT)
$(THIS)datastruc/sal_table$(OBJEXT):	\
	$(THIS)datastruc/sal_string$(OBJEXT)
$(THIS)gmp/sal_fltgmp$(OBJEXT):	\
	$(THIS)datastruc/sal_string$(OBJEXT)	\
	$(THIS)arith/sal_dfloat$(OBJEXT)	\
	$(THIS)gmp/sal_intgmp$(OBJEXT)
$(THIS)gmp/sal_gmptls$(OBJEXT):	\
	$(THIS)datastruc/sal_parray$(OBJEXT)
$(THIS)gmp/sal_intgmp$(OBJEXT):	\
	$(THIS)arith/sal_int$(OBJEXT)	\
	$(THIS)gmp/sal_gmptls$(OBJEXT)
$(THIS)util/ald_trace$(OBJEXT):	\
	$(THIS)datastruc/sal_string$(OBJEXT)	\
	$(THIS)arith/sal_pointer$(OBJEXT)
$(THIS)util/eio_rsto$(OBJEXT):	\
	$(THIS)base/sal_base$(OBJEXT)
$(THIS)util/rtexns$(OBJEXT):	\
	$(THIS)base/sal_manip$(OBJEXT)	\
	$(THIS)util/ald_trace$(OBJEXT)
$(THIS)util/sal_agat$(OBJEXT):	\
	$(THIS)datastruc/sal_string$(OBJEXT)	\
	$(THIS)arith/sal_dfloat$(OBJEXT)
$(THIS)util/sal_cmdline$(OBJEXT):	\
	$(THIS)datastruc/sal_string$(OBJEXT)	\
	$(THIS)arith/sal_pointer$(OBJEXT)
$(THIS)util/sal_file$(OBJEXT):	\
	$(THIS)datastruc/sal_string$(OBJEXT)
$(THIS)util/sal_timer$(OBJEXT):	\
	$(THIS)arith/sal_pointer$(OBJEXT)
$(THIS)util/sal_version$(OBJEXT):	\
	$(THIS)datastruc/sal_string$(OBJEXT)
