
noinst_PROGRAMS=zacc msgcat

msgcat_SOURCES=msgcat.c

zacc_SOURCES=zaccscan.l zaccgram.y zacc.c cenum.c
# Forces a dependency on the zaccgram.h header file
zaccscan.o: zaccgram.c

# zacc: Parser generator with parameterized rules. Based on Yacc.
