-----------------------------------------------------------------------------
----
---- integer.as: Arbitrary precision (big) integer arithmetic.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "axllib"

+++ Integer implements infinite precision integers.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: infinite precision integer

extend Integer: Join(Steppable, IntegerNumberSystem) with {
	export from Segment %;
}
== add {
	Rep == BBInt;
	SI  ==> SingleInteger;
	import from Machine;

	import { formatBInt: BBInt -> String } from Foreign;

	0: %			  == per 0;
	1: %			  == per 1;

	zero?	  (i: %): Boolean == zero?    (rep i)::Boolean;
	negative? (i: %): Boolean == negative?(rep i)::Boolean;
	positive? (i: %): Boolean == positive?(rep i)::Boolean;
	even?	  (i: %): Boolean == even?    (rep i)::Boolean;
	odd?	  (i: %): Boolean == odd?     (rep i)::Boolean;
	single?	  (i: %): Boolean == single?  (rep i)::Boolean;
	(i: %) =  (j: %): Boolean == (rep i =  rep j)::Boolean;
	(i: %) ~= (j: %): Boolean == (rep i ~= rep j)::Boolean;
	(i: %) <  (j: %): Boolean == (rep i <  rep j)::Boolean;
	(i: %) >  (j: %): Boolean == (rep j <  rep i)::Boolean;
	(i: %) <= (j: %): Boolean == (rep i <= rep j)::Boolean;
	(i: %) >= (j: %): Boolean == (rep j <= rep i)::Boolean;
	prev(i: %): %		  == per prev (rep i);
	next(i: %): %		  == per next (rep i);
	+ (i: %): %		  == i;
	- (i: %): %		  == per (- rep i);
	(i: %) + (j: %): %	  == per (rep i + rep j);
	(i: %) - (j: %): %	  == per (rep i - rep j);
	(i: %) * (j: %): %	  == per (rep i * rep j);
	(i: %) ^ (j: %): %	  == {
		--j < 0 => except RangeError(%, j, "Division by zero");
		per (rep i ^ rep j);
	}
	(i: %) mod (j: %): %	  == {
		-- Assume j > 0
		k := i rem j;
		if k < 0 then k := k + j;
		k
	}
	(i: %) quo (j: %): %	  == {
		--zero? j => except RangeError(%, 0, "Division by zero");
		per (rep i  quo  rep j);
	}
	(i: %) rem (j: %): %	  == {
		--zero? j => except RangeError(%, 0, "Division by zero");
		per (rep i  rem  rep j);
	}

	gcd(i: %, j: %): %	  == per gcd (rep i, rep j);

	length(i: %): SI	     == length(rep i)::SingleInteger;
	bit   (i: %, n: SI): Boolean == bit(rep i, n::BSInt)::Boolean;
	
	shift (i: %, n: SI): % == {
		n > 0 => per shiftUp  (rep i, (+n)::BSInt);
		n < 0 => per shiftDown(rep i, (-n)::BSInt);
		i
	}

	divide(i: %, j: %): (%,%) == {
		--zero? j => except RangeError(%, 0, "Division by zero");
		(a,b) := divide(rep i, rep j);
		(per a, per b)
	}

	(w: TextWriter) << (i: %): TextWriter ==
		{ write!(w, formatBInt rep i); w }

	stepsBetween(x: %, y: %, step: %): SingleInteger == 
		retract((y-x) quo step + 1)@SingleInteger;

	coerce(i: Integer): %          == i pretend %;
	coerce(i: SingleInteger): %    == per(convert(i::BSInt));

	retract(i: %): SingleInteger   == (convert rep i)::SingleInteger;

	hash(i: %): SingleInteger == {
		-- treat the number as a string of characters,
		-- and use the same algorithm.
		local combine(h: SI, c: SI): SI == {
			h := xor(h, shift(h, 8));
			h := h + c + 200041;
			h := h /\ 1073741823;
		}
		local x: SI;
		h: SingleInteger := 0;
		while (not zero? i) repeat {
			x := (i mod 256)::SingleInteger;
			h := combine(h, x);
			i := shift(i, -8);
		}
		h
	}
}
