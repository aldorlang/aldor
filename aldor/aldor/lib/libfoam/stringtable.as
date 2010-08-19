
#include "foamlib"

macro {
	Ptr		== Pointer;
	Int		== SingleInteger;
	Bit		== Boolean;
	ptr x		== x @ % pretend Ptr;
	Nil S		== nil() @ Ptr pretend S;
	Nil?(S)(x)	== x @ S pretend Ptr = nil();
	Reserved	== Pointer;
	DomainRep       == Pointer;
	DomainPtr       == Pointer;
}

macro {
	ObjectTyper	== () -> Int;
	ObjDomain	== 0;
	ObjCategory	== 10;
}

macro {
	DomainFun S	== S -> (S, Hash) -> ();
	DomGetter 	== (DomainRep, Domain, Hash, Hash, Box, Bit) -> Box;
	DomHasher	== DomainRep -> Hash;
	DomNamer	== DomainRep -> DomainName;
	DomInheritTo    == (DomainRep, Domain) -> Domain;
}
-- Hash code type.
macro {
	Hash		== Int;
	wildcard	== -1;
}

-- hashCode of "%%"
pcentPcentHash ==> 51482908;

StringTable: with {
	addNames: (%, Array Hash, Array String)->();
	find: (%, Hash) -> String;
	new: () -> %;
} == add {
	import from List String, List Hash, Format;
	Rep ==> Record(names: List String, codes: List SingleInteger);
	import from Rep;

	new(): % == per [ ["%%"], [pcentPcentHash]];
	names(x: %): List String == rep(x).names;
	codes(x: %): List SingleInteger == rep(x).codes;

	addNames(t: %, a1: Array Hash, a2: Array String):() == {
		for code in a1 
		for name in a2 repeat {
			rep(t).codes := cons(code, codes t);
			rep(t).names := cons(name, names t);
		}
	}

	find(t: %, i: Hash): String == {
		for code in codes t
		for name in names t repeat {
			if code = i then return name
		}	
		x: String := copy "??:             ";
		end!(x, format(i, x, 3)+1);
		x
	}
}
