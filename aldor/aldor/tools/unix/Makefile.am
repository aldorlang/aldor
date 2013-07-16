
#tooldir= $(abs_top_builddir)/build/tools
#tool_SCRIPTS=doaldor doas doaxiomxl docc dog++ dolatex domkmk	\
#	     dopdflatex doranlib buildarg

noinst_PROGRAMS=zacc msgcat atinlay

msgcat_SOURCES=msgcat.c

zacc_SOURCES=zaccscan.l zaccgram.y zacc.c cenum.c

atinlay_SOURCES = atinlay.c

# Forces a dependency on the zaccgram.h header file
zacc.o: zaccgram.c
zaccscan.o: zaccgram.c

# zacc: Parser generator with parameterized rules. Based on Yacc.
