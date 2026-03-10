# For AM_V_*

include $(testdir)/test-config.mk
include $(top_srcdir)/mk/step.mk
include $(top_builddir)/lib/config.mk
include $(top_builddir)/aldor/test/test-config.mk

testcommon = $(abs_testsrcdir)/test-common.mk

$(call am_define_steps,\
    ALDOR ALDOR_AP ALDOR_CMD ALDOR_EXE ALDOR_FM ALDOR_GENC LSP_SCRIPT LSP_TEST\
    ALDOR_JAVATEST ALDOR_OBJ JAVAC ALDOR_JAVA JAVAC JAVA_CP JUNIT CLEANONE)

all: really-all

.PRECIOUS: Makefile
Makefile: $(srcdir)/Makefile.in $(top_builddir)/config.status
	@case '$?' in \
	  *config.status*) \
	    cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh;; \
	  *) \
	    echo ' cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@ '; \
	    cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@ ;; \
	esac;

AM_DBG := $(if $(filter 1,$(DBG)), gdb --args, $(DBG))
AM_DBG_J := $(if $(filter 1,$(DBG_J)), gdb --args, $(DBG_J))
AM_DBG_C := $(if $(filter 1,$(DBG_C)), gdb --args, $(DBG_C))
AM_DBG_FM := $(if $(filter 1,$(DBG_FM)), gdb --args, $(DBG_FM))

aldorsrcdir   = $(abs_top_srcdir)/aldor/src
aldorexedir   = $(abs_top_builddir)/aldor/src
aldortooldir  = $(abs_top_builddir)/aldor/subcmd/unitools
foamsrclibdir = $(abs_top_srcdir)/aldor/lib/libfoamlib
foamlibdir    = $(abs_top_builddir)/aldor/lib/libfoamlib
foamsrcdir    = $(abs_top_srcdir)/aldor/lib/libfoam
foamdir       = $(abs_top_builddir)/aldor/lib/libfoam

utilsrcdir   = $(abs_top_srcdir)/aldor/test/utils

_aptests    := $(sort $(aptests))
_jruntests  := $(if $(BUILD_JAVA), $(sort $(jruntests)),)
_junittests := $(if $(HAS_JUNIT),  $(sort $(junittests)),)
_jtests     := $(if $(BUILD_JAVA), $(sort $(jtests) $(_jruntests)),)
_lsptests   := $(if $(BUILD_LISP), $(sort $(lsptests)))
_xtests     := $(sort $(xtests))
_otests     := $(sort $(otests) $(_xtests))
_ctests     := $(sort $(ctests) $(_otests))
_fmtests    := $(sort $(fmtests) $(_jtests) $(_ctests))
_aotests    := $(sort $(_fmtests) $(_ctests) $(_xtests))
_alltests   := $(sort $(_aptests) $(_fmtests) $(_ctests) $(_otests) $(_xtests) $(_jtests))

nfile := -Nfile=$(aldorsrcdir)/aldor.conf

.PHONY: all-fm
all-fm: $(patsubst %, out/fm/%.fm, $(utils) $(_fmtests))

$(patsubst %, out/fm/%.fm, $(utils) $(_fmtests)): out/fm/%.fm: out/ao/%.ao
	$(AM_V_ALDOR_FM)		\
	mkdir -p $$(dirname $@);	\
	$(AM_DBG_FM) $(aldorexedir)/aldor $(nfile) -Ffm=$(builddir)/$@ $<

.PHONY: all-lsp
all-lsp: $(patsubst %, out/lsp/%.lsp, $(utils) $(_lsptests))

$(patsubst %, out/lsp/%.lsp, $(utils) $(_fmtests)): out/lsp/%.lsp: out/ao/%.ao
	$(AM_V_ALDOR_LSP)		\
	mkdir -p $$(dirname $@);	\
	$(AM_DBG_FM) $(aldorexedir)/aldor $(nfile) -Flsp=$(builddir)/$@ $<

.PHONY: all-c
all-c: $(patsubst %,out/c/%.c,$(_ctests))

$(patsubst %, out/c/%.c, $(utils) $(_ctests)): out/c/%.c: out/ao/%.ao
	$(AM_V_ALDOR_GENC)		\
	mkdir -p $$(dirname $@);	\
	$(AM_DBG_C) $(aldorexedir)/aldor $(nfile) -Fc=$(builddir)/$@ $<

.PHONY: all-java
all-java: $(patsubst %,out/java/aldorcode/%.java,$(_jtests))

