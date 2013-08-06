-- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
--> testcomp

#include "axllib"

import from Pointer;

D : Conditional with == add {
	Rep ==> Pointer;

	-- Do not cover meanings from Pointer.
	-- Do cover meanings from D.

	sample: % == per nil;
	(x: %) = (y: %) : Boolean == rep x = rep y;
	test (x: %) : Boolean == test rep x;
	(p: TextWriter) << (x: %) : TextWriter == p << rep x;
}

-- The meanings introduced by this import are now equal
-- to the export meanings in the add body of D.

import from D;
