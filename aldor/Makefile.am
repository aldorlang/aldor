SUBDIRS=aldor
if BUILD_LIBS
SUBDIRS+=lib
endif

ACLOCAL_AMFLAGS=-I m4

if CI_CHECK
releasecheck-local:
	@$(YQ) $(top_srcdir)/../.github/workflows/ci.yaml > /dev/null
endif
