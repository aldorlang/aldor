SUBDIRS = util numbers extree extree/operators extree/parser categories	\
	  basic basic/compbug mat mat/gauss mat/modular			\
	  mat/modular/compbug mat/linalg multpoly/multpolydata		\
	  multpoly/exponent univpoly/categories mat/linalg2 univpoly	\
	  univpoly/gcd fraction series series/compbug mat/linalg3	\
	  algext polyfactorp ffield

# polyfactor0 multpoly/multpolycat	\
#	  multpoly/multpolydom multpoly/multpolypkg			\
#	  multpoly/multpolytest


#lang_ALDOR = lang/sal_lang.c

aldorsrcdir = $(abs_top_builddir)/aldor/src

AM_CFLAGS = -I$(aldorsrcdir)

all: libalgebra.al libalgebra.a

libalgebra.al: $(foreach i,$(SUBDIRS),$(i)/algebra$(i).al)
	for i in $(SUBDIRS); \
	do 				\
	   l=$${i}/algebra$${i}.al;	\
	   for mbr in $$(ar t $$l);	\
	   do 				\
		ar x $$l $$mbr;		\
		ar r $@ $$mbr;		\
		rm $$mbr;		\
	   done;			\
	done

libalgebra.a: libalgebra.al
	rm -f libalgebra.a
	for i in $(ar t libalgebra.a); 	\
	do				\
		ar x libalgebra.al $$i	\
		$(aldorsrcdir)/aldor -Fo  $$i
		ar r libalgebra.a $$(basename $$i).o
	done