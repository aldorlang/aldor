##########################################
#
# Makefile to build an Aldor distribution
#
# Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
#
##########################################
#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

include Makefile.globals

all:
	@echo "Usage:"
	@echo "   make aldorcompiler => make the compiler and install it"
	@echo "   make generic       => make the generic directory containing platform-independent libs and sources for the tests"
	@echo "   make cleanforest   => clean the source directories"
	@echo "   make testaxl       => tests aldor compiler using axllib tests"
	@echo "   make distrib       => make the whole distribution"
	@echo "Modify Makefile.globals for setup."

# ----------------------------------------------------
# COMPILER
# ----------------------------------------------------

#aldorcompiler: clean cleangeneric init
aldorcompiler: init
	@(cd aldor; $(MAKE) compiler)

# ----------------------------------------------------
# GENERIC AND DOCUMENTATION
# ----------------------------------------------------

generic: initgeneric genericlibs utilities
	@cp README.binary-only $(GENERIC_DIR)/README

genericlibs:
	@(cd lib; $(MAKE) generic)

utilities:
	@(PATH=$(ALDORROOT)/bin:$(PATH); export PATH; cd utils; $(MAKE) install)

preparedistrib: docpdf gentest copysrc

# ----------------------------------------------------------
# Copy sources that should be provided in the distribution.
# ----------------------------------------------------------

copysrc: srclibaldor

srclibaldor:
	mkdir -p $(GENERIC_DIR)/src
	cp -r $(ALDORLIBROOT)/src $(GENERIC_DIR)/src/libaldor
	for i in `find $(GENERIC_DIR)/src/libaldor -name CVS`; do rm -rf $$i; done 

# ----------------------------------------------------
# Documentation pdf of libraries and user guide.
# ----------------------------------------------------

docpdf: libaldordocpdf algebradocpdf aldorugpdf

