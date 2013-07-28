-----------------------------------------------------------------------------
----
---- sinteger.as: Single precision integer arithmetic.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "axllib"

+++ SingleInteger implements single-precision integers.  Typically 32 bits.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: single-precision integer

extend SingleInteger: Join(
	Logic,
	OrderedFinite,
	IntegerNumberSystem,
	Steppable,
	DenseStorageCategory
) with {
	^:	(%,%)-> %;
	mod_+:	(%,%,%)-> %;
	mod_-:	(%,%,%)-> %;
	mod_*:	(%,%,%)-> %;
	mod_/:  (%,%,%)-> %;
	mod_^:	(%,%,%)-> %;

	export from Segment %;
}
== add {
	Rep == BSInt;
	import from Machine;

	import { string: Literal -> %; } from String;
	import { formatSInt: BSInt -> String } from Foreign;

	0:   %	   == per 0;
	1:   %	   == per 1;
	min: %	   == per min;
	max: %	   == per max;

	#: Integer == (convert rep max - convert rep min + 1)::Integer;

	integer   (l:Literal): %  == per convert (l pretend BArr);
	zero?	  (i: %): Boolean == zero?     (rep i)::Boolean;
	negative? (i: %): Boolean == negative? (rep i)::Boolean;
	positive? (i: %): Boolean == positive? (rep i)::Boolean;
	even?	  (i: %): Boolean == even?     (rep i)::Boolean;
	odd?	  (i: %): Boolean == odd?      (rep i)::Boolean;
	single?   (i: %): Boolean == true;

	(i: %) =  (j: %): Boolean == (rep i =  rep j)::Boolean;
	(i: %) ~= (j: %): Boolean == (rep i ~= rep j)::Boolean;
	(i: %) <  (j: %): Boolean == (rep i <  rep j)::Boolean;
	(i: %) >  (j: %): Boolean == (rep j <  rep i)::Boolean;
	(i: %) <= (j: %): Boolean == (rep i <= rep j)::Boolean;
	(i: %) >= (j: %): Boolean == (rep j <= rep i)::Boolean;

	prev(i: %): %		  == per prev rep i;
	next(i: %): %		  == per next rep i;
	+ (i: %): %		  == i;
	- (i: %): %		  == per (- rep i);
	(i: %) + (j: %): %	  == per (rep i + rep j);
	(i: %) - (j: %): %	  == per (rep i - rep j);
	(i: %) * (j: %): %	  == per (rep i * rep j);

	coerce (i: SingleInteger): % == i;
	retract(i: %): SingleInteger == i;

	(x: %) ^ (n: Integer): %     == x^per(convert(n::BBInt));

	coerce(i: Integer): %        == per convert(i::BBInt);

	(i: %) quo (j: %): %	  == per (rep i quo rep j);
	(i: %) rem (j: %): %	  == per (rep i rem rep j);
	gcd(i: %, j: %): %	  == per gcd(rep i, rep j);

	length(i: %): SingleInteger == per length rep i;
	shift (i: %, n: SingleInteger): % == {
		n > 0 => per shiftUp  (rep i, rep(+n));
		n < 0 => per shiftDown(rep i, rep(-n));
		i
	}
	bit(i: %, n: SingleInteger): Boolean == bit(rep i, rep n)::Boolean;

	~ (i: %): %	 	  == per (~ rep i);
	(i: %) /\ (j: %): %	  == per (rep i /\ rep j);
	(i: %) \/ (j: %): %	  == per (rep i \/ rep j);
	xor(i: %,  j: %): %	  == per xor(rep i, rep j);

	-- All the mod operations are assuming n > 0.
	-- and also that 0 <= i,j < n
	(i: %) mod (n: %): %	== {
		assert( n>0);
		k := i rem n;
		if k < 0 then k := k + n;
		k;
	}
	mod_+(i:%, j:%, n:%): %	 == {
		assert(n>0);
		assert(i>=0 and i<n);
		assert(j>=0 and j<n);
		-- tacky (but avoids overflow problems)
		i := i - n;
		k := i + j;
		if k < 0 then k := k + n;
		k;
	}
	mod_-(i:%, j:%, n:%): %	 == {
		assert(n>0);
		assert(i>=0 and i<n);
		assert(j>=0 and j<n);
		k := i - j;
		if k < 0 then k := k + n;
		k;
	}
	mod_*(i:%, j:%, n:%): %	 == {
		assert(n>0);
		assert(i>=0 and i<n);
		assert(j>=0 and j<n);
		i = 1 => j;
		j = 1 => i;
		-- Tacky stuff (but we want this to const fold on all
		-- platforms).
		-- XXX change when we have fastDoubleDiv$Machine
		HalfWord ==> 32767;
		(n < HalfWord) or 
			(i < HalfWord and j < HalfWord) => (i * j) mod n;
		(nh, nl) := double_*(i pretend Word, j pretend Word);
		(qh, ql, rm) := doubleDivide(nh, nl, n pretend Word);
		rm pretend %;
	}
	mod_/(i: %, j: %, n: %): % == {
		assert(n>0);
		assert(i>=0 and i<n);
		assert(j>=0 and j<n);
		local c0, d0, c1, d1: %;
		(c0, d0) := (j, n);
		(c1, d1) := (1, 0);
		while not zero? d0 repeat {
			q := c0 quo d0;
			(c0, d0) := (d0, c0 - q*d0);
			(c1, d1) := (d1, c1 - q*d1)
		}
		if c0 ~= 1 then error "inverse does not exist";
		if c1 < 0  then c1 := c1 + n;
		mod_*(i, c1, n);
	}
	mod_^(i:%, j:%, n:%): %	 == {
		assert(n>0);
		assert(i>=0 and i<n);
		j < 0  => mod_^(mod_/(1, i, n), -j, n);
		j = 0  => 1;
		odd? j => mod_*(i, mod_^(i, j-1, n), n);
		mod_^(mod_*(i,i,n), j quo 2, n)
	}

	divide(i: %, j: %): (%,%) ==  {
		(a,b) := divide(rep i, rep j);
		(per a, per b);
	}

	(x: %) ^ (n: %): % == {
		n = 0 => 1;
		n = 1 => x;
		n = 2 => x*x;

		y: % := 1;
		i: % := 0;
		l: % := length n;
		while i < l repeat {
			if bit(n, i) then y := y * x;
			x := x * x;
			i := i + 1;
		}
		y
	}

	stepsBetween(x: %, y: %, step: %): SingleInteger ==
		(y-x) quo step + 1;

	(w: TextWriter) << (i: %): TextWriter ==
		{ write!(w, formatSInt rep i); w }

	-- avoids bug in opt1.as
	max(x: %, y: %): % == if x > y then x else y;
	min(x: %, y: %): % == if x < y then x else y;

	hash(x: %): SingleInteger == x;


#if ExplicitPackedExports
	-- The compiler can generate these exports automatically
	-- but could not always inline them unless they were defined
	-- explicitly.
	import
	{
			ArrNew: (Rep, BSInt) -> BArr;
			ArrElt: (BArr, BSInt) -> Rep;
			ArrSet: (BArr, BSInt, Rep) -> Rep;
	} from Builtin;

	PackedArrayNew(n:BSInt):BArr ==
		ArrNew(0, n);

	PackedArrayGet(a:BArr, i:BSInt):% ==
		per ArrElt(a, i);

	PackedArraySet(a:BArr, i:BSInt, e:%):% ==
		per ArrSet(a, i, rep e);

	PackedRecordGet(p:BPtr):% ==
		per ArrElt(p pretend BArr, 0);

	PackedRecordSet(p:BPtr, e:%):% ==
		per ArrSet(p pretend BArr, 0, rep e);

	PackedRepSize():BSInt == {
		import {
			TypeSInt: () -> BSInt;
			RawRepSize: BSInt -> BSInt;
		} from Builtin;

		RawRepSize(TypeSInt());
	}
#endif
}
