
#include "foamlib"
import from RuntimeLib;
inline from RuntimeLib;

#if DebugGets
DEBUG(x) ==> x;
#else
DEBUG(x) ==> ();

#endif

macro {
	Ptr		== Pointer;
	Int		== SingleInteger;
	Bit		== Boolean;
	ptr x		== x @ % pretend Ptr;
	Nil S		== (nil$Ptr)() @ Ptr pretend S;
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
	DomGetter 	== (DomainRep, DomainPtr, Hash, Hash, Box, Bit) -> Box;
	DomHasher	== DomainRep -> Hash;
	DomNamer	== DomainRep -> DomainName;
	DomInheritTo    == (DomainRep, DomainPtr) -> DomainPtr;
}

macro {
      CatRep == Pointer;
      CatPtr == Pointer;
}

macro {
	CatRepInit S		== (S, Domain) -> ();
	CatBuilder		== (CatRep, Domain) -> CatPtr;
	CatGetter		== (CatRep, Domain, Hash, Hash, Box) -> Box;
	CatHasher		== CatRep -> Hash;
	CatParentCounter	== CatRep -> Int;
	CatParentGetter		== (CatRep, Int) -> CatPtr;
	CatNamer		== (CatRep) -> DomainName;
}


pcentPcentHash ==> 51482908;

-- Hash code type.
macro {
	Hash		== Int;
	wildcard	== -1;
}

import {
	fiRaiseException: String -> ();
} from Foreign;

