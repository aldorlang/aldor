include $(srcdir)/Makefile.deps

default: all

aldorsrcdir	:= $(top_srcdir)/aldor/src
aldorexedir	:= $(top_builddir)/aldor/src
aldorlibdir	:= $(top_builddir)/aldor/lib

libraryincdir	:= $(top_srcdir)/lib/$(libraryname)/include
librarylibdir	:= $(top_builddir)/lib/$(libraryname)/src

UNIQ		:= perl $(top_srcdir)/aldor/tools/unix/uniq

# Aldor
AM_V_ALDOR = $(am__v_ALDOR_$(V))
am__v_ALDOR_ = $(am__v_ALDOR_$(AM_DEFAULT_VERBOSITY))
am__v_ALDOR_0 = @echo "  ALDOR " $@;

AM_V_AO2FM = $(am__v_AO2FM_$(V))
am__v_AO2FM_ = $(am__v_AO2FM_$(AM_DEFAULT_VERBOSITY))
am__v_AO2FM_0 = @echo "  AO2FM " $@;

AM_V_AR = $(am__v_AR_$(V))
am__v_AR_ = $(am__v_AR_$(AM_DEFAULT_VERBOSITY))
am__v_AR_0 = @echo "  AR    " $@;

AM_V_DEP = $(am__v_DEP_$(V))
am__v_DEP_ = $(am__v_DEP_$(AM_DEFAULT_VERBOSITY))
am__v_DEP_0 = @echo "  DEP   " $@;

AM_V_FOAMJ = $(am__v_FOAMJ_$(V))
am__v_FOAMJ_ = $(am__v_FOAMJ_$(AM_DEFAULT_VERBOSITY))
am__v_FOAMJ_0 = @echo "  FOAMJ " $@;

AM_V_JAR = $(am__v_JAR_$(V))
am__v_JAR_ = $(am__v_JAR_$(AM_DEFAULT_VERBOSITY))
am__v_JAR_0 = @echo "  JAR   " $@;

AM_V_JAVAC = $(am__v_JAVAC_$(V))
am__v_JAVAC_ = $(am__v_JAVAC_$(AM_DEFAULT_VERBOSITY))
am__v_JAVAC_0 = @echo "  JAVAC " $@;

# Check the makefile
.PRECIOUS: Makefile
Makefile: $(srcdir)/Makefile.in $(top_builddir)/config.status 
	@case '$?' in \
	  *config.status*) \
	    cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh;; \
	  *) \
	    echo ' cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@ '; \
	    cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@ ;; \
	esac;


$(addsuffix .c, $(library)): %.c: %.ao %.dep
	 $(aldorexedir)/aldor -Nfile=$(aldorsrcdir)/aldor.conf -Fc=$(builddir)/$@ $<	

Libraryname := $(shell echo '$(libraryname)' | sed -e 's/^[a-z]/\u&/')
aldor_args =					\
	-Nfile=$(aldorsrcdir)/aldor.conf 	\
	-Y.					\
	-I$(libraryincdir)			\
	-l$(Libraryname)Lib=$(libraryname)_$*	\
	-DBuild$(Libraryname)Lib		\
	$(AXLFLAGS)				\
	-Fao=$*.ao $(srcdir)/$*.as

$(addsuffix .dep,$(library)): %.dep: Makefile $(srcdir)/%.as Makefile.deps
$(addsuffix .ao, $(library)): %.ao: $(srcdir)/%.as
$(addsuffix .ao, $(library)): %.ao: _sublib_libdep.al
	$(AM_V_ALDOR)								\
	rm -f $*.c $*.ao;							\
	cp _sublib_libdep.al lib$(libraryname)_$*.al;			\
	ar r lib$(libraryname)_$*.al $(addsuffix .ao, $(shell $(UNIQ) $*.dep));	\
	$(aldorexedir)/aldor $(aldor_args);					\
	rm lib$(libraryname)_$*.al;						\

