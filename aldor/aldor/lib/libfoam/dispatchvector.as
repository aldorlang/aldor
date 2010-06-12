
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
DispatchVector: Conditional with {
	new:		(DomNamer, DomGetter, DomHasher, DomInheritTo) -> %;
		++ new(get, hash) constructs a dispatch vector.

	getter:		% -> DomGetter;
		++ getter(dv) returns the getter function.

	hasher:		% -> DomHasher;
		++ hasher(dv) returns the hash code function.

	namer:		%->  DomNamer;
		++ namer(dv) returns the function giving the name of a domain
	tag: 		% -> Int;
	reserved:	% -> Reserved;
	inheriter:	% -> DomInheritTo;

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

	new(n: DomNamer, g: DomGetter, h: DomHasher, 
	    i: DomInheritTo): %
		== per [axiomxlDispatchTag, n, nil(), g, h, i];

	tag(dv: %)   : Int	 == rep(dv).tag;
	reserved(dv: %): Reserved == rep(dv).noname;
	getter(dv: %): DomGetter == rep(dv).getter;
	hasher(dv: %): DomHasher == rep(dv).hasher;
	namer(dv: %) : DomNamer  == rep(dv).namer;
	inheriter(dv: %) : DomInheritTo  == rep(dv).inheriter;

	-- Conditional operations.

	(d1: %) = (d2: %): Bit == ptr d1 = ptr d2;
	sample : % == Nil %;
	(p: TextWriter) << (d: %) : TextWriter == p;
	test (d: %) : Bit == not nil? ptr d;


	-- Originally these were defaults from BasicType etc
	(x: %) ~= (y: %): Boolean == not (x = y);
	(<<)(x: %)(p: TextWriter): TextWriter == p << x;
	hash(x: %): SingleInteger == (0$Machine)::SingleInteger;
	(x: %) case (y: %): Boolean == x = y;
}


