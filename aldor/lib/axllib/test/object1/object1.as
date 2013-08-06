-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--!! --> testrun

#include "axllib.as"

f(): () == {
	import from SingleInteger, SingleFloat, DoubleFloat, Ratio Integer;
	import from Character;

	x: List Object Order := [
		object(Character, char "g"),
		object(Integer, -13),
		object(SingleInteger, 2),
		object(SingleFloat, 1.25),
		object(DoubleFloat, -0.4),
		object(Ratio Integer, 2/3)
	];

	--!! Both these tests test code generation of multi-valued defines.

	-- Test that type inference can match the dependent product.
	print << "Test 1: ";
	for ob in x repeat {
		(S: Order, s: S) == avail ob;
		if S has OrderedAbelianMonoid then
			if s > 0 then print << "[" << s << "]";
	}
	print << newline;

	-- Test that type inference can deduce the types on the lhs of define.
	print << "Test 2: ";
	for ob in x repeat {
		(T, t) == avail ob;
		--!! Remove g() when type inference can deduce the type of T
		--!! before it needs to import it in the same scope.
		g () : () == {
			import from T;
			if T has OrderedAbelianMonoid then
				if t > 0 then print << "[" << t << "]";
		}
		g();
	}
	print << newline;
}

f();
