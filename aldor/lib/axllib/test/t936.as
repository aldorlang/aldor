-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs

#include "axllib"

SExpr  ==> Integer;

define SExprFormer(X: BasicType): Category == with {
	convert: X -> SExpr;
	convert: SExpr -> X;
}

FractionSExprOperations(
	X: Ring,
	R: with {
		numerator: % -> X;
		denominator: % -> X;
		/: (X,X) -> %;
	}
): with { foo: R-> R} == add {
	import from X, SExpr, R, C;

	foo(x:R): R == numerator x/denominator x;


}
ANYTHING==>List FileName;

import from SingleInteger;
print << (foo$(FractionSExprOperations(Integer, ANYTHING))
		pretend SingleInteger)

