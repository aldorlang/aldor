-----------------------------------------------------------------------------
----
---- partial.as:  A type which allows values or soft failures to be returned.
----
-----------------------------------------------------------------------------
---- Copyright (c) 1990-2007 Aldor Software Organization Ltd (Aldor.org).
-----------------------------------------------------------------------------

#include "foamlib.as"

Partial(T: Type): with {
	failed : %;
	failed?: % -> Boolean;
	coerce:  T -> %;
	coerce:  % -> T;
	retract: % -> T;
	bracket: T -> %;
	-- following 2 are hacks to support old syntax for Union(%, "failed")
	case:   (%, String) -> Boolean;
	case:   (%, Type) -> Boolean;
}
== add {
	macro Rep == Pointer;
	macro Rec == Record(val:T);
	import from Rep, Rec, String;

	failed? (p: %): Boolean          == nil? rep p;
	failed: %                        == per nil();
	coerce(v: T): %                  == [v]$Rec pretend %;
	[v: T]: %                        == v::%;
	(p: %) case (f: String): Boolean == failed? p;
	(p: %) case (S: Type): Boolean   == not failed? p;
	coerce(p: %): T                  == retract p;
	retract(p: %): T == {
		failed? p => error "cannot retract failed";
		(p pretend Rec).val
	}
}
