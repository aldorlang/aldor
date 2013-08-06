-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
#include "axllib.as"

GcdDomain: Category == Ring with { gcd: (%, %) -> % };

Foo(R:Ring):Category == with {
	foo: % -> %;
	if R has GcdDomain then GcdDomain;
	if R has Field then {
-- COMPILES OK IF THE ORDER OF THOSE 2 LINES IS INVERTED!!!!
		EuclideanDomain;
		GcdDomain;
	}
}

Bar(F:Join(Field, GcdDomain), P:Foo F): with { ident: P -> P } == add {
	ident(p:P):P == gcd(p,p);
}