$(patsubst %, out/java/aldorcode/%.java, $(utils) $(_jtests)): out/java/aldorcode/%.java: out/fm/%.fm
	$(AM_V_ALDOR_JAVA)		\
	mkdir -p $$(dirname $@);	\
	$(AM_DBG_J) $(aldorexedir)/aldor \
                -Wcheck $(nfile) 	 \
		$(if $(filter $(_jruntests), $*), -Jmain,) \
		-Fjava=$(builddir)/out/java/$*.java $<

$(patsubst %, %-javatest, $(_jtests)): out/java/aldorcode/tassert.class
$(patsubst %, out/ao/%.ao, $(_aotests)): out/ao/tassert.ao

javaopts=-cp $(abs_top_builddir)/aldor/lib/java/src/foamj.jar

define java_import_dependency_template
out/java/aldorcode/$1.class: $(patsubst %,out/java/%.java,$(subst .,/,$($(1)_srcjava)))
$(1)_srcjava:
	echo $(1) $($(1)-srcjava) $(patsubst %,out/java/%.java,$(subst .,/,$($(1)_srcjava)))
endef

define junit_class_dependency_template
out/java/$1.class: $(patsubst %,out/java/%.class,$(subst .,/,$($(1)_classes)))
endef

$(foreach jtest,$(utils) $(_jtests), $(eval $(call java_import_dependency_template,$(jtest))))
$(foreach junit,$(_junittests),      $(eval $(call junit_class_dependency_template,$(junit))))

allsrcjava := $(foreach jtest,$(_jtests),$(patsubst %,out/java/%.java,$(subst .,/,$($(jtest)_srcjava))))

$(allsrcjava): out/java/%.java: $(srcdir)/%.java
	$(AM_V_JAVA_CP) \
	(mkdir -p $(dir $@); cp $(srcdir)/$*.java $@)

$(patsubst %, out/java/aldorcode/%.class, $(utils) $(_jtests)): out/java/aldorcode/%.class: out/java/aldorcode/%.java
	$(AM_V_JAVAC) \
	(mkdir -p $$(dirname $@); cd $(builddir)/out/java;  \
	 javac $(javaopts) aldorcode/$*.java \
			   $(addsuffix .java,$(subst .,/,$($*_extjava) $($*_srcjava))) \
	)

# Create .o files locally as unicl creates files in the
# current directory...
$(patsubst %, %.o, $(utils) $(_otests)): %.o: out/ao/%.ao
	$(AM_V_ALDOR_OBJ)		\
	mkdir -p $$(dirname $@);	\
	$(AM_DBG) $(aldorexedir)/aldor $(nfile) 	\
			      -Ccc=$(aldortooldir)/unicl \
			      -Cargs="-Wconfig=$(aldorsrcdir)/aldor.conf $(UNICLFLAGS) -I$(aldorsrcdir)" \
			      -Fo=$(builddir)/$@ $<

$(patsubst %, out/ap/%.ap, $(_aptests)): out/ap/%.ap: $(srcdir)/%.as
	$(AM_V_ALDOR_AP)		\
	mkdir -p $$(dirname $@);	\
	$(AM_DBG) $(aldorexedir)/aldor $(nfile) -I$(foamsrclibdir)/al \
			      -Fap=$(builddir)/$@ $<

define aldor_args
	$(nfile) 		\
	-Wcheck			\
	-Y$(foamlibdir)/al	\
	-Y out/ao		\
	-I$(foamsrclibdir)/al	\
	-I$(utilsrcdir)		\
	-lAxlLib=foamlib 		\
	$(AXLFLAGS) $($*_AXLFLAGS)	\
	-Fao=out/ao/$*.ao $(if $($*_loc),$($*_loc)/$*.as,$(srcdir)/$*.as)
endef

$(foreach u,$(utils), $(eval $(u)_loc=$(utilsrcdir)))

.PHONY: all-ap
all-ap: $(patsubst %, out/ao/%.ap, $(_aotests))
$(patsubst %, out/ap/%.ap, $(_aotests)): out/ap/%.ap: %.as
	$(AM_V_ALDOR_AP)		\
	mkdir -p $$(dirname $@);	\
	$(aldorexedir)/aldor $(nfile) -Fap=$@ $(srcdir)/$*.as

.PHONY: all-ao
all-ao: $(patsubst %, out/ao/%.ao, $(_aotests))
$(patsubst %, out/ao/%.ao, $(_aotests)): out/ao/%.ao: %.as
$(patsubst %, out/ao/%.ao, $(utils) $(_aotests)): $(aldorexedir)/aldor
$(patsubst %, out/ao/%.ao, $(utils) $(_aotests)): out/ao/%.ao:
	$(AM_V_ALDOR)			\
	mkdir -p $$(dirname $@);	\
	$(AM_DBG) $(aldorexedir)/aldor $($*_opts) $(aldor_args)

