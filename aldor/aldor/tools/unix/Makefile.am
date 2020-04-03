# Install aldor wrapper script.
bin_SCRIPTS = aldor gdb-aldor


#tool_SCRIPTS=doaldor doas doaxiomxl docc dog++ dolatex domkmk	\
#	     dopdflatex doranlib buildarg
$(builddir)/mkaolib: $(srcdir)/mkaolib.sh
	cp $(srcdir)/mkaolib.sh $@
	chmod a+x $@

all: $(builddir)/mkaolib

noinst_PROGRAMS = zacc msgcat atinlay

msgcat_SOURCES = msgcat.c

# zacc: Parser generator with parameterized rules. Preprocessor
# generating input for Yacc.
zacc_SOURCES =		\
	zaccscan.l	\
	zaccgram.y	\
	zacc.c		\
	cenum.c

atinlay_SOURCES = atinlay.c

# Forces a dependency on the zaccgram.h header file
zacc.o: zaccgram.c
zaccscan.o: zaccgram.c
