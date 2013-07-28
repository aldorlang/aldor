-----------------------------------------------------------------------------
----
---- byte.as: Extend the Byte data with basic operations.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "axllib"

+++ Byte implements single byte integers. Typically 8 bits.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: byte, single byte integer

extend Byte: Join(
	Logic,
	OrderedFinite,
	OrderedRing,
	DenseStorageCategory
) with == add {
	Rep == BByte;
	import from Machine;

	import { string: Literal -> % } from String;
	import { formatSInt: BSInt -> String } from Foreign;

	0:   %			== per 0;
	1:   %			== per 1;
	min: %			== per min;
	max: %			== per max;

	#: Integer		== {
		import from SingleInteger;
		(convert rep max - convert rep min + 1)::SingleInteger::Integer
	}

	coerce(n: SingleInteger): % == error "coerce$Byte not implemented";
	coerce(n: Integer): %       == error "coerce$Byte not implemented";

	+ (a: %): %		== a;
	- (a: %): %		== error "-$Byte not implemented";
	(a: %) + (b: %): %	== error "+$Byte not implemented";
	(a: %) - (b: %): %	== a + (-b);
	(a: %) * (b: %): %	== error "*$Byte not implemented";
	(a: %) ^ (n: SingleInteger): % == error "^$Byte not implemented";
	(a: %) ^ (n: Integer): % == error "^$Byte not implemented";

	(a: %) =  (b: %): Boolean== error "=$Byte not implemented";
	(a: %) >  (b: %): Boolean== error ">$Byte not implemented";

	~ (a: %): %		== error "~$Byte not implemented";
	(a: %) /\ (b: %): %	== error "/\$Byte not implemented";
	(a: %) \/ (b: %): %	== error "\/$Byte not implemented";

	(w: TextWriter) << (b: %): TextWriter ==
		{ write!(w, formatSInt convert rep b); w }
}