$(patsubst %, out/ao/%.cmd, $(_aotests)): out/ao/%.cmd: %.as
	$(AM_V_ALDOR_CMD)	 \
	mkdir -p $$(dirname $@); \
	echo run '$(aldor_args)' > $@

.PHONY: all-exe
all-exe: $(patsubst %, %.exe, $(_xtests))
$(patsubst %, %.exe, $(_xtests)): %.exe: %.o rtexns.o tassert.o
	$(AM_V_ALDOR_EXE)				\
	rm -f $@;					\
	$(AM_DBG) $(aldorexedir)/aldor $(nfile) 	\
		      -Ccc=$(aldortooldir)/unicl	\
		      -Y$(foamdir)			\
		      -Y$(foamlibdir)			\
		      -Lfoamlib				\
		      -Cargs="-Wconfig=$(aldorsrcdir)/aldor.conf -I$(aldorsrcdir) $(UNICLFLAGS)" \
		      -Fx=$@ out/ao/$*.ao rtexns.o tassert.o

classpath=$(abs_top_builddir)/aldor/lib/java/src/foamj.jar:$(abs_top_builddir)/aldor/lib/libfoam/al/foam.jar:$(abs_top_builddir)/aldor/lib/libfoamlib/al/foamlib.jar
$(patsubst %, %-javatest,$(_jruntests)): %-javatest: out/java/aldorcode/%.class
	$(AM_V_ALDOR_JAVATEST) java -cp out/java:$(classpath) aldorcode.$*

$(patsubst %,out/java/%.class,$(_junittests)): out/java/%.class: %.java
	$(AM_V_JAVAC) \
	(mkdir -p $$(dirname $@); \
	 cd $(builddir)/out/java; \
         javac -d . -cp $(classpath)/../src/foamj.jar:$(JUNIT_JAR):. \
		$(abs_srcdir)/$*.java)

.PHONY: $(addsuffix -junittest,$(_junittests))

$(addsuffix -junittest,$(_junittests)): %-junittest: out/java/%.class
	$(AM_V_JUNIT) \
	java -cp $(classpath):$(JUNIT_JAR):out/java \
	         org.junit.runner.JUnitCore $*

check-java: $(patsubst %,%-javatest,$(_jruntests)) $(patsubst %,%-junittest,$(_junittests))

$(patsubst %,out/lsp/%_in.lsp, $(_lsptests)): out/lsp/%_in.lsp: Makefile $(testcommon)
	@mkdir -p out/lsp				
	$(AM_V_LSP_SCRIPT) 				\
	(echo '(load "$(utilsrcdir)/prelude.lsp")';	\
	 echo '(load "$(foamsrcdir)/foam_l.lsp")';	\
	 echo '(load "$(foamdir)/al/runtime.lsp")';	\
	 echo '(load "$(foamlibdir)/al/array.lsp")';	\
	 echo '(load "$(foamlibdir)/al/sinteger.lsp")';	\
	 echo '(load "$(foamlibdir)/al/list.lsp")';	\
	 echo '(load "$(foamlibdir)/al/lang.lsp")';	\
	 echo '(load "$(foamlibdir)/al/character.lsp")';	\
	 echo '(load "$(foamlibdir)/al/textwrit.lsp")';	\
	 echo '(load "$(foamlibdir)/al/basic.lsp")';	\
	 echo '(load "$(foamlibdir)/al/bool.lsp")';	\
	 echo '(load "$(foamlibdir)/al/string.lsp")';	\
	 echo '(load "$(foamlibdir)/al/foamcat.lsp")';	\
	 echo '(load "$(foamlibdir)/al/opsys.lsp")';	\
	 echo '(load "$(foamlibdir)/al/segment.lsp")';	\
	 echo '(load "$(foamlibdir)/al/machine.lsp")';	\
	 echo '(load "out/lsp/tassert.lsp")';		\
	 echo '(load "out/lsp/$*.lsp")';		\
	 echo '(in-package :FOAM-USER)';		\
	 echo '(print "calling G-$*")';			\
	 echo '(|CCall| |G-$*|)') > $@

.PHONY: $(patsubst %,%-lspsbcl,$(_lsptests))

$(patsubst %,%-lspsbcl,$(_lsptests)): %-lspsbcl: Makefile $(testcommon)
	sbcl --load out/lsp/$*_in.lsp

.PHONY: all-lsp-tests
check: all-lsp-tests
all-lsp-tests: $(patsubst %, %-lsptest, $(_lsptests))
$(addsuffix -lsptest, $(_lsptests)): %-lsptest: $(srcdir)/prelude.lsp out/lsp/%.lsp out/lsp/%_in.lsp out/lsp/tassert.lsp
	$(AM_V_LSP_TEST) 	\
	(sbcl --non-interactive --load out/lsp/$*_in.lsp)

