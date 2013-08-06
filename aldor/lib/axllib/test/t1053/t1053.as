-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testrun  -O -l axllib
--> testcomp -O
#include "axllib"

BinarySearch(R:Join(EuclideanDomain, Order), S:Order): with {
	binarySearch: (S, R -> S, R, R) -> (Boolean, R);
} == add {
	binarySearch(s:S, f:R -> S, a:R, b:R):(Boolean, R) == {
		fa := f a; fb := f b;
		s < fa => (false, a - 1);
		s > fb => (false, b + 1);
		s = fa => (true, a);
		s = fb => (true, b);
		two:R := 1 + 1;
		while (m := (a + b) quo two) > a and m < b repeat {
			fm := f m;
			s = fm => return (true, m);
			if s < fm then b := m; else a := m;
		}
		(false, a);
	}
}

macro Z	== SingleInteger;

SmallPrimes: with { primeIndex: Z -> Z } == add {
	import from Z, Array Z;

	smallprimes:Array(Z) == [3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59];

	search(n:Z):(Boolean, Z) == {
		import from BinarySearch(Z, Z);
		binarySearch(n, (m:Z):Z +-> smallprimes.m, 1, 16);
	}

	primeIndex(prime:Z):Z == {
		(found?, index) := search prime;
		found? => index;
		0;
	}
}

Foo: with { foo: Z -> Z } == add {
	foo(n:Z):Z == {
		import from SmallPrimes;
		n < 2 or primeIndex(n) ~= 0 => 0;
		n;
	}
}

import from Z, Foo;
print << "foo(2) = " << foo 2 << newline;
print << "foo(10) = " << foo 11 << newline;
print << "foo(16) = " << foo 16 << newline;

