SUBDIRS = lang base arith datastruc util gmp 

lib_LIBRARIES = libaldor.a

libaldor_a_SOURCES =	\
	arith/sal_arith.c	\
	arith/sal_binpow.c	\
	arith/sal_bool.c	\
	arith/sal_bsearch.c	\
	arith/sal_complex.c	\
	arith/sal_dfloat.c	\
	arith/sal_fltcat.c	\
	arith/sal_ftools.c	\
	arith/sal_int.c	\
	arith/sal_intcat.c	\
	arith/sal_itools.c	\
	arith/sal_lincomb.c	\
	arith/sal_mint.c	\
	arith/sal_oarith.c	\
	arith/sal_pointer.c	\
	arith/sal_random.c	\
	arith/sal_segment.c	\
	arith/sal_sfloat.c	\
	base/ald_pfunc.c	\
	base/sal_base.c	\
	base/sal_bstream.c	\
	base/sal_byte.c	\
	base/sal_char.c	\
	base/sal_copy.c	\
	base/sal_gener.c	\
	base/sal_htype.c	\
	base/sal_itype.c	\
	base/sal_manip.c	\
	base/sal_order.c	\
	base/sal_otype.c	\
	base/sal_partial.c	\
	base/sal_serial.c	\
	base/sal_syntax.c	\
	base/sal_torder.c	\
	base/sal_tstream.c	\
	datastruc/ald_symbol.c	\
	datastruc/ald_symtab.c	\
	datastruc/sal_array.c	\
	datastruc/sal_barray.c	\
	datastruc/sal_bdata.c	\
	datastruc/sal_bstruc.c	\
	datastruc/sal_data.c	\
	datastruc/sal_ddata.c	\
	datastruc/sal_fstruc.c	\
	datastruc/sal_hash.c	\
	datastruc/sal_kntry.c	\
	datastruc/sal_list.c	\
	datastruc/sal_lstruc.c	\
	datastruc/sal_memblk.c	\
	datastruc/sal_parray.c	\
	datastruc/sal_pkarray.c	\
	datastruc/sal_set.c	\
	datastruc/sal_slist.c	\
	datastruc/sal_sortas.c	\
	datastruc/sal_sset.c	\
	datastruc/sal_stream.c	\
	datastruc/sal_string.c	\
	datastruc/sal_table.c	\
	lang/sal_lang.c	\
	util/ald_trace.c	\
	util/eio_rsto.c	\
	util/rtexns.c	\
	util/sal_agat.c	\
	util/sal_cmdline.c	\
	util/sal_file.c	\
	util/sal_timer.c	\
	util/sal_util.c	\
	util/sal_version.c

libraryname = aldor
include ../../buildlib.am

aldorincdir = $(srcdir)/../include

datalibdir = $(datadir)/lib
datalib_DATA = libaldor.al aldor_gloop.ao #aldor_gloopd.ao

CLEANFILES = $(datalib_DATA)


%d.ao: %.as $(aldorexedir)/aldor libaldor.al
	$(aldorexedir)/aldor -DDEBUG	\
	   -Nfile=$(aldorsrcdir)/aldor.conf -Y. -I$(aldorincdir) -Fao=$@ $<

%.ao: %.as $(aldorexedir)/aldor libaldor.al
	$(aldorexedir)/aldor	\
	   -Nfile=$(aldorsrcdir)/aldor.conf -Y. -I$(aldorincdir) -Fao=$@ $<
