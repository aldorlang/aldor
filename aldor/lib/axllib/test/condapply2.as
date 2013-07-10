-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).

--> testrun -laxllib -Q3


#include "axllib"

Foo(T:Type): with { foo: T -> T } == add {
	if T has Ring then
		innerFoo(a:T):T == a + a;
	else
		innerFoo(a:T):T == a;

        foo(a:T):T == innerFoo(a);
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


test2();
test1();

