-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testint -Mno-ALDOR_W_GenDomFunNotConst
--> testcomp -Mno-ALDOR_W_GenDomFunNotConst
--> testrun -l axllib -Mno-ALDOR_W_GenDomFunNotConst
#pile

#include "axllib"

Foo(n: Integer): IntegerNumberSystem ==
	Integer

foo(n: Integer): () ==
	local a: Foo(n)
	a := 1
	a := a+1
	print<<a<<newline

import from Integer

foo(3)
