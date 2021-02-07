include $(srcdir)/Makefile.deps

default: all

aldorsrcdir	:= $(top_srcdir)/aldor/src
aldorexedir	:= $(top_builddir)/aldor/src
aldorlibdir	:= $(top_builddir)/aldor/lib

libraryincdir	:= $(top_srcdir)/lib/$(libraryname)/include
librarylibdir	:= $(top_builddir)/lib/$(libraryname)/src

UNIQ		:= perl $(top_srcdir)/aldor/tools/unix/uniq

asdomains	:= $(internal) $(library) $(tests)
axdomains	:= $(axlibrary)
alldomains	:= $(asdomains) $(axdomains)

libsubdir	:= $(subst $(abs_libdir)/,,$(abs_builddir)/.)

include $(top_builddir)/lib/config.mk

define am_silent_template
AM_V_$(1)    = $$(am__v_$(1)_$$(V))
am__v_$(1)_  = $$(am__v_$(1)_$$(AM_DEFAULT_VERBOSITY))
am__v_$(1)_0 = @echo "  $(subst _,-,$(1))   " $$@;
endef

define am_silent_template_quiet
AM_V_$(1)    = $$(am__v_$(1)_$$(V))
am__v_$(1)_  = $$(am__v_$(1)_$$(AM_DEFAULT_VERBOSITY))
am__v_$(1)_0 = @
endef

STEPS := ALDOR AO2C AO2FM AR DEP FOAMJ JAR JAR JAVAC SRCJAR
QUIET_STEPS := ALDORTEST ALDORTESTJ

$(foreach rule,$(STEPS),$(eval $(call am_silent_template,$(rule))))
$(foreach rule,$(QUIET_STEPS),$(eval $(call am_silent_template_quiet,$(rule))))

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

aldor_common_args :=				\
	-Nfile=$(aldorsrcdir)/aldor.conf 	\
	-Mno-ALDOR_W_WillObsolete		\
	-Wcheck -Waudit $(AXLFLAGS)

AM_DBG := $(if $(filter 1,$(DBG)), gdb --args, $(DBG))
$(addsuffix .c, $(library)): %.c: %.ao %.dep
	$(AM_V_AO2C)				\
	$(AM_DBG) $(aldorexedir)/aldor			\
	  $(aldor_common_args)			\
	  -Fc=$(builddir)/$@			\
	  $<	

ifndef Libraryname
Libraryname := $(shell echo '$(libraryname)' | sed -e 's/^[a-z]/\u&/')
endif

aldor_args = $(aldor_common_args)		\
	-Y.					\
	-I$(libraryincdir)			\
	-l$(Libraryname)Lib=$(libraryname)_$*	\
	-DBuild$(Libraryname)Lib		\
	$($*_AXLFLAGS)				\
	-Fasy=$*.asy				\
	-Fao=$*.ao				\
	-Fabn=$*.abn				\
	$(filter %$*.as,$^)			\
	$(filter %$*.ax,$^)

$(addsuffix .dep,$(asdomains)): %.dep: %.as Makefile.in Makefile.deps
$(addsuffix .dep,$(axdomains)): %.dep: %.ax Makefile.in Makefile.deps
$(addsuffix .ao, $(asdomains)): %.ao: %.as
$(addsuffix .ao, $(axdomains)): %.ao: %.ax

$(addsuffix .ao, $(alldomains)): %.ao: $(foreach x,$(librarydeps),$(top_builddir)/lib/$(x)/src/lib$(x).al)
$(addsuffix .abn, $(alldomains)): %.abn: %.ao

SUBLIB		:= _sublib_$(libraryname)
SUBLIB_DEPEND	:= _sublib_depend_$(libraryname)

$(addsuffix .ao, $(alldomains)): %.ao: $(SUBLIB_DEPEND).al
	$(AM_V_ALDOR)set -e;							\
	rm -f $*.c $*.ao;							\
	cp $(SUBLIB_DEPEND).al lib$(libraryname)_$*.al;				\
	ar r lib$(libraryname)_$*.al $(addsuffix .ao, $(shell $(UNIQ) $*.dep));	\
	$(AM_DBG) $(aldorexedir)/aldor $(aldor_args);				\
	rm lib$(libraryname)_$*.al