libaldordocpdf: checklibaldor cleanlibaldordoc
	@(PATH=$(ALDORROOT)/bin:$(PATH); export PATH; \
	  cd $(ALDORLIBROOT)/src; $(MAKE) doc; \
	  cd $(ALDORLIBROOT)/doc/tex; $(MAKE) pdf; \
	  cd $(ALDORLIBROOT); $(MAKE) buildtutorial; \
	  mkdir -p $(GENERIC_DIR)/doc/libaldor; \
	  mkdir -p $(GENERIC_DIR)/doc/libaldor/tutorial; \
	  cp $(ALDORLIBROOT)/doc/libaldor.pdf $(GENERIC_DIR)/doc/libaldor; \
	  cp $(ALDORLIBROOT)/tutorial/*.as $(GENERIC_DIR)/doc/libaldor/tutorial; \
	  chmod a-x $(ALDORLIBROOT)/tutorial/*.as; \
	  cp $(ALDORLIBROOT)/tutorial/tutorial.pdf $(GENERIC_DIR)/doc/libaldor/tutorial)

# had to copy aldoc.cls and ttyverb.sty from libaldor dir
algebradocpdf: checklibaldor checkalgebra cleanalgebradoc
	@(PATH=$(ALDORROOT)/bin:$(PATH); export PATH; \
	  cd $(ALDORLIBROOT)/src; $(MAKE) doc; \
	  cd $(ALGEBRAROOT)/src; $(MAKE) ALGEBRAROOT=$(ALGEBRAROOT) doc; \
	  cd $(ALGEBRAROOT)/doc/tex; $(MAKE) ALDORLIBROOT=$(ALDORLIBROOT) ALGEBRAROOT=$(ALGEBRAROOT) pdf; \
	  mkdir -p $(GENERIC_DIR)/doc/algebra; \
	  cp $(ALGEBRAROOT)/doc/algebra.pdf $(GENERIC_DIR)/doc/algebra)

aldorugpdf:
	@(PATH=$(ALDORROOT)/bin:$(PATH); export PATH; \
	  cd aldorug; $(MAKE) aldorug.pdf; \
	  mkdir -p $(GENERIC_DIR)/doc/aldorug; \
	  cp aldorug.pdf $(GENERIC_DIR)/doc/aldorug)

# ----------------------------------------------------
# Documentation (ps and pdf) of libaldor and algebra libraries.
# ----------------------------------------------------
#rhx: If a path in TEXINPUTS ends in two / signs then latex looks down
# all subdirectories for input/style files. The program aldoc2html
# does not follow that rule but instead puts '\input FILENAME' into
# the file so that latex2html must interpret it.
documentation: libaldordoc algebradoc

cleanlibaldordoc: checklibaldor
	cd $(ALDORLIBROOT)/doc/tex; $(MAKE) clean

libaldordoc: checklibaldor
	cd $(ALDORLIBROOT)/src; $(MAKE) doc
	cd $(ALDORLIBROOT)/doc/tex; $(MAKE) ps pdf html

cleanalgebradoc: checkalgebra
	cd $(ALGEBRAROOT)/doc/tex; $(MAKE) ALDORLIBROOT=$(ALDORLIBROOT) clean

algebradoc: checklibaldor checkalgebra
	cd $(ALDORLIBROOT)/src; $(MAKE) doc
# We need to 'make doc' in ALGEBRAROOT as well don't we?
	cd $(ALGEBRAROOT)/doc/tex; $(MAKE) ALDORLIBROOT=$(ALDORLIBROOT) prepare ps pdf html

# ------------------------------------------------------------
# Generating the tests for the aldor and algebra libraries ...
# ------------------------------------------------------------
gentest: gentestaldor gentestalgebra


gentestaldor:
	@(PATH=$(ALDORROOT)/bin:$(PATH); export PATH; \
	  cd $(ALDORLIBROOT)/src; $(MAKE) test)
	@(PATH=$(ALDORROOT)/bin:$(PATH); export PATH; \
	  utils/removezerosized.sh $(ALDORLIBROOT)/test; \
	  mkdir -p $(GENERIC_DIR)/tests/libaldor; \
	  cp $(ALDORLIBROOT)/test/*.as $(GENERIC_DIR)/tests/libaldor; \
	  cp $(ALDORLIBROOT)/test/testall $(GENERIC_DIR)/tests/libaldor; \
	  cp $(ALDORLIBROOT)/test/Makefile $(GENERIC_DIR)/tests/libaldor)

gentestalgebra:
	@(PATH=$(ALDORROOT)/bin:$(PATH); export PATH; \
	  cd $(ALGEBRAROOT); $(MAKE) testfiles; \
	  mkdir -p $(GENERIC_DIR)/tests/algebra; \
	  cp $(ALGEBRAROOT)/test/*.as $(GENERIC_DIR)/tests/algebra; \
	  cp $(ALGEBRAROOT)/test/testall $(GENERIC_DIR)/tests/algebra; \
	  cp $(ALGEBRAROOT)/test/Makefile $(GENERIC_DIR)/tests/algebra)

# ----------------------------------------------------
# Testing the aldor and algebra libraries ...
# ----------------------------------------------------
#rhx: Here we assume that "make install" has been performed.
# Run the testsuite for aldor and algebra. Note that axllib has no
# testsuite.
test: testaldor testalgebra

testaldor:
	(PATH=$(ALDORROOT)/bin:$(PATH); export PATH; \
	cd $(ALDORLIBROOT); $(MAKE) runtest)

testalgebra:
	(PATH=$(ALDORROOT)/bin:$(PATH); export PATH; \
	cd $(ALGEBRAROOT);  $(MAKE) testfiles runtest)

#testaxl: ax0  axldem
testaxl: $(ALDORROOT)/lib/libax0.al $(ALDORROOT)/lib/libaxldem.al
	(PATH=$(ALDORROOT)/bin:$(PWD)/aldor/tools/unix:$(PATH); export PATH; \
	cd $(AXLROOT)/test; $(MAKE) all ) 
#	(PATH=$(ALDORROOT)/bin:$(PATH); export PATH; \
#	cd $(AXLROOT)/test; $(MAKE) all ) 

$(ALDORROOT)/lib/libaxldem.al:
	(PATH=$(PWD)/aldor/tools/unix:$(PATH); export PATH; \
	 cd lib/libaxldem ; make all)

$(ALDORROOT)/lib/libax0.al:
	chmod u+x $(PWD)/aldor/tools/unix/docc
	chmod u+x $(PWD)/aldor/tools/unix/doaldor
	chmod u+x $(PWD)/aldor/tools/unix/buildarg
	chmod u+x $(PWD)/aldor/tools/unix/doranlib
	(cd $(PWD) ; cd lib; \
	 cd libax0; $(MAKE) all; cd ..;)

#axldem:
#	(PATH=$(PWD)/aldor/tools/unix:$(PATH); export PATH; \
#	 cd lib/libaxldem ; make all)
#
#ax0:
#	chmod u+x $(PWD)/aldor/tools/unix/docc
#	chmod u+x $(PWD)/aldor/tools/unix/doaldor
#	chmod u+x $(PWD)/aldor/tools/unix/buildarg
#	chmod u+x $(PWD)/aldor/tools/unix/doranlib
#	(cd $(PWD) ; cd lib; \
#	 cd libax0; $(MAKE) all; cd ..;)


# ----------------------------------------------------
# SPECIFIC LIBRARIES
# ----------------------------------------------------

libaxllib: checklibaxllib
	@(cd $(LIBAXLLIBROOT); $(MAKE) library)

checklibaxllib:
	-@ if [ "x$(LIBAXLLIBROOT)" = "x" ] ; then echo "*** LIBAXLLIBROOT is not set"; fi
	@ [ "x$(LIBAXLLIBROOT)" != "x" ]

libaldor: checklibaldor
	@(cd $(ALDORLIBROOT); $(MAKE) library)

checklibaldor:
	-@ if [ "x$(ALDORLIBROOT)" = "x" ] ; then echo "*** ALDORLIBROOT is not set"; fi
	@ [ "x$(ALDORLIBROOT)" != "x" ]

algebra: checklibaldor checkalgebra
	@(cd $(ALGEBRAROOT); $(MAKE) library)

checkalgebra:
	-@ if [ "x$(ALGEBRAROOT)" = "x" ] ; then echo "*** ALGEBRAROOT is not set"; fi
	@ [ "x$(ALGEBRAROOT)" != "x" ]


# ----------------------------------------------------
# CLEANING TARGETS
# ----------------------------------------------------

cleanall: clean cleangeneric


clean: cleancompiler cleanlibs
	@echo "Cleaning done."

cleancompiler:
	@(cd aldor; $(MAKE) remove)

#cleandoc:
#	@(cd aldorug; $(MAKE) clean)

cleanlibs:
	@(cd lib; $(MAKE) cleansources)

cleangeneric:
	@(/bin/rm -rf $(GENERIC_DIR))

cleanforest:
	@(echo "Cleaning compiler sources"; cd aldor; $(MAKE) cleansources; cd ..; \
	  echo "Cleaning libraries sources"; cd lib; $(MAKE) cleansources; cd ..)

# ----------------------------------------------------
# INITIALIZATION
# ----------------------------------------------------

init:
	@(mkdir -p $(ALDOR_INSTALL)/$(MACHINE)/$(ALDOR_VERSION); \
	  echo "Initialization done.")

initgeneric:
	@mkdir -p $(GENERIC_DIR)
	@mkdir -p $(GENERIC_DIR)/lib
	@mkdir -p $(GENERIC_DIR)/doc
	@mkdir -p $(GENERIC_DIR)/tests
	@mkdir -p $(GENERIC_DIR)/utils
	@cp Makefile.generic $(GENERIC_DIR)/Makefile
	@cp Makefile.globals $(GENERIC_DIR)/Makefile.globals

# ----------------------------------------------------

# ----------------------------------------------------
# DISTRIB
# ----------------------------------------------------

distrib: aldorcompiler generic
	(cd $(GENERIC_DIR); make install)

