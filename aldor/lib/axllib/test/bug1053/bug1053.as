--* Received: from inf.ethz.ch (neptune.ethz.ch) by nags2.nag.co.uk (4.1/UK-2.1)
--* 	id AA27438; Tue, 19 Mar 96 17:28:00 GMT
--* Received: from ru7.inf.ethz.ch (bronstei@ru7.inf.ethz.ch [129.132.12.16]) by inf.ethz.ch (8.6.10/8.6.10) with ESMTP id SAA02846 for <ax-bugs@nag.co.uk>; Tue, 19 Mar 1996 18:22:51 +0100
--* From: Manuel Bronstein <bronstei@ch.ethz.inf>
--* Received: (bronstei@localhost) by ru7.inf.ethz.ch (8.6.8/8.6.6) id SAA07582 for ax-bugs@nag.co.uk; Tue, 19 Mar 1996 18:22:51 +0100
--* Date: Tue, 19 Mar 1996 18:22:51 +0100
--* Message-Id: <199603191722.SAA07582@ru7.inf.ethz.ch>
--* To: ax-bugs@uk.co.nag
--* Subject: [2] Another bad inlining problem

--@ Fixed  by: <Who> <Date>
--@ Tested by: <Name of new or existing file in test directory>
--@ Summary:   <Description of real problem and the fix>

-- Command line: axiomxl -q2 -fx primes.as
-- Version: 1.1.6
-- Original bug file name: primes.as

----------------------------- primes.as ----------------------------------
--
-- Looks like a bad inlining problem:
--
-- % axiomxl -q1 -fx primes.as
-- % primes
-- foo(2) = 2
--
-- % axiomxl -q2 -fx primes.as
-- % primes
-- Segmentation fault
--

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

