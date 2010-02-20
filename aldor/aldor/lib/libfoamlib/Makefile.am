
libfoamlib_ALDOR=\
	array.as	\
	basic.as	\
	boolean.as	\
	char.as		\
	file.as		\
	fname.as	\
	foamcat.as	\
	foamlib.as	\
	format.as	\
	gener.as	\
	lang.as		\
	langx.as	\
	list.as		\
	machine.as	\
	opsys.as	\
	oslow.as	\
	parray.as	\
	partial.as	\
	pointer.as	\
	segment.as	\
	sfloat.as	\
	sinteger.as	\
	string.as	\
	textwrit.as	\
	tuple.as	


basics= lang machine 

lang.ao:
machine.ao: lang.ao


noinst_LIBRARIES=libfoamlib.a

libfoamlib_a_SOURCES=lang.c machine.c

aldorexedir=$(abs_top_builddir)/aldor/src
$(patsubst %, %.c, $(basics)): %.c: %.ao
	ALDORROOT=$(aldorexedir) 	\
	 $(aldorexedir)/aldor -Fc $<	

$(patsubst %, %.ao, $(basics)): %.ao: %.as
	ALDORROOT=$(aldorexedir) 	\
	 $(aldorexedir)/aldor -Fas $<	


