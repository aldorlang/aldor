-----------------------------------------------------------------------------
----
---- bpower.as: Binary powering
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "axllib"

+++ `BinaryPowering' provides efficient exponentiation in a general setting. 
+++ For mutable data-structures, an in-place method is obtained by giving
+++ a multiplication which updates its first argument.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1994
+++ Keywords: exponentiation, binary powering

BinaryPowering(
	T: Type,
	*: (T, T) -> T,
	E: with {
		0:	 %;
		1:	 %;
		integer: Literal -> %;
		length:  % -> SingleInteger;
		bit:	 (%, SingleInteger) -> Boolean;
		=:	 (%, %) -> Boolean;
		<:	 (%, %) -> Boolean;
		-:	 % -> %;
	}
)
: with {
	power: (T, T, E) -> T
		++ `power(u,x,n)' computes `u*x^n'.  
		++ If desired, `*' is permitted to update its first argument
		++ to contain the product.  In this case `x' is be modified,
		++ and `u' is updated to contain the final result.
}
== add {
	power(u: T, x: T, n: E): T == {
		(n < 0) =>
			error "power:  negative exponent";

		-- A few cases to help the optimizer.
		n = 0 => u;
		n = 1 => u*x;
		n = 2 => u*x*x;

		-- General code.
		i: SingleInteger := 0;
		l: SingleInteger := length n;
		repeat {
			if bit(n, i) then u := u * x;
			if i >= l then break;
			x := x * x;
			i := i + 1;
		}
		u
	}
}
