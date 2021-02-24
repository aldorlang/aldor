# Install aldor wrapper script.
bin_SCRIPTS = aldor gdb-aldor


#tooldir= $(abs_top_builddir)/build/tools
#tool_SCRIPTS=doaldor doas doaxiomxl docc dog++ dolatex domkmk	\
#	     dopdflatex doranlib buildarg

noinst_PROGRAMS = zacc msgcat atinlay extract aldoc2html

msgcat_SOURCES = msgcat.c

# zacc: Parser generator with parameterized rules. Preprocessor
# generating input for Yacc.
zacc_SOURCES =		\
	zaccscan.l	\
	zaccgram.y	\
	zacc.c		\
	cenum.c

atinlay_SOURCES = atinlay.c

extract_SOURCES = \
	extract.c \
	flags.c

aldoc2html_SOURCES = \
	aldoc2html.c \
	flags.c


# Forces a dependency on the zaccgram.h header file
zacc.o: zaccgram.c
zaccscan.o: zaccgram.c
