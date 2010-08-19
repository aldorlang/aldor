
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

+++ CatRep defines the run-time representation of axiomxl categories.
AldorCatRep: Conditional with {
	new:		(CatRepInit %, ()->Hash, ()->DomainName) -> %;
		++ new(fun) creates a new category.

	prepare!:	% -> ();
		++ prepare!(cat) forces a category to fully instantiate.

	addExports!:	(%, Array Hash, Array Hash, Array Value) -> ();
		++ addExports!(cat, names, types, exports)
		++ sets the exports fields of a category.

	addHashFn!:	(%, ()->Hash) -> ();
	addParents!:	(%, Array CatObj) -> ();
		++ addParents!(cat, pars) set the parent field of a category.
	addNameFn!: 	(%, ()->DomainName) -> ();
		++ addName! sets the name of an category.
		
	name0: % -> DomainName;
	build0: (cdv: ()->CatDispatchVector, cat: %, d: Domain) -> CatObj;
        hash0: % -> Hash;
	get0: (%, Domain, Hash, Hash, Box) -> Box;
	parentCount0: % -> Int;
	parentGet0: (%, Int) -> CatObj;


	nameDummy: % -> DomainName;
        hashDummy: % -> Hash;
	getDummy: (%, Domain, Hash, Hash, Box) -> Box;
	parentCountDummy: % -> Int;
	parentGetDummy: (%, Int) -> CatObj;
	buildDummy: (cat: %, dom: Domain) -> CatObj;
}
== add {
	--- this representation is a compromise between 
	--- non-instantiated categories and built categories.
	--- it is quite possible to split the two as a 
	--- non-instantiated category should never be asked for exports
	--- but could be asked for a hashcode.
	Rep1 ==> Ptr;
	Per1 ==> CatRepInit %;
	rep1 x ==> x @ Per1 pretend Rep1;
	per1 x ==> x @ Rep1 pretend Per1;

	Rep ==> Record (buildFn:	Rep1,
			hashFn:		()->Hash,
			dc:		PtrCache,
			domain:		Domain,
			parents:	Array CatObj,
			nparents:	Int,
			names:		Array Hash,
			types:		Array Hash,
			exports:	Array Value,
			nameFn:		()->DomainName);

	import from Rep, String;

	local serialThis: Int := 1;
	local get1;

	categoryStdName(): DomainName == noName();

	new(fn: Per1, hfn: ()->Hash, nFn: ()->DomainName): % == {
		import from PtrCache;
		per [rep1 fn, hfn, newCache(),
		     Nil Domain, Nil Array CatObj, 0,
		     Nil Array Hash, Nil Array Hash,
		     Nil Array Value, nFn ];
	}

	hashFn	(x: %): ()->Hash	== rep(x).hashFn;
	names	(x: %): Array Hash	== rep(x).names;
	types	(x: %): Array Hash	== rep(x).types;
	exports	(x: %): Array Value	== rep(x).exports;
	parents	(x: %): Array CatObj	== rep(x).parents;
	nparents(x: %): Int		== rep(x).nparents;
	dom	(x: %): Domain		== rep(x).domain;
	cache	(x: %): PtrCache	== rep(x).dc;
	nameFn	(x: %): ()->DomainName	== rep(x).nameFn;
	builder	(x: %): Per1		== per1(rep(x).buildFn);

	prepare! (cat: %): () ==
		if (Nil?(Array Hash)(names(cat))) then {
			builder(cat)(cat, dom(cat));
		}

	addExports!(cat: %, nams: Array Hash, typs: Array Hash,
		    expts: Array Value): () == {
		rep(cat).names    := nams;
		rep(cat).types    := typs;
		rep(cat).exports  := expts;
	}

	addParents!(cat: %, prnts: Array CatObj): () == {
		dom0 := dom(cat);
		i := 0;
		for p: CatObj in prnts repeat {
			if p then {
				i := i + 1;
				prnts.i := build(p, dom0);
			}
		}
		rep(cat).parents  := prnts;
		rep(cat).nparents := i;
	}

	addHashFn!(cat: %, fn: ()->Hash): () == {
		rep(cat).hashFn := fn;
	}
	
	addNameFn!(cat: %, nmfn: ()->DomainName): () == {
		rep(cat).nameFn := nmfn;
	}

	-- Axiomxl category dispatch vector functions.	

	name0(cat: %): DomainName == {
		nameFn(cat)();
	}

	build0(cdv: ()->CatDispatchVector, cat: %, d: Domain): CatObj == {
		import from PtrCache;
		DEBUG(Nil?(PtrCache)(cache(cat)) => ERROR "oops");
		DEBUG(PRINT() << "(Building: ");
		key: BasicTuple := tuple(d pretend Pointer);
		DEBUG(PRINT() << name0 cat
			      << " [" << getName d << "]");
		(v, flag) := getEntry(cache(cat), key);
		flag => {
			r   := v pretend CatObj;
			DEBUG(PRINT() << " cached)" << NL());
			r
		}
		newCat := new(builder(cat), hashFn(cat), nameFn(cat));
		rep(newCat).domain  := d;
		rep(newCat).dc      := Nil PtrCache;
		cobj: CatObj := new(cdv(), newCat pretend CatRep);
		DEBUG(PRINT() << " new)" << NL());
		addEntry(cache(cat), key, cobj pretend Ptr);
		cobj
	}

	get0(cat: %, pcent: Domain, name: Hash, type: Hash, box: Box): Box == {
		import from String, TextWriter, SingleInteger, StringTable;
		DEBUG(PRINT() << NL() << "(Get Cat: " << find name << " " << type);
		if (name = pcentPcentHash) then {
			type ~= hash0(cat) => { 	
				DEBUG(PRINT() << " " << name0 cat << " "); 
				DEBUG(PRINT() << "Failed)" << NL()); 
				return nullBox();
			}
			DEBUG(PRINT() << " Done)" << NL()); 
			setVal!(box, pcent pretend Value);
			return box;
		}
		val := get1(cat, pcent, name, type, box);
		DEBUG(PRINT() << " " << name0 cat << " [" << domainName(dom cat) << "])" << NL());
		val;
	}
	get1(cat: %, pcent: Domain, name: Hash, type: Hash, box: Box): Box == {
		prepare!(cat);
		for expName  in names(cat)
		for expType  in types(cat)
		for expValue in exports(cat)
		repeat {
			name=expName and (type=expType or type=wildcard) => {
				setVal!(box, expValue);
				return box;
			}
		}
		nullBox();
	}

	hash0(cat: %): Hash == hashFn(cat)();

	parentCount0(cat: %): Int == {
		prepare!(cat);
		nparents(cat);
	}

	parentGet0(cat: %, i: Int): CatObj == {
		prepare!(cat);
		parents(cat).i;
	}

	nameDummy(cat: %): DomainName == noName();
	buildDummy(cat: %, dom: Domain): CatObj == 
		ERROR "build on incomplete category";
	
	getDummy(cat: %, pcent: Domain, name: Hash,
	         type: Hash, box: Box): Box == 
		ERROR "get on incomplete category";
	
	hashDummy(cat: %): Hash ==
		ERROR "hash on incomplete category";

	parentGetDummy(cat: %, i: Int): CatObj == 
		ERROR "parent get on incomplete category";

	parentCountDummy(cat: %): Int ==
		ERROR "parent count on incomplete category";


	-- Conditional operations.

	(d1: %) = (d2: %): Bit == ptr d1 = ptr d2;
	sample : % == Nil %;
	(p: TextWriter) << (d: %): TextWriter == p;
	test (d: %) : Bit == not nil? ptr d;


	-- Originally these were defaults from BasicType etc
	(x: %) ~= (y: %): Boolean == not (x = y);
	(<<)(x: %)(p: TextWriter): TextWriter == p << x;
	hash(x: %): SingleInteger == (0$Machine)::SingleInteger;
	(x: %) case (y: %): Boolean == x = y;
}
