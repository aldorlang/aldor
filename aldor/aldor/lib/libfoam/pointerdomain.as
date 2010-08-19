#include "runtimelib.as"

PointerDomain: Conditional with {
--	pointerDV: () -> DispatchVector;
	new:	   (() -> Domain) -> DomainRep;
	name: % -> DomainName;
	hash: % -> Hash;
	get: (d: %, pc: Domain, n1: Hash, n2: Hash, box: Box, skip: Bit) -> Box;
	inheritTo: (%, Domain, DomainRep -> Domain) -> Domain;
} == add {
	Rep ==> Union(init: ()->Domain, dom: Domain);
	DV  ==> DispatchVector;
	import from Rep;
	import from Pointer;

	new(fn: () -> Domain): DomainRep == [fn] pretend DomainRep;

	deref(d: %): Domain == {
		if rep(d) case init then
			rep(d).dom := rep(d).init();
		rep(d).dom;
	}

	name(d: %): DomainName == getName(deref d);
	
	hash(d: %): Hash == getHash!(deref d);

	get(d: %, pc: Domain, n1: Hash, n2: Hash, box: Box, skip: Bit): Box == 
		getExportInner!(deref d, pc, n1, n2, box, skip);

	inheritTo(d: %, d2: Domain, bf: DomainRep -> Domain): Domain == {
		import from Domain;
		if rep(d) case dom then
			inheritTo(rep(d).dom, d2)
		else {
			fn(): Domain == inheritTo(deref d, d2);
			--new(pointerDV(), new fn);
			never;
		}
	}

	--pdv: DispatchVector := Nil DispatchVector;

	-- Conditional operations.

	(d1: %) = (d2: %): Bit == ptr d1 = ptr d2;
	sample : % == Nil %;
	(p: TextWriter) << (d: %) : TextWriter == p;
	test (d: %) : Bit == not nil? ptr d;

	-- Originally these were defaults from BasicType etc
	(x: %) ~= (y: %): Boolean == not (x = y);
	(<<)(x: %)(p: TextWriter): TextWriter == p << x;
	(x: %) case (y: %): Boolean == x = y;
}

