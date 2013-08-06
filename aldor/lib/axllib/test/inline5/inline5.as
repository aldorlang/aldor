-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Based on bug 1039
--> testrun -Q3 -l axllib 

#include "axllib"

define C1: Category == with {
	bar: () -> DoubleFloat;
	foo: () -> DoubleFloat;
	default {
		foo(): DoubleFloat == 1.2;
	}
}


-- D1 used to confuse the compiler because it thought it was a
-- domain-valued function rather than a parameterised domain.
-- When the optimiser tried to inline the calls to foo() it only
-- saw the default and inlined that instead. When it saw bar()
-- it couldn't find anything to inline so it didn't.
--
-- The work around is to ensure that the type of D1 is `C1 with'
-- so that the compiler knows that the RHS is a domain. This is
-- actually what we do in abnorm now but it can still be fooled
-- if the RHS doesn't have an obvious `add' as a return value.
D1(d: DoubleFloat): C1 == {
	print<< "XX: " << d << newline;
	add {
		foo(): DoubleFloat == d;
		bar(): DoubleFloat == d;
	}
}

t1(): () == {
	import from DoubleFloat;
	import from D1(2.2);
	print << foo() << newline;
	print << foo() << newline;
	print << bar() << newline;
	print << bar() << newline;
}

t1();

