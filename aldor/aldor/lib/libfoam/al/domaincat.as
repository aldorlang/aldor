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


DomainCat: Category == with {

}