$(addsuffix -xtest,$(_xtests)): %-xtest: %.exe
	$(AM_V_ALDOR_EXE) ./$*.exe

.PHONY: check-xtest
check-xtest: $(patsubst %,%-xtest,$(_xtests))
check: check-xtest

define all_template
all-$(1): $(patsubst %, out/ao/%.ao, $(filter $(1), $(_aotests)))
all-$(1): $(patsubst %, out/fm/%.fm, $(filter $(1), $(_fmtests)))
all-$(1): $(patsubst %, out/c/%.c,   $(filter $(1), $(_ctests)))
all-$(1): $(patsubst %, out/java/aldorcode/%.java, $(filter $(1), $(_jtests)))
.PHONY: all-$(1)
endef

define check_template
check-$(1): $(patsubst %, %-javatest,  $(filter $(1), $(_jruntests)))
check-$(1): $(patsubst %, %-junittest, $(filter $(1), $(_junittests)))
check-$(1): $(patsubst %, %-xtest,     $(filter $(1), $(_xtests)))
check-$(1): $(patsubst %, %-lsptest,   $(filter $(1), $(_lsptests)))
.PHONY: check-$(1)
endef

define clean_template
clean-$(1):
	$(call AM_P_CLEANONE, $(1)) \
	for ii in $(patsubst %, out/ao/%.ao, $(filter $(1), $(_aotests))) 	\
		  $(patsubst %, out/ao/%.ao, $(filter $(1), $(_aotests)))	\
		  $(patsubst %, out/fm/%.fm, $(filter $(1), $(_fmtests)))	\
		  $(patsubst %, out/c/%.c,   $(filter $(1), $(_ctests)))	\
		  $(patsubst %, %.exe,       $(filter $(1), $(_xtests)))	\
		  $(patsubst %, out/lsp/%.lsp,    	   $(filter $(1), $(_lsptests)))	\
		  $(patsubst %, out/lsp/%_in.lsp, 	   $(filter $(1), $(_lsptests)))	\
		  $(patsubst %, out/java/aldorcode/%.java, $(filter $(1), $(_jtests))); do \
		rm -f $$$$ii; \
	done
.PHONY: clean-$(1)
endef

$(foreach test, $(_alltests), $(eval $(call all_template,$(test))))
$(foreach test, $(_alltests), $(eval $(call check_template,$(test))))
$(foreach test, $(_alltests), $(eval $(call clean_template,$(test))))

.PHONY: check-java
check: check-java

really-all: \
     $(patsubst %,out/ap/%.ap,$(_aptests)) \
     $(patsubst %,out/ao/%.cmd,$(_aotests)) \
     $(patsubst %,out/fm/%.fm,$(_fmtests)) \
     $(patsubst %,out/c/%.c,$(_ctests)) \
     $(patsubst %,out/java/aldorcode/%.java,$(_jtests)) \
     $(patsubst %,out/java/aldorcode/%.class,$(_jtests)) \
     $(patsubst %,out/java/aldorcode/%.class,$(_jruntests)) \
     $(patsubst %,%.o,$(_otests))  \
     $(patsubst %,%.exe,$(_xtests))

.PHONY: all

check: all

# Check each target is buildable standalone
check-makefile:
	set -e; for i in $(_alltests); do make clean && make all-$$i; done

# Check all files are in git
all-check-source: $(patsubst %,check-source-%, $(_aotests))
$(patsubst %,check-source-%, $(utils) $(_aotests)): check-source-%:
	@(cd $(srcdir); git ls-files --error-unmatch $*.as)


releasecheck: all-check-source check-makefile

#
# :: Automake requires this little lot
#
mostlyclean:
	rm -rf $(builddir)/out
	rm -f $(patsubst %,%.o,$(utils) $(_otests))
	rm -f $(patsubst %,%.exe,$(_xtests))
clean: mostlyclean

distclean: clean
	rm Makefile

maintainer-clean: distclean


EMPTY_AUTOMAKE_TARGETS  = dvi pdf ps info html tags ctags
EMPTY_AUTOMAKE_TARGETS += install install-data install-exec uninstall
EMPTY_AUTOMAKE_TARGETS += install-dvi install-html install-info install-ps install-pdf
EMPTY_AUTOMAKE_TARGETS += installdirs
EMPTY_AUTOMAKE_TARGETS += check installcheck

.PHONY: $(EMPTY_AUTOMAKE_TARGETS)
$(EMPTY_AUTOMAKE_TARGETS):
