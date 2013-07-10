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
$(THIS)%.c: $(THIS)%.as build/aldor build/include/aldor.conf build/include/aldor.as build/include/aldorio.as
	build/aldor $(AFLAGS) $<
	$(AR) cr build/libaldor.al $(@:.c=.ao)

# Clean
clean: clean-$(THIS)
clean-$(THIS):
	$(RM) $(libaldor_AOBJECTS)
	$(RM) $(libaldor_COBJECTS)
	$(RM) $(libaldor_ASOURCES:.as=.c)
	$(RM) build/libaldor.a build/libaldor.al

# Depend
$(THIS)arith/sal_arith.c:	\
	$(THIS)base/sal_copy.c
$(THIS)arith/sal_binpow.c:	\
	$(THIS)arith/sal_arith.c	\
	$(THIS)arith/sal_intcat.c
$(THIS)arith/sal_bool.c:	\
	$(THIS)base/sal_htype.c
$(THIS)arith/sal_bsearch.c:	\
	$(THIS)arith/sal_intcat.c
$(THIS)arith/sal_complex.c:	\
	$(THIS)arith/sal_fltcat.c	\
	$(THIS)arith/sal_lincomb.c
$(THIS)arith/sal_dfloat.c:	\
	$(THIS)arith/sal_sfloat.c
$(THIS)arith/sal_fltcat.c:	\
	$(THIS)arith/sal_int.c
$(THIS)arith/sal_ftools.c:	\
	$(THIS)arith/sal_fltcat.c
$(THIS)arith/sal_int.c:	\
	$(THIS)arith/sal_random.c	\
	$(THIS)arith/sal_binpow.c	\
	$(THIS)arith/sal_segment.c	\
	$(THIS)arith/sal_itools.c	\
	$(THIS)base/sal_byte.c
$(THIS)arith/sal_intcat.c:	\
	$(THIS)arith/sal_bool.c	\
	$(THIS)arith/sal_oarith.c	\
	$(THIS)base/sal_serial.c	\
	$(THIS)base/sal_itype.c	\
	$(THIS)base/sal_otype.c
$(THIS)arith/sal_itools.c:	\
	$(THIS)arith/sal_bsearch.c	\
	$(THIS)arith/sal_intcat.c	\
	$(THIS)base/sal_partial.c
$(THIS)arith/sal_lincomb.c:	\
	$(THIS)arith/sal_arith.c
$(THIS)arith/sal_mint.c:	\
	$(THIS)arith/sal_itools.c	\
	$(THIS)arith/sal_segment.c	\
	$(THIS)base/sal_byte.c
$(THIS)arith/sal_oarith.c:	\
	$(THIS)arith/sal_arith.c	\
	$(THIS)base/sal_torder.c
$(THIS)arith/sal_pointer.c:	\
	$(THIS)arith/sal_mint.c
$(THIS)arith/sal_random.c:	\
	$(THIS)arith/sal_mint.c	\
	$(THIS)base/sal_gener.c
$(THIS)arith/sal_segment.c:	\
	$(THIS)arith/sal_intcat.c	\
	$(THIS)base/sal_partial.c
$(THIS)arith/sal_sfloat.c:	\
	$(THIS)arith/sal_ftools.c
$(THIS)base/ald_pfunc.c:	\
	$(THIS)arith/sal_pointer.c
$(THIS)base/sal_base.c:	\
	$(THIS)lang/sal_lang.c
$(THIS)base/sal_bstream.c:	\
	$(THIS)base/sal_base.c
$(THIS)base/sal_byte.c:	\
	$(THIS)base/sal_htype.c	\
	$(THIS)base/sal_itype.c	\
	$(THIS)base/sal_otype.c	\
	$(THIS)base/sal_bstream.c	\
	$(THIS)base/sal_serial.c
$(THIS)base/sal_char.c:	\
	$(THIS)base/sal_htype.c	\
	$(THIS)base/sal_itype.c	\
	$(THIS)base/sal_otype.c	\
	$(THIS)base/sal_bstream.c	\
	$(THIS)base/sal_serial.c	\
	$(THIS)base/sal_byte.c	\
	$(THIS)base/sal_torder.c
$(THIS)base/sal_copy.c:	\
	$(THIS)base/sal_base.c
$(THIS)base/sal_gener.c:	\
	$(THIS)lang/sal_lang.c
$(THIS)base/sal_htype.c:	\
	$(THIS)base/sal_base.c
$(THIS)base/sal_itype.c:	\
	$(THIS)base/sal_tstream.c
$(THIS)base/sal_manip.c:	\
	$(THIS)base/sal_byte.c
$(THIS)base/sal_order.c:	\
	$(THIS)base/sal_base.c
$(THIS)base/sal_otype.c:	\
	$(THIS)base/sal_tstream.c
$(THIS)base/sal_partial.c:	\
	$(THIS)base/sal_char.c	\
	$(THIS)base/sal_gener.c	\
	$(THIS)base/sal_syntax.c
$(THIS)base/sal_serial.c:	\
	$(THIS)base/sal_bstream.c
$(THIS)base/sal_syntax.c:	\
	$(THIS)base/sal_base.c
$(THIS)base/sal_torder.c:	\
	$(THIS)base/sal_order.c
$(THIS)base/sal_tstream.c:	\
	$(THIS)base/sal_base.c
$(THIS)datastruc/ald_flags.c:	\
	$(THIS)datastruc/sal_barray.c
$(THIS)datastruc/ald_queue.c:	\
	$(THIS)datastruc/ald_symtab.c