_sublib_libdep.al: $(foreach l,$(library_deps),$(librarylibdir)/$l/_sublib.al)
	$(AM_V_AR)			\
	ar cr $@;			\
	for l in $+; do 		\
	   if [ ! -f $$l ]; then	\
	      echo "missing $$l";	\
	      exit 1;			\
	   fi;				\
	   ar x $$l;			\
	   ar r $@ $$(ar t $$l);	\
	   rm $$(ar t $$l);		\
        done

$(addsuffix .cmd, $(library)): %.cmd: Makefile
	echo "run $(aldor_args)" > $@

$(addsuffix .fm,$(library)): %.fm: %.ao
	$(AM_V_AO2FM)				\
	$(aldorexedir)/aldor			\
	   -Nfile=$(aldorsrcdir)/aldor.conf	\
	   -Ffm=$@ $<

define dep_template
$1.ao: $1.dep $(addsuffix .ao,$($1_deps))
$1.dep: $(addsuffix .dep,$($1_deps))
endef

$(addsuffix .dep,$(library) _sublib): 
	$(AM_V_DEP)						\
	truncate --size 0 $@_tmp;				\
	for i in $(filter %.dep, $^); do			\
	   d=$$(basename $$i .dep);				\
	   cat $$i >> $@_tmp; echo $(basename $$d )>> $@_tmp;	\
	done;							\
	if test ! -f $@; then					\
	   mv $@_tmp $@;					\
	elif diff $@ $@_tmp > /dev/null; then			\
	   mv $@_tmp $@;					\
	else							\
	   rm $@_tmp;						\
	fi

$(foreach l,$(library), $(eval $(call dep_template,$(l))))

_sublib.dep: $(addsuffix .dep,$(library))
_sublib.dep: Makefile.deps

_sublib.al: _sublib.dep
_sublib.al: $(addsuffix .ao,$(library))
_sublib.al:
	$(AM_V_AR)							\
	rm -f $@;							\
	ar cr $@ $(addsuffix .ao, $(shell $(UNIQ) $(@:.al=.dep)))

all: Makefile _sublib.al		\
	$(addsuffix .c,$(library))	\
	$(addsuffix .fm,$(library))

ifneq ($(javalibrary),)
$(addsuffix .java, $(library)): %.java: %.fm $(aldorexedir)/javagen
	$(AM_V_FOAMJ)$(aldorexedir)/javagen $< > $@

$(addsuffix .class, $(library)): %.class: $(libraryname).classlib
$(libraryname).classlib: $(addsuffix .java, $(library))
	$(AM_V_JAVAC)javac -cp $(aldorlibdir)/java/src/foamj.jar $^
	@touch $@

$(libraryname).jar: $(addsuffix .class, $(library))
	$(AM_V_JAR)jar -cf $@ $(addsuffix *.class, $(library))

all: $(libraryname).jar				\
	$(addsuffix .java,$(javalibrary))	\
	$(addsuffix .class,$(javalibrary))
endif

# 
# :: Automake requires this little lot
#
mostlyclean: 
	rm -f _sublib_libdep.al
	rm -f lib$(libraryname).al
	rm -f $(libraryname).classlib
	rm -f $(libraryname).jar
	rm -f $(addsuffix .c,$(library))
	rm -f $(addsuffix .ao,$(library))
	rm -f $(addsuffix .fm,$(library))
	rm -f $(addsuffix .java,$(library))
	rm -f $(addsuffix *.class,$(library))

clean: mostlyclean
	rm -f _sublib.al

distclean: clean 
	rm -f $(addsuffix .dep,$(library))
	rm Makefile
maintainer-clean: distclean


EMPTY_AUTOMAKE_TARGETS  = dvi pdf ps info html tags ctags
EMPTY_AUTOMAKE_TARGETS += install install-data install-exec uninstall
EMPTY_AUTOMAKE_TARGETS += install-dvi install-html install-info install-ps install-pdf
EMPTY_AUTOMAKE_TARGETS += installdirs
EMPTY_AUTOMAKE_TARGETS += check installcheck

.PHONY: $(EMPTY_AUTOMAKE_TARGETS)
$(EMPTY_AUTOMAKE_TARGETS):
