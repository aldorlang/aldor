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

libaldor_ASOURCES := $(addprefix $(THIS), $(libaldor_ASOURCES))
libaldor_CSOURCES := $(addprefix $(THIS), $(libaldor_CSOURCES))

libaldor_AOBJECTS := $(libaldor_ASOURCES:.as=.ao)
libaldor_COBJECTS := $(libaldor_ASOURCES:.as=.o) $(libaldor_CSOURCES:.c=.o)


# C library
build/libaldor.a: $(libaldor_COBJECTS)
	$(AR) cr $@ $^

# Local aldor build rule
$(THIS)%.o: $(THIS)%.as build/aldor build/unicl $(aldor_HEADERS) build/include/aldor.as build/include/aldorio.as
	build/aldor $(AFLAGS) $<
	$(AR) cr build/libaldor.al $(@:.o=.ao)
	mv $(notdir $@) $@

# Clean
clean: clean-$(THIS)
clean-$(THIS):
	$(RM) $(libaldor_AOBJECTS)
	$(RM) $(libaldor_COBJECTS)
	$(RM) $(libaldor_ASOURCES:.as=.c)
	$(RM) build/libaldor.a build/libaldor.al

# Depend
$(THIS)arith/sal_arith.o:	\
	$(THIS)base/sal_copy.o
$(THIS)arith/sal_binpow.o:	\
	$(THIS)arith/sal_arith.o	\
	$(THIS)arith/sal_intcat.o
$(THIS)arith/sal_bool.o:	\
	$(THIS)base/sal_htype.o
$(THIS)arith/sal_bsearch.o:	\
	$(THIS)arith/sal_intcat.o
$(THIS)arith/sal_complex.o:	\
	$(THIS)arith/sal_fltcat.o	\
	$(THIS)arith/sal_lincomb.o
$(THIS)arith/sal_dfloat.o:	\
	$(THIS)arith/sal_sfloat.o
$(THIS)arith/sal_fltcat.o:	\
	$(THIS)arith/sal_int.o
$(THIS)arith/sal_ftools.o:	\
	$(THIS)arith/sal_fltcat.o
$(THIS)arith/sal_int.o:	\
	$(THIS)arith/sal_random.o	\
	$(THIS)arith/sal_binpow.o	\
	$(THIS)arith/sal_segment.o	\
	$(THIS)arith/sal_itools.o	\
	$(THIS)base/sal_byte.o
$(THIS)arith/sal_intcat.o:	\
	$(THIS)arith/sal_bool.o	\
	$(THIS)arith/sal_oarith.o	\
	$(THIS)base/sal_serial.o	\
	$(THIS)base/sal_itype.o	\
	$(THIS)base/sal_otype.o
$(THIS)arith/sal_itools.o:	\
	$(THIS)arith/sal_bsearch.o	\
	$(THIS)arith/sal_intcat.o	\
	$(THIS)base/sal_partial.o
$(THIS)arith/sal_lincomb.o:	\
	$(THIS)arith/sal_arith.o
$(THIS)arith/sal_mint.o:	\
	$(THIS)arith/sal_itools.o	\
	$(THIS)arith/sal_segment.o	\
	$(THIS)base/sal_byte.o
$(THIS)arith/sal_oarith.o:	\
	$(THIS)arith/sal_arith.o	\
	$(THIS)base/sal_torder.o
$(THIS)arith/sal_pointer.o:	\
	$(THIS)arith/sal_mint.o
$(THIS)arith/sal_random.o:	\
	$(THIS)arith/sal_mint.o	\
	$(THIS)base/sal_gener.o
$(THIS)arith/sal_segment.o:	\
	$(THIS)arith/sal_intcat.o	\
	$(THIS)base/sal_partial.o
$(THIS)arith/sal_sfloat.o:	\
	$(THIS)arith/sal_ftools.o
$(THIS)base/ald_pfunc.o:	\
	$(THIS)arith/sal_pointer.o
