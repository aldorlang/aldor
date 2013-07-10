# Targets:
#
# update
#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

# All directories that require building
DIRS = util extree extree/operators extree/parser categories basic numbers\
	mat mat/gauss mat/modular mat/linalg\
	multpoly/multpolydata multpoly/exponent\
	univpoly/categories mat/linalg2 univpoly univpoly/gcd\
	fraction series mat/linalg3 algext polyfactorp ffield polyfactor0\
	multpoly/multpolycat multpoly/multpolydom multpoly/multpolytest

UPDATED=$(ALGEBRAROOT)/updated
UPDATING=$(ALGEBRAROOT)/updating
ALGEBRABIN=$(ALGEBRAROOT)/bin

UPDATETARGS = $(DIRS:%=%.update)

update: updateroot updatedirs

updateroot:
	cp -f $(UPDATING)/Makefile $(UPDATED)/
	cp -f $(UPDATING)/Makefile.Rules $(UPDATED)/

updatedirs: 
	for i in $(DIRS) ; do \
		echo "Updating $$i ... "; \
		mkdir -p $(UPDATED)/$$i ; \
		cd $(ALGEBRABIN) ; \
		./updatedir.sh $(UPDATING)/$$i $(UPDATED)/$$i ; \
		echo " ...  done." ; done









