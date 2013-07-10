-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun -l axllib
#pile
#include "axllib"

P: with
	R: Ring
	r: R
	f: R -> R
  == add
	R: Ring == Integer
	r: R    == 1$R
	f: R->R == -$R

h1():() == {                   (print << f(r$P)$P)$R$P;  print<<newline }
h2():() == { import from P;    (print << f r     )$R;    print<<newline }
h3():() == { import from P, R; (print << f r);           print<<newline }

h1()
h2()
h3()
