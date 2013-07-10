-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--% ratio
--!!> testgen y		Reinstate when recompiling ratio0.as works.
--> testgen c
--> testrun -l axllib

#include "axllib"

f() : () == {
	import from Integer;
	import from SingleInteger;

	r: Ratio Integer := 1111111111111111111111111111 /
			    2222222222222222222222222222 ;
	s: Ratio SingleInteger := 1/2 + 1/3;
	print<<r<<newline;
	print<<r + r<<newline;
	print<<s<<newline;
}

f()
