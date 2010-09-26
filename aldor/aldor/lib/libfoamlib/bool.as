-----------------------------------------------------------------------------
----
---- boolean.as: Extend the Boolean type with logical operations.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "foamlib"

+++ The Boolean data type supports logical operations.
+++ Both arguments of the binary operations are always evaluated.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: logical operation

extend Boolean: Join(
	Conditional,
	Logic,
	OrderedFinite
) with {
	false: %;	++ false is the constant representing logical falsity.
	true:  %;	++ true is the constant representing logical truth.
}
== add {
	-- Rep ==> BBool;
	Rep == BBool;
	import from Machine;

	(a: %) =  (b: %): Boolean == per (rep a =  rep b);
	(a: %) ~= (b: %): Boolean == per (rep a ~= rep b);
	(a: %) >  (b: %): Boolean ==  a /\ ~b;
	(a: %) <  (b: %): Boolean == ~a /\  b;
	(a: %) <= (b: %): Boolean == ~a \/  b;
	(a: %) >= (b: %): Boolean ==  a \/ ~b;

	#: Integer	     == 2;
	min: %		     == per false;
	max: %		     == per true;

	false: %	     == per false;
	true:  %	     == per true;
	sample: %            == per true;

	~(a: %): %	     == per (~ rep a);
	(a: %) /\ (b: %): %  == per (rep a /\ rep b);
	(a: %) \/ (b: %): %  == per (rep a \/ rep b);

	max(a: %, b: %): %   == a \/ b;
	min(a: %, b: %): %   == a /\ b;

	test(b: %): Boolean  == b;

	(p: TextWriter) << (b: %): TextWriter == {
		import from String;
		p << (if b then "true" else "false");
	}


	-- Originally these were defaults from BasicType
	hash(x: %): SingleInteger == (0$Machine)::SingleInteger;
	(<<)(x: %)(p: TextWriter): TextWriter == p << x;
	(x: %) case (y: %): Boolean == x = y;
	xor(x: %, y: %): % == (x /\ ~y) \/ (~x /\ y);
}
