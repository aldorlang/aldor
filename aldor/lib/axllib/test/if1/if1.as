-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testerrs -M no-emax

#include "axllib.as"
#pile

-- Test a basic if-then-else set of conditional exports.

f0(): () == {
	-- We still need to define how exports from else clauses work.
	PType ==> (if R has Field then Field else Ring);
	P(R: Ring): PType == add pretend PType;

	f(): () == x: P Complex Float := (1 + 1) / 0;
#if TestErrorsToo
	g(): () == y: P Integer := (1 + 1) / 0;
#endif
}

-- Test an if-then without the else.

f1(): () == {
	PType ==> (if R has Field then Field);
	P(R: Ring): PType == add pretend PType;

	f(): () == x: P Complex Float := (1 + 1) / 0;
#if TestErrorsToo
	g(): () == y: P Integer := (1 + 1) / 0;
#endif
}

-- Test a Join of an if-then with additional exports.

f2(): () == {
	PType ==> Join(if R has Field then Field, Ring) with;
	P(R: Ring): PType == add pretend PType;

	f(): () == x: P Complex Float := (1 + 1) / 0;
#if TestErrorsToo
	g(): () == y: P Integer := (1 + 1) / 0;
#endif
}

-- Make sure we can substitute for % in the conditions of the if.

f3(): () == {
	PType ==> Join(Ring, if R has Field then Field) with;
	P(R: Ring): PType == add pretend PType;

	f(): () == x: P Complex Float := (1 + 1) / 0;
#if TestErrorsToo
	g(): () == y: P Integer := (1 + 1) / 0;
#endif
}
