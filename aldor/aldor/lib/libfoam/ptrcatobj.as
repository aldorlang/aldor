

#include "runtimelib.as"

--!! This unmitigated hack is to get around inliner limitation to functions.
--!!\begin{hack}
import {
	stdoutFile: () -> OutFile;
} from Foreign;

local filePutc(ofile: OutFile)(c: Character):() ==
	write!(ofile, c);

local filePuts(ofile: OutFile)(s: String, start: Int, limit: Int): () ==
	write!(ofile, s, start, limit);

local PRINT(): TextWriter == {
	((): (Character -> (), (String, Int, Int) -> (), ()->Int) +->
	 (filePutc stdoutFile(), filePuts stdoutFile(), ():Int+->0))
	pretend TextWriter;
}

local NL(): Character == {
	import from Machine;
	newline::Character
}

local ERROR(s: String): Exit == {
	fiRaiseException s;	
	never;
}


PointerCatObj: with {
	pointerCatDV: () -> CatDispatchVector;
	new:	      (()->CatObj) -> CatRep;
} == add {
	Rep ==> Union(init: () -> CatObj, cat: CatObj);
	DV ==> CatDispatchVector;
	import from Rep;

	pcdv: CatDispatchVector := Nil(CatDispatchVector);

	new(fn: () -> CatObj): CatRep == [fn] pretend CatRep;

	deref(c: %): CatObj == {
		if rep(c) case init then {
			DEBUG(PRINT() << "(Deref");
			rep(c).cat := rep(c).init();
			DEBUG(PRINT() << rep(c).cat pretend SingleInteger <<")"<<newline);
		}
		rep(c).cat;
	}

	name0(c: %): DomainName == name(deref c);
	-- maybe should be lazier
	build0(c: %, dom: Domain): CatObj == build(deref c, dom);
	
	get0(c: %, pc: Domain, n1: Hash, n2: Hash, box: Box): Box == 
		getDefault!(deref c, pc, n1, n2, box);
	
	hash0(c: %): Hash == hash(deref c);

	parentCount0(c: %): Int		== parentCount(deref c);
	parentGet0(c: %, n: Int): CatObj == getParent(deref c, n);

	pointerCatDV(): DV == {
		n0(x: %): DomainName == name0 x;
		b0(x: %, d: Domain): CatPtr == build0(x, d) pretend CatPtr;
		g0(x: %, d: Domain, h: Hash, h2: Hash, b: Box): Box == get0(x, d, h, h2, b);
		h0(x: %): Int == hash0 x;
		pc0(x: %): Int == parentCount0 x;
		pg0(x: %, i: Int): CatPtr == parentGet0(x, i) pretend CatPtr;
		dv := new(n0  pretend CatNamer,
				    b0 pretend CatBuilder,
				    g0   pretend CatGetter,
				    h0  pretend CatHasher,
				    pc0 pretend CatParentCounter,
				    pg0    pretend CatParentGetter);
		dv;
			
	}


}

