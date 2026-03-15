SUBDIRS=aldor
if BUILD_LIBS
SUBDIRS+=lib
endif

ACLOCAL_AMFLAGS=-I m4

if CI_CHECK
all: ci-check
ci-check:
	@set -e; \
	for file in $$(find $(top_srcdir)/../.github/workflows -name \*.yaml); \
	do \
	  echo Checking $$file; \
	  $(YQ) $$file > /dev/null; \
	done
endif