$(THIS)base/sal_base.o:	\
	$(THIS)lang/sal_lang.o
$(THIS)base/sal_bstream.o:	\
	$(THIS)base/sal_base.o
$(THIS)base/sal_byte.o:	\
	$(THIS)base/sal_htype.o	\
	$(THIS)base/sal_itype.o	\
	$(THIS)base/sal_otype.o	\
	$(THIS)base/sal_bstream.o	\
	$(THIS)base/sal_serial.o
$(THIS)base/sal_char.o:	\
	$(THIS)base/sal_htype.o	\
	$(THIS)base/sal_itype.o	\
	$(THIS)base/sal_otype.o	\
	$(THIS)base/sal_bstream.o	\
	$(THIS)base/sal_serial.o	\
	$(THIS)base/sal_byte.o	\
	$(THIS)base/sal_torder.o
$(THIS)base/sal_copy.o:	\
	$(THIS)base/sal_base.o
$(THIS)base/sal_gener.o:	\
	$(THIS)lang/sal_lang.o
$(THIS)base/sal_htype.o:	\
	$(THIS)base/sal_base.o
$(THIS)base/sal_itype.o:	\
	$(THIS)base/sal_tstream.o
$(THIS)base/sal_manip.o:	\
	$(THIS)base/sal_byte.o
$(THIS)base/sal_order.o:	\
	$(THIS)base/sal_base.o
$(THIS)base/sal_otype.o:	\
	$(THIS)base/sal_tstream.o
$(THIS)base/sal_partial.o:	\
	$(THIS)base/sal_char.o	\
	$(THIS)base/sal_gener.o	\
	$(THIS)base/sal_syntax.o
$(THIS)base/sal_serial.o:	\
	$(THIS)base/sal_bstream.o
$(THIS)base/sal_syntax.o:	\
	$(THIS)base/sal_base.o
$(THIS)base/sal_torder.o:	\
	$(THIS)base/sal_order.o
$(THIS)base/sal_tstream.o:	\
	$(THIS)base/sal_base.o
$(THIS)datastruc/ald_flags.o:	\
	$(THIS)datastruc/sal_barray.o
$(THIS)datastruc/ald_queue.o:	\
	$(THIS)datastruc/ald_symtab.o
$(THIS)datastruc/ald_symbol.o:	\
	$(THIS)datastruc/sal_hash.o
$(THIS)datastruc/ald_symtab.o:	\
	$(THIS)datastruc/ald_symbol.o
$(THIS)datastruc/sal_array.o:	\
	$(THIS)arith/sal_bsearch.o	\
	$(THIS)datastruc/sal_parray.o
$(THIS)datastruc/sal_barray.o:	\
	$(THIS)datastruc/sal_pkarray.o
$(THIS)datastruc/sal_bdata.o:	\
	$(THIS)arith/sal_mint.o	\
	$(THIS)datastruc/sal_data.o
$(THIS)datastruc/sal_bstruc.o:	\
	$(THIS)datastruc/sal_bdata.o	\
	$(THIS)datastruc/sal_fstruc.o
$(THIS)datastruc/sal_ckarray.o:	\
	$(THIS)datastruc/sal_array.o
$(THIS)datastruc/sal_cklist.o:	\
	$(THIS)datastruc/sal_list.o	\
	$(THIS)arith/sal_random.o
$(THIS)datastruc/sal_ckmembk.o:	\
	$(THIS)datastruc/sal_memblk.o
$(THIS)datastruc/sal_data.o:	\
	$(THIS)base/sal_char.o
$(THIS)datastruc/sal_ddata.o:	\
	$(THIS)datastruc/sal_bdata.o
$(THIS)datastruc/sal_fstruc.o:	\
	$(THIS)datastruc/sal_lstruc.o
$(THIS)datastruc/sal_hash.o:	\
	$(THIS)datastruc/sal_kntry.o	\
	$(THIS)datastruc/sal_table.o
