-----------------------------------------------------------------------------
----
---- hint.as: Half-word sized integers.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "axllib"

+++ HalfInteger implements half-precision integers.  Typically 16 bits.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: half-precision integer

extend HalfInteger: Join(
	Logic,
	OrderedFinite,
	OrderedRing,
	DenseStorageCategory
) with == add {
	Rep == BHInt;
	import from Machine;

	import { string: Literal -> % } from String;
	import { formatSInt: BSInt -> String } from Foreign;

	0:   %			== per 0;
	1:   %			== per 1;
	min: %			== per min;
	max: %			== per max;

	#: Integer == {
		import from SingleInteger;
		(convert rep max - convert rep min + 1)::SingleInteger::Integer
	}
	local sint(x:%):BSInt == convert(rep x);
	local hint(x:BSInt):% == per(convert x);

	+ (a: %): %		== a;
	- (a: %): %		== hint (- sint a);
	(a: %) + (b: %): %	== hint( sint a + sint b);
	(a: %) - (b: %): %	== hint( sint a - sint b);
	(a: %) * (b: %): %	== hint( sint a * sint b);

	(a: %) = (b: %): Boolean== (sint a = sint b)::Boolean;
	(a: %) > (b: %): Boolean== ((sint b) < (sint a))::Boolean;

	~ (a: %): %		== hint( ~ sint a);
	(a: %) /\ (b: %): %	== hint(sint a /\ sint b);
	(a: %) \/ (b: %): %	== hint(sint a \/ sint b);

	coerce(n: SingleInteger):%==hint (coerce n);
	coerce(n: Integer): %     ==hint (convert coerce n);

	(a: %) ^ (n:Integer): %       == hint  coerce ((coerce sint a)^n) ;

	(w: TextWriter) << (h: %): TextWriter ==
		{ write!(w, formatSInt convert rep h); w }
}
