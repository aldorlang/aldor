-----------------------------------------------------------------------------
----
---- object.as: Dynamic objects (OO).
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "axllib"

+++ Object implements dynamic objects, pairing data values with
+++ associated domains.
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1992-94
+++ Keywords: object

Object(C: Category): with {
	object:		(T: C, T) -> %;
	avail:		% -> (T: C, T);
}
== add {
	Rep == Record(T: C, val: T);
	import from Rep;

	object	(T: C, t: T) : %	== per [T, t];
	avail	(ob: %) : (T: C, T)	== explode rep ob;
}
