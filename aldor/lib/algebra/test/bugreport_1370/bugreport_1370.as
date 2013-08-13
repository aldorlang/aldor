-- Author: Ralf Hemmecke, Johannes Kepler Universit"at Linz
-- EMail: ralf@hemmecke.de
-- Date: 14-Jun-2004
-- Aldor version 1.0.2 for LINUX(glibc2.3)
-- Subject: integer multiplication overloading

-- Compile with
-- aldor -q5 -lalgebra -laldor -fx xxx.as
-- and run the executable. The result is
--: e=1
--: r=3
--: Segmentation fault

#assert TRACE

#include "algebra"

main(): () == {
	import from String, Symbol;
	R ==> Integer;
	V ==> OrderedVariableTuple(-"x");
	E ==> MachineIntegerDegreeLexicographicalExponent(V);
	P ==> DistributedMultivariatePolynomial1(R, V, E);
	import from R, E, P;
	eins: P := 1;
	TRACE("e=", eins);
	r: Integer := 3@Integer;
	TRACE("r=", r);
	p := r * eins;
	TRACE("p=", p);
}

main();
