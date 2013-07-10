-----------------------------------------------------------------------------
----
---- ref.as: Extension of Ref(T) with ops for explicit manipulation.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "axllib"

macro RBase == ( ()->T, T->T);
macro RRep  == () -> RBase;

+++ Ref(T) is a type which allows T lvalues to be aliased
+++
+++ Author: AXIOM-XL library
+++ Date Created: 1998
+++ Keywords: ref, deref, Ref

extend Ref(T: Type): with {
	deref: % -> T;
		++ deref(R) extracts the value from the object which
		++ is being referenced by R.

	update!: (%, T) -> T;
		++ update!(R, v) assigns the value v to the object
		++ being referenced by R.

	export from T;
}
== add {
	Rep ==> RRep;

	deref(r:%):T ==
	{
		(getter, setter) := (rep r)();
		z := getter();
		z;
	}


	update!(r:%, v:T):T ==
	{
		(getter, setter) := (rep r)();
		z := setter(v);
		z;
	}
}

