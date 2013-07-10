-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp
--> testrun -O -l axllib

#include "axllib.as"

macro Z == SingleInteger;
import from Z;

Foo: with {
	foo: (n:Z, a:Z, b:Z == 0) -> Z;
	apply: (n:Z, a:Z, b:Z == 0) -> Z;
} == add {
	apply(n:Z, a:Z, b:Z == 0):Z == foo(n, a, b);
	foo(n:Z, a:Z, b:Z == 0):Z == (n * a) + b;
}

import from Foo;

x:Z := 5;
y:Z := 2;
-- both statements should print 10
print << foo(x, y) << newline;
print << x(y) << newline;

