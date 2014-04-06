
#include "runtimelib.as"

--!! This unmitigated hack is to get around inliner limitation to functions.
--!!\begin{hack}
import {
	stdoutFile: () -> OutFile;
} from Foreign;

import {
	puts:     	(String) -> ();
} from Foreign;

--import {
--	fiCounter:     	() -> Int;
--} from Foreign C "foam__c.h";

local fiCounter(): Int == 0;

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

output(d, txt) ==> { puts(concat(txt, " ", coerce(rep(d).id))$String); }
output3(d, txt, suffix) ==> puts(concat(txt, " ", coerce(rep(d).id), " ", suffix)$String);

local addGetCache(cache: PtrCache, key: BasicTuple, box: Box): () == {
	-- must copy the box...
	val := if not box then Nil Ptr else (new value box) pretend Ptr;
	addEntry(cache, key, val);
}


local checkGetCache(cache: PtrCache, pcent: Domain, 
	      name: Hash, type: Hash, skip: Bit): 
			(Box, Bit, BasicTuple) == {
	import from BasicTuple;
	key: BasicTuple := tuple(pcent pretend Ptr, name pretend Ptr, 
	  			 type pretend Ptr,  skip pretend Ptr);
	(val, flag) := getEntry(cache, key);
	DEBUG(if flag then PRINT() << "Found in cache" << NL());
	if flag then {
		dispose! key;
		key := Nil BasicTuple;
	}
	(val pretend Box, flag, key)
}