$(SUBLIB_DEPEND).al: $(foreach l,$(library_deps),$(librarylibdir)/$l/$(SUBLIB).al) Makefile.deps
	$(AM_V_AR)set -e;		\
	ar cr $@;			\
	for l in $(filter %.al,$+); do	\
	   if [ ! -f $$l ]; then	\
	      echo "missing $$l";	\
	      exit 1;			\
	   fi;				\
	   ar x $$l;			\
	   ar r $@ $$(ar t $$l);	\
	   rm $$(ar t $$l);		\
        done

$(addsuffix .fm,$(alldomains)): %.fm: %.ao
	$(AM_V_AO2FM)				\
	$(aldorexedir)/aldor			\
	  $(aldor_common_args)			\
	  -Ffm=$@ $<

.PHONY: $(addsuffix .gloop, $(alldomains))
$(addsuffix .gloop, $(alldomains)): %.gloop:
	$(AM_V_ALDOR)set -e;							\
	rm -f $*.c $*.ao;							\
	cp $(SUBLIB_DEPEND).al lib$(libraryname)_$*.al;				\
	ar r lib$(libraryname)_$*.al $(addsuffix .ao, $(shell $(UNIQ) $*.dep));	\
	$(AM_DBG) $(aldorexedir)/aldor -gloop 	\
	  $(aldor_common_args) 			\
	  -Y.					\
	  -Y$(aldorlibdir)/libfoam/al		\
	  -I$(libraryincdir)			\
	  -l$(Libraryname)Lib=$(libraryname)_$*	\
	  -DBuild$(Libraryname)Lib		\
	  $($*_AXLFLAGS)			\

.PHONY: help
help:
	@echo 'additional targets:'
	@echo '%.gloop	- run interpreter with environment of %.as'
	@echo '%.fm	- generate foam file'
	@echo ''
	@echo 'useful variables:'
	@echo '	DBG 	- prefix for any aldor invocations'
	@echo '	DBG=1 	- shortcut for DBG="gdb --args"'

define dep_template
$1.ao: $1.dep $(addsuffix .ao,$($1_deps))
$1.dep: $(addsuffix .dep,$($1_deps))
endef

$(addsuffix .dep,$(alldomains) $(SUBLIB)): 
	$(AM_V_DEP)set -e;			\
	true > $@_tmp;				\
	for i in $(filter %.dep, $^); do	\
	   d=$$(basename $$i .dep);		\
	   cat $$i >> $@_tmp;			\
	   echo $(basename $$d) >> $@_tmp;	\
	done;					\
	if test ! -f $@; then			\
	   mv $@_tmp $@;			\
	elif diff $@ $@_tmp > /dev/null; then	\
	   rm $@_tmp;				\
	else					\
	   mv $@_tmp $@;			\
	fi

$(foreach l,$(alldomains), $(eval $(call dep_template,$(l))))

$(SUBLIB).dep: $(addsuffix .dep,$(library))
$(SUBLIB).dep: Makefile.deps

$(SUBLIB).al: $(SUBLIB).dep
$(SUBLIB).al: $(addsuffix .ao,$(library))
$(SUBLIB).al:
	$(AM_V_AR)							\
	rm -f $@;							\
	ar cr $@ $(addsuffix .ao, $(shell $(UNIQ) $(@:.al=.dep)))

all: Makefile $(SUBLIB).al
all: $(addsuffix .fm,$(library))
ifeq ($(bytecode_only),)
all: $(addsuffix .c,$(library))
endif

ifneq ($(BUILD_JAVA),)
ifneq ($(javalibrary),)
_javalibrary = $(filter-out $(java_blacklist), $(javalibrary))

$(patsubst %,aldorcode/%.java, $(_javalibrary)): aldorcode/%.java: %.ao
	$(AM_V_FOAMJ)$(AM_DBG)	\
	$(aldorexedir)/aldor $(aldor_common_args) -Fjava $*.ao

$(patsubst %,aldorcode/%.class, $(_javalibrary)): aldorcode/%.class: $(libraryname).classlib
# FIXME: -g here is ropey
$(libraryname).classlib: $(patsubst %,aldorcode/%.java, $(_javalibrary))
	$(AM_V_JAVAC)javac -g -cp $(aldorlibdir)/java/src/foamj.jar $^
	@touch $@

