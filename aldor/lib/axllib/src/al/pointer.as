-----------------------------------------------------------------------------
----
---- pointer.as: Extend the Pointer type with basic operations.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "axllib"

+++ Pointer is the type of pointers to opaque objects.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: pointer

extend Pointer: Conditional with {
	nil:	 %;
  	nil?:	 % -> Boolean;

	coerce:	 BPtr -> %;
	coerce:	 % -> BPtr;
}
== add {
	Rep == BPtr;
	import from Machine;
	import { formatSInt: BSInt -> String } from Foreign;

	sample: %                == per nil;

	nil: %		         == per nil;
  	nil?(p: %): Boolean      == nil?(rep p)::Boolean;
	(p: %) = (q: %): Boolean == (rep p = rep q)::Boolean;
	(w: TextWriter) << (p: %): TextWriter ==
		{ write!(w, formatSInt convert rep p); w }

	coerce(p: BPtr): % == per p;
	coerce(p: %): BPtr == rep p;

	test (p: %) : Boolean == not nil? p;
}