$(THIS)datastruc/sal_kntry.o:	\
	$(THIS)base/sal_char.o	\
	$(THIS)base/sal_copy.o
$(THIS)datastruc/sal_list.o:	\
	$(THIS)base/sal_partial.o	\
	$(THIS)datastruc/sal_bstruc.o	\
	$(THIS)datastruc/sal_ddata.o
$(THIS)datastruc/sal_lstruc.o:	\
	$(THIS)base/sal_gener.o	\
	$(THIS)datastruc/sal_data.o
$(THIS)datastruc/sal_memblk.o:	\
	$(THIS)datastruc/sal_array.o	\
	$(THIS)datastruc/sal_barray.o
$(THIS)datastruc/sal_parray.o:	\
	$(THIS)arith/sal_random.o	\
	$(THIS)datastruc/sal_fstruc.o	\
	$(THIS)datastruc/sal_list.o
$(THIS)datastruc/sal_pkarray.o:	\
	$(THIS)datastruc/sal_parray.o
$(THIS)datastruc/sal_set.o:	\
	$(THIS)datastruc/ald_symtab.o
$(THIS)datastruc/sal_slist.o:	\
	$(THIS)arith/sal_random.o	\
	$(THIS)datastruc/sal_bstruc.o	\
	$(THIS)datastruc/sal_list.o
$(THIS)datastruc/sal_sortas.o:	\
	$(THIS)datastruc/sal_kntry.o	\
	$(THIS)datastruc/sal_sset.o	\
	$(THIS)datastruc/sal_table.o
$(THIS)datastruc/sal_sset.o:	\
	$(THIS)datastruc/sal_list.o	\
	$(THIS)datastruc/sal_ddata.o	\
	$(THIS)datastruc/sal_bstruc.o	\
	$(THIS)arith/sal_random.o
$(THIS)datastruc/sal_stream.o:	\
	$(THIS)arith/sal_pointer.o	\
	$(THIS)datastruc/sal_list.o	\
	$(THIS)datastruc/sal_lstruc.o	\
	$(THIS)datastruc/sal_parray.o	\
	$(THIS)datastruc/sal_string.o
$(THIS)datastruc/sal_string.o:	\
	$(THIS)datastruc/sal_array.o	\
	$(THIS)datastruc/sal_pkarray.o
$(THIS)datastruc/sal_table.o:	\
	$(THIS)datastruc/sal_string.o
$(THIS)gmp/sal_fltgmp.o:	\
	$(THIS)datastruc/sal_string.o	\
	$(THIS)arith/sal_dfloat.o	\
	$(THIS)gmp/sal_intgmp.o
$(THIS)gmp/sal_gmptls.o:	\
	$(THIS)datastruc/sal_parray.o
$(THIS)gmp/sal_intgmp.o:	\
	$(THIS)arith/sal_int.o	\
	$(THIS)gmp/sal_gmptls.o
$(THIS)util/ald_trace.o:	\
	$(THIS)datastruc/sal_string.o	\
	$(THIS)arith/sal_pointer.o
$(THIS)util/eio_rsto.o:	\
	$(THIS)base/sal_base.o
$(THIS)util/rtexns.o:	\
	$(THIS)base/sal_manip.o	\
	$(THIS)util/ald_trace.o
$(THIS)util/sal_agat.o:	\
	$(THIS)datastruc/sal_string.o	\
	$(THIS)arith/sal_dfloat.o
$(THIS)util/sal_cmdline.o:	\
	$(THIS)datastruc/sal_string.o	\
	$(THIS)arith/sal_pointer.o
$(THIS)util/sal_file.o:	\
	$(THIS)datastruc/sal_string.o
$(THIS)util/sal_timer.o:	\
	$(THIS)arith/sal_pointer.o
$(THIS)util/sal_version.o:	\
	$(THIS)datastruc/sal_string.o