$(THIS)datastruc/ald_symbol.c:	\
	$(THIS)datastruc/sal_hash.c
$(THIS)datastruc/ald_symtab.c:	\
	$(THIS)datastruc/ald_symbol.c
$(THIS)datastruc/sal_array.c:	\
	$(THIS)arith/sal_bsearch.c	\
	$(THIS)datastruc/sal_parray.c
$(THIS)datastruc/sal_barray.c:	\
	$(THIS)datastruc/sal_pkarray.c
$(THIS)datastruc/sal_bdata.c:	\
	$(THIS)arith/sal_mint.c	\
	$(THIS)datastruc/sal_data.c
$(THIS)datastruc/sal_bstruc.c:	\
	$(THIS)datastruc/sal_bdata.c	\
	$(THIS)datastruc/sal_fstruc.c
$(THIS)datastruc/sal_ckarray.c:	\
	$(THIS)datastruc/sal_array.c
$(THIS)datastruc/sal_cklist.c:	\
	$(THIS)datastruc/sal_list.c	\
	$(THIS)arith/sal_random.c
$(THIS)datastruc/sal_ckmembk.c:	\
	$(THIS)datastruc/sal_memblk.c
$(THIS)datastruc/sal_data.c:	\
	$(THIS)base/sal_char.c
$(THIS)datastruc/sal_ddata.c:	\
	$(THIS)datastruc/sal_bdata.c
$(THIS)datastruc/sal_fstruc.c:	\
	$(THIS)datastruc/sal_lstruc.c
$(THIS)datastruc/sal_hash.c:	\
	$(THIS)datastruc/sal_kntry.c	\
	$(THIS)datastruc/sal_table.c
$(THIS)datastruc/sal_kntry.c:	\
	$(THIS)base/sal_char.c	\
	$(THIS)base/sal_copy.c
$(THIS)datastruc/sal_list.c:	\
	$(THIS)base/sal_partial.c	\
	$(THIS)datastruc/sal_bstruc.c	\
	$(THIS)datastruc/sal_ddata.c
$(THIS)datastruc/sal_lstruc.c:	\
	$(THIS)base/sal_gener.c	\
	$(THIS)datastruc/sal_data.c
$(THIS)datastruc/sal_memblk.c:	\
	$(THIS)datastruc/sal_array.c	\
	$(THIS)datastruc/sal_barray.c
$(THIS)datastruc/sal_parray.c:	\
	$(THIS)arith/sal_random.c	\
	$(THIS)datastruc/sal_fstruc.c	\
	$(THIS)datastruc/sal_list.c
$(THIS)datastruc/sal_pkarray.c:	\
	$(THIS)datastruc/sal_parray.c
$(THIS)datastruc/sal_set.c:	\
	$(THIS)datastruc/ald_symtab.c
$(THIS)datastruc/sal_slist.c:	\
	$(THIS)arith/sal_random.c	\
	$(THIS)datastruc/sal_bstruc.c	\
	$(THIS)datastruc/sal_list.c
$(THIS)datastruc/sal_sortas.c:	\
	$(THIS)datastruc/sal_kntry.c	\
	$(THIS)datastruc/sal_sset.c	\
	$(THIS)datastruc/sal_table.c
$(THIS)datastruc/sal_sset.c:	\
	$(THIS)datastruc/sal_list.c	\
	$(THIS)datastruc/sal_ddata.c	\
	$(THIS)datastruc/sal_bstruc.c	\
	$(THIS)arith/sal_random.c
$(THIS)datastruc/sal_stream.c:	\
	$(THIS)arith/sal_pointer.c	\
	$(THIS)datastruc/sal_list.c	\
	$(THIS)datastruc/sal_lstruc.c	\
	$(THIS)datastruc/sal_parray.c	\
	$(THIS)datastruc/sal_string.c
$(THIS)datastruc/sal_string.c:	\
	$(THIS)datastruc/sal_array.c	\
	$(THIS)datastruc/sal_pkarray.c
$(THIS)datastruc/sal_table.c:	\
	$(THIS)datastruc/sal_string.c
$(THIS)gmp/sal_fltgmp.c:	\
	$(THIS)datastruc/sal_string.c	\
	$(THIS)arith/sal_dfloat.c	\
	$(THIS)gmp/sal_intgmp.c
$(THIS)gmp/sal_gmptls.c:	\
	$(THIS)datastruc/sal_parray.c
$(THIS)gmp/sal_intgmp.c:	\
	$(THIS)arith/sal_int.c	\
	$(THIS)gmp/sal_gmptls.c
$(THIS)util/ald_trace.c:	\
	$(THIS)datastruc/sal_string.c	\
	$(THIS)arith/sal_pointer.c
$(THIS)util/eio_rsto.c:	\
	$(THIS)base/sal_base.c
$(THIS)util/rtexns.c:	\
	$(THIS)base/sal_manip.c	\
	$(THIS)util/ald_trace.c
$(THIS)util/sal_agat.c:	\
	$(THIS)datastruc/sal_string.c	\
	$(THIS)arith/sal_dfloat.c
$(THIS)util/sal_cmdline.c:	\
	$(THIS)datastruc/sal_string.c	\
	$(THIS)arith/sal_pointer.c
$(THIS)util/sal_file.c:	\
	$(THIS)datastruc/sal_string.c
$(THIS)util/sal_timer.c:	\
	$(THIS)arith/sal_pointer.c
$(THIS)util/sal_version.c:	\
	$(THIS)datastruc/sal_string.c