$(libraryname).jar: $(patsubst %,aldorcode/%.class, $(_javalibrary)) $(top_srcdir)/lib/buildlib.mk
	$(AM_V_JAR) \
	rm -f $@;	\
	rm -rf jar;	\
	mkdir jar;	\
	jar cf $@ $(patsubst %,aldorcode/%*.class, $(_javalibrary));  \
	for i in $(foreach i, $(SUBDIRS), $i/$(libraryname).jar); do \
		(cd jar; jar xf ../$$i);				\
		jar uf ../$@ -C jar .; done;				\
	rm -rf jar

$(libraryname)-sources.jar: $(patsubst %,aldorcode/%.class, $(_javalibrary)) $(top_srcdir)/lib/buildlib.mk
	$(AM_V_SRCJAR) \
	rm -f $@;	\
	rm -rf sources-jar;	\
	mkdir sources-jar;	\
	jar cf $@ $(patsubst %,aldorcode/%*.java, $(_javalibrary)); \
	for i in $(foreach i, $(SUBDIRS), $i/$(libraryname)-sources.jar); do \
		(cd jar; jar xf ../$$i);				\
		jar uf ../$@ -C sources-jar .; done;				\
	rm -rf jar

all: $(libraryname).jar	$(libraryname)-sources.jar \
	$(patsubst %,aldorcode/%.class,$(_javalibrary))

endif
endif

aldortests := $(patsubst %,%.aldortest-exec-interp,$(library))

$(aldortests): %.aldortest-exec-interp: Makefile
	$(AM_V_ALDORTEST) \
         (if ! grep -q '^#if ALDORTEST' $(srcdir)/$*.as; then exit 0; fi; \
	 echo "  ALDORTEST $*.as"; \
	 sed -n -e '/^#if ALDORTEST/,/^#endif/p' < $(srcdir)/$*.as > $*_test.as; \
	 $(AM_DBG) $(aldorexedir)/aldor $(aldor_common_args) -Y$(aldorlibdir)/libfoam/al \
			-I$(top_srcdir)/lib/aldor/include -Y$(top_builddir)/lib/aldor/src \
			-Y$(librarylibdir) -I$(libraryincdir) -ginterp -DALDORTEST \
			$($*_test_AXLFLAGS) \
			$*_test.as; \
	$(CHECK_TEST_STATUS) \
	)

CHECK_TEST_STATUS = \
	 status=$$?; \
	 exstatus=$(filter $*, $(XFAIL) $(XFAIL_$(subst $*.aldortest-exec-,,$@))); \
	 if ! [ "$$exstatus" = "" ] ; then \
	     if [ $$status = 0 ] ; then echo XPASS: $*; exit 1; else echo XFAIL: $*; exit 0; fi;  \
	 fi;\
	 if [ $$status = 0 ] ; then echo PASS: $*; else echo FAIL: $*; fi; \
	 exit $$status;

.PHONY: $(aldortests)

aldortestexecs := $(patsubst %,%.aldortest.exe,$(library))
aldortooldir = $(abs_top_builddir)/aldor/subcmd/unitools
foamdir = $(abs_top_builddir)/aldor/lib/libfoam
foamlibdir = $(abs_top_builddir)/aldor/lib/libfoamlib

$(aldortestexecs): %.aldortest.exe: Makefile %.as
	$(AM_V_ALDORTEST) \
         (if ! grep -q '^#if ALDORTEST' $(srcdir)/$*.as; then touch $@; fi; \
	 echo "  ALDORTEST $*.as"; \
	 sed -n -e '/^#if ALDORTEST/,/^#endif/p' < $(srcdir)/$*.as > $*_test.as; \
	 $(AM_DBG) $(aldorexedir)/aldor $(aldor_common_args) -Y$(aldorlibdir)/libfoam/al \
		        -Ccc=$(aldortooldir)/unicl	\
		      -Y$(foamdir) -Y			\
		      -Y$(foamlibdir) -l$(libraryname) $(patsubst %,-l%,$(librarydeps))  \
		        -Cargs="-Wconfig=$(aldorsrcdir)/aldor.conf -I$(aldorsrcdir) $(UNICLFLAGS)" \
			-I$(top_srcdir)/lib/aldor/include -Y$(top_builddir)/lib/aldor/src \
			-Y$(librarylibdir) -I$(libraryincdir) -fx=$@ -DALDORTEST $($*_TESTAXLFLAGS) \
			$*_test.as; )

