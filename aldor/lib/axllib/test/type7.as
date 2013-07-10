-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun  -O -l axllib
--> testcomp -O
-- Testing sundry type combinations.
-- major type kinds are builtins (Records, Unions) and user-defined.
-- Enumerations should be covered a little more completely...

#include "axllib"

f1(): List Integer == [];

f(): List Record(Integer) == {
	import from Integer;
	[[2]]
}

#if 0
--- needs to be optimised 'cos List Record(...) can't be made at runtime 
f(m: Integer): List Record(n: Integer, x:IntegerMod n) == {
	import from Integer;
	import from IntegerMod m;
	a: Record(n: Integer, x: IntegerMod n) := [m, coerce m];
	b: Record(n: Integer, x: IntegerMod n) := [m, coerce m];
	c: Record(n: Integer, x: IntegerMod n) := [m, coerce m];
	[a, b, c]
}

g(): () == {
	import from List Record(n: Integer, x:IntegerMod n);
	for e in f(10) repeat print << e.n << " " 
				    << e.x pretend Integer << newline;
}

#endif

h(): Record(Integer) == {
	import from Integer;
	[10];
}

i(): () == {
	import from Record Integer, Integer;
	x := explode h();
	print << x << newline;
}
i();

h1(): List Record(Integer) == {
	import from Integer;
	[[10]]
}

j(): () == {
	import from List Record Integer, Integer;
	x := explode first h1();
	print << x << newline;
}
j();

k(): List Cross(String, Integer) == {
	import from Integer;
	cons(("hello " ,2), empty());
}

l(): () == {
	import from List Cross(String, Integer);
	import from Integer;
	(a, b) := first k();
	print << a << b<<newline;
}

l();

m(): () == {
	import from List 'a,b,c';
	print << [a,b,c] << newline;
}

m();
