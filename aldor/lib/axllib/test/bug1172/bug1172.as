-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-- Original author: Manuel Bronstein

--> testrun -Q1 -l axllib
--> testrun -Q3 -l axllib

#include "axllib"

Foo(T:Type): with { foo: T -> T } == add {
	if T has Ring then {
		foo(a:T):T == a + a;
	}
	else {
		foo(a:T):T == a;
	}
}


test1():() ==
{
	import from String, Foo String;
	print << "foo(_"very bad bug_") = _"";
	print << (foo "very bad bug") << "_"" << newline;
}


test2():() ==
{
	import from SingleInteger, Foo SingleInteger;
	print << "foo(42) = ";
	print << (foo 42) << newline;
}


test1();
test2();
