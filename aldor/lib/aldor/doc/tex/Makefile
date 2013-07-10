#
#  2006/Dec/22
#
#  On Solaris, the value of PWD does not change when make is invoked
#  recursively.  Consequently, it is necessary to explicitly set it
#  at the start of the makefile.
#
PWD = $(shell pwd)

all: dvi

dvi: ../libaldor.dvi
ps: ../libaldor.ps
pdf: ../libaldor.pdf
html: ../html/html.html

../libaldor.ps: ../libaldor.dvi sallicat.eps sallidata.eps
	rm -fr ../libaldor.ps
	dvips -f ../libaldor.dvi | psnup -2 > ../libaldor.ps

../libaldor.dvi: libaldor.tex alltypes.tex title.tex intro.tex guide.tex\
		 refer.tex sallicat.eps sallidata.eps
	rm -f *.aux *.ind *.idx *.toc *.ilg
	latex libaldor | grep -v Underfull | grep -v vbox
	latex libaldor | grep -v Underfull | grep -v vbox
	makeindex libaldor
	latex libaldor | grep -v Underfull | grep -v vbox
	mv libaldor.dvi ..

sallicat.eps: sallicat.fig
	fig2dev -Leps sallicat.fig sallicat.eps

sallidata.eps: sallidata.fig
	fig2dev -Leps sallidata.fig sallidata.eps

sallicat.pdf: sallicat.fig
	fig2dev -Lpdf sallicat.fig sallicat.pdf

sallidata.pdf: sallidata.fig
	fig2dev -Lpdf sallidata.fig sallidata.pdf

../libaldor.pdf: libaldor.tex sallicat.pdf sallidata.pdf alltypes.tex\
		title.tex intro.tex guide.tex refer.tex
	rm -f *.aux *.ind *.idx *.toc *.ilg
	pdflatex libaldor | grep -v Underfull | grep -v vbox
	pdflatex libaldor | grep -v Underfull | grep -v vbox
	makeindex libaldor
	pdflatex libaldor | grep -v Underfull | grep -v vbox
	mv libaldor.pdf ..

html.tex: libaldor.tex alltypes.tex title.tex intro.tex guide.tex refer.tex
	aldoc2html -o html.tex libaldor

html.aux: html.tex
	latex html > /dev/null
	latex html > /dev/null

../html/html.html: html.aux
	rm -fr ../html
	latex2html html
	mv html ..
	rm -f html.*

.PHONY: clean
clean:
	-rm -f *.dvi *.aux *.log *.idx *.ilg *.ind *.toc sal_*.tex ald_*.tex
	-rm -f rtexns.tex *.out ../html.tex ../*.dvi ../*.ps ../*.pdf stamp*
	-rm -f *.pdf *.eps
	rm -fr ../html

