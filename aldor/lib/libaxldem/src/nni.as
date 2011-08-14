#include "axllib.as"
--#library DemoLib "axldem"
import from DemoLib;

NonNegativeInteger: Join(Monoid, OrderedAbelianMonoidSup) with {
	subtractIfCan: (%, %) -> Partial %;
		++ subtractIfCan(a,b) returns the difference when a >= b.
	exquo: (%,%) -> Partial %;
		++ exquo(a,b) returns the quotient of `a' and b, or failed
		++ if b is zero or `a' rem b is zero.
        coerce: % -> Integer;
                ++ coerce(n) converts a NonNegativeInteger to an Integer.
        retract: Integer -> %;
                ++ retract(i) tries to convert an Integer to a NonNegativeInteger
                ++ error if i < 0.
	gcd:    (%, %) -> %;		++ Greatest commond divisor.
	quo:    (%, %) -> %;		++ Quotient leaving remainder.
	rem:    (%, %) -> %;		++ Remainder.
	divide: (%, %) -> (%,%); 	++ Quotient-remainder pair.

}
== Integer add {
	Rep ==> Integer;
	import from Rep;

	coerce(x: %): Integer ==
		rep(x);
	retract(x: Integer): % == {
		x < 0 => error "invalid non negative integer";
		per x
	}
	sup(x: %, y: %): % ==
		per max(rep x,rep y);
	sum(x: %): SingleInteger ==
		error "sum$NonNegativeInteger not implemented";
	nonNegative?(x: %): Boolean  == true;
	exquo(x: %, y: %): Partial(%) == {
		(q, r) := divide(rep(x), rep(y));
		r = 0 => per(q)::Partial(%);
		failed
	}
	subtractIfCan(x: %, y: %): Partial(%) == {
		c:Integer := rep(x) - rep(y);
		c < 0 => failed;
		per(c)::Partial(%)
	}
}
