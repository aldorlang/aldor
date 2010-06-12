
#include "foamlib"
import from RuntimeLib;
inline from RuntimeLib;

macro {
	Ptr		== Pointer;
	Int		== SingleInteger;
	Bit		== Boolean;
	ptr x		== x @ % pretend Ptr;
	Nil S		== nil() @ Ptr pretend S;
	Nil?(S)(x)	== x @ S pretend Ptr = nil();
	Reserved	== Pointer;
	DomainRep       == Pointer;
	DomainPtr	== Pointer;
}

macro {
	ObjectTyper	== () -> Int;
	ObjDomain	== 0;
	ObjCategory	== 10;
}

macro {
	DomainFun S	== S -> (S, Hash) -> ();
	DomGetter 	== (DomainRep, DomainPtr, Hash, Hash, Box, Bit) -> Box;
	DomHasher	== DomainRep -> Hash;
	DomNamer	== DomainRep -> DomainName;
	DomInheritTo    == (DomainRep, DomainPtr) -> DomainPtr;
}
-- Hash code type.
macro {
	Hash		== Int;
	wildcard	== -1;
}

+++ Structure containing a domain's protocol for getting exports and
+++ producing hash codes.  This is in a separate structure to accomodate
+++ mixed runtime environments.
xDispatchVector: with {
	x: () -> DomainName;
	y: () -> Integer;
--	namer:		%->  DomNamer;
--		++ namer(dv) returns the function giving the name of a domain

}
== add {
	Rep ==> Record(tag: Int,
		       namer:  DomNamer,
		       noname:  Reserved,
                       getter: DomGetter,
		       hasher: DomHasher,
		       inheriter: DomInheritTo);

	import from Rep;

        axiomxlDispatchTag ==> 0;
--	namer(dv: %) : DomNamer  == rep(dv).namer;
	x(): DomainName == never;
	y(): Integer == never;
}