ifneq ($(BUILD_JAVA),)
ifneq ($(javalibrary),)
aldortestjavas := $(patsubst %,%.aldortest-exec-java, \
			$(filter-out $(java_test_blacklist), $(_javalibrary)))

$(aldortestjavas): %.aldortest-exec-java: Makefile %.as
	$(AM_V_ALDORTESTJ) \
        (if grep -q '^#if ALDORTEST' $(srcdir)/$*.as; then \
	 echo "  ALDORTESTJ $*"; \
	 sed -n -e '/^#if ALDORTEST/,/^#endif/p' < $(srcdir)/$*.as > $*_jtest.as; \
	 $(AM_DBG) $(aldorexedir)/aldor $(aldor_common_args) -Y$(aldorlibdir)/libfoam/al \
		-Y$(foamdir) -Y$(foamlibdir) -l$(libraryname) $(patsubst %,-l%,$(librarydeps)) \
		-I$(top_srcdir)/lib/aldor/include -Y$(top_builddir)/lib/aldor/src \
		-Y$(librarylibdir) -I$(libraryincdir) -DALDORTEST $$(cat $*_jtest.as | grep ^aldoroptions: | sed -e 's/aldoroptions://') \
		-Fjava -Ffm -Jmain \
		$($*_test_AXLFLAGS) \
		$*_jtest.as; \
	 javac -g -cp $(aldorlibdir)/java/src/foamj.jar aldorcode/$*_jtest.java; \
	 java -cp .:$(aldorlibdir)/java/src/foamj.jar:$(aldorlibdir)/libfoam/al/foam.jar:$(top_builddir)/lib/$(libraryname)/src/$(libraryname).jar:$(top_builddir)/lib/aldor/src/aldor.jar aldorcode.$*_jtest; \
	 $(CHECK_TEST_STATUS) \
	 fi;)

.PHONY: $(aldortestjavas)
endif
endif

check: $(aldortests) $(aldortestjavas)

# 
# :: Automake requires this little lot
#
mostlyclean: 
	rm -f $(SUBLIB_DEPEND).al
	rm -f $(SUBLIB).dep
	rm -f lib*.al
	rm -f $(libraryname).classlib
	rm -f $(libraryname).jar
	rm -f *.dep
	rm -f *.ao
	rm -f $(addsuffix .c,$(alldomains))
	rm -f $(addsuffix .fm,$(alldomains))
	rm -f *.java
	rm -f *.class
	rm -f *.exe
	rm -f *.abn
	rm -f *.asy

clean: mostlyclean
	rm -f $(SUBLIB).al

distclean: clean 
	rm -f $(addsuffix .dep,$(alldomains))
	rm Makefile
maintainer-clean: distclean

install-data:
	$(MKDIR_P) $(DESTDIR)$(datarootdir)/aldor/lib/$(libraryname)/$(libsubdir)
	for i in $(library); do \
		if test -f $(abs_srcdir)/$$i.as; then \
			$(INSTALL_DATA) $(abs_srcdir)/$$i.as $(DESTDIR)$(datarootdir)/aldor/lib/$(libraryname)/$(libsubdir)/$$i.as; \
		fi; \
		if test -f $$i.abn; then \
			$(INSTALL_DATA) $$i.abn $(DESTDIR)$(datarootdir)/aldor/lib/$(libraryname)/$(libsubdir)/$$i.abn; \
		fi; \
		if test -f $$i.asy; then \
			$(INSTALL_DATA) $$i.asy $(DESTDIR)$(datarootdir)/aldor/lib/$(libraryname)/$(libsubdir)/$$i.asy; \
		fi; \
	done

uninstall:
	rm -rf $(datarootdir)/lib/$(libraryname)/$(libsubdir)

install: install-data install-exec

EMPTY_AUTOMAKE_TARGETS  = dvi pdf ps info html tags ctags
EMPTY_AUTOMAKE_TARGETS += install-exec uninstall
EMPTY_AUTOMAKE_TARGETS += install-dvi install-html install-info install-ps install-pdf
EMPTY_AUTOMAKE_TARGETS += installdirs
EMPTY_AUTOMAKE_TARGETS += check installcheck

.PHONY: $(EMPTY_AUTOMAKE_TARGETS)
$(EMPTY_AUTOMAKE_TARGETS):
