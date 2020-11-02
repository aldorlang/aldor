
tooldir= $(abs_top_builddir)/aldor/tools/unix

include $(top_builddir)/lib/config.mk

all: pdf dvi

dvi: $(doc).dvi
pdf: $(doc).pdf
html: html/html/index.html
.PHONY: dvi ps pdf html

.PRECIOUS: Makefile
Makefile: $(srcdir)/Makefile.in $(top_builddir)/config.status
	@case '$?' in \
	  *config.status*) \
	    cd $(top_builddir) && $(MAKE) $(AM_MAKEFLAGS) am--refresh;; \
	  *) \
	    echo ' cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@ '; \
	    cd $(top_builddir) && $(SHELL) ./config.status $(subdir)/$@ ;; \
	esac;

generated = $(patsubst %.fig,%.eps,$(figs)) $(other)
all_deps = $(patsubst %,$(srcdir)/%,$(sources)) $(generated)

$(patsubst %.fig, %.eps,$(figs)): $(top_srcdir)/lib/builddoc.mk
$(patsubst %.fig, %.eps,$(figs)): %.eps: $(srcdir)/%.fig
	if [ ! -f $*.fig ]; then cp $(srcdir)/$*.fig .; fi
	fig2eps --viewer=true $*.fig

$(doc).dvi: $(top_srcdir)/lib/builddoc.mk
$(doc).dvi: $(all_deps)
	set -e; 		\
	rm -rf ./dvi; mkdir dvi; cd dvi;	  \
	ln -sf $(patsubst %,../%,$(all_deps)) . ; \
	ln -sf ../gen/* .;				\
	latex $(doc) < /dev/null | grep -v Underfull | grep -v vbox ; 	\
	latex $(doc) < /dev/null | grep -v Underfull | grep -v vbox ; 	\
	makeindex $(doc) ;						\
	latex $(doc) < /dev/null | grep -v Underfull | grep -v vbox;	\
	mv $(doc).dvi ..

$(doc).pdf: $(top_srcdir)/lib/builddoc.mk
$(doc).pdf: $(all_deps)
	set -e; 			\
	rm -rf pdf; mkdir pdf; cd pdf;	\
	ln -sf $(patsubst %,../%,$(all_deps)) . ; \
	ln -sf ../gen/* .;				\
	pdflatex $(doc) < /dev/null | grep -v Underfull | grep -v vbox;	\
	pdflatex $(doc) < /dev/null | grep -v Underfull | grep -v vbox;	\
	makeindex $(doc);							\
	pdflatex $(doc) < /dev/null | grep -v Underfull | grep -v vbox;	\
	mv $(doc).pdf ..

# This kind of works, but the aldoc.cls is not correctly processed, leading
# to a mess.  Looking at the documentation, latex2html reads a .perl file
# instead of the .cls.  There may well be other issues here, but not yet
# investigated.

#html/html/index.html: $(all_deps)
#	set -e; set -x;					\
#	rm -rf html_work;				\
#	mkdir html_work;				\
#	cd html_work; 					\
#	ln -sf $(patsubst %,../%,$(all_deps)) . ;	\
#	ln -sf ../gen/* .;				\
#	$(tooldir)/aldoc2html -o html.tex $(doc).tex; \
#	latex html < /dev/null;		\
#	latex html < /dev/null;		\
#	TEXINPUTS=. latex2html html;		\
#	mv html_work/html html

install: install-pdf install-dvi

install-pdf: $(doc).pdf
	$(MKDIR_P) $(DESTDIR)/$(datarootdir)/doc
	$(INSTALL_DATA) $(doc).pdf $(DESTDIR)/$(datarootdir)/doc/$(doc).pdf

install-dvi: $(doc).dvi
	$(MKDIR_P) $(DESTDIR)/$(datarootdir)/doc
	$(INSTALL_DATA) $(doc).dvi $(DESTDIR)/$(datarootdir)/doc/$(doc).dvi

clean:
	rm -rf pdf $(doc).pdf
	rm -rf dvi $(doc).dvi
	rm -rf html_work html
	rm -f $(patsubst %.fig,%.eps,$(figs))

EMPTY_AUTOMAKE_TARGETS  = ps info html tags ctags
EMPTY_AUTOMAKE_TARGETS += install-exec uninstall
EMPTY_AUTOMAKE_TARGETS += install-dvi install-html install-info install-ps install-pdf
EMPTY_AUTOMAKE_TARGETS += installdirs
EMPTY_AUTOMAKE_TARGETS += check installcheck

.PHONY: $(EMPTY_AUTOMAKE_TARGETS)
$(EMPTY_AUTOMAKE_TARGETS):