+++ AldorDomainRep defines the run-time representation of axiomxl domains.  Domains
+++ are lazy.  Initially domains only hold a function which, when called,
+++ fills in the hash code for the domain, and sets another function.
+++ When this funcation is called, the parent and export fields are set, and
+++ any code from the "add" body is run.
+++ Domains cache the last few lookups (this gives up to 10% speedup, 
+++ depending on the example.
AldorDomainRep: Conditional with {
	new:		DomainFun % -> %;
		++ new(fun) creates a new domain.

	prepare!:	% -> ();
		++ prepare!(dom) forces a domain to fully instantiate.

	addExports!:	(%, Array Hash, Array Hash, Array Value) -> ();
		++ addExports!(dom, names, types, exports)
		++ sets the exports fields of a domain.

	addDefaults!:	(%, CatObj, Domain) -> ();
		++ addDefaults!(dom, defaults, domain)
		++ sets the default package for a domain.
		++ Additional arg is the wrapped domain.

	addParents!:	(%, Array Domain, Domain) -> ();
		++ addParents!(dom, parents) sets the parent field of a domain.

	addHash!:	(%, Hash) -> ();
		++ addHash!(dom, hash) sets the hash field of a domain.

	addNameFn!:	(%, ()->DomainName) -> ();
		++ addName!(dom, name) sets the naming fn  of a domain.

	name: % -> DomainName;
	get:		(%, Domain, Hash, Hash, Box, Bit) -> Box;
		++ get(dom, name, type, box, skipDefaults)
		++ Fetch an export from the given domain, putting the result
		++ in the box.  It won't look in category default packages if
		++ if skipDefaults is true.  Returns nullBox on failure.
		++ If the type hash code is wildcard, then any type is OK.

	hash:		% -> Hash;
		++ hash(dom) returns the hash code for a domain.


	inheritTo: (dom: %, child: Domain) -> %;
	
	nameExtend: % -> DomainName;
	getExtend:	(%, Domain, Hash, Hash, Box, Bit) -> Box;
		++ get(dom, name, type, box, skipDefaults)
		++ Fetch an export from an extended domain.
	inheritToExtend: (dom: %, child: Domain) -> Domain;

	hashExtend:	% -> Hash;
		++ hashExtend(dom) returns the hash code for extended domains.
	extendFillObj!: (%, Array Domain) -> ();
		++ fills extend info

	nameDummy: % -> DomainName;
	hashDummy:	% -> Hash;
	getDummy:	(%, Domain, Hash, Hash, Box, Bit) -> Box;
	inheritToDummy: (dom: %, child: Domain) -> Domain;

	expandInherited: (%, %, Domain) -> ((%, Hash) -> ());
	expandExtend: (%, %, Domain) -> ((%, Hash) -> ());
}
== add {
	Rep1 ==> Ptr;
	Per1 ==> DomainFun %;
	rep1 x ==> x @ Per1 pretend Rep1;
	per1 x ==> x @ Rep1 pretend Per1;

	Rep2 ==> Ptr;
	Per2 ==> (%, Hash) -> ();
	rep2 x ==> x @ Per2 pretend Rep2;
	per2 x ==> x @ Rep2 pretend Per2;

	Rep ==> Record (f1:		Rep1,
			f2:		Rep2,
			hashcode:	Hash, 
			parents:	Array Domain,
			extendees:	Array Domain,
			defaults:	CatObj,
			names:		Array Hash,
			types:		Array Hash, 
			exports:	Array Value,
			ngets:		Int,
			serial: 	SingleInteger,
			cache:		PtrCache,
			nameFn: ()->DomainName,
			id: Int);

	import from Rep;

	--local serialThis: SingleInteger := 0;
	domainStdName(): DomainName == noName();

	new(fn1: DomainFun %): % == {
		--free serialThis;
		--serialThis := serialThis + 1;
		per [rep1 fn1, rep2 Nil Per2, 0, 
		     Nil Array Domain, Nil Array Domain, Nil CatObj,
		     Nil Array Hash, 
		     Nil Array Hash, Nil Array Value, 0,
		     --serialThis, 
		     0,
		     newCache(), (): DomainName +->  new "Dunno", fiCounter()]
	}
	prepare!(dom: %): () == {
#if ExtendReplace
		prepareHash!(dom);
#endif
		prepareGetter!(dom);
	}

	addExports!(dom: %, nams: Array Hash, typs: Array Hash,
		    expts: Array Value): () == {
		rep(dom).names    := nams;
		rep(dom).types    := typs;
		rep(dom).exports  := expts;
		for i in 1..#nams repeat {
		    rep(dom).names(i) := 0;
		    rep(dom).types(i) := 0;
		}
	}


	addDefaults!(dom: %, defs: CatObj, domAsDomain: Domain): () ==
		--rep(dom).defaults := categoryBuild(defs, domAsDomain);
		rep(dom).defaults := build(defs, domAsDomain);

	addParents!(dom: %, parnts: Array Domain, child: Domain): () == {
		for i in 1..#parnts repeat
			parnts.i := inheritTo(parnts.i, child);
		rep(dom).parents  := parnts;
	}

	addHash!(dom: %, code: Hash): () ==
		rep(dom).hashcode := code;

	addNameFn!(dom: %, namefn: ()->DomainName): () == {
		rep(dom).nameFn := namefn;
	}

	local prepareGetter!(dom: %): () == {
		h := hash(dom);
		if Nil?(Array Hash)(rep(dom).names) then
			per2(rep(dom).f2)(dom, h);
	}

	prepareHash!(dom: %): () == {
		if Nil?(Rep2)(rep(dom).f2) then {
			rep(dom).f2 := rep2(per1(rep(dom).f1)(dom));
			DEBUG(PRINT()<<"Initialised: "  <<name dom
			             <<" with hashcode "<<rep(dom).hashcode
				     <<NL());
		}
	}

	get(dom: %, pcent: Domain, nameCode: Hash, type: Hash, 
	    box: Box, skip: Bit): Box == {
		import from String, TextWriter, SingleInteger, StringTable;
		(newBox, flag, key) := checkGetCache(rep(dom).cache, 
						    pcent, nameCode, type, skip);
		flag => {
			not newBox => newBox;
			setVal!(box, value newBox); 
			box;
		}
		DEBUG(PRINT()<<"(Looking for "<<find nameCode
			     <<" with code "  <<type<<NL());
		rep(dom).ngets := rep(dom).ngets + 1;
		if rep(dom).ngets > 100 then {
			--printDomain(PRINT(), name dom) << NL();
			ERROR "Circular get broken";
		}
		if (nameCode = pcentPcentHash) then 
			newBox := getauxPcentPcent(dom, pcent, type, box, skip);
		else 
			newBox := getaux(dom, pcent, nameCode, type, box, skip);
		rep(dom).ngets := rep(dom).ngets - 1;
		DEBUG(PRINT()<<(if newBox then "OK " else "Fail ")
		             <<"from "<<name dom<<")");
		addGetCache(rep(dom).cache, key, newBox);
		newBox;
	}

	getaux(dom: %, pcent: Domain, nameCode: Hash, type: Hash, 
	       box: Box, skip: Bit): Box == {
		import from String, TextWriter, SingleInteger, StringTable;
		local newBox: Box;
		DEBUG(PRINT() << "(Preparing " << name dom << ")" << NL());
		prepareGetter! dom;
		DEBUG(PRINT() << "(" << name dom << " is prepared)" << NL());
		for expName  in rep(dom).names 
		for expType  in rep(dom).types
		for expValue in rep(dom).exports
	        repeat {
			zero? expName => iterate;

			DEBUG(PRINT() << "(Export: " << find expName
				      << " with type " << expType << NL());

			nameCode=expName and (type=expType or type=wildcard) => {
				setVal!(box, expValue);
				return box;
			}
		}

		DEBUG(PRINT()<<"(Not found "<<find nameCode
			     <<" with type "  <<type
			     <<" in " << name dom << ")" <<NL());

		-- call in order:
		-- parents (skipping defaults)
		-- extends (skipping defaults)
		-- defaults
		-- parents (defaults)
		-- extents (defaults)
		count := if skip then 1 else 2;
		skip  := true;
		for i in 1..count repeat {
			def := rep(dom).defaults;
			if not skip and def then {
				newBox := getDefault!(def, pcent, nameCode, type, box);
				newBox => return newBox;
			}
			pars := rep(dom).parents;
			newBox := getauxArray(pars, pcent, nameCode, type, box, skip);
			newBox => return newBox;
	
			exts := rep(dom).extendees;
			newBox := getauxArray(exts, pcent, nameCode, type, box, skip);
			newBox => return newBox;

			skip := not skip;
		}

		nullBox();
	}

	getauxArray(doms: Array Domain, pcent: Domain, name: Hash, 
		    type: Hash, box: Box, skip: Bit): Box == {
		Nil?(Array Domain)(doms) => nullBox();
		for dom in doms repeat {
			if dom then {
				newBox := getExportInner!(dom, pcent, name,
							 type, box, skip);
				newBox => return newBox;
			}
		}
		return nullBox();
	}

	getauxPcentPcent(dom: %, pcent: Domain, type: Hash, 
			 box: Box, skip: Bit): Box == {
		prepareGetter! dom;
		def := rep(dom).defaults;
		not def => return nullBox();
		newBox := getDefault!(def, pcent, pcentPcentHash, type, box);
		newBox => newBox;
		exts := rep(dom).extendees;
		Nil?(Array Domain)(exts) => nullBox();
		for ext in exts repeat {
			if ext then {
				newBox := getExportInner!(
						ext, pcent, 
						pcentPcentHash,
						type, box, skip);
				newBox => return newBox;
			}
		}
		nullBox();
	}

	hash(dom: %): Hash == {
		prepareHash! dom;
		rep(dom).hashcode;
	}

	name(dom: %): DomainName == {
		prepareHash! dom;
		(rep(dom).nameFn)();
	}

	inheritTo(dom: %, child: Domain): % == {
		fn1(self: %): ((%, Hash) -> ()) == {
			prepareHash!(dom);
			addHash!(self, getHash!(child));
			addNameFn!(self, rep(dom).nameFn);
			if not Nil?(Array Domain)(rep(dom).extendees) then {
				arr := new(#(rep(dom).extendees), Nil(Domain));
				for i in 1..#arr repeat
				       arr.i := inheritTo(rep(dom).extendees.i,
				 			  child);
				rep(self).extendees    := arr;
			}
			rep(self).cache        := newCache();
			per2(rep(dom).f2);
		}
		dr := new(fn1);
		--domainMakeDispatch(dr pretend AldorDomainRep)
		--makeDomain(aldorDispatchVector(), dr pretend DomainRep);
		--nil() pretend Domain
		dr
	}

	expandInherited(self: %, dom: %, child: Domain): ((%, Hash) -> ()) == {
		prepareHash!(dom);
		addHash!(self, getHash!(child));
		addNameFn!(self, rep(dom).nameFn);
		if not Nil?(Array Domain)(rep(dom).extendees) then {
			arr := new(#(rep(dom).extendees), Nil(Domain));
			for i in 1..#arr repeat
			       arr.i := inheritTo(rep(dom).extendees.i, child);
			rep(self).extendees    := arr;
		}
		rep(self).cache := newCache();
		per2(rep(dom).f2);
	}

	-- Operations for extension domains.

	getExtend (dom: %, pcent: Domain, name: Hash, type: Hash, 
		   box: Box, skip: Bit): Box
	== {
		local newBox: Box;
		prepareHash! dom;
		pars  := rep(dom).parents;
		newBox := getauxArray(pars, pcent, name, type, box, skip);
		newBox => return newBox;
		nullBox();
	}

	hashExtend (dom: %): Hash == {
		prepareHash! dom;
		pars := rep(dom).parents;
		if not Nil?(Array Domain)(pars) then
			getHash! pars.1;
		else
			0@Hash;
	}

	nameExtend(dom: %): DomainName == {
		prepareHash! dom;
		pars := rep(dom).parents;
		if not Nil?(Array Domain)(pars) then
			getName pars.1;
		else
			noName();
	}

	expandExtend(self: %, dom: %, child: Domain): ((%, Hash) -> ()) == {
		local arr: Array Domain;
		prepareHash!(dom);
		arr := new(#(rep(dom).parents), Nil(Domain));
		for i in 1..#arr repeat arr.i := rep(dom).parents.i;
		addParents!(self, arr, child);
		if not Nil?(Array Domain)(rep(dom).extendees) then {
			arr := new(#(rep(dom).extendees), Nil(Domain));
			for i in 1..#arr repeat
				arr.i := inheritTo(rep(dom).extendees.i, 
					           child);
			rep(self).extendees := arr;
		}
		rep(self).cache     := newCache();
		(x: %, h: Hash): () +-> {} 
	}

	inheritToExtend(dom: %, child: Domain): Domain == {
		fn1(self: %): ((%, Hash) -> ()) == {
			local arr: Array Domain;
			prepareHash!(dom);
			arr := new(#(rep(dom).parents), Nil(Domain));
			for i in 1..#arr repeat arr.i := rep(dom).parents.i;
			addParents!(self, arr, child);
			if not Nil?(Array Domain)(rep(dom).extendees) then {
				arr := new(#(rep(dom).extendees), Nil(Domain));
				for i in 1..#arr repeat
					arr.i := inheritTo(rep(dom).extendees.i, 
						           child);
				rep(self).extendees := arr;
			}
			rep(self).cache     := newCache();
			(x: %, h: Hash): () +-> {} };
		dr:=new(fn1);
		--extendMakeDispatch(dr pretend AldorDomainRep);
		--newExtend(dr);
		never;
	}

	extendFillObj!(dom: %, pars: Array Domain): () == {
		rep(dom).parents := pars;
#if ExtendReplace
		ndom := extendMakeDispatch(dom pretend AldorDomainRep);
		for i in 1..#pars repeat {
			par := pars.i;
			prepare!(par);
			npar := copy par;
			ncopy(par, ndom);
			pars.i := npar;
		}
#endif
		--!! The first domain must be the thing being extended
		-- this forces all extends to have adds on the RHS
		last := pars.1;
		extendFillExtender!((domainRep last) pretend %, pars, #pars);
	}

	extendFillExtender!(dom: %, pars: Array Domain, n: Int): () == {
		exts : Array Domain := new(n-1, Nil Domain);
		for i in 2..n repeat
			exts.(i-1) := pars.i;
		rep(dom).extendees := exts;
	}

	-- dummied operations

	nameDummy(dom: %): DomainName == noName();

	getDummy(dom: %, pcent: Domain, name: Hash, 
		 type: Hash, box: Box, skip: Bit): Box == 
		ERROR 	"get on incomplete domain";

	hashDummy(dom: %): Hash == 
		ERROR 	"hash value read on incomplete domain";

	inheritToDummy(dom: %, child: Domain): Domain == 
		ERROR "inherit to incomplete domain";

	-- Dispatch vector creation operations.

	--!! These functions are here rather than in DispatchVector so
	--!! we can initialize the functions used to construct the vector
	--!! using global constants instead of using calls to domainGetExport!.

	DV ==> DispatchVector;

	local adv: DV := Nil DV;
	local edv: DV := Nil DV;
	local ddv: DV := Nil DV;

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


