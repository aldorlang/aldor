
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
	addNames: (Array Hash, Array String)->();
	find: Hash->String;
} == add {
	import from List String, List Hash, Format;
	names: List String := empty();
	codes: List SingleInteger := empty();

	addNames(a1: Array Hash, a2: Array String):() == {
		free names, codes;
		if empty?(names) then {
			names := cons("%%", names);
			codes := cons(pcentPcentHash, codes);
		}
		for code in a1 
		for name in a2 repeat {
			codes := cons(code, codes);
			names := cons(name, names);
		}
	}

	find(i: Hash): String == {
		for code in codes
		for name in names repeat {
			if code = i then return name
		}	
		x: String := copy "??:             ";
		end!(x, format(i, x, 3)+1);
		x
	}
}
