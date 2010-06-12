


#include "foamlib"
import from RuntimeLib;
inline from RuntimeLib;

macro {
	Ptr		== Pointer;
	Int		== SingleInteger;
	Bit		== Boolean;
	ptr x		== x @ % pretend Ptr;
	Nil S		== nil$Ptr() @ Ptr pretend S;
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

PointerDomain: with {
	pointerDV: () -> DispatchVector;
	new:	   (() -> Domain) -> DomainRep;
} == add {
	Rep ==> Union(init: ()->Domain, dom: Domain);
	DV  ==> DispatchVector;
	import from Rep;
	pdv: DispatchVector := Nil DispatchVector;
	new(fn: () -> Domain): DomainRep == [fn] pretend DomainRep;

	deref(d: %): Domain == {
		if rep(d) case init then
			rep(d).dom := rep(d).init();
		rep(d).dom;
	}

	name(d: %): DomainName == 
		domainName(deref d);
	
	domHash(d: %): Hash == domainHash!(deref d);

	get(d: %, pc: Domain, n1: Hash, n2: Hash, box: Box, skip: Bit): Box == 
		getExportInner!(deref d, pc, n1, n2, box, skip);

	inheritTo(d: %, d2: Domain): Domain == {
		import from Domain;
		if rep(d) case dom then
			inheritTo(rep(d).dom, d2)
		else {
			fn(): Domain == inheritTo(deref d, d2);
			rtLazyDomFrFn(fn);
		}
	}

	pointerDV() : DV == {
		free pdv: DV;
		not pdv => pdv := new(name@(%->DomainName) pretend DomNamer,
				      get pretend DomGetter,
				      domHash@(%->Hash) pretend DomHasher,
				      inheritTo@((%, Domain)->Domain)
						  pretend DomInheritTo
					);
		pdv;
	}

}

